// GsMain.cpp  -*- C++ -*-
// $Id: GsMain.cpp,v 1.48 1998/05/11 18:11:35 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

extern int h_errno;


#ifdef __GNUC__
#pragma implementation "GsMain.h"
#endif
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#ifndef WIN32
#include <sys/wait.h>
#include <sched.h>
#include <netdb.h>
#endif

#include "GsMain.h"
#include "GsUtil.h"
#include "LyraDefs.h"
#include "LmNew.h"
#include "LmUtil.h"
#include "LmGlobalDB.h"
#include "LmServerDBC.h"
#include "LmMesgBufPool.h"
#include "LmLogFile.h"
#include "LmThreadPool.h"
#include "LmConnectionSet.h"
#include "GsPlayerSet.h"
#include "GsInputDispatch.h"
#include "GsOutputDispatch.h"
#include "LmSocket.h"
#include "LmSockAddrInet.h"
#include "PThAttr.h"
#include "GsSignalThread.h"
#include "GsGameThread.h"
#include "GsNetworkInput.h"
#include "GsNetworkOutput.h"
#include "GsForwardThread.h"
#include "GsPositionThread.h"
#include "GsLevelSet.h"
#include "GsPlayerThreadSet.h"
#include "PTh.h"
#include "LmGuildDBC.h"
#include "LmItemDBC.h"
#include "LmPlayerNameMap.h"
#include "LmRand.h"
#include "LmPlayerDBC.h"
#include "LmBillingDBC.h"
#include "LmDebug.h"
#include "SMsg_GS_Action.h"
#include "GsMacros.h"
#include "LmRand.h"


#ifndef USE_INLINE
#include "GsMain.i"
#endif

////
// Constructor
////

GsMain::GsMain()
  : pid_(0),
    ppid_(0),
    globaldbc_(0),
    serverdbc_(0),
    lset_(0),
    pset_(0),
    ptset_(0),
    mpool_(0),
    cset_(0),
    alarms_(0),
    portnum_(0),
    tsock_(0),
    usock_(0),
    inp_(0),
    outp_(0),
    log_(0),
    slog_(0),
    tpool_(0),
    gdbc_(0),
    idbc_(0),
    pdbc_(0),
    bdbc_(0),
    pnames_(0),
    last_alrm_(0),
    max_players_(0),
	num_logins_(0),
	num_redirects_(0),
	closed_(false),
	run_forever_(false),
    sigterm_(false),
    sigerr_(false)
{
  // set alarms_ to a random value of up to one our of fake uptime, so that
  // all servers aren't synchronized
  alarms_ = LmRand::Generate(1, 3600);

}

////
// Destructor
////

GsMain::~GsMain()
{
#ifndef USE_LMNEW
  LmNew::SetLog(0); // if not using the logging new/delete, reset the log
#endif /* !USE_LMNEW */
  close_logs();
  // destroy everything (order is somewhat important here)
  LmDELETE(ptset_);
  LmDELETE(tpool_);
  LmDELETE(cset_);
  LmDELETE(inp_);
  LmDELETE(outp_);
  LmDELETE(pset_);
  LmDELETE(mpool_);
  LmDELETE(lset_);
  LmDELETE(globaldbc_);
  LmDELETE(serverdbc_);
  LmDELETE(pnames_);
  LmDELETE(tsock_);
  LmDELETE(usock_);
  LmDELETE(gdbc_);
  LmDELETE(idbc_);
  LmDELETE(pdbc_);
  LmDELETE(bdbc_);
  LmDELETE(log_);
  LmDELETE(slog_);
}

////
// Log, SecurityLog: return pointers to log objects (cannot be inline, since the
//   objects are not LmLog*, but superclasses)
////

LmLog* GsMain::Log() const
{
  return log_;
}

LmLog* GsMain::SecurityLog() const
{
  return slog_;
}

////
// Init: load global database, create objects; return 0 if ok, -1 if not
////

int GsMain::Init(const TCHAR* root_dir, int max_players, const char* next_ip_address, int next_server_port)
{
  // check that port has been assigned already
  if (portnum_ == 0) {
    return -1;
  }

  max_players_ = MIN(max_players, MAX_PLAYERS); // avoid overflow

  // create databases
  globaldbc_ = LmNEW(LmGlobalDB(root_dir));
  serverdbc_ = LmNEW(LmServerDBC());

  // load passwords & server info

  TCHAR pw_file[FILENAME_MAX];
  globaldbc_->GetPasswordFile(pw_file);
  serverdbc_->LoadPasswords(pw_file);
  if (serverdbc_->Connect() < 0)
	  return -1;
  if (serverdbc_->Load() < 0)
	  return -1;
  serverdbc_->Disconnect();

  // create and open log files
  log_ = LmNEW(LmLogFile());
  slog_ = LmNEW(LmLogFile());
  open_logs();

#ifndef USE_LMNEW
  // if not using the logging new/delete, install the handler using the main log
  LmNew::SetLog(log_);
  LmNew::InstallNewHandler();
#endif /* !USE_LMNEW */

  // create everything else
  lset_ = LmNEW(GsLevelSet(this));
  pset_ = LmNEW(GsPlayerSet(this));
  ptset_ = LmNEW(GsPlayerThreadSet(this));
  mpool_ = LmNEW(LmMesgBufPool(SPARE_MBUFS));
  cset_ = LmNEW(LmConnectionSet(MAX_CONNECTIONS));
  inp_ = LmNEW(GsInputDispatch(this));
  outp_ = LmNEW(GsOutputDispatch(this));
  tpool_ = LmNEW(LmThreadPool());


  gdbc_ = LmNEW(LmGuildDBC(serverdbc_->GuildDBUsername(), serverdbc_->GuildDBPassword(), serverdbc_->DatabaseHost(), serverdbc_->DatabasePort()));
  idbc_ = LmNEW(LmItemDBC(serverdbc_->ItemDBUsername(), serverdbc_->ItemDBPassword(), serverdbc_->DatabaseHost(), serverdbc_->DatabasePort()));
  pdbc_ = LmNEW(LmPlayerDBC(serverdbc_->PlayerDBUsername(), serverdbc_->PlayerDBPassword(), serverdbc_->DatabaseHost(), serverdbc_->DatabasePort()));
  bdbc_ = LmNEW(LmBillingDBC(serverdbc_->BillingDBUsername(), serverdbc_->BillingDBPassword(), serverdbc_->DatabaseHost(), serverdbc_->DatabasePort()));

  pnames_ = LmNEW(LmPlayerNameMap(pdbc_));

  gdbc_->SetLog(log_);
  idbc_->SetLog(log_);
  pdbc_->SetLog(log_);
  bdbc_->SetLog(log_);

  // seed random number generator
  LmRand::InitSeed(getpid());

  _tcscpy(next_ip_, next_ip_address);
  next_port_ = next_server_port;
  // if the "next" IP/port is the same as ours, 
  // ignore login counts and run until shut down
  if ((next_port_ == portnum_) &&
	  (0 == _tcscmp(next_ip_address, host_ip_)))
	  run_forever_ = true;

  log_->Debug(_T("Initialized game server"));
  log_->Debug(_T("Next gamed ip/port: %s, %d"), next_ip_address, next_port_);


  return 0;
}

////
// CreateSockets: create and bind server sockets; return 0 if ok, < 0 if error
//   (rest of main object may not be initialized!)
////

int GsMain::CreateSockets(int min_port, int max_port, const char* ip_address)
{
  DEFMETHOD(GsMain, CreateSockets);
  // initialize server address
  LmSockAddrInet addr;
  
  if (addr.Init(ip_address, ServerPort()) == -1) { // bind to host's IP address
    log_->Error(_T("%s: could not bind to address %s"), method, ip_address);
  }

  // create TCP listening socket
  tsock_ = LmNEW(LmSocket());
  tsock_->Socket(LmSockType::Inet_Stream());
  // if min/max ports are the same, then set the "reuse" flag, just because
  if (min_port == max_port) {
    tsock_->SetSockOpt(SO_REUSEADDR, 1);
  }
  // find unused port in given range
  if (tsock_->BindRange(addr, min_port, max_port) < 0) {
    log_->Error(_T("%s: could not bind TCP socket: %s"), method, strerror(errno));
    return -1;
  }
  if (tsock_->Listen() < 0) {
    // log_->Error(_T("%s: could not listen at TCP socket: %s"), method, strerror(errno));
    return -2;
  }
  // create UDP listening socket
  usock_ = LmNEW(LmSocket());
  usock_->Socket(LmSockType::Inet_Datagram());
  // if (min_port == max_port) {
  usock_->SetSockOpt(SO_REUSEADDR, 1);  // in case server was started soon after it was killed
  // }
  if (usock_->Bind(addr) < 0) {
    // log_->Error(_T("%s: could not bind UDP socket: %s"), method, strerror(errno));
    return -3;
  }
  // get the socket addresses
  tsock_->ComputeAddresses(true);
  usock_->ComputeAddresses(true);
  // sockets created and bound successfully; assign server port
  portnum_ = tsock_->SockName().Port();
 _tcscpy(host_ip_, ip_address);
  host_ip_addr_ = (unsigned long)(tsock_->SockName().IPAddress());
  // DEBUGLOG(("%s: port=%d, server address: %s:%d", method, portnum_, addr.AddressString(), addr.Port()));
  return 0;
}

////
// Go: run the server; when this ireturns, the server program exits
////

int GsMain::Go()
{
  DEFMETHOD(GsMain, Go);
#ifndef WIN32
  pid_ = getpid();
  ppid_ = getppid();
#endif
  start_time_ = time(NULL);
  log_->Log("%s: main process id = %ld, parent = %ld", method, ServerPid(), ParentPid());

  // connect to databases

  if (connect_to_databases() < 0) {
    log_->Error(_T("%s: could not connect to databases"), method);
    return Lyra::EXIT_EXEC;
  }

  // recover from a crash
  //save_player_files();

  // create pidfile
#ifndef WIN32
  if (create_pidfile() < 0) {
    log_->Error(_T("%s: could not create server pidfile"), method);
    return Lyra::EXIT_EXEC;
  }
#endif

  // now, if we're starting up and listed in the gamed closing file,
  // we need to remove ourselves from that file. because two gamed's
  // can be closing at a time, we need to check both positions, and handle
  // both positions accordingly

	// BMP 10/03: deprected for ul3d; round robin only
	//if (this->ReadClosingFile()) {
	//	for (int i=0; i<Lyra::MAX_CLOSING_GAMEDS; i++) {
	//		if ((closing_ip[i] == host_ip_addr_) &&
	//			(portnum_ == closing_port[i])) { // we're in the closing file
	//			closing_ip[i] = 0;
	//			closing_port[i] = 0; // write in a null server (none/0) where we were
	//			serverdbc_->WriteNewClosingFile(closing_ip, closing_port);
	//			break;
	//		}
	//	} 
	//}

  // start threads
  if (start_threads() < 0) {
    log_->Error(_T("%s: could not start threads"), method);
    return Lyra::EXIT_EXEC;
  }

  log_->Debug(_T("Game server started threads"));

  // get game thread
  LmThread* gthr = tpool_->GetThread(THREAD_GAMESERVER);
  if (!gthr) {
    log_->Error(_T("%s: could not get level thread"), method);
    return Lyra::EXIT_EXEC;
  }

  // get signal thread
  GsSignalThread* sthr = (GsSignalThread*) tpool_->GetThread(THREAD_SIGNAL);
  if (!sthr) {
    log_->Error(_T("%s: could not get signal thread"), method);
    return Lyra::EXIT_EXEC;
  }

  // wait for game thread to exit
  while (gthr->IsRunning()) {
    // we now check the alarm timer manually
    if ((time(NULL) - last_alrm_) > (2*ALARM_DELAY)) { 
      //log_->Debug(_T("%s: time to manually raising SIGALRM"), method);
      last_alrm_ = time(NULL);
      process_SIGALRM();
    }

    if (sigterm_)
      process_SIGTERM();
    if (sigerr_) {
      log_->Error(sigerrtxt_);
      sigerr_ = false;
    }
#ifdef WIN32
	Sleep(1000);
#else
	pth_sleep(1); 
#endif
    

  }
  // TODO: possibly periodically check if all main server threads
  //   are running, and if one isn't, restart it?  or exit?
  
  // cleanup
  remove_pidfile();

  log_->Debug(_T("%s: game server pid %d shutting down"), method, getpid());
// GAMED_POINTER
#if 0
  if (this->IsActiveServer()) {
		serverdbc_->WriteNewLoginFile(next_ip_, next_port_);  // success
	    log_->Debug(_T("Gamed exiting; redirected to %s, %d"), next_ip_, next_port_);
  }
#endif

  //if (closed_)
	  //return Lyra::EXIT_CLOSED;
  //else
  return Lyra::EXIT_OK;
}

////
// IsActiveServer (are we the one pointed to in the gamed pointer login file?)
////

// this is for GAMED_POINTER only
#if 0
bool GsMain::IsActiveServer() const
{
	FILE* loginf =_tfopen(serverdbc_->LoginFilePath(), _T("r"));

	if (!loginf)
		return false;

	char current_ip[64];
	int current_port;

	_ftscanf(loginf, "%s\n%u\n", current_ip, &current_port);

	fclose(loginf);
	 

	if ((current_port == portnum_) && 
		(0 == _tcscmp(current_ip, host_ip_)))
		return true;
	else
		return false;
}
#endif // IsActiveServer

#if 0
// BMP 10/03: deprecated for ul3d; round robin login only

//// 
// TryToClose
////
bool GsMain::TryToClose() 
{   // ROUND_ROBIN only
	if (run_forever_)
		return false; 
	if (closed_)
		return true;

	// if the next ip and port are the same as the current, we never close
	if ((0 == _tcscmp(host_ip_, next_ip_)) &&
		(portnum_ == next_port_))
		return false;

	if (!this->ReadClosingFile())
		return false;

	for (int i=0; i<Lyra::MAX_CLOSING_GAMEDS; i++) {
		if ((0 == closing_ip[i]) ||
			(0 == closing_port[i])) { // we found an empty slot in the closing file
			closed_ = true;
			closing_ip[i] = host_ip_addr_;
			closing_port[i] = portnum_;
			serverdbc_->WriteNewClosingFile(closing_ip, closing_port);
			break;
		}
	} 
		
	if (closed_) {
		log_->Debug(_T("Gamed closed; will restart when player count goes to zero\n"));
		return true;
	} else {
		if (3 == Lyra::MAX_CLOSING_GAMEDS) {
			log_->Debug(_T("Gamed not closed; servers %d,%d,%d closing\n"), 
				closing_port[0], closing_port[1], closing_port[2]);
		} else {
			log_->Debug(_T("Gamed not closed; other servers closing\n"));
		}
		return false;
	}
}
#endif


#if 0
// BMP 10/03 - deprecated; round robin only

////
//  ReadClosingFile- retrieve the values listed in the closing server file
////


bool GsMain::ReadClosingFile()
{
	FILE* loginf =_tfopen(serverdbc_->ClosingFilePath(), _T("r"));
	if (!loginf)
		return false;
	int i = 0;
	for (i=0; i<Lyra::MAX_CLOSING_GAMEDS; i++) {
		closing_port[i] = 0;
		closing_ip[i] = 0;
	}

	for (i=0; i<Lyra::MAX_CLOSING_GAMEDS; i++) {
		if (_ftscanf(loginf, _T("%u %u\n"), &(closing_ip[i]), &(closing_port[i])) == EOF)
			break;
	}
	
	fclose(loginf);

	return true;
}

#endif

////
// Dump
////

void GsMain::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GsMain[%p,%d]: build=%d maxplayers=%d port=%d pid=%lu ppid=%lu uptime=%d>\n\n"), this, sizeof(GsMain),
	  GAME_VERSION, MaxPlayers(), ServerPort(), ServerPid(), ParentPid(), Uptime());
  // dump each object
  pset_->Dump(f, indent);     _ftprintf(f, _T("\n")); fflush(f);
  cset_->Dump(f, indent);     _ftprintf(f, _T("\n")); fflush(f);
  mpool_->Dump(f, indent);    _ftprintf(f, _T("\n")); fflush(f);
  tpool_->Dump(f, indent);    _ftprintf(f, _T("\n")); fflush(f);
  ptset_->Dump(f, indent);    _ftprintf(f, _T("\n")); fflush(f);
  tsock_->Dump(f, indent);    _ftprintf(f, _T("\n")); fflush(f);
  usock_->Dump(f, indent);    _ftprintf(f, _T("\n")); fflush(f);
  inp_->Dump(f, indent);      _ftprintf(f, _T("\n")); fflush(f);
  outp_->Dump(f, indent);     _ftprintf(f, _T("\n")); fflush(f);
  lset_->Dump(f, indent);     _ftprintf(f, _T("\n")); fflush(f);
  globaldbc_->Dump(f, indent); _ftprintf(f, _T("\n")); fflush(f);
  serverdbc_->Dump(f, indent); _ftprintf(f, _T("\n")); fflush(f);
  pnames_->Dump(f, indent);   _ftprintf(f, _T("\n")); fflush(f);
  gdbc_->Dump(f, indent);     _ftprintf(f, _T("\n")); fflush(f);
  idbc_->Dump(f, indent);     _ftprintf(f, _T("\n")); fflush(f);
  pdbc_->Dump(f, indent);     _ftprintf(f, _T("\n")); fflush(f);
  bdbc_->Dump(f, indent);     _ftprintf(f, _T("\n")); fflush(f);
}

////
// RotateLogs
////

void GsMain::RotateLogs()
{
  close_logs();
  open_logs();
}

////
// open_logs
////

void GsMain::open_logs()
{
  // main log
  if (log_) {
    // LmLogFile implementation
    log_->Init(_T("gs"), _T("main"), ServerPort());
    log_->Open(GlobalDB()->LogDir());
    // LmLogMPSysLog implementation
    //    const char* universe = _tgetenv("UL_UNIVERSE");
    //if (!universe) {
    // universe = "prod"; // default universe
    //}
    //char univ[80];
    //_stprintf(univ, "lyra-%s", universe);
    //log_->Open("gamed", univ, "ul_main");
  }
  // security log
  if (slog_) {
    // LmLogFile implementation
     slog_->Init(_T("gs"), _T("security"), ServerPort());
     slog_->Open(GlobalDB()->LogDir());
     // LmLogMPSyslog: uses log_
  }
}

////
// close_logs
////

void GsMain::close_logs()
{
  if (log_) {
    log_->Close();
  }
  if (slog_) {
    // slog_->Close();
  }
}

////
// create_pidfile: create server pid file
////

int GsMain::create_pidfile()
{
  TCHAR pf[80];
 _stprintf(pf, _T("gamed_%d.pid"), ServerPort());
  TCHAR pidfname[FILENAME_MAX];
  GlobalDB()->GetPidFile(pidfname, pf);
  FILE* pidf =_tfopen(pidfname, _T("w"));
  if (!pidf) {
    return -1;
  }
 _ftprintf(pidf, _T("%lu\n"), ServerPid());
  fclose(pidf);
  return 0;
}

////
// remove_pidfile: remove server pid file
////

void GsMain::remove_pidfile()
{
  TCHAR pf[80];
 _stprintf(pf, _T("gamed_%d.pid"), ServerPort());
  TCHAR pidfname[FILENAME_MAX];
  GlobalDB()->GetPidFile(pidfname, pf);
  _tunlink(pidfname);
}

////
// start_threads: start server threads, return 0 if ok, -1 otherwise
////

int GsMain::start_threads()
{
  DEFMETHOD(GsMain, start_threads);
  PThAttr attr;
  LmThread* thread;

  attr.Init();
  // none of the server threads have any return state, and thus are detached
  attr.SetJoinable(FALSE);
  //  attr.SetDetachState(PTH_CREATE_DETACHED);
  // set the stack size to a more reasonable value: 128K (default is 1MB)
  attr.SetStackSize(2091752);
  // default scope: process (ie. not bound)
  //attr.SetScope(PTH_SCOPE_PROCESS);

  // create signal-handling thread
  thread = LmNEW(GsSignalThread(this));
  if (!thread || (thread->Create(&attr)) < 0) {
    log_->Error(_T("%s: could not create signal thread: %s"), method, strerror(errno));
    return -1;
  }
  tpool_->AddThread(GsMain::THREAD_SIGNAL, thread);

  // create game server thread
  thread = LmNEW(GsGameThread(this));
  if (!thread || (thread->Create(&attr)) < 0) {
    log_->Error(_T("%s: could not game server thread: %s"), method, strerror(errno));
    return -1;
  }
  tpool_->AddThread(GsMain::THREAD_GAMESERVER, thread);

  // create message forwarding thread
  thread = LmNEW(GsForwardThread(this));
  if (!thread || (thread->Create(&attr)) < 0) {
    log_->Error(_T("%s: could not create network output thread: %s"), method, strerror(errno));
    return -1;
  }
  tpool_->AddThread(GsMain::THREAD_FORWARD, thread);


  // networking threads have system scope (ie. bound to LWP)
  //  attr.SetScope(PTH_SCOPE_SYSTEM);

  // create network output thread
  thread = LmNEW(GsNetworkOutput(this));
  if (!thread || (thread->Create(&attr)) < 0) {
    log_->Error(_T("%s: could not create network output thread: %s"), method, strerror(errno));
    return -1;
  }
  tpool_->AddThread(GsMain::THREAD_NETOUTPUT, thread);

  // create position update thread
  thread = LmNEW(GsPositionThread(this));
  if (!thread || (thread->Create(&attr)) < 0) {
    log_->Error(_T("%s: could not create position update thread: %s"), method, strerror(errno));
    return -1;
  }
  tpool_->AddThread(GsMain::THREAD_POSITION, thread);


 // create network input thread
  thread = LmNEW(GsNetworkInput(this));
  if (!thread || (thread->Create(&attr)) < 0) {
    log_->Error(_T("%s: could not level server thread: %s"), method, strerror(errno));
    return -1;
  }
  tpool_->AddThread(GsMain::THREAD_NETINPUT, thread);

  // start up idle player threads 
  ptset_->StartIdlePlayerThreads(20);
 //ptset_->StartIdlePlayerThreads(20);

  // all threads created
  return 0;
}

////
// connect_to_databases
////

int GsMain::connect_to_databases()
{
  DEFMETHOD(GsMain, connect_to_databases);
  int rc, sc, lt;
  // guild database
  rc = gdbc_->Connect();
  sc = gdbc_->LastSQLCode();
  lt = gdbc_->LastCallTime();
  //  log_->Debug(_T("%s: LmGuildDBC::Connect took %d ms"), method, lt);
  if (rc < 0) {
    log_->Error(_T("%s: could not connect to guild database; rc=%d, sqlcode=%d"), method, rc, sc);
    return -1;
  }
  // item database
  rc = idbc_->Connect();
  sc = idbc_->LastSQLCode();
  lt = idbc_->LastCallTime();
  //  log_->Debug(_T("%s: LmItemDBC::Connect took %d ms"), method, lt);
  if (rc < 0) {
    log_->Error(_T("%s: could not connect to item database; rc=%d, sqlcode=%d"), method, rc, sc);
    return -1;
  }
  // player database
  rc = pdbc_->Connect();
  sc = pdbc_->LastSQLCode();
  lt = pdbc_->LastCallTime();
  log_->Debug(_T("%s: LmPlayerDBC::Connect took %d ms"), method, lt);
  if (rc < 0) {
    log_->Error(_T("%s: could not connect to player database; rc=%d, sqlcode=%d"), method, rc, sc);
    return -1;
  }
  // billing database
  rc = bdbc_->Connect();
  sc = bdbc_->LastSQLCode();
  lt = bdbc_->LastCallTime();
  log_->Debug(_T("%s: LmBillingDBC::Connect took %d ms"), method, lt);
  if (rc < 0) {
    log_->Error(_T("%s: could not connect to billing database; rc=%d, sqlcode=%d"), method, rc, sc);
    return -1;
  }
  // connected to all successfully
  return 0;
}

////
// save_player_files - read any player db files left over from previous game servers and
//   write them to the player database
////

void GsMain::save_player_files() const
{
  // TODO: implement
}



////
// Close: means we have accepted the max # of logins; 
//    update gamed pointer file and rejected further login attempts
////

void GsMain::Close()
{
	if (run_forever_)
		return; 

	// if the next ip and port are the same as the current, we ignore
	// the number  of logins and run indefinitely
	if ((0 == _tcscmp(host_ip_, next_ip_)) &&
		(portnum_ == next_port_))
		return;

	num_redirects_++;
	
// GAMED_POINTER
#if 0
	// if we're closed, only rewrite the file for every 10 redirects, in case
	// we have really old logins preventing us from closing
	if (closed_ && (num_redirects_%10))
		return; 
	if (0 == serverdbc_->WriteNewLoginFile(next_ip_, next_port_)) { // success
		closed_ = true;
	    log_->Debug(_T("Gamed closed; redirected to %s, %d"), next_ip_, next_port_);
		return;
	} else { // error writing file; keep current gamed
		num_logins_ = 0; // reset
	}
	return;
#endif
}

////
// SetSIGERR - write signal error condition to a buffer so that a non-signal
//    thread can write it to the error log
////

void GsMain::SetSIGERR(const TCHAR* text)
{
 _tcscpy(sigerrtxt_, text);
  sigerr_ = true;
}


////
// process_SIGALRM
////

void GsMain::process_SIGALRM()
{
  DEFMETHOD(GsMain, process_SIGALRM);
  DECLARE_TheLineNum;

  ++alarms_;

  // reset the alarm timer
  //  alarm(ALARM_DELAY);

  // we need to set the last alarm time in the main thread in case
  // we lose a signal

  SetLastAlarm(time(NULL));

  //TLOG_Debug(_T("%s: sigalrm received"), method);

  SMsg_GS_Action msg;

  // print heartbeat?
  if ((alarms_ % ALARM_HEARTBEAT) == 0) {
    //TLOG_Log(_T("%s: called %lu times"), method, alarms_);
    // send heartbeat message to other threads
    msg.Init(SMsg_GS_Action::ACTION_HEARTBEAT);
    GsUtil::BroadcastInternalMessage(this, msg, GsMain::THREAD_SIGNAL);
  }

  // free message buffers?
  if ((alarms_ % ALARM_FREEBUFS) == 0) {
    msg.Init(SMsg_GS_Action::ACTION_FREEBUFS);
    if (GsUtil::SendInternalMessage(this, msg, GsMain::THREAD_GAMESERVER) < 0) {
      TLOG_Error(_T("%s: could not send free_bufs message to game thread"), method);
    }
  }

  // check idle players? (only if there are players in the game)
  if (((alarms_ % ALARM_CHECKIDLE) == 0) && (PlayerSet()->NumPlayers() > 0)) {
    // TLOG_Debug(_T("%s: sending check_idle messages"), method);
    msg.Init(SMsg_GS_Action::ACTION_CHECK_IDLE_PLAYERS);
    if (GsUtil::SendInternalMessage(this, msg, GsMain::THREAD_NETINPUT) < 0) {
      TLOG_Error(_T("%s: could not send check_players message to netin thread"), method);
    }
  }

  // check idle clients? (only if something's connected to the server)
  if (((alarms_ % ALARM_CHECKIDLE) == 0) && (ConnectionSet()->NumConnections() > 0)) {
    // TLOG_Debug(_T("%s: sending check_idle messages"), method);
    msg.Init(SMsg_GS_Action::ACTION_CHECK_IDLE_CLIENTS);
    if (GsUtil::SendInternalMessage(this, msg, GsMain::THREAD_NETINPUT) < 0) {
      TLOG_Error(_T("%s: could not send check_clients message to netin thread"), method);
    }
  }

  // ping connected servers?
  if (((alarms_ % ALARM_PING) == 0) && (ConnectionSet()->NumConnections() > 0)) {
    msg.Init(SMsg_GS_Action::ACTION_PING);
    if (GsUtil::SendInternalMessage(this, msg, GsMain::THREAD_NETOUTPUT) < 0) {

      TLOG_Error(_T("%s: could not send ping_servers message to netout thread"), method);
    }
  }

  // save player info? (only if there are players in the game) - deprecated BMP 6/01
  //if (((alarms_ % ALARM_SAVEPLAYER) == 0) && (PlayerSet()->NumPlayers() > 0)) {
//    msg.Init(SMsg_GS_Action::ACTION_SAVE_PLAYER);
    //GsUtil::BroadcastInternalMessagePlayers(this, msg);
  //}

  // bail? (only if there are no players connected, and we've been up for a while)
  //  if (((alarms_ % ALARM_BAIL) == 0) && (Uptime() > MIN_UPTIME) && (PlayerSet()->NumPlayers() == 0)) {
    //    TLOG_Debug(_T("%s: shutting down due to uptime"), method);
  //    msg.Init(SMsg_GS_Action::ACTION_EXIT);
  //    if (GsUtil::SendInternalMessage(this, msg, GsMain::THREAD_GAMESERVER) < 0) {
  //      TLOG_Error(_T("%s: could not send exit message to game server"), method);
  //    }
}


////
// process_SIGTERM: shut down, and exit
////

void GsMain::process_SIGTERM()
{
  DEFMETHOD(GsMain, process_SIGTERM);
  DECLARE_TheLineNum;

  SetSIGTERM(false);

  TLOG_Debug(_T("%s: received termination signal; sending exit_message to game server"), method);
  // send exit message to level thread
  SMsg_GS_Action msg;
  msg.Init(SMsg_GS_Action::ACTION_EXIT);
  if (GsUtil::SendInternalMessage(this, msg, GsMain::THREAD_GAMESERVER) < 0) {
    TLOG_Error(_T("%s: could not send exit message to game server"), method);
  }
}


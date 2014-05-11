// LsMain.cpp  -*- C++ -*-
// $Id: LsMain.cpp,v 1.50 1998-05-11 11:11:43-07 jason Exp jason $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LsMain.h"
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sched.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif

#include "LsMain.h"
#include "LmNew.h"
#include "LmUtil.h"
#include "LsUtil.h"
#include "LmGlobalDB.h"
#include "LmServerDBC.h"
#include "LmLevelDBC.h"
#include "LmMesgBufPool.h"
#include "LmLogFile.h"
#include "LmThreadPool.h"
#include "LmConnectionSet.h"
#include "LsLevelState.h"
#include "LsPlayerSet.h"
#include "LsInputDispatch.h"
#include "LsOutputDispatch.h"
#include "LmSocket.h"
#include "LmSockAddrInet.h"
#include "PThAttr.h"
#include "LsSignalThread.h"
#include "LsPositionThread.h"
#include "LsLevelThread.h"
#include "LsRoomThread.h"
#include "LsNetworkInput.h"
#include "LsNetworkOutput.h"
#include "PTh.h"
#include "LmItemDBC.h"
#include "LmSrvMesgBuf.h"
#include "RMsg_PlayerUpdate.h"
#include "LmRand.h"
#include "LsCodexText.h"
#include "LmMesg.h"
#include "LmThreadPool.h"
#include "LmMesgBufPool.h"
#include "SMsg_LS_Action.h"
#include "LmTimer.h"
#include "LsPlayer.h"
#include "LsPlayerSet.h"
#include "RMsg_PlayerUpdate.h"
#include "LmConnection.h"
#include "LmConnectionSet.h"
#include "LsMacros.h"
#include "LsPlayerList.h"
#include "LmRand.h"

#ifndef WIN32
#include <unistd.h>
#endif


#ifndef USE_INLINE
#include "LsMain.i"
#endif

////
// Constructor
////

LsMain::LsMain()
  : pid_(0),
    ppid_(0),
    globaldbc_(0),
    serverdbc_(0),
    leveldbc_(0),
    levelstate_(0),
    pset_(0),
    alarms_(0),
    mpool_(0),
    cset_(0),
    usock_(0),
    tsock_(0),
    inp_(0),
    outp_(0),
    log_(0),
    updates_(0),
    slog_(0),
    tpool_(0),
    idbc_(0),
    ctext_(0),
    last_alrm_(0),
    //	last_updates_(0),
    start_time_(0),
    sigterm_(false),
    sigint_(false),
    sigerr_(false)
{
  // set alarms_ to a random value of up to one our of fake uptime, so that
  // all level servers aren't synchronized
  alarms_ = LmRand::Generate(1, 3600);
  // message buffer needs to be large enough for a RMsg_PlayerUpdate message
  RMsg_PlayerUpdate m1;
  msgbuf_ = LmNEW(LmSrvMesgBuf(m1.MaxMessageSize()));
  gettimeofday(&first_update_time_, NULL);
  gettimeofday(&last_update_time_, NULL);
}

////
// Destructor
////

LsMain::~LsMain()
{
#ifndef USE_LMNEW
  LmNew::SetLog(0); // if not using the logging new/delete, reset the log
#endif /* !USE_LMNEW */
  close_logs();
  // destroy everything (order is somewhat important here)
  LmDELETE(tpool_);
  LmDELETE(cset_);
  LmDELETE(inp_);
  LmDELETE(outp_);
  LmDELETE(pset_);
  LmDELETE(mpool_);
  LmDELETE(levelstate_);
  LmDELETE(leveldbc_);
  LmDELETE(globaldbc_);
  LmDELETE(serverdbc_);
  LmDELETE(idbc_);
  LmDELETE(usock_);
  LmDELETE(tsock_);
  LmDELETE(log_);
  LmDELETE(ctext_);
  LmDELETE(slog_);
  LmDELETE(msgbuf_);
}

////
// Log, SpeechLog: return pointers to log objects
////

LmLog* LsMain::Log() const
{
  return log_;
}

LmLog* LsMain::SpeechLog() const
{
  return slog_;
}

////
// Init: load global and level databases, create objects; return 0 if ok, < 0 if not
////

int LsMain::Init(const TCHAR* root_dir, int port_num, int levelnum)
{
  port_num_ = port_num;

  // create databases
  globaldbc_ = LmNEW(LmGlobalDB(root_dir));
  serverdbc_ = LmNEW(LmServerDBC());

  // load passwords & server info
  TCHAR pw_file[FILENAME_MAX];
  globaldbc_->GetPasswordFile(pw_file);
  serverdbc_->LoadPasswords(pw_file);
  serverdbc_->Connect();
  serverdbc_->Load();
  serverdbc_->Disconnect();

  level_num_ = levelnum;

  leveldbc_ = new LmLevelDBC(serverdbc_->LevelDBUsername(), serverdbc_->LevelDBPassword(), serverdbc_->DatabaseHost(), serverdbc_->DatabasePort());

  // load level
  
  leveldbc_->SetLog(log_);
  
  leveldbc_->Connect();
  // load level info, with item generators
  leveldbc_->Load(levelnum, true);
  leveldbc_->Disconnect();

  // create and open log files
  log_ = LmNEW(LmLogFile());
  slog_ = LmNEW(LmLogFile());

  int level_id = leveldbc_->LevelID();
  open_logs();

#ifndef USE_LMNEW
  // if not using the logging new/delete, install the handler using the main log
  LmNew::SetLog(log_);
  LmNew::InstallNewHandler();
#endif /* !USE_LMNEW */

  idbc_ = LmNEW(LmItemDBC(serverdbc_->ItemDBUsername(), serverdbc_->ItemDBPassword(), serverdbc_->DatabaseHost(), serverdbc_->DatabasePort()));
  idbc_->SetLog(log_);

  // create level state
  levelstate_ = LmNEW(LsLevelState(this));
  levelstate_->Init();

  // create everything else
  ctext_ = LmNEW(LsCodexText(this));
  pset_ = LmNEW(LsPlayerSet(this));
  mpool_ = LmNEW(LmMesgBufPool(SPARE_MBUFS));
  cset_ = LmNEW(LmConnectionSet(MAX_CONNECTIONS));
  inp_ = LmNEW(LsInputDispatch(this));
  outp_ = LmNEW(LsOutputDispatch(this));
  tpool_ = LmNEW(LmThreadPool());

  LmRand::InitSeed(getpid());

  log_->Log(_T("Level server initialized"));
  return 0;
}

////
// Go: run the server; when this returns, the server program exits
////

int LsMain::Go(const char* ip_address)
{
  DEFMETHOD(LsMain, Go);
#ifndef WIN32
  pid_ = getpid();
  ppid_ = getppid();
#endif
  start_time_ = time(NULL);
  log_->Log(_T("%s: main process id = %ld, parent = %ld"), method, ServerPid(), ParentPid());

#ifndef WIN32
  if (create_pidfile() < 0) {
    log_->Error(_T("%s: could not create server pidfile"), method);
    return Lyra::EXIT_EXEC;
  }
#endif

  // create server sockets
  if (create_sockets(ip_address) < 0) {
    log_->Error(_T("%s: could not create server sockets"), method);
    return Lyra::EXIT_EXEC;
  }
  // connect to databases
  if (connect_to_databases() < 0) {
    log_->Error(_T("%s: could not connect to databases"), method);
    return Lyra::EXIT_EXEC;
  }
  // load level state, either from disk file if it exists or database otherwise
  if (load_state() < 0) {
    log_->Error(_T("%s: could not load level state"), method);
    return Lyra::EXIT_EXEC;
  }
  log_->Debug(_T("%s: loaded level state"), method);

  idbc_->SetNumDreamers(level_num_, 0);

  // start threads
  if (start_threads() < 0) {
    log_->Error(_T("%s: could not start threads"), method);
    return Lyra::EXIT_EXEC;
  }

  log_->Debug(_T("%s: started threads"), method);

  LsSignalThread* sthr = (LsSignalThread*) tpool_->GetThread(THREAD_SIGNAL);
  if (!sthr) {
    log_->Error(_T("%s: could not wait on signal thread"), method);
    return Lyra::EXIT_EXEC;
  }
  //  sthr->SetUpdateTimerEnabled(0);

  // wait for level thread to exit
  LmThread* lthr = tpool_->GetThread(THREAD_LEVELSERVER);
  if (!lthr) {
    log_->Error(_T("%s: could not get level thread pointer"), method);
    return Lyra::EXIT_EXEC;
  }

  bool send_neighbors = false;
  int updates = 0;

  unsigned int sec_elapsed;
  unsigned int sec_last;
  unsigned int msec_elapsed;
  unsigned int msec_last_interval;
  unsigned int sec_now;
  unsigned int msec_last_time;
  unsigned int msec_now;
  unsigned int next_send;
  timeval now;

  while (lthr->IsRunning()) {

    // we now check the alarm timer manually
    if ((time(NULL) - last_alrm_) > (2*ALARM_DELAY)) { 
      //log_->Debug(_T("%s: manually raising SIGALRM"), method);
      last_alrm_ = time(NULL);
      process_SIGALRM();
    }

    if (sigterm_)
      process_SIGTERM();
    if (sigerr_) {
      log_->Error(sigerrtxt_);
      sigerr_ = false;
    }

    // now we check to see if its time to send updates
    gettimeofday(&now,NULL);

 sec_elapsed = now.tv_sec - first_update_time_.tv_sec;
 sec_last = now.tv_sec - last_update_time_.tv_sec;
 msec_elapsed = sec_elapsed*1000 + ((now.tv_usec - first_update_time_.tv_usec)/1000);
 msec_last_interval = sec_last*1000 + ((now.tv_usec - last_update_time_.tv_usec)/1000);
 sec_now = now.tv_sec;
 msec_last_time = last_update_time_.tv_sec*1000 + (last_update_time_.tv_usec/1000);
 msec_now = sec_now*1000 + (now.tv_usec/1000);
 next_send = msec_last_time + POS_UPDATE_INTERVAL;

 // TLOG_Debug(_T("Total elapsed = %d, since last=%d, last send=%d, msec now=%d, next send=%d \n"), msec_elapsed, msec_last_interval, msec_last_time, msec_now, next_send);

  unsigned int sleep_interval;

  if (msec_now >= next_send) {
    sleep_interval = 0;
  } else {
  sleep_interval = next_send - msec_now;
  sleep_interval = sleep_interval * 1000; // turn into usec
    if (sleep_interval > 50000)
      sleep_interval = 50000;
  }

#ifdef WIN32
	Sleep(200);
#else
	//TLOG_Debug(_T("sleeping for %d msec"), sleep_interval); 
  	if (sleep_interval > 0) { 
	  pth_nap(pth_time(0, sleep_interval));
	}
#endif

	gettimeofday(&now,NULL);
	sec_now = now.tv_sec;
	msec_now = sec_now*1000 + now.tv_usec;

	if (msec_now >= next_send) {
	  gettimeofday(&last_update_time_,NULL);
	  process_SIGINT();
	}
  }


  // TODO: possibly periodically check if all main server threads
  //   are running, and if one isn't, restart it?  or exit?

  // cleanup
  remove_pidfile();
  return Lyra::EXIT_OK;
}

////
// Dump////

void LsMain::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LsMain[%p,%d]: port=%d pid=%lu uptime=%d>\n\n"), this, sizeof(LsMain),
	  ServerPort(), ServerPid(), Uptime());
  // dump each object
  pset_->Dump(f, indent);       _ftprintf(f, _T("\n")); fflush(f);
  cset_->Dump(f, indent);       _ftprintf(f, _T("\n")); fflush(f);
  mpool_->Dump(f, indent);      _ftprintf(f, _T("\n")); fflush(f);
  tpool_->Dump(f, indent);      _ftprintf(f, _T("\n")); fflush(f);
  inp_->Dump(f, indent);        _ftprintf(f, _T("\n")); fflush(f);
  outp_->Dump(f, indent);       _ftprintf(f, _T("\n")); fflush(f);
  levelstate_->Dump(f, indent); _ftprintf(f, _T("\n")); fflush(f);
  ctext_->Dump(f, indent);      _ftprintf(f, _T("\n")); fflush(f);
  usock_->Dump(f, indent);      _ftprintf(f, _T("\n")); fflush(f);
  tsock_->Dump(f, indent);      _ftprintf(f, _T("\n")); fflush(f);
  globaldbc_->Dump(f, indent);   _ftprintf(f, _T("\n")); fflush(f);
  leveldbc_->Dump(f, indent);    _ftprintf(f, _T("\n")); fflush(f);
  //  serverdbc_->Dump(f, indent);   _ftprintf(f, _T("\n")); fflush(f);
  idbc_->Dump(f, indent);       _ftprintf(f, _T("\n")); fflush(f);
}

////
// RotateLogs
////

void LsMain::RotateLogs()
{
  close_logs();
  open_logs();
}

////
// open_logs
////

void LsMain::open_logs()
{
  // main log
  if (log_) {
    // LmLogFile implementation
    log_->Init(_T("ls"), _T("main"), LevelDBC()->LevelID());
    log_->Open(GlobalDB()->LogDir());
    // LmLogMPSyslog implementation
    //const char* universe = _tgetenv("UL_UNIVERSE");
    //if (!universe) {
    //  universe = "prod"; // default universe
    // }
    //char univ[80];
    //_stprintf(univ, "lyra-%s", universe);
    //log_->Open("leveld", univ, "ul_main");
  }
  // speech log
  if (slog_) {
    // LmLogFile implementation
     slog_->Init(_T("ls"), _T("speech"), LevelDBC()->LevelID());
     slog_->Open(GlobalDB()->LogDir());
  }
}

////
// close_logs
////

void LsMain::close_logs()
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

int LsMain::create_pidfile()
{
  TCHAR pfname[80];
 _stprintf(pfname, _T("leveld_%u.pid"), LevelDBC()->LevelID());
  TCHAR pidfname[FILENAME_MAX];
  GlobalDB()->GetPidFile(pidfname, pfname);
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

void LsMain::remove_pidfile()
{
  TCHAR pfname[80];
 _stprintf(pfname, _T("leveld_%u.pid"), LevelDBC()->LevelID());
  TCHAR pidfname[FILENAME_MAX];
  GlobalDB()->GetPidFile(pidfname, pfname);
  _tunlink(pidfname);
}

////
// create_sockets: create server sockets, return 0 if ok, -1 otherwise
////

int LsMain::create_sockets(const char* ip_address)
{
  DEFMETHOD(LsMain, create_sockets);
  // initialize server address
  LmSockAddrInet addr;

  if (addr.Init(ip_address, ServerPort()) == -1) { // bind to host's IP address
    log_->Error(_T("%s: could not bind to address %s"), method, ip_address);
  }

  log_->Debug(_T("%s: creating sockets bound to port %d, address %s"), method, ServerPort(), ip_address);
  // create UDP listening socket
  usock_ = LmNEW(LmSocket());
  if (!usock_) {
    log_->Error(_T("%s: could not create UDP socket"), method);
    return -1;
  }
  usock_->Socket(LmSockType::Inet_Datagram());
  usock_->SetSockOpt(SO_REUSEADDR, 1);  // in case server was started soon after it was killed
 
  addr.Init(ip_address, ServerPort());
  if (usock_->Bind(addr) < 0) {
    log_->Error(_T("%s: could not bind UDP socket: %s"), method, strerror(errno));
    return -1;
  }
  addr.Init(ip_address, ServerPort()); // bind back to host's IP address
  log_->Debug(_T("%s: bound to UDP socket at port %d\n"), method, ServerPort());
  // create TCP listening socket
  tsock_ = LmNEW(LmSocket());
  if (!tsock_) {
    log_->Error(_T("%s: could not create TCP socket"), method);
    return -1;
  }
  tsock_->Socket(LmSockType::Inet_Stream());
  tsock_->SetSockOpt(SO_REUSEADDR, 1);
  if (tsock_->Bind(addr) < 0) {
    log_->Error(_T("%s: could not bind TCP socket: %s"), method, strerror(errno));
    return -1;
  }
  if (tsock_->Listen() < 0) {
    log_->Error(_T("%s: could not listen at TCP socket: %s"), method, strerror(errno));
    return -1;
  }

  // sockets created and bound successfully
  return 0;
}

////
// start_threads: start server threads, return 0 if ok, -1 otherwise
////

int LsMain::start_threads()
{
  DEFMETHOD(LsMain, start_threads);
  PThAttr attr;
  LmThread* thread;

  attr.Init();
  // none of the server threads have any return state, and thus are detached
  attr.SetJoinable(FALSE);
  //  attr.SetDetachState(PTH_CREATE_DETACHED);
 // set the stack size to a more reasonable value: 256K (default is 1MB)
  attr.SetStackSize(262144);
  // default scope: process (ie. not bound)
  //  attr.SetScope(PTH_SCOPE_PROCESS);

  // create signal-handling thread
  thread = LmNEW(LsSignalThread(this));
  if (!thread || (thread->Create(&attr)) < 0) {
    log_->Error(_T("%s: could not create signal thread: %s"), method, strerror(errno));
    return -1;
  }
  tpool_->AddThread(LsMain::THREAD_SIGNAL, thread);

 // create level server thread
   thread = LmNEW(LsLevelThread(this));
  if (!thread || (thread->Create(&attr)) < 0) {
    log_->Error(_T("%s: could not level server thread: %s"), method, strerror(errno));
    return -1;
  }
  tpool_->AddThread(LsMain::THREAD_LEVELSERVER, thread);

  // create room server thread
  thread = LmNEW(LsRoomThread(this));
  if (!thread || (thread->Create(&attr)) < 0) {
    log_->Error(_T("%s: could not room server thread: %s"), method, strerror(errno));
    return -1;
  }
  tpool_->AddThread(LsMain::THREAD_ROOMSERVER, thread);


  // create position update thread
  thread = LmNEW(LsPositionThread(this));
  if (!thread || (thread->Create(&attr)) < 0) {
    log_->Error(_T("%s: could not create position thread: %s"), method, strerror(errno));
    return -1;
  }
  tpool_->AddThread(LsMain::THREAD_POSITION, thread);


  // networking threads have system scope (ie. bound to LWP)
  //  attr.SetScope(PTH_SCOPE_SYSTEM);

  // create network output thread
  thread = LmNEW(LsNetworkOutput(this));
  if (!thread || (thread->Create(&attr)) < 0) {
    log_->Error(_T("%s: could not create network output thread: %s"), method, strerror(errno));
    return -1;
  }
  tpool_->AddThread(LsMain::THREAD_NETOUTPUT, thread);

  // create network input thread
  thread = LmNEW(LsNetworkInput(this));

  if (!thread || (thread->Create(&attr)) < 0) {
    log_->Error(_T("%s: could not level server thread: %s"), method, strerror(errno));
    return -1;
  }
  tpool_->AddThread(LsMain::THREAD_NETINPUT, thread);

  return 0;
}

////
// connect_to_databases
////

int LsMain::connect_to_databases()
{
  DEFMETHOD(LsMain, connect_to_databases);

  // item database
  int rc = idbc_->Connect();
  int sc = idbc_->LastSQLCode();
  int lt = idbc_->LastCallTime();
  //  log_->Debug(_T("%s: LmItemDBC::Connect took %d ms"), method, lt);
  if (rc < 0) {
    log_->Error(_T("%s: could not connect to item database; rc=%d, sqlcode=%d"), method, rc, sc);
    return -1;
  }

  // connected to all successfully
  return 0;
}

////
// load_state - load level state from the database
////

int LsMain::load_state()
{
  // load codex text, ignore any errors
  ctext_->Load();
  // load level state

  TCHAR statefile[FILENAME_MAX];
  globaldbc_->GetLevelState(statefile, leveldbc_->LevelID());

  // these lines below load from a GDBM file
  // check if local file exists
  FILE* sf =_tfopen(statefile, _T("r"));
  if (sf != 0) { // file exists
	fclose(sf);
	if (0 == levelstate_->LoadFromDisk()) {
		// remove state file
		_tunlink(statefile);
		return 0;
	}
	else
	{	// on error reading from disk, fall through and try the DB
      TCHAR hname[256];
     _stprintf(hname, _T("(unknown)"));
      gethostname(hname, sizeof(hname));
      LmUtil::SendMail(_T("leveld@underlight"), 
				serverdbc_->DatabaseAdminEmail(), 
				_T("Underlight: database error"),
		       _T("HostName: %s\n")
		       _T("Server Info: level %d, pid %lu\n")
		       _T("Error: Previously saved state file is corrupt - can not load\n"),
		       hname, leveldbc_->LevelID(), ServerPid());
	}
  }

  if (levelstate_->LoadFromDB() < 0) {
    return -1;
  }


  return 0;
}

////
// SetSIGERR - write signal error condition to a buffer so that a non-signal
//    thread can write it to the error log
////

void LsMain::SetSIGERR(const TCHAR* text)
{
 _tcscpy(sigerrtxt_, text);
  sigerr_ = true;
}




////
// process_SIGALRM
////

void LsMain::process_SIGALRM()
{
  DEFMETHOD(LsMain, process_SIGALRM);
  DECLARE_TheLineNum;

  ++alarms_;

  // we need to set the last alarm time in the main thread in case
  // we lose a signal

  SetLastAlarm(time(NULL));

  //  TLOG_Debug(_T("%s: signal '%s' (%d) received, %lu"), method, strsignal(sig), sig, alarms_);

  SMsg_LS_Action msg;

  // print heartbeat?
  if ((alarms_ % ALARM_HEARTBEAT) == 0) {
    //TLOG_Log(_T("%s: called %lu times"), method, alarms_);
    // send heartbeat message to other threads
    msg.Init(SMsg_LS_Action::ACTION_HEARTBEAT);
    LsUtil::BroadcastInternalMessage(this, msg, LsMain::THREAD_SIGNAL);
  }

  // compute local groups? (only if there are players)
  if (((alarms_ % ALARM_COMPUTEGROUPS) == 0) && (PlayerSet()->NumPlayers() > 0)) {
    //    TLOG_Debug(_T("%s: sending compute_groups message"), method);
    msg.Init(SMsg_LS_Action::ACTION_COMPUTE_GROUPS, 0);
    if (LsUtil::SendInternalMessage(this, msg, LsMain::THREAD_ROOMSERVER) < 0) {
      TLOG_Error(_T("%s: could not send compute_groups message to room thread"), method);
    }
  }

  // free old buffers?
  if ((alarms_ % ALARM_FREEBUFS) == 0) {
    msg.Init(SMsg_LS_Action::ACTION_FREEBUFS);
    if (LsUtil::SendInternalMessage(this, msg, LsMain::THREAD_LEVELSERVER) < 0) {
      TLOG_Error(_T("%s: could not send freebufs message to level thread"), method);
    }
  }

  // check idle players? (only if there are players)
  if (((alarms_ % ALARM_CHECKIDLE) == 0) && (PlayerSet()->NumPlayers() > 0)) {
    msg.Init(SMsg_LS_Action::ACTION_CHECK_IDLE_PLAYERS);
    if (LsUtil::SendInternalMessage(this, msg, LsMain::THREAD_NETINPUT) < 0) {
      TLOG_Error(_T("%s: could not send check_players message to netin thread"), method);
    }
  }

  // check idle clients? (only if there are connections)
  if (((alarms_ % ALARM_CHECKIDLE) == 0) && (ConnectionSet()->NumConnections() > 0)) {
    msg.Init(SMsg_LS_Action::ACTION_CHECK_IDLE_CLIENTS);
    if (LsUtil::SendInternalMessage(this, msg, LsMain::THREAD_NETINPUT) < 0) {
      TLOG_Error(_T("%s: could not send check_clients message to netin thread"), method);
    }
  }

  // reap items?
  if ((alarms_ % ALARM_REAPITEMS) == 0) {
    //TLOG_Debug(_T("%s: sending reap_items message"), method);
    msg.Init(SMsg_LS_Action::ACTION_REAP_ITEMS);
    if (LsUtil::SendInternalMessage(this, msg, LsMain::THREAD_ROOMSERVER) < 0) {
      TLOG_Error(_T("%s: could not send reap_items message to room thread"), method);
    }
  }

  // generate items?
  if ((alarms_ % ALARM_GENERATEITEMS) == 0) {
    //TLOG_Debug(_T("%s: sending generate_items message"), method);
    msg.Init(SMsg_LS_Action::ACTION_GENERATE_ITEMS);
    if (LsUtil::SendInternalMessage(this, msg, LsMain::THREAD_ROOMSERVER) < 0) {
      TLOG_Error(_T("%s: could not send generate_items message to room thread"), method);
    }
  }

  // reload codex text?
  if ((alarms_ % ALARM_READCODEX) == 0) {
    msg.Init(SMsg_LS_Action::ACTION_READCODEX);
    if (LsUtil::SendInternalMessage(this, msg, LsMain::THREAD_LEVELSERVER) < 0) {
      TLOG_Error(_T("%s: could not send read_codex message to level thread"), method);
    }
  }

  // save state?
  // no that this code is no longer used- uncomment it to 
  // reactivate the periodic saving of state to the file
  if ((alarms_ % ALARM_SAVESTATE_FILE) == 0) {
	//TLOG_Debug(_T("%s: sending save_state_file message"), method);
  //  msg.Init(SMsg_LS_Action::ACTION_SAVE_STATE_FILE);
    //if (LsUtil::SendInternalMessage(this, msg, LsMain::THREAD_LEVELSERVER) < 0) {
      //TLOG_Error(_T("%s: could not send save_state message to level thread"), method);
  //  }
  }
  if ((alarms_ % ALARM_SAVESTATE_DB) == 0) {
    //TLOG_Debug(_T("%s: sending save_state_db message"), method);
    msg.Init(SMsg_LS_Action::ACTION_SAVE_STATE_DB);
    if (LsUtil::SendInternalMessage(this, msg, LsMain::THREAD_LEVELSERVER) < 0) {
      TLOG_Error(_T("%s: could not send save_state message to level thread"), method);
    }
  }

  // ping connections?
  if (((alarms_ % ALARM_PING) == 0) && (ConnectionSet()->NumConnections() > 0)) {
    msg.Init(SMsg_LS_Action::ACTION_PING);
    if (LsUtil::SendInternalMessage(this, msg, LsMain::THREAD_NETOUTPUT) < 0) {
      TLOG_Error(_T("%s: could not send ping_servers message to netout thread"), method);
    }
  }

  //  TLOG_Debug(_T("%s: alarm reset at exit"), method);
}



////
// process_SIGTERM: shut down, and exit
////

void LsMain::process_SIGTERM()
{
  DEFMETHOD(LsMain, process_SIGTERM);
  DECLARE_TheLineNum;

  SetSIGTERM(false);

  TLOG_Debug(_T("%s: got termination signal; sending exit_message to level server in thread"), method);

  // send exit message to level thread
  SMsg_LS_Action msg;
  msg.Init(SMsg_LS_Action::ACTION_EXIT);
  if (LsUtil::SendInternalMessage(this, msg, LsMain::THREAD_LEVELSERVER) < 0) {
    TLOG_Error(_T("%s: could not send exit message to level server"), method);
  }
}


////
// process_SIGINT - send peer update messages, and periodically include
//   neighbors

void LsMain::process_SIGINT()
{
  DEFMETHOD(LsMain, process_SIGINT);
  DECLARE_TheLineNum;

  //TLOG_Debug(_T("%s: got update timer signal '%s' (%d) received, %lu"), method, strsignal(sig), sig, alarms_);

  ++updates_;

  // LmTimer timer;
  // timer.Start();

  // bool send_neighbors = false;

  // every 10th call, send neighbors
  ///if ((updates_ % 10) == 0) {
    //send_neighbors = true;
    //updates_ = 0;
  //}
  // send localgroup/neighbors peer update
  send_player_updates(updates_);

  // timer.Stop();
  //  TLOG_Debug(_T("%s: send_neighbors=%d"), method, send_neighbors);
}

////
// send_player_updates - send peer update message to all players, optionally
//   adding information on neighbors
////

void LsMain::send_player_updates(int num_updates)
{
  DECLARE_TheLineNum;
  // send local group info to all players

  timeval now;
  gettimeofday(&now, NULL);

  unsigned int sec_elapsed = now.tv_sec - first_update_time_.tv_sec;
  unsigned int sec_last = now.tv_sec - last_update_time_.tv_sec;
  unsigned int msec_elapsed = sec_elapsed*1000 + ((now.tv_usec - first_update_time_.tv_usec)/1000);
  unsigned int msec_last = sec_last*1000 + ((now.tv_usec - last_update_time_.tv_usec)/1000);

  //  TLOG_Debug(_T("Sending updates; elapsed = %d sec, %d msec; since = %d sec, %d msec\n"),  sec_elapsed, msec_elapsed, sec_last, msec_last);

  LsPlayerList players;
  PlayerSet()->GetPlayerList(players);
  for (LsPlayerList::iterator i = players.begin(); !(bool)(i == players.end()); ++i) {
	  // skip every other real time update for TCP-only players
//	if (((*i)->TCPOnly()) && (num_updates%2 == 1)){   
	  //TLOG_Debug(_T("skipping update for TCP only client %d"), (*i)->PlayerName());
	  //continue; }
	//if ((*i)->TCPOnly())
	  //  TLOG_Debug(_T("sending update for TCP only client %d"), (*i)->PlayerName());
    send_RMsg_PlayerUpdate(*i, num_updates);
  }
}

////
// should_add_neighbor
////

bool LsMain::should_add_neighbor(LsPlayer* player, RMsg_PlayerUpdate& msg, 
				 lyra_id_t neighbor_id, LmPeerUpdate& peer_update)
{
    LsPlayer* p = PlayerSet()->GetPlayer(neighbor_id);

    if (!p) 
		return false;

	if (player->RoomID() != p->RoomID()) 
		return false;
	
    peer_update = p->PlayerUpdate();

    // race condition: if the playerid is 0, RMsg_Update message hasn't yet been
    // received from this player; if so, ignore
    if (peer_update.RealtimeID() == Lyra::ID_UNKNOWN) 
		return false;

    // skip update for stationary, inactive players
    if (IsStationary(player, p, &peer_update))  
		return false;

    //TLOG_Debug(_T("Peer update for neighbor %d RTID = %d, player rtid = %d\n"), neighbor_id, peer_update.RealtimeID(), p->RealtimeID());
    
	return true;
}

////
// send_RMsg_PlayerUpdate
////

void LsMain::send_RMsg_PlayerUpdate(LsPlayer* player, int num_updates)
{
  DECLARE_TheLineNum;
  //static const TCHAR* method = "send_RMsg_PlayerUpdate";
  lyra_id_t peers[128]; // a larger number than we should ever include
  memset(&(peers[0]), 0, 128*sizeof(lyra_id_t));

  int lgsize = player->LocalGroup().Size();
  int nnsize = player->Neighbors().Size();
  int num_players = lgsize + nnsize;
  timeval now;
  gettimeofday(&now,NULL);
  int msec_now = now.tv_sec*1000 + (now.tv_usec/1000);
  //TLOG_Debug(_T("Possible update at time %u for player %u; num locals, neighbors = %u, %u"), msec_now, player->PlayerID(), lgsize, nnsize);

  // any work to do?
  if (num_players == 0) {
    //TLOG_Debug(_T("nothing to send to player %u"), player->PlayerID());
    // client actually relies on getting these updates, even if there's nothing in them
    return;
  }
  RMsg_PlayerUpdate msg;
  msg.Init(player->PlayerID(), num_players);
  int player_index = 0;
  LmPeerUpdate peer_update;
  bool add = false;

  LmIdSet lg = player->LocalGroup();
  for (int i = 0; i < lg.Size(); ++i) {
    lyra_id_t neighbor_id = lg.Member(i);
    //    TLOG_Debug(_T("Checking lg %d (player =%d)\n"), neighbor_id, player->PlayerID());
	add = this->should_add_neighbor(player, msg, neighbor_id, peer_update);
	if (add) {
		peer_update.SetLocal(1);
		msg.SetPeerUpdate(player_index, peer_update);
		peers[player_index] = neighbor_id;
		//TLOG_Debug(_T("Adding lg member %d, rtid %d, index %d \n"), neighbor_id, peer_update.RealtimeID(), player_index);
		++player_index;
	}
  }
	
  if ((player->Neighbors().Size() > 0)) {
    // copy neighbors
    LmIdSet nb = player->Neighbors();
    for (int j = 0; j < nb.Size(); ++j) {
      lyra_id_t neighbor_id = nb.Member(j);
      //  TLOG_Debug(_T("Checking neighbor %d (player =%d)\n"), neighbor_id, player->PlayerID());
	  // if we don't match mod 8, discard this neighbor
	  if ((num_updates%8) != (neighbor_id%8))
		  continue;

	  bool duplicate = false;
	  for (int k=0; k< player_index; k++) {
		  if (peers[k] == neighbor_id) {
			  duplicate = true;
			  break; // already in local group!
		  }
	  }

	  if (duplicate)
		  continue;

	  add = this->should_add_neighbor(player, msg, neighbor_id, peer_update);

	  if (add) {
		if (peer_update.AttackBits())
			peer_update.SetLocal(1);
		else 
		  peer_update.SetLocal(0);
		// copy into message
		msg.SetPeerUpdate(player_index, peer_update);
		peers[player_index] = neighbor_id;
		++player_index;
		//	TLOG_Debug(_T("Adding neighbor %u with rtid %d, num_updates %d"), neighbor_id, msg.PeerUpdate(player_index).RealtimeID(), num_updates);
		//TLOG_Debug(_T("sent peer update %d, rtid %d"),player_index, peer_update.RealtimeID());
	  }
    }
  }

  msg.SetNumPlayers(player_index);
  //for (int q=0; q<player_index; q++) {
  //  TLOG_Debug("Message position %d, rtid %d\n", q, msg.PeerUpdate(q).RealtimeID());
  //}

  // now add an update for anyone who has attacked, regardless of position!

  // anything added to message?
  if (player_index == 0) {
    //TLOG_Debug(_T("skipping UDP update to player %p; player index = 0"));
    return;
  }

  // store in message buffer
  //LmSrvMesgBuf mbuf(msg.MessageSize());
  msgbuf_->ReadMessage(msg);
  
  //  if (player->TCPOnly()) { // send message over TCP
  // send message to player over UDP (forward through game server)
  this->SocketUDP()->SendTo(msgbuf_->BufferAddress(), msgbuf_->BufferSize(), player->UpdateAddress());
  //TLOG_Debug(_T("sent UDP update, size=%d, num players=%d, to player %p, addr %d, port %d"),msg.MessageSize(), num_players, player, player->UpdateAddress().IPAddress(), player->UpdateAddress().Port());
}


bool LsMain::IsStationary(LsPlayer* player, LsPlayer* neighbor, LmPeerUpdate* peer_update) {

  // now let's determine if we need to update him
  LsUpdateSet* ps = player->UpdateSet();
  LsLastUpdate* last_update;

  LsUpdateSet::iterator i = ps->find(neighbor->PlayerID());
  if (i != ps->end()) { // position in set; check for stationary and inactive
    last_update = (LsLastUpdate*)((*i).second);
     
    // if the following are equal, consider them stationary and inactive:
    // position, attackbits, hit, wave, sound flags

    //    TLOG_Debug(_T("player %u: last X, Y = %u, %u; current X, Y = %u, %u\n"), neighbor->PlayerID(), last_update->last_update.X(), last_update->last_update.Y(), peer_update->X(), peer_update->Y());

    if ((last_update->last_update.X() == peer_update->X()) &&
	(last_update->last_update.Y() == peer_update->Y()) &&
	(last_update->last_update.Angle() == peer_update->Angle()) &&
	(last_update->last_update.AttackBits() == peer_update->AttackBits()) &&
	(last_update->last_update.HitBits() == peer_update->HitBits()) &&
	(last_update->last_update.Wave() == peer_update->Wave()) &&
	(last_update->last_update.SoundID() == peer_update->SoundID()) &&
	(last_update->last_update.Flags() == peer_update->Flags())) {
      //TLOG_Debug(_T("Position stationary for player %u in list for player %u"), neighbor->PlayerID(), player->PlayerID());
      last_update->num_updates++;
      if ((last_update->num_updates < 3) ||
	  ((last_update->num_updates%10) == 0)) {
	//TLOG_Debug(_T("Added update number %d for player %u in list for player %u"), last_update->num_updates, neighbor->PlayerID(), player->PlayerID());
	return false;
      }  else {
	//		TLOG_Debug(_T("Player %u stationary; update skipped"), neighbor->PlayerID());
	return true;
      }
    } else { // player has moved; reset
      //            TLOG_Debug(_T("Player %u moved; updates reset to 0 in list of player %u"), last_update->num_updates, neighbor->PlayerID(), player->PlayerID());
      last_update->last_update = *peer_update;
      last_update->num_updates = 0;
      return false;
    }
  } else { // not in set - allocate, add now
    //TLOG_Debug(_T("Allocated position for player %u in list of player %u"), neighbor->PlayerID(), player->PlayerID());
    last_update = LmNEW(LsLastUpdate());
    last_update->last_update = *peer_update;
    last_update->num_updates = 0;
    (*ps)[neighbor->PlayerID()] = last_update;
    return false;
  }
}

// MsMain.cpp  -*- C++ -*-
// $Id: MsMain.cpp,v 1.25 1998-02-12 15:33:49-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "MsMain.h"
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <netdb.h>
#include <netinet/in.h>

#include "MsMain.h"
#include "LmGlobalDB.h"
#include "LmUtil.h"
#include "LmServerDBC.h"
#include "MsServerInfo.h"
#include "LmNew.h"
#include "LmLogFile.h"
#include "LmSocket.h"
#include "LmConnection.h"
#include "LmSrvMesgBuf.h"
#include "LmServerDBC.h"
#include "SMsg_All.h"

#ifndef USE_INLINE
#include "MsMain.i"
#endif

// static members
MsMain* MsMain::self = 0;

////
// Constructor
////

MsMain::MsMain()
  : pid_(0),
    restart_(false),
    check_servers_(false)
{
  self = this;
  // see Init
}

////
// Destructor
////

MsMain::~MsMain()
{
#ifndef USE_LMNEW
  LmNew::SetLog(0); // if not using the logging new/delete, reset the log
#endif /* !USE_LMNEW */
  close_log();
  LmDELETE(log_);
  LmDELETE(globaldb_);
  LmDELETE(serverdbc_);
  LmDELETE(sinfo_);
}

////
// Log: return pointer to log object
////

LmLog* MsMain::Log() const
{
  return log_;
}

////
// Init: load server info table; return 0 if ok, < 0 if not
////

int MsMain::Init(const TCHAR* root_dir)
{
  pid_ = getpid();

  // get host name
  if (gethostname(hostname_, sizeof(hostname_)) < 0) {
    return -1;
  }

  // get host IP address in string form
  //hostent* lphp = gethostbyname(hostname_);
  
  //in_addr* localIP = (in_addr*)lphp->h_addr;
  
  //char* hoststr = (char*)inet_ntoa((in_addr*)localIP);
  
  //_tcscpy(hostaddrstr_, hoststr);
  
  // create databases

  globaldb_ = LmNEW(LmGlobalDB(root_dir));
  serverdbc_ = LmNEW(LmServerDBC());

  // load passwords

  // create and open log file
  log_ = LmNEW(LmLogFile());

  open_log();

  TCHAR pw_file[FILENAME_MAX];
  globaldb_->GetPasswordFile(pw_file);
  if (serverdbc_->LoadPasswords(pw_file) < 0)
    return -1;
  if (serverdbc_->Connect() < 0)
    return -1;
  if (serverdbc_->Load() < 0)
    return -1;
  serverdbc_->Disconnect();

#ifndef USE_LMNEW
  // if not using the logging new/delete, install the handler using the main lo
  LmNew::SetLog(log_);
  LmNew::InstallNewHandler();
#endif /* !USE_LMNEW */

  // create everything else
  sinfo_ = LmNEW(MsServerInfo(this));

  //  log_->Debug(_T("Host address: %s"), hostaddrstr_);

  return 0;
}

////
// Go: when this returns, the server exits
////

void MsMain::Go()
{
  DEFMETHOD(MsMain, Go);

  if (create_pidfile() < 0) {
    log_->Error(_T(": could not create pidfile"), method);
    return;
  }
  install_signals();

  log_->Debug(_T("Masterd about to start servers"));

  // start servers -- first dbd, then level servers, then game servers
  if ((sinfo_->StartServers(LmServerDBC::ST_DATABASE) < 0) ||
      (sinfo_->StartServers(LmServerDBC::ST_LEVEL) < 0) ||
      (sinfo_->StartServers(LmServerDBC::ST_GAME) < 0)) {
    log_->Error(_T(": could not start servers"), method);
    // in case some got started, kill 'em all
    sinfo_->SignalServers(SIGTERM);
    return;
  }
  log_->Debug(_T("Masterd started servers"));

  // main loop
  done_ = false;
  bool clean_exit = false;
  log_->Debug(_T("Masterd entering main loop"));

  while (!done_) {
    // wait for a child to die, or for a signal to be caught
    // (only want signals to be caught here, blocked at all other times)
    int status;
    dump_status_ = rotate_logs_ = false;
    LmUtil::UnBlockAllSignals();
    log_->Debug(_T("Masterd waiting for child to die"));
    pid_t pid = wait(&status);
    LmUtil::BlockAllSignals();
    if ((pid == -1) && (errno == EINTR)) { // signal received and handled
#if 0
      if (dump_status_) {
	TCHAR dfname[80];
_stprintf(dfname, "ms_%lu_dump.%lu", pid_, time(NULL));
	TCHAR dumpfile[FILENAME_MAX];
	GlobalDB()->GetDumpFile(dumpfile, dfname);
	FILE* df =_tfopen(dumpfile, "w");
	Dump(df);
	fclose(df);
      } 
#endif
      if (rotate_logs_) {
	log_->Log(_T("%s: rotating log"), method);
	close_log();
	open_log();
      }
      if (check_servers_) {
	//	log_->Log(_T("%s: checking server status at time %d"), method, time(NULL));
	for (int i = 0; i < serverdbc_->NumServers(); ++i) {
	  // if server arg1 is 0, it's an inactive server, don't try to connect
	  if (serverdbc_->Arg1(i) == 0) 
	    continue;
	  // we're really only worried about gamed's right now
	  if (serverdbc_->ServerType(i) != LmServerDBC::ST_GAME)
	    continue; 
	  // create socket
	  LmSocket sock;
	  if (sock.Socket(LmSockType::Inet_Stream()) < 0) {
	    log_->Log(_T("Could not create socket to check server index %d\n"), i);
	    continue;
	  }
	  // get server address
	  LmSockAddrInet addr;
	  addr.Init(serverdbc_->HostIPAddr(i), serverdbc_->Arg1(i));
	  // attempt to connect to server
	  if (sock.Connect(addr) < 0) {
	    pid = sinfo_->ServerPid(i);
	    if (pid > -1) {
	      log_->Log(_T("Server Index %d DOWN; restarting\n"), i);
	      handle_child_death(pid, Lyra::EXIT_OK);
	    } else {
	      log_->Log(_T("Server Index %d DOWN; could not find in list, not restarting\n"), i);
	    }
	  } else {  // use a dummy login/logout to be clean about things
	    get_status(sock);
	  }
	  sock.Close();
	}

	check_servers_ = false;
      }
      continue;
    }
    else if (pid == -1) { // error
      if (errno == ECHILD) { // no children, so don't bother to stick around
	done_ = true;
      }
      else {
	log_->Error(_T(": wait: "), method, strerror(errno));
      }
    }
    else { // child died
      handle_child_death(pid, status);
    }
  }
  // stop servers
  sinfo_->StopServers();
  // wait for them all to exit
  int slept = 0;
  while (slept < 60) { // wait up to a minute
    // int rc = wait(NULL); // this would hang forever
    int rc = waitpid(-1, NULL, WNOHANG);
    if (rc == 0) { // no children exited yet
      log_->Debug(_T("%s: no children available; slept=%d"), method, slept);
      sleep(5); // sleep a bit until they do
      slept += 5;
      continue;
    }
    else if (rc == -1) { // error (most likely no child processes)
      if (errno == ECHILD) {
	log_->Log(_T("%s: all children exited normally"), method);
	clean_exit = true;
      }
      else {
	log_->Warning(_T("%s: waitpid: %s"), method, strerror(errno));
      }
      break; // exit on any error
    }
    log_->Log(_T("%s: child %lu exited"), method, rc);
  }
  // just in case there are some stragglers
  if (!clean_exit)
    sinfo_->SignalServers(SIGKILL);
  // cleanup
  remove_pidfile();
}

////
// get_status
////

void MsMain::get_status(LmSocket& sock)
{ 
  // this function is essentially a dummy, because whether or not we
  // can connect at all tells us if we're OK. However, if we do not
  // properly login and logout, we'll confused the gamed's and they
  // will crash!

  LmSrvMesgBuf msgbuf;
  // initialize connection object
  LmConnection conn;
  conn.Init(sock);
  conn.SetMessageRange(SMsg::MIN, SMsg::MAX);
  // login to server
  SMsg_Login msg_login;
  msg_login.Init(LmConnection::CT_MONITOR, 0);
  msgbuf.ReadMessage(msg_login);
  if (conn.SendMessage(msgbuf) < 0) {
    return;
  }
  // logout from server
  SMsg_Logout msg_logout;
  msgbuf.ReadMessage(msg_logout);
  conn.SendMessage(msgbuf);
}


////
// Dump
////

void MsMain::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<MsMain[%p,%d]: pid=%lu>\n\n"), this, sizeof(MsMain), ServerPid());
  // dump each object
  sinfo_->Dump(f, indent);    _ftprintf(f, "\n");
  serverdbc_->Dump(f, indent); _ftprintf(f, "\n");
  globaldb_->Dump(f, indent); _ftprintf(f, "\n");
}

////
// open_log
////

void MsMain::open_log()
{
  if (log_) {
    log_->Init("ms", "main", ServerPid());
    log_->Open(GlobalDB()->LogDir());
    //const char* universe = _tgetenv("UL_UNIVERSE");
    //if (!universe) {
    //  universe = "prod"; // default universe
    // }
    //char univ[80];
    //_stprintf(univ, "lyra-%s", universe);
    //log_->Open("masterd", univ, "ul_main");
  }
}

////
// close_log
////

void MsMain::close_log()
{
  if (log_) {
    log_->Close();
  }
}

////
// create_pidfile: create server pid file
////

int MsMain::create_pidfile()
{
  TCHAR pidfname[FILENAME_MAX];
  GlobalDB()->GetPidFile(pidfname, "masterd.pid");
  FILE* pidf =_tfopen(pidfname, "w");
  if (!pidf) {
    return -1;
  }
 _ftprintf(pidf, "%lu\n", ServerPid());
  fclose(pidf);
  return 0;
}

////
// remove_pidfile: remove server pid file
////

void MsMain::remove_pidfile()
{
  TCHAR pidfname[FILENAME_MAX];
  GlobalDB()->GetPidFile(pidfname, "masterd.pid");
  _tunlink(pidfname);
}

////
// install_signals: tell system about signals we want to get
////

void MsMain::install_signals()
{
  static int sigs[] = {
    SIGTERM,
    SIGUSR1,
    SIGUSR2
  };
  static int num_sigs = sizeof(sigs) / sizeof(int);
  // install dummy handler, necessary to unblock ignored signals
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = dispatch_signal;
  sigemptyset(&sa.sa_mask);
  for (int i = 0; i < num_sigs; ++i) {
    sigaction(sigs[i], &sa, NULL);
  }
}

////
// dispatch_signal: dummy handler that sends signal to object
////

void MsMain::dispatch_signal(int sig)
{
  MsMain::self->handle_signal(sig);
}

////
// handle_signal: handle a signal
////

void MsMain::handle_signal(int sig)
{
  DEFMETHOD(MsMain, handle_signal);
  // be very careful logging in signal handlers
  // log_->Debug(_T("%s: signal '%s' (%d) received"), method, strsignal(sig), sig);
  // handle
  switch (sig) {

    // restarting is no longer supported, since other tasks (such
    // as removing items from full rooms, and waiting for the servers
    // to shut down properly) are necessary on start

    //case SIGHUP: { // exit, and restart
    //    done_ = true;
    //    restart_ = true;
    //  }
    //  break;

    // SIGUSR1 used to be dump state (per-Ul2000)
    // now it means check all servers to ensure they are up

  case SIGUSR1: { // check server status
      check_servers_ = true;
      break;
    }

  case SIGUSR2: { // rotate logs
      rotate_logs_ = true;
      break;
    }

  case SIGTERM: { // exit, no restart
    done_ = true;
    restart_ = false;
    break;
  }

  break;
  default: {
    //    log_->Warning(_T("%s: unhandled signal '%s' (%d) received"), method, strsignal(sig), sig);
  }

  break;
  }
}

////
// handle_child_death: deal with a child server exiting/crashing
////

void MsMain::handle_child_death(pid_t childpid, int status)
{
  DEFMETHOD(MsMain, handle_child_death);
  log_->Debug(_T("%s: child %lu died with status %d"), method, childpid, status);
  // if we're exiting anyway, don't do anything more
  if (done_) {
    log_->Debug(_T("%s: not restarting, we're bailing"), method);
    return;
  }
  int sleep_time = 30; // 30 seconds sleep by default before restart
#ifdef RELEASE
  bool restart = true;
#else
  bool restart = false;
#endif

  bool email_admin = false; // don't email by default
  TCHAR* death_reason = _T("unknown");
  int email_code = 0;
  // check that child was one of the servers
  if (!sinfo_->HasServer(childpid)) {
    log_->Error(_T(": child 0 not found in server list!"), method, childpid);
    return;
  }
  // get server index, type, args
  int server_index = sinfo_->ServerIndex(childpid);
  int srv_type = serverdbc_->ServerType(server_index);
  int srv_arg1 = serverdbc_->Arg1(server_index);
  int srv_arg2 = serverdbc_->Arg2(server_index);

  // did child exit normally?
  if (WIFEXITED(status)) {
    int cstatus = WEXITSTATUS(status);
    // restart, but sleep a bit longer
    switch (cstatus) {
    case Lyra::EXIT_CLOSED: 
      log_->Log(_T("%s: child %lu exited normally, does not want a restart"), method, childpid);
      restart = false;
    case Lyra::EXIT_OK:
      log_->Log(_T("%s: child %lu exited normally"), method, childpid);
      // no email
      break;
    case Lyra::EXIT_ARGS:
    case Lyra::EXIT_INIT:
    case Lyra::EXIT_EXEC:
      log_->Log(_T("%s: child %lu exited with known error code %d"), method, childpid, cstatus);
      // email notification
      //      email_admin = true;
      death_reason = "exited with error code";
      email_code = cstatus;
      break;
    default:
      log_->Log(_T("%s: child %lu exited with unknown status code %d"), method, childpid, cstatus);
      // restart = false; // now we do restart
      // email notification
      //      email_admin = true;

      death_reason = "exited with unknown status code";
      email_code = cstatus;
      break;
    }
  }
  // was child killed by a signal?  (ie. crash)
  else if (WIFSIGNALED(status)) {
    int termsig = WTERMSIG(status);
    log_->Log(_T("%s: child %lu killed by signal %d"), method, childpid,  termsig);
    //    log_->Log(_T("%s: child %lu killed by signal '%s' (%d)"), method, childpid, strsignal(termsig), termsig);
    // rename the core file, so it won't be overwritten by others
    TCHAR newname[40];
   _stprintf(newname, "../cores/core.%lu", childpid);
    _trename("core", newname);
    // email notification
    //    email_admin = true;
    death_reason = "killed by signal";
    email_code = termsig;
  }
  // something else happen?
  else {
    log_->Log(_T("%s: child %lu exited unusually with status %d"), method, childpid, status);
    // email notification
    //    email_admin = true;
    death_reason = "exited/crashed with unusual status";
    email_code = status;
  }

  // ensure process is gone for good
  kill(childpid, SIGKILL);
  // if a gamed crashed, launch fix_ghost utility to clean up ghosts

  if ((srv_type == LmServerDBC::ST_GAME) && (restart)) {
    // fork

    pid_t pid = fork();

    if (pid == -1) {
      Log()->Error(": could not fork to launch fix_ghosts.pl : ", method, strerror(errno));
    }
    
    if (pid == 0) { // child
      LmUtil::CloseAllDescriptors(); // close all files
      const TCHAR* rootdir = GlobalDB()->RootDir();
      TCHAR servexec[FILENAME_MAX], arg1[32], arg2[32];
     _stprintf(arg1, "%s", serverdbc_->HostName(server_index));
     _stprintf(arg2, "%d", srv_arg1);
     _stprintf(servexec, "/bin/fix_ghost", rootdir);
      // if execl fails, it will fall through
      int rc =_texecl(servexec, rootdir, arg1, arg2, NULL);
      FILE* f =_tfopen("/tmp/masterd.out", "a");
     _ftprintf(f, _T("return value %d, error %s while executing %s\n"), rc, strerror(errno), servexec);
      fclose(f);
      exit(Lyra::EXIT_ARGS);
    } else { // add new child to list of known children processes
      //      sinfo_->AddGamedCleanup(pid);
    }
  }

  // restart?
  if (restart) {
    log_->Log(_T("%s: restarting server in %d seconds"), method, sleep_time);
    sleep(sleep_time);
    sinfo_->RestartServer(childpid);
  }
  else {
    log_->Log(_T("%s: not restarting server"), method);
    sinfo_->RemoveServer(childpid);
  }

  // send mail to admin?
  if (email_admin) {
    LmUtil::SendMail(_T("masterd@underlight"), serverdbc_->DatabaseAdminEmail(), _T("Underlight: server exit"),  _T("HostName: %s\n")     _T("Error: return code %d from Underlight server, child %lu\n")     _T("Reason: %s %d\n")     _T("Server type: %c, args: %d %d; restarted: %d\n"),     HostName(),  status, childpid, death_reason, email_code, srv_type, srv_arg1, srv_arg2, restart);
  }
}

// MsMain.h  -*- C++ -*-
// $Id: MsMain.h,v 1.7 1998-02-12 15:33:49-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// master server main class

#ifndef INCLUDED_MsMain
#define INCLUDED_MsMain

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <sys/types.h>
#include <list>

#include "LyraDefs.h"
#include "LmServerDBC.h"

// forward declarations

class LmGlobalDB;
class MsServerInfo;
class LmLog;
class LmLogFile;
class LmSocket;

// the class


class MsMain {


public:

  MsMain();
  ~MsMain();

  int Init(const TCHAR* root_dir);

  void Go();

  LmLog* Log() const;
  const LmGlobalDB* GlobalDB() const;
  const LmServerDBC* ServerDB() const; 
  MsServerInfo* ServerInfo() const;
  pid_t ServerPid() const;
  const char* HostName() const;
  const char* HostAddrString() const;
  bool Restart() const;
  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  MsMain(const MsMain&);
  //operator=(const MsMain&);

  void open_log();
  void close_log();
  int create_pidfile();
  void remove_pidfile();
  int wait_for_signal();
  void install_signals();
  void handle_child_death(pid_t childpid, int status);
  static void dispatch_signal(int sig);
  void handle_signal(int sig);
  void get_status(LmSocket& sock);

  static MsMain* self;    // static pointer to instance, for signal handler to use

  pid_t pid_;             // process id of server
  pid_t closed_gamed_pid_; // pid of gamed server that sent SIGUSR1 to indicate it is closing and needs to be restarted
  bool dump_status_;          // signal to dump status received
  bool rotate_logs_;          // signal to rotate logs received

  //  char hostid_[LmServerDBC::MAX_HOSTID];  // logical server name for this host
  char hostname_[LmServerDBC::HOSTNAME_MAX];  // name of this host
  char hostaddrstr_[LmServerDBC::HOSTNAME_MAX];  // ip string of this host

  LmLogFile* log_;        // log file

  LmGlobalDB* globaldb_;  // global database
  LmServerDBC* serverdbc_;  // server info database
  MsServerInfo* sinfo_;   // child server info

  bool done_;             // exit main loop flag
  bool restart_;          // restart after exit flag
  bool check_servers_;   // time to check server status

};

#ifdef USE_INLINE
#include "MsMain.i"
#endif

#endif /* INCLUDED_MsMain */

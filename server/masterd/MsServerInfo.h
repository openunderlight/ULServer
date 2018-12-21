// MsServerInfo.h  -*- C++ -*-
// $Id: MsServerInfo.h,v 1.6 1997-10-09 19:12:25-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// child server information

#ifndef INCLUDED_MsServerInfo
#define INCLUDED_MsServerInfo

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <list>
#include <stdlib.h>
#include "../../libsrc/shared/LyraDefs.h"

// forward declarations

class MsMain;

// the class

class MsServerInfo {

public:

  MsServerInfo(MsMain* msmain);
  ~MsServerInfo();

  int NumServers() const;
  bool HasServer(pid_t pid) const;
  int ServerIndex(pid_t pid) const;
  int ServerPid(int index) const;

  int StartServers(int servertype);
  int AddGamedCleanup(int pid);
  int SignalServers(int sig);
  int StopServers();
  int RemoveServer(pid_t pid);
  int RestartServer(pid_t pid);

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  MsServerInfo(const MsServerInfo&);
  //operator=(const MsServerInfo&);

  pid_t start_server(int server_index, int next_index);

  int next_server_index(int server_index);

  MsMain* main_;

  // child server info
  struct cs_t {
    pid_t pid;      // process id of child
    time_t started; // time server was started
    int index;      // index into server db
    cs_t(pid_t p = 0, time_t s = 0, int i = 0) {
      pid = p;
      started = s;
      index = i;
    }
  };
  typedef std::list<cs_t> cs_list_t;

  cs_list_t children_;

};

#ifdef USE_INLINE
#include "MsServerInfo.i"
#endif

#endif /* INCLUDED_MsServerInfo */

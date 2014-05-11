// MsServerInfo.cpp  -*- C++ -*-
// $Id: MsServerInfo.cpp,v 1.16 1998-02-12 15:33:49-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <list>

#include "MsServerInfo.h"
#include "MsMain.h"
#include "LmServerDBC.h"
#include "LmLog.h"
#include "LmUtil.h"
#include "LmGlobalDB.h"

#define ROUND_ROBIN
//#define GAMED_POINTER

#ifndef USE_INLINE
#include "MsServerInfo.i"
#endif

DECLARE_TheFileName;

////
// Constructor
////

MsServerInfo::MsServerInfo(MsMain* msmain)
  : main_(msmain)
{
  DECLARE_TheLineNum;
  // empty
}

////
// Destructor
////

MsServerInfo::~MsServerInfo()
{
  DECLARE_TheLineNum;
  // empty
}

////
// ServerIndex: return index into ServerDB for given child, -1 if not found
////

int MsServerInfo::ServerIndex(pid_t pid) const
{
  for (cs_list_t::const_iterator i = children_.begin(); i != children_.end(); ++i) {
    if ((*i).pid == pid) {
      return (*i).index;
    }
  }
  return -1;
}

////
// ServerPid: given a server index, return the associated pid 
////

int MsServerInfo::ServerPid(int index) const
{
  for (cs_list_t::const_iterator i = children_.begin(); i != children_.end(); ++i) {
    if ((*i).index == index) {
      return (*i).pid;
    }
  }
  return -1;
}

////
// HasServer: return true if pid is one of the children in list
////

bool MsServerInfo::HasServer(pid_t child) const
{
  return (ServerIndex(child) != -1);
}

////
// StartServers: start all servers for this logical server
////

int MsServerInfo::StartServers(int servtype)
{
  DEFMETHOD(MsServerInfo, StartServers);
  DECLARE_TheLineNum;
  int retval = 0;

  main_->Log()->Debug(_T("%s: starting servers of type %u; total servers = %u"), method, servtype, main_->ServerDB()->NumServers());
  for (int i = 0; i < main_->ServerDB()->NumServers(); ++i) {
    // right server type?
    if (servtype != main_->ServerDB()->ServerType(i)) {
      continue;
    }

    // arg1 = 0?
    if (main_->ServerDB()->Arg1(i) == 0) {
      continue;
    }

    // start it up
    int next = -1;
    if (main_->ServerDB()->AllowPlayers(i))
      next = next_server_index(i);
    else
      next = i; // for non-player gameds, no rotation - just keep running

    pid_t childpid = start_server(i, next);
    if (childpid < 0) {
      main_->Log()->Error(_T("%s: could not start server at index %d"), method, i);
      retval = -1;
    }
    else {
      cs_t child(childpid, time(NULL), i);
      children_.push_back(child);
    }

    // wait a few seconds before forking the next one off
    sleep(1);
  }
  return retval;
}

////
// StopServers: stop all active servers (send SIGTERM signal)
////

int MsServerInfo::StopServers()
{
  DEFMETHOD(MsServerInfo, StopServers);
  DECLARE_TheLineNum;
  main_->Log()->Debug(_T("%s: shutting down servers"), method);
  return SignalServers(SIGTERM);
}

////
// SignalServers: signal all active servers with given signal
////

int MsServerInfo::SignalServers(int sig)
{
  DEFMETHOD(MsServerInfo, SignalServers);
  DECLARE_TheLineNum;
  int retval = 0; // OK
  for (cs_list_t::iterator i = children_.begin(); i != children_.end(); ++i) {
    cs_t cs = *i;
    main_->Log()->Debug(_T("%s: sending signal %d to child %lu"), method, sig, cs.pid);
    //    main_->Log()->Debug(_T("%s: sending signal '%s' (%d) to child %lu"), method, strsignal(sig), sig, cs.pid);
    if (kill(cs.pid, sig) < 0) {
      //main_->Log()->Error(_T("%s: sigsend: %s"), method, strerror(errno));
      retval = -1;
      // continue anyway
    }
  }
  return retval;
}

////
// RemoveServer: remove given server from list
////

int MsServerInfo::RemoveServer(pid_t pid)
{
  DEFMETHOD(MsServerInfo, RemoveServer);
  DECLARE_TheLineNum;
  main_->Log()->Debug(_T("%s: removing pid %lu"), method, pid);
  // find child info
  for (cs_list_t::iterator i = children_.begin(); i != children_.end(); ++i) {
    cs_t cs = *i;
    if (cs.pid == pid) { // found
      main_->Log()->Debug(_T("%s: found child %lu, index %d"), method, pid, cs.index);
      children_.erase(i); // remove from list
      return 0;
    }
  }
  return -1; // not found
}

////
// RestartServer: restart server with given pid (after it crashed or otherwise exited)
////

int MsServerInfo::RestartServer(pid_t pid)
{
  DEFMETHOD(MsServerInfo, RestartServer);
  DECLARE_TheLineNum;
  // main_->Log()->Debug(_T("%s: restarting pid %lu"), method, pid);
  // find child info
  for (cs_list_t::iterator i = children_.begin(); i != children_.end(); ++i) {
    cs_t cs = *i;
    if (cs.pid == pid) { // found
      // main_->Log()->Debug(_T("%s: found child %lu, index %d"), method, pid, cs.index);
      children_.erase(i); // remove from list
      int next = next_server_index(cs.index);
      cs.pid = start_server(cs.index, next); // restart
      if (cs.pid < 0) { // couldn't restart
	return -1;
      }
      cs.started = time(NULL);
      children_.push_back(cs); // add to list
      return 0;
    }
  }
  return -1; // not found
}

////
// Dump
////

void MsServerInfo::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  INDENT(indent, f);
 _ftprintf(f, _T("<MsServerInfo[%p,%d] >\n"), this, sizeof(MsServerInfo));
  // print child list
  for (cs_list_t::const_iterator i = children_.begin(); i != children_.end(); ++i) {
    cs_t cs = *i;
    INDENT(indent + 1, f);
   _ftprintf(f, _T("child %lu: index=%d started=%lu : %s"), // ctime adds \n
	    cs.pid, cs.index, cs.started,_tctime(&cs.started));
  }
}

////
// next_server_index: given a server index, find the index of the 
//     "next" instance of a server of the same type that allows players
////

int MsServerInfo::next_server_index(int server_index) {
#ifdef ROUND_ROBIN
  return -1; // not necessary for round robin selection
#endif
  int first = 9999999; // first, last are used to find the first, last indexes
  int last = -1; // of servers of this type
  int next = -1; // used to determine the "next" server index
  int i;
  int count = 0;

  int servtype = main_->ServerDB()->ServerType(server_index);

  for (i = 0; i < main_->ServerDB()->NumServers(); ++i) {
    // right server type?
    if (servtype != main_->ServerDB()->ServerType(i)) 
      continue;
    if (!main_->ServerDB()->AllowPlayers(i)) 
      continue;

    count++; 
 
    if (i < first)
      first = i;
    if (i > last)
      last  = i;
  }

  if (count == 0) 
    return server_index; // no other servers of this type that allow players

  // now that we have the first and the last, we can loop through
  // to find the "next" server, from the given index

  // determine index of "next" server of this type
  if (server_index == last) {
    return first;
  } else {
    next = server_index;
    while (1) {
      next = next + 1;
      if ((servtype == main_->ServerDB()->ServerType(next)) && 
	  (main_->ServerDB()->AllowPlayers(next)))
	  return next;
    }
  }
  
  // we should never get here, but if we do, return same index
  return server_index;
}

////
// StartGamedCleanup: start fix_ghost.pl to clean up after gamed exit
////

int MsServerInfo::AddGamedCleanup(int pid) 
{
      cs_t child(pid, time(NULL), children_.size());
      children_.push_back(child);
      return 0;
}

////
// start_server: start given server, return its pid, or -1 if error
////

pid_t MsServerInfo::start_server(int server_index, int next_index)
{
  DEFMETHOD(MsServerInfo, start_server);
  DECLARE_TheLineNum;
  // determine server name, args
  int servtype = main_->ServerDB()->ServerType(server_index);
  const TCHAR* servname = main_->ServerDB()->ProgramName(servtype);
  if (!servname) {
    main_->Log()->Error(_T("%s: null server name, type=%c"), method, servtype);
    return -1;
  }
  // put integer args into strings
  TCHAR arg1[32], arg2[32], arg3[32], arg4[32], arg5[32];
 _stprintf(arg1, "%d", main_->ServerDB()->Arg1(server_index));
 _stprintf(arg2, "%d", main_->ServerDB()->Arg2(server_index));
 // _stprintf(arg3, "\"%s\"", main_->ServerDB()->HostName(server_index));
  _stprintf(arg3, "%s", main_->ServerDB()->HostName(server_index));
  
  if ((servtype == LmServerDBC::ST_GAME) && (0 <= next_index)) {
    _stprintf(arg4, "%s", main_->ServerDB()->HostName(next_index));
    _stprintf(arg5, "%d", main_->ServerDB()->Arg1(next_index));
  } else {
    _stprintf(arg4, "0");
    _stprintf(arg5, "0");
  } 
  // get full path of executable, root directory
  TCHAR servexec[FILENAME_MAX];
  main_->GlobalDB()->GetExecFile(servexec, servname);
  const TCHAR* rootdir = main_->GlobalDB()->RootDir();
  // fork
  pid_t pid = fork();

  if (pid == -1) {
    main_->Log()->Error(_T("%s: could not fork: %s"), method, strerror(errno));
    return -1;
  }
  if (pid == 0) { // child
    LmUtil::CloseAllDescriptors(); // close all files
    // exec server; all servers take 3 args: the root dir, plus two integral args
    int rc;
    if (servtype == LmServerDBC::ST_GAME) 
       rc =_texecl(servexec, servexec, rootdir, arg1, arg2, arg3, arg4, arg5, NULL);
    else if (servtype == LmServerDBC::ST_DATABASE)
       rc =_texecl(servexec, servexec, rootdir, arg1, arg3, NULL);
    else if (servtype == LmServerDBC::ST_LEVEL)
       rc =_texecl(servexec, servexec, rootdir, arg1, arg2, arg3, NULL);
    if (rc < 0) {
      // FILE* f =_tfopen("child.out", "a");
      //_ftprintf(f, _T("error: while trying to execute '%s %s %s %s'"), servexec, rootdir, arg1, arg2); 
      //_ftprintf(f, _T("error: execl: %s"), strerror(errno));
      // fclose(f);
    }
    // if execl fails, this is called
    exit(Lyra::EXIT_ARGS);
  }
  // else - parent
  main_->Log()->Debug(_T("%s: started '%s', args '%s %s %s %s %s', pid %lu"), method, servexec, arg1, arg2, arg3, arg4, arg5, pid);
  return pid;
}

// LmNew.cpp  -*- C++ -*-
// $Id: LmNew.cpp,v 1.8 1998-02-12 15:33:26-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#include <stddef.h>
#include <stdlib.h>
#include <new>
//#include <new.h>

#include "LmNew.h"
#include "LmLog.h"
#include "PTh.h"

#ifndef WIN32
#include <unistd.h>
#endif

// log for new/delete ops
static LmLog* the_log = 0;

////
// TheNewHandler - called when new can't allocate memory
////

#ifdef WIN32
int __cdecl TheNewHandler(size_t size)
#else
void TheNewHandler()
#endif
{
  DEFMETHOD(LmNew, TheNewHandler);
  // if the log has been set up, print an error message
  if (the_log) {
#ifdef WIN32
	  MessageBox(NULL, "operator new failed, aborting", "New Failed!", MB_OK);
#else
	the_log->Error("%s: operation new failed, aborting", method);
#endif
    the_log->FlushLog();
  }
  abort();
}


////
// InstallNewHandler - installs global new handler
////

void LmNew::InstallNewHandler()
{
#ifdef WIN32
	_set_new_handler(&(::TheNewHandler));
#else
	std::set_new_handler(&::TheNewHandler);
#endif
}

////
// SetLog - sets the log to use for the new handler, new/delete operators
////

void LmNew::SetLog(LmLog* log)
{
  the_log = log;
}


////
// New (must use malloc(), as this replaces global operator new)
////

void* LmNew::New(size_t size, const TCHAR* file, int line)
{
  DEFMETHOD(LmNew, New);
  // allocate some memory
  void* retval = malloc(size);
  // log it if log is defined
  if (the_log) {
#ifdef WIN32
    //pthread_t self = PTh::Self();
	void* brk = NULL;
#else
   // st_thread_t self = PTh::Self(); // NOTE: removed for gcc 3.0
    void* brk = sbrk(0);
#endif
    if (line != 0) {
      //the_log->Debug(_T("%s: thread %lu <%s:%d> alloc(%d) -> [%p]; brk=[%p]"), method,
		//     self, file, line, size, retval, brk);
      the_log->Debug(_T("%s: <%s:%d> alloc(%d) -> [%p]; brk=[%p]"), method,
		     file, line, size, retval, brk);
    }
    else {
      //the_log->Debug(_T("%s: thread %lu <%s> alloc(%d) -> [%p]; brk=[%p]"), method,
		//     self, file, size, retval, brk);
      the_log->Debug(_T("%s: <%s> alloc(%d) -> [%p]; brk=[%p]"), method,
		     file, size, retval, brk);

    }
    the_log->FlushLog();
  }
  // if memory could not be allocated, call the new handler
  if (!retval) {
#ifdef WIN32
    ::TheNewHandler(size);
#else
    ::TheNewHandler();
#endif
  }
  // return it
  return retval;
}

////
// Delete (must use free(), as this replaces global operator delete) 
////

void LmNew::Delete(void* ptr)
{
  // free it if it's not null
  if (ptr) {
    free(ptr);
  }
}

////
// LogDelete - since we can't make an operator delete(ptr, file, line), use this instead
////

void LmNew::LogDelete(void* ptr, const TCHAR* file, int line)
{
  DEFMETHOD(LmNew, LogDelete);
  // if log is defined
  if (the_log) {
#ifdef WIN32
    //pthread_t self = PTh::Self();
    void* brk = NULL;
#else
    //st_thread_t self = PTh::Self(); // NOTE: removed for gcc 3.0
    void* brk = sbrk(0);
#endif
    if (line != 0) {
      the_log->Debug(_T("%s: <%s:%d> freeing [%p]; brk=[%p]"), method, file, line, ptr, brk);
      //the_log->Debug(_T("%s: thread %lu <%s:%d> freeing [%p]; brk=[%p]"), method, self, file, line, ptr, brk);
    }
    else {
      the_log->Debug(_T("%s: <%s> freeing [%p]; brk=[%p]"), method, file, ptr, brk);
      //the_log->Debug(_T("%s: thread %lu <%s> freeing [%p]; brk=[%p]"), method, self, file, ptr, brk);
    }
    the_log->FlushLog();
  }
}

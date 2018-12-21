// PTh.cpp -*- C++ -*-
// $Id: PTh.cpp,v 1.11 1997-08-20 01:34:36-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// Implementation of PTh class, a C++ wrapper around a POSIX thread

#ifdef __GNUC__
#pragma implementation "PTh.h"
#endif

#include "PTh.h"
#include "PThAttr.h"
#include "../../libsrc/shared/LyraDefs.h"

#ifndef USE_INLINE
#include "PTh.i"
#endif

////
// constructor
////

PTh::PTh()
  : running_(false)
{
	stackaddr_ = 0;
}

////
// destructor
////

PTh::~PTh()
{
#ifndef WIN32
	if (0 != stackaddr_)
		LmDELETEARRAY(stackaddr_);
#endif
  // check for running?  (and then what?)
}

////
// Create: creates thread
//   returns -1 if thread has already been started, or result from pth_create()
////

int PTh::Create(const PThAttr* p_Attr)
{
  // only allow one thread per object to be running
  if (running_) return -1;
  running_ = true;

  // start thread
#ifdef WIN32
  return pthread_create(&thread_, (p_Attr ? &p_Attr->attr_ : 0), entry, this);
#else

  // create the stack to be used for the thread	
  // we can't trust the pTh memory management!
  unsigned int stacksize;
  p_Attr->GetStackSize(&stacksize);
  stackaddr_ = LmNEW(char[stacksize]);
  pth_attr_set(p_Attr->attr_, PTH_ATTR_STACK_ADDR, stackaddr_);

  thread_ = pth_spawn(p_Attr->attr_, entry, this);
  return 1;
#endif
}

////
// entry: static entry point for threads; simply calls "Run" for given thread object
////

void* PTh::entry(void* arg)
{
  PTh* this_ = (PTh*) arg;
  this_->Run();
  this_->DoneRunning();
#ifndef WIN32
  pth_exit(0);
#endif
  return 0;
}

////
// Dump
////

void PTh::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<PTh[%p,%d]: thread=%lu running=%d>\n"), this, sizeof(PTh), thread_, running_);
}

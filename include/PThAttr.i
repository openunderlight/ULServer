// PThAttr.i  -*- C++ -*-
// $Id: PThAttr.i,v 1.2 1997-07-08 13:58:37-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// optionally inlined methods


INLINE PThAttr::PThAttr()
{

}

INLINE PThAttr::~PThAttr()
{
#ifdef WIN32
  pthread_attr_destroy(&attr_);
#endif
}

INLINE int PThAttr::Init()
{
#ifdef WIN32
  return pthread_attr_init(&attr_);
#else
  stacksize = 2091752;
  joinable = true;
#endif
}

INLINE int PThAttr::SetJoinable(int joinable)
{
#ifdef WIN32
  return pthread_attr_setdetachstate(&attr_, joinable);  
#else
  this->joinable = joinable;
  return 0;
#endif
}

INLINE int PThAttr::GetJoinable(int* joinable) const
{
#ifdef WIN32
  return pthread_attr_getdetachstate(&attr_, joinable);
#else
  return this->joinable;
#endif
}

INLINE int PThAttr::SetStackSize(unsigned int stacksize)
{
#ifdef WIN32
  return pthread_attr_setstacksize(&attr_, stacksize);
#else
  this->stacksize = stacksize;
#endif
}

INLINE int PThAttr::GetStackSize(unsigned int *stacksize) const
{
#ifdef WIN32
  return pthread_attr_getstacksize(&attr_, stacksize);
#else
  return this->stacksize;
#endif
}


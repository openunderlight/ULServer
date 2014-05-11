// PThAttr.i  -*- C++ -*-
// $Id: PThAttr.i,v 1.2 1997-07-08 13:58:37-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// optionally inlined methods


INLINE PThAttr::PThAttr()
{
#ifndef WIN32
  attr_ = pth_attr_new();
#endif
}

INLINE PThAttr::~PThAttr()
{
#ifdef WIN32
  pthread_attr_destroy(&attr_);
#else
  pth_attr_destroy(attr_);
#endif
}

INLINE int PThAttr::Init()
{
#ifdef WIN32
  return pthread_attr_init(&attr_);
#else
  return pth_attr_init(attr_);
#endif
}

INLINE int PThAttr::SetJoinable(int joinable)
{
#ifdef WIN32
  return pthread_attr_setdetachstate(&attr_, joinable);  
#else
  return pth_attr_set(attr_, PTH_ATTR_JOINABLE, joinable);
#endif
}

INLINE int PThAttr::GetJoinable(int* joinable) const
{
#ifdef WIN32
  return pthread_attr_getdetachstate(&attr_, joinable);
#else
  return pth_attr_get(attr_, PTH_ATTR_JOINABLE, joinable);
#endif
}

INLINE int PThAttr::SetStackSize(unsigned int stacksize)
{
#ifdef WIN32
  return pthread_attr_setstacksize(&attr_, stacksize);
#else
  return pth_attr_set(attr_, PTH_ATTR_STACK_SIZE, stacksize);
#endif
}

INLINE int PThAttr::GetStackSize(unsigned int *stacksize) const
{
#ifdef WIN32
  return pthread_attr_getstacksize(&attr_, stacksize);
#else
  return pth_attr_get(attr_, PTH_ATTR_STACK_SIZE, stacksize);
#endif
}


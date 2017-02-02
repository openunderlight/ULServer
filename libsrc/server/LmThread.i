// LmThread.h  -*- C++ -*-
// $Id: LmThread.i,v 1.6 1997-08-29 18:04:32-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE bool LmThread::Done() const
{
  return done_;
}

INLINE LmMesgBufPool* LmThread::BufferPool() const
{
  return bp_;
}

INLINE void LmThread::SetDone(bool done)
{
  done_ = done;
}

INLINE void LmThread::SetDefaultHandler(MsgHandler hfunc)
{
  default_handler_ = hfunc;
}

INLINE bool LmThread::Busy() const
{
  return busy_;
}

INLINE LmLog* LmThread::Log() const
{
  return log_;
}

//INLINE PthCond* LmThread::Condition() const
//{
  //return &condition_;	
//}
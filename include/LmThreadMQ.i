// LmThreadMQ.h  -*- C++ -*-
// $Id: LmThreadMQ.i,v 1.2 1997-07-14 17:53:36-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE LmThreadMQItem::LmThreadMQItem(LmSrvMesgBuf* b, LmConnection* c)
{
  msgbuf_ = b;
  conn_ = c;
}

INLINE LmSrvMesgBuf* LmThreadMQItem::MessageBuffer() const
{
  return msgbuf_;
}

INLINE LmConnection* LmThreadMQItem::Connection() const
{
  return conn_;
}

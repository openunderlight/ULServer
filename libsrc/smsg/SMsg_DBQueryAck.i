// SMsg_DBQueryAck.i  -*- C++ -*-
// $Id: SMsg_DBQueryAck.i,v 1.1 1998-04-17 16:32:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void SMsg_DBQueryAck::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t SMsg_DBQueryAck::ID() const
{
  return data_.player_id;
}

INLINE unsigned int SMsg_DBQueryAck::Size() const
{
  return data_.size;
}


INLINE unsigned char* SMsg_DBQueryAck::Result() const
{
  return (unsigned char*)data_.result;
}

INLINE unsigned char SMsg_DBQueryAck::DB() const
{
  return data_.db;
}

INLINE unsigned char SMsg_DBQueryAck::ThreadType() const
{
  return data_.thread_type;
}


INLINE void SMsg_DBQueryAck::SetDB(unsigned char db)
{
  data_.db = db;
}

INLINE void SMsg_DBQueryAck::SetThreadType(unsigned char thread_type)
{
  data_.thread_type = thread_type;
}

INLINE void SMsg_DBQueryAck::SetID(lyra_id_t id)
{
  data_.player_id = id;
}



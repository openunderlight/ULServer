// SMsg_DBQuery.i  -*- C++ -*-
// $Id: SMsg_DBQuery.i,v 1.1 1998-04-17 16:32:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void SMsg_DBQuery::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t SMsg_DBQuery::ID() const
{
  return data_.player_id;
}

INLINE unsigned char SMsg_DBQuery::Result() const
{
  return data_.result;
}

INLINE unsigned char SMsg_DBQuery::DB() const
{
  return data_.db;
}

INLINE unsigned char SMsg_DBQuery::ThreadType() const
{
  return data_.thread_type;
}

INLINE const TCHAR* SMsg_DBQuery::Query() const
{
  return data_.query;
}

INLINE void SMsg_DBQuery::SetResult(unsigned char result)
{
  data_.result = result;
}

INLINE void SMsg_DBQuery::SetDB(unsigned char db)
{
  data_.db = db;
}

INLINE void SMsg_DBQuery::SetThreadType(unsigned char thread_type)
{
  data_.thread_type = thread_type;
}

INLINE void SMsg_DBQuery::SetID(lyra_id_t id)
{
  data_.player_id = id;
}



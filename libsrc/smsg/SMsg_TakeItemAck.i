// SMsg_TakeItemAck.i  -*- C++ -*-
// $Id: SMsg_TakeItemAck.i,v 1.1 1997-09-18 16:49:05-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void SMsg_TakeItemAck::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int SMsg_TakeItemAck::Status() const
{
  return data_.status;
}

INLINE lyra_id_t SMsg_TakeItemAck::SourceID() const
{
  return data_.sourceid;
}

INLINE lyra_id_t SMsg_TakeItemAck::TargetID() const
{
  return data_.targetid;
}

INLINE const LmItemHdr& SMsg_TakeItemAck::ItemHeader() const
{
  return data_.itemheader;
}

INLINE void SMsg_TakeItemAck::SetStatus(int status)
{
  data_.status = status;
}

INLINE void SMsg_TakeItemAck::SetSourceID(lyra_id_t sourceid)
{
  data_.sourceid = sourceid;
}

INLINE void SMsg_TakeItemAck::SetTargetID(lyra_id_t targetid)
{
  data_.targetid = targetid;
}

INLINE void SMsg_TakeItemAck::SetItemHeader(const LmItemHdr& itemheader)
{
  data_.itemheader = itemheader;
}

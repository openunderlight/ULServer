// SMsg_GiveItemAck.i  -*- C++ -*-
// $Id: SMsg_GiveItemAck.i,v 1.1 1997-09-19 16:42:02-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void SMsg_GiveItemAck::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int SMsg_GiveItemAck::Status() const
{
  return data_.status;
}

INLINE lyra_id_t SMsg_GiveItemAck::SourceID() const
{
  return data_.sourceid;
}

INLINE lyra_id_t SMsg_GiveItemAck::TargetID() const
{
  return data_.targetid;
}

INLINE const LmItemHdr& SMsg_GiveItemAck::ItemHeader() const
{
  return data_.itemheader;
}

INLINE void SMsg_GiveItemAck::SetStatus(int status)
{
  data_.status = status;
}

INLINE void SMsg_GiveItemAck::SetSourceID(lyra_id_t sourceid)
{
  data_.sourceid = sourceid;
}

INLINE void SMsg_GiveItemAck::SetTargetID(lyra_id_t targetid)
{
  data_.targetid = targetid;
}

INLINE void SMsg_GiveItemAck::SetItemHeader(const LmItemHdr& itemheader)
{
  data_.itemheader = itemheader;
}

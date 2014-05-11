// LmConnection.i  -*- C++ -*-
// $Id: LmConnection.i,v 1.10 1997-10-30 12:38:37-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE time_t LmConnection::ConnectTime() const
{
  return time_connect_;
}

INLINE time_t LmConnection::LastIn() const
{
  return time_in_;
}

INLINE time_t LmConnection::LastOut() const
{
  return time_out_;
}

INLINE int LmConnection::MessagesIn() const
{
  return msgs_in_;
}

INLINE int LmConnection::MessagesInRate() const
{
  return MessagesIn() / Online();
}

INLINE int LmConnection::MessagesOut() const
{
  return msgs_out_;
}

INLINE int LmConnection::MessagesOutRate() const
{
  return MessagesOut() / Online();
}

INLINE int LmConnection::BytesIn() const
{
  return bytes_in_;
}

INLINE int LmConnection::BytesInRate() const
{
  return BytesIn() / Online();
}

INLINE int LmConnection::BytesOut() const
{
  return bytes_out_;
}

INLINE int LmConnection::BytesOutRate() const
{
  return BytesOut() / Online();
}

INLINE void LmConnection::SetID(lyra_id_t id)
{
  id_ = id;
}

INLINE void LmConnection::SetType(int contype)
{
  type_ = contype;
}

INLINE LmSocket& LmConnection::Socket()
{
  return sock_;
}

INLINE lyra_id_t LmConnection::ID() const
{
  return id_;
}

INLINE int LmConnection::Type() const
{
  return type_;
}



INLINE time_t LmConnection::Online() const
{
  // don't return 0 ever
  time_t retval = LmUtil::TimeSince(time_connect_);
  if (retval == 0) {
    retval = 1;
  }
  return retval;
}

INLINE time_t LmConnection::IdleIn() const
{
  return LmUtil::TimeSince(time_in_);
}

INLINE time_t LmConnection::IdleOut() const
{
  return LmUtil::TimeSince(time_out_);
}

INLINE bool LmConnection::IsConnected() const
{
  return is_connected_;
}

#if 0
INLINE bool LmConnection::IsClosed() const
{
  return is_closed_;
}
#endif

INLINE void LmConnection::SetMessageRange(int min_mtype, int max_mtype)
{
  msg_min_ = min_mtype;
  msg_max_ = max_mtype;
}

INLINE int LmConnection::MinMessageType() const
{
  return msg_min_;
}

INLINE int LmConnection::MaxMessageType() const
{
  return msg_max_;
}

INLINE time_t LmConnection::ChallengeTime() const
{
	return challenge_time_;
}

INLINE TCHAR* LmConnection::Challenge() const
{
	return (TCHAR*)(challenge_);
}

INLINE void LmConnection::SetChallengeTime() 
{
	time(&challenge_time_);
}

INLINE void LmConnection::SetChallenge(TCHAR* value) 
{
	_tcsncpy(challenge_, value, Lyra::CHALLENGE_LENGTH);
}



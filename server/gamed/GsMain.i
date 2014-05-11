// GsMain.i  -*- C++ -*-
// $Id: GsMain.i,v 1.18 1998-02-12 15:33:31-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

INLINE int GsMain::MaxPlayers() const
{
  return max_players_;
}

INLINE const LmGlobalDB* GsMain::GlobalDB() const
{
  return globaldbc_;
}


INLINE const LmServerDBC* GsMain::ServerDBC() const
{
  return serverdbc_;
}

INLINE LmPlayerNameMap* GsMain::PlayerNameMap() const
{
  return pnames_;
}

INLINE GsLevelSet* GsMain::LevelSet() const
{
  return lset_;
}

INLINE GsPlayerSet* GsMain::PlayerSet() const
{
  return pset_;
}

INLINE LmThreadPool* GsMain::ThreadPool() const
{
  return tpool_;
}

INLINE GsOutputDispatch* GsMain::OutputDispatch() const
{
  return outp_;
}

INLINE GsInputDispatch* GsMain::InputDispatch() const
{
  return inp_;
}

INLINE LmConnectionSet* GsMain::ConnectionSet() const
{
  return cset_;
}

INLINE LmMesgBufPool* GsMain::BufferPool() const
{
  return mpool_;
}

INLINE LmSocket* GsMain::SocketTCP() const
{
  return tsock_;
}

INLINE LmSocket* GsMain::SocketUDP() const
{
  return usock_;
}

INLINE int GsMain::ServerPort() const
{
  return portnum_;
}

INLINE int GsMain::NextPort() const
{
  return next_port_;
}

INLINE pid_t GsMain::ServerPid() const
{
  return pid_;
}

INLINE pid_t GsMain::ParentPid() const
{
  return ppid_;
}

INLINE time_t GsMain::LastAlarm(void) const
{
  return last_alrm_;
}


INLINE GsPlayerThreadSet* GsMain::PlayerThreadSet() const
{
  return ptset_;
}

INLINE LmGuildDBC* GsMain::GuildDBC() const
{
  return gdbc_;
}

INLINE LmItemDBC* GsMain::ItemDBC() const
{
  return idbc_;
}

INLINE LmPlayerDBC* GsMain::PlayerDBC() const
{
  return pdbc_;
}

INLINE LmBillingDBC* GsMain::BillingDBC() const
{
  return bdbc_;
}

INLINE int GsMain::Uptime() const
{
  return LmUtil::TimeSince(start_time_);
}

INLINE char* GsMain::HostIP() const
{
  return (char*)host_ip_;
}

INLINE char* GsMain::NextIP() const
{
  return (char*)next_ip_;
}

INLINE unsigned long GsMain::HostIPAddress() 
{
	return host_ip_addr_;
}

INLINE bool GsMain::Closed() const
{
  return closed_;
}

INLINE int GsMain::NumLogins() const
{
 return num_logins_;
}

INLINE void GsMain::SetNumLogins(int num_logins)
{
 num_logins_ = num_logins;
}

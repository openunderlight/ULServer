// LmServerDBC.i  -*- C++ -*-
// $Id: LmServerDBC.i,v 1.3 1997/10/30 20:38:19 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods


INLINE int LmServerDBC::NumServers() const
{
  return num_servers_;
}

INLINE const TCHAR* LmServerDBC::HostName(int index) const
{
  return servers_[index].hostname;
}

INLINE const TCHAR* LmServerDBC::HostID(int index) const
{
  return servers_[index].hostid;
}

INLINE unsigned long LmServerDBC::HostIPAddr(int index) const
{
  return servers_[index].host_addr;
}

INLINE int LmServerDBC::ServerType(int index) const
{
  return servers_[index].servertype;
}

INLINE int LmServerDBC::Arg1(int index) const
{
  return servers_[index].arg1;
}

INLINE int LmServerDBC::Arg2(int index) const
{
  return servers_[index].arg2;
}

INLINE bool LmServerDBC::AllowPlayers(int index) const
{
  return servers_[index].allow_players;
}


INLINE unsigned long LmServerDBC::LevelServerIP(lyra_id_t levelid) const
{
  int index = FindLevelServer(levelid);
  return (index != -1) ? HostIPAddr(index) : 0;
}

INLINE int LmServerDBC::LevelServerPort(lyra_id_t levelid) const
{
  int index = FindLevelServer(levelid);
  return (index != -1) ? Arg1(index) : 0;
}

INLINE unsigned long LmServerDBC::DatabaseServerIP() const
{
  int index = FindDatabaseServer();
  return (index != -1) ? HostIPAddr(index) : 0;
}

INLINE int LmServerDBC::DatabaseServerPort() const
{
  int index = FindDatabaseServer();
  return (index != -1) ? Arg1(index) : 0;
}

INLINE const TCHAR* LmServerDBC::DatabaseHost() const
{
  return db_host_;
}

INLINE const TCHAR* LmServerDBC::DatabaseAdminEmail() const
{
  return db_admin_email_;
}

INLINE const TCHAR* LmServerDBC::DatabaseReturnEmail() const
{
  return db_return_email_;
}

INLINE int LmServerDBC::DatabasePort() const
{
  return db_port_;
}

INLINE const TCHAR* LmServerDBC::ServerDBPassword() const
{
  return sdb_passwd_;
}

INLINE const TCHAR* LmServerDBC::ItemDBPassword() const
{
  return idb_passwd_;
}

INLINE const TCHAR* LmServerDBC::PlayerDBPassword() const
{
  return pdb_passwd_;
}

INLINE const TCHAR* LmServerDBC::GuildDBPassword() const
{
  return gdb_passwd_;
}

INLINE const TCHAR* LmServerDBC::LevelDBPassword() const
{
  return ldb_passwd_;
}

INLINE const TCHAR* LmServerDBC::BillingDBPassword() const
{
  return bdb_passwd_;
}


INLINE long LmServerDBC::LastSQLCode() const
{
  return last_sql_code_;
}

INLINE void LmServerDBC::SetLog(LmLog* log)
{
  log_ = log;
}

INLINE LmLog* LmServerDBC::Log() const
{
  return log_;
}

INLINE long LmServerDBC::LastCallTime() const
{
  return last_ms_;
}

INLINE long LmServerDBC::TotalCallTime() const
{
  return num_ms_;
}

INLINE long LmServerDBC::TotalSQLTime() const
{
  return sql_ms_;
}


INLINE const TCHAR* LmServerDBC::ServerDBUsername() const
{
  return _T("ul_server");;
}

INLINE const TCHAR* LmServerDBC::ItemDBUsername() const
{
  return _T("ul_item");
}

INLINE const TCHAR* LmServerDBC::PlayerDBUsername() const
{
  return _T("ul_player");
}

INLINE const TCHAR* LmServerDBC::GuildDBUsername() const
{
  return _T("ul_guild");
}

INLINE const TCHAR* LmServerDBC::LevelDBUsername() const
{
  return _T("ul_level");
}

INLINE const TCHAR* LmServerDBC::BillingDBUsername() const
{
  return _T("ul_billing");
}


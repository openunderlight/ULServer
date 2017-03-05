// LsMain.i  -*- C++ -*-
// $Id: LsMain.i,v 1.20 1998-02-20 20:26:54-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions


INLINE const LmGlobalDB* LsMain::GlobalDB() const
{
  return globaldbc_;
}


INLINE const LmServerDBC* LsMain::ServerDBC() const
{
  return serverdbc_;
}

INLINE const LmLevelDBC* LsMain::LevelDBC() const
{
  return leveldbc_;
}

INLINE LsLevelState* LsMain::LevelState() const
{
  return levelstate_;
}

INLINE LsPlayerSet* LsMain::PlayerSet() const
{
  return pset_;
}

INLINE LmThreadPool* LsMain::ThreadPool() const
{
  return tpool_;
}

INLINE LsOutputDispatch* LsMain::OutputDispatch() const
{
  return outp_;
}

INLINE LsInputDispatch* LsMain::InputDispatch() const
{
  return inp_;
}

INLINE LmConnectionSet* LsMain::ConnectionSet() const
{
  return cset_;
}

INLINE LmMesgBufPool* LsMain::BufferPool() const
{
  return mpool_;
}

INLINE LmSocket* LsMain::SocketUDP() const
{
  return usock_;
}

INLINE LmSocket* LsMain::SocketTCP() const
{
  return tsock_;
}

INLINE pid_t LsMain::ServerPid() const
{
  return pid_;
}

INLINE pid_t LsMain::ParentPid() const
{
  return ppid_;
}

INLINE int LsMain::ServerPort() const
{
  return port_num_;
}

INLINE LmItemDBC* LsMain::ItemDBC() const
{
  return idbc_;
}

INLINE lyra_id_t LsMain::LevelNum() const
{
  return level_num_;
}

INLINE int LsMain::Uptime() const
{
  return LmUtil::TimeSince(start_time_);
}

INLINE LsCodexText* LsMain::CodexText() const
{
  return ctext_;
}

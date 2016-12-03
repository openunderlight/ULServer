// LsMain.h  -*- C++ -*-
// $Id: LsMain.h,v 1.40 1998-02-20 20:26:54-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// level server main class

#ifndef INCLUDED_LsMain
#define INCLUDED_LsMain

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <sys/types.h>

#include "LyraDefs.h"
#include "LmUtil.h"

// forward declarations

class LmGlobalDB;
class LmServerDBC;
class LmThreadPool;
class LmMesgBufPool;
class LmConnectionSet;
class LmSocket;
class LsLevelState;
class LsPlayerSet;
class LsInputDispatch;
class LsOutputDispatch;
class LmPeerUpdate;
class LsSignalThread;
class LsCodexText;
class PTh;
class LmItemDBC;
class LmLevelDBC;
class LmLogFile;
class LmLog;
class LsPlayer;
class LmSrvMesgBuf;
//class LmLogMPSyslog;
class RMsg_PlayerUpdate;

// the class

class LsMain {

public:

  enum {
    // thread id constants
    THREAD_LEVELSERVER = -1,
    THREAD_SIGNAL = -2,
    THREAD_NETINPUT = -3,
    THREAD_NETOUTPUT = -4,
    THREAD_ROOMSERVER = -5,
    THREAD_POSITION = -6,

    // constants
    SPARE_MBUFS = 128,           // spare message buffers in pool
    MAX_CONNECTIONS = 64,      // maximum number of network connections supported (<FD_SETSIZE)
                                 // (computed roughly as (max game servers) + some extra)

    SPEECH_DIST2 = (1300*1300),   // distance speech can travel, squared

    // timer parameters
    ALARM_DELAY = 1,             // alarm delay in seconds
    ALARM_HEARTBEAT = 600,       // delay: print heartbeat message
    ALARM_COMPUTEGROUPS = 2,     // delay: compute and send local groups
    ALARM_CHECKIDLE = 29,        // delay: check idle players and clients
    ALARM_REAPITEMS = 19,        // delay: reap items
    ALARM_FREEBUFS = 599,        // delay: free old message buffers
    ALARM_GENERATEITEMS = 17,    // delay: generate items
    //ALARM_SAVESTATE_FILE = 123,  // delay: save room state to disk
    //ALARM_SAVESTATE_DB = 1800,   // delay: save room state to DB (thirty minutes)
    ALARM_SAVESTATE_FILE = 60,  // delay: save room state to disk
    ALARM_SAVESTATE_DB = 300,   // delay: save room state to DB (thirty minutes)
    ALARM_READCODEX = 300,       // delay: read codex generator text
    ALARM_PING = 61,              // delay: ping connected servers
	POS_UPDATE_INTERVAL = 250	  // send position updates every 125 ms

 
  };

public:

  LsMain();
  ~LsMain();

  int Init(const TCHAR* root_dir, int port_num, int levelnum);
  int Go(const char* ip_address);
  int DoPostSignalProcessing();

  int Uptime() const;
  pid_t ServerPid() const;
  pid_t ParentPid() const;
  const LmGlobalDB* GlobalDB() const;
  const LmServerDBC* ServerDBC() const;
  const LmLevelDBC* LevelDBC() const;
  LsLevelState* LevelState() const;
  LsCodexText* CodexText() const;
  LsPlayerSet* PlayerSet() const;
  LmThreadPool* ThreadPool() const;
  LmLog* Log() const;
  LmLog* SpeechLog() const;
  LsInputDispatch* InputDispatch() const;
  LsOutputDispatch* OutputDispatch() const;
  LmConnectionSet* ConnectionSet() const;
  LmMesgBufPool* BufferPool() const;
  int ServerPort() const;
  LmSocket* SocketUDP() const;
  LmSocket* SocketTCP() const;
  LmItemDBC* ItemDBC() const;
  time_t LastAlarm() const;
  lyra_id_t LevelNum() const;
  void SetLastAlarm(time_t value) { last_alrm_ = value; };

  void SetSIGINT(bool value) { sigint_ = value; };
  void SetSIGTERM(bool value) { sigterm_ = value; };
  void SetSIGERR(const TCHAR* text);

  void process_SIGALRM();
  void process_SIGTERM();
  void process_SIGINT();

  bool IsStationary(LsPlayer* player, LsPlayer* neighbor, LmPeerUpdate* peer_update);

  // aux message sending methods
  void send_player_updates(int num_updates);
  bool should_add_neighbor(LsPlayer* player, RMsg_PlayerUpdate& msg, 
	  lyra_id_t neighbor_id, LmPeerUpdate& peer_update);

  // message sending methods
  void send_RMsg_PlayerUpdate(LsPlayer* player, int num_updates);

  void RotateLogs();

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  LsMain(const LsMain&);
  //operator=(const LsMain&);

  void open_logs();
  void close_logs();
  int create_pidfile();
  int create_sockets(const char* ip_address);
  int start_threads();
  int connect_to_databases();
  int load_state();
  void remove_pidfile();
  LmSrvMesgBuf* msgbuf_;
  
  unsigned int updates_;
  TCHAR sigerrtxt_[256];
  bool sigerr_;
  bool sigint_;
  bool sigterm_;

  unsigned long alarms_;

  timeval first_update_time_;
  timeval last_update_time_;


  TCHAR sigdebuglogtxt_[256];
  TCHAR sigwarnlogtxt_[256];
  TCHAR sigerrorlogtxt_[256];

  int level_num_;             // id of level being served

  pid_t pid_;                 // process id of server
  pid_t ppid_;                // process id of server's parent

  time_t last_alrm_;          // time of last alarm signal

  time_t start_time_;         // time when server started

  LmGlobalDB* globaldbc_;      // global property database
  LmServerDBC* serverdbc_;      // server info database

  LmLevelDBC* leveldbc_;        // static level properties & db info
  LsLevelState* levelstate_;  // dynamic level state
  LsPlayerSet* pset_;         // players in level
  LsCodexText* ctext_;        // codex generator text

  LmMesgBufPool* mpool_;      // global message buffers
  LmConnectionSet* cset_;     // network connections

  int port_num_;              // port to use for sockets
  LmSocket* usock_;           // UDP socket for position update
  LmSocket* tsock_;           // TCP listening socket

  LsInputDispatch* inp_;      // input message dispatcher
  LsOutputDispatch* outp_;    // output message dispatcher

  LmLogFile* log_;            // global log file
  LmLogFile* slog_;           // speech log file

  LmThreadPool* tpool_;       // server threads

  LmItemDBC* idbc_;           // item database connection

};

#ifdef USE_INLINE
#include "LsMain.i"
#endif

#endif /* INCLUDED_LsMain */

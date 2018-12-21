// GsMain.h  -*- C++ -*-
// $Id: GsMain.h,v 1.68 1998-02-12 15:33:31-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// game server main class

#ifndef INCLUDED_GsMain
#define INCLUDED_GsMain

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <sys/types.h>
#include "../../libsrc/shared/LyraDefs.h"
#include "LmUtil.h"



// forward declarations

class LmGlobalDB;
class LmServerDBC;
class LmLevelDBC;
class LmThreadPool;
class LmMesgBufPool;
class LmConnectionSet;
class LmSocket;
class GsPlayerSet;
class GsInputDispatch;
class GsOutputDispatch;
class GsLevelSet;
class GsPlayerThreadSet;
class PTh;
class LmGuildDBC;
class LmPlayerNameMap;
class LmItemDBC;
class LmPlayerDBC;
class LmBillingDBC;
class LmLogFile;
class LmLog;
class LmSockAddrInet;

// the class

class GsMain {

public:

  enum {
    // thread id constants
    THREAD_GAMESERVER = -1,
    THREAD_SIGNAL = -2,
    THREAD_NETINPUT = -3,
    THREAD_NETOUTPUT = -4,
    THREAD_FORWARD = -5,
    THREAD_POSITION = -6,

    // constants
    GAME_VERSION = Lyra::GAME_VERSION,
    GAME_SUBVERSION = Lyra::GAME_SUBVERSION,

    SPARE_MBUFS = 128,       // spare message buffers in pool
    MAX_CONNECTIONS = 1000,   // maximum number of network connections supported (<FD_SETSIZE)
    //MAX_CONNECTIONS = 51,   // maximum number of network connections supported (<FD_SETSIZE)
                             // (computed roughly as MAX_PLAYERS + (total level servers) + some extra)
    MAX_PLAYERS = 160,       // absolute maximum number of players using server
    EXTRA_PLAYERS = 10,      // extra admin players allowed
	//MAX_LOGINS = 100,		 // total # of logins accepted before closing (GAMED_POINTER only)
//	CLOSING_THRESHHOLD = 200, // don't close until we've handled >= this many logins (ROUND_ROBIN only)
	
    MAX_MOVE = (300 * 300),   // max distance squared a player can move between updates received (max
                             // movement is roughly 100-150),
    // timer parameters
    ALARM_DELAY = 1,             // alarm delay in seconds
    ALARM_FREEBUFS = 599,        // delay: free old message buffers
    ALARM_HEARTBEAT = 600,       // delay: print heartbeat message
    ALARM_CHECKIDLE = 31,        // delay: check idle players and clients
    ALARM_CHECKTHREADS = 29,     // delay: check idle message threads
    ALARM_PING = 59,             // delay: ping server connections
    ALARM_WAITPID = 33,          // delay: wait() on any children (Oracle creates zombies... sheesh)
    //ALARM_SAVEPLAYER = 10,       // delay: save player information (notify player threads)
    ALARM_SAVEPLAYER = 60,       // delay: save player information (notify player threads)
    ALARM_BAIL = (3600 * 1),     // delay: exit, unless players are logged in (check every hour)

    MIN_UPTIME = (3600 * 12)     // minimum amount of server uptime before we will exit (12 hours)
  };

public:

  GsMain();
  ~GsMain();

  int Init(const TCHAR* root_dir, int max_players, const char* next_ip_address, int next_server_port);
  int CreateSockets(int min_port, int max_port, const char* ip_address);

  int Go();
  int DoPostSignalProcessing();
  void Close();

  int MaxPlayers() const;
  pid_t ServerPid() const;
  pid_t ParentPid() const;
  int Uptime() const;
  const LmGlobalDB* GlobalDB() const;
  const LmServerDBC* ServerDBC() const;
  GsPlayerSet* PlayerSet() const;
  GsLevelSet* LevelSet() const;
  LmThreadPool* ThreadPool() const;
  GsPlayerThreadSet* PlayerThreadSet() const;
  LmLog* Log() const;
  LmLog* SecurityLog() const;
  GsInputDispatch* InputDispatch() const;
  GsOutputDispatch* OutputDispatch() const;
  LmConnectionSet* ConnectionSet() const;
  LmMesgBufPool* BufferPool() const;
  LmSocket* SocketTCP() const;
  LmSocket* SocketUDP() const;
  char* HostIP() const;
  unsigned long HostIPAddress();
  char* NextIP() const;
  int ServerPort() const;
  int NextPort() const;

  void SetSIGTERM(bool value) { sigterm_ = value; };
  void SetSIGERR(const TCHAR* text);

  void process_SIGTERM();
  void process_SIGALRM();

  time_t LastAlarm() const;
  void SetLastAlarm(time_t value) { last_alrm_ = value; };

  LmGuildDBC* GuildDBC() const;
  LmItemDBC* ItemDBC() const;
  LmPlayerDBC* PlayerDBC() const;
  LmBillingDBC* BillingDBC() const;
  LmPlayerNameMap* PlayerNameMap() const;

  //bool IsActiveServer() const;
  bool Closed() const;
#if 0 // round robin only now
  bool TryToClose();
  bool ReadClosingFile();
#endif
  int NumLogins() const;
  void SetNumLogins(int num_logins);

  void RotateLogs();
  

  void Dump(FILE* f, int indent = 0) const;

private:

  TCHAR sigerrtxt_[256];
  bool sigerr_;
  bool sigterm_;

  unsigned long alarms_;  

  TCHAR sigdebuglogtxt_[256];
  TCHAR sigwarnlogtxt_[256];
  TCHAR sigerrorlogtxt_[256];

  char host_ip_[Lyra::IP_LENGTH];
  unsigned long host_ip_addr_; // same as above, as a 4 byte IP address
  char next_ip_[Lyra::IP_LENGTH];
  unsigned long closing_ip[Lyra::MAX_CLOSING_GAMEDS];
  unsigned short closing_port[Lyra::MAX_CLOSING_GAMEDS];

  // not implemented
  GsMain(const GsMain&);
  //operator=(const GsMain&);

  void open_logs();
  void close_logs();
  int create_pidfile();
  int create_sockets();
  int start_threads();
  int connect_to_databases();
  void save_player_files() const;
  void remove_pidfile();

  pid_t pid_;                 // process id of server
  pid_t ppid_;                // process id of server's parent
  time_t start_time_;         // time when server started
  time_t last_alrm_;          // time of last alarm

  LmGlobalDB* globaldbc_;      // global property database
  LmServerDBC* serverdbc_;      // server info database
  LmPlayerNameMap* pnames_;   // player name map

  GsLevelSet* lset_;          // level info

  int max_players_;           // maximum number of players
  int num_logins_;			  // total # of logins so far
  bool closed_;				  // closed to further logins?
  int num_redirects_;		  // # of players redirected
  bool run_forever_;		  // don't exit based on # of logins
  GsPlayerSet* pset_;         // players in game
  GsPlayerThreadSet* ptset_;  // "spare" player threads

  LmMesgBufPool* mpool_;      // global message buffers
  LmConnectionSet* cset_;     // network connections

  int portnum_;               // port number to listen at
  int next_port_;			  // port # of next gamed to send players to after closing
  LmSocket* tsock_;           // TCP listening socket
  LmSocket* usock_;           // UDP listening socket

  GsInputDispatch* inp_;      // input message dispatcher
  GsOutputDispatch* outp_;    // output message dispatcher

  LmLogFile* log_;            // global log file
  LmLogFile* slog_;           // security log

  LmThreadPool* tpool_;       // server threads

  LmGuildDBC* gdbc_;          // guild database connection
  LmItemDBC* idbc_;           // item database connection
  LmPlayerDBC* pdbc_;         // player database connection
  LmBillingDBC* bdbc_;         // player database connection

};

#ifdef USE_INLINE
#include "GsMain.i"
#endif

// macro to make security logging easier (usage: SECLOG(3, "level 3 warning %d", foo); )
#define SECLOG main_->SecurityLog()->Security

// debug log definition
#ifdef USE_DEBUG
extern LmLogFile DebugLog;
#endif

#endif /* INCLUDED_GsMain */

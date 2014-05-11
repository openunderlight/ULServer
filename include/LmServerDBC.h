// LmServerDBC.h  -*- C++ -*-
// $Id: LmServerDBC.h,v 1.9 1998-02-20 20:25:45-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item database connection class

#ifndef INCLUDED_LmServerDBC
#define INCLUDED_LmServerDBC

#include <stdio.h>
#include "LyraDefs.h"
#include "mysql.h"
#include "PThMutex.h"
#include "SharedConstants.h"

// local types

// forward declarations

class LmRoomDB;
class LmLog;

// the class

class LmServerDBC {

public:

  enum {
    MAX_HOSTID = 32,
    HOSTNAME_MAX = 80,

    ST_UNKNOWN = 'U',

    ST_AGENT = 'a',   // agent server
	ST_DATABASE = 'D',      // database proxy server
    ST_GAME = 'G',    // "standard" game server
    ST_LEVEL = 'L',   // level server
    ST_MONITOR = 'm',  // monitor machine
	ST_CLEANUP = 'c', // gamed cleanup program

    // return codes
    MYSQL_ERROR = -1, // bad things, man
	FILE_ERROR = -2
  };

  typedef TCHAR ip_address_t[HOSTNAME_MAX];


public:

  LmServerDBC();
  ~LmServerDBC();

  int LoadPasswords(TCHAR* pw_filename);
  int Connect();
  void Disconnect();

  // load values for this level, prepare for retrieval
  int Load();

  // database selectors 


  int NumServers() const;
  const TCHAR* HostName(int index) const;
  const TCHAR* HostID(int index) const;
  unsigned long HostIPAddr(int index) const;
  int ServerType(int index) const;
  int Arg1(int index) const;
  int Arg2(int index) const;
  bool AllowPlayers(int index) const;

  // database passwords
  const TCHAR* ServerDBPassword() const;
  const TCHAR* ItemDBPassword() const;
  const TCHAR* PlayerDBPassword() const;
  const TCHAR* GuildDBPassword() const;
  const TCHAR* LevelDBPassword() const;
  const TCHAR* BillingDBPassword() const;

  const TCHAR* ServerDBUsername() const;
  const TCHAR* ItemDBUsername() const;
  const TCHAR* PlayerDBUsername() const;
  const TCHAR* GuildDBUsername() const;
  const TCHAR* LevelDBUsername() const;
  const TCHAR* BillingDBUsername() const;

  
  // database host
  const TCHAR* DatabaseHost() const;
  const TCHAR* DatabaseAdminEmail() const;
  const TCHAR* DatabaseReturnEmail() const;
  int DatabasePort() const;

#if 0 // deprecated BMP 10/03 - round robin only
  const TCHAR* ClosingFilePath() const;
  int WriteNewClosingFile(unsigned long ip_address[Lyra::MAX_CLOSING_GAMEDS], 
						  unsigned short port[Lyra::MAX_CLOSING_GAMEDS]) const;
#endif
  unsigned long DatabaseServerIP() const;
  int DatabaseServerPort() const;

  unsigned long LevelServerIP(lyra_id_t levelid) const;
  int LevelServerPort(lyra_id_t levelid) const;
  int ChooseServer(int serv_type, const TCHAR* hostid) const;
  bool IsServerIP(unsigned long ip_addr) const;
  int GetServerIPs(int serv_type, TCHAR* hostID, ip_address_t* ip_addresses);

  long LastSQLCode() const;
  void SetLog(LmLog* log);

  long LastCallTime() const;
  long TotalCallTime() const;
  long TotalSQLTime() const;

  void Dump(FILE* f, int indent = 0) const;

  static const TCHAR* ProgramName(int servertype);

protected:

  LmLog* Log() const;

  int room_index(lyra_id_t roomid) const;

private:

  // not implemented
  LmServerDBC(const LmServerDBC&);
  //operator=(const LmServerDBC&);

  int FindLevelServer(lyra_id_t levelid) const;
  int FindDatabaseServer() const;

  // mysql object
  MYSQL mysql_;

  // passwords
  TCHAR sdb_passwd_[40];
  TCHAR idb_passwd_[40];
  TCHAR pdb_passwd_[40];
  TCHAR gdb_passwd_[40];
  TCHAR ldb_passwd_[40];
  TCHAR bdb_passwd_[40];

  
  TCHAR db_host_[HOSTNAME_MAX]; // contains the address for the db server
  int db_port_; // contains the IP port for the db server
  TCHAR db_admin_email_[64]; // contains the address emailed for db problems
  TCHAR db_return_email_[64]; // contains the return address for email
							// sent by the Underlight servers
  TCHAR closing_file_path_[256]; // contains the file path to the gamed pointer			
  TCHAR lock_file_path_[256]; // lock file used to access above
			// file downloaded by clients that tells them where the gamed is

  // server info
  struct si_t {
    TCHAR hostname[HOSTNAME_MAX];
    TCHAR hostid[MAX_HOSTID];
    unsigned long host_addr;
    int servertype;
    int arg1;
    int arg2;
	bool allow_players;
    si_t();
  };

  int num_servers_;
  si_t* servers_;

  bool connected_;
  PThMutex lock_;
  long last_sql_code_;
  LmLog* log_;

  TCHAR username_[20]; 
  TCHAR password_[20];   
  TCHAR dbname_[20];

  int num_calls_;          // total number of calls to class
  unsigned long last_ms_;  // time of last call (milliseconds)
  unsigned long num_ms_;   // total time of all calls
  unsigned long sql_ms_;   // total time spent in "EXEC SQL" blocks

};


#ifdef USE_INLINE
#include "LmServerDBC.i"
#endif /* USE_INLINE */

#endif /* INCLUDED_LmServerDBC */

// LmServerDBC.pc  -*- C++ -*-
// $Id: LmServerDBC.pc,v 1.17 1998-05-01 17:04:49-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#include <string.h>
#include <stdlib.h>

#ifndef WIN32
#include <unistd.h>
#include <fcntl.h>
#endif

#include "../../libsrc/util/LmLocker.h"
#include "../../libsrc/util/LmLog.h"
#include "../../libsrc/util/LmTimer.h"
#include "../../libsrc/util/LmFuncTimer.h"
#include "../../libsrc/util/LmRand.h"
#include "../../libsrc/db/LmRoomDB.h"
#include "../../libsrc/net/LmSockAddrInet.h"
#include "../../libsrc/dbi_mysql/LmServerDBC.h"

#ifndef USE_INLINE
#include "LmServerDBC.i"
#endif /* !USE_INLINE */

extern int errno;

inline unsigned int ATOI(char* value)
{
  if (!value)
    return 0;
  else
    return ((unsigned int)(atof(value)));
}

// some log macros (from LmThread.h)
#define LOG_Log       if (Log()) Log()->Log
#define LOG_Warning   if (Log()) Log()->Warning
#define LOG_Debug     if (Log()) Log()->Debug
#define LOG_Error     if (Log()) Log()->Error
#define LOG_Security  if (Log()) Log()->Security

//#define LOG_Log       printf
//#define LOG_Warning   printf
//#define LOG_Debug     printf
//#define LOG_Error     printf
//#define LOG_Security  printf


////
// si(_t Constructor
////

LmServerDBC::si_t::si_t()
  : host_addr(0),
    servertype('U'),
    arg1(0),
    arg2(0),
	allow_players(true)
{
 _tcscpy(hostname, _T("unknown"));
 _tcscpy(hostid, _T("0"));
}



////
// Constructor
////

LmServerDBC::LmServerDBC()
  : connected_(false),
    log_(0),
    num_calls_(0),
    last_sql_code_(0),
    num_ms_(0),
    sql_ms_(0),
    last_ms_(0)
{
  lock_.Init();

 _tcscpy(sdb_passwd_, _T("unknown"));
 _tcscpy(idb_passwd_, _T("unknown"));
 _tcscpy(pdb_passwd_, _T("unknown"));
 _tcscpy(gdb_passwd_, _T("unknown"));
 _tcscpy(ldb_passwd_, _T("unknown"));  
 _tcscpy(bdb_passwd_, _T("unknown"));  
  // default to local host
 _tcscpy(db_host_, _T("localhost")); 

}

////
// Destructor
////

LmServerDBC::~LmServerDBC()
{
  Disconnect();
  LmDELETEARRAY(servers_);
}


////
// Dump
////

void LmServerDBC::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmServerDBC[%p,%d]: servers=%d>\n"), this, sizeof(LmServerDBC), NumServers());
  INDENT(indent + 1, f);
 _ftprintf(f, _T("passwords: s='%s', i='%s', p='%s', g='%s'\n"),
	  ServerDBPassword(), ItemDBPassword(), PlayerDBPassword(), GuildDBPassword());
  for (int i = 0; i < NumServers(); ++i) {
    INDENT(indent + 1, f);
   _ftprintf(f, _T("hostname=%s hostid=%s ip=%s type=%c (%s) arg1=%d arg2=%d allow_players=%d\n"),
	    HostName(i), HostID(i), LmSockAddrInet::AddressString(HostIPAddr(i)), ServerType(i),
	    ProgramName(ServerType(i)), Arg1(i), Arg2(i), AllowPlayers(i));
  }
}


////
// LoadPasswords
////

// the server database bootstraps the others, so it must load 
// the passwords from a text file

int LmServerDBC::LoadPasswords(TCHAR* pw_filename)
{
  FILE* fh =_tfopen(pw_filename, _T("r"));
  if (!fh)
    {
      LOG_Error(_T("Could not open db password file %s"), pw_filename);
      return MYSQL_ERROR;
    }

  TCHAR buffer[64], user[40], pw[40], db[40];
  int port;

  // first ensure that the hostname is on the first line of
  // the file in the form DBHOST <hostname>

  if ((_ftscanf(fh, _T("%s %s"), db, buffer) != EOF) &&
      (!_tcscmp(db, _T("DBHOST")))) {
   _tcscpy(db_host_, buffer);
  } else
    {
      LOG_Error(_T("Could not read database host from db password file %s"), pw_filename);
      return MYSQL_ERROR;
    }

  // next ensure that the port is on the first line of
  // the file in the form DBPORT <port>

  if ((_ftscanf(fh, _T("%s %d"), db, &port) != EOF) &&
      (!_tcscmp(db, _T("DBPORT")))) {
    db_port_ = port;
  } else
    {
      LOG_Error(_T("Could not read database port from db password file %s"), pw_filename);
      return MYSQL_ERROR;
    }

  // same for admin email
  if ((_ftscanf(fh, _T("%s %s"), db, buffer) != EOF) &&
      (!_tcscmp(db, _T("DBADMIN")))) {
   _tcscpy(db_admin_email_, buffer);
  } else
    {
      LOG_Error(_T("Could not read database administrative email address from db password file %s"), pw_filename);
      return MYSQL_ERROR;
    }

  // same for return email
  if ((_ftscanf(fh, _T("%s %s"), db, buffer) != EOF) &&
      (!_tcscmp(db, _T("DBRETURNEMAIL")))) {
   _tcscpy(db_admin_email_, buffer);
  } else
    {
      LOG_Error(_T("Could not read database return email address from db password file %s"), pw_filename);
      return MYSQL_ERROR;
    }
	
  TCHAR temp[64];

  // look for db key and salt, but don't store
  if ((_ftscanf(fh, _T("%s %s %s"), db, temp, buffer) != EOF) &&
      (!_tcscmp(db, _T("DBKEY")))) {
		// do nothing
  } else
    {
      LOG_Error(_T("Could not read database key from db password file %s"), pw_filename);
      return MYSQL_ERROR;
    }

  if ((_ftscanf(fh, _T("%s %s"), db, buffer) != EOF) &&
      (!_tcscmp(db, _T("DBSALT")))) {
	  // do nothing
  } else
    {
      LOG_Error(_T("Could not read database salt from db password file %s"), pw_filename);
      return MYSQL_ERROR;
    }


#ifdef Ul3D
  while (_ftscanf(fh, _T("%s %s %s"), db, user, pw) != EOF)
    if (!_tcscmp(db, _T("ul3d_player")))
		_tcscpy(pdb_passwd_, pw);
	else if (!_tcscmp(db, _T("ul3d_guild")))
		_tcscpy(gdb_passwd_, pw);
    else if (!_tcscmp(db, _T("ul3d_level")))
		_tcscpy(ldb_passwd_, pw);
    else if (!_tcscmp(db, _T("ul3d_server")))
		_tcscpy(sdb_passwd_, pw);
    else if (!_tcscmp(db, _T("ul3d_item")))
		_tcscpy(idb_passwd_, pw);
    else if (!_tcscmp(db, _T("ul3d_billing")))
		_tcscpy(bdb_passwd_, pw);
  // copy to member variables
 _tcscpy(username_, _T("ul3d_server"));
 _tcscpy(dbname_, _T("ul3d_server"));

#else

  while (_ftscanf(fh, _T("%s %s %s"), db, user, pw) != EOF)
    if (!_tcscmp(db, _T("ul_player")))
		_tcscpy(pdb_passwd_, pw);
	else if (!_tcscmp(db, _T("ul_guild")))
		_tcscpy(gdb_passwd_, pw);
    else if (!_tcscmp(db, _T("ul_level")))
		_tcscpy(ldb_passwd_, pw);
    else if (!_tcscmp(db, _T("ul_server")))
		_tcscpy(sdb_passwd_, pw);
    else if (!_tcscmp(db, _T("ul_item")))
		_tcscpy(idb_passwd_, pw);
    else if (!_tcscmp(db, _T("ul_billing")))
		_tcscpy(bdb_passwd_, pw);


  // copy to member variables
 _tcscpy(username_, _T("ul_server"));
 _tcscpy(dbname_, _T("ul_server"));
#endif

  fclose(fh);

 _tcscpy(password_, sdb_passwd_);

  return 0;
}



////
// Connect
////

int LmServerDBC::Connect()
{
 
  DEFMETHOD(LmServerDBC, Connect);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  if (connected_) {
    // don't connect twice (disconnect, perhaps?)
    return 0; // fake successful result
  }

  ////timer.Start();

  if (!mysql_init(&mysql_))
    {
      LOG_Error(_T("%s: MYSQL ul_server init error %s\n"), method, mysql_error(&mysql_));
      return MYSQL_ERROR;
    }

  if (!mysql_real_connect(&mysql_, db_host_, username_, password_, dbname_, db_port_, _T("/tmp/mysql.sock"), 0))
    {
      LOG_Error(_T("%s: MYSQL connect error %s\n"), method, mysql_error(&mysql_));
      return MYSQL_ERROR;
    }

  ////timer.Stop();

  connected_ = true;
  return 0;
}

////
// Disconnect
////

void LmServerDBC::Disconnect()
{
 
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  if (!connected_) {
    return;
  }

  ////timer.Start();
  mysql_close(&mysql_);
  ////timer.Stop();

  connected_ = false;
}

int LmServerDBC::Load()
{
  DEFMETHOD(LmServerDBC, Load);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  TCHAR query[1024];
  MYSQL_RES* res;
  MYSQL_ROW row;

  int i;

 _stprintf(query, _T("SELECT host_name, host_id, server_type, arg1, arg2, allow_players FROM server"));

  ////timer.Start();
  int error = mysql_query(&mysql_, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("Could not get server info; mysql error %s"), mysql_error(&mysql_));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&mysql_);

  num_servers_ = (int)mysql_num_rows(res);
  
  servers_ = LmNEW(si_t[num_servers_]);

  for (i=0; i<num_servers_; i++) {
    row = mysql_fetch_row(res);
   _tcscpy(servers_[i].hostname, row[0]);
   _tcscpy(servers_[i].hostid, row[1]);
    //    tmp = row[2][0];
    servers_[i].servertype = row[2][0];
    servers_[i].arg1 = ATOI(row[3]);
    servers_[i].arg2 = ATOI(row[4]);
    servers_[i].host_addr = inet_addr(servers_[i].hostname);
	if (0 == _tcscmp(row[5], _T("NO")))
		servers_[i].allow_players = false;
	else
		servers_[i].allow_players = true;
  }
  
  mysql_free_result(res);

  // BMP deprecated 10/03 for Ul3D; round robin only

#if 0
   _stprintf(query, _T("SELECT file_name FROM login_file LIMIT 1"));

  ////timer.Start();
  error = mysql_query(&mysql_, query);
  ////timer.Stop();

  if (error)
    {
      //LOG_Error(_T("Could not get gamed pointer file path info; mysql error %s"), mysql_error(&mysql_));
      LOG_Error(_T("Could not get gamed closing file path info; mysql error %s"), mysql_error(&mysql_));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&mysql_);

  row = mysql_fetch_row(res);
  _tcscpy(closing_file_path_, row[0]);
  _tcscpy(lock_file_path_, row[0]);
  _tcscat(lock_file_path_, _T("-lock"));

  mysql_free_result(res);

#endif

  // done
  return 0;
}


////
// FindDatabaseServer
////

int LmServerDBC::FindDatabaseServer() const
{
  // linear search for database server
  for (int i = 0; i < NumServers(); ++i) {
    if ((ServerType(i) == ST_DATABASE)) {
      return i;
    }
  }
  return -1; // not found
}


////
// FindLevelServer
////

int LmServerDBC::FindLevelServer(lyra_id_t levelid) const
{
  // linear search for level server with given levelid
  for (int i = 0; i < NumServers(); ++i) {
    if ((ServerType(i) == ST_LEVEL) && (Arg2(i) == levelid)) {
      return i;
    }
  }
  return -1; // not found
}

////
// ChooseServer - return a randomly selected server's port on the given host,
//   for the given server type, or 0 if none found
////

int LmServerDBC::ChooseServer(int serv_type, const TCHAR* hostid) const
{
  int* ports = LmNEW(int[NumServers()]);
  int num_ports = 0;
  int retval = 0;
  // search for game servers on host
  for (int i = 0; i < NumServers(); ++i) {
    if ((ServerType(i) == serv_type) && (!_tcscmp(hostid, HostID(i)) == 0)) {
      ports[num_ports] = Arg1(i);
      num_ports++;
    }
  }
  // any found?
  if (num_ports > 0) {
    // choose one randomly
    retval = LmRand::ChooseOne(ports, num_ports);
  }
  LmDELETEARRAY(ports);
  return retval;
}

int LmServerDBC::GetServerIPs(int serv_type, TCHAR* hostID, ip_address_t* ip_addresses)
{
	int num_ips = 0;
	bool duplicate; 

	for (int i = 0; i < NumServers(); ++i) {
		if ((ServerType(i) == serv_type) && (_tcscmp(hostID, HostID(i)) == 0)) {
			// if this IP is not already in the list, add it
			duplicate = false;
			for (int j = 0; j < num_ips; j++) {
				if (_tcscmp(ip_addresses[j], HostName(i)) == 0) {
					duplicate = true;
					break;
				}
			}
			if (!duplicate) {
				_tcscpy(ip_addresses[num_ips], HostName(i));
				num_ips++;
			}
		}
	}
	return num_ips;
}


////
// IsServerIP - return true if ip address is one of any kind of server
////

bool LmServerDBC::IsServerIP(unsigned long ip_addr) const
{
  // linear search for server with given ip
  for (int i = 0; i < NumServers(); ++i) {
    if (HostIPAddr(i) == ip_addr) {
      return true;
    }
  }
  return false; // not found
}

////
// ProgramName - return executable name for given server type
////

const TCHAR* LmServerDBC::ProgramName(int servertype)
{
  switch (servertype) {
  // servers that actually have executables
  case ST_GAME:
    return _T("gamed");
  case ST_DATABASE:
    return _T("dbd");
  case ST_LEVEL:
    return _T("leveld");
  // server types that don't have executables
  case ST_MONITOR:
    return _T("[monitor]");
  case ST_AGENT:
    return _T("[agent]");
  case ST_UNKNOWN:
  default: // not found
    return _T("[unknown]");
  }
  return 0;
}

#if 0 // deprecated BMP 10/03 - round robin only


////
// ClosingFilePath - return file path to gamed closing file
// used to be used to get pointer file (for http download)
////


const TCHAR* LmServerDBC::ClosingFilePath() const 
{
	return closing_file_path_;
}

////
// WriteNewClosingFile - write a new gamed pointer file (for http download)
////

// NOTE: this function has been coopted for writing the gamed shared closing
// file in round-robin login mode; it used to be called WriteNewLoginFile
int LmServerDBC::WriteNewClosingFile(
			 unsigned long ip_address[Lyra::MAX_CLOSING_GAMEDS],
			 unsigned short port[Lyra::MAX_CLOSING_GAMEDS]) const
{
	int retval = 0;

#ifndef WIN32
	int lockfd = _topen(lock_file_path_, O_WRONLY);
	flock locker;
	locker.l_type = F_WRLCK; 
	fcntl(lockfd, F_SETLKW, &locker);
#endif
	FILE* loginf =_tfopen(closing_file_path_, "w+");

	if (!loginf) {
		retval = FILE_ERROR;
	} else {
		for (int i = 0; i<Lyra::MAX_CLOSING_GAMEDS; i++)
			_ftprintf(loginf, "%u %u\n", ip_address[i], port[i]);
		fclose(loginf);
	}
#ifndef WIN32
	locker.l_type = F_UNLCK; 
	fcntl(lockfd, F_SETLKW, &locker);
	close(lockfd);
#endif

	return retval;
}

#endif
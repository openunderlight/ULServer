// get_server_status.cpp  -*- C++ -*-
// $Id: get_server_status.cpp,v 1.6 1998-02-03 23:38:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// program which attempts to make a connection to active Lyra servers, and reports
// their status (contents of status message)

#include <time.h>
#include <stdio.h>
#include <unistd.h>

int _tmain(int argc, TCHAR** argv)
{
  unsigned int hostid = gethostid();
  /*  if (!hostid) {
   _tprintf(_T("Host ID = 0; attempting to set - please re-run if not superuser!\n"));
    time_t t;
    time(&t);
    srand(t);
    hostid = rand();
    sethostid(hostid);
    }*/
  hostid = 2381155968;
 _tprintf(_T("%u\n"), hostid);
  exit(0);
}

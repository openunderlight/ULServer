// Main.cpp  -*- C++ -*-
// $Id: Main.cpp,v 1.8 1997-10-14 21:40:08-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// _tmain() for master super-server

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#include "LmUtil.h"
#include "MsMain.h"
#include "LmLogFile.h"
#include "LmNew.h"

////
// _tmain()
////

int _tmain(int argc, TCHAR** argv)
{

  // check command-line arguments
  if ((argc != 2) && (argc != 3)) {
   _tprintf(_T("Usage: %s root_dir <debugging>\n"), argv[0]);
    exit(1);
    }

  TCHAR* root_dir = argv[1];
  bool debugging = false;
  if (argv[2])
    debugging = true;


  if (!debugging) { // don't spawn new process if debugging
    LmUtil::DaemonStart();
  }

  LmUtil::BlockAllSignals();
  LmUtil::NullStdDescriptors();

  _tsetlocale(LC_ALL, _T("C"));

  pth_init();

#ifdef USE_LMNEW
  // open new/delete log file
  LmLogFile log;
  log.Init(_T("masterd"), _T("new"));
  log.Open(_T("."));
  LmNew::SetLog(&log);
  LmNew::InstallNewHandler();
#endif /* USE_LMNEW */

  // loop until done
  bool restart = true;
  int retval = Lyra::EXIT_OK;

  do {
    // create main object, initialize
    MsMain* Main = LmNEW(MsMain());

    if (Main->Init(root_dir) < 0) {
      retval = Lyra::EXIT_INIT;
      restart = false;
    }
    else {
      Main->Go(); // main server loop
      restart = Main->Restart(); // determine if restarting
    }
    LmDELETE(Main);
  } while (restart);

#ifdef USE_LMNEW
  log.Close();
#endif /* USE_LMNEW */

  pth_kill();

  return 0;
}

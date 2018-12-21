
// Main.cpp  -*- C++ -*-
// $Id: Main.cpp,v 1.22 1997-11-07 16:48:35-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// _tmain() for level server

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#ifndef WIN32
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#else
#include <process.h>    /* _beginthread, _endthread */
#endif
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "LyraDefs.h"
#include "LmUtil.h"
#include "LmLog.h"
#include "LsMain.h"
#include "LmNew.h"
//#include "Quantify.h"


static LsMain* Main = 0;

////
// _tmain()
////


#ifndef WIN32 

int _tmain(int argc, TCHAR** argv)
{
  // check command-line arguments
  if (argc != 5) {
   _tprintf(_T("Usage: %s root_dir port_num level_num ip_address\n"), argv[0]);
    exit(Lyra::EXIT_ARGS);
  }
  TCHAR* root_dir = argv[1];
  int port_num = _ttoi(argv[2]);
  int level_num = _ttoi(argv[3]);
  // no longer needed (strip quotes from IP address)
  //TCHAR* tmp = new TCHAR[_tcslen(argv[4])-2];
 //_tcsnccpy(tmp, argv[4]+1,_tcslen(argv[4])-2);
  //TCHAR* ip_address = tmp;
  TCHAR* ip_address = argv[4];

  setlocale(LC_ALL, _T("C"));

  // Note: the level server is designed to be started from the lyra
  // master server, and so it does not fork itself off

  // prepare to start server 
  LmUtil::BlockAllSignals();
  LmUtil::CloseAllDescriptors();
  LmUtil::NullStdDescriptors();

  pth_init(); // initialize GNU pTh user threads library

#ifdef USE_LMNEW
  // open new/delete log file
  LmLog log;
  log.Init(_T("leveld"), _T("new"), level_num);
  log.Open(_T("."));
  LmNew::SetLog(&log);
  LmNew::InstallNewHandler();
#endif /* USE_LMNEW */

  // sometimes we want these, actually
  //_tfreopn("ls_out.txt", "w", stdout);
  //_tfreopn("ls_err.txt", "w", stderr);

  // create main object, initialize
  int retval = Lyra::EXIT_OK;
  Main = LmNEW(LsMain());
  if (Main->Init(root_dir, port_num, level_num) < 0) {
    retval = Lyra::EXIT_INIT;
  }
  else {
    //    LmUtil::UnBlockAllSignals();
    // initialization OK, run the server
    retval = Main->Go(ip_address);
  }
  LmDELETE(Main);

#ifdef USE_LMNEW
  log.Close();
#endif /* USE_LMNEW */

  pth_kill();
  // now do chdir so that profiling output is seperated for game/level servers
  _tchdir(root_dir);
  _tchdir(_T("./var/log/prof_ls"));

  return retval;
}

#else // WIN32

TCHAR szTitle[32] = _T("Leveld"); 
TCHAR szWindowClass[32] = _T("Leveld"); 
HINSTANCE g_hInst;
HWND g_hWnd;
static int wm_shutdown_servers;
static unsigned int port_num, level_num;
static TCHAR ip_address[32];
static TCHAR root_dir[_MAX_PATH];


LRESULT CALLBACK LeveldWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == wm_shutdown_servers)
	{
		DestroyWindow(hWnd); // got shutdown from masterd
	}
    else switch (message) 
	{
		case WM_DESTROY:
			Main->SetSIGTERM(true);
			Sleep(5000); // allow threads to exit gracefully
			WSACleanup();
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance; // Store instance handle in our global variable
	g_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	
	if (!g_hWnd)
		return FALSE;
	
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);
	
	return TRUE;
}

ATOM RegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	
	wcex.cbSize = sizeof(WNDCLASSEX); 
	
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)LeveldWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;
	
	return RegisterClassEx(&wcex);
}


void __cdecl LeveldHelper(void* param)
{
	// create main object, initialize

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	
	wVersionRequested = MAKEWORD( 2, 2 );
	
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return;
	}

	int retval = Lyra::EXIT_OK;
	Main = LmNEW(LsMain());
	int rc = 0;

#ifdef USE_LMNEW
  // open new/delete log file
  LmLog log;
  log.Init(_T("leveld"), _T("new"), level_num);
  log.Open(_T("."));
  LmNew::SetLog(&log);
  LmNew::InstallNewHandler();
#endif /* USE_LMNEW */


	if ((rc = Main->Init(root_dir, port_num, level_num)) < 0) {
		// DEBUGLOG(("main: could not init Main; rc=%d", rc));
		retval = Lyra::EXIT_INIT;
	}
	else {
		// initialization OK, run the server; when this returns we're done
		retval = Main->Go(ip_address);
	}
	LmDELETE(Main);
	
	// DEBUGLOG(("main: exiting"));
	// DebugLog.Close();
	
	exit(retval);
}


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	LPSTR argv = lpCmdLine;

	// check command-line arguments
	if ((!argv[0]) || (!argv[1]) || (!argv[2]) || (!argv[3])) {
		MessageBox(NULL, _T("Usage: Leveld RootDirectory ServerPort LevelNum IPAddress"), _T("Invalid Parameters"), MB_OK);
		exit(Lyra::EXIT_ARGS);
	}
	
	TCHAR buffer[256];
	_stscanf(argv, _T("%s %u %u %s"), root_dir, &port_num, &level_num, buffer);
	// strip quotes from IP address
_tcsnccpy(ip_address, buffer+1,_tcslen(buffer)-2);
	
	RegisterClass(hInstance);

	setlocale(LC_ALL, _T("C"));
	
	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		MessageBox(NULL, _T("Could not create Window for Leveld"), _T("Window Error"), MB_OK);
		exit(Lyra::EXIT_ARGS);
	}
	
	wm_shutdown_servers = RegisterWindowMessage(SERVER_SHUTDOWN_STRING);
	
	// open log file
	// DebugLog.Init("Leveld", "", 0, getpid());
	// DebugLog.Open("/tmp");
	// DEBUGLOG(("main: rootdir=%s serverport=%d maxplayers=%d", RootDirectory, ServerPort, MaxPlayers));
	
#ifdef USE_LMNEW
	LmNew::SetLog(&log);
	LmNew::InstallNewHandler();
#endif
	
	// spawn a new thread to run the servers; the main thread needs to
	// run the Windows loop to wait for the shutdown message
	unsigned long thread_id = _beginthread(::LeveldHelper, 0, 0);

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return msg.wParam;	

}


#endif


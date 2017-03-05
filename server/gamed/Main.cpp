
// Main.cpp  -*- C++ -*-
// $Id: Main.cpp,v 1.22 1998/02/12 23:33:31 jason Exp brent $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// _tmain() for game server

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#ifndef WIN32
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#else
#include <process.h>    /* _beginthread, _endthread */
#endif
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "LyraDefs.h"
#include "LmUtil.h"
#include "LmLogFile.h"
#include "LmNew.h"
#include "GsMain.h"
#include "LmDebug.h"

////
// local variables
////

// the main object
static GsMain* Main = 0;

// server params
static int ServerPort = 0;
static int NextServerPort = 0;
static int MaxPlayers = 0;

// LmLogFile DebugLog;

////
// main
////

#ifndef WIN32

static TCHAR* RootDirectory = 0;
static TCHAR* IPAddress = 0;
static TCHAR* NextIPAddress = 0;

int _tmain(int argc, TCHAR** argv)
{
	// check command-line arguments
	if (argc != 7) {
	_tprintf(_T("Usage: %s RootDirectory ServerPort MaxPlayers IPAddress NextIPAddress NextServerPort\n"), argv[0]);
		exit(Lyra::EXIT_ARGS);
	}
	RootDirectory = argv[1];
	ServerPort = _ttoi(argv[2]);
	MaxPlayers = _ttoi(argv[3]);
	// this no longer needed (strip quotes from IP address)
	//TCHAR* tmp = new TCHAR[_tcslen(argv[4])-2];
	//_tcsnccpy(tmp, argv[4]+1,_tcslen(argv[4])-2);
	//IPAddress = (TCHAR*)tmp;
	IPAddress = argv[4];
	NextIPAddress = argv[5];
	NextServerPort = _ttoi(argv[6]);

	setlocale(LC_ALL, _T("C"));
	
	// Note: the game server is designed to be started from the lyra
	// master server, and so it does not fork itself off
	
	LmUtil::BlockAllSignals();
	LmUtil::CloseAllDescriptors(); 
	LmUtil::NullStdDescriptors();
	
	pth_init();
	
	// open log file
	// DebugLog.Init("gamed", "", 0, getpid());
	// DebugLog.Open("/tmp");
	// DEBUGLOG(("main: rootdir=%s serverport=%d maxplayers=%d", RootDirectory, ServerPort, MaxPlayers));
	
#ifdef USE_LMNEW
	LmNew::SetLog(&log);
	LmNew::InstallNewHandler();
#endif
	
	
	// create main object, initialize
	int retval = Lyra::EXIT_OK;
	Main = LmNEW(GsMain());
	int rc = 0;
	if ((rc = Main->CreateSockets(ServerPort, ServerPort, IPAddress)) < 0) {
		// DEBUGLOG(("main: could not create sockets; rc=%d; error=%s", rc, strerror(errno)));
		retval = Lyra::EXIT_INIT;
	}
	else if ((rc = Main->Init(RootDirectory, MaxPlayers, NextIPAddress, NextServerPort)) < 0) {
		// DEBUGLOG(("main: could not init Main; rc=%d", rc));
		retval = Lyra::EXIT_INIT;
	}
	else {
		// initialization OK, run the server; when this returns we're done
		retval = Main->Go();
	}
	LmDELETE(Main);
	
	// DEBUGLOG(("main: exiting"));
	// DebugLog.Close();
	pth_kill();
	// now do chdir so that profiling output is seperated for game/level servers
	_tchdir(RootDirectory);
	//_tchdir("./var/log/prof_gs");
	
	exit(retval);
}

#else

static TCHAR RootDirectory[_MAX_PATH];
static TCHAR IPAddress[32];
static TCHAR NextIPAddress[32];
TCHAR szTitle[32] = _T("GameD"); 
TCHAR szWindowClass[32] = _T("GameD"); 
HINSTANCE g_hInst;
HWND g_hWnd;
static int wm_shutdown_servers;

LRESULT CALLBACK GamedWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
	wcex.lpfnWndProc	= (WNDPROC)GamedWndProc;
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


void __cdecl GamedHelper(void* param)
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
	Main = LmNEW(GsMain());
	int rc = 0;

	if ((rc = Main->CreateSockets(ServerPort, ServerPort, IPAddress)) < 0) {
		// DEBUGLOG(("main: could not create sockets; rc=%d; error=%s", rc, strerror(errno)));
		retval = Lyra::EXIT_INIT;
	}
	else if ((rc = Main->Init(RootDirectory, MaxPlayers, NextIPAddress, NextServerPort)) < 0) {
		// DEBUGLOG(("main: could not init Main; rc=%d", rc));
		retval = Lyra::EXIT_INIT;
	}
	else {
		// initialization OK, run the server; when this returns we're done
		retval = Main->Go();
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
		MessageBox(NULL, _T("Usage: gamed RootDirectory ServerPort MaxPlayers IPAddress"), _T("Invalid Parameters"), MB_OK);
		exit(Lyra::EXIT_ARGS);
	}
	
	TCHAR buffer[256];
	_stscanf(argv, _T("%s %d %d %s"), RootDirectory, &ServerPort, &MaxPlayers, buffer);
	// strip quotes from IP address
_tcsnccpy(IPAddress, buffer+1,_tcslen(buffer)-2);
	
	RegisterClass(hInstance);

	setlocale(LC_ALL, _T("C"));
	
	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		MessageBox(NULL, "Could not create Window for GameD", "Window Error", MB_OK);
		exit(Lyra::EXIT_ARGS);
	}
	
	wm_shutdown_servers = RegisterWindowMessage(SERVER_SHUTDOWN_STRING);
	
	// open log file
	// DebugLog.Init("gamed", "", 0, getpid());
	// DebugLog.Open("/tmp");
	// DEBUGLOG(("main: rootdir=%s serverport=%d maxplayers=%d", RootDirectory, ServerPort, MaxPlayers));
	
#ifdef USE_LMNEW
	LmNew::SetLog(&log);
	LmNew::InstallNewHandler();
#endif
	
	// spawn a new thread to run the servers; the main thread needs to
	// run the Windows loop to wait for the shutdown message
	unsigned long thread_id = _beginthread(::GamedHelper, 0, 0);

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

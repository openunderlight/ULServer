// LmDebug.h -*- C++ -*- 
// $Id: LmDebug.h,v 1.3 1997-10-22 19:21:59-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// Debug Level macros

#ifndef INCLUDED_LmDebug
#define INCLUDED_LmDebug

////
//// Macros
////

// no debugging
#ifndef DEBUGLEVEL
#define DBUG1(x)  /* nothing */
#define DBUG2(x)  /* nothing */
#define DBUG3(x)  /* nothing */
#define DBUG4(x)  /* nothing */
#define DBUG5(x)  /* nothing */
#endif

// debug level 1
#if DEBUGLEVEL == 1
#define DBUG1(x)  x
#define DBUG2(x)  /* nothing */
#define DBUG3(x)  /* nothing */
#define DBUG4(x)  /* nothing */
#define DBUG5(x)  /* nothing */
#endif

// debug level 2
#if DEBUGLEVEL == 2
#define DBUG1(x)  x
#define DBUG2(x)  x
#define DBUG3(x)  /* nothing */
#define DBUG4(x)  /* nothing */
#define DBUG5(x)  /* nothing */
#endif

// debug level 3
#if DEBUGLEVEL == 3
#define DBUG1(x)  x
#define DBUG2(x)  x
#define DBUG3(x)  x
#define DBUG4(x)  /* nothing */
#define DBUG5(x)  /* nothing */
#endif

// debug level 4
#if DEBUGLEVEL == 4
#define DBUG1(x)  x
#define DBUG2(x)  x
#define DBUG3(x)  x
#define DBUG4(x)  x
#define DBUG5(x)  /* nothing */
#endif

// debug level 5
#if DEBUGLEVEL == 5
#define DBUG1(x)  x
#define DBUG2(x)  x
#define DBUG3(x)  x
#define DBUG4(x)  x
#define DBUG5(x)  x
#endif

// if, for some reason, DEBUGLEVEL was something other than 1-5,
// macros are turned off
#ifndef DBUG1
#define DBUG1(x)  /* nothing */
#define DBUG2(x)  /* nothing */
#define DBUG3(x)  /* nothing */
#define DBUG4(x)  /* nothing */
#define DBUG5(x)  /* nothing */
#endif

// if USE_DEBUG is defined, then there is a global debug log
#ifdef USE_DEBUG
class LmLogFile;
extern LmLogFile DebugLog;
// DEBUGLOG macro usage: DEBUGLOG(("bar::baz: foo=%d\n", foo));
//   (note double parens)
#define DEBUGLOG(x) DebugLog.Debug x ; DebugLog.FlushLog()
#else
#define DEBUGLOG(x) /* nothing */
#endif

#endif /* INCLUDED_LmDebug */

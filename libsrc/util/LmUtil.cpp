// LmUtil.cpp  -*- C++ -*-
// $Id: LmUtil.cpp,v 1.13 1997-11-04 23:03:42-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// Utility Function Implementation

#ifdef __GNUC__
#pragma implementation "LmUtil.h"
#endif

#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#ifndef WIN32
#include <termios.h>
#include <time.h>
#include <sys/time.h>
#include <sys/param.h>
#include <unistd.h>
#include <sys/procfs.h>
#endif
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#include "LmUtil.h"
#include "../../libsrc/shared/LyraDefs.h"

////
// CloseAllDescriptors: close all open file descriptors
////

void LmUtil::CloseAllDescriptors()
{
#ifndef WIN32 // UNIX only
  // close any open files descriptors
  for (int fd = 0; fd < FOPEN_MAX; ++fd) {
    close(fd);
  }
#endif
  // errno probably got set to EBADF from a close, so reset it
  errno = 0;
}

////
// NullStdDescriptors: assumes all descriptors are closed; opens the 3 standard
//   descriptors (stdin, stdout, stderr) and redirects them to /dev/null
////

void LmUtil::NullStdDescriptors()
{
#ifndef WIN32 // UNIX only
  // all files have been closed, so the next three are fd's 0, 1, 2: stdin, stdout, stderr
  // open these to point to /dev/null in case anything tries to use them
  int fd = open(_T("/dev/null"), O_RDWR);
  dup(fd);
  dup(fd);
#endif
  // reset errno
  errno = 0;
}

////
// DaemonStart: detach a daemon process from login session context; return 0 if
//   successful, -1 otherwise.  (modified from APUE, Stevens)
//   Note: should be called by a program as the very first thing, and definitely
//   before creating any worker threads!
////

int LmUtil::DaemonStart()
{

#ifndef WIN32 // UNIX only

  // fork and let the parent exit.  This guarantees the first child is not a process group leader.

  pid_t childpid = fork();
  if (childpid < 0) {
    return -1;
  }
  else {
    if (childpid > 0) {
      // parent
      exit(0);
    }
  }

  // become session leader: Disassociate from controlling terminal and process group
  if (setsid() < 0) {
    return -1;
  }

  // close any open descriptors
  LmUtil::CloseAllDescriptors();

  // clear any inherited file mode creation mask
  umask(0);

#endif

  return 0;
}

////
// BlockAllSignals: mask out all signals for calling thread/process
////

void LmUtil::BlockAllSignals()
{
#ifndef WIN32 // UNIX only
  sigset_t ss;
  //  sigemptyset(&ss);
  sigfillset(&ss);
  //  sigprocmask(SIG_BLOCK, &ss, 0);
  pth_sigmask(SIG_SETMASK, &ss, 0);
#endif
}

////
// UnBlockAllSignals: unmask all signals for calling thread/process
////

void LmUtil::UnBlockAllSignals()
{
#ifndef WIN32 // UNIX only
  sigset_t ss;
  //  sigemptyset(&ss);
  sigfillset(&ss);
  pth_sigmask(SIG_UNBLOCK, &ss, 0);
#endif
}

////
// SendMail: send an email message to the target, with given subject and message
////

static const TCHAR* sendmail_exec = _T("/usr/lib/sendmail");

void LmUtil::SendMail(const TCHAR* source, const TCHAR* target, const TCHAR* subject, const TCHAR* msg_fmt, ...)
{
  // we do this by first creating a temp file with the headers/message, and then
  // running "sendmail -t" to send it.  simplicity itself...  who needs MAPI?  :)


#ifdef WIN32
	// NO EMAIL SUPPORT YET FOR WIN32 PORT
#else
  // make temp filename
  TCHAR tmpfname[FILENAME_MAX];
  //  if (tmpnam_r(tmpfname) == NULL) { // could not make temporary filename?
  if (mkstemp(tmpfname) == -1) { // could not make temporary filename?
   _stprintf(tmpfname, _T("/tmp/ul_%lu.mailtmp"), getpid()); // make our own
  }
  // open temp file
  FILE* outf =_tfopen(tmpfname, _T("w"));
  if (!outf) { // doh!
    return;
  }
  // write message header, and trailing newline
 _ftprintf(outf, _T("From: %s\n"), source);
 _ftprintf(outf, _T("To: %s\n"), target);
 _ftprintf(outf, _T("Subject: %s\n\n"), subject);
  // write message body (varargs)
  va_list args;
  va_start(args, msg_fmt);
 _vftprintf(outf, msg_fmt, args);
  va_end(args);
  // close
  fclose(outf);
  // assemble command to run
  TCHAR cmd[512];
 _stprintf(cmd, "%s -t < %s", sendmail_exec, tmpfname);
  // run it, ignoring result value
 _tsystem(cmd);
  // remove temp file
  _tunlink(tmpfname);
#endif
}

////
// GetCPULoad - return CPU load (as given by ps) -- very Solaris 2.6 dependent
////

double LmUtil::GetCPULoad(unsigned short* frac_value)
{
  // ye olde "don't know how to implement this function" return value
  return 0.0;

  /*
  // this is for Solaris 2.5 and hopefully later
  prpsinfo_t psinfo; // process info structure
  memset(&psinfo, 0, sizeof(psinfo));
  pid_t mypid = getpid();
  // open psinfo entry in /proc/$$
  TCHAR fname[FILENAME_MAX];
 _stprintf(fname, "/proc/%05lu", mypid);
  int procfd = open(fname, O_RDONLY, 0);
  if (procfd < 0) {
    return -1.0; // "invalid" value
  }
  // read it in
  if (ioctl(procfd, PIOCPSINFO, &psinfo) < 0) {
    close(procfd);
    return -2.0;
  }
  if (frac_value) {
    *frac_value = psinfo.pr_pctcpu;
  }
  // compute load (16-bit binary fraction, 1.0 == 0x8000, or so the manpage says, whatever that means)
  // double load = (double) psinfo.pr_pctcpu / (double) 0x8000;
  double load = (double) psinfo.pr_pctcpu / (double) FSCALE; // actually looks like divisor is 256.0
  // close up
  close(procfd);
  return load;

#if 0
  // this is for Solaris 2.6 only
  psinfo_t psinfo; // process info structure
  memset(&psinfo, 0, sizeof(psinfo));
  pid_t mypid = getpid();
  // open psinfo entry in /proc/$$
  TCHAR fname[FILENAME_MAX];
 _stprintf(fname, "/proc/%lu/psinfo", mypid);
  FILE* inf =_tfopen(fname, "r");
  if (!inf) {
    return -1.0; // "invalid" value
  }
  // read it in
  if (fread(&psinfo, sizeof(psinfo), 1, inf) == 0) {
    fclose(inf);
    return -1.0;
  }
  if (frac_value) {
    *frac_value = psinfo.pr_pctcpu;
  }
  // compute load (16-bit binary fraction, 1.0 == 0x8000, or so the manpage says, whatever that means)
  // double load = (double) psinfo.pr_pctcpu / (double) 0x8000;
  double load = (double) psinfo.pr_pctcpu / (double) FSCALE; // actually looks like divisor is 256.0
  // close up
  fclose(inf);
  return load;
#endif
  */
}

////
// InitProcTitle: initialize handling for SetProcTitle; returns new envp
//   (borrowed from sendmail v8)
////

static char** Argv = NULL;          // pointer to argument vector
static char* LastArgv = NULL;       // end of argv

char** LmUtil::InitProcTitle(int /* argc */, char** /* argv */, char** /* envp */)
{
  // maybe one of these days this will work...
#if 0
  int i;
  char** environ = 0;

  // Move the environment so setproctitle can use the space at the top of memory
  for (i = 0; envp[i] != NULL; i++) {
    continue;
  }
  environ = (char **) malloc(sizeof (char *) * (i + 1));
  for (i = 0; envp[i] != NULL; i++) {
    environ[i] =_tcsdup(envp[i]);
  }
  environ[i] = NULL;
  // Save start and extent of argv for setproctitle.
  Argv = argv;
  if (i > 0) {
    LastArgv = envp[i - 1] +_tcslen(envp[i - 1]);
  }
  else {
    LastArgv = argv[argc - 1] +_tcslen(argv[argc - 1]);
  }
  // return new environment
  return environ;
#endif
  return 0;
}

////
// SetProcTitle: printf-like function for setting the process title (as shown by ps)
//   (borrowed from sendmail v8)
////

void LmUtil::SetProcTitle(const char* /* fmt */, ...)
{
  // maybe one of these days this will work...
#if 0
  int i;
  static char buf[512];
  char* p = buf;
    
  // print the argument string
  va_list args;
  va_start(args, fmt);
 _vstprintf(buf, fmt, args);
  va_end(args);
  // get result string length
  i =_tcslen(buf);
  // truncate if necessary
  if (i > (LastArgv - Argv[0] - 2)) {
    i = LastArgv - Argv[0] - 2;
    buf[i] = '\0';
  }
  // copy into argv[0]
 _tcscpy(Argv[0], buf);
  // pad with spaces
  p = &Argv[0][i];
  while (p < LastArgv) {
    *p++ = ' ';
  }
  // zero out argv[1]
  Argv[1] = NULL;
#endif
}


////
// TimeSince: return time (seconds) since given time
////

time_t LmUtil::TimeSince(time_t t)
{
  return time(NULL) - t;
}

////
// TimeUntil: return time (seconds) until given time
////

time_t LmUtil::TimeUntil(time_t t)
{
  return t - time(NULL);
}

////
// DayNumber: return days since 1/1/70 for given time
////

int LmUtil::DayNumber(time_t t)
{
  return t / (60 * 60 * 24); // seconds in one day
}

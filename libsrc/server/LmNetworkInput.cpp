// LmNetworkInput.cpp  -*- C++ -*-
// $Id: LmNetworkInput.cpp,v 1.31 1998-02-12 15:33:05-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmNetworkInput.h"
#endif

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>


#include "LmNetworkInput.h"
#include "LyraDefs.h"
#include "LmSocket.h"
#include "LmConnection.h"
#include "LmConnectionSet.h"
#include "LmLog.h"
#include "LmMessageReader.h"
#include "SMsg_RecvMesg.h"
#include "LmSrvMesgBuf.h"
#include "LmMesgBufPool.h"
#include "LmRand.h"

#ifndef USE_INLINE
#include "LmNetworkInput.i"
#endif

DECLARE_TheFileName;

////
// Constructor
////

LmNetworkInput::LmNetworkInput(LmDispatch* dispatch, LmConnectionSet* cset, LmMesgBufPool* bpool, LmLog* log)
  : LmThread(bpool, log),
    dispatch_(dispatch),
    cset_(cset)
{
  DECLARE_TheLineNum;
  // empty
}

////
// Destructor
////

LmNetworkInput::~LmNetworkInput()
{
  DECLARE_TheLineNum;
  // close listening sockets
  socklist_t::iterator i;
  for (i = listen_.begin(); !(bool)(i == listen_.end()); ++i) {
    LmSocket* sock = *i;
    sock->Close();
    //sock->Shutdown();
  }
  // cancel and delete reader threads
  for (readerlist_t::iterator j = readers_.begin(); !(bool)(j == readers_.end()); ++j) {
    LmMessageReader* thr = *j;
    if (thr->IsRunning()) {
      thr->Cancel();
    }
    LmDELETE(thr);
  }
}

////
// AddListener
////

void LmNetworkInput::AddListener(LmSocket* sock)
{
  DECLARE_TheLineNum;
  listen_.push_back(sock);
}

////
// Run: thread's main
////

void LmNetworkInput::Run()
{
  DECLARE_TheLineNum;
  // modified LmThread mainloop
  SetDone(false);
  // block all signals
  //  LmUtil::BlockAllSignals();
  while (!Done()) {
    HandleNetworkInput();
    HandleAvailableMessages();
    ThreadYield();
  }
  DoneRunning();
}

////
// AddReader
////

void LmNetworkInput::AddReader(LmMessageReader* thr)
{
  DECLARE_TheLineNum;
  if (thr) {
    readers_.push_back(thr);
  }
}

////
// GetIdleReader
////

LmMessageReader* LmNetworkInput::GetIdleReader() const
{
  DECLARE_TheLineNum;
  for (readerlist_t::const_iterator i = readers_.begin(); !(bool)(i == readers_.end()); ++i) {
    // if thread isn't busy processing a message, and if it doesn't have any pending messages, return it
    LmMessageReader* thr = *i;
    if (!thr->Busy() && (thr->MessagesInQueue() == 0)) {
      return thr;
    }
  }
  // none found
  return 0;
}

////
// ChooseReader
////

LmMessageReader* LmNetworkInput::ChooseReader() const
{
  int num = LmRand::Generate(0, readers_.size() - 1);
  int j = 0;
  for (readerlist_t::const_iterator i = readers_.begin(); !(bool)(i == readers_.end()); ++i) {
    LmMessageReader* thr = *i;
    // if this is the one
    if (j == num) {
      return thr;
    }
    ++j;
  }
  // shouldn't be reached, but if it is return the head
  return *(readers_.begin());
}

////
// HandleNetworkInput
////

void LmNetworkInput::HandleNetworkInput()
{
  DEFMETHOD(LmNetworkInput, HandleNetworkInput);
  DECLARE_TheLineNum;
  // create fd_set
  fd_set fds;
  int maxfd = 0;
  FD_ZERO(&fds);
  // add listening sockets
  for (socklist_t::iterator i = listen_.begin(); !(bool)(i == listen_.end()); ++i) {
    FD_SET((*i)->Descriptor(), &fds);
    maxfd = MAX(maxfd, (*i)->Descriptor());
  }
  // add connections that aren't currently processing a message
  int maxcfd = cset_->FillSet(&fds);
  maxfd = MAX(maxfd, maxcfd);
  // call select() with a 1/2 second (500000 usec) timeout
  // TODO: use dummy descriptor to make this timeout longer
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 500000;
  int rc = select(maxfd + 1, &fds, NULL, NULL, &tv);
  // check error
  if (rc < 0) {
    if (!HandleError(ERR_SELECT)) {
      return;
    }
  }
  // check if any sockets are ready for reading
  if (rc == 0) {
    //TLOG_Debug(_T("%s: nothing available for reading\n"), method);
    return;
  }
  // check listening sockets
  for (socklist_t::iterator j = listen_.begin(); !(bool)(j == listen_.end()); ++j) {
    if (FD_ISSET((*j)->Descriptor(), &fds)) {
      int newfd = AcceptConnection(*j);
      if (newfd != -1) {
	// clear listening socket and new socket from active descriptors
	FD_CLR(newfd, &fds);
	FD_CLR((*j)->Descriptor(), &fds);
      }
    }
  }

  // check normal sockets
  for (int fd = 0; fd < (maxfd + 1); ++fd) {
    if (FD_ISSET(fd, &fds)) {
      LmConnection* conn = cset_->GetConnection(fd);
      if (!conn) {
	TLOG_Error(_T("%s: could not get connection for fd=%d"), method, fd);
	continue;
      }
	  //if (conn->IsClosed()) // don't use if they're closed
		//  continue;
      // check that connection isn't already being used
      if (conn->MessageBeingRead()) {
	TLOG_Warning(_T("%s: conn [%p] already active, but returned in fd_set"), method, conn);
	continue;
      }
      // check that connection doesn't already have an active reader
      if (has_reader(conn)) {
		TLOG_Warning(_T("%s: conn [%p] already has a reader"), method, conn);
		continue;

      }
      // read message from connection
      ReadMessage(conn);
    }
  }

}

////
// AcceptConnection
////

int LmNetworkInput::AcceptConnection(LmSocket* sock)
{
  DEFMETHOD(LmNetworkInput, AcceptConnection);
  DECLARE_TheLineNum;
  int retval = -1;
  // get new socket
  LmSocket newsock;
  if (sock->Accept(newsock) < 0) {
    return -1;
  }
  newsock.ComputeAddresses();
  //  TLOG_Debug(_T("%s: connection accepted from %s:%d"), method, newsock.PeerName().AddressString(), newsock.PeerName().Port());
  // check that this connection is allowed
  if (!AllowConnection(&newsock)) {
    TLOG_Warning(_T("%s: connection from %s:%d not allowed"), method,
		 newsock.PeerName().AddressString(), newsock.PeerName().Port());
    newsock.Close();
    return -1;
  }
  // add to connection set, initialize with socket
  LmConnection* conn = cset_->AllocateConnection(newsock);
  if (!conn) {
    TLOG_Error(_T("%s: could not allocate connection"), method);
    // close socket immediately
    newsock.Close();
  }
  else {
    retval = newsock.Descriptor();
  }
  //   TLOG_Debug(_T("%s: new conn [%p] (%d) added to set"), method, conn, retval);
  return retval;
}

////
// ReadMessage
////

void LmNetworkInput::ReadMessage(LmConnection* conn)
{
  DEFMETHOD(LmNetworkInput, ReadMessage);
  DECLARE_TheLineNum;
  // get an idle reader thread
  LmMessageReader* thr = GetIdleReader();
  // none found? check for _T("too many") before starting another
  if (!thr) {
    if (readers_.size() < MAX_READERS) {
      thr = StartReader();  // start a reader, and add it to the list
      AddReader(thr);
    }
    else {
	  TLOG_Warning(_T("%s: randomly choosing reader thread for conn [%p]"), method, conn);
      thr = ChooseReader(); // choose one randomly
    }
  }
  if (thr) {
    // indicate connection is busy having message read
    conn->StartMessageReading();
    // set reader thread's current connection
    thr->SetCurrentConnection(conn);
    // send RecvMesg message to it
    SMsg_RecvMesg msg;
    msg.Init(conn);
    LmSrvMesgBuf* mbuf = BufferPool()->AllocateBuffer(msg.MessageSize());
    mbuf->ReadMessage(msg);
    thr->PassMessage(mbuf, 0);
  }
  else {
    TLOG_Error(_T("%s: could not read message for conn [%p]"), method, conn);
  }
}

////
// AllowConnection - return true if connection from given socket is allowed; default implementation
//   returns true trivially
////

bool LmNetworkInput::AllowConnection(LmSocket* /* sock */)
{
  return true;
}

////
// HandleError: return true if processing can continue - default implementation
////

bool LmNetworkInput::HandleError(int errcode)
{
  DECLARE_TheLineNum;
  TLOG_Error(_T("LmNetworkInput::HandleError: error %d, strerror: %s"), errcode, strerror(errno));
  return false;
}

////
// Dump
////

void LmNetworkInput::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  INDENT(indent, f);
 _ftprintf(f, _T("<LmNetworkInput[%p,%d]: dispatch=[%p] cset=[%p]>\n"), this, sizeof(LmNetworkInput),
	  dispatch_, cset_);
  indent++;
  INDENT(indent, f);
 _ftprintf(f, _T("listeners (%d):\n"), listen_.size());
  for (socklist_t::const_iterator i = listen_.begin(); !(bool)(i == listen_.end()); ++i) {
    (*i)->Dump(f, indent + 1);
  }
  INDENT(indent, f);
 _ftprintf(f, _T("readers (%d):\n"), readers_.size());
  for (readerlist_t::const_iterator j = readers_.begin(); !(bool)(j == readers_.end()); ++j) {
    (*j)->Dump(f, indent + 1);
  }
  LmThread::Dump(f, indent);
}

////
// has_reader - return true if given connection has an active reader thread
////

bool LmNetworkInput::has_reader(LmConnection* conn) const
{
  for (readerlist_t::const_iterator i = readers_.begin(); !(bool)(i == readers_.end()); ++i) {
    LmMessageReader* thr = *i;
    if (thr->CurrentConnection() == conn) {
      return true;
    }
  }
  return false;
}

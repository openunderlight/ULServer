// GsMessageReader.cpp  -*- C++ -*-
// $Id: GsMessageReader.cpp,v 1.19 1998/02/12 23:33:31 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "GsMessageReader.h"
#endif

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "GsMessageReader.h"
#include "LyraDefs.h"
#include "LmLog.h"
#include "GMsg_Logout.h"
#include "SMsg_Logout.h"
#include "LmSrvMesgBuf.h"
#include "LmMesgBufPool.h"
#include "LmDispatch.h"
#include "LmConnection.h"
#include "GsMain.h"
#include "LyraMessage.h"
#include "GsUtil.h"
#include "GsInputDispatch.h"

#ifndef USE_INLINE
#include "GsMessageReader.i"
#endif

////
// Constructor
////

GsMessageReader::GsMessageReader(GsMain* gsmain, LmLog* log)
  : LmMessageReader(gsmain->InputDispatch(), gsmain->BufferPool(), log),
    main_(gsmain)
{
  // empty
}

////
// Destructor
////

GsMessageReader::~GsMessageReader()
{
  // empty
}

////
// Run
////

void GsMessageReader::Run()
{
  LmMessageReader::Run();
}

////
// HandleError
////

bool GsMessageReader::HandleError(LmConnection* conn, int errcode, int mtype, int msize)
{
  DEFMETHOD(GsMessageReader, HandleError);
  bool retval = false; // default: non-fatal error
  switch (errcode) {
  case LmMessageReader::ERR_NODATA:
    //    TLOG_Warning(_T("%s: no data available on conn [%p]?"), method, conn);
    // reset the connection's message read flag (handled in LmMessageReader)
    //conn->EndMessageReading();
    break;
  case LmMessageReader::ERR_DISPATCH:
  case LmMessageReader::ERR_HEADERPEEK:
  case LmMessageReader::ERR_RECEIVE:
  case LmMessageReader::ERR_MSGSIZE:
    TLOG_Error(_T("%s: conn=[%p] error=%c errstr(%d)='%s' mtype(%d)=%s msize=%d"), method, conn, errcode, errno, strerror(errno), mtype, LyraMessage::MessageName(mtype), msize);

    // don't log for EOF - it happens for each logout
  case LmMessageReader::ERR_EOF:
    // don't want to read anything else from this socket
	  if ((!conn->Socket().HasShutdown()) && (conn->ID() != LmConnection::DEAD_CONNECTION))  {
	//  if (!conn->Socket().HasShutdown()) {
      // do not shutdown & do fake logout more than once per connection;
	  // if conn ID is 1 or 0, we've already logged them out
      conn->Socket().Shutdown(LmSocket::SHUTDOWN_READ);
      // if we get an error while reading a message, we fake a logout
      // message of the appropriate type; the server will close the connection
      GsUtil::FakeLogout(main_, conn);
    }
    retval = true; // fatal error, don't read anything more from connection
    break;
  default:
    TLOG_Error(_T("%s: unknown error condition %d"), method, errcode);
    // TODO: close connection?
    break;
  }
  return retval;
}

////
// Dump
////

void GsMessageReader::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GsMessageReader[%p,%d]: >\n"), this, sizeof(GsMessageReader));
  LmMessageReader::Dump(f, indent + 1);
}

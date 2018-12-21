// LsMessageReader.cpp  -*- C++ -*-
// $Id: LsMessageReader.cpp,v 1.19 1998-02-12 15:33:40-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LsMessageReader.h"
#endif

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "LsMessageReader.h"
#include "LyraDefs.h"
#include "LmLog.h"
#include "RMsg_Logout.h"
#include "SMsg_Logout.h"
#include "LmSrvMesgBuf.h"
#include "LmMesgBufPool.h"
#include "LmDispatch.h"
#include "LmConnection.h"
#include "LsMain.h"
#include "LyraMessage.h"
#include "LsInputDispatch.h"

#ifndef USE_INLINE
#include "LsMessageReader.i"
#endif

////
// Constructor
////

LsMessageReader::LsMessageReader(LsMain* lsmain, LmLog* log)
  : LmMessageReader(lsmain->InputDispatch(), lsmain->BufferPool(), log),
    main_(lsmain)
{
  // empty
}

////
// Destructor
////

LsMessageReader::~LsMessageReader()
{
  // empty
}

////
// Run
////

void LsMessageReader::Run()
{
  LmMessageReader::Run();
}

////
// HandleError
////

bool LsMessageReader::HandleError(LmConnection* conn, int errcode, int mtype, int msize)
{
  DEFMETHOD(LsMessageReader, HandleError);
  bool retval = false; // default: non-fatal error
  switch (errcode) {
  case LmMessageReader::ERR_NODATA:
    Log()->Warning(_T("%s: no data available on conn [%p]?"), method, conn);
    // reset the connection's message read flag (LmMessageReader handles this)
    //conn->EndMessageReading();
    break;
  case LmMessageReader::ERR_DISPATCH:
  case LmMessageReader::ERR_HEADERPEEK:
  case LmMessageReader::ERR_RECEIVE:
  case LmMessageReader::ERR_MSGSIZE:
    Log()->Error(_T("%s: conn=[%p] error=%c errstr(%d)='%s' mtype(%d)=%s msize=%d"), method,
		 conn, errcode, errno, strerror(errno), mtype, LyraMessage::MessageName(mtype), msize);
    // don't log EOF, it happens every time
  case LmMessageReader::ERR_EOF:
  {
      // if we get an error while reading a message, we fake a logout
      // message of the appropriate type; the server will close the connection
      SMsg_Logout msg;
      LmSrvMesgBuf* mbuf = BufferPool()->AllocateBuffer(msg.MessageSize());
      mbuf->ReadMessage(msg);
      // dispatch
	  LmDispatch* pDispatch = Dispatch(); //dispatch_;
      if (pDispatch->DispatchMessage(mbuf, conn) < 0) {
	  
	BufferPool()->ReturnBuffer(mbuf);
	Log()->Error(_T("%s: could not dispatch fake logout message from conn [%p]"), method, conn);
      }
      retval = true; // fatal error, no more reading
  }
  break;
  default:
    Log()->Error(_T("%s: unknown error condition %d"), method, errcode);
    // TODO: close connection?
    break;
  }
  return retval;
}

////
// Dump
////

void LsMessageReader::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LsMessageReader[%p,%d]: >\n"), this, sizeof(LsMessageReader));
  LmMessageReader::Dump(f, indent + 1);
}

// GMsg_LocateAvatar.cpp  -*- C++ -*-
// $Id: GMsg_LocateAvatar.cpp,v 1.4 1997-12-05 14:44:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_LocateAvatar.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock2.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "GMsg_LocateAvatar.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_LocateAvatar.i"
#endif

////
// constructor
////

GMsg_LocateAvatar::GMsg_LocateAvatar()
  : LmMesg(GMsg::LOCATEAVATAR, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0);
}

////
// destructor
////

GMsg_LocateAvatar::~GMsg_LocateAvatar()
{
  // empty
}

////
// Init
////

void GMsg_LocateAvatar::Init(int num_players)
{
  SetNumPlayers(num_players);
}

void GMsg_LocateAvatar::Init(const TCHAR* playername)
{
  SetNumPlayers(1);
  SetPlayerName(0, playername);
}

////
// hton
////

void GMsg_LocateAvatar::hton()
{
  // not converted: names
}

////
// ntoh
////

void GMsg_LocateAvatar::ntoh()
{
  calc_players();
  // not converted: names
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_LocateAvatar::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_LocateAvatar[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("numplayers=%d>\n"), NumPlayers());
    INDENT(indent + 1, f);
   _ftprintf(f, _T("names: "));
    for (int i = 0; i < NumPlayers(); ++i) {
     _ftprintf(f, _T("'%s' "), PlayerName(i));
    }
   _ftprintf(f, _T("\n"));
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetNumPlayers
////

void GMsg_LocateAvatar::SetNumPlayers(int num_players)
{
  // avoid bad range
  if ((num_players < 0) || (num_players > MAX_PLAYERS)) {
    num_players_ = 0;
  }
  else {
    num_players_ = num_players;
  }
  calc_size();
}

////
// SetPlayerName
////

void GMsg_LocateAvatar::SetPlayerName(int player_num, const TCHAR* playername)
{
 _tcsnccpy(data_.players[player_num], playername, sizeof(pname_t));
  TRUNC(data_.players[player_num], sizeof(pname_t));
}

////
// calc_players - calculate num_players_ based on message size
////

void GMsg_LocateAvatar::calc_players()
{
  int msgsize = MessageSize();
  // determine portion of message size related to fixed fields
  int fixed_size = sizeof(data_t) - sizeof(data_.players);
  // subtract this from the overall message size; result is the
  // size attributable to the variable-sized field
  msgsize -= fixed_size;
  // calculate number of changes
  SetNumPlayers(msgsize / sizeof(pname_t));
}

////
// calc_size - calculate message size based on number of player names
////

void GMsg_LocateAvatar::calc_size()
{
  // initial size: overall size minus variable-length field
  int size = sizeof(data_t) - sizeof(data_.players);
  // add space for players
  size += NumPlayers() * sizeof(pname_t);
  SetMessageSize(size);
}

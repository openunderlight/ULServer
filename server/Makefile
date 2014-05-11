# Makefile  -*- Makefile -*-
# $Id: Makefile,v 1.14 1998-01-27 10:18:29-08 jason Exp $
# Copyright 1996-1997 Lyra LLC, All rights reserved.
#
# Lyra Server Makefile

# subdirectories
SUBDIRS := gamed leveld masterd 

# server executables (with paths)
EXECS := gamed/gamed_exec leveld/leveld_exec masterd/masterd_exec 

# standard targets
all: all-subdirs
checkin: checkin-local checkin-subdirs
depend: depend-subdirs
install: install-subdirs
clean: clean-subdirs
tags: tags-subdirs

# makefile-specific
link: remove-servers all

remove-servers:
	/bin/rm -f $(EXECS)

copy:
	/bin/cp $(EXECS) /opt/lyra/ul/prod/sbin

# get global rules
include ../Rules.make

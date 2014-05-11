# Makefile  -*- Makefile -*-
# $Id: Makefile,v 1.13 1997-10-22 19:21:00-07 jason Exp $
# Copyright 1996-1997 Lyra LLC, All rights reserved.
#
# Lyra Top-Level Makefile

# "source" files
SOURCES := Rules.make.gnu Rules.make.sun ProC.conf Makefile

# subdirectories
SUBDIRS := libsrc server util

# standard targets
all: all-subdirs
checkin: checkin-local checkin-subdirs
depend: depend-subdirs
install: install-subdirs
clean: clean-subdirs
tags: tags-subdirs

# remove all "generated" files
nuke: clean
	$(RM) -r -f include/* shared/* lib/*

# generate global "TAGS" file
global-tags:
	$(RM) -f $(TAGFILE)
	$(FIND) $(TOPSRCDIR)/{libsrc,server} \
		-name '*.cpp' -print -o -name '*.i' -print -o -name '*.h' -print \
	| $(GREP) -v '\.old' \
	| $(XARGS) $(ETAGS) -a -C -o $(TAGFILE)

# get global rules
include Rules.make


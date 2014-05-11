# Makefile  -*- Makefile -*-
# Copyright 1996-1997 Lyra LLC, All rights reserved.
#
# Lyra C++ Library Top-Level Makefile

# target library
LIB_TARGET := liblyra.a

# subdirs
SUBDIRS := db gdbm dbi_mysql dbi_gdbm gmsg net private pth rmsg server shared smsg util

# all object files
OBJS := $(patsubst %.cpp,%.o,$(wildcard */*.cpp)) 

# targets that this Makefile creates
TARGETS := $(LIB_TARGET) 

# standard targets

all: all-subdirs $(LIB_TARGET)
checkin: checkin-subdirs checkin-local
clean: clean-subdirs clean-local
depend: depend-subdirs
install: install-subdirs install-lib
tags: tags-subdirs

# makefile-specific targets

includes:
	$(MAKE) -C include all 

install-lib: $(LIB_TARGET)
	$(INSTALL) -m 0644 $(LIB_TARGET) $(TOPSRCDIR)/lib
	$(RANLIB) $(TOPSRCDIR)/lib/$(LIB_TARGET)

# get global rules
include ../Rules.make






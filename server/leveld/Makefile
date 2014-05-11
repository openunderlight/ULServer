# Makefile  -*- Makefile -*-
# $Id: Makefile,v 1.7 1997-08-12 23:15:10-07 jason Exp $
# Copyright 1996-1997 Lyra LLC, All rights reserved.
#
# Level Server

# include directories
INCDIRS = -I$(TOPSRCDIR)/include -I$(MYSQL_INCDIR) -I$(PTH_INCDIR)

# optimizer flags
#OPT =				# no optimization or debugging
#OPT = $(OPT_DEBUG)		# debugging only
#OPT = $(OPT_OPT)		# standard optimization
#OPT = $(OPT_DEBUG) $(OPT_OPT)	# debugging and optimization
#OPT = $(OPT_MAXOPT)		# maximal optimization

# profiler flags
#PROF =				# no profiling
#PROF = $(PROF_ON)		# turn on profiling

# extra compiler definitions
XCPLUSDEFS =

# library dirs
LIBDIRS = -L$(TOPSRCDIR)/lib $(LDEFS)

# libraries to link
LIBS = -llyra -lgdbm $(MYSQL_LIBS) -lm $(PTH_LIBS) -lnsl -lz

# use the line below instead to link in the memory profiler
#LIBS = $(CCMALLOC) -llyra -lgdbm $(MYSQL_LIBS) -lm $(PTH_LIBS) -lnsl 

# extra linker definitions
XLDEFS =

# source, object files
SRCS := $(wildcard *.cpp)
INLS := $(wildcard *.i)
INCS := $(wildcard *.h)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

# target file
PRG_TARGET := leveld_exec

# anything that this makefile can build
TARGETS = $(OBJS) $(PRG_TARGET) $(PRG_TARGET_QUANTIFY) $(PRG_TARGET_PURIFY)

# all source files
SOURCES := $(SRCS) $(INCS) $(INLS)

# all doc files
DOCS := README

# standard Makefile targets

all: $(PRG_TARGET)

leveld_exec: $(OBJS)
	$(CXX) -o $(@) $(OBJS) $(LDFLAGS) $(LIBDIRS) $(LIBS)

checkin: checkin-local

clean: clean-local

depend: depend-local-c++

install: $(PRG_TARGET)
	$(INSTALL) -m 0755 $(PRG_TARGET) $(INSTALLDIR)/sbin

install-purify: $(PRG_TARGET_PURIFY)
	$(INSTALL) -m 0755 $(PRG_TARGET_PURIFY) $(INSTALLDIR)/sbin

install-quantify: $(PRG_TARGET_QUANTIFY)
	$(INSTALL) -m 0755 $(PRG_TARGET_QUANTIFY) $(INSTALLDIR)/sbin

# get global rules
include ../../Rules.make

# include generated dependencies
-include .depend

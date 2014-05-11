# Makefile  -*- Makefile -*-
# Copyright 1996-1997 Lyra LLC, All rights reserved.
#
# Lyra Library - MySQL-related source code (server only)

# include directories
INCDIRS = -I. -I../include

# optimizer flags
#OPT =				# no optimization or debugging
#OPT = $(OPT_DEBUG)		# debugging only
#OPT = $(OPT_OPT)		# standard optimization
#OPT = $(OPT_DEBUG) $(OPT_OPT)	# debugging and optimization
#OPT = $(OPT_MAXOPT)		# maximal optimization

# profiler flags
#PROF =				# no profiling
#PROF = $(PROF_ON)		# turn on profiling

# source, object files
SRCS := $(wildcard *.cpp)
INLS := $(wildcard *.i)
INCS := $(wildcard *.h)
LIBS = -lmysqlclient

OBJS := $(patsubst %.cpp,%.o,$(SRCS)) 

# target file
LIB_TARGET := subdir.a

# anything that this makefile can build
TARGETS := $(OBJS) ($LIB_TARGET)

# standard targets

# all source files
SOURCES := $(SRCS) $(INLS) $(INCS) $(OSRC)

# all doc files
DOCS := README

# standard targets

all: $(LIB_TARGET)

# these are just hacks to help the development testing

checkin: checkin-local

#clean: clean-local
clean: 
	rm -f *.o core *~ *.a

depend: 

install: install-includes

# makefile-specific targets

# get global rules
include ../../Rules.make

# include generated dependencies if they exist
-include .depend

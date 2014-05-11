# Makefile  -*- Makefile -*-
# Copyright 1996-1997 Lyra LLC, All rights reserved.
#
# Lyra Library - database abstraction classes

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

# extra compiler definitions
XCPLUSDEFS =

# source, object files
SRCS := $(wildcard *.cpp)
INLS := $(wildcard *.i)
INCS := $(wildcard *.h)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

# target file
LIB_TARGET := subdir.a

# anything that this makefile can build
TARGETS := $(OBJS) $(LIB_TARGET)

# all source files
SOURCES := $(SRCS) $(INLS) $(INCS)

# all documentation files
DOCS := README

# standard Makefile targets

all: $(LIB_TARGET)

checkin: checkin-local

clean: clean-local

depend: depend-local-c++

install: install-includes

# get global rules
include ../../Rules.make

# include generated dependencies
-include .depend

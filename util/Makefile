# Makefile  -*- Makefile -*-
# $Id: Makefile,v 1.10 1997-08-28 16:57:29-07 jason Exp $
# Copyright 1996-1997 Lyra LLC, All rights reserved.
#
# utility programs

# include directories
INCDIRS = -I$(TOPSRCDIR)/include -I$(MYSQL_INCDIR) -I$(PTH_INCDIR)

# optimizer flags
#OPT =				# no optimization or debugging
#OPT = $(OPT_DEBUG)		# debugging only
OPT = $(OPT_OPT)		# standard optimization
#OPT = $(OPT_DEBUG) $(OPT_OPT)	# debugging and optimization
#OPT = $(OPT_MAXOPT)		# maximal optimization

# profiler flags
#PROF =				# no profiling
#PROF = $(PROF_ON)		# turn on profiling

# extra compiler definitions
XCPLUSDEFS =

# library dirs
LIBDIRS = -L$(TOPSRCDIR)/lib -L$(MYSQL_LIBDIR) -L$(PTH_LIBDIR)

# libraries to link
LIBS = -llyra -lgdbm -lm -lpth -lnsl -lmysqlclient

# extra linker definitions
XLDEFS =

# source, object files
SRCS := $(wildcard *.cpp)
INLS := $(wildcard *.i)
INCS := $(wildcard *.h)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

# target files
PRGS := gdbm_clean check_level print_xp check_serverdb dump_item  \
	ping_servers get_server_status get_server_status2 gen_item \
	dump_item2 rotate_logs dump_state fix_ghost

# programs to install
PRGS_INSTALL := gdbm_clean check_level check_serverdb dump_item  \
	ping_servers get_server_status get_server_status2 dump_item2  rotate_logs dump_state fix_ghost

# anything that this makefile can build
TARGETS := $(OBJS) $(PRGS)

# all source files
SOURCES := $(SRCS) $(INCS) $(INLS)

# all doc files
DOCS := README

# standard Makefile targets

all: $(PRGS)

checkin: checkin-local

clean: clean-local

depend: depend-local-c++

install:
	$(INSTALL) -m 0755 $(PRGS_INSTALL) $(INSTALLDIR)/bin

# makefile-specific targets

ping_servers: ping_servers.o
	$(RM) -f $@
	$(CXXLD) -o $@ $^ $(LDFLAGS)

get_server_status: get_server_status.o
	$(RM) -f $@
	$(CXXLD) -o $@ $^ $(LDFLAGS)

get_server_status2: get_server_status2.o
	$(RM) -f $@
	$(CXXLD) -o $@ $^ $(LDFLAGS)

rotate_logs: rotate_logs.o
	$(RM) -f $@
	$(CXXLD) -o $@ $^ $(LDFLAGS)

dump_state: dump_state.o
	$(RM) -f $@
	$(CXXLD) -o $@ $^ $(LDFLAGS)

gdbm_clean: gdbm_clean.o
	$(RM) -f $@
	$(CXXLD) -o $@ $^ $(LDFLAGS)

dump_item: dump_item.o
	$(RM) -f $@
	$(CXXLD) -o $@ $^ $(LDFLAGS)

dump_item2: dump_item2.o
	$(RM) -f $@
	$(CXXLD) -o $@ $^ $(LDFLAGS)

check_level: check_level.o
	$(RM) -f $@
	$(CXXLD) -o $@ $^ $(LDFLAGS)

check_serverdb: check_serverdb.o
	$(RM) -f $@
	$(CXXLD) -o $@ $^ $(LDFLAGS)

print_xp: print_xp.o
	$(RM) -f $@
	$(CXXLD) -o $@ $^ $(LDFLAGS)

gen_item: gen_item.o
	$(RM) -f $@
	$(CXXLD) -o $@ $^ $(LDFLAGS)


fix_ghost: fix_ghost.o
	$(RM) -f $@
	$(CXXLD) -o $@ $^ $(LDFLAGS)

# trivial/temporary test programs named foo
foo: foo.o
	$(RM) -f $@
	$(CXXLD) -o $@ $^ $(LDFLAGS)

# get global rules
include ../Rules.make

# include generated dependencies
-include .depend


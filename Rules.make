# Rules.make.sun  -*- Makefile -*-
# Copyright 1996-1997 Lyra LLC, All rights reserved.
#
# Global Rules file - Linux GNU C++ compiler version

# directories
#
# top-level project directory
TOPDIR := /home/brent/lyra
# top-level source directory
TOPSRCDIR := $(TOPDIR)/src
# directory where shared files are to be copied
SHAREDDIR := $(TOPSRCDIR)/shared
# top-level install directory
INSTALLDIR := /opt/lyra/ul/prod

# global TAGS file
TAGFILE := $(TOPSRCDIR)/.etags

# name of the makefile
#
MAKEFILE := Makefile

# shell that Makefile should use (bash)
#
SHELL := /bin/bash

GCC := gcc 
GXX := g++ -std=c++0x 

# compilers and related programs
#
CC := $(GCC)
CXX := $(GXX)
CXXLD := $(GXX)
CCMKDEP := $(GCC) -MM
CXXMKDEP := $(GXX) -MM
AR := ar -r
LD := ld
RANLIB := ranlib
RM := /bin/rm
CI := ci
CO := co
FIND := find
GREP := grep
XARGS := xargs
INSTALL := install
ENSCRIPT := enscript
ETAGS := etags
LN := ln

MYSQL_INCDIR := /usr/include/mysql
#MYSQL_LIBDIR := /usr/lib/mysql
MYSQL_LIBDIR := /usr/lib64/mysql
MYSQL_LIBS := -lmysqlclient

PTH_INCDIR := /usr/include/
#PTH_INCDIR := /usr/local/include/
PTH_LIBDIR := /usr/lib64
#PTH_LIBDIR := /usr/local/lib
PTH_LIBS := -lpth

# ccmalloc is for memory leak detection; need to copy object files 
# to get proper reports
#CCMALLOC := ccmalloc-g++.o ccmalloc-gcc.o callchain.o hash.o wrapper.o -ldl

CCMALLOC := -ldl -lccmalloc

# development or release build
# RELEASE means release mode restrictios are present; 
#   you can use both this and DEBUG to run debugging code
#   in the live production environment
# DEBUG means generate debug code

BUILD := -DRELEASE
#BUILD := -DDEBUG
#OPT := -g2
OPT := -O2
# optimization/debugging macros
#
# -xtarget=native: generate code for native architecture
#OPT_OPTMAX := -fast 


# profiling macros
#
#PROF_GPROF := -pg
#PROF_TCOV := -pg -a

# default profiler flags (can be overridden in local Makefile)
#
#PROF := $(PROF_GPROF)


# how to compile .c files into .s (asm) and .o files
#
.SUFFIXES: .c
.c.o:
#	$(CC) $(CFLAGS) -c $<
	$(CC) $(CFLAGS) -c $< -o $@
.c.s:
	$(CC) $(CFLAGS) -S $< -o $@

# how to compile .cpp files into .s (asm) and .o files
#
.SUFFIXES: .cpp
.cpp.o:
#	$(CXX) $(CPLUSFLAGS) -c $< -o $@
	$(CXX) $(CPLUSFLAGS) -c $< 
.cpp.s:
	$(CXX) $(CPLUSFLAGS) -S $< -o $@

.SUFFIXES: .C
.C.o:
	$(CXX) $(CPLUSFLAGS) -c $< -o $@

# Common Definitions for C/C++
#  -D_REENTRANT: required for MT programs
#  -D_POSIX_PTHREAD_SEMANTICS: POSIX-compliance
#  -D_POSIX_PER_PROCESS_TIMER_SOURCE: POSIX-compliance
#  -DUSE_INLINE: if defined, inline methods are inline, otherwise they aren't
#  -DUSE_DEBUG: include debugging code (Dump() methods, etc)
#  -DSERVER: server-only code
#  -DCLIENT: client-only code
#  -I/usr/local/include: get local include files
#  -I$(PURE_INCDIR): get purify/quantify include files
#  $(BUILD): defines RELEASE for release builds, nothing for dev builds
#
#  -DUSE_MPATH: if defined, compile for Mpath integration
#  -I$(MPATH_INCDIR): if USE_MPATH defined, where to get Mpath includes
#
# COMDEFS := -DUSE_INLINE -DUSE_DEBUG -D_REENTRANT -DSERVER $(BUILD)\
#		-D_POSIX_PTHREAD_SEMANTICS -D_POSIX_PER_PROCESS_TIMER_SOURCE\
		 #-I/usr/local/include -I$(MYSQL_INCDIR) -I$(PTH_INCDIR)\
		 #-Wno-pmf-conversions
COMDEFS := -DUSE_INLINE -DUSE_DEBUG -D_REENTRANT -DSERVER $(BUILD) \
           -D_POSIX_PTHREAD_SEMANTICS -D_POSIX_PER_PROCESS_TIMER_SOURCE \
	   -I/usr/include -I/usr/include/c++/4.4.4 -I/usr/include/mysql -I$(MYSQL_INCDIR) -I$(PTH_INCDIR) \
	   -Wno-pmf-conversions


#
#
#		 -DUNICODE -_UNICODE

#  compiler/preprocessor definitions
#
CDEFS = $(COMDEFS)

# C compiler flags
#
# individual Makefiles set:
#  $(INCDIRS): list of include dirs (eg. -I/usr/local/include -I..)
#  $(OPT): optimization flags (eg. -g -O, -g, -O2, etc.)
#  $(PROF): profiling flags (eg. -pg)  (Note: profiling currently doesn't work)
#  $(XCDEFS): any other extra compilation flags
#
CFLAGS = $(strip $(INCDIRS) $(CDEFS) $(OPT) $(PROF))
CFLAGS_DEP = $(strip $(INCDIRS))

# C++ compiler/preprocessor definitions
#  -mt: multi-threaded support
#  -W: warnings
#  -I$(TOPSRCDIR)/stl/*: include free STL for given compiler
#  -instances=static: static template instantiation (no database)
#  -noex: don't generate code for exceptions
#  -xs: store debugging info in executable (makes them really big)
#
#  -DUSE_LMNEW: use logging new/delete operators (slow!)
#  -ptr$(TOPSRCDIR)/lib: template instantiation database location
#
CPLUSDEFS = $(COMDEFS) -W 

# -instances=static 

# -I$(TOPSRCDIR)/stlport 

# C++ compiler flags
#
# individual Makefiles set:
#  $(INCDIRS): list of include dirs (eg. -I/usr/local/include -I..)
#  $(OPT): optimization flags (eg. -g -O, -g, -O2, etc.)
#  $(PROF): profiling flags (eg. -pg)  (Note: profiling currently doesn't work)
#  $(XCPLUSDEFS): any other extra compilation flags
#
CPLUSFLAGS = $(strip $(INCDIRS) $(CPLUSDEFS) $(XCPLUSDEFS) $(OPT) $(PROF))
CPLUSFLAGS_DEP = $(strip $(INCDIRS))

# linker definitions
#  -L/usr/local/lib: get local libraries
#  -L/usr/lib/mysql: get mysql libraries
#  -L$(PURE_LIBDIR): get purify/quantify stub libs
#  $(PURE_LIBS): include purify/quantify stub libs
#
#  -xildoff: don't use incremental linker
#  -ptr$(TOPSRCDIR)/lib: template instantiation database location
#

LDEFS = -L/usr/lib64 -L$(MYSQL_LIBDIR)
#LDEFS = -L/usr/local/lib -L$(MYSQL_LIBDIR)

# linker flags
#
# individual Makefiles set:
#  $(LIBDIRS): list of library dirs (eg. -L..)
#  $(LIBS): extra libraries to link (eg. -lsocket)
#  $(XLDEFS): any other extra linker flags (eg. -s, for stripping binaries)
#  $(OPT): optimization flags
#  $(PROF): profiling
#
LDFLAGS = $(strip $(PROF) $(LIBDIRS) $(OPT) $(LDEFS) $(XLDEFS) $(LIBS))

# rule to compile a set of .o files into a single .o file
#  $(OBJ_TARGET): target .o file (eg. pthreads.o)
#  $(OBJS): source .o file(s)
#
ifdef OBJ_TARGET
$(OBJ_TARGET): $(OBJS)
	$(RM) -f $@
	$(LD) -r -o $@ $(OBJS)
endif

# rule to compile a set of .o files into a single .a file
#  $(LIB_TARGET): .a file (eg. liblyra.a)
#  $(OBJS): source .o file(s)
#
ifdef LIB_TARGET
$(LIB_TARGET): $(OBJS)
	$(RM) -f $@
	$(AR) $@ $(OBJS)
	$(RANLIB) $@
endif


# rule to make dependencies
#  $(SRCS): source files (.c, .cpp)
#
ifdef SRCS
depend-local-c++:
	$(RM) -f .depend
	$(CXXMKDEP) $(CPLUSFLAGS_DEP) $(SRCS) > .depend

depend-local-c:
	$(RM) -f .depend
	$(CCMKDEP) $(CFLAGS_DEP) $(SRCS) > .depend
endif

# rule to make tags file (actually, just symlink to global one)
#   $(SOURCES): source files (.h, .i, .cpp)
#
ifdef SOURCES
tags:
	ln -sf $(TAGFILE) ./TAGS
endif

# rule to clean directory
#  $(TARGETS): files that can be rebuilt - ie. can be removed
#
ifdef TARGETS
clean-local:
	$(RM) -f $(TARGETS) Makefile.bak core *_pure_*.o *.s *~
endif

# rule to check in source files
#  $(SOURCES): files that should be checked in
#
ifdef SOURCES
checkin-local:
	$(CI) -zLT -q -l -mnone $(SOURCES)
else
# assume no sources, just the makefile
checkin-local:
	$(CI) -zLT -q -l -mnone $(MAKEFILE)
endif

# rule to make printable version of source files
#  $(SOURCES): source files that should be printed
#  $(DOCS): documentation files that should be printed
#
ifdef SOURCES
print:
	$(RM) -f source.ps doc.ps
	$(ENSCRIPT) -G -pdoc.ps $(DOCS)
	$(C2PS) -b -m -f 8 -_ box -p Letter -d 0.5 $(SOURCES) > source.ps
endif

# rule to copy include files to top-level include directory
#  $(INCS): include files (*.h)
#  $(INLS): include files (*.i)
#
ifdef INCS
install-includes:
	for file in $(INCS) $(INLS); do \
	  if $$(cmp -s $$file $(TOPSRCDIR)/include/$$file); then true; \
          else $(INSTALL) -m 0644 $$file $(TOPSRCDIR)/include; fi; \
        done
endif

# rule to copy shared source files to shared dir (for export to Win32 system)
#  $(SHARED): files to be copied
#
ifdef SHARED
install-shared:
	$(INSTALL) -m 0644 $(SHARED) $(SHAREDDIR)
endif

# rules to do a 'make' in a list of subdirectories for various targets
#  $(SUBDIRS): subdirectories
#
ifdef SUBDIRS
all-subdirs:
	for i in $(SUBDIRS); do $(MAKE) -j 6 -C $$i all; done

clean-subdirs:
	for i in $(SUBDIRS); do $(MAKE) -C $$i clean; done

install-subdirs:
	for i in $(SUBDIRS); do $(MAKE) -C $$i install; done

checkin-subdirs:
	for i in $(SUBDIRS); do $(MAKE) -C $$i checkin; done

depend-subdirs:
	for i in $(SUBDIRS); do $(MAKE) -C $$i depend; done

tags-subdirs:
	for i in $(SUBDIRS); do $(MAKE) -C $$i tags; done

endif
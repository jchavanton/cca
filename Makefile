# Since the programs in this directories are examples for the user, this
# make file should be as ordinary as possible.  It should not rely heavily
# on included make files or configuration parameters.  It should not use
# libtool.  Also, we don't try to build or rebuild the libraries on which
# these programs depend.
CXXFLAGS= -std=c++11 -g

ifeq ($(SRCDIR),)
SRCDIR = $(CURDIR)/xmlrpc
BLDDIR = $(SRCDIR)
endif
SUBDIR=../src

include $(BLDDIR)/config.mk

default: all

CXXFLAGS_ALL = $(CFLAGS_PERSONAL) $(CXXFLAGS) $(CADD)
LDFLAGS_ALL = $(LDFLAGS_PERSONAL) $(LDFLAGS) $(LADD)

# If this were a real application, working from an installed copy of
# Xmlrpc-c, XMLRPC_C_CONFIG would just be 'xmlrpc-c-config'.  It would be
# found in the user's PATH.
XMLRPC_C_CONFIG = $(BLDDIR)/xmlrpc-c-config.test

SERVERPROGS_ABYSS = \
  call_control_agent

# Build up PROGS:
PROGS =

ifeq ($(ENABLE_ABYSS_SERVER),yes)
  PROGS += $(SERVERPROGS_ABYSS)
endif

# PROGS += pstream_inetd_server pstream_serial_server

INCLUDES = -I. $(shell $(XMLRPC_C_CONFIG) c++2 client abyss-server --cflags)

LIBS_SERVER_ABYSS = \
  $(shell $(XMLRPC_C_CONFIG) c++2 abyss-server --libs)

LIBS_SERVER_CGI = \
  $(shell $(XMLRPC_C_CONFIG) c++2 cgi-server --libs)

LIBS_CLIENT = \
  $(shell $(XMLRPC_C_CONFIG) c++2 client --libs)

LIBS_BASE = \
  $(shell $(XMLRPC_C_CONFIG) c++2 --libs)

all: $(PROGS)

$(SERVERPROGS_CGI):%.cgi:%_cgi.o
	$(CXXLD) -o $@ $^ $(LIBS_SERVER_CGI) $(LDFLAGS_ALL)

$(SERVERPROGS_ABYSS):%:%.o
	$(CXXLD) -o $@ $^ $(LIBS_SERVER_ABYSS) $(LIBS_CLIENT) $(LDFLAGS_ALL)

$(CLIENTPROGS):%:%.o
	$(CXXLD) -o $@ $^ $(LIBS_CLIENT) $(LDFLAGS_ALL)


LIBS_PSTREAM_CLIENT = \
  $(shell $(XMLRPC_C_CONFIG) c++2 client --libs)

pstream_client:%:%.o
	$(CXXLD) -o $@ $^ $(LIBS_PSTREAM_CLIENT) $(LDFLAGS_ALL)

LIBS_PSTREAM_SERVER = \
  $(shell $(XMLRPC_C_CONFIG) c++2 pstream-server --libs)

pstream_inetd_server pstream_serial_server:%:%.o
	$(CXXLD) -o $@ $^ $(LIBS_PSTREAM_SERVER) $(LDFLAGS_ALL)

OBJECTS = $(patsubst %,%.o,$(patsubst %.cgi,%_cgi,$(PROGS)))

$(OBJECTS):%.o:%.cpp
	$(CXX) -c $(INCLUDES) $(CXXFLAGS_ALL) $<

# See example/Makefile for an explanation of config.h and xmlrpc_amconfig.h

$(OBJECTS): config.h xmlrpc_amconfig.h

config.h:
	$(LN_S) $(BLDDIR)/xmlrpc_config.h $@
xmlrpc_amconfig.h:
	$(LN_S) $(BLDDIR)/$@ .

.PHONY: clean
clean:
	rm -f $(PROGS) *.o config.h xmlrpc_amconfig.h

.PHONY: distclean
distclean: clean

.PHONY: dep depend
dep depend:
# We don't do dependencies in this directory, because it's supposed to be
# an example of what a program outside this package would do, so we can't
# go weaving it into the rest of the package.  Ergo, a developer must
# carefully clean and remake examples as he updates other parts of the tree.

install: call_control_agent
	mv call_control_agent /usr/local/bin

EXTENSION = qr

PG_CONFIG ?= pg_config

MODULE_big = $(EXTENSION)
OBJS = pgqr.o qrcodegen.o
HEADERS = qrcodegen.h

PKG_CONFIG ?= pkg-config
ifeq (no,$(shell $(PKG_CONFIG) libpng || echo no))
$(warning libpng not registed with pkg-config, build might fail)
endif

PG_CPPFLAGS += $(shell $(PKG_CONFIG) --cflags-only-I libpng)
SHLIB_LINK += $(shell $(PKG_CONFIG) --libs libpng)

#ifdef WITH_DEMO
#OBJS += qrcodegen-demo.o
#PROGRAM = qrcodegen-demo
#PG_LIBS += $(shell $(PKG_CONFIG) --libs libpng)
#endif

DATA = $(wildcard sql/*--*.sql)

TESTS = $(wildcard test/sql/*.sql)
REGRESS = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test --load-extension=$(EXTENSION)

EXTRA_CLEAN = *.png

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

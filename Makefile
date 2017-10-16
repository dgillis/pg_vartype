PGVARTYPEVERSION = 0.4
MODULES = pg_vartype
MODULE_big = pg_vartype
EXTENSION = pg_vartype
DOCS = README.md
OBJS = src/pg_vartype.o src/pg_vartype_datetime.o src/pg_vartype_parser.o
DATA = sql/pg_vartype--*.sql

TESTS = $(wildcard test/sql/*.sql)
REGRESS = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

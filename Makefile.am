AUTOMAKE_OPTIONS=foreign
SUBDIRS=src
AMLDFLAGS=@NCURSES_LIBS@
bin_PROGRAMS=vmtop
vmtop_SOURCES= \
	src/field.c \
	src/field.h \
	src/vmtop.c \
	src/vmtop.h
vmtop_LDADD= src/libdomain.a

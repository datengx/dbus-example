TARGETS=readline_simple readline_completion
CC=gcc
CCOPTS=-Wall -Wextra
.PHONY: all clean

INC = -I/usr/include -I./ $(shell pkg-config --cflags dbus-glib-1) \
                     $(shell pkg-config --cflags dbus-1)\
		     $(shell pkg-config --cflags boost)\
		     $(shell pkg-config --cflags glib-2.0)

# Object cli-pair depends on
LIBS = $(shell pkg-config --libs dbus-glib-1) \
       $(shell pkg-config --libs dbus-1) \
       $(shell pkg-config --libs glib-2.0)\
       $(shell pkg-config --libs boost)\
       -L/usr/local/lib -lreadline

all: destroy_bin create_bin $(TARGETS)

destroy_bin:
	rm -rf ./bin

create_bin:
	mkdir bin

%: %.c
	$(CC) $(CCOPTS) $(INC) -o $@ $< $(LIBS)
	mv $@ ./bin

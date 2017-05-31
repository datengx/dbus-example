TARGETS=readline_simple readline_completion
CC=gcc
CCOPTS=-Wall -Wextra -Wunused-parameter
.PHONY: all clean

DEP_OBJECT = gdbus/mainloop.o gdbus/client.o gdbus/object.o gdbus/polkit.o gdbus/watch.o
client_bluetoothctl_SOURCES_DIST = client/display.c client/agent.c \
																		client/gatt.c monitor/uuid.c

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

## Object files
gdbus/mainloop.o:
	gcc -O $(CCOPTS) $(INC) -c ./gdbus/mainloop.c
	mv mainloop.o gdbus/mainloop.o

gdbus/client.o:
	gcc -O $(CCOPTS) $(INC) -c ./gdbus/client.c
	mv client.o gdbus/client.o

gdbus/object.o:
	gcc -O $(CCOPTS) $(INC) -c ./gdbus/object.c
	mv object.o gdbus/object.o

gdbus/polkit.o:
	gcc -O $(CCOPTS) $(INC) -c ./gdbus/polkit.c
	mv polkit.o gdbus/polkit.o

gdbus/watch.o:
	gcc -O $(CCOPTS) $(INC) -c ./gdbus/watch.c
	mv watch.o gdbus/watch.o

destroy_bin:
	rm -rf ./bin

create_bin:
	mkdir bin

%: %.c
	$(CC) $(CCOPTS) $(INC) -o $@ $< $(client_bluetoothctl_SOURCES_DIST) $(DEP_OBJECT) $(LIBS)
	mv $@ ./bin

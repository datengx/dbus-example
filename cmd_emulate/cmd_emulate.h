#ifndef __C_WRAPPER_H__
#define __C_WRAPPER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <glib.h>
#include <dbus/dbus.h>
#include "gdbus/gdbus.h"

#include "signal-def.h"

/* String display constants */
#define COLORED_NEW	"NEW"
#define COLORED_CHG	"CHG"
#define COLORED_DEL	"DEL"

#define PROMPT_ON	"[Test readline]# "
#define PROMPT_OFF	"[Test readline]# "

GMainLoop *main_loop;
DBusConnection *dbus_conn;
guint input;
GDBusProxy *agent_manager;

void rl_handler(char *in);
gboolean service_is_child(GDBusProxy *service);
gboolean device_is_child(GDBusProxy *device, GDBusProxy *master);
void print_iter(const char *label, const char *name,
						DBusMessageIter *iter);
void set_default_device(GDBusProxy *proxy, const char *attribute);
void set_default_attribute(GDBusProxy *proxy);
void print_adapter(GDBusProxy *proxy, const char *description);
void print_device(GDBusProxy *proxy, const char *description);
void connect_handler(DBusConnection *connection, void *user_data);
void disconnect_handler(DBusConnection *connection, void *user_data);
void proxy_added(GDBusProxy *proxy, void *user_data);
gboolean signal_handler(GIOChannel *channel, GIOCondition condition,
							gpointer user_data);
gboolean input_handler(GIOChannel *channel, GIOCondition condition,
							gpointer user_data);
guint setup_signalfd(void);
guint setup_standard_input(void);
void client_ready(GDBusClient *client, void *user_data);
void message_handler(DBusConnection *connection,
					DBusMessage *message, void *user_data);
void proxy_removed(GDBusProxy *proxy, void *user_data);
void property_changed(GDBusProxy *proxy, const char *name,
					DBusMessageIter *iter, void *user_data);
char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);

#endif

#include <iostream>
extern "C" {
#include "cmd_emulate/cmd_emulate.h"
}

extern GMainLoop *main_loop;
extern DBusConnection *dbus_conn;
extern guint input;
extern GDBusProxy *agent_manager;

const char* g_signals[] = {
  "quit"
};

/**
 *  Input handler function for the interactive shell
 */
void rl_handler(char *in)
{
  printf("You just entered\n");
  printf("%s\n", in);

  // If it's the quit user signal
  if (strcmp(in, g_signals[CMDLINE_SIG_QUIT]) == 0) {
    // rage quit!
    exit(-2);
  }


  // Add the code for ndn

}

int
main(int argc, char *argv[])
{

    guint signal;

    main_loop = g_main_loop_new(NULL, FALSE);
    dbus_conn = g_dbus_setup_bus(DBUS_BUS_SYSTEM, NULL, NULL);
    GDBusClient *client;

    // each time TAB key is hit, the function assigned to
    // rl_attempted_completion_function will be called
    rl_attempted_completion_function = character_name_completion;

    // @@ using callback behavior
    rl_erase_empty_line = 1;
	  rl_callback_handler_install(NULL, rl_handler);
    rl_set_prompt(PROMPT_OFF);
	  rl_redisplay();

    signal = setup_signalfd();
    client = g_dbus_client_new(dbus_conn, "org.bluez", "/org/bluez");

    g_dbus_client_set_connect_watch(client, connect_handler, NULL);
	  g_dbus_client_set_disconnect_watch(client, disconnect_handler, NULL);
	  g_dbus_client_set_signal_watch(client, message_handler, NULL);

	  g_dbus_client_set_proxy_handlers(client, proxy_added, proxy_removed,
							property_changed, NULL);

	  input = 0;
	  g_dbus_client_set_ready_watch(client, client_ready, &input);



    rl_on_new_line();
    rl_redisplay();
    g_main_loop_run(main_loop);

    g_source_remove(signal);

    rl_message();
	  rl_callback_handler_remove();

    g_main_loop_unref(main_loop);

    return 0;
}

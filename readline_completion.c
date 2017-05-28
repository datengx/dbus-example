#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <glib.h>
#include <dbus/dbus.h>


#define PROMPT_OFF	"[Test readline]# "



static GMainLoop *main_loop;
static DBusConnection *dbus_conn;
static guint input = 0;
char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);

char *character_names[] = {
    "Arthur Dent",
    "Ford Prefect",
    "Tricia McMillan",
    "Zaphod Beeblebrox",
    NULL
};

static void rl_handler(char *in)
{
  printf("You just entered\n");
  printf("%s", in);
}

static gboolean signal_handler(GIOChannel *channel, GIOCondition condition,
							gpointer user_data)
{
	static bool terminated = false;
	struct signalfd_siginfo si;
	ssize_t result;
	int fd;

	if (condition & (G_IO_NVAL | G_IO_ERR | G_IO_HUP)) {
		g_main_loop_quit(main_loop);
		return FALSE;
	}

	fd = g_io_channel_unix_get_fd(channel);

	result = read(fd, &si, sizeof(si));
	if (result != sizeof(si))
		return FALSE;

	switch (si.ssi_signo) {
	case SIGINT:
		if (input) {
			rl_replace_line("", 0);
			rl_crlf();
			rl_on_new_line();
			rl_redisplay();
			break;
		}

		/*
		 * If input was not yet setup up that means signal was received
		 * while daemon was not yet running. Since user is not able
		 * to terminate client by CTRL-D or typing exit treat this as
		 * exit and fall through.
		 */
	case SIGTERM:
		if (!terminated) {
			rl_replace_line("", 0);
			rl_crlf();
			g_main_loop_quit(main_loop);
		}

		terminated = true;
		break;
	}

	return TRUE;
}

static guint setup_signalfd(void)
{
	GIOChannel *channel;
	guint source;
	sigset_t mask;
	int fd;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGTERM);

	if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
		perror("Failed to set signal mask");
		return 0;
	}

	fd = signalfd(-1, &mask, 0);
	if (fd < 0) {
		perror("Failed to create signal descriptor");
		return 0;
	}

	channel = g_io_channel_unix_new(fd);

	g_io_channel_set_close_on_unref(channel, TRUE);
	g_io_channel_set_encoding(channel, NULL, NULL);
	g_io_channel_set_buffered(channel, FALSE);

	source = g_io_add_watch(channel,
				G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_NVAL,
				signal_handler, NULL);

	g_io_channel_unref(channel);

	return source;
}

int
main(int argc, char *argv[])
{

    guint signal;

    main_loop = g_main_loop_new(NULL, FALSE);


    // each time TAB key is hit, the function assigned to
    // rl_attempted_completion_function will be called
    rl_attempted_completion_function = character_name_completion;

    printf("Who's your favourite Hitchiker's Guide character?\n");
    //char *buffer = readline("> ");
    //if (buffer) {
    //    printf("You entered: %s\n", buffer);
    //    free(buffer);
    //}

    // @@ using callback behavior
    rl_erase_empty_line = 1;
	  rl_callback_handler_install(NULL, rl_handler);
    rl_set_prompt(PROMPT_OFF);
	  rl_redisplay();

    signal = setup_signalfd();

    rl_on_new_line();
    rl_redisplay();
    g_main_loop_run(main_loop);

    g_source_remove(signal);

    rl_message("");
	  rl_callback_handler_remove();

    g_main_loop_unref(main_loop);

    return 0;
}

char **
character_name_completion(const char *text, int start, int end)
{
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, character_name_generator);
}

char *
character_name_generator(const char *text, int state)
{
    static int list_index, len;
    char *name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    // Comparing the text that has already been input
    // to the ones in the list
    while ((name = character_names[list_index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return NULL;
}

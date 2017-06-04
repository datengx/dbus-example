#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
extern "C" {
#include "cmd_emulate/cmd_emulate.h"
}

#include "endpoints/producer.hpp"
#include "core/scheduler.hpp"
#include "core/common.hpp"
#include <unistd.h>

extern GMainLoop *main_loop;
extern DBusConnection *dbus_conn;
extern guint input;
extern GDBusProxy *agent_manager;

const char* g_signals[] = {
  "quit",
  "run_producer"
};
/*
void WorkerThread( boost::shared_ptr< ndn::chunks::Producer > producer )
{
	std::cout << "Thread Start\n";
	producer->run(ndn::time::milliseconds(1000));
	std::cout << "Thread Finish\n";
}
*/

std::shared_ptr<std::thread> producer_thread;

void
launch_thread(std::shared_ptr<ndn::chunks::Producer> producer)
{
  std::cout << "schedule event" << std::endl;
  //nfd::scheduler::schedule(ndn::time::seconds(0),
  //                      std::bind((void(ndn::chunks::Producer::*)(const ndn::time::milliseconds&))&ndn::chunks::Producer::run, &*producer, ndn::time::milliseconds(1000)));
  producer->run(ndn::time::milliseconds(100));
  std::cout << "after schedule event" << std::endl;
}



/**
 * Start the producer
 */
int
run_producer()
{
  // parameter setup
  bool printVersion = false;
  uint64_t freshnessPeriod = 10000;
  size_t maxChunkSize = ndn::MAX_NDN_PACKET_SIZE >> 1;
  std::string signingStr;
  bool isVerbose = true;
  // Check validity of the prefix
  // TODO: !!
  std::string prefix = "/localhop/bluetooth";

  ndn::security::SigningInfo signingInfo;
  try {
    signingInfo = ndn::security::SigningInfo(signingStr);
  }
  catch (const std::invalid_argument& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 2;
  }

  std::cout << "Initializing Producer!" << std::endl;

  try {
    ndn::Face face;
    ndn::KeyChain keyChain;
    std::string input("Hello World!\n");
    std::shared_ptr<ndn::chunks::Producer> producer = std::make_shared<ndn::chunks::Producer>(prefix, face, keyChain, signingInfo, ndn::time::milliseconds(freshnessPeriod),
                                   maxChunkSize, isVerbose, printVersion, input);
    // producer.run(ndn::time::milliseconds(-1));
    //worker_threads.create_thread( boost::bind( &WorkerThread, producer ) );

    //producer->run(ndn::time::milliseconds(1000));
    //nfd::scheduler::schedule(ndn::time::seconds(1),
    //                    std::bind((void(ndn::chunks::Producer::*)(const ndn::time::milliseconds&))&ndn::chunks::Producer::run, &*producer, ndn::time::milliseconds(1000)));
    //boost::thread_group;
    //producer_thread = std::make_shared<std::thread>(launch_thread, producer);
    //producer_thread->detach();
    // producer->run(ndn::time::milliseconds(100));
    producer->run();
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL)
            result += buffer.data();
    }
    return result;
}

/**
 *  Input handler function for the interactive shell
 */
void
rl_handler(char *in)
{
  printf("command: %s\n", in);
  int err = 0;
  char* in_orig;
  in_orig = strdup(in);

  // If it's the quit user signal
  if (strcmp(in, g_signals[CMDLINE_SIG_QUIT]) == 0) {
    // rage quit!
    exit(-2);
  }

  // Initialize producer
  //if (strcmp(in, g_signals[CMDLINE_SIG_RUN_PRODUCER]) == 0) {
  //
  //  err = run_producer();
  //  std::cout << "run_producer Error: " << err << std::endl;
  //}

  add_history(in);

  char *cmd, *arg;
  cmd = strtok_r(in, " ", &arg);

  if (!cmd)
    return;

  if (arg) {
		int len = strlen(arg);
		if (len > 0 && arg[len - 1] == ' ')
			arg[len - 1] = '\0';
	}

  // std::cout << "cmd: " << cmd << std::endl;
  // std::cout << "arg: " << arg << std::endl;


  if (strcmp(cmd, "face") == 0) {
    std::string nfdc_out;
    std::string nfdc_exec("nfdc ");
    std::string nfdc_arg1(cmd);
    std::string nfdc_arg2(arg);
    std::string arg_list = nfdc_arg1 + " " + nfdc_arg2;
    std::string nfdc_cmd = nfdc_exec + arg_list;
    nfdc_out = exec(nfdc_cmd.c_str());
    std::cout << nfdc_out << std::endl;
    return;
  }

  // Using bluetoothctl command handler
  bt_handler(in_orig);
}

namespace ndn {

namespace chunks {
}
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
    rl_attempted_completion_function = cmd_completion;

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

    resource_release();
    return 0;

}

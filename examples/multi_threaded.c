#include "mongoose.h"

// Start a browser and hit refresh couple of times. The replies will
// come from both server instances.
static int request_handler(struct mg_connection *conn) {
  mg_printf(conn, "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\n\r\n"
            "This is a reply from server instance # %s",
            (char *) conn->server_param);
  return 1;
}

static void *serve(void *server) {
  for (;;) mg_poll_server((struct mg_server *) server, 1000);
  return NULL;
}

int main(void) {
  struct mg_server *server1, *server2;

  server1 = mg_create_server("1");
  server2 = mg_create_server("2");

  mg_add_uri_handler(server1, "/", request_handler);
  mg_add_uri_handler(server2, "/", request_handler);

  // Make both server1 and server2 listen on the same socket
  mg_set_option(server1, "listening_port", "8080");
  mg_set_listening_socket(server2, mg_get_listening_socket(server1));

  // server1 goes to separate thread, server 2 runs in main thread
  mg_start_thread(serve, server1);
  serve(server2);

  return 0;
}

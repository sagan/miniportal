#include "EmbeddableWebServer.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <libgen.h>

struct Response* responseAllocRedirect(const char* host) {
  struct Response* response = responseAlloc(302, "Found", "text/html", 0);
  response->extraHeaders = (char*)malloc(64);
  sprintf(response->extraHeaders, "Location: http://%s/\r\n", host);
  heapStringAppendFormat(&response->body, "Redirect to http://%s/", host);
  return response;
}

int PORT = 8080;
char HOSTNAME[32] = {0};
char HOST[40] = {0};

void clean() {
  system("iptables -t nat -F portal_prerouting_a");
  system("iptables -t nat -D PREROUTING -j portal_prerouting");
  system("iptables -t nat -D portal_prerouting -j portal_prerouting_a");
}

void handleClearAndExit(int signal) {
  clean();	
  exit(0);
}

int main(int argc, char* argv[]) {
  int opt;
  char command[256] = {0};

  while ((opt = getopt(argc, argv, "p:h:")) != -1) {
    switch (opt) {
      case 'p':
        PORT = atoi(optarg);
        break;
      case 'h':
        strcpy(HOSTNAME, optarg);
        break;
      default:
        fprintf(stderr, "Usage: %s [-p port] [-h hostname]\n", argv[0]);
        exit(1);
    }
  }
  if( PORT != 80 ) {
    sprintf(HOST, "%s:%d", HOSTNAME, PORT);
  } else {
    strcpy(HOST, HOSTNAME);
  }


  clean();
  signal(SIGHUP, handleClearAndExit);
  signal(SIGINT, handleClearAndExit);
  signal(SIGQUIT, handleClearAndExit);
  signal(SIGTERM, handleClearAndExit);
  
  readlink("/proc/self/exe", command, sizeof(command) - 1);
  chdir(dirname(command));

  system("ipset create portal_wl hash:ip");
  system("iptables -t nat -N portal_prerouting");
  system("iptables -t nat -N portal_prerouting_a");

  system("iptables -t nat -I PREROUTING 1 -j portal_prerouting");
  system("iptables -t nat -A portal_prerouting -j portal_prerouting_a");
  system("iptables -t nat -A portal_prerouting_a -m set --match-set portal_wl src -j RETURN");
  system("iptables -t nat -A portal_prerouting_a -d 192.168.0.0/16 -j RETURN");
  system("iptables -t nat -A portal_prerouting_a -d 10.0.0.0/8 -j RETURN");
  sprintf(command, "iptables -t nat -A portal_prerouting_a -p tcp -j REDIRECT --to-port %d", PORT);
  system(command);

  return acceptConnectionsUntilStoppedFromEverywhereIPv4(NULL, PORT);
}

// why not use ipset hash:mac ? Because some router ROM build do not include hash:mac ipset support

struct Response* createResponseForRequest(const struct Request* request, struct Connection* connection) {
  // printf("Host %s connected\n", connection->remoteHost);
  char buf[128] = {0}, mac[18] = {0}; // 17: 00:00:00:00:00:00 type mac address length
  FILE* pipe;
  int status;
  struct Header* hostHeader;

  if( HOST[0] != 0 &&
      (hostHeader = headerInRequest("Host", request)) != NULL &&
      strcmp(hostHeader->value.contents, HOST)
    ) {
    return responseAllocRedirect(HOST);
  }

  if (0 == strcmp(request->pathDecoded, "/__auth__")) {
    sprintf(buf, "cat /proc/net/arp | grep %s | awk '{print $4}'", connection->remoteHost);
    pipe = popen(buf, "r");
    if( !pipe ) {
      return responseAllocWithFormat(503, "Service Unavailable", "application/json", "{}");
    }
    
    status = fread(mac, sizeof(mac) - 1, 1, pipe);
    pclose(pipe);
    if( !status ) {
      return responseAllocWithFormat(400, "Bad Request", "application/json", "{}");
    }
    
    sprintf(buf, "iptables -t nat -C portal_prerouting_a -m mac --mac-source %s -j RETURN", mac);
    status = system(buf);
    
    if (0 == strcmp(request->method, "POST") || 0 == strcmp(request->method, "PUT")) {
      if( status ) {
        sprintf(buf, "iptables -t nat -I portal_prerouting_a 2 -m mac --mac-source %s -j RETURN", mac);
        system(buf);
      }
      return responseAllocWithFormat(200, "OK", "application/json", "{}");
    }
    
    // assume GET
    if( status ) {
      return responseAllocWithFormat(401, "Unauthorized", "application/json", "{}");
    }
    return responseAllocWithFormat(200, "OK", "application/json", "{}");
  }

  /* Serve files from the current directory */
  if (request->pathDecoded == strstr(request->pathDecoded, "/__files__")) {
    return responseAllocServeFileFromRequestPath("/", request->path, request->pathDecoded, ".");
  }
  return responseAllocHTML("<!DOCTYPE html><html lang=\"zh-CN\">"
    "<head><meta charset=\"utf-8\"><title>Welcome</title><link rel=\"stylesheet\" href=\"/__files__/app.css\"></head>"
    "<body><div id=\"root\"></div></body><script src=\"/__files__/config.js\"></script><script src=\"/__files__/app.js\"></script></html>");
}
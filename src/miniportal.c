#include "EmbeddableWebServer.h"

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

int PORT = 8080;

void handleClearAndExit(int signal) {
	char command[256];
	
	// keep authed sessions ?
	// sprintf(command, "ipset flush portal");
	// system(command);
	
	sprintf(command, "iptables -t nat -D PREROUTING ! -d 192.168.0.0/16 "
		"-p tcp -m set ! --match-set portal src -j REDIRECT --to-port %d", PORT);
	system(command);
	
	exit(0);
}

int main(int argc, char* argv[]) {
	char command[256];
	char* v;
	
	v = getenv("PORT");
	if( v ) {
		PORT = atoi(v);
	}
	
	sprintf(command, "iptables -t nat -I PREROUTING 1 ! -d 192.168.0.0/16 "
		"-p tcp -m set ! --match-set portal src -j REDIRECT --to-port %d", PORT);
	system(command);
	
	signal(SIGHUP, handleClearAndExit);
	signal(SIGINT, handleClearAndExit);
	signal(SIGQUIT, handleClearAndExit);
	signal(SIGTERM, handleClearAndExit);
	
	system("ipset create portal hash:ip");
	return acceptConnectionsUntilStoppedFromEverywhereIPv4(NULL, PORT);
}

struct Response* createResponseForRequest(const struct Request* request, struct Connection* connection) {
	// printf("Host %s connected\n", connection->remoteHost);
	if (0 == strcmp(request->pathDecoded, "/auth")) {
		char command[128];
		sprintf(command, "ipset add portal %s", connection->remoteHost);
		system(command);
		int status = 0;
		return responseAllocWithFormat(200, "OK", "application/json", "{\"status\": %d}", status);
	}
	/* Serve files from the current directory */
	if (request->pathDecoded == strstr(request->pathDecoded, "/__files__")) {
		return responseAllocServeFileFromRequestPath("/", request->path, request->pathDecoded, ".");
	}
	return responseAllocHTML("<!DOCTYPE html><html lang=\"zh-CN\">"
		"<head><meta charset=\"utf-8\"><title>Welcome</title><link rel=\"stylesheet\" href=\"/__files__/app.css\"></head>"
		"<body><div id=\"root\"></div></body><script src=\"/__files__/app.js\"></script></html>");
}
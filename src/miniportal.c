#include "EmbeddableWebServer.h"

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

int PORT = 8080;

void clean() {
	char command[256];

	system(command);
	sprintf(command, "iptables -t nat -F portal_prerouting");
	system(command);
	sprintf(command, "iptables -t nat -D PREROUTING -j portal_prerouting");
	system(command);
}

void handleClearAndExit(int signal) {
	clean();	
	exit(0);
}

int main(int argc, char* argv[]) {
	clean();

	char command[256];
	char* v;
	
	v = getenv("PORT");
	if( v ) {
		PORT = atoi(v);
	}

	system("ipset create portal_wl hash:ip");
	sprintf(command, "iptables -t nat -N portal_prerouting");
	system(command);
	sprintf(command, "iptables -t nat -I PREROUTING 1 -j portal_prerouting");
	system(command);
	sprintf(command, "iptables -t nat -A portal_prerouting -m set --match-set portal_wl src -j RETURN");
	system(command);
	sprintf(command, "iptables -t nat -A portal_prerouting -j REDIRECT --to-port %d", PORT);
	system(command);
	
	signal(SIGHUP, handleClearAndExit);
	signal(SIGINT, handleClearAndExit);
	signal(SIGQUIT, handleClearAndExit);
	signal(SIGTERM, handleClearAndExit);	
	
	return acceptConnectionsUntilStoppedFromEverywhereIPv4(NULL, PORT);
}

// why not use ipset hash:mac ? Because some router ROM build do not include hash:mac ipset support

struct Response* createResponseForRequest(const struct Request* request, struct Connection* connection) {
	// printf("Host %s connected\n", connection->remoteHost);
	char req_mac[32] = {0}, buf[128], FILE* pipe, size_t len;

	if (0 == strcmp(request->pathDecoded, "/auth")) {
		sprintf(buf, "cat /proc/net/arp | grep %s | awk '{print $4}'", connection->remoteHost);
		pipe = popen(buf, "r");
		if( !pipe ) {
			return responseAllocWithFormat(503, "Error", "application/json", "{}");
		}
		if( ! (len = fread(buf, sizeof(buf), 1, pipe) ) ) {
			return responseAllocWithFormat(400, "Error", "application/json", "{}");
		}
		buf[len] = 0;
		printf("Got req mac: %s\n", buf);
		return responseAllocWithFormat(200, "OK", "application/json", "{}");
	}
	/* Serve files from the current directory */
	if (request->pathDecoded == strstr(request->pathDecoded, "/__files__")) {
		return responseAllocServeFileFromRequestPath("/", request->path, request->pathDecoded, ".");
	}
	return responseAllocHTML("<!DOCTYPE html><html lang=\"zh-CN\">"
		"<head><meta charset=\"utf-8\"><title>Welcome</title><link rel=\"stylesheet\" href=\"/__files__/app.css\"></head>"
		"<body><div id=\"root\"></div></body><script src=\"/__files__/app.js\"></script></html>");
}
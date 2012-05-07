#include <iostream>
#include <string>
#include <cstring>
#include <server.h>
#include <cstdio>
#include <cstdlib>

extern "C" 
{
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
}

using namespace std;

char **argv_;

void help() {
	printf("Usage:\n");
	printf("\t%s [config file]\n",*argv_);
	printf("\t%s -help\n",*argv_);
	printf("\t%s -default\n",*argv_);
}

void cfhelp() {
	printf("Config file:\n");
	printf("Format: [OPTION] [VALUE]\n");
	printf("        [OPTION] [VALUE] etc.\n");
	printf("Options:\n");
	printf("\tteamname\n");
	printf("\tserverport\n");
	printf("\tcontrolserverport\n");
	printf("\tcontrolserverip\n");
	printf("\tmaxclients\n");
	printf("-default:\n");
	printf("\tteamname           victor\n");
	printf("\tserverport         2003\n");
	printf("\tcontrolserverport  2001\n");
	printf("\tcontrolserverip    146.50.4.35 (deze)\n");
	printf("\tmaxclients         100\n");
}


int main(int argc, char * argv[])
{  
	argv_=argv; 
	if (argc != 2) {
		help();
		return 1;
	}
	char buffer[200];
	int pid = getpid(), maxclients = 100;
	unsigned short csport=2001;
	unsigned short port=2003;
	string csip = "146.50.4.35"; 
	string teamname = "victor";

	if (!strcmp(argv[1], "-default")) {
	} else
	if (!strcmp(argv[1], "-help")) {
		cfhelp();
		return 0;
	} else {
		FILE * fd = fopen(argv[1], "r");
		if (!fd) {
			printf("Config file does not exist\n");
			return 1;
		}
		while(fgets(buffer, 200, fd)) {
			char * t = strtok(buffer, "= \n");
			if (!strcmp(t, "teamname")) {
				t = strtok(NULL, "= \n");
				teamname = t;
				if (teamname.length() > 20) {
					printf("Error, Teamname too large\n");
					return 1;
				}
			}
			if (!strcmp(t, "serverport")) {
				t = strtok(NULL, "= \n");
				port = atoi(t);
				if (!port) {
					printf("Error, serverport not valid");
					return 1;
				}
			}
			if (!strcmp(t, "controlserverport")) {
				t = strtok(NULL, "= \n");
				csport = atoi(t);
				if (!csport) {
					printf("Error, controlserverport not valid");
					return 1;
				}
			}
			if (!strcmp(t, "controlserverip")) {
				t = strtok(NULL, "= \n");
				csip = t;
				if (csip.length() > 20) {
					printf("Error, controlserverip not valid");
					return 1;
				}
			}
			if (!strcmp(t, "maxclients")) {
				t = strtok(NULL, "= \n");
				maxclients = atoi(t);
				if (maxclients <= 0) {
					printf("Error, maxclients not valid");
					return 1;
				}
			}
		}
	}
	string ident;
	sprintf(buffer, "T%sS%d...", teamname.c_str(), pid);
	ident = buffer;
	FILE * rfd = fopen("/dev/urandom", "r");
	unsigned int rand;
	while(ident.length() < 32) {
		fread(&rand, sizeof rand, 1, rfd);
		rand = rand % 10;
		sprintf(buffer, "%s%d", ident.c_str(), rand);
		ident = buffer;
	}
	fclose(rfd);
	Server server (port, csip, csport, ident, maxclients);
    server.start();
    return 0;     
}

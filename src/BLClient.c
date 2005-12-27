#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "BLhelper.h"

/*  Global constants  */

#define MAX_LINE           (100000)


/*  Function declarations  */

int ParseCmdLine(int argc, char *argv[], char **szAddress, char **szPort);
void print_usage();


int main(int argc, char *argv[]) {

    int       conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *szAddress;             /*  Holds remote IP address   */
    char     *szPort;                /*  Holds remote port         */
    char     *endptr;                /*  for strtol()              */


    /*  Get command line arguments  */

    ParseCmdLine(argc, argv, &szAddress, &szPort);


    /*  Set the remote port  */

    port = strtol(szPort, &endptr, 0);
    if ( *endptr ) {
	printf("BLClient: Invalid port supplied.\n");
	exit(EXIT_FAILURE);
    }
	

    /*  Create the listening socket  */

    if ( (conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
	fprintf(stderr, "BLClient: Error creating listening socket.\n");
	exit(EXIT_FAILURE);
    }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(port);


    /*  Set the remote IP address  */

    if ( inet_aton(szAddress, &servaddr.sin_addr) <= 0 ) {
	printf("BLClient: Invalid remote IP address.\n");
	exit(EXIT_FAILURE);
    }

    
    /*  connect() to the remote server  */

    if ( connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 ) {
	printf("BLClient: Error calling connect()\n");
	exit(EXIT_FAILURE);
    }


    fgets(buffer, MAX_LINE, stdin);
    

    /*  Send string to the server, and retrieve response  */

    Writeline(conn_s, buffer, strlen(buffer));
    Readline(conn_s, buffer, MAX_LINE-1);


    printf("%s", buffer);

   /*  Close the connected socket  */

   if ( close(conn_s) < 0 ) {
     fprintf(stderr, "BLClient: Error calling close()\n");
     exit(EXIT_FAILURE);
   }


    return EXIT_SUCCESS;
}

void print_usage(){

     fprintf(stderr,"Usage:\n");
     fprintf(stderr,"BLClient -a (remote IP) -p (remote port)\n");
     exit(EXIT_SUCCESS);

}

int ParseCmdLine(int argc, char *argv[], char **szAddress, char **szPort) {

    int n = 1;

    if(argc == 1){
      print_usage();
    }


    while ( n < argc ) {
	if ( !strncmp(argv[n], "-a", 2) ) {
	    *szAddress = argv[++n];
	}
	else if ( !strncmp(argv[n], "-p", 2) ) {
	    *szPort = argv[++n];
	}
	else if ( !strncmp(argv[n], "-h", 2) ) {
            print_usage();
	}
	++n;
    }

    return 0;
}


#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define ECHOMAX 255     /* Longest string to echo */

void DieWithError(char *errorMessage);  /* External error handling function */

typedef struct {

     enum {toPigLatin, toEnglish}  requestType;     /* same size as an unsigned int */ 

    unsigned int requestID;                                    /* unique identifier for each request */

    char fromPhrase[256];                                              /* phrase to be translated */

} translateRequest;    

typedef struct {

    unsigned int requestID;                                    /* unique identifier for each request */

    char toPhrase[256];                                              /* translated phrase */

} translateResponse;  

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned int echoServPort;       /* Echo server port */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    char *servIP;                    /* IP address of server */
    char *echoString;                /* String to send to echo server */
    char echoBuffer[ECHOMAX+1];      /* Buffer for receiving echoed string */
    int echoStringLen;               /* Length of string to echo */
    int respStringLen;               /* Length of received response */
	int userChoice;
	char temp;
	translateRequest *client = malloc(sizeof(translateRequest));
	translateResponse server;
	
    
    if ((argc < 4) || (argc > 5))    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <Server IP> <requestType> <requestID> <phrase> [<Echo Port>]\n", argv[0]);
        exit(1);
    }
    servIP = argv[1];           /* First input: server IP address (dotted quad) */
	
	if (argc == 4)
        echoServPort = atoi(argv[2]);  /* Use given port, if any */
    else
        echoServPort = 7;  /* 7 is the well-known port for the echo service */
	
	client->requestID = atoi(argv[3]);		/* Request ID*/
	
	
	do{
		printf("\n 1 for toPigLatin translation, 2 for FromPigLatin translation, 3 for exit: - ");
		scanf("%d", &userChoice); 					/* reading user choice*/
				
		scanf("%c",&temp); // temp statement to clear buffer		
		
		// Assigning values to enum depending on user choice
		if (userChoice==1)
			client->requestType=toPigLatin;
		else if(userChoice==2)
			client->requestType=toEnglish;
		else if(userChoice==3)
			exit(0);	
		else
			DieWithError("Wrong requestType selected");
		
		printf("\n Enter the phrase :- ");
		scanf("%[^\n]",client->fromPhrase);	/* reading the phrase to be converted */
		client->fromPhrase[strlen(client->fromPhrase)] ='\0';
	
		if ((echoStringLen = strlen(client->fromPhrase)) > ECHOMAX)  /* Check input length */
			DieWithError("Echo word too long");
    
		/* Create a datagram/UDP socket */
		if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
			DieWithError("socket() failed");
    
		/* Construct the server address structure */
		memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
		echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
		echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
		echoServAddr.sin_port   = htons(echoServPort);     /* Server port */
		
		/* Send the string to the server */
		printf("generating message with requestID %d \n",client->requestID);
		   /* Send the string to the server */
		sendto(sock, client, sizeof(*client), 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) ;
			
    
		/* Recv a response */
		fromSize = sizeof(fromAddr);
		respStringLen = recvfrom(sock, &server, sizeof(server), 0,(struct sockaddr *) &fromAddr, &fromSize);
	
		if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr )
		{
			fprintf(stderr,"Error: received a packet from unknown source.\n");
			exit(1);
		}

		if (client->requestID != server.requestID )
		{
			fprintf(stderr,"Error: request ID does not match.\n");
			exit(1);
		}
    
		/* null-terminate the received data */
		server.toPhrase[respStringLen] = '\0';
		printf("Received: %s\n", server.toPhrase);    /* Print the echoed arg */
		} while(userChoice != 3);
    close(sock);
    exit(0);
}

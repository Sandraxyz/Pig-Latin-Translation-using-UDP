#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
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

char* replaceWord(const char* s, const char* oldW,
                  const char* newW)
{
    char* result;
    int i, cnt = 0;
    int newWlen = strlen(newW);
    int oldWlen = strlen(oldW);
  
    // Counting the number of times old word
    // occur in the string
    for (i = 0; s[i] != '\0'; i++) {
        if (strstr(&s[i], oldW) == &s[i]) {
            cnt++;
  
            // Jumping to index after the old word.
            i += oldWlen - 1;
        }
    }
  
    // Making new string of enough length
    result = (char*)malloc(i + cnt * (newWlen - oldWlen) + 1);
  
    i = 0;
    while (*s) {
        // compare the substring with the result
        if (strstr(s, oldW) == s) {
            strcpy(&result[i], newW);
            i += newWlen;
            s += oldWlen;
        }
        else
            result[i++] = *s++;
    }
  
    result[i] = '\0';
    return result;
}


char * convert_to_english(char str[])
{
	strcat(str, " ");
	char* result = NULL;
    result = replaceWord(str, "yay ", " ");
    char* result2 = NULL;
    result2 = replaceWord(result, "ay ", " "); 
    char *pch,*answer;
    int length,tempLength;
	char temp[ECHOMAX];
    pch = strtok (result2," ,.-");

    while (pch != NULL)
    {
        length = strlen(pch);
		tempLength=strlen(temp);
		temp[tempLength] = pch[length-1];
        pch[strlen(pch)-1] = ' ';
        strcat(temp, pch);
        pch = strtok (NULL, " ,.-");
    }
	
	answer = temp;
	
	return answer;
}

//Function to check whether character is vovel or consonanr
int isVowel(char c){
	if(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U')
		return 1;
	else 
		return 0;
}
char* convert_to_pig_latin(char str[])
{
	char input[ECHOMAX];
	char *answer;
	char current_char = ' ', previous_char;
	char first_character_of_word = '?';
	int input_index = 0, str_index = 0;
	strcpy(input, str);
	while(1)
	{
		previous_char = current_char;
		current_char = input[input_index];
        input_index++;
        if(' ' == current_char || '\n' == current_char || '.' == current_char || '\0' == current_char)
        {
            if(previous_char != ' ' && previous_char != '\n' && previous_char != '.')
            {
                if(isVowel(first_character_of_word)){
                    str[str_index] = first_character_of_word;
                    str_index++;
                    str[str_index] = 'y';
                    str_index++;
                    str[str_index] = 'a';
                    str_index++;
                    str[str_index] = 'y';
                    str_index++;
                }
                else{
                    str[str_index] = first_character_of_word;
                    str_index++;
                    str[str_index] = 'a';
                    str_index++;
                    str[str_index] = 'y';
                    str_index++;
                }
            }
            str[str_index] = current_char;
            str_index++;
            if('\0' == current_char)
                break;
        }
        else{
            if(' ' == previous_char || '\n' == previous_char || '.' == previous_char)
                first_character_of_word = current_char;
            else
            {
                str[str_index] = current_char;
                str_index++;
            }
		}
    }
	answer = str;
	return answer;
}



int main(int argc, char *argv[])
{
    int sock;                        /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */	
    char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */
	translateRequest client;
	translateResponse *server = malloc(sizeof(translateRequest));
	
    if (argc != 2)         /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage:  %s <UDP SERVER PORT>\n", argv[0]);
        exit(1);
    }
    
    echoServPort = atoi(argv[1]);  /* First arg:  local port */
    
    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");
    
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */
    
    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");
    
    for (;;) /* Run forever */
    {
    	printf("Waiting...\n");
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(echoClntAddr);
		      
        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock, &client, sizeof(client), 0,
                                    (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
            DieWithError("recvfrom() failed");
		printf("Received request from  %s \n", inet_ntoa(echoClntAddr.sin_addr));	
		server->requestID = client.requestID;	
		
		printf("Message received: %d %s \n", client.requestID, client.fromPhrase);
		if(client.requestType == toPigLatin){	
			strcpy(server->toPhrase,convert_to_pig_latin(client.fromPhrase));
		} else {
			//convert_to_english(client.fromPhrase);
			strcpy(server->toPhrase,convert_to_english(client.fromPhrase));
		}	
		printf("Translated message: %s \n", server->toPhrase);			
      
        
        /* Send received datagram back to the client */
        if (sendto(sock, server, sizeof(*server), 0,
                   (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != recvMsgSize){
         //   DieWithError("sendto() sent a different number of bytes than expected");
		}
		printf("Message sent.: \n");			
    }
    /* NOT REACHED */
}
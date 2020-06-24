#include<arpa/inet.h>
#include<ctype.h>
#include<libgen.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>	
#include<sys/socket.h>
#include<unistd.h>

#define BUFFER_SIZE 4096
#define LINE_SIZE 4096

//Keep track of the number of bytes sent
ssize_t total = 0;

//Function declaration
void sendFile(FILE *filePointer, int mySocket);

int main(int argc , char *argv[]){

	char* fileName;
    char* filePath;
    FILE *filePointer;
    char* hashingAlgorithm;
    int index;
    char* ipAddress;
    int mySocket;
    ssize_t numBytesRecv;
    char sendBuffer[BUFFER_SIZE] = {0};
    char recvBuffer[BUFFER_SIZE] = {0};
    int optionChar; 
    unsigned long portNum = 2345;
    struct sockaddr_in server;
    opterr = 0;

	//Input validation for the number of arguments
	if (argc < 4){
        puts("Usage: ./hashClient [-p port] <IP Address> <Hashing Algorithm> <File Path>\n");
        puts("Example: ./hashClient [-p 2345] 127.0.0.1 sha1 /Desktop/file.txt\n");
        return 1;
    }

    //Input validation for optional port number and additional arguments
    while ((optionChar = getopt (argc, argv, "p:")) != -1)
      switch (optionChar){
        case 'p':
            portNum = strtoul(optarg, NULL, 10);
            if (portNum <= 0 || portNum >= 65536){
                puts("Please enter a valid port number (0-65535).");
                return 1;
            }
          break;
        case '?':
          if (optopt == 'p')
            fprintf (stderr, "Option -%c requires an argument.\n", optopt);
          else if (isprint (optopt))
            fprintf (stderr, "Unknown option `-%c'.\n", optopt);
          else
            fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
          return 1;
        default:
          abort ();
    }
    ipAddress = argv[optind];
    hashingAlgorithm = argv[optind + 1];

    //Make sure that the IP address and hashing algorithm variables aren't null
    if (ipAddress == NULL){
        puts("Something is wrong with the IP Address.\n");
        return 1;
    }
    if (hashingAlgorithm == NULL){
        puts("Something is wrong with the Hashing Algorithm.\n");
        return 1;
    }

	return 0;
}

void sendFile(FILE *filePointer, int mySocket){

    int numBytes; 
    char line[LINE_SIZE] = {0}; 
   
   //Read and send 4096 bytes of the file at a time to the server
    while ((numBytes = fread(line, sizeof(char), LINE_SIZE, filePointer)) > 0){
        total = total + numBytes;
        if (numBytes != LINE_SIZE && ferror(filePointer)){
            perror("The file can't be read.\n");
            exit(1);
        }   
        if (send(mySocket, line, numBytes, 0) == -1){
            perror("The file can't be sent.\n");
            exit(1);
        }
        //Reset the buffer each time for retransmission
        memset(line, 0, LINE_SIZE);
    }
}
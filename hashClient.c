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

    printf("Connecting to %s:%lu\n", ipAddress, portNum);

    //Create socket
    mySocket = socket(AF_INET , SOCK_STREAM , 0);
    if (mySocket == -1){
        perror("The socket could not be created.\n");
    }
        
    //Configure the IP address, port and protocol (TCP)
    server.sin_addr.s_addr = inet_addr(ipAddress);
    server.sin_family = AF_INET;
    server.sin_port = htons(portNum);

    //Connect to the hashing server
    if (connect(mySocket, (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("There is an error with the connection.\n");
        return 1;
    }

    //Copy the hashing algorithm into the buffer
    strncpy(sendBuffer, hashingAlgorithm, strlen(hashingAlgorithm));

    //Send the hashing algorithm and reset the buffer
    if (send(mySocket, sendBuffer, BUFFER_SIZE, 0) == -1){
        perror("The hashing algorithm can't be sent.\n");
        return 1;
    }
    memset(sendBuffer, 0, BUFFER_SIZE);

     //Iterate through the filepath argument(s)
    //Send file size(s), file name(s) and the file(s)
    for (index = optind + 2; index < argc; index++){
        filePath = argv[index];

        //Make sure the file path is not null
        if (filePath == NULL){
            puts("Something is wrong with the file path.\n");
            return 1;
        }

        //Open the file
        filePointer = fopen(filePath, "rb");
        if (filePointer == NULL){
            perror("The file can't be opened.\n");
            return 1;
        }

        //Get the file size
        fseek(filePointer, 0L, SEEK_END);
        int myFileSize = ftell(filePointer);
        fseek(filePointer, 0L, SEEK_SET);

        //Copy the file size into the buffer
        sprintf(sendBuffer, "%d", myFileSize);

        //Send the file size and reset the buffer
        if (send(mySocket, sendBuffer, BUFFER_SIZE, 0) == -1){
            perror("The file size can't be sent.\n");
            return 1;
        }
        memset(sendBuffer, 0, BUFFER_SIZE);

        //Cut the filename from the file path and copy into the buffer
        fileName = basename(filePath);
        strncpy(sendBuffer, fileName, strlen(fileName));
    
        //Send the file name and reset the buffer
        if (send(mySocket, sendBuffer, BUFFER_SIZE, 0) == -1){
            perror("The file name can't be sent.\n");
            return 1;
        }
        memset(sendBuffer, 0, BUFFER_SIZE);

        //Send the file
        sendFile(filePointer, mySocket);
    
        //Receive the hash back from the server and print to stdout
        numBytesRecv = recv(mySocket, recvBuffer, BUFFER_SIZE, 0);
        if (numBytesRecv < 0){
          perror("The file can't be received.\n");
          return 1;
        }
        printf("%s %s\n", recvBuffer, fileName);
        fclose(filePointer);        
    }

    //Close the file and the socket connection
    close(mySocket);

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
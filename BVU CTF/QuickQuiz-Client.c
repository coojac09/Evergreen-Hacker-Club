#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#define BUFF_SIZE 257

// General purpose: show error and quit routine
void die(const char* str) {
    printf("Error: %s\n\tErrno(%d) %s\n", str, errno, strerror(errno));
    exit(1);
}

void sendStr(int fd, char* msg) {
    uint8_t len = (uint8_t)strlen(msg);
    write(fd, &len, sizeof(len));
    write(fd, msg, len);
}
int recvStr(int fd, char* buf) {
    uint8_t len;
    if (read(fd, &len, sizeof(len)) <= 0)
        return -1;
    read(fd, buf, (size_t)len);
    buf[len]='\0';
    return 0;
}


int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Usage: %s <host name>\n", argv[0]);
        exit(1);
    }

    struct hostent *he = gethostbyname(argv[1]);
    if (!he) {
        printf("Couldn't find host\n");
        exit(1);
    }

    signal(SIGPIPE, SIG_IGN);

    // Get a file descriptor for our socket
    int sockFD;
    if ((sockFD = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        die("Could not obtain file descriptor for socket");

    // Specify connection info
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)he->h_addr,
            (char *)&server_address.sin_addr.s_addr,
            he->h_length);
    server_address.sin_port = htons(26000); // Server port
 // Establish connection to server
    if (connect(sockFD, (struct sockaddr*)(&server_address), sizeof(server_address)) < 0)
        die("Failed to establish connection to server");



    char *userInput = NULL;
    char serverMsg[BUFF_SIZE];


    while( recvStr(sockFD, serverMsg) != -1 ) {
        if ( serverMsg[ strlen(serverMsg)-1 ] == '!' ) {
            // Exiting scenario
            printf("%s\n", serverMsg);
            break;
        } else if ( serverMsg[ strlen(serverMsg)-1 ] == ':' ) {
            // Get input from the user
            printf("%s ", serverMsg);

            // Get input from the user
            size_t len;
            getline(&userInput, &len, stdin);

            // Truncate string if it is larger than server can handle
            // Then send it to the server
            if (len > 255)
                userInput[255] = '\0';
            sendStr(sockFD, userInput);

            // Release memory allocated by getline
            free(userInput);
            userInput = NULL;
        } else {
            // Just print it and get more
            printf("%s\n", serverMsg);
        }

    }

    close(sockFD);
    return 0;
}

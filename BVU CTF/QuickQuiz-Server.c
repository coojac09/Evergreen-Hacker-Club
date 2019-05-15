#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>

#define BUFF_SIZE 257

// General purpose: show error and quit routine
void die(const char* str) {
    printf("Error: %s\n\tErrno(%d) %s\n", str, errno, strerror(errno));
    exit(1);
}

// Send strings to the user. Strings are prefixed with a 1-byte length
void sendStr(int fd, char* msg) {
    uint8_t len = (uint8_t)strlen(msg);
    write(fd, &len, sizeof(len));   // Write the 1-byte length
    write(fd, msg, len);            // Write the len-byte msg
}

// Get strings from the user. Strings are prefixed with a 1-byte length
int recvStr(int fd, char* buf) {
    uint8_t len;
    if (read(fd, &len, sizeof(len)) <= 0) // get 1-byte length
        return -1;

    read(fd, buf, (size_t)len);           // get len-byte msg

    buf[len]='\0'; // Null-terminate it ourselves since length-prefixed
    return 0;
}

// User strings are sent w/ newlines. Replaces them with something else.
// '?' for back-and-forth, '\0' to terminate early, or anything else.
void replaceNewline(char *str, char ch) {
    str[strlen(str)-1] = ch;
}

struct ClientData {
    int isAdmin;
    char inputData[BUFF_SIZE];
};


int main(int argc, char** argv) {

    signal(SIGPIPE, SIG_IGN);


    // Get file desriptor handle for socket
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Specify TCP info
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(26000);

    // Allow reuse of socket address (after server is killed w/o properly closing)
    int yes=1;
    if (setsockopt(listenSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1)
        die("Unable to set socket flags");

    // Start listening for client connections
    if (bind(listenSocket, (struct sockaddr*)(&server_address), sizeof(server_address)) != 0)
        die("Failed to bind socket");

    // Listen on socket
    listen(listenSocket, 128);

    char clientInput[BUFF_SIZE];

    // Loop for many clients -- kill server with: control-c
    while (1) {

        // Wait for and receive new connection
        int clientConn = accept(listenSocket, NULL, NULL);
        if (clientConn == -1)
            die("Failed to accept client connection");
        fflush(stdout);

        signal(SIGCHLD, SIG_IGN);
        if (fork() == 0) {

            // Prepare strings for use
            char flag[16];
            FILE *flagFile = fopen("/etc/flags/A", "r");
            fscanf(flagFile, "%s", flag);
            fclose(flagFile);

            char password[16];
            FILE *passFile = fopen("password.txt", "r");
            fscanf(passFile, "%s", password);
            fclose(passFile);

            struct ClientData client;
            client.isAdmin = 0;


            sendStr(clientConn, "Answer questions, get the flag.");


            sendStr(clientConn, "Which Steve, Woz or Jobs?:");
            recvStr(clientConn, client.inputData);
            replaceNewline(client.inputData, '\0');
            if (strcmp(client.inputData, "Woz") == 0) {
                sendStr(clientConn, "Correct.");
            } else {
                sendStr(clientConn, "Wrong. Goodbye!");
                exit(0);
            }


            sendStr(clientConn, "Soda of champions?:");
            recvStr(clientConn, client.inputData);
            replaceNewline(client.inputData, '\0');
            if (strcmp(client.inputData, "cream soda") == 0) {
                sendStr(clientConn, "Correct.");
            } else {
                sendStr(clientConn, "Wrong. Goodbye!");
                exit(0);
            }


            sendStr(clientConn, "What is the admin password?:");
            recvStr(clientConn, client.inputData);
            replaceNewline(client.inputData, '\0');
            if (strcmp(client.inputData, password) == 0) {
                client.isAdmin = 1;
            } else {
            }
            sendStr(clientConn, "We'll see...");


            sendStr(clientConn, "Ready to quit yet?:");
            recvStr(clientConn, client.inputData);
            replaceNewline(client.inputData, '?');
            sendStr(clientConn, client.inputData);
            sendStr(clientConn, "Did you think you even had a choice? Next Question.");


            sendStr(clientConn, "pc^2 or kattis?:");
            recvStr(clientConn, client.inputData);
            replaceNewline(client.inputData, '\0');
            if (strcmp(client.inputData, "kattis") == 0) {
                sendStr(clientConn, "Correct.");
            } else {
                sendStr(clientConn, "Wrong. Goodbye!");
                exit(0);
            }


            sendStr(clientConn, "Master of x86?:");
            recvStr(clientConn, client.inputData);
            replaceNewline(client.inputData, '\0');
            if (strcmp(client.inputData, "Michael Abrash") == 0) {
                sendStr(clientConn, "Correct.");
            } else {
                sendStr(clientConn, "Wrong. Goodbye!");
                exit(0);
            }


            if (client.isAdmin) {
                sendStr(clientConn, "Welcome, admin. Here is your flag.");
                sendStr(clientConn, flag);
            } else {
                sendStr(clientConn, "Better luck next time.");
            }

            sendStr(clientConn, "Goodbye!");


            close(clientConn);
            exit(0);
        }
    }


    return 0;
}





#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>
#define LISTENQ 1024
#define MAXLINE 1024
void sig_child(int signo) // signal handler function to decrement the clients number and print the terminated child
{
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("[+]Child %d terminated.\n", pid);
}
void proc_data(int connfd)
{
    char buff[MAXLINE];
    int n;
    while ((n = read(connfd, buff, MAXLINE)) > 0)
    {

        buff[n] = 0;
        printf("[+]Word recived: %s", buff);
        int i = 0;
        while (buff[i])
        {
            buff[i] = toupper(buff[i]);
            i++;
        }
        int n = write(connfd, buff, strlen(buff));
        if (n < 0)
        {
            fprintf(stderr, "sending error");
        }
        printf("Capital version sent: %s", buff);
    }
}
int main(int argc, char const *argv[])
{
    if (argc < 1)
    {
        fprintf(stderr, "Usage: %s <PORT> ", argv[0]);
        exit(0);
    }
    int socketfd, bindd, lis, connfd;
    struct sockaddr_in server, client;
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "[-]Error in connection.\n");
        exit(0);
    }
    printf("[+]Server Socket is created.\n");
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if ((bindd = bind(socketfd, (const struct sockaddr *)&server, sizeof(server))) < 0) // binding to a given port and any available interface.
    {
        fprintf(stderr, "[-]Error in binding..Port Already in Use.\n");
        exit(0);
    }
    printf("[+]Bind to port %s\n", argv[1]);
    if ((lis = listen(socketfd, LISTENQ)) < 0) // listening on the port and ip that we bind to them
    {
        fprintf(stderr, "[-]Error in binding.\n");
        exit(0);
    }
    printf("[+]Listening....\n");
    pid_t pid;
    while (1)
    {
        socklen_t l = sizeof(client);
        connfd = accept(socketfd, (struct sockaddr *)&client, &l);
        if (connfd < 0)
        {
            fprintf(stderr, "[-]connection error");
            exit(0);
        }
        printf("[+]Connection accepted from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        signal(SIGCHLD, sig_child);
        if ((pid = fork()) == 0) // client
        {

            close(socketfd);   // close the listen socket
            proc_data(connfd); // process the data on this child process
            close(connfd);
            exit(0);
        }
        // server
        close(connfd); // close the connected socket to handle other clients
    }

    return 0;
}

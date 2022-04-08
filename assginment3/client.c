#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#define MAXLINE 8192
//ِName: Anas shlool, SN:129390
//ِName: Malik Tawfiq, SN:123202
//ِName: Laith al-Faouri, SN:126341

int maxpone(int a, int b)
{
    int maximum = 0;
    if (a > b)
        maximum = a;
    else
        maximum = b;
    return maximum + 1;
}
int main(int argc, char *argv[])
{
    if (argc < 2) // if there are less than 2 arguments print the usage error.
    {
        fprintf(stderr, "Usage: %s <IP> <PORT> ", argv[0]);
        exit(0);
    }
    struct sockaddr_in client;
    int socketfd, connfd;
    memset(&client, 0, sizeof(client));
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 3) // opening the socket , must be bigger than 3 (0->in,1->out,2->error) so any thing less than 3 is an error.
    {
        fprintf(stderr, "[-]Error in connection.\n");
        exit(0);
    }
    printf("[+]Client Socket is created.\n");
    if ((inet_pton(AF_INET, argv[1], &client.sin_addr)) <= 0) // get the IPv4 as argument  and convert it to binary network order stream
    {
        fprintf(stderr, "%s : ip address %s error ", argv[0], argv[1]);
        exit(0);
    }
    client.sin_port = htons(atoi(argv[2]));
    client.sin_family = AF_INET;
    if ((connfd = connect(socketfd, (const struct sockaddr *)&client, sizeof(client))) < 0) // connect to the client to the socket
    {
        fprintf(stderr, "[-]Error in connection.\n");
        exit(0);
    }
    printf("[+]Connected to Server.\n");
    int maxfdp1, sel;
    fd_set rset;
    int stdincheck = 0;
    FD_ZERO(&rset);
    char send[MAXLINE], recv[MAXLINE];
    while (1)
    {
        FD_SET(socketfd, &rset);
        if (stdincheck == 0)
            FD_SET(fileno(stdin), &rset);
        maxfdp1 = maxpone(fileno(stdin), socketfd);
        sel = select(maxfdp1, &rset, NULL, NULL, NULL);
        if (sel < 0)
        {
            fprintf(stderr, "[-]Some interrupt happens..");
        }
        if (FD_ISSET(socketfd, &rset))
        {

            int n;
            if ((n = read(socketfd, recv, MAXLINE)) == 0)
            {
                if (stdincheck == 1)
                    return;
                else
                {
                    fprintf(stderr, "[-]Server Crashed...");
                    exit(1);
                }
            }
            recv[n] = 0;
            printf("The capital version of your string: %s", recv);
        }

        if (FD_ISSET(fileno(stdin), &rset))
        {
            if (fgets(send, MAXLINE, stdin) == NULL)
            {
                stdincheck = 1;
                shutdown(socketfd, SHUT_WR);
                FD_CLR(fileno(stdin), &rset);
                continue;
            }
            write(socketfd, send, strlen(send));
        }
    }

    return 0;
}

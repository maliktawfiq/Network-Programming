#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#define MAXLINE 8192
//ِName: Anas shlool, SN:129390
//ِName: Malik Tawfiq, SN:123202
//ِName: Laith al-Faouri, SN:126341
int main(int argc, char const *argv[])
{
    int socketfd, BindCheck;
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 3)
    {
        perror("[-]Socket Problem..");
        exit(1);
    }
    printf("[+]Server Socket Created..\n");
    struct sockaddr_in client, server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(atoi(argv[1]));
    if ((BindCheck = bind(socketfd, (struct sockaddr *)&server, sizeof(server))) < 0)
    {
        perror("[-]Binding Error...");
        exit(1);
    }
    printf("[+]listening...\n");
    char recvline[MAXLINE];
    int n, m;
    int Clientlength;
    int countword;
    int charcount;
    while (1)
    {
        bzero(recvline, sizeof(recvline));
        Clientlength = sizeof(client);
        n = recvfrom(socketfd, recvline, MAXLINE, 0, (struct sockaddr *)&client, &Clientlength);
        if (n < 0)
        {
            perror("[-]Reciveing Error..");
            exit(1);
        }
        recvline[n] = 0;
        printf("[+]Line Recived from %s on port %d: %s\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), recvline);
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("ERROR while forking new process.\n");
            exit(1);
        }
        if (pid == 0)
        {
            countword = 1;
            charcount = -1;
            for (int i = 0; i < strlen(recvline); i++)
            {
                if (recvline[i] == ' ' && recvline[i + 1] != ' ')
                    countword++;
                else
                    charcount++;
            }
            char count[500];
            sprintf(count, "number of words: %d\nnumber of characters: %d", countword, charcount);
            m = sendto(socketfd, count, strlen(count), 0, (struct sockaddr *)&client, Clientlength);
            if (m < 0)
            {
                perror("[-]sending error");
            }
            printf("[+]Response Sent....\n");
            close(socketfd);
            exit(0);
        }
    }
    return 0;
}
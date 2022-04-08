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
    int socketfd;
    struct sockaddr_in server, conn;

    if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 3)
    {
        perror("[-]Socket Problem..");
        exit(1);
    }
    printf("[+]Client Socket Created..\n");
    server.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &server.sin_addr);
    server.sin_port = htons(atoi(argv[2]));
    char sendline[MAXLINE], recvline[MAXLINE];
    int connlen;
    printf("[+]Enter a lines To count the Characters And words: \n");
    while ((fgets(sendline, MAXLINE, stdin)) != NULL)
    {
        int n;
        n = sendto(socketfd, sendline, strlen(sendline), 0, (struct sockaddr *)&server, sizeof(server));
        if (n < 0)
        {
            perror("[-]Sending Error..");
            exit(1);
        }
        printf("[+]Line Sent succssfully...\n");
        connlen = sizeof(conn);
        n = recvfrom(socketfd, recvline, MAXLINE, 0, (struct sockaddr *)&conn, &connlen);
        printf("[+]response Recived from %s on port %d\n", inet_ntoa(conn.sin_addr), ntohs(conn.sin_port));
        if (n < 0)
        {
            perror("[-]Receiving Error..");
        }
        recvline[n] = 0;
        printf("Response: \n%s\n", recvline);
    }

    return 0;
}

#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#define LISTENQ 1500
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c1 = PTHREAD_COND_INITIALIZER;
struct clientinfo
{
    int connfd;
    struct sockaddr_in client;
};
int clients = 0;
void *proc(void *arg)
{

    struct clientinfo c = *((struct clientinfo *)arg);
    free(arg);
    int size;
    int request = 0;
    int n;
    while ((n = recvfrom(c.connfd, &size, sizeof(int), 0, NULL, NULL)) > 0)
    {

        printf("[+]Size Recived: %d\n", size);
        request = request + 1;
        printf("Request number #%d from %s\n", request, inet_ntoa(c.client.sin_addr));
        int reciv[size];
        int temp[size];
        if ((n = recvfrom(c.connfd, &reciv, size * sizeof(int), 0, NULL, NULL)) < 0)
        {
            printf("[-]recieving error");
        }
        reciv[size] = 0;
        for (int i = 0; i < size; i++)
        {
            temp[size - 1 - i] = reciv[i];
        }
        temp[size] = 0;
        n = sendto(c.connfd, &temp, size * sizeof(int), 0, (struct sockaddr *)&c.client, sizeof(c.client));
        if (n < 0)
        {
            perror("[-]Sending Error..");
            exit(1);
        }
        printf("[+]Array Sent succssfully...\n");
    }
    pthread_mutex_lock(&m1);
    clients--;
    printf("[+]Client terminates, Clients Number: %d\n", clients);
    if (clients <= 4)
    {
        pthread_cond_signal(&c1);
    }
    close(c.connfd);
    pthread_mutex_unlock(&m1);
}
int main(int argc, char const *argv[])
{
    struct sockaddr_in server;

    int socketfd;
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 3)
    {
        perror("[-]Creating socket error");
        exit(1);
    }
    printf("[+]Socket Created Successfully...\n");
    memset(&server, 0, sizeof(server));
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    if ((bind(socketfd, (struct sockaddr *)&server, sizeof(server))) < 0)
    {
        perror("[-]Bind Error");
        exit(1);
    }
    printf("[+]Bind to port %s\n", argv[1]);
    if ((listen(socketfd, LISTENQ)) < 0)
    {
        perror("[-]Error in binding.\n");
        exit(0);
    }
    printf("[+]Listening....\n");

    struct clientinfo *c;
    int status;
    while (1)
    {
        pthread_t tid;

        c = malloc(sizeof(c));
        socklen_t l = sizeof(c->client);
        c->connfd = accept(socketfd, (struct sockaddr *)&c->client, &l);

        clients++;
        printf("[+]Connection accepted from %s:%d\n", inet_ntoa(c->client.sin_addr), ntohs(c->client.sin_port));
        pthread_mutex_lock(&m1);
        if (clients > 4)
        {
            printf("Waiting for any client to terminate\n");
            while (clients > 4)
            {
                pthread_cond_wait(&c1, &m1);
                printf("Client terminates, Clients Number: %d\n", clients);
            }
        }
        else
            printf("Clients Number: %d\n", clients);

        pthread_mutex_unlock(&m1);

        pthread_create(&tid, NULL, &proc, c);
    }
    close(socketfd);
    return 0;
}

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
int size = 0;
struct test
{
    int socketfd;
};
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c1 = PTHREAD_COND_INITIALIZER;
void *threadwork(void *arg)
{
    pthread_detach(pthread_self());

    pthread_mutex_lock(&m1);
    struct test t = *((struct test *)arg);
    while (1)
    {
        size = 0;
        while (size == 0)
            pthread_cond_wait(&c1, &m1);
        int reciv[size];
        int n;
        if (((n = recvfrom(t.socketfd, &reciv, size * sizeof(int), 0, NULL, NULL))) > 0)
        {
            // reciv[n] = 0;
            printf("Your reversed array is: ");
            for (int i = 0; i < size; i++)
            {
                printf("%d ", reciv[i]);
            }
            printf("\n");
            pthread_mutex_unlock(&m1);
        }
    }
}
int main(int argc, char const *argv[])
{
    struct sockaddr_in server;
    struct test t;
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <IP> <port>", argv[0]);
        exit(1);
    }

    if ((t.socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 3)
    {
        perror("[-]Socket");
        exit(1);
    }
    printf("[+]Socket Created Successfully...\n");
    server.sin_family = AF_INET;
    if ((inet_pton(AF_INET, argv[1], &server.sin_addr.s_addr)) < 0)
    {
        perror("[-]IPv4 error");
        exit(1);
    }
    server.sin_port = htons(atoi(argv[2]));
    if ((connect(t.socketfd, (const struct sockaddr *)&server, sizeof(server))) < 0)
    {
        perror("[-]Error in connection.\n");
        exit(1);
    }
    printf("[+]Connected to Server.\n");
    pthread_t tid;
    if ((pthread_create(&tid, NULL, &threadwork, &t)) != 0)
    {
        fprintf(stderr, "[-]threading error..");
        exit(1);
    }

    while (1)
    {

        int choice, n;

        printf("\t\tOptions:\n");
        printf("\t\t\t1. Send New Request\n\t\t\t2. exit\n");
        printf("Please Enter Your choice: \n");
        scanf("%d", &choice);

        if (choice == 1)
        {
            pthread_mutex_lock(&m1);
            printf("Please Enter Array size: ");
            scanf("%d", &size);
            if (size > 0)
                pthread_cond_signal(&c1);
            pthread_mutex_unlock(&m1);
            n = sendto(t.socketfd, &size, sizeof(int), 0, (struct sockaddr *)&server, sizeof(server));
            if (n < 0)
            {
                perror("[-]Writing error");
                exit(1);
            }

            printf("[+]Size Sent succssfully...\n");
            int arr[size];
            printf("Enter array elemants: ");
            for (int i = 0; i < size; i++)
            {
                scanf("%d", &arr[i]);
            }
            printf("[+]Array Sent succssfully...\n");
            printf("Your array befor reversing: ");
            for (int i = 0; i < size; i++)
            {
                printf("%d ", arr[i]);
            }
            printf("\n");
            n = sendto(t.socketfd, &arr, size * sizeof(int), 0, (struct sockaddr *)&server, sizeof(server));
            if (n < 0)
            {
                perror("[-]Writing error");
                exit(1);
            }
        }
        else if (choice == 2)
            break;
        else
            printf("Please Try Again and enter a right choice 1 or 2 easy ^^.\n");
        sleep(2);
    }
    shutdown(t.socketfd, SHUT_WR);
    return 0;
}

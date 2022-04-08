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
#define LISTENQ 1024
#define MAXLINE 1024
int clients = 0;
int count_spaces(char *str) //count the spaces on a given string
{
    int count = 0;
    for (int i = 0; i <= str[i]; i++)
    {
        if (str[i] == ' ')
        {
            count++;
        }
    }
    return count;
}
char *spaces(char *str) // a function to remove white spaces from a string .
{
    int i, len = 0, j;
    len = sizeof(str) / sizeof(str[0]);
    for (i = 0; i < len; i++)
    {
        if (str[i] == ' ')
        {
            for (j = i; j < len; j++)
            {
                str[j] = str[j + 1];
            }
            len--;
        }
    }
    return str;
}

void sig_child(int signo) // signal handler function to decrement the clients number and print the terminated child
{
    clients--;
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("[+]Child %d terminated.\n", pid);
}
void proc_data(int connfd, struct sockaddr_in client)
{
    time_t t;
    time(&t);
    FILE *connlog = fopen("connlog.log", "ab+"); //open a log file and create it (if not exist)
    fprintf(connlog, "Connection Date&time: %s\n", ctime(&t));
    int n;
    char buff[MAXLINE];
    while ((n = read(connfd, buff, MAXLINE)) > 0) // read a line from the client
    {
        bool tmp = false; // tmp boolean variable to check if the operation success or not .
        char buffer[50];
        buff[n] = 0;
        char *num;
        int sumEVN = 0, sumODD = 0;
        int number;
        int check = count_spaces(buff);
        char *operation;
        operation = strtok(buff, " ");
        if (check < 2) // check the number of spaces if its bigger than 2 that means there is more than one number if not ,send error message to client
        {
            char err[] = "Enter more than one number (:";
            int q = write(connfd, err, strlen(err));
            if (q < 0)
            {
                fprintf(stderr, "[-]sending error");
                exit(0);
            }
        }
        else
        {
            tmp = true;

            while ((num = strtok(NULL, " ")) != NULL) // cutting the string into spaces and take the numbers
            {
                number = atoi(num);
                if (number % 2 == 0)
                {
                    sumEVN += number;
                }
                else
                {
                    sumODD += number;
                }
            }
            operation = spaces(operation);
            if ((strcmp("EVN", operation) == 0)) // return the odd summation or even , depend on the operation on the first word of the line or error if its invalid op. .
            {
                char result[200];
                sprintf(result, "%d", sumEVN);
                n = write(connfd, result, strlen(result));
                if (n < 0)
                {
                    fprintf(stderr, "sending error");
                }
            }
            else if ((strcmp("ODD", operation) == 0))
            {
                char result[200];
                sprintf(result, "%d", sumODD);
                int n = write(connfd, result, strlen(result));
                if (n < 0)
                {
                    fprintf(stderr, "sending error");
                }
            }
            else
            {
                tmp = false;
                char err[] = "Unsupported operation";
                int n = write(connfd, err, strlen(err));
                if (n < 0)
                {
                    fprintf(stderr, "[-]sending error");
                }
            }
        }
        char succ[50];
        if (tmp)
        {
            strcpy(succ, "YES");
        }
        else
        {
            strcpy(succ, "NO");
        }

        //logging every request from the client
        fprintf(connlog, "IpAddress: %s\nOperation: %s\nSuccess(yes or no): %s\n---------------------------------------------------\n", inet_ntop(AF_INET, &client.sin_addr.s_addr, buffer, sizeof(buffer)), operation, succ);
    }
}

int main(int argc, char *argv[])
{
    int fd, bindd, lis, connfd;
    struct sockaddr_in server, client;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "[-]Error in connection.\n");
        exit(0);
    }
    printf("[+]Server Socket is created.\n");
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if ((bindd = bind(fd, (const struct sockaddr *)&server, sizeof(server))) < 0) //binding to a given port and any available interface.
    {
        fprintf(stderr, "[-]Error in binding..Port Already in Use.\n");
        exit(0);
    }
    printf("[+]Bind to port %s\n", argv[1]); //print the binding port
    if ((lis = listen(fd, LISTENQ)) < 0)     // listening on the port and ip that we bind to them
    {
        fprintf(stderr, "[-]Error in binding.\n");
        exit(0);
    }
    printf("[+]Listening....\n");
    int i = 0;
    pid_t pid;
    while (true)
    {

        socklen_t l = sizeof(client);
        connfd = accept(fd, (struct sockaddr *)&client, &l);
        if (connfd < 0)
        {
            fprintf(stderr, "[-]connection error");
            exit(0);
        }
        printf("[+]Connection accepted from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        signal(SIGCHLD, sig_child); // signal handler installation to count the clients number
        clients++;                  // if the excution come to this point so we had a new client .
        if (clients > 4)            //checking the number of clients to print blocking statment
        {
            printf("[+]Blocking Until Any Client terminate....\n");
        }
        while (clients > 4) // if the number of clients bigger than 4 stay in the loop and block the fork until a signal decreamnt the counter and fork
            ;
        printf("clients num: %d\n", clients);
        if ((pid = fork()) == 0) //client
        {

            close(fd);                 // close the listen socket
            proc_data(connfd, client); //process the data on this child process
            close(connfd);
            exit(0);
        }
        //server
        close(connfd); //close the connected socket to handle other clients
    }

    return 0;
}
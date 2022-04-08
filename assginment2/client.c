#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define MAXLINE 8192
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
void read_send(FILE *fp, int fd, int wait)
{
    ssize_t n;
    size_t line_buff_size = 0;
    char *buff;

    while ((n = getline(&buff, &line_buff_size, fp)) != EOF) // read lines until End Of File (-1)
    {
        buff[n] = 0; //add the null terminated at the end of every line.
        char recive[50];
        n = write(fd, buff, strlen(buff)); // write the line on the socket to the server
        if (n < 0)
        {
            fprintf(stderr, "[-]Error in sending data.\n");
        }
        n = read(fd, recive, MAXLINE); // read the response from the server
        recive[n] = 0;                 //add the null terminated at the end of every line recevied .
        if (n < 0)
        {
            fprintf(stderr, "[-]Error in receiving data.\n");
        }
        char *operation = spaces(strtok(buff, " ")); // formating the response from The server and print it
        if ((strcmp(operation, "EVN")) == 0 || (strcmp(operation, "ODD")) == 0)
        {
            printf("the SUM%s =  %s\n", operation, recive);
        }
        else
        {
            printf("%s\n", recive);
        }
        usleep(wait); // sleep between subsequences.
    }
}

int main(int argc, char *argv[])
{
    if (argc < 4) //if there are less than 4 arguments print the usage error.
    {
        fprintf(stderr, "Usage: %s <IP> <PORT> <wait-time> <File>", argv[0]);
        exit(0);
    }
    int fd, conn;
    struct sockaddr_in client;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 3) // opening the socket , must be bigger than 3 (0->in,1->out,2->error) so any thing less than 3 is an error.
    {
        fprintf(stderr, "[-]Error in connection.\n");
        exit(0);
    }
    printf("[+]Client Socket is created.\n");
    client.sin_family = AF_INET;
    if ((inet_pton(AF_INET, argv[1], &client.sin_addr)) <= 0) // get the IPv4 as argument  and convert it to binary network order stream
    {
        fprintf(stderr, "%s : ip address %s error ", argv[0], argv[1]);
        exit(0);
    }
    client.sin_port = htons(atoi(argv[2]));                                         //take the port and convert it to network order
    int wait_time = atoi(argv[3]);                                                  // take the wait time between subsequence and convert it to integer
    FILE *fp = fopen(argv[4], "r");                                                 // read from a given file
    if ((conn = connect(fd, (const struct sockaddr *)&client, sizeof(client))) < 0) //connect to the client to the socket
    {
        fprintf(stderr, "[-]Error in connection.\n");
        exit(0);
    }
    printf("[+]Connected to Server.\n");
    read_send(fp, fd, wait_time); //caling the function to process data and send it
    printf("[+]Mission Completed Disconnected from server.\n");
    close(fd);

    return 0;
}

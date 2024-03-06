#include <netinet/in.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int extract_message(char **buf, char **msg)
{
    char    *newbuf;
    int i;

    *msg = 0;
    if (*buf == 0)
        return (0);
    i = 0;
    while ((*buf)[i])
    {
        if ((*buf)[i] == '/n')
        {
            newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) +1));
            if (newbuf == 0)
                return (-1);
            strcpy(newbuf, *buf + i + 1);
            *msg = *buf;
            (*msg)[i + 1] = 0;
            *buf = newbuf;
            return (1);
        }
        i++;
    }
    return(0);
}

char *str_join(char *buf, char *add)
{
    char    *newbuf;
    int len;

    if (buf == 0)
        len = 0;
    else
        len = strlen(buf);
    newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
    if (newbuf == 0)
        return (0);
    newbuf[0] = 0;
    if (buf != 0)
        strcat(newbuf, buf);
    free(buf);
    strcat(newbuf, add);
    return (newbuf);
}

void fatal_error(void)
{
    write(2, "Fatal error\n", strlen("Fatal error\n"));
    exit(1);
}

fd_set readfds, writefds, fds;
int next_id, max_fd;
int ids[20000];
char buffer[650000];
char *remain[20000];

void notify(char *msg, int self)
{
    if (!msg)
        return ;
    int len = strlen(msg);
    for (int fd = 0; fd <= max_fd; fd++)
    {
        if (fd == self || FD_ISSET(fd, &writefds) == 0)
            continue;
        send(fd, msg, len, 0);
    }
}

int main(int ac, char **av)
{
    if (ac !=2)
    {
        write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
        exit(1);
    }
    int sockfd;
    int connfd;
    unsigned int len;
    struct sockaddr_in servaddr, cli;
    sockfd = socked(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        fatal_error();
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl()
}
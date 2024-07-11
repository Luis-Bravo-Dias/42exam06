#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

int maxSock;
char *msg = NULL;

int allCLiId[5000];
char *cliBuff[5000];

char buff_send[1001];
char buff_read[1001];

fd_set rd_set, wrt_set, atv_set;

void ft_error(char *s)
{
    perror(s);
    exit(1);
}

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
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
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

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

void    send_msg(int fd)
{
    for (int sockId = 3; sockId <= maxSock; sockId++)
    {
        if(FD_ISSET(sockId, &wrt_set) && sockId != fd)
        {
            send(sockId, buff_send, strlen(buff_send), 0);
            if (msg)
                send(sockId, msg, strlen(msg), 0);
        }
    }
}


int main(int ac, char **av)
{
	if (ac != 2)
        ft_error("Wrong number of arguments\n");
    
    int sockfd, connfd, cliId;
    cliId = 0;
	struct sockaddr_in servaddr, cliaddr; 
	bzero(&servaddr, sizeof(servaddr));
    socklen_t len_cli = sizeof(cliaddr);

	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //127.0.0.1
	servaddr.sin_port = htons(atoi(av[1])); 
	
    // socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd < 0)
        ft_error("Fatal error\n");
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) < 0)
        ft_error("Fatal error\n");
	if (listen(sockfd, SOMAXCONN) < 0)
        ft_error("Fatal error\n");
    
    maxSock = sockfd;
    FD_ZERO(&atv_set);
    FD_SET(sockfd, &atv_set);

    while(1)
    {
        rd_set = wrt_set = atv_set;
        if (select(maxSock + 1, &rd_set, &wrt_set, 0, 0) <= 0)
            continue;
        if (FD_ISSET(sockfd, &rd_set))
        {
	        connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len_cli);
            if (connfd < 0)
                ft_error("Fatal error\n");
            FD_SET(connfd, &atv_set);
            maxSock = (connfd > maxSock) ? connfd : maxSock;
            allCLiId[connfd] = cliId++;
            sprintf(buff_send, "server: client %d just arrived\n", allCLiId[connfd]);
            send_msg(connfd);
            cliBuff[connfd] = 0;
            continue;
        }

        for(int sockId = 3; sockId <= maxSock; sockId++)
        {
            if (FD_ISSET(sockId, &rd_set) && sockId != sockfd)
             {  
                int rd = recv(sockId, buff_read, 1000, 0);
                if (rd <= 0)
                {
                    FD_CLR(sockId, &atv_set);
                    sprintf(buff_send, "server: client %d just left\n", allCLiId[sockId]);
                    send_msg(sockId);
                    if (cliBuff[sockId] != 0)
                        free(cliBuff[sockId]);
                    close(sockId);
                }
                else
                {
                    buff_read[rd] = 0;
                    cliBuff[sockId] = str_join(cliBuff[sockId], buff_read);
                    msg = 0;
                    while (extract_message(&cliBuff[sockId], &msg))
                    {
                        sprintf(buff_send, "client %d: ", allCLiId[sockId]);
                        send_msg(sockId);
                        if (msg)
                        {
                            free(msg);
                            msg = 0;
                        }
                    }
                }
            }
        }
    }

    return (0);
}
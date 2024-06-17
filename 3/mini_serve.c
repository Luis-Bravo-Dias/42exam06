#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

int maxSock;
char *msg = NULL;

int	g_cliId[5000];
char	*cliBuff[5000];

char	buff_sd[1001];
char	buff_rd[1001];

fd_set rd_set, wrt_set, atv_set;

void	ft_error(char *str)
{
	perror(str);
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
			newbuf = calloc(strlen(*buf + i + 1) + 1, sizeof(*newbuf));
			if (!newbuf)
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

char	*str_join(char *buff, char *add)
{
	int len_buff = (buff == 0) ? 0 : strlen(buff);
	int	len_add = (add == 0) ? 0 : strlen(add);

	char *new_str = malloc(sizeof(*new_str) * (len_buff + len_add + 1));
	if (!new_str)
		return (0);
	new_str[0] = 0;
	if (buff)
	{
		strcat(new_str, buff);
		free(buff);
	}
	if (add)
		strcat(new_str, add);
	return (new_str);
}

void	send_msg(int fd)
{
	for (int sockId = 3; sockId <= maxSock; sockId++)
	{
		if (FD_ISSET(sockId, &wrt_set) && sockId != fd)
		{
			send(sockId, buff_sd, strlen(buff_sd), 0);
			if (msg)
				send(sockId, msg, strlen(msg), 0);
		}
	}
}


int main(int	ac, char **av) {
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(8081); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n");
	if (listen(sockfd, 10) != 0) {
		printf("cannot listen\n"); 
		exit(0); 
	}
	len = sizeof(cli);
	connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
	if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server acccept the client...\n");
}
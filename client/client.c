/* Author : Malola Simman Srinivasan Kannan
 *  Date : 23 April 2023
 *  mail id : masr4788@colorado.edu
 *  file name : client.c
 */

/* header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <syslog.h>

/* macros */
#define MAX 80
#define PORT 9000
#define SA struct sockaddr
#define FILE_PATH "/home/socketdata"

/* function definition */
void recv_pressure_data(int sockfd, int fd)
{
        char buffer[1024];
        int bytes_received=0;
        int write_status=0;
         while ((bytes_received = recv(sockfd, buffer, 1024, 0)) > 0)
        {
                write_status = write(fd, buffer, bytes_received);
                if(write_status == -1)
		{
			syslog(LOG_ERR,"Could'nt write total bytes to the file");
			exit(6);
		}
        }   
        if(bytes_received == -1)
        {
                syslog(LOG_ERR, "Error: recv() with code, %d", errno);
        }   
}

/* main function */
int main()
{
        /* initialization */
        int sockfd, connfd;
        char *ip_addr ="10.0.0.173";
        struct sockaddr_in server_addr;

        //open connection for sys logging, ident is NULL to use this Program for the user level messages
        openlog(NULL, LOG_CONS | LOG_PID | LOG_PERROR, LOG_USER);
        
        //create socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) 
        {
                syslog(LOG_ERR, "fail to create socket");
                exit(0);
        }
        else
                syslog(LOG_DEBUG,"Socket successfully created");
        bzero(&server_addr, sizeof(server_addr));
	
	
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(ip_addr);
        server_addr.sin_port = htons(PORT);
	/*
	if (inet_pton(AF_INET, "10.0.0.173", &server_addr.sin_addr)<= 0) 
	{
		printf("Address not supports");
		exit(2);
	}
	*/
        // connect the client socket to server socket
        connfd = connect(sockfd, (SA*)&server_addr, sizeof(server_addr));

        if (connfd <0) 
        {
                syslog(LOG_ERR, "fail to connect with the server");
                exit(0);
        }
        else
                syslog(LOG_DEBUG, "connected to the server");
        
        //open or create file
        int fd =open(FILE_PATH, (O_RDWR|O_CREAT|O_APPEND),0766);
	if (fd == -1)
	{
		syslog(LOG_ERR,"The file could not be created/found");
		exit(4);
        }
        
        //receive data and write it to file
        recv_pressure_data(sockfd, fd);
        
        //close the file descriptor and the socket
        close(fd);
        close(sockfd);
        return 0;
}


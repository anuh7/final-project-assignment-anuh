#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>


int signal_interrupt = 0;
int server_socket_fd;
int new_fd;

struct sockaddr_in their_addr;
struct addrinfo hints, *server_info;



void signal_handler(int signum)
{
    if (signum == SIGINT) 
    {
        syslog(LOG_DEBUG, "Caught signal SIGINT, exiting!!");
        signal_interrupt = 1;
    }
    else if( signum == SIGTERM)
    {
        syslog(LOG_DEBUG, "Caught signal SIGTERM, exiting!!");
        signal_interrupt = 1;
    }
    //close fds
    exit(0);
}


int daemon_creation()
{
    	pid_t pid;
	pid = fork();
	
	if (pid == -1)
	{
		syslog(LOG_ERR, "fork failed in daemon process");
		exit(-1);
	}
	
	if (pid > 0)
	{
		exit(0);
	}
	
	if (setsid() == -1)
		return -1;
	
	if (chdir("/") == -1)
		return -1;
	
	open ("/dev/null", O_RDWR); 
	dup (0);
	dup (0); 
	
	return 0;
}


int main(int argc, char *argv[])
{
    int ret; 
    const int yes = 1;
    socklen_t addr_size=sizeof(struct sockaddr);
    
    openlog(NULL, 0, LOG_USER);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);


    bool is_daemon = false;

    if( argv[1] == NULL )
    {
        is_daemon =  false;
    }
    else if( strcmp(argv[1], "-d") == 0)
    {
        is_daemon = true;
    }
    else
    {
	syslog(LOG_ERR, "invalid argument");
	return -1;
    }
 
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if( server_socket_fd == -1 )
    {
        syslog(LOG_ERR, "Error: socket() with code, %d", errno);
        exit(1);
    }
    printf("created socket\n");
   
    hints.ai_flags = AI_PASSIVE;

    ret = getaddrinfo(NULL, "9000", &hints, &server_info);
    printf("getaddrinfo\n");
    
    if(ret != 0)
    {
        syslog(LOG_ERR, "Error: getaddrinfo() with code, %d", errno);
        exit(1);
    }

    
    ret = setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    
    if (ret != 0)
    {
    	syslog(LOG_ERR, "Error: setsocketopt() with code, %d", errno);
        exit(1);
    }
    printf("setsockopt\n"); 
    
    //int flags = fcntl(server_socket_fd, F_GETFL);
    //fcntl(server_socket_fd, F_SETFL, flags | O_NONBLOCK);
    
    ret = bind(server_socket_fd, server_info->ai_addr, sizeof(struct sockaddr));

    if(ret != 0)
    {
        syslog(LOG_ERR, "Error: bind() with code, %d", errno);
        exit(1);
    }
    printf("bind\n");

    freeaddrinfo(server_info);

    if(is_daemon)
    {
        if(daemon_creation() != 0)
        {
            syslog(LOG_ERR, "Daemon creation failed");
        }
    }


    ret = listen(server_socket_fd, 1);
    if(ret != 0)
    {
        syslog(LOG_ERR, "Error: listen() with code, %d", errno);
        exit(1);
    }
    printf("listen success\n");

    ret = accept(server_socket_fd, (struct sockaddr *)&their_addr, &addr_size);
    if(ret  == -1)
    {
            syslog(LOG_ERR, "Error : accept with error no : %d", errno);
            exit(1);
    }
    else
    {
    	    syslog(LOG_INFO, "Accepted connection from %s", inet_ntoa(their_addr.sin_addr));
    	    printf("Accepted connected from %s\n", inet_ntoa(their_addr.sin_addr));
    }
    
    
    while(!signal_interrupt)
    {
   	 send(new_fd,"\n Server is working" , sizeof("\n Server is working"), 0);
   	 printf("Inside while loop\n");
    }
    
    close(server_socket_fd);
}
  

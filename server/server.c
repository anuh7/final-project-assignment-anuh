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
   // char ip_string[INET_ADDRSTRLEN];
    
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
 
    struct sockaddr their_addr;
    socklen_t addr_size = sizeof(their_addr);
    struct addrinfo hints, *server_info;
    
    int server_socket_fd;
    int new_fd;// bytes_sent;   ///CHANGE THESE

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;  
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; 
    hints.ai_protocol = 0;    


    int ret;

    ret = getaddrinfo(NULL, "9000", &hints, &server_info);
    if( ret !=0 )
    {
        syslog(LOG_ERR, "Error: getaddrinfo() with code, %d", errno);
       // return(1);
    }

    server_socket_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if( server_socket_fd == -1 )
    {
        syslog(LOG_ERR, "Error: socket() with code, %d", errno);
      //  return(1);
    }

    const int yes = 1;
    if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
    {
    	syslog(LOG_ERR, "Error: setsocketopt() with code, %d", errno);
    //  return(1);
    }
    
    int flags = fcntl(server_socket_fd, F_GETFL);
    fcntl(server_socket_fd, F_SETFL, flags | O_NONBLOCK);
    
    ret = bind(server_socket_fd, server_info->ai_addr, server_info->ai_addrlen);
    if( ret != 0 )
    {
        syslog(LOG_ERR, "Error: bind() with code, %d", errno);
        //return(1);
    }

    freeaddrinfo(server_info);

    if( is_daemon )
    {
        if(daemon_creation() != 0)
        {
            syslog(LOG_ERR, "Daemon creation failed");
        }
    }


    ret = listen(server_socket_fd, 10);
    if( ret != 0 )
    {
        syslog(LOG_ERR, "Error: listen() with code, %d", errno);
    }

    new_fd = accept(server_socket_fd, &their_addr, &addr_size);
    if(new_fd  == -1)
        {
            if(errno == EWOULDBLOCK)
            {
          		//return(1);
            }
            syslog(LOG_ERR, "Error : accept with error no : %d", errno);
            //return(1);
        }
    
   // char send_data[] = "Server is working";
    
    while(!signal_interrupt)
    {
    //int bytes_sent = 
    send(new_fd,"\n Server is working" , sizeof("\n Server is working"), 0);
    }
    
    close(server_socket_fd);
}
  

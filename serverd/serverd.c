 /*
 Attributions: https://github.com/davebm1/c-bme280-pi 
 */

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

#include <time.h>
#include <sys/time.h>


#include "compensation.h"

#include <linux/i2c-dev.h>
#include <math.h>
#include <sys/ioctl.h>


#define DEV_PATH "/dev/i2c-1"


int fd = 0;
uint8_t pressure_data[8];			
    
int32_t press_int = 0;
int32_t temp_int = 0;
int32_t hum_int = 0;
    
double station_press = 0.0;
double tire_press = 0.0;
double humidity = 0.0;
       
int signal_interrupt = 0;
int server_socket_fd;
int new_fd;

char *str_pressure = NULL;
char *str_packet = NULL;

struct sockaddr_in their_addr;
struct addrinfo hints, *server_info;


int bme_init(void){
  
    if ((fd = open(DEV_PATH, O_RDWR)) < 0) {			//opening file for i2c device
        return 1;
    }

    if (ioctl(fd, I2C_SLAVE, 0x77) < 0) {			//setting device address
        close(fd);
        return 1;
    }
   
    char config[2] = {0};
    config[0] = 0xE0;
    config[1] = 0xB6;
    write(fd, config, 2);		//device reset
    usleep(50000);

    setCompensationParams(fd);

	
    config[0] = 0xF2;
    config[1] = 0x01;
    write(fd, config, 2);	//turning humidity on
   
   
    config[0] = 0xF5;
    config[1] = 0x00;
    write(fd, config, 2);
   
    config[0] = 0xF4;
    config[1] = 0x25;
    write(fd, config, 2);	//0b00100101: temperature on, pressure-oversampling*1, forced mode

    return 0;
}

void bme_values(){
	
     char config[2] = {0};
     sleep(1);

     char reg[1] = {0xF7};
     write(fd, reg, 1);
     read(fd, pressure_data, 8);

   
     config[0] = 0xF4;
     config[1] = 0x25;
     write(fd, config, 2);
        
     temp_int = (pressure_data[3] << 16 | pressure_data[4] << 8 | pressure_data[5]) >> 4;		//temp value
     press_int = (pressure_data[0] << 16 | pressure_data[1] << 8 | pressure_data[2]) >> 4;		//pressure value
     hum_int = pressure_data[6] << 8 | pressure_data[7];						//humidity value
        
     BME280_compensate_T_double(temp_int);

     station_press = BME280_compensate_P_double(press_int) / 100.0;
     tire_press = station_press * (0.0145038);

     //printf("Tyre pressure: %.2f PSI", tire_press);
     //printf("humidity: %.2f %%rH\n", BME280_compensate_H_double(hum_int));
     humidity = BME280_compensate_H_double(hum_int);
}


void cleanup_function(){
    printf("Closed socket communication");	
    free(str_pressure);
    free(str_packet);
    close(server_socket_fd);
    close(new_fd);
}



void signal_handler(int signum)
{
    if (signum == SIGINT) 
    {
        syslog(LOG_DEBUG, "Caught signal SIGINT, exiting!!");
        cleanup_function();
        signal_interrupt = 1;
    }
    else if( signum == SIGTERM)
    {
        syslog(LOG_DEBUG, "Caught signal SIGTERM, exiting!!");
        cleanup_function();
        signal_interrupt = 1;
    }    
    //exit(0);
}



int main(int argc, char *argv[])
{
    int ret; 
    const int yes = 1;
    socklen_t addr_size=sizeof(struct sockaddr);
    
    openlog(NULL, 0, LOG_USER);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
 
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

    ret = listen(server_socket_fd, 1);
    if(ret != 0)
    {
        syslog(LOG_ERR, "Error: listen() with code, %d", errno);
        exit(1);
    }
    printf("listen success\n");

    new_fd = accept(server_socket_fd, (struct sockaddr *)&their_addr, &addr_size);
    if(ret  == -1)
    {
            syslog(LOG_ERR, "Error : accept with error no : %d", errno);
            exit(1);
            printf("accept not wokring");
    }
    else
    {
    	    syslog(LOG_INFO, "Accepted connection from %s\n", inet_ntoa(their_addr.sin_addr));
    	    printf("Accepted connected from %s\n", inet_ntoa(their_addr.sin_addr));
    	    //printf("accept working");
    }
    
    int length1 = 0;
    int total_length = 0;
    char *str1 = "Tire pressure:";
    char *str2 = "\n";
    char *str3 = " psi";

    
    bme_init();
    
    while(!signal_interrupt)
    {
   	 bme_values();
   	 
   	 length1 = snprintf(NULL, 0, "%0.2f", tire_press);
   	 str_pressure = malloc(length1+1);
   	 length1 = snprintf(str_pressure, length1+1, "%0.2f", tire_press);
   	 
   	 total_length = length1 + strlen(str1) + strlen(str2) + strlen(str3);
   	 str_packet = malloc(total_length+1);
   	 memset(str_packet, 0, total_length);
   	 
   	 strcat(str_packet, str1);
   	 strcat(str_packet, str_pressure);
   	 strcat(str_packet, str3);
   	 strcat(str_packet, str2);
   	 
   	 printf("Sending data: %s\n", str_packet);
   	 write(new_fd, str_packet, strlen(str_packet));
   	 
	//free(str_pressure);
   	//free(str_packet);
   	sleep(2);	
    }
    return 0;
}
  

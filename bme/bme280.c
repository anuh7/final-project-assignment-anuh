 /*
 Attributions: https://github.com/davebm1/c-bme280-pi 
 */

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "compensation.h"

#define DEV_ID 0x77
#define DEV_PATH "/dev/i2c-1"

int main(void) {
    int fd = 0;
    uint8_t dataBlock[8];
    int32_t temp_int = 0;
    int32_t press_int = 0;
    int32_t hum_int = 0;
    double station_press = 0.0;

    if ((fd = open(DEV_PATH, O_RDWR)) < 0) {
        perror("Unable to open i2c device");
        return 1;
    }

    if (ioctl(fd, I2C_SLAVE, 0x77) < 0) {
        perror("Unable to configure i2c slave device");
        close(fd);
        return 2;
    }

    char config[2] = {0};
    config[0] = 0xE0;
    config[1] = 0xB6;
    write(fd, config, 2);
	
    usleep(50000);

    setCompensationParams(fd);

    config[0] = 0xF2;
    config[1] = 0x01;
    write(fd, config, 2);

    config[0] = 0xF5;
    config[1] = 0x00;
    write(fd, config, 2);

    config[0] = 0xF4;
    config[1] = 0x25;
    write(fd, config, 2);


    while (1) {
        sleep(1);

	char reg[1] = {0xF7};
	write(fd, reg, 1);
	read(fd, dataBlock, 8);

        config[0] = 0xF4;
	config[1] = 0x25;
	write(fd, config, 2);
   
        /* get raw temp */
        temp_int = (dataBlock[3] << 16 | dataBlock[4] << 8 | dataBlock[5]) >> 4;

        /* get raw pressure */
        press_int = (dataBlock[0] << 16 | dataBlock[1] << 8 | dataBlock[2]) >> 4;

        /* get raw humidity */
        hum_int = dataBlock[6] << 8 | dataBlock[7];

        BME280_compensate_T_double(temp_int);

        station_press = BME280_compensate_P_double(press_int) / 100.0;

        printf("Station pressure: %.2f psi \n", station_press);

        printf("Humidity: %.2f %%rH\n", BME280_compensate_H_double(hum_int));
    }

    return 0;
}


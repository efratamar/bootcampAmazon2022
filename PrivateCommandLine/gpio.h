#ifndef GPIO_H
#define GPIO_H
#include"gaz_cam_lib.h"
#include<unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include<pthread.h>
#define RECORD_RATE 200
#define STOP_RECORD_RATE 500
#define SNAPSHOT_RATE -1
#define UPDATE_STATUS 50000
#define GPIONUM 24
void get_status_thread();
void blink(int,int*,int*);
void off_led();
void on_led(int*);

enum STATE{
    ON,
    OFF,
    CONST_ON,
    CONST_OFF
};

typedef struct{
    int* rate;
    int* state;
}blink_status;
#endif // GPIO_H

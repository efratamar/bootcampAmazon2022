#ifndef GPIO_H
#define GPIO_H
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "gaz_cam_lib.h"
#define RECORDRATE 200
#define STOPRECORDRATE 500
#define SNAPSHOTRATE -1
#define CAPTURERATE 0
#define UPDATESTATUS 50000
#define WORKFOREVER 1
#define GPIONUM 26

enum STATE{
    ON,//blink
    OFF,//off blink
    CONST_ON,
    CONST_OFF,
    STOPON,
    STOPOFF
};

typedef struct{
    int* rate;
    int* state;
}blink_status;

int export_gpio();
int unexport_gpio();
void on_led(int);
void off_led();
void* blink(void*);
void get_status_thread();
void main_gpio();


#endif // GPIO_H

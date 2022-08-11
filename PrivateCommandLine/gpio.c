#include "gpio.h"

extern gas_api* p_gaz;
extern p_handler handler;

int export_gpio(){
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/export");
        exit(1);
    }

    if (write(fd, "24", 2) != 2) {
        perror("Error writing to /sys/class/gpio/export");
        exit(1);
    }

    close(fd);

    // Set the pin to be an output by writing "out" to /sys/class/gpio/gpio24/direction

    fd = open("/sys/class/gpio/gpio24/direction", O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/gpio24/direction");
        exit(1);
    }

    if (write(fd, "out", 3) != 3) {
        perror("Error writing to /sys/class/gpio/gpio24/direction");
        exit(1);
    }

    close(fd);
    fd = open("/sys/class/gpio/gpio24/value", O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/gpio24/value");
        exit(1);
    }
    return fd;
}
int unexport_gpio(){
    int fd;
    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/unexport");
        exit(1);
    }
    if (write(fd, "24", 2) != 2) {
        perror("Error writing to /sys/class/gpio/unexport");
        exit(1);
    }
    close(fd);
    return SUCCESS;
}
void on_led(int rate){
    int fd = export_gpio();
    while(WORKFOREVER) {
        if (write(fd, "1", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio24/value");
            exit(1);
        }
        usleep(rate);

        if (write(fd, "0", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio24/value");
            exit(1);
        }
        usleep(rate);
    }
}
void off_led(){
    unexport_gpio();
}
void get_status_thread(){ //app ,0.5
    int status = 0,*state = NULL;
    pthread_t record_thread, snapshot_thread;
    while(WORKFOREVER){
        if(status == p_gaz->get_status(handler))
            continue;
        status=p_gaz->get_status(handler);
        if(status & RECORD_ACTIVE){
            *state = ON;
            pthread_create(&record_thread,NULL,blink,state);
        }
        else if(status & STOP_RECORD_ACTIVE){
            *state = OFF;
            pthread_create(&record_thread,NULL,blink,state);
        }
        else if(status & SNAPSHOT_ACTIVE){
            *state = CONST_ON;
            pthread_create(&snapshot_thread,NULL,blink,state);
        }
        usleep(UPDATESTATUS);
    }
}
void* blink(void* stt){
    int* state = (int*)stt;
    int active = ACTIVE;
    while(active){
        switch (*state) {
        case ON:
        {
            off_led();
            *state = OFF;
            break;
        }
        case OFF:
        {
            on_led(RECORDRATE);
            *state = ON;
            break;
        }
        case STOPON:
        {
            off_led();
            *state = STOPOFF;
            break;
        }
        case STOPOFF:
        {
            on_led(RECORDRATE);
            *state = STOPON;
            break;
        }
        case CONST_ON:
        {
            on_led(SNAPSHOTRATE);
            *state = CONST_OFF;
            break;
        }
        case CONST_OFF:
        {
            off_led();
            active = INACTIVE;
            break;
        }
        }
    }
    return NULL;
}
void main_gpio(){
    get_status_thread();
}

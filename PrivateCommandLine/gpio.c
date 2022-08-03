#include "gpio.h"

extern gas_api* p_gaz;
extern handler_t* handler;

int export()
{
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
int unexport()
{
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
    // And exit
    return 0;
}
void off_led()
{
    unexport();
}
void on_led(int* rate)
{
    int fd=export();
    while(1)
    {
        if (write(fd, "1", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio24/value");
            exit(1);
        }
        usleep(*rate);
        if (write(fd, "0", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio24/value");
            exit(1);
        }
        usleep(*rate);
    }
}
void blink(int gpio_num,int* rate,int* state)
{
    while(*rate){
        switch (*state) {
        case ON:
        {
            off_led();
            *state=OFF;
            break;
        }
        case OFF:
        {
            on_led(rate);
            *state=ON;
            break;
        }
        case CONST_ON:
        {
            on_led(rate);
            *state=CONST_OFF;
            break;
        }
        case CONST_OFF:
        {
            off_led();
            *rate=0;
            break;
        }
        }
    }
}
void get_status_thread()
{
    int status=0;
    int rate=0;
    int state;
    while(1)
    {
        if(status==p_gaz->get_status(handler))
            continue;
        status=p_gaz->get_status(handler);
        switch (status) {
        case RECORD_ACTIVE:
        {
            rate=RECORD_RATE;
            state=ON;
            break;
        }
        case STOP_RECORD_ACTIVE:
        {
            rate=STOP_RECORD_RATE;
            state=ON;
            break;
        }
        case SNAPSHOT_ACTIVE:
        {
            rate=SNAPSHOT_RATE;
            state=CONST_ON;
            break;
        }
        }
        usleep(UPDATE_STATUS);
        blink(GPIONUM,&rate,&state);
    }
}
void main_gpio()
{
    int active=1;
    while(active)
    {
        get_status_thread();

    }
}

#ifndef GAS_LIB_H
#define GAS_LIB_H
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <libavcodec/avcodec.h>
#include "queue.h"
#include "encode.h"
#include <sys/time.h>
#define LENGTH 320
#define WIDTH 240
#define MACROSECONDSPERSECOND 1000000
#define FRAMESPERSECOND 250
#define WAITPERSECONDS MACROSECONDSPERSECOND/FRAMESPERSECOND
#define WAIT 40000
#define DEBUG 0
#define BMPFILEPATH "/home/efrat/Desktop/bmp-test: %s.bmp"
#define JPGFILEPATH "/home/efrat/Desktop/jpg-test: %s.jpg"
#define YFORMULA(R,G,B) (0.257 * R) + (0.504 * G) + (0.098 * B) + 16 + '0'
#define UFORMULA(R,G,B) (0.439 * R) - (0.368 * G) - (0.071 * B) + 128 + '0'
#define VFORMULA(R,G,B) -(0.148 * R) - (0.291 * G) + (0.439 * B) + 128 + '0'
//#define MY_DEBUG
#ifdef MY_DEBUG
#define PRINTF_DBG printf
#else
#define PRINTF_DBG(m...)
#endif

enum STATUS{
    CAPTURE_ACTIVE = 1,
    RECORD_ACTIVE = 2,
    STOP_RECORD_ACTIVE = 4,
    SNAPSHOT_ACTIVE = 8
};

enum RESULT_E{
    ERROR,
    SUCCESS
};

enum ACTIVE_E{
    ACTIVE,
    INACTIVE
};

enum RECORD_STATUS_E{
    VALID,
    FINISHED
};

typedef struct streaming_t{
    int ip;
    int port;
}streaming_t;

typedef struct gpio_t{
    int num;
    int dir;
    int value;
    int is_blink;
    int opertion;
}gpio_t;

typedef struct record_t{
    char* file_name;
    int codec;
    int width;
    int height;
    int fps;
}record_t;

typedef struct snapshot_t{
    char* file_name;
    int width;
    int height;
    int type;
}snapshot_t;

typedef struct {
    int width;
    int height;
    char *data;
    size_t size;
}ppm_image;

typedef struct stage
{
    pthread_t thread_id;
    int is_active;
    Queue* src_queue;
    Queue* dest_queue;
}stage_t,*p_stage;

typedef struct working_amount_for_thread{
    int counter_capture;
    int counter_rgb_converet;
    int counter_yuv_convert;
    int counter_decoder;
    int counter_write;
} working_amount,*p_working_amount;

typedef struct handler
{
    char RGB_static_mat[3][80];
    p_stage stg_capture;
    p_stage stg_rgb_convertor;
    p_stage stg_yuv_convertor;
    p_stage stg_decoder;
    p_stage stg_write;
    int status;
    int record_status;
    working_amount* counter_thread;
}handler_t,*p_handler;

typedef struct yuv{
    char y[WIDTH*LENGTH];
    char u[WIDTH*LENGTH/4];
    char v[WIDTH*LENGTH/4];
}YUV;

typedef struct {
    p_handler (*init_dll)();
    int (*start_record)(p_handler,record_t);
    int (*stop_record)(p_handler);
    int (*start_streaming)(p_handler,streaming_t);
    int (*stop_streaming)(p_handler);
    int (*do_snapshot)(p_handler,snapshot_t);
    int (*get_status)(p_handler);
    int (*get_video_statistic)(p_handler);
    int (*get_dll_version)(p_handler);
}gas_api;

p_handler INIT_DLL();
p_stage init_stage(Queue* ,int);
void init_record(p_handler);
void* capture(void*);
void* rgb_convertor(void*);
void* convert_yuv(void*);
void* encoder(void*);
void* stg_write(void*);
char* snapshot_capture(handler_t*,snapshot_t);
char* snapshot_RGB(handler_t*,char*);
size_t ppm_save(ppm_image*,time_t);
void jpg_save(uint8_t*, time_t, int, int);
int GAS_API_DO_SNAPSHOT(p_handler,snapshot_t);
int GAS_API_GET_DLL_VERSION(p_handler);
int GAS_API_START_RECORD(p_handler,record_t);
int GAS_API_STOP_RECORD(p_handler);
int GAS_API_START_STREAMING(p_handler,streaming_t);
int GAS_API_STOP_STREAMING(p_handler);
int GAS_API_GET_VIDEO_STATISTIC(p_handler);
int GAS_API_GET_STATUS (p_handler);
void initRGB_static_mat(p_handler);
void* thread_tracking(void*);
#endif // GAS_LIB_H

#include "gaz_cam_lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include"queue.h"
#define LENGTH 320
#define WIDTH 240
int GAS_API_GET_DLL_VERSION(p_handler handler)
{
    return 1;
}
int GAS_API_START_RECORD(p_handler handler,record_t record)
{
    pthread_create(handler->stg_capture->thread_id,NULL,capture,handler);
    pthread_create(handler->stg_rgb_convertor->thread_id,NULL,rgb_convertor,handler);
    //pthread_create(handler->stg_yuv_convertor->thread_id,NULL,convert_yuv,handler);
    //pthread_create(handler->stg_decoder->thread_id,NULL,decoder,handler);
    //pthread_create(handler->stg_write->thread_id,NULL,stg_write,handler);
    pthread_join(handler->stg_capture->thread_id,NULL);
    pthread_join(handler->stg_rgb_convertor->thread_id,NULL);
    //pthread_join(handler->stg_yuv_convertor->thread_id,NULL);
    //pthread_join(handler->stg_decoder->thread_id,NULL);
    //pthread_join(handler->stg_write->thread_id,NULL);
    return 1;
}
int GAS_API_STOP_RECORD(p_handler handler)
{
    printf("GAS_API_STOP_RECORD\n");
    return 1;
}
int GAS_API_START_STREAMING(p_handler handler,streaming_t streaming)
{
    return 1;
}
int GAS_API_STOP_STREAMING(p_handler handler)
{
    return 1;

}
int GAS_API_DO_SNAPSHOT(p_handler handler,snapshot_t snapshot)
{
    return 1;

}
int GAS_API_GET_VIDEO_STATISTIC(p_handler handler)
{
    return 1;

}
int GAS_API_GET_STATUS (p_handler handler)
{
    return 1;
}
handler_t * INIT_DLL(){
    handler_t * handler=(handler_t *)calloc(1,sizeof (handler_t));

    handler->stg_capture=(p_stage)calloc(1,sizeof(stage_t));
    handler->stg_capture->src_queue=NULL;
    handler->stg_capture->dest_queue=createQueue(10);

    handler->stg_rgb_convertor=(p_stage)calloc(1,sizeof(stage_t));
    handler->stg_rgb_convertor->src_queue=handler->stg_capture->dest_queue;
    handler->stg_rgb_convertor->dest_queue=createQueue(10);

    handler->stg_yuv_convertor=(p_stage)calloc(1,sizeof (stage_t));
    handler->stg_yuv_convertor->src_queue=handler->stg_rgb_convertor->dest_queue;
    handler->stg_yuv_convertor->dest_queue=createQueue(10);

    handler->stg_decoder=(p_stage)calloc(1,sizeof (stage_t));
    handler->stg_decoder->src_queue=handler->stg_yuv_convertor->dest_queue;
    handler->stg_decoder->dest_queue=createQueue(10);

    handler->stg_write=(p_stage)calloc(1,sizeof (stage_t));
    handler->stg_write->src_queue=handler->stg_decoder->dest_queue;
    handler->stg_write->dest_queue=NULL;
    initRGB_static_mat(handler);
    return handler;
}
void initRGB_static_mat(p_handler handler)
{
    int i=0,x=0,y=0,z=255;
    for ( i=0;i<20 ; i++) {
        handler->RGB_static_mat[0][i]=x;
        handler->RGB_static_mat[1][i]=y;
        handler->RGB_static_mat[2][i]=z;
        y+=10;
    }
    for ( i=20;i<40 ; i++) {
        handler->RGB_static_mat[0][i]=x;
        handler->RGB_static_mat[1][i]=y;
        handler->RGB_static_mat[2][i]=z;
        x+=12;y+=2;z-=8;
    }
    x=255;y=255;z=95;
    for ( i=40;i<60 ; i++) {
        handler->RGB_static_mat[0][i]=x;
        handler->RGB_static_mat[1][i]=y;
        handler->RGB_static_mat[2][i]=z;
        y-=3;z-=5;
    }
    x=255;y=200;z=0;
    for ( i=60;i<80 ; i++) {
        handler->RGB_static_mat[0][i]=x;
        handler->RGB_static_mat[1][i]=y;
        handler->RGB_static_mat[2][i]=z;
        y-=10;
    }
}
void* capture(void* handler){
    p_handler hand=(p_handler)handler;
    int j,k;
    hand->stg_capture->is_active=1;
    while(1){
        int mat[WIDTH][LENGTH];
        for (k=0;k<WIDTH ;k++ ) {
            for (j=0;j<LENGTH ;j++ ) {
                mat[k][j]=rand()%79+0;
            }
        }
        //print
        for (k=0;k<WIDTH ;k++ ) {
            for (j=0;j<LENGTH ;j++ ) {
                printf("mat[%d][%d] = %d \n",k,j,mat[k][j]);
            }
        }
        int *temp_mat = (int *)malloc(WIDTH * LENGTH * sizeof(int));
        memcpy(temp_mat,mat,sizeof (mat));
        for(int i=0;i<WIDTH;i++){
            for(int j=0;j<LENGTH;j++){
                printf("temp_mat[%d][%d]= %d\n",i,j,*(temp_mat + i*320 + j));
            }
        }
        enqueue(hand->stg_capture->dest_queue,createNode(&temp_mat));
        sleep(0.4);
    }
    hand->stg_capture->is_active=0;
}

void* rgb_convertor(void* handler){
    handler_t hand=*(p_handler)handler;
    (&hand)->stg_rgb_convertor->is_active=1;
    int mat[WIDTH][LENGTH][3],temp_mat[WIDTH][LENGTH][3];
    Node * n;
    //declare base mat
    int *base_mat = (int *)malloc(WIDTH * LENGTH * sizeof(int));
    while(1){
        while(isEmpty((&hand)->stg_capture->dest_queue)==1){
            printf("busy wait!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        }
        base_mat=*((int**)(dequeue((&hand)->stg_capture->dest_queue)->data));
        //print
        for(int i=0;i<WIDTH;i++){
            for(int j=0;j<LENGTH;j++){
                printf("temp_mat[%d][%d]= %d\n",i,j,*(base_mat + i*LENGTH + j));
            }
        }
        //put mat by RGB_Static_mat
        for(int i=0;i<WIDTH;i++){
            for(int j=0;j<LENGTH;j++){
                mat[i][j][0]=(&hand)->RGB_static_mat[0][*(base_mat + i*LENGTH + j)];
                mat[i][j][1]=(&hand)->RGB_static_mat[1][*(base_mat + i*LENGTH + j)];
                mat[i][j][2]=(&hand)->RGB_static_mat[2][*(base_mat + i*LENGTH + j)];
            }
        }
        memcpy(temp_mat,mat,sizeof(mat));
        enqueue((&hand)->stg_rgb_convertor->dest_queue,createNode(temp_mat));
    }
    free(base_mat);
    (&hand)->stg_rgb_convertor->is_active=0;
}
void* convert_yuv(void* handler){
    p_handler hand=(p_handler)handler;
    //printf("yuv");
}
void* decoder(void* handler){
    p_handler hand=(p_handler)handler;
    printf("decoder");
}
void* stg_write(void* handler){
    p_handler hand=(p_handler)handler;
    printf("write");
}

gas_api gas_api_lib={
    .get_dll_version=GAS_API_GET_DLL_VERSION,
    .start_record=GAS_API_START_RECORD,
    .stop_record=GAS_API_STOP_RECORD,
    .start_streaming=GAS_API_START_STREAMING,
    .stop_streaming=GAS_API_STOP_STREAMING,
    .do_snapshot=GAS_API_DO_SNAPSHOT,
    .get_video_statistic=GAS_API_GET_VIDEO_STATISTIC,
    .get_status=GAS_API_GET_STATUS,
    .init_dll=INIT_DLL
};

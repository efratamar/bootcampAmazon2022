#include "gaz_cam_lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "queue.h"
#include <time.h>
#include <errno.h>
#define WAIT 4000

int GAS_API_GET_DLL_VERSION(p_handler handler)
{
    return 1;
}
int GAS_API_START_RECORD(p_handler handler,record_t record)
{
    init_record(handler);
    handler->record_status=1;
    pthread_create(&handler->stg_capture->thread_id,NULL,capture,handler);
    pthread_create(&handler->stg_rgb_convertor->thread_id,NULL,rgb_convertor,handler);
    pthread_create(&handler->stg_yuv_convertor->thread_id,NULL,convert_yuv,handler);
    //pthread_create(&handler->stg_decoder->thread_id,NULL,decoder,handler);
    //pthread_create(&handler->stg_write->thread_id,NULL,stg_write,handler);
    return 1;
}
int GAS_API_STOP_RECORD(p_handler handler)
{
    handler->record_status=0;
    while(handler->record_status!=2)
    {
        msleep(WAIT);
    }
    freeQueue(handler->stg_capture->dest_queue);
    freeQueue(handler->stg_rgb_convertor->dest_queue);
    freeQueue(handler->stg_yuv_convertor->dest_queue);
    freeQueue(handler->stg_decoder->dest_queue);
    exit(0);
    return 1;
}
int GAS_API_START_STREAMING(p_handler handler,streaming_t streaming)
{
    p_handler hand=(p_handler)handler;
    int i=hand->RGB_static_mat[0][0];
    // streaming_t* stream=()
    return i;
}
int GAS_API_STOP_STREAMING(p_handler handler)
{
    p_handler hand=(p_handler)handler;
    int i=hand->RGB_static_mat[0][0];
    return i;
}
int GAS_API_GET_VIDEO_STATISTIC(p_handler handler)
{
    p_handler hand=(p_handler)handler;
    int i=hand->RGB_static_mat[0][0];
    return i;
}
int GAS_API_GET_STATUS (p_handler handler)
{
    p_handler hand=(p_handler)handler;
    int i=hand->RGB_static_mat[0][0];
    return i;
}
handler_t * init_record(p_handler handler){
    handler->stg_capture=(p_stage)calloc(1,sizeof(stage_t));
    if(!handler->stg_capture)exit(1);
    handler->stg_capture->src_queue=NULL;
    handler->stg_capture->dest_queue=createQueue(10);

    handler->stg_rgb_convertor=(p_stage)calloc(1,sizeof(stage_t));
    if(!handler->stg_rgb_convertor)exit(1);
    handler->stg_rgb_convertor->src_queue=handler->stg_capture->dest_queue;
    handler->stg_rgb_convertor->dest_queue=createQueue(10);

    handler->stg_yuv_convertor=(p_stage)calloc(1,sizeof (stage_t));
    if(!handler->stg_yuv_convertor)exit(1);
    handler->stg_yuv_convertor->src_queue=handler->stg_rgb_convertor->dest_queue;
    handler->stg_yuv_convertor->dest_queue=createQueue(10);

    handler->stg_decoder=(p_stage)calloc(1,sizeof (stage_t));
    if(!handler->stg_decoder)exit(1);
    handler->stg_decoder->src_queue=handler->stg_yuv_convertor->dest_queue;
    handler->stg_decoder->dest_queue=createQueue(10);

    handler->stg_write=(p_stage)calloc(1,sizeof (stage_t));
    if(!handler->stg_write)exit(1);
    handler->stg_write->src_queue=handler->stg_decoder->dest_queue;
    handler->stg_write->dest_queue=NULL;
    initRGB_static_mat(handler);
    return handler;
}
p_handler INIT_DLL(){
    p_handler handler=(p_handler)calloc(1,sizeof(handler_t));
    if(!handler)exit(1);
    initRGB_static_mat(handler);
    return handler;

}
int GAS_API_DO_SNAPSHOT(p_handler handler,snapshot_t snapshot)
{
    char* mat=snapshot_capture(handler,snapshot);
    char* RGB_mat=snapshot_RGB(handler,mat);
    ppm_image ppm={ppm.height=LENGTH,ppm.width=WIDTH,ppm.data=RGB_mat,ppm.size=LENGTH*WIDTH*3};
    FILE* snapshot_file=fopen(SNAPSHOTFILEPATH,"wb+");
    if(!snapshot_file)exit(1);
    ppm_save(&ppm,snapshot_file);
    fclose(snapshot_file);
    return 1;
}
char* snapshot_capture(handler_t* handler,snapshot_t snapshot){
    char* degrees_mat;
    int i,j;
    if(handler->stg_capture&&handler->stg_capture->is_active){
        degrees_mat = (char *)calloc(WIDTH * LENGTH , sizeof(char));
        if(!degrees_mat)exit(1);
        degrees_mat=*((char**)(top(handler->stg_capture->dest_queue)->data));
        return degrees_mat;
    }
    degrees_mat = (char *)calloc(WIDTH*LENGTH,sizeof(char));
    if(!degrees_mat)exit(1);
    for (i=0;i<LENGTH ;i++ ) {
        for (j=0;j<WIDTH ;j++ ) {
            *(degrees_mat+(i*WIDTH)+j)=rand()%79;
        }
    }
    return degrees_mat;
}
char* snapshot_RGB( handler_t* handler,char* base_mat){
    char* RGB_mat=(char*)calloc(WIDTH*LENGTH*3, sizeof (char));
    if(!RGB_mat)exit(1);
    for(int i=0;i<LENGTH;i++){
        for(int j=0;j<WIDTH;j++){
            *(RGB_mat +(i*WIDTH*3)+j*3)=handler->RGB_static_mat[0][(int)*(base_mat + i*  WIDTH + j)];
            *(RGB_mat +(i*WIDTH*3)+j*3+1)=handler->RGB_static_mat[1][(int)*(base_mat + i*WIDTH + j)];
            *(RGB_mat +(i*WIDTH*3)+j*3+2)=handler->RGB_static_mat[2][(int)*(base_mat + i*WIDTH + j)];
        }
    }
    int r=255;
    for(int i=150;i<170;i++)
    {
        for(int j=150;j<170;j++)
        {
            *(RGB_mat+(i*WIDTH*3)+j*3)=(char)r;
            r-=2;
            if(j==160)r=255;
        }
    }
    if(base_mat)
        free(base_mat);
    return RGB_mat;
}
size_t ppm_save(ppm_image *img, FILE *outfile) {
    size_t n = 0;
    n += fprintf(outfile, "P6\n# THIS IS A COMMENT\n%d %d\n%d\n",
                 img->height, img->width, 0xFF);
    n += fwrite(img->data,1, img->height * img->width * 3, outfile);
    return n;
}
void initRGB_static_mat(p_handler handler)
{
    int i=0;
    char x=0,y=0,z=(char)255;
    for ( i=0;i<80 ; i++) {
        handler->RGB_static_mat[0][i]=x;
        handler->RGB_static_mat[1][i]=y;
        handler->RGB_static_mat[2][i]=z;
        z-=3;
    }
}
void* capture(void* handler){
    p_handler hand=(p_handler)handler;
    int i,j;
    char *mat;
    hand->stg_capture->is_active=1;
    while(hand->record_status){
        mat = (char *)calloc(WIDTH*LENGTH,sizeof(char));
        if(!mat)exit(1);
        for (i=0;i<LENGTH ;i++ ) {
            for (j=0;j<WIDTH ;j++ ) {
                *(mat+(i*WIDTH)+j)=rand()%79;
            }
        }
        for (i=0;i<LENGTH ;i++ ) {
            for (j=0;j<WIDTH ;j++ ) {
                printf("mat[%d][%d] = %d \n",i,j, *(mat+(i*WIDTH)+j));
            }
        }
        while(enqueue(hand->stg_capture->dest_queue,createNode(&mat)))
        {
            msleep(WAIT);
        }
        msleep(10000000/FRAMESPERSECOND);
    }
    hand->stg_capture->is_active=0;
    return NULL;
}
void* rgb_convertor(void* handler){
    handler_t hand=*(p_handler)handler;
    (&hand)->stg_rgb_convertor->is_active=1;
    char* mat;
    char* base_mat;
    while(hand.record_status||!isEmpty((&hand)->stg_capture->src_queue)){
        while(isEmpty((&hand)->stg_rgb_convertor->src_queue)==1){
            msleep(WAIT);
        }
        base_mat=*((char**)(dequeue((&hand)->stg_capture->dest_queue)->data));
        mat=(char*)calloc(WIDTH*LENGTH*3, sizeof (char));
        if(!mat)exit(1);
        //print
        for(int i=0;i<LENGTH;i++){
            for(int j=0;j<WIDTH;j++){
                if(i==0&&j<4)
                    printf("base_mat[%d][%d]= %d\n",i,j,*(base_mat + i*WIDTH + j));
            }
        }
        for(int i=0;i<LENGTH;i++){
            for(int j=0;j<WIDTH;j++){
                *(mat+(i*WIDTH*3)+j*3)=(&hand)->RGB_static_mat[0][(int)*(base_mat + i*  WIDTH + j)];
                *(mat+(i*WIDTH*3)+j*3+1)=(&hand)->RGB_static_mat[1][(int)*(base_mat + i*WIDTH + j)];
                *(mat+(i*WIDTH*3)+j*3+2)=(&hand)->RGB_static_mat[2][(int)*(base_mat + i*WIDTH + j)];
            }
        }
        while(enqueue((&hand)->stg_rgb_convertor->dest_queue,createNode(&mat)))
        {
            msleep(WAIT);
        }
    }
    if(base_mat)
        free(base_mat);
    (&hand)->stg_rgb_convertor->is_active=0;
    return NULL;
}
void* convert_yuv(void* handler){
    handler_t hand=*(p_handler)handler;
    (&hand)->stg_yuv_convertor->is_active=1;
    char* base_mat=(char*)calloc(LENGTH*WIDTH*3,sizeof(char));
    if(!base_mat)exit(1);
    YUV* yuv;
    while(hand.record_status||!isEmpty((&hand)->stg_yuv_convertor->src_queue))
    {
        while(isEmpty((&hand)->stg_yuv_convertor->src_queue)){
            msleep(WAIT);
        }
        base_mat=*((char**)(dequeue((&hand)->stg_yuv_convertor->src_queue)->data));
        yuv=(YUV*)calloc(1,sizeof(YUV));
        if(!yuv)exit(1);
        for(int i=0;i<LENGTH;i++)
        {
            for(int j=0;j<WIDTH;j++)
            {
                unsigned char R=*(base_mat+i*WIDTH*3+j*3);
                unsigned char G=*(base_mat+i*WIDTH*3+j*3+1);
                unsigned char B=*(base_mat+i*WIDTH*3+j*3+2);
                yuv->y[WIDTH*i+j]= (0.257 * R) + (0.504 * G) + (0.098 * B) + 16+'0';
                if(i%2==0&&j%2==0)
                {
                    yuv->u[(WIDTH*i+j)/4]= (0.439 * R) - (0.368 * G) - (0.071 * B) + 128+'0';
                    yuv->v[(WIDTH*i+j)/4]= -(0.148 * R) - (0.291 * G) + (0.439 * B) + 128+'0';
                }
                printf("YUV");
            }
        }
        while(enqueue((&hand)->stg_yuv_convertor->dest_queue,createNode(yuv)))
        {
            msleep(WAIT);
        }
        if(!base_mat)
            free(base_mat);
    }
    (&hand)->stg_yuv_convertor->is_active=0;
    hand.record_status=2;
    return NULL;
}
void* decoder(void* handler){
    p_handler hand=(p_handler)handler;
    hand->stg_decoder->is_active=0;
    printf("decoder");
    return NULL;
}
void* stg_write(void* handler){
    p_handler hand=(p_handler)handler;
    hand->stg_write->is_active=0;
    printf("write");
    return NULL;
}
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
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

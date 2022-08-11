#include "gaz_cam_lib.h"
#include "assert.h"


//TODO
int GAS_API_GET_DLL_VERSION(p_handler handler)
{
    return SUCCESS;
}
//TODO
int GAS_API_START_RECORD(p_handler handler,record_t record)
{
    pthread_t thread;
    init_record(handler);
    handler->record_status=VALID;
    if(handler->status & STOP_RECORD_ACTIVE)
        handler->status &= ~STOP_RECORD_ACTIVE;
    handler->status|= RECORD_ACTIVE;
    pthread_create(&handler->stg_capture->thread_id,NULL,capture,handler);
    pthread_create(&handler->stg_rgb_convertor->thread_id,NULL,rgb_convertor,handler);
    pthread_create(&handler->stg_yuv_convertor->thread_id,NULL,convert_yuv,handler);
    pthread_create(&handler->stg_decoder->thread_id,NULL,encoder,handler);
    pthread_create(&thread,NULL,thread_tracking,handler->counter_thread);
    return SUCCESS;
}

int GAS_API_STOP_RECORD(p_handler handler)
{
    handler->status |= STOP_RECORD_ACTIVE;
    if(handler->status & RECORD_ACTIVE)
        handler->status &= ~RECORD_ACTIVE;
    while(handler->record_status!=FINISHED){
        usleep(WAIT);//sleep for busy wait
    };
    //free all queues
    freeQueue(handler->stg_capture->dest_queue);
    freeQueue(handler->stg_rgb_convertor->dest_queue);
    freeQueue(handler->stg_yuv_convertor->dest_queue);
    freeQueue(handler->stg_decoder->dest_queue);
    //exit(0);
    return SUCCESS;
}
//TODO
int GAS_API_START_STREAMING(p_handler handler,streaming_t streaming)
{
    p_handler hand=(p_handler)handler;
    int i=hand->RGB_static_mat[0][0];
    return SUCCESS;
}
//TODO
int GAS_API_STOP_STREAMING(p_handler handler)
{
    p_handler hand=(p_handler)handler;
    int i=hand->RGB_static_mat[0][0];
    return SUCCESS;
}
//TODO
int GAS_API_GET_VIDEO_STATISTIC(p_handler handler)
{
    p_handler hand=(p_handler)handler;
    int i=hand->RGB_static_mat[0][0];
    return SUCCESS;
}
//return the status
int GAS_API_GET_STATUS (p_handler handler)
{
    return handler->status;
}
//intial the library
p_handler INIT_DLL(){
    p_handler handler=(p_handler)calloc(1,sizeof (handler_t));
    handler->status&=0;
    if(!handler)exit(1);
    initRGB_static_mat(handler);
    return handler;
}
//init stage for the pipline
p_stage init_stage(Queue* src_queue,int last_stage_flag){
    p_stage stage=(p_stage)calloc(1,sizeof(stage_t));
    if(!stage)exit(1);
    stage->src_queue=src_queue;
    if(last_stage_flag)
        stage->dest_queue=createQueue(10);
    return stage;
}
//init pipline
void init_record(p_handler handler){
    //init thread and queues to connect between the threads
    handler->counter_thread=(working_amount*)calloc(1,sizeof(working_amount));
    handler->counter_thread->counter_capture=0;
    handler->stg_capture=(p_stage)calloc(1,sizeof(stage_t));
    if(!handler->stg_capture)exit(1);
    handler->stg_capture->src_queue=NULL;
    handler->stg_capture->dest_queue=createQueue(10);

    handler->counter_thread->counter_rgb_converet=0;
    handler->stg_rgb_convertor=(p_stage)calloc(1,sizeof(stage_t));
    if(!handler->stg_rgb_convertor)exit(1);
    handler->stg_rgb_convertor->src_queue=handler->stg_capture->dest_queue;
    handler->stg_rgb_convertor->dest_queue=createQueue(10);

    handler->counter_thread->counter_yuv_convert=0;
    handler->stg_yuv_convertor=(p_stage)calloc(1,sizeof (stage_t));
    if(!handler->stg_yuv_convertor)exit(1);
    handler->stg_yuv_convertor->src_queue=handler->stg_rgb_convertor->dest_queue;
    handler->stg_yuv_convertor->dest_queue=createQueue(10);

    handler->counter_thread->counter_decoder=0;
    handler->stg_decoder=(p_stage)calloc(1,sizeof (stage_t));
    if(!handler->stg_decoder)exit(1);
    handler->stg_decoder->src_queue=handler->stg_yuv_convertor->dest_queue;
    handler->stg_decoder->dest_queue=createQueue(10);

    handler->counter_thread->counter_write=0;
    handler->stg_write=(p_stage)calloc(1,sizeof (stage_t));
    if(!handler->stg_write)exit(1);
    handler->stg_write->src_queue=handler->stg_decoder->dest_queue;
    handler->stg_write->dest_queue=NULL;
    /*
    handler->stg_capture=init_stage(NULL,INACTIVE);
    handler->stg_rgb_convertor=init_stage(handler->stg_capture->dest_queue,INACTIVE);
    handler->stg_yuv_convertor=init_stage(handler->stg_rgb_convertor->dest_queue,INACTIVE);
    //handler->stg_decoder=init_stage(NULL,INACTIVE);
    //handler->stg_write=init_stage(NULL,ACTIVE);
    */
}

void initRGB_static_mat(p_handler handler)
{
    //init the matrix by cold degrees
    int i=0;
    char x=0,y=0,z=(char)255;
    for ( i=0;i<80 ; i++) {
        handler->RGB_static_mat[0][i]=x;
        handler->RGB_static_mat[1][i]=y;
        handler->RGB_static_mat[2][i]=z;
        z-=3;
    }
}
//function to create a snapshot
int GAS_API_DO_SNAPSHOT(p_handler handler,snapshot_t snapshot)
{
    handler->status|= SNAPSHOT_ACTIVE;
    char* mat=snapshot_capture(handler,snapshot);
    char* RGB_mat=snapshot_RGB(handler,mat);
    time_t t;
    time(&t); //get the time at the moment
    //ppm_image struct to write the snapshot to the file
    ppm_image ppm={ppm.height=LENGTH,ppm.width=WIDTH,ppm.data=RGB_mat,ppm.size=LENGTH*WIDTH*3};
    ppm_save(&ppm,t);
    jpg_save((uint8_t*)RGB_mat,t,WIDTH,LENGTH);
    return SUCCESS;
}
//initial the snapshot
char* snapshot_capture(handler_t* handler,snapshot_t snapshot){
    int i,j;
    char* frame;
    //check if a record is playing during now - take a ready matrix from the queue
    if(handler->stg_capture&&handler->stg_capture->is_active){
        frame=(char*)(top(handler->stg_capture->dest_queue)->data);
        return frame;
    }
    //if a record is not playing during now - fill the matrix
    frame=(char*)calloc(WIDTH*LENGTH,sizeof(char));
    if(!frame)exit(1);
    for (i=0;i<LENGTH ;i++ ) {
        for (j=0;j<WIDTH ;j++ ) {
            frame[i*WIDTH+j]=rand()%79;
        }
    }
    return frame;
}
//convert the snapshot to RGB format
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
    // int i,j,heat_degrees=255,RGB_index,celsius;
    // //declare a mat for the RGB format
    // char* frame_RGB=(char*)calloc(WIDTH*LENGTH*3, sizeof (char));
    // if(!frame_RGB)exit(1);
    // //fill mat by RGB_Static_mat fields
    // for(i=0;i<LENGTH;i++){
    //     for(j=0;j<WIDTH;j++){
    //         RGB_index=i*WIDTH*3+j*3;
    //         celsius=(int)frame[i*WIDTH + j];
    //         frame_RGB[RGB_index]=handler->RGB_static_mat[0][celsius];
    //         frame_RGB[RGB_index+1]=handler->RGB_static_mat[1][celsius];
    //         frame_RGB[RGB_index+2]=handler->RGB_static_mat[2][celsius];
    //     }
    // }
    // //put in the static matrix heat degrees
    // for(i=150;i<170;i++){
    //     for(j=150,heat_degrees=255;j<170;j++,heat_degrees-=2){
    //         frame_RGB[i*WIDTH*3+j*3]=(char)heat_degrees;
    //     }
    // }
    // if(frame)free(frame);
    // return frame_RGB;
}
//write the snapshot to the file in format BMP
size_t ppm_save(ppm_image *img,time_t t) {
    //write the snapshot to the file
    char szFilename[1024];
    size_t n = 0;
    sprintf(szFilename, BMPFILEPATH, ctime(&t));//Picture name is video name + number
    FILE* snapshot_file=fopen(szFilename,"wb+");
    if(!snapshot_file)exit(1);
    n += fprintf(snapshot_file, "P6\n# THIS IS A COMMENT\n%d %d\n%d\n",
                 img->width, img->height, 0xFF);
    n += fwrite(img->data,1, img->width * img->height * 3, snapshot_file);
    fclose(snapshot_file);
    return n;
}
//write the snapshot to the file in format JPG
void jpg_save(uint8_t *pRGBBuffer, time_t t, int width, int height)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    char szFilename[1024];
    int row_stride;
    FILE* snapshot_file;
    JSAMPROW row_pointer[1];//One row of bitmap
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    sprintf(szFilename, JPGFILEPATH, ctime(&t));//Picture name is video name + number
    snapshot_file = fopen(szFilename, "wb");
    if(!snapshot_file)exit(1);
    jpeg_stdio_dest(&cinfo, snapshot_file);
    cinfo.image_width = width;//is the width and height of the image, in pixels
    cinfo.image_height = height;
    cinfo.input_components = 3;//Here is 1, which means grayscale image, if it is a color bitmap, it is 3
    cinfo.in_color_space = JCS_RGB;//JCS_GRAYSCALE means grayscale image, JCS_RGB means color image
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality (&cinfo, 80, 1);
    jpeg_start_compress(&cinfo, TRUE);
    row_stride = cinfo.image_width * 3;//The number of bytes in each row, if it is not an index image, it needs to be multiplied by 3 here
    //Compress each row
    while (cinfo.next_scanline <cinfo.image_height) {
        row_pointer[0] = &(pRGBBuffer[cinfo.next_scanline * row_stride]);
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(snapshot_file);
}
//initial records snapshot for the record
void* capture(void* handler){
    p_handler hndlr=(p_handler)handler;
    Node* node=NULL;
    int i,j;
    char *frame;
    hndlr->stg_capture->is_active=ACTIVE;
    while(hndlr->status&RECORD_ACTIVE){
        hndlr->counter_thread->counter_capture++;
        frame =(char*)calloc(WIDTH*LENGTH,sizeof(char));
        if(!frame)exit(1);
        for (i=0;i<LENGTH ;i++ ) {
            for (j=0;j<WIDTH ;j++ ) {
                frame[i*WIDTH+j]=rand()%79;
                PRINTF_DBG("capture loop\n");
            }
        }
        //try to enqueue the matrix to the queue until there is an empty place in the queue
        createNode(&frame,&node);
        while(enqueue(hndlr->stg_capture->dest_queue,node)){
            usleep(WAIT);//sleep for busy wait
        };
       // fprintf(stdout,"enqueue capture %p\n",node->data);
        usleep(WAITPERSECONDS);
    }
    hndlr->stg_capture->is_active=INACTIVE;
    return NULL;
}
//convert the records snapshot to RGB format
void* rgb_convertor(void* handler){
    handler_t hndlr=*(p_handler)handler;
    Node *node;
    int i,j,RGB_index,celsius;
    hndlr.stg_rgb_convertor->is_active=ACTIVE;
    char* frame_RGB,*frame;
    while(hndlr.status & RECORD_ACTIVE||!isEmpty(hndlr.stg_rgb_convertor->src_queue))
    {
        hndlr.counter_thread->counter_rgb_converet++;
        while(isEmpty(hndlr.stg_rgb_convertor->src_queue)){
            usleep(WAIT);//sleep for busy wait
        };
        frame=*((char**)(dequeue(hndlr.stg_rgb_convertor->src_queue)->data));
        //fprintf(stdout,"dequeue rgb %p\n",frame);
        frame_RGB=(char*)calloc(WIDTH*LENGTH*3,sizeof (char));
        assert(frame_RGB);
        //fill mat fieleds by RGB_Static_mat fields
        for(i=0;i<LENGTH;i++){
            for(j=0;j<WIDTH;j++){
                RGB_index=i*WIDTH*3+j*3;
                celsius=(int)frame[i*WIDTH + j];
                frame_RGB[RGB_index]=hndlr.RGB_static_mat[0][celsius];
                frame_RGB[RGB_index+1]=hndlr.RGB_static_mat[1][celsius];
                frame_RGB[RGB_index+2]=hndlr.RGB_static_mat[2][celsius];
                PRINTF_DBG("RGB loop\n");
            }
        }
        createNode(&frame_RGB,&node);
        //try to enqueue the matrix to the queue until there is an empty place in the queue
        while(enqueue(hndlr.stg_rgb_convertor->dest_queue,node)){
            usleep(WAIT);//sleep for busy wait
        };
    }
    //if(frame)
    //    free(frame);
    //frame=0;
    hndlr.stg_rgb_convertor->is_active=INACTIVE;
    return NULL;
}
//convert the records snapshot to YUV format
void* convert_yuv(void* handler){
    handler_t hndlr =*(p_handler)handler;
    Node* node;
    unsigned char R,G,B;
    char* frame_RGB;
    YUV* yuv;
    int RGB_index, YUV_index;
    hndlr.stg_yuv_convertor->is_active=ACTIVE;
    while(hndlr.status & RECORD_ACTIVE||!isEmpty(hndlr.stg_yuv_convertor->src_queue))
    {
        hndlr.counter_thread->counter_yuv_convert++;
        while(isEmpty(hndlr.stg_yuv_convertor->src_queue)){
            usleep(WAIT);//sleep for busy wait
        }
        frame_RGB=*((char**)(dequeue(hndlr.stg_yuv_convertor->src_queue)->data));
        yuv=(YUV*)calloc(1,sizeof(YUV));
        if(!yuv)exit(1);
        //convert from RGB format to YUV format
        for(int i=0;i<LENGTH;i++)
        {
            for(int j=0;j<WIDTH;j++)
            {
                RGB_index=i*WIDTH*3+j*3;
                YUV_index=WIDTH*i+j;
                R=frame_RGB[RGB_index];
                G=frame_RGB[RGB_index+1];
                B=frame_RGB[RGB_index+2];
                yuv->y[YUV_index] = YFORMULA(R,G,B);
                if(i%2 == 0 && j%2 == 0)
                {
                    yuv->u[YUV_index/4]= UFORMULA(R,G,B);
                    yuv->v[YUV_index/4]= VFORMULA(R,G,B);
                }
                PRINTF_DBG("yuv loop\n");
            }
        }
        createNode(yuv,&node);
        //try to enqueue the matrix to the queue until there is an empty place in the queue
        while(enqueue(hndlr.stg_yuv_convertor->dest_queue,node)){
            usleep(WAIT);//sleep for busy wait
        };
    }
    //if(frame_RGB)
    //   free(frame_RGB);
    //frame_RGB=0;
    hndlr.stg_yuv_convertor->is_active=INACTIVE;
}
//encoder function
void* encoder(void* hand){
    p_handler hndlr=(p_handler)hand;
    int result=0,yuv_index;
    AVFrame* frame=NULL;
    FILE* outline=NULL;
    encoder_t* p_encoder;
    uint64_t start_time=0;
    YUV *yuv;
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
    fflush(stdout);
    p_encoder=init_encoder(WIDTH,LENGTH,12);
    frame=p_encoder->frame;
    outline=fopen(FILEPATH,"wb");
    assert(outline);
    while(hndlr->status & RECORD_ACTIVE||!isEmpty(hndlr->stg_decoder->src_queue)){
        hndlr->counter_thread->counter_decoder++;
        while(isEmpty(hndlr->stg_decoder->src_queue)){
            usleep(WAIT);
        }
        yuv = (YUV*)(dequeue(hndlr->stg_decoder->src_queue)->data);
        result = av_frame_make_writable(frame);
        assert(result>=0);
        for (int i=0;i<frame->height;i++)
            for (int j=0;j<frame->width;j++)
                frame->data[0][i*frame->linesize[0]+j] =yuv->y[i*frame->width+j/2];
        for(int i=0;i<p_encoder->enc_ctx->height/2;i++)
            for(int j=0;j<p_encoder->enc_ctx->width/2;j++){
                yuv_index = (i/2)*frame->width+j/2;
                frame->data [1][i*frame->linesize[1]+j] = yuv->u[yuv_index];
                frame->data [2][i*frame->linesize[2]+j] = yuv->v[yuv_index];
            }
        if (!start_time){
            frame->pts = 0;
            start_time = GetMHClock();
        }
        else
            frame->pts = GetMHClock()-start_time;
        ff_encoder(p_encoder,outline);
        hndlr->stg_decoder->is_active=INACTIVE;
        if(yuv)
          free(yuv);
    }
    fwrite(endcode, 1, sizeof(endcode), outline);
    fclose(outline);
    hndlr->record_status=FINISHED;
    return NULL;
}
uint64_t GetMHClock(void){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (uint64_t)tv.tv_sec * 1000000L + (uint64_t)tv.tv_usec;
}
void* thread_tracking(void* count){
    working_amount * pcount=(working_amount*)count;
    while(1)
    {
        //usleep(SLEEP_THREAD_TRACKING);
        sleep(5);
        printf("-------------------\n");
        printf("thread capture works %d time\n",pcount->counter_capture);
        pcount->counter_capture=0;
        printf("thread rgb_convert works %d time\n",pcount->counter_rgb_converet);
        pcount->counter_rgb_converet=0;
        printf("thread yov_convert works %d time\n",pcount->counter_yuv_convert);
        pcount->counter_yuv_convert=0;
        printf("thread decoder works %d time\n",pcount->counter_decoder);
        pcount->counter_decoder=0;
        printf("thread write works %d time\n",pcount->counter_write);
        pcount->counter_write=0;
    }
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

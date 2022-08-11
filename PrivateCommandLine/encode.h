#ifndef ENCODE_H
#define ENCODE_H
#include <libavcodec/avcodec.h>
#define FILEPATH "/home/efrat/Desktop/video.ts"

typedef struct
{
    AVCodecContext *enc_ctx;
    AVFrame *frame;
    AVPacket *pkt;
}encoder_t;

encoder_t * init_encoder(int w,int h,int gop);
int ff_encoder(encoder_t *p_encoder,FILE * outline);
uint64_t GetMHClock(void);
#endif // ENCODE_H

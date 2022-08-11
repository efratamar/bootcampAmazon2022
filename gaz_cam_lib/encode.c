#include "encode.h"
#include "assert.h"
#include "encode.h"
#include "assert.h"

encoder_t * init_encoder(int w,int h,int gop){
    const AVCodec *codec;
    int ret=0;
    encoder_t *p_encoder =(encoder_t*) calloc (1,sizeof (encoder_t)) ;
    codec = avcodec_find_encoder(AV_CODEC_ID_MPEG2VIDEO);
    assert(codec);
    p_encoder->frame=av_frame_alloc();
    assert(p_encoder->frame);
    p_encoder->frame->width=w;
    p_encoder->frame->height=h;
    p_encoder->frame->format=AV_PIX_FMT_YUV420P;
    ret = av_frame_get_buffer(p_encoder->frame, 0);
    assert(ret>=0);
    p_encoder->enc_ctx=avcodec_alloc_context3(codec);
    assert(p_encoder->enc_ctx) ;
    p_encoder->pkt=av_packet_alloc();
    assert(p_encoder->pkt);
    p_encoder->enc_ctx->bit_rate=400000;
    p_encoder->enc_ctx->width=w;
    p_encoder->enc_ctx->height=h;

    p_encoder->enc_ctx->time_base = (AVRational){1, 25};
    p_encoder->enc_ctx->framerate = (AVRational){25, 1};
    p_encoder->enc_ctx->gop_size = gop;
    p_encoder->enc_ctx->max_b_frames = 1;
    p_encoder->enc_ctx->pix_fmt=AV_PIX_FMT_YUV420P;

    ret=avcodec_open2(p_encoder->enc_ctx, codec, NULL);
    assert(ret>=0);
    return (p_encoder);

}

int ff_encoder(encoder_t *p_encoder,FILE * outfile){
    int ret=0;
    ret = avcodec_send_frame(p_encoder->enc_ctx, p_encoder->frame);
    assert(ret >= 0);
    while (ret >= 0) {
        ret = avcodec_receive_packet(p_encoder->enc_ctx, p_encoder->pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return 0;
        assert(ret >= 0);
        //fprintf(stdout,"ff_encoder%p\n",p_encoder->pkt->data);
        fwrite(p_encoder->pkt->data, 1, p_encoder->pkt->size, outfile);
        av_packet_unref(p_encoder->pkt);
    }
    return 1;
}

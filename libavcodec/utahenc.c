/**
* This files defines a utah image format encoder. A utah
* image is encoded with a simple header including a magic
* number and the height and width of an image. The header
* is then directly followed by pixel information. Utah image
* files are only encoded in RGB8 pixel format.
*
* Dominic Furano
* Jamie Iong
*
* March 2013
**/

#include "libavutil/imgutils.h"
#include "avcodec.h"
#include "bytestream.h"
#include "utah.h"
#include "internal.h"


static av_cold int utah_encode_init(AVCodecContext *avctx)
{
    // Set a pointer to the private data in the AVCodecContext.
    UTAHContext *s = avctx->priv_data;      

    // Set default values in the AVFrame.
    avcodec_get_frame_defaults(&s->picture);
    
    // Set the coded_frame in the AVCodecContext to the private data's
    // AVFrame.
    avctx->coded_frame = &s->picture;
    
    // If the improper pixel format is used in the incoming frame,
    // log it and return an error.
    if (avctx->pix_fmt != AV_PIX_FMT_RGB8)
    {
        av_log(avctx, AV_LOG_INFO, "Unsupported pixel format.\n");
        return -1;
    }

    return 0;
}

static int utah_encode_frame(AVCodecContext *avctx, AVPacket *pkt,
                            const AVFrame *pict, int *got_packet)
{
    // The context and picture will be stored in the AVCodecContext parameter.
    UTAHContext *s = avctx->priv_data;
    AVFrame *const p = &s->picture;  
    *p = *pict;
    
    p->pict_type = AV_PICTURE_TYPE_I;
    p->key_frame = 1;
    
    // Allocate space for the packet.
    ff_alloc_packet2(avctx, pkt, 10 + avctx->width * avctx->height);
    
    // Save the buffer to the AVPacket parameter's data value.
    uint8_t *buffer = pkt->data;
    
    /*
     * Write the header to the utah file.
     */
    
    bytestream_put_byte(&buffer, 'U');              // Magic Number
    bytestream_put_byte(&buffer, 'T');
    bytestream_put_le32(&buffer, avctx->width);     // Width
    bytestream_put_le32(&buffer, avctx->height);    // Height
    
    /*
     * Write the pixel data to the utah file.
     */
    
    // Advance the pointer past the header.
    buffer += 10;
    
    for (int i = 0; i < avctx->height; i++)
    {
        // Copy an entire line of data from the frame into the packet.
        memcpy(buffer, p->data[0] + (i * p->linesize[0]), avctx->width);

        buffer += avctx->width;
    }
    
    // Signal that we successfully received the packet.
    *got_packet = 1;

    return 0;
}

AVCodec ff_utah_encoder = {
    .name           = "utah",
    .type           = AVMEDIA_TYPE_VIDEO,
    .id             = AV_CODEC_ID_UTAH,
    .priv_data_size = sizeof(UTAHContext),
    .init           = utah_encode_init,
    .encode2        = utah_encode_frame,
    .pix_fmts       = (const enum AVPixelFormat[]){
        AV_PIX_FMT_RGB8, AV_PIX_FMT_NONE
    },
    .long_name      = NULL_IF_CONFIG_SMALL("UTAH (Built for CS 3505 in U of U) image"),
};

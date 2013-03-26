/**
* This files defines a utah image format decoder. A utah
* image is decoded by reading a simple header including a magic
* number and the height and width of an image. The reading
* is then directly followed by pixel information.
*
* Dominic Furano
* Jamie Iong
*
* March 2013
**/

#include "avcodec.h"
#include "bytestream.h"
#include "internal.h"
#include "utah.h"

static av_cold int utah_dec_init(AVCodecContext *avctx)
{    
    // Set a pointer to the private data in the AVCodecContext.
    UTAHContext *s = avctx->priv_data;      

    // Set default values in the AVFrame.
    avcodec_get_frame_defaults(&s->picture);
    
    // Set the coded_frame in the AVCodecContext to the private data's
    // AVFrame.
    avctx->coded_frame = &s->picture;

    return 0;
}


static int decode_frame(AVCodecContext *avctx,
                         void *data, int *got_frame,
                        AVPacket *avpkt)
{
  // This is a buffer for all data in the incoming image data.
  const uint8_t *buffer = avpkt->data;
  // This is the size of the incoming image.
  int buf_size = avpkt->size;
  // Pointer to private data in AVCodecContext paramter which will hold
  // the UTAHContext
  UTAHContext *s = avctx->priv_data;
  // Pointer to the AVFrame
  AVFrame *picture = data;
  // Points to the AVFrame in the AVCodecContext parameter
  AVFrame *p = &s->picture;
  // Points to the data array in the AVFrame in the AVFrame in the AVCodecContext parameter
  uint8_t *ptr;
  
  /*******************
   * Read UTAH header.
   *******************/
  
  // If the file doesn't start with "UT" something is wrong.
  if (bytestream_get_byte(&buffer) != 'U' || bytestream_get_byte(&buffer) != 'T')
  {
    av_log(avctx, AV_LOG_ERROR, "Bad magic number.\n");
    return AVERROR_INVALIDDATA;
  }
  
  // Read in the width and height of the image.
  int width  = bytestream_get_le32(&buffer);
  int height = bytestream_get_le32(&buffer);
  
  // Save the height and width to the AVCodecContext
  avctx->width = width;
  avctx->height = height;  
  
  // Save the pixel format in AVCodecContext.  
  avctx->pix_fmt = AV_PIX_FMT_RGB8;
  
  // Release the buffer if any data exists in it.
  if (p->data[0])
      avctx->release_buffer(avctx, p);
  
  // Allocate space in the buffer for new data.
  ff_get_buffer(avctx, p);
  
  // Point to the data member in AVFrame.
  ptr = p->data[0];
  
  // Set the picture type of the AVFrame.
  p->pict_type = AV_PICTURE_TYPE_I;
  
  // Indicate that this is the key frame.
  p->key_frame = 1;  
  
  /***********************
   * Read the pixel array.
   ***********************/
  
  // Advance the buffer pointer past the header.
  buffer += 10;
  
  for (int i = 0; i < avctx->height; i++)
  {
     // Copy and entire line of pixel data into the packet.
     memcpy(ptr, buffer + (i * avctx->width), avctx->width);
     ptr += p->linesize[0];
  }
  
  // Save the frame in the AVCodecContext parameter.
  *picture = s->picture;
  // Indicate the we successfully received the frame.
  *got_frame = 1;
    
  return buf_size;
}

static av_cold int utah_dec_end(AVCodecContext *avctx)
{
    UTAHContext* c = avctx->priv_data;
    
    // Clean up any resources left over from decoding.
    if (c->picture.data[0])
        avctx->release_buffer(avctx, &c->picture);

    return 0;
}


AVCodec ff_utah_decoder = {
    .name           = "utah",
    .type           = AVMEDIA_TYPE_VIDEO,
    .id             = AV_CODEC_ID_UTAH,
    .priv_data_size = sizeof(UTAHContext),
    .init           = utah_dec_init,
    .close          = utah_dec_end,
    .decode         = decode_frame,
    .capabilities   = CODEC_CAP_DR1 /*| CODEC_CAP_DRAW_HORIZ_BAND*/,
    .long_name      = NULL_IF_CONFIG_SMALL("UTAH (Built for CS 3505 in U of U) image"),
};

/**
*
* This header file is used by any utah format files.
* It only contains an AVFrame of picture of a utah file.
*
* Dominic Furano
* Jamie Iong
*
* March 2013
**/


#ifndef AVCODEC_UTAH_H
#define AVCODEC_UTAH_H

#include "avcodec.h"

typedef struct UTAHContext {
    AVFrame picture;
} UTAHContext;
#endif 

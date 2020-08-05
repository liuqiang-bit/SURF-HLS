#ifndef _MTYPE_H_
#define _MTYPE_H_
#include "hls_video.h"
#include "ap_fixed.h"

//typedef  PIXEL;
typedef hls::stream<ap_axiu<8,1,1,1> > AXI_STREAM_8;
typedef hls::stream<ap_axis<16,1,1,1> > AXI_STREAM_16;
typedef hls::stream<ap_axiu<24,1,1,1> > AXI_STREAM_24;
typedef hls::stream<ap_int<32> > AXI_STREAM_32;

#define MAX_HEIGHT 1920
#define MAX_WIDTH 1280
typedef hls::Scalar<1, unsigned char>                 PIXEL_C1;
typedef hls::Mat<MAX_HEIGHT, MAX_WIDTH, HLS_8UC1>     IMAGE_C1;
typedef hls::Scalar<2, unsigned char>                 PIXEL_C2;
typedef hls::Mat<MAX_HEIGHT, MAX_WIDTH, HLS_8UC2>     IMAGE_C2;
typedef hls::Scalar<3, unsigned char>                 PIXEL_C3;
typedef hls::Mat<MAX_HEIGHT, MAX_WIDTH, HLS_8UC3>     IMAGE_C3;

typedef struct{
	int y, x;
}KeyPoint;

#endif

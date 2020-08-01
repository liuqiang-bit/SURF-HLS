#include "mSURF.h"
#include "hls_opencv.h"
#include <stdio.h>
#include "mType.h"
#include <fstream>
#include "top.h"

using namespace cv;

int main(int argc, char** argv)
{
	AXI_STREAM_24 src_axi;

	Mat src = imread("C:/Users/GUDONG/Desktop/HLS_SURF/common/images/1.png");
	Mat src_rgb = src;

	cvMat2AXIvideo(src_rgb, src_axi);
	top(src_axi);
	std::cout << "done";

	return 0;
}

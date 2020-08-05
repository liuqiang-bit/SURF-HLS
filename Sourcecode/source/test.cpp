#include "mSURF.h"
#include "hls_opencv.h"
#include <stdio.h>
#include "mType.h"
#include <fstream>
#include "top.h"

//using namespace cv;

int main(int argc, char** argv)
{

 	AXI_STREAM_24 src_axi;

	cv::Mat src = cv::imread("C:/Users/GUDONG/Desktop/HLS_SURF/common/images/1.png");
	cv::Mat src_rgb = src;
	cv::Mat kPImage;
	cvMat2AXIvideo(src_rgb, src_axi);
	hls::stream<KeyPoint> keyPoints;
	KeyPoint keyPoint;

	top(src_axi, keyPoints);

	cv::KeyPoint point;
	std::vector<cv::KeyPoint> points;

	for(int i = 0; i < 2000/*471*/; i++)
	{
		keyPoints >> keyPoint;
		point.pt.y = keyPoint.y;
		point.pt.x = keyPoint.x;
		points.push_back(point);
	}
	cv::drawKeypoints(src, points, kPImage, cv::Scalar(0, 0, 255), 0);
	cv::imwrite("C:/Users/GUDONG/Desktop/HLS_SURF/common/images/keyPoint.png", kPImage);
	std::cout << "done";

	return 0;
}

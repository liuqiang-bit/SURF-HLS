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
	int kpn = 0;
	top(src_axi, keyPoints, &kpn);
std::cout << kpn << std::endl;
	cv::KeyPoint point;
	std::vector<cv::KeyPoint> points[3];

	for(int i = 0; i < 415/*471*/; i++)
	{
		keyPoints >> keyPoint;
		point.pt.y = keyPoint.range(31, 17);
		point.pt.x = keyPoint.range(16, 2);
		point.octave = keyPoint.range(1, 0);
		points[point.octave].push_back(point);
	}
	cv::drawKeypoints(src, points[0], kPImage, cv::Scalar(0, 0, 255), 0);
	cv::drawKeypoints(kPImage, points[1], kPImage, cv::Scalar(0, 255, 0), 0);
	cv::drawKeypoints(kPImage, points[2], kPImage, cv::Scalar(255, 0, 0), 0);
	cv::imwrite("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/keyPoint.png", kPImage);
	std::cout << "done";

	return 0;
}

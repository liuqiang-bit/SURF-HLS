#ifndef _MSURF_H_
#define _MSURF_H_
#include "hls_video.h"
#include "ap_fixed.h"
#include "hls_stream.h"
#include <ap_axi_sdata.h>
#include "mType.h"

/*----------�����----------*/
//#define DEBUG
/*--------------------------*/
#define MaxKeyPoint 900

#define nTotalLayers 9
#define nMiddleLayers 3

#define MaxRow 1080
#define MaxCol 1920
#define ROW 600
#define COL 800

#define sumRow 601
#define sumCol 801

#define sumBufRow 76

#ifdef DEBUG
#include <fstream>
#endif
namespace my{

class SURF
{
private:

	static const int MaxSize = 1920;

//	typedef struct{
//		int x1, x2, y1, y2;
//		float n;
//	}SurfHB;

public:

	SURF();

	void createHessianBox(const int box[4][5], SurfHB dst[4], int n, int oldSize, int newSize, int cols);

	SurfHB calcHaarPattern_x_y(int sumBuf[sumBufRow][sumCol], const SurfHB box[3][5], ap_uint< sumBufRow << 3 > sumBufIndex, int rOffset, int cOffset);
	SurfHB calcHaarPattern_xy(int sumBuf[sumBufRow][sumCol], const SurfHB box[4][5], ap_uint< sumBufRow << 3 > sumBufIndex, int rOffset, int cOffset);

	//bool interpolateKeypoint(float N[][9], int dx, int dy, int ds, my::KeyPoint& kpt);

	void integralImg(AXI_STREAM_24& video_in, hls::stream<int>& dst);

	void calcLayerDetAndTrace(
			hls::stream<int>& sum,
//			int size[nTotalLayers],
//			int sampleStep[nTotalLayers],
			hls::stream<float>& det0,
			hls::stream<float>& det1,
			hls::stream<float>& det2,
			hls::stream<float>& det3,
			hls::stream<float>& det4,
			hls::stream<float>& det5,
			hls::stream<float>& det6,
			hls::stream<float>& det7,
			hls::stream<float>& det8,
			hls::stream<float>& trace);


	void findCharacteristicPoint(
//			int size[nTotalLayers],
//			int sampleStep[nTotalLayers],
//			int middleIndices[nMiddleLayers],
			hls::stream<float>& det0,
			hls::stream<float>& det1,
			hls::stream<float>& det2,
			hls::stream<float>& det3,
			hls::stream<float>& det4,
			hls::stream<float>& det5,
			hls::stream<float>& det6,
			hls::stream<float>& det7,
			hls::stream<float>& det8,
			hls::stream<float>& trace,
			SurfHB hessianThreshold,
			KeyPoint* keyPoints,
			int* pointNumber);

	void HessianDetector(hls::stream<int>& sum, KeyPoint* keyPoints, int* pointNumber, int nOctaves, int nOctaveLayers,SurfHB hessianThreshold);
};


/*---------------------------------------class SURF end----------------------------------------*/

}
#endif

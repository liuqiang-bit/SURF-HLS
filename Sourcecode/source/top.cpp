#include "mSURF.h"
#include "mType.h"
#include "top.h"

template<typename T>
void consume(hls::stream<T>& S, int row, int col)
{
	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			static T s;
			S >> s;
		}
	}
}

void top(AXI_STREAM_24& video_in, KeyPoint* keyPoints, int* kpn){
#pragma HLS INTERFACE m_axi depth=32 port=keyPoints
#pragma HLS INTERFACE m_axi depth=32 port=kpn
#pragma HLS INTERFACE axis register both port=video_in

#pragma HLS DATAFLOW
	//hls::stream<KeyPoint> points;
	my::SURF surf;
	static hls::stream<int> sum;

	surf.integralImg(video_in, sum);

//	consume(sum, sumRow, sumCol);

//	/*¼ì²âÌØÕ÷µã*/
	surf.HessianDetector(sum, keyPoints, kpn, 3, 1, 10000);
	//consume(points, 1, 471);
}




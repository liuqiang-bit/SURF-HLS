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

void top(AXI_STREAM_24& video_in){
#pragma HLS INTERFACE axis register both port=video_in

#pragma HLS DATAFLOW
	hls::stream<KeyPoint> points;
	my::SURF surf;
	int pointNumber = 0;
	static hls::stream<int> sum;

	surf.integralImg(video_in, sum);

//	consume(sum, sumRow, sumCol);


//	/*¼ì²âÌØÕ÷µã*/
	surf.HessianDetector(sum, points, pointNumber, 3, 1, 10000/*33000*/);
	//consume(points, 1, 807);
}




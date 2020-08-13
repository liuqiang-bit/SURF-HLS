#include "mSURF.h"
#include "hls_linear_algebra.h"
#include "hls_math.h"

using namespace my;
//#define DEBUG
#ifdef DEBUG
#include <fstream>
#endif

#ifdef DEBUG
std::ofstream fout_sum("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/sum.txt");
std::ofstream fout_pic("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/pic.txt");
std::ofstream fout_HBox("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/HBox.txt");
std::ofstream fout_sumBufIndex("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/sumBufIndex.txt");
std::ofstream fout_size_sampleSteps("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/size_sampleSteps.txt");
std::ofstream fout_middleIndices("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/size_middleIndices.txt");
std::ofstream fout_det0("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det0.txt");
std::ofstream fout_det1("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det1.txt");
std::ofstream fout_det2("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det2.txt");
std::ofstream fout_det3("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det3.txt");
std::ofstream fout_det4("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det4.txt");
std::ofstream fout_det5("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det5.txt");
std::ofstream fout_det6("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det6.txt");
std::ofstream fout_det7("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det7.txt");
std::ofstream fout_det8("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det8.txt");
std::ofstream fout_det0Index("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det0Index.txt");
std::ofstream fout_det1Index("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det1Index.txt");
std::ofstream fout_det2Index("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det2Index.txt");
std::ofstream fout_det3Index("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det3Index.txt");
std::ofstream fout_det4Index("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det4Index.txt");
std::ofstream fout_det5Index("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det5Index.txt");
std::ofstream fout_det6Index("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det6Index.txt");
std::ofstream fout_det7Index("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det7Index.txt");
std::ofstream fout_det8Index("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/det8Index.txt");
std::ofstream fout_sum2buf("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/sum2buf.txt");
std::ofstream fout_sumBuf("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/sumBuf.txt");
std::ofstream fout_rIndex("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/rIndex.txt");
std::ofstream fout_NIndex("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/NIndex.txt");
std::ofstream fout_val0("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/val0.txt");
std::ofstream fout_keyPoint("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/keyPoint.txt");
std::ofstream fout_rOffset("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/rOffset.txt");
#endif

static int margin = 3;

SURF::SURF()
{

}

void SURF::integralImg(AXI_STREAM_24& video_in, hls::stream<int>& dst)
{
	static ap_axiu<24,1,1,1> s;
	static int s2i = 0;
	static int pix_val = 0;
	static int d = 0;

	/*缓存两行积分结果*/
	static int buf[2][COL] = {0};
#pragma HLS ARRAY_PARTITION variable=buf complete dim=1

	ap_uint<1> preRow = 0, curtRow = ~preRow;

	integralImg_row:for (ap_uint<11> r = 0; r < sumRow; r++)
	{
		preRow = ~preRow; curtRow = ~curtRow;
		integralImg_col:for (ap_uint<11> c = 0; c < sumCol; c++)
		{
			/*积分图第一行第一列为0*/
			if(r == 0 || c == 0)
			{
				d = 0;
				s2i = 0;
			}else{
				video_in >> s;
				s2i = s.data.to_int();

				pix_val = 0;
				integralImg_channel:for(int i = 0; i < 3; i++)
				{
					pix_val += ((s2i >> 8*i) & 0xFF);
				}
				pix_val /= 3;

				/*还有优化余地：buf[preRow][c] 和 buf[preRow][c-1]需要2个周期才能读取完*/
				d = buf[preRow][c] - buf[preRow][c-1] + buf[curtRow][c-1] + pix_val;
			}
			buf[curtRow][c] = d;

			dst << d;

#ifdef DEBUG
			fout_pic << s2i << " ";
			fout_sum << d << " ";
#endif

		}

#ifdef DEBUG
		fout_pic << std::endl;
		fout_sum << std::endl;
#endif

	}
}

SurfHB SURF::calcHaarPattern_x_y(int sumBuf[sumBufRow][sumCol], const SurfHB box[3][5], ap_uint< sumBufRow << 3 > sumBufIndex, int rOffset, int cOffset)
{

	SurfHB d = 0;
	calcHaarPattern_kn:for(int kn = 0; kn < 3; kn++)
	{
		d += (
			 sumBuf[sumBufIndex.range((((int)box[kn][3] + 1 + rOffset) << 3) - 1, ((int)box[kn][3] + rOffset) << 3)][(int)box[kn][2] + cOffset]
			-sumBuf[sumBufIndex.range((((int)box[kn][3] + 1 + rOffset) << 3) - 1, ((int)box[kn][3] + rOffset) << 3)][(int)box[kn][0] + cOffset]
			-sumBuf[sumBufIndex.range((((int)box[kn][1] + 1 + rOffset) << 3) - 1, ((int)box[kn][1] + rOffset) << 3)][(int)box[kn][2] + cOffset]
			+sumBuf[sumBufIndex.range((((int)box[kn][1] + 1 + rOffset) << 3) - 1, ((int)box[kn][1] + rOffset) << 3)][(int)box[kn][0] + cOffset]
			 ) * box[kn][4];
	}
	return d;
}

SurfHB SURF::calcHaarPattern_xy(int sumBuf[sumBufRow][sumCol], const SurfHB box[4][5], ap_uint< sumBufRow << 3 > sumBufIndex, int rOffset, int cOffset)
{

	SurfHB d = 0;
	calcHaarPattern_kn:for(int kn = 0; kn < 4; kn++)
	{
		d += (
			 sumBuf[sumBufIndex.range((((int)box[kn][3] + 1 + rOffset) << 3) - 1, ((int)box[kn][3] + rOffset) << 3)][(int)box[kn][2] + cOffset]
			-sumBuf[sumBufIndex.range((((int)box[kn][3] + 1 + rOffset) << 3) - 1, ((int)box[kn][3] + rOffset) << 3)][(int)box[kn][0] + cOffset]
			-sumBuf[sumBufIndex.range((((int)box[kn][1] + 1 + rOffset) << 3) - 1, ((int)box[kn][1] + rOffset) << 3)][(int)box[kn][2] + cOffset]
			+sumBuf[sumBufIndex.range((((int)box[kn][1] + 1 + rOffset) << 3) - 1, ((int)box[kn][1] + rOffset) << 3)][(int)box[kn][0] + cOffset]
			 ) * box[kn][4];
	}
	return d;
}
void SURF::createHessianBox(const int box[4][5], SurfHB dst[4], int n, int oldSize, int newSize, int cols)
{
//	float ratio = (float)newSize / oldSize;
//	createHessianBox_i:for (int i = 0; i < n; i++)
//	{
//		dst[i].x1 = hls::round(box[i][0] * ratio);
//		dst[i].y1 = hls::round(box[i][1] * ratio);
//		dst[i].x2 = hls::round(box[i][2] * ratio);
//		dst[i].y2 = hls::round(box[i][3] * ratio);
//
//		dst[i].n = box[i][4] / (float)((dst[i].y2 - dst[i].y1) * (dst[i].x2 - dst[i].x1));
//
//#ifdef DEBUG
//		fout_HBox << dst[i].x1 <<" "<< dst[i].y1 <<" "<< dst[i].x2 <<" "<< dst[i].y2 <<" "<< dst[i].n << std::endl;
//#endif
//	}
//#ifdef DEBUG
//	fout_HBox << std::endl;
//#endif
}

void SURF::calcLayerDetAndTrace(
		hls::stream<int>& sum,
		hls::stream<float>& det0,
		hls::stream<float>& det1,
		hls::stream<float>& det2,
		hls::stream<float>& det3,
		hls::stream<float>& det4,
		hls::stream<float>& det5,
		hls::stream<float>& det6,
		hls::stream<float>& det7,
		hls::stream<float>& det8,
		hls::stream<float>& trace)
{
#ifdef DEBUG
	static int detT[nTotalLayers] = {0};
#endif
	static int sizes[nTotalLayers] = {9, 15, 21, 15, 27, 39, 27, 51, 75};						// 每一层用的 Harr模板的大小
	static int sampleSteps[nTotalLayers] = {0, 0, 0, 1, 1, 1, 2, 2, 2};							// 每一层用的采样步长是2的sampleSteps[nTotalLayers]次幂
//	static int sample_r[nTotalLayers];
//	static int sample_c[nTotalLayers];

//	//不处理的边界大小
//	static int margin[nTotalLayers];

	/*定义盒子滤波器*/
	static const int NX = 3, NY = 3, NXY = 4;
//	static const int dx_s[NXY][5]  = { { 0, 2, 3, 7, 1 }, { 3, 2, 6, 7, -2 }, { 6, 2, 9, 7, 1 }, {0, 0, 0, 0, 0} };
//	static const int dy_s[NXY][5]  = { { 2, 0, 7, 3, 1 }, { 2, 3, 7, 6, -2 }, { 2, 6, 7, 9, 1 }, {0, 0, 0, 0, 0} };
//	static const int dxy_s[NXY][5] = { { 1, 1, 4, 4, 1 }, { 5, 1, 8, 4, -1 }, { 1, 5, 4, 8, -1 }, { 5, 5, 8, 8, 1 } };
//		calcLayerDetAndTrace_hessian:for(int i = 0; i < nTotalLayers; i++)
//		{
//			/*有优化空间，目前及其耗费FF和LUT,可考虑去掉此步骤，直接调用生成好的hessianBox*/
//			createHessianBox(dx_s, Dx[i], NX, 9, sizes[i], sumCol);
//			createHessianBox(dy_s, Dy[i], NY, 9, sizes[i], sumCol);
//			createHessianBox(dxy_s, Dxy[i], NXY, 9, sizes[i], sumCol);
//		}
//		Dx[0][0].x1 = 0; Dx[0][0].y1 = 2; Dx[0][0].x2 = 3; Dx[0][0].y2 = 7; Dx[0][0].n = 0.0666667;
//		Dx[0][1].x1 = 3; Dx[0][1].y1 = 2; Dx[0][1].x2 = 6; Dx[0][1].y2 = 7; Dx[0][1].n = -0.133333;
//		Dx[0][2].x1 = 6; Dx[0][2].y1 = 2; Dx[0][2].x2 = 9; Dx[0][2].y2 = 7; Dx[0][2].n = 0.0666667;
//
//		Dy[0][0].x1 = 2; Dy[0][0].y1 = 0; Dy[0][0].x2 = 7; Dy[0][0].y2 = 3; Dy[0][0].n = 0.0666667;
//		Dy[0][1].x1 = 2; Dy[0][1].y1 = 3; Dy[0][1].x2 = 7; Dy[0][1].y2 = 6; Dy[0][1].n = -0.133333;
//		Dy[0][2].x1 = 2; Dy[0][2].y1 = 6; Dy[0][2].x2 = 7; Dy[0][2].y2 = 9; Dy[0][2].n = 0.0666667;
//
//		Dxy[0][0].x1 = 1; Dxy[0][0].y1 = 1; Dxy[0][0].x2 = 4; Dxy[0][0].y2 = 4; Dxy[0][0].n = 0.111111;
//		Dxy[0][1].x1 = 5; Dxy[0][1].y1 = 1; Dxy[0][1].x2 = 8; Dxy[0][1].y2 = 4; Dxy[0][1].n = -0.111111;
//		Dxy[0][2].x1 = 1; Dxy[0][2].y1 = 5; Dxy[0][2].x2 = 4; Dxy[0][2].y2 = 8; Dxy[0][2].n = -0.111111;
//		Dxy[0][3].x1 = 5; Dxy[0][3].y1 = 5; Dxy[0][3].x2 = 8; Dxy[0][3].y2 = 8; Dxy[0][3].n = 0.111111;
//
//		Dx[1][0].x1 = 0;  Dx[1][0].y1 = 3; Dx[1][0].x2 = 5; Dx[1][0].y2 = 12; Dx[1][0].n = 0.0222222;
//		Dx[1][1].x1 = 5;  Dx[1][1].y1 = 3; Dx[1][1].x2 = 10; Dx[1][1].y2 = 12; Dx[1][1].n = -0.0444444;
//		Dx[1][2].x1 = 10; Dx[1][2].y1 = 3; Dx[1][2].x2 = 15; Dx[1][2].y2 = 12; Dx[1][2].n = 0.0222222;
//
//		Dy[1][0].x1 = 3; Dy[1][0].y1 = 0; Dy[1][0].x2 = 12; Dy[1][0].y2 = 5; Dy[1][0].n = 0.0222222;
//		Dy[1][1].x1 = 3; Dy[1][1].y1 = 5; Dy[1][1].x2 = 12; Dy[1][1].y2 = 10; Dy[1][1].n = -0.0444444;
//		Dy[1][2].x1 = 3; Dy[1][2].y1 = 10; Dy[1][2].x2 = 12; Dy[1][2].y2 = 15; Dy[1][2].n = 0.0222222;
//
//		Dxy[1][0].x1 = 2; Dxy[1][0].y1 = 2; Dxy[1][0].x2 = 7; Dxy[1][0].y2 = 7; Dxy[1][0].n = 0.04;
//		Dxy[1][1].x1 = 8; Dxy[1][1].y1 = 2; Dxy[1][1].x2 = 13; Dxy[1][1].y2 = 7; Dxy[1][1].n = -0.04;
//		Dxy[1][2].x1 = 2; Dxy[1][2].y1 = 8; Dxy[1][2].x2 = 7; Dxy[1][2].y2 = 13; Dxy[1][2].n = -0.04;
//		Dxy[1][3].x1 = 8; Dxy[1][3].y1 = 8; Dxy[1][3].x2 = 13; Dxy[1][3].y2 = 13; Dxy[1][3].n = 0.04;
//
//		Dx[2][0].x1 = 0;  Dx[2][0].y1 = 5; Dx[2][0].x2 = 7;  Dx[2][0].y2 = 16; Dx[2][0].n = 0.012987;
//		Dx[2][1].x1 = 7;  Dx[2][1].y1 = 5; Dx[2][1].x2 = 14; Dx[2][1].y2 = 16; Dx[2][1].n = -0.025974;
//		Dx[2][2].x1 = 14; Dx[2][2].y1 = 5; Dx[2][2].x2 = 21; Dx[2][2].y2 = 16; Dx[2][2].n = 0.012987;
//
//		Dy[2][0].x1 = 5; Dy[2][0].y1 = 0;  Dy[2][0].x2 = 16; Dy[2][0].y2 = 7; Dy[2][0].n = 0.012987;
//		Dy[2][1].x1 = 5; Dy[2][1].y1 = 7;  Dy[2][1].x2 = 16; Dy[2][1].y2 = 14; Dy[2][1].n = -0.025974;
//		Dy[2][2].x1 = 5; Dy[2][2].y1 = 14; Dy[2][2].x2 = 16; Dy[2][2].y2 = 21; Dy[2][2].n = 0.012987;
//
//		Dxy[2][0].x1 = 2;  Dxy[2][0].y1 = 2;  Dxy[2][0].x2 = 9;  Dxy[2][0].y2 = 9;  Dxy[2][0].n = 0.0204082;
//		Dxy[2][1].x1 = 12; Dxy[2][1].y1 = 2;  Dxy[2][1].x2 = 19; Dxy[2][1].y2 = 9;  Dxy[2][1].n = -0.0204082;
//		Dxy[2][2].x1 = 2;  Dxy[2][2].y1 = 12; Dxy[2][2].x2 = 9;  Dxy[2][2].y2 = 19; Dxy[2][2].n = -0.0204082;
//		Dxy[2][3].x1 = 12; Dxy[2][3].y1 = 12; Dxy[2][3].x2 = 19; Dxy[2][3].y2 = 19; Dxy[2][3].n = 0.0204082;
//
//		Dx[3][0].x1 = 0;  Dx[3][0].y1 = 3; Dx[3][0].x2 = 5;  Dx[3][0].y2 = 12; Dx[3][0].n = 0.0222222;
//		Dx[3][1].x1 = 5;  Dx[3][1].y1 = 3; Dx[3][1].x2 = 10; Dx[3][1].y2 = 12; Dx[3][1].n = -0.0444444;
//		Dx[3][2].x1 = 10; Dx[3][2].y1 = 3; Dx[3][2].x2 = 15; Dx[3][2].y2 = 12; Dx[3][2].n = 0.0222222;
//
//		Dy[3][0].x1 = 3; Dy[3][0].y1 = 0;  Dy[3][0].x2 = 12; Dy[3][0].y2 = 5; Dy[3][0].n = 0.0222222;
//		Dy[3][1].x1 = 3; Dy[3][1].y1 = 5;  Dy[3][1].x2 = 12; Dy[3][1].y2 = 10; Dy[3][1].n = -0.0444444;
//		Dy[3][2].x1 = 3; Dy[3][2].y1 = 10; Dy[3][2].x2 = 12; Dy[3][2].y2 = 15; Dy[3][2].n = 0.0222222;
//
//		Dxy[3][0].x1 = 2; Dxy[3][0].y1 = 2; Dxy[3][0].x2 = 7;  Dxy[3][0].y2 = 7;  Dxy[3][0].n = 0.04;
//		Dxy[3][1].x1 = 8; Dxy[3][1].y1 = 2; Dxy[3][1].x2 = 13; Dxy[3][1].y2 = 7;  Dxy[3][1].n = -0.04;
//		Dxy[3][2].x1 = 2; Dxy[3][2].y1 = 8; Dxy[3][2].x2 = 7;  Dxy[3][2].y2 = 13; Dxy[3][2].n = -0.04;
//		Dxy[3][3].x1 = 8; Dxy[3][3].y1 = 8; Dxy[3][3].x2 = 13; Dxy[3][3].y2 = 13; Dxy[3][3].n = 0.04;
//
//		Dx[4][0].x1 = 0;  Dx[4][0].y1 = 6; Dx[4][0].x2 = 9;  Dx[4][0].y2 = 21; Dx[4][0].n = 0.00740741;
//		Dx[4][1].x1 = 9;  Dx[4][1].y1 = 6; Dx[4][1].x2 = 18; Dx[4][1].y2 = 21; Dx[4][1].n = -0.0148148;
//		Dx[4][2].x1 = 18; Dx[4][2].y1 = 6; Dx[4][2].x2 = 27; Dx[4][2].y2 = 21; Dx[4][2].n = 0.00740741;
//
//		Dy[4][0].x1 = 6; Dy[4][0].y1 = 0;  Dy[4][0].x2 = 21; Dy[4][0].y2 = 9;  Dy[4][0].n = 0.00740741;
//		Dy[4][1].x1 = 6; Dy[4][1].y1 = 9;  Dy[4][1].x2 = 21; Dy[4][1].y2 = 18; Dy[4][1].n = -0.0148148;
//		Dy[4][2].x1 = 6; Dy[4][2].y1 = 18; Dy[4][2].x2 = 21; Dy[4][2].y2 = 27; Dy[4][2].n = 0.00740741;
//
//		Dxy[4][0].x1 = 3;  Dxy[4][0].y1 = 3;  Dxy[4][0].x2 = 12; Dxy[4][0].y2 = 12; Dxy[4][0].n = 0.0123457;
//		Dxy[4][1].x1 = 15; Dxy[4][1].y1 = 3;  Dxy[4][1].x2 = 24; Dxy[4][1].y2 = 12; Dxy[4][1].n = -0.0123457;
//		Dxy[4][2].x1 = 3;  Dxy[4][2].y1 = 15; Dxy[4][2].x2 = 12; Dxy[4][2].y2 = 24; Dxy[4][2].n = -0.0123457;
//		Dxy[4][3].x1 = 15; Dxy[4][3].y1 = 15; Dxy[4][3].x2 = 24; Dxy[4][3].y2 = 24; Dxy[4][3].n = 0.0123457;
//
//		Dx[5][0].x1 = 0;  Dx[5][0].y1 = 9; Dx[5][0].x2 = 13; Dx[5][0].y2 = 30; Dx[5][0].n = 0.003663;
//		Dx[5][1].x1 = 13; Dx[5][1].y1 = 9; Dx[5][1].x2 = 26; Dx[5][1].y2 = 30; Dx[5][1].n = -0.00732601;
//		Dx[5][2].x1 = 26; Dx[5][2].y1 = 9; Dx[5][2].x2 = 39; Dx[5][2].y2 = 30; Dx[5][2].n = 0.003663;
//
//		Dy[5][0].x1 = 9; Dy[5][0].y1 = 0;  Dy[5][0].x2 = 30; Dy[5][0].y2 = 13; Dy[5][0].n = 0.003663;
//		Dy[5][1].x1 = 9; Dy[5][1].y1 = 13; Dy[5][1].x2 = 30; Dy[5][1].y2 = 26; Dy[5][1].n = -0.00732601;
//		Dy[5][2].x1 = 9; Dy[5][2].y1 = 26; Dy[5][2].x2 = 30; Dy[5][2].y2 = 39; Dy[5][2].n = 0.003663;
//
//		Dxy[5][0].x1 = 4;  Dxy[5][0].y1 = 4;  Dxy[5][0].x2 = 17; Dxy[5][0].y2 = 17; Dxy[5][0].n = 0.00591716;
//		Dxy[5][1].x1 = 22; Dxy[5][1].y1 = 4;  Dxy[5][1].x2 = 35; Dxy[5][1].y2 = 17; Dxy[5][1].n = -0.00591716;
//		Dxy[5][2].x1 = 4;  Dxy[5][2].y1 = 22; Dxy[5][2].x2 = 17; Dxy[5][2].y2 = 35; Dxy[5][2].n = -0.00591716;
//		Dxy[5][3].x1 = 22; Dxy[5][3].y1 = 22; Dxy[5][3].x2 = 35; Dxy[5][3].y2 = 35; Dxy[5][3].n = 0.00591716;
//
//		Dx[6][0].x1 = 0;  Dx[6][0].y1 = 6; Dx[6][0].x2 = 9;  Dx[6][0].y2 = 21; Dx[6][0].n = 0.00740741;
//		Dx[6][1].x1 = 9;  Dx[6][1].y1 = 6; Dx[6][1].x2 = 18; Dx[6][1].y2 = 21; Dx[6][1].n = -0.0148148;
//		Dx[6][2].x1 = 18; Dx[6][2].y1 = 6; Dx[6][2].x2 = 27; Dx[6][2].y2 = 21; Dx[6][2].n = 0.00740741;
//
//		Dy[6][0].x1 = 6; Dy[6][0].y1 = 0;  Dy[6][0].x2 = 21; Dy[6][0].y2 = 9;  Dy[6][0].n = 0.00740741;
//		Dy[6][1].x1 = 6; Dy[6][1].y1 = 9;  Dy[6][1].x2 = 21; Dy[6][1].y2 = 18; Dy[6][1].n = -0.0148148;
//		Dy[6][2].x1 = 6; Dy[6][2].y1 = 18; Dy[6][2].x2 = 21; Dy[6][2].y2 = 27; Dy[6][2].n = 0.00740741;
//
//		Dxy[6][0].x1 = 3;  Dxy[6][0].y1 = 3;  Dxy[6][0].x2 = 12; Dxy[6][0].y2 = 12; Dxy[6][0].n = 0.0123457;
//		Dxy[6][1].x1 = 15; Dxy[6][1].y1 = 3;  Dxy[6][1].x2 = 24; Dxy[6][1].y2 = 12; Dxy[6][1].n = -0.0123457;
//		Dxy[6][2].x1 = 3;  Dxy[6][2].y1 = 15; Dxy[6][2].x2 = 12; Dxy[6][2].y2 = 24; Dxy[6][2].n = -0.0123457;
//		Dxy[6][3].x1 = 15; Dxy[6][3].y1 = 15; Dxy[6][3].x2 = 24; Dxy[6][3].y2 = 24; Dxy[6][3].n = 0.0123457;
//
//		Dx[7][0].x1 = 0;  Dx[7][0].y1 = 11; Dx[7][0].x2 = 17; Dx[7][0].y2 = 40; Dx[7][0].n = 0.0020284;
//		Dx[7][1].x1 = 17; Dx[7][1].y1 = 11; Dx[7][1].x2 = 34; Dx[7][1].y2 = 40; Dx[7][1].n = -0.0040568;
//		Dx[7][2].x1 = 34; Dx[7][2].y1 = 11; Dx[7][2].x2 = 51; Dx[7][2].y2 = 40; Dx[7][2].n = 0.0020284;
//
//		Dy[7][0].x1 = 11; Dy[7][0].y1 = 0;  Dy[7][0].x2 = 40; Dy[7][0].y2 = 17; Dy[7][0].n = 0.0020284;
//		Dy[7][1].x1 = 11; Dy[7][1].y1 = 17; Dy[7][1].x2 = 40; Dy[7][1].y2 = 34; Dy[7][1].n = -0.0040568;
//		Dy[7][2].x1 = 11; Dy[7][2].y1 = 34; Dy[7][2].x2 = 40; Dy[7][2].y2 = 51; Dy[7][2].n = 0.0020284;
//
//		Dxy[7][0].x1 = 6;  Dxy[7][0].y1 = 6;  Dxy[7][0].x2 = 23; Dxy[7][0].y2 = 23; Dxy[7][0].n = 0.00346021;
//		Dxy[7][1].x1 = 28; Dxy[7][1].y1 = 6;  Dxy[7][1].x2 = 45; Dxy[7][1].y2 = 23; Dxy[7][1].n = -0.00346021;
//		Dxy[7][2].x1 = 6;  Dxy[7][2].y1 = 28; Dxy[7][2].x2 = 23; Dxy[7][2].y2 = 45; Dxy[7][2].n = -0.00346021;
//		Dxy[7][3].x1 = 28; Dxy[7][3].y1 = 28; Dxy[7][3].x2 = 45; Dxy[7][3].y2 = 45; Dxy[7][3].n = 0.00346021;
//
//		Dx[8][0].x1 = 0;  Dx[8][0].y1 = 17; Dx[8][0].x2 = 25; Dx[8][0].y2 = 58; Dx[8][0].n = 0.00097561;
//		Dx[8][1].x1 = 25; Dx[8][1].y1 = 17; Dx[8][1].x2 = 50; Dx[8][1].y2 = 58; Dx[8][1].n = -0.00195122;
//		Dx[8][2].x1 = 50; Dx[8][2].y1 = 17; Dx[8][2].x2 = 75; Dx[8][2].y2 = 58; Dx[8][2].n = 0.00097561;
//
//		Dy[8][0].x1 = 17; Dy[8][0].y1 = 0;  Dy[8][0].x2 = 58; Dy[8][0].y2 = 25; Dy[8][0].n = 0.00097561;
//		Dy[8][1].x1 = 17; Dy[8][1].y1 = 25; Dy[8][1].x2 = 58; Dy[8][1].y2 = 50; Dy[8][1].n = -0.00195122;
//		Dy[8][2].x1 = 17; Dy[8][2].y1 = 50; Dy[8][2].x2 = 58; Dy[8][2].y2 = 75; Dy[8][2].n = 0.00097561;
//
//		Dxy[8][0].x1 = 8;  Dxy[8][0].y1 = 8;  Dxy[8][0].x2 = 33; Dxy[8][0].y2 = 33; Dxy[8][0].n = 0.0016;
//		Dxy[8][1].x1 = 42; Dxy[8][1].y1 = 8;  Dxy[8][1].x2 = 67; Dxy[8][1].y2 = 33; Dxy[8][1].n = -0.0016;
//		Dxy[8][2].x1 = 8;  Dxy[8][2].y1 = 42; Dxy[8][2].x2 = 33; Dxy[8][2].y2 = 67; Dxy[8][2].n = -0.0016;
//		Dxy[8][3].x1 = 42; Dxy[8][3].y1 = 42; Dxy[8][3].x2 = 67; Dxy[8][3].y2 = 67; Dxy[8][3].n = 0.0016;
	static const SurfHB Dx[nTotalLayers][NX][5] =
	{
			{
					{0, 2, 3, 7, 0.0666667}, {3, 2, 6, 7, -0.133333}, {6, 2, 9, 7, 0.0666667}
			},
			{
					{0, 3, 5, 12, 0.0222222}, {5, 3, 10, 12, -0.0444444}, {10, 3, 15, 12, 0.0222222}
			},
			{
					{0, 5, 7, 16, 0.012987}, {7, 5, 14, 16, -0.025974}, {14, 5, 21, 16, 0.012987}
			},
			{
					{0, 3, 5, 12, 0.0222222}, {5, 3, 10, 12, -0.0444444}, {10, 3, 15, 12, 0.0222222}
			},
			{
					{0, 6, 9, 21, 0.00740741}, {9, 6, 18, 21, -0.0148148}, {18, 6, 27, 21, 0.00740741}
			},
			{
					{0, 9, 13, 30, 0.003663}, {13, 9, 26, 30, -0.00732601}, {26, 9, 39, 30, 0.003663}
			},
			{
					{0, 6, 9, 21, 0.00740741}, {9, 6, 18, 21, -0.0148148}, {18, 6, 27, 21, 0.00740741}
			},
			{
					{0, 11, 17, 40, 0.0020284}, {17, 11, 34, 40, -0.0040568}, {34, 11, 51, 40, 0.0020284}
			},
			{
					{0, 17, 25, 58, 0.00097561}, {25, 17, 50, 58, -0.00195122}, {50, 17, 75, 58, 0.00097561}
			}
	};
	static const SurfHB Dy[nTotalLayers][NY][5] =
	{
			{
					{2, 0, 7, 3, 0.0666667}, {2, 3, 7, 6, -0.133333}, {2, 6, 7, 9, 0.0666667}
			},
			{
					{3, 0, 12, 5, 0.0222222}, {3, 5, 12, 10, -0.0444444}, {3, 10, 12, 15, 0.0222222}
			},
			{
					{5, 0, 16, 7, 0.012987}, {5, 7, 16, 14, -0.025974}, {5, 14, 16, 21, 0.012987}
			},
			{
					{3, 0, 12, 5, 0.0222222}, {3, 5, 12, 10, -0.0444444}, {3, 10, 12, 15, 0.0222222}
			},
			{
					{6, 0, 21, 9, 0.00740741}, {6, 9, 21, 18, -0.0148148}, {6, 18, 21, 27, 0.00740741}
			},
			{
					{9, 0, 30, 13, 0.003663}, {9, 13, 30, 26, -0.00732601}, {9, 26, 30, 39, 0.003663}
			},
			{
					{6, 0, 21, 9, 0.00740741}, {6, 9, 21, 18, -0.0148148}, {6, 18, 21, 27, 0.00740741}
			},
			{
					{11, 0, 40, 17, 0.0020284}, {11, 17, 40, 34, -0.0040568}, {11, 34, 40, 51, 0.0020284}
			},
			{
					{17, 0, 58, 25, 0.00097561}, {17, 25, 58, 50, -0.00195122}, {17, 50, 58, 75, 0.00097561}
			}
	};
	static const  SurfHB Dxy[nTotalLayers][NXY][5] =
	{
			{
					{1, 1, 4, 4, 0.111111}, {5, 1, 8, 4, -0.111111}, {1, 5, 4, 8, -0.111111}, {5, 5, 8, 8, 0.111111}
			},
			{
					{2, 2, 7, 7, 0.04}, {8, 2, 13, 7, -0.04}, {2, 8, 7, 13, -0.04}, {8, 8, 13, 13, 0.04}
			},
			{
					{2, 2, 9, 9, 0.0204082}, {12, 2, 19, 9, -0.0204082}, {2, 12, 9, 19, -0.0204082}, {12, 12, 19, 19, 0.0204082}
			},
			{
					{2, 2, 7, 7, 0.04}, {8, 2, 13, 7, -0.04}, {2, 8, 7, 13, -0.04}, {8, 8, 13, 13, 0.04}
			},
			{
					{3, 3, 12, 12, 0.0123457}, {15, 3, 24, 12, -0.0123457}, {3, 15, 12, 24, -0.0123457}, {15, 15, 24, 24, 0.0123457}
			},
			{
					{4, 4, 17, 17, 0.00591716}, {22, 4, 35, 17, -0.00591716}, {4, 22, 17, 35, -0.00591716}, {22, 22, 35, 35, 0.00591716}
			},
			{
					{3, 3, 12, 12, 0.0123457}, {15, 3, 24, 12, - 0.0123457}, {3, 15, 12, 24, - 0.0123457}, {15, 15, 24, 24, 0.0123457}
			},
			{
					{6, 6, 23, 23, 0.00346021}, {28, 6, 45, 23, - 0.00346021}, {6, 28, 23, 45, - 0.00346021}, {28, 28, 45, 45, 0.00346021}
			},
			{
					{8, 8, 33, 33, 0.0016}, {42, 8, 67, 33, - 0.0016}, {8, 42, 33, 67, - 0.0016}, {42, 42, 67, 67, 0.0016}
			}
	};
//		static const SurfHB Dx[nTotalLayers][NX][5] =
//	{
//			{
//					{0, 2, 3, 7, 67}, {3, 2, 6, 7, -133}, {6, 2, 9, 7, 67}
//			},
//			{
//					{0, 3, 5, 12, 22}, {5, 3, 10, 12, -44}, {10, 3, 15, 12, 22}
//			},
//			{
//					{0, 5, 7, 16, 13}, {7, 5, 14, 16, -26}, {14, 5, 21, 16, 13}
//			},
//			{
//					{0, 3, 5, 12, 22}, {5, 3, 10, 12, -44}, {10, 3, 15, 12, 22}
//			},
//			{
//					{0, 6, 9, 21, 7}, {9, 6, 18, 21, -15}, {18, 6, 27, 21, 7}
//			},
//			{
//					{0, 9, 13, 30, 4}, {13, 9, 26, 30, -7}, {26, 9, 39, 30, 4}
//			},
//			{
//					{0, 6, 9, 21, 7}, {9, 6, 18, 21, -15}, {18, 6, 27, 21, 7}
//			},
//			{
//					{0, 11, 17, 40, 2}, {17, 11, 34, 40, -4}, {34, 11, 51, 40, 2}
//			},
//			{
//					{0, 17, 25, 58, 1}, {25, 17, 50, 58, -2}, {50, 17, 75, 58, 1}
//			}
//	};
//	static const SurfHB Dy[nTotalLayers][NY][5] =
//	{
//			{
//					{2, 0, 7, 3, 67}, {2, 3, 7, 6, -133}, {2, 6, 7, 9, 67}
//			},
//			{
//					{3, 0, 12, 5, 22}, {3, 5, 12, 10, -44}, {3, 10, 12, 15, 22}
//			},
//			{
//					{5, 0, 16, 7, 13}, {5, 7, 16, 14, -26}, {5, 14, 16, 21, 13}
//			},
//			{
//					{3, 0, 12, 5, 22}, {3, 5, 12, 10, -44}, {3, 10, 12, 15, 22}
//			},
//			{
//					{6, 0, 21, 9, 7}, {6, 9, 21, 18, -15}, {6, 18, 21, 27, 7}
//			},
//			{
//					{9, 0, 30, 13, 4}, {9, 13, 30, 26, -7}, {9, 26, 30, 39, 4}
//			},
//			{
//					{6, 0, 21, 9, 7}, {6, 9, 21, 18, -15}, {6, 18, 21, 27, 7}
//			},
//			{
//					{11, 0, 40, 17, 2}, {11, 17, 40, 34, -4}, {11, 34, 40, 51, 2}
//			},
//			{
//					{17, 0, 58, 25, 1}, {17, 25, 58, 50, -2}, {17, 50, 58, 75, 1}
//			}
//	};
//	static const  SurfHB Dxy[nTotalLayers][NXY][5] =
//	{
//			{
//					{1, 1, 4, 4, 111}, {5, 1, 8, 4, -111}, {1, 5, 4, 8, -111}, {5, 5, 8, 8, 111}
//			},
//			{
//					{2, 2, 7, 7, 40}, {8, 2, 13, 7, -40}, {2, 8, 7, 13, -40}, {8, 8, 13, 13, 40}
//			},
//			{
//					{2, 2, 9, 9, 20}, {12, 2, 19, 9, -20}, {2, 12, 9, 19, -20}, {12, 12, 19, 19, 20}
//			},
//			{
//					{2, 2, 7, 7, 40}, {8, 2, 13, 7, -40}, {2, 8, 7, 13, -40}, {8, 8, 13, 13, 40}
//			},
//			{
//					{3, 3, 12, 12, 12}, {15, 3, 24, 12, -12}, {3, 15, 12, 24, -12}, {15, 15, 24, 24, 12}
//			},
//			{
//					{4, 4, 17, 17, 6}, {22, 4, 35, 17, -6}, {4, 22, 17, 35, -6}, {22, 22, 35, 35, 6}
//			},
//			{
//					{3, 3, 12, 12, 12}, {15, 3, 24, 12, -12}, {3, 15, 12, 24, -12}, {15, 15, 24, 24, 12}
//			},
//			{
//					{6, 6, 23, 23, 3}, {28, 6, 45, 23, -3}, {6, 28, 23, 45, -3}, {28, 28, 45, 45, 3}
//			},
//			{
//					{8, 8, 33, 33, 2}, {42, 8, 67, 33, -2}, {8, 42, 33, 67, -2}, {42, 42, 67, 67, 2}
//			}
//	};




	/*9层模板共用一个缓存数组，缓存数组的行数为最大模板的尺寸*/
	static int sumBuf[sumBufRow][sumCol];
	ap_uint< sumBufRow << 3 > sumBufIndex;
	static ap_uint<sumBufRow << 3> MSB = 0;

		int rIndex = -1;
		calcLayerDetAndTrace_row:for (int r = 0; r < sumRow; r++)
		{
			if(rIndex < sumBufRow - 1)
			{
				rIndex++;
			}
			else{
				rIndex = 0;
			}

#ifdef DEBUG
			fout_rIndex << rIndex << std::endl;
#endif

	#pragma HLS LOOP_TRIPCOUNT min=1 max=601
			if(r < sumBufRow)
			{
				sumBufIndex.range(((r + 1) << 3) - 1, r << 3) = r;
#ifdef DEBUG
		fout_sumBufIndex << "将第" << r <<"行写入sumBuf第" << rIndex << "行"<< std::endl;
#endif
			}
			else{
	#ifdef DEBUG
			fout_sumBufIndex << "将第" << r <<"行写入sumBuf第" << rIndex << "行"<< std::endl;
	#endif
			MSB.range((sumBufRow << 3) - 1, (sumBufRow << 3) - 8) = sumBufIndex.range(7, 0);
			sumBufIndex = (sumBufIndex >> 8) | MSB;
	#ifdef DEBUG
			for(int i = 0; i < sumBufRow; i++)
			{
				fout_sumBufIndex << "第" << i <<"行索引为：" << sumBufIndex.range(((i + 1) << 3) - 1, i << 3) << std::endl;
			}
	#endif
			}
		calcLayerDetAndTrace_col:for (int c = 0; c < sumCol; c++)
		{
			static SurfHB dx = 0, dy = 0, dxy = 0, dt = 0, tt = 0;
#pragma HLS LOOP_TRIPCOUNT min=1 max=801
			sum >> sumBuf[rIndex][c];

			/*所有尺寸的模板遍历积分图*/
			calcLayerDetAndTrace_layer:for(int k = 0; k < nTotalLayers; k++)
			{
				/*考虑此模板的采样步长*/
				if((r & (((int)1 << sampleSteps[k]) - 1)) == 0 && (c & (((int)1 << sampleSteps[k]) - 1)) == 0)
				{
					int cOffset = c - (sizes[k]);

					if(r > sizes[k] && cOffset > 0)
					{
						int rOffset = 0;
						if(r < sumBufRow)
						{
							rOffset = r - sizes[k];
						}
						else{
							rOffset = (sumBufRow - sizes[k]) - 1;
						}

#ifdef DEBUG
						if(k == 0 && c == sumCol - 1)
						{
							fout_rOffset << rOffset << std::endl;
							for(int kr = 0; kr <= sizes[k]; kr++)
							{
								for(int kc = 1; kc < sumCol; kc++)
								{
									fout_sumBuf << sumBuf[sumBufIndex.range(((kr + 1 + rOffset) << 3) - 1, (kr + rOffset) << 3)][kc] << " ";
								}
								fout_sumBuf << std::endl;
							}
							fout_sumBuf << std::endl;
						}
#endif

						dx  = calcHaarPattern_x_y(sumBuf, Dx[k], sumBufIndex, rOffset, cOffset);
						dy  = calcHaarPattern_x_y(sumBuf, Dy[k], sumBufIndex, rOffset, cOffset);
						dxy = calcHaarPattern_xy(sumBuf, Dxy[k], sumBufIndex, rOffset, cOffset);
						dt  = dx * dy - 0.9 * dxy * dxy;

						switch (k) {
							case 0:
								det0 << dt;
#ifdef DEBUG
							detT[0]++;
#endif
								break;
							case 1:
								det1 << dt;
#ifdef DEBUG
							detT[1]++;
#endif
								break;
							case 2:
								det2 << dt;
#ifdef DEBUG
							detT[2]++;
#endif
								break;
							case 3:
								det3 << dt;
#ifdef DEBUG
							detT[3]++;
#endif
								break;
							case 4:
								det4 << dt;
#ifdef DEBUG
							detT[4]++;
#endif
								break;
							case 5:
								det5 << dt;
#ifdef DEBUG
							detT[5]++;
#endif
								break;
							case 6:
								det6 << dt;
#ifdef DEBUG
							detT[6]++;
#endif
								break;
							case 7:
								det7 << dt;
#ifdef DEBUG
							detT[7]++;
#endif
								break;
							case 8:
								det8 << dt;
#ifdef DEBUG
							detT[8]++;
#endif
								break;
							default:
								break;
						}

#ifdef DEBUG
						switch (k) {
						case 0:
							fout_det0 << dt <<" ";
							fout_det0Index <<"(" << r << "," << c << ") ";
							break;
						case 1:
							fout_det1 << dt <<" ";
							fout_det1Index <<"(" << r << "," << c << ") ";
							break;
						case 2:
							fout_det2 << dt <<" ";
							fout_det2Index <<"(" << r << "," << c << ") ";
							break;
						case 3:
							fout_det3 << dt <<" ";
							fout_det3Index <<"(" << r << "," << c << ") ";
							break;
						case 4:
							fout_det4 << dt <<" ";
							fout_det4Index <<"(" << r << "," << c << ") ";
							break;
						case 5:
							fout_det5 << dt <<" ";
							fout_det5Index <<"(" << r << "," << c << ") ";
							break;
						case 6:
							fout_det6 << dt <<" ";
							fout_det6Index <<"(" << r << "," << c << ") ";
							break;
						case 7:
							fout_det7 << dt <<" ";
							fout_det7Index <<"(" << r << "," << c << ") ";
							break;
						case 8:
							fout_det8 << dt <<" ";
							fout_det8Index <<"(" << r << "," << c << ") ";
							break;
						default:
							break;
						}
#endif
					}
				}
			}
		}
#ifdef DEBUG
		fout_sum2buf << std::endl;
		fout_det0 << std::endl;
		fout_det1 << std::endl;
		fout_det2 << std::endl;
		fout_det3 << std::endl;
		fout_det4 << std::endl;
		fout_det5 << std::endl;
		fout_det6 << std::endl;
		fout_det7 << std::endl;
		fout_det8 << std::endl;
		fout_det0Index << std::endl;
		fout_det1Index << std::endl;
		fout_det2Index << std::endl;
		fout_det3Index << std::endl;
		fout_det4Index << std::endl;
		fout_det5Index << std::endl;
		fout_det6Index << std::endl;
		fout_det7Index << std::endl;
		fout_det8Index << std::endl;
#endif
	}
#ifdef DEBUG
	std::cout << "det0~det8数据量分别为：";
	for(int i = 0; i < nTotalLayers; i++)
	{
		std::cout << detT[i] <<" ";
	}
	std::cout << std::endl;
#endif
}

void SURF::findCharacteristicPoint(
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
		int* pointNumber)
{
#ifdef DEBUG
	static int detT[nTotalLayers] = {0};
#endif
	static const int detRow[nTotalLayers] = {591, 585, 579, 293, 287, 281, 144, 138, 132};
	static const int detCol[nTotalLayers] = {791, 785, 779, 393, 387, 381, 194, 188, 182};
	static int sizes[nTotalLayers] = {9, 15, 21, 15, 27, 39, 27, 51, 75};						// 每一层用的 Harr模板的大小
	static int sampleSteps[nTotalLayers] = {0, 0, 0, 1, 1, 1, 2, 2, 2};				// 每一层用的采样步长是2的sampleSteps[nTotalLayers]次幂
	static int middleIndices[nMiddleLayers] = {1, 4, 7};			// 中间层的索引值

	static KeyPoint point;
	static float N1[3][3][791];
	static float N2[3][3][393];
	static float N3[3][3][194];

	static int tmpPointNum = 0;
	static ap_uint<2> bRow[3] = {0};
	static int midIndex = 0;
	/*中间层第一行或第一列元素在原图中的坐标偏移量*/
	static int iSOffset = 0;
	findCharacteristicPoint_layer:for(int ly = 0; ly < nMiddleLayers; ly++)
	{
		/*必须在每层模板都初始化rIndex，否则切换到其它层时rIndex可能不是从0开始*/
		int rIndex = -1;

		ap_uint<6>RowIndex = 0;
		static ap_uint<6> MSB = 0;

#ifdef DEBUG
		fout_NIndex << "====================================初始索引为====================================" << std::endl;
	for(int Nr = 0; Nr < 3; Nr++)
	{
		fout_NIndex << "第" << Nr <<"行索引为：" << RowIndex.range(((Nr + 1) << 1) - 1, Nr << 1) << std::endl;
	}
	fout_NIndex << "==================================================================================" << std::endl;
#endif

		midIndex = middleIndices[ly];

		iSOffset = ((sizes[midIndex - 1] - 1) >> 1);

		findCharacteristicPoint_r0:for(int r = 0; r < detRow[midIndex - 1]; r++)
			{
			if(rIndex < 3 - 1)
			{
				rIndex++;
			}
			else{
				rIndex = 0;
			}

#ifdef DEBUG
				fout_NIndex << "将第" << r <<"行写入N的第" << rIndex << "行"<< std::endl;
#endif

				if(r < 3){
					RowIndex.range(((r + 1) << 1) - 1, r << 1) = r;
				}
				else{
					MSB.range(5,4) = RowIndex.range(1,0);
					RowIndex = (RowIndex >> 2) | MSB;
				}

#ifdef DEBUG
		for(int Nr = 0; Nr < 3; Nr++)
		{
			fout_NIndex << "第" << Nr <<"行索引为：" << RowIndex.range(((Nr + 1) << 1) - 1, Nr << 1) << std::endl;
		}
#endif
				bRow[0] = RowIndex.range(1,0);
				bRow[1] = RowIndex.range(3,2);
				bRow[2] = RowIndex.range(5,4);

				findCharacteristicPoint_c0:for(int c = 0; c < detCol[midIndex - 1]; c++)
				{
					switch (ly) {
						case 0:
							det0 >> N1[0][rIndex][c];
#ifdef DEBUG
							detT[0]++;
#endif
							break;
						case 1:
							det3 >> N2[0][rIndex][c];
#ifdef DEBUG
							detT[3]++;
#endif
							break;
						case 2:
							det6 >> N3[0][rIndex][c];
#ifdef DEBUG
							detT[6]++;
#endif
							break;
						default:
							break;
					}

					if(r >= margin && r < detRow[midIndex] + margin && c >= margin && c < detCol[midIndex] + margin)
					{
						switch (ly) {
							case 0:
								det1 >> N1[1][rIndex][c];
#ifdef DEBUG
							detT[1]++;
#endif
								break;
							case 1:
								det4 >> N2[1][rIndex][c];
#ifdef DEBUG
							detT[4]++;
#endif
								break;
							case 2:
								det7 >> N3[1][rIndex][c];
#ifdef DEBUG
							detT[7]++;
#endif
								break;
							default:
								break;
						}
					}

					int maxMargin = (margin << 1);
					if(r >= maxMargin && r < detRow[midIndex + 1] + maxMargin && c >= maxMargin && c < detCol[midIndex + 1] + maxMargin)
					{
						switch (ly) {
							case 0:
								det2 >> N1[2][rIndex][c];
#ifdef DEBUG
							detT[2]++;
#endif
								break;
							case 1:
								det5 >> N2[2][rIndex][c];
#ifdef DEBUG
							detT[5]++;
#endif
								break;
							case 2:
								det8 >> N3[2][rIndex][c];
#ifdef DEBUG
							detT[8]++;
#endif
								break;
							default:
								break;
						}
					}

					int firstIndex = maxMargin + 2;
					static float val0  = 0;
					static int cOffset = -2;

					/*开始寻找特征点*/
					if(r >= firstIndex && r < detRow[midIndex + 1] + maxMargin && c >= firstIndex && c < detCol[midIndex + 1] + maxMargin)
					{
						switch (ly) {
							case 0:
								val0 = N1[1][bRow[1]][c - 1];
								break;
							case 1:
								val0 = N2[1][bRow[1]][c - 1];
								break;
							case 2:
								val0 = N3[1][bRow[1]][c - 1];
								break;
							default:
								break;
						}

						if(val0 > hessianThreshold)
						{
#ifdef DEBUG
							fout_val0 << val0 << std::endl;
#endif
							switch (ly) {
								case 0:
								{
									if(
											val0 > N1[0][bRow[0]][c - 2] && val0 > N1[0][bRow[0]][c - 1] && val0 > N1[0][bRow[0]][c]
									     && val0 > N1[0][bRow[1]][c - 2] && val0 > N1[0][bRow[1]][c - 1] && val0 > N1[0][bRow[1]][c]
										 && val0 > N1[0][bRow[2]][c - 2] && val0 > N1[0][bRow[2]][c - 1] && val0 > N1[0][bRow[2]][c]
										 && val0 > N1[1][bRow[0]][c - 2] && val0 > N1[1][bRow[0]][c - 1] && val0 > N1[1][bRow[0]][c]
										 && val0 > N1[1][bRow[1]][c - 2] 							     && val0 > N1[1][bRow[1]][c]
										 && val0 > N1[1][bRow[2]][c - 2] && val0 > N1[1][bRow[2]][c - 1] && val0 > N1[1][bRow[2]][c]
									     && val0 > N1[2][bRow[0]][c - 2] && val0 > N1[2][bRow[0]][c - 1] && val0 > N1[2][bRow[0]][c]
										 && val0 > N1[2][bRow[1]][c - 2] && val0 > N1[2][bRow[1]][c - 1] && val0 > N1[2][bRow[1]][c]
										 && val0 > N1[2][bRow[2]][c - 2] && val0 > N1[2][bRow[2]][c - 1] && val0 > N1[2][bRow[2]][c])
									{
										/*将坐标转换到原图像中的坐标*/
										point.range(31,17) = (r << sampleSteps[midIndex]) + iSOffset;
										point.range(16,2) = (c << sampleSteps[midIndex]) + iSOffset;
										point.range(1,0) = 0;
										*(keyPoints + tmpPointNum) = point;

#ifdef DEBUG
										fout_keyPoint << "(" << point.range(31,17) << "," << point.range(16,2) << ")" << std::endl;
#endif
										tmpPointNum++;
									}
								}
									break;
								case 1:
								{
									if(
											val0 > N2[0][bRow[0]][c - 2] && val0 > N2[0][bRow[0]][c - 1] && val0 > N2[0][bRow[0]][c]
									     && val0 > N2[0][bRow[1]][c - 2] && val0 > N2[0][bRow[1]][c - 1] && val0 > N2[0][bRow[1]][c]
										 && val0 > N2[0][bRow[2]][c - 2] && val0 > N2[0][bRow[2]][c - 1] && val0 > N2[0][bRow[2]][c]
										 && val0 > N2[1][bRow[0]][c - 2] && val0 > N2[1][bRow[0]][c - 1] && val0 > N2[1][bRow[0]][c]
										 && val0 > N2[1][bRow[1]][c - 2] 							     && val0 > N2[1][bRow[1]][c]
										 && val0 > N2[1][bRow[2]][c - 2] && val0 > N2[1][bRow[2]][c - 1] && val0 > N2[1][bRow[2]][c]
									     && val0 > N2[2][bRow[0]][c - 2] && val0 > N2[2][bRow[0]][c - 1] && val0 > N2[2][bRow[0]][c]
										 && val0 > N2[2][bRow[1]][c - 2] && val0 > N2[2][bRow[1]][c - 1] && val0 > N2[2][bRow[1]][c]
										 && val0 > N2[2][bRow[2]][c - 2] && val0 > N2[2][bRow[2]][c - 1] && val0 > N2[2][bRow[2]][c])
									{
										point.range(31,17) = (r << sampleSteps[midIndex]) + iSOffset;
										point.range(16,2) = (c << sampleSteps[midIndex]) + iSOffset;
										point.range(1,0) = 1;
										*(keyPoints + tmpPointNum) = point;

#ifdef DEBUG
										fout_keyPoint << "(" << point.range(31,17) << "," << point.range(16,2) << ")" << std::endl;
#endif
										tmpPointNum++;
									}
								}
									break;
								case 2:
								{
									if(
											val0 > N3[0][bRow[0]][c - 2] && val0 > N3[0][bRow[0]][c - 1] && val0 > N3[0][bRow[0]][c]
									     && val0 > N3[0][bRow[1]][c - 2] && val0 > N3[0][bRow[1]][c - 1] && val0 > N3[0][bRow[1]][c]
										 && val0 > N3[0][bRow[2]][c - 2] && val0 > N3[0][bRow[2]][c - 1] && val0 > N3[0][bRow[2]][c]
										 && val0 > N3[1][bRow[0]][c - 2] && val0 > N3[1][bRow[0]][c - 1] && val0 > N3[1][bRow[0]][c]
										 && val0 > N3[1][bRow[1]][c - 2] 							     && val0 > N3[1][bRow[1]][c]
										 && val0 > N3[1][bRow[2]][c - 2] && val0 > N3[1][bRow[2]][c - 1] && val0 > N3[1][bRow[2]][c]
									     && val0 > N3[2][bRow[0]][c - 2] && val0 > N3[2][bRow[0]][c - 1] && val0 > N3[2][bRow[0]][c]
										 && val0 > N3[2][bRow[1]][c - 2] && val0 > N3[2][bRow[1]][c - 1] && val0 > N3[2][bRow[1]][c]
										 && val0 > N3[2][bRow[2]][c - 2] && val0 > N3[2][bRow[2]][c - 1] && val0 > N3[2][bRow[2]][c])
									{
										point.range(31,17) = (r << sampleSteps[midIndex]) + iSOffset;
										point.range(16,2) = (c << sampleSteps[midIndex]) + iSOffset;
										point.range(1,0) = 2;
										*(keyPoints + tmpPointNum) = point;

#ifdef DEBUG
										fout_keyPoint << "(" << point.range(31,17) << "," << point.range(16,2) << ")" << std::endl;
#endif
										tmpPointNum++;
									}
								}
									break;
								default:
									break;
							}
						}
					}
				}
			}
	}
#ifdef DEBUG
	std::cout << "det0~det8读取次数分别为：";
	for(int i = 0; i < nTotalLayers; i++)
	{
		std::cout << detT[i] <<" ";
	}
	std::cout << std::endl;
		std::cout << "检测到" << tmpPointNum << "个特征点" << std::endl;
#endif
		*pointNumber = tmpPointNum;
		tmpPointNum = 0;
}

void SURF::HessianDetector(hls::stream<int>& sum,  KeyPoint* keyPoints, int* pointNumber, int nOctaves, int nOctaveLayers, SurfHB hessianThreshold)
{

	hls::stream<float> dets[nTotalLayers];				// 每一层图像 对应的 Hessian行列式的值
	hls::stream<float> traces[nTotalLayers];			// 每一层图像 对应的 Hessian矩阵的迹的值

	calcLayerDetAndTrace(
			sum,
			dets[0],
			dets[1],
			dets[2],
			dets[3],
			dets[4],
			dets[5],
			dets[6],
			dets[7],
			dets[8],
			traces[0]);

	findCharacteristicPoint(
			dets[0],
			dets[1],
			dets[2],
			dets[3],
			dets[4],
			dets[5],
			dets[6],
			dets[7],
			dets[8],
			traces[0],
			hessianThreshold,
			keyPoints,
			pointNumber);

}

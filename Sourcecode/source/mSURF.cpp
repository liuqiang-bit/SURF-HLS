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
std::ofstream fout_NIndex("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/NIndex.txt");
std::ofstream fout_val0("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/val0.txt");
std::ofstream fout_keyPoint("C:/Users/GUDONG/Desktop/HLS_SURF/common/OutputFile/keyPoint.txt");
#endif

static const int detRow[nTotalLayers] = {559, 554,548, 277, 272, 266, 136, 130, 125};
static const int detCol[nTotalLayers] = {858, 853, 847, 426, 421, 415, 211, 205, 200};

/*��ʾÿ��ģ�����м��ģ�������߲�ģ��İ뾶֮��*/
static int margin[nMiddleLayers] = {3, 6, 12};

SURF::SURF()
{

}

void SURF::integralImg(AXI_STREAM_24& video_in, hls::stream<int>& dst)
{
	static ap_axiu<24,1,1,1> s;
	static int s2i = 0;
	static int pix_val = 0;
	static int d = 0;

	/*�������л��ֽ��*/
	static int buf[2][COL] = {0};
#pragma HLS ARRAY_PARTITION variable=buf complete dim=1

	ap_uint<1> preRow = 0, curtRow = ~preRow;

	integralImg_row:for (ap_uint<11> r = 0; r < sumRow; r++)
	{
		preRow = ~preRow; curtRow = ~curtRow;
		integralImg_col:for (ap_uint<11> c = 0; c < sumCol; c++)
		{
			/*����ͼ��һ�е�һ��Ϊ0*/
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

				/*�����Ż���أ�buf[preRow][c] �� buf[preRow][c-1]��Ҫ2�����ڲ��ܶ�ȡ��*/
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

float SURF::calcHaarPattern(int sumBuf[sumBufRow][sumCol], SurfHB box[4], int n, ap_uint< sumBufRow << 3 > sumBufIndex, int cOffset)
{

	float d = 0;
	calcHaarPattern_kn:for(int kn = 0; kn < n; kn++)
	{
		d += (
			 sumBuf[sumBufIndex.range(((box[kn].y2 + 1) << 3) - 1, ((box[kn].y2 + 1) << 3) - 8)][box[kn].x2 + cOffset]
			-sumBuf[sumBufIndex.range(((box[kn].y2 + 1) << 3) - 1, ((box[kn].y2 + 1) << 3) - 8)][box[kn].x1 + cOffset]
			-sumBuf[sumBufIndex.range(((box[kn].y1 + 1) << 3) - 1, ((box[kn].y1 + 1) << 3) - 8)][box[kn].x2 + cOffset]
			+sumBuf[sumBufIndex.range(((box[kn].y1 + 1) << 3) - 1, ((box[kn].y1 + 1) << 3) - 8)][box[kn].x1 + cOffset]
			 ) * box[kn].n;
	}
	return d;
}

void SURF::createHessianBox(const ap_int<5> box[4][5], SurfHB dst[4], int n, int oldSize, int newSize, int cols)
{
	float ratio = (float)newSize / oldSize;
	createHessianBox_i:for (int i = 0; i < n; i++)
	{
		dst[i].x1 = hls::round(box[i][0] * ratio);
		dst[i].y1 = hls::round(box[i][1] * ratio);
		dst[i].x2 = hls::round(box[i][2] * ratio);
		dst[i].y2 = hls::round(box[i][3] * ratio);

		dst[i].n = box[i][4] / (float)((dst[i].y2 - dst[i].y1) * (dst[i].x2 - dst[i].x1));

#ifdef DEBUG
		fout_HBox << dst[i].x1 <<" "<< dst[i].y1 <<" "<< dst[i].x2 <<" "<< dst[i].y2 <<" "<< dst[i].n << std::endl;
#endif
	}
#ifdef DEBUG
	fout_HBox << std::endl;
#endif
}

void SURF::calcLayerDetAndTrace(
		hls::stream<int>& sum,
		int size[nTotalLayers],
		int sampleStep[nTotalLayers],
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

	static int sample_r[nTotalLayers];
	static int sample_c[nTotalLayers];

	//������ı߽��С
	static int margin[nTotalLayers];

	/*��������˲���*/
	static const int NX = 3, NY = 3, NXY = 4;
	static const ap_int<5> dx_s[NXY][5]  = { { 0, 2, 3, 7, 1 }, { 3, 2, 6, 7, -2 }, { 6, 2, 9, 7, 1 }, {0, 0, 0, 0, 0} };
	static const ap_int<5> dy_s[NXY][5]  = { { 2, 0, 7, 3, 1 }, { 2, 3, 7, 6, -2 }, { 2, 6, 7, 9, 1 }, {0, 0, 0, 0, 0} };
	static const ap_int<5> dxy_s[NXY][5] = { { 1, 1, 4, 4, 1 }, { 5, 1, 8, 4, -1 }, { 1, 5, 4, 8, -1 }, { 5, 5, 8, 8, 1 } };

	static SurfHB Dx[nTotalLayers][NXY], Dy[nTotalLayers][NXY], Dxy[nTotalLayers][NXY];
	calcLayerDetAndTrace_hessian:for(int i = 0; i < nTotalLayers; i++)
	{
		/*���Ż��ռ䣬Ŀǰ����ķ�FF��LUT,�ɿ���ȥ���˲��裬ֱ�ӵ������ɺõ�hessianBox*/
		createHessianBox(dx_s, Dx[i], NX, 9, size[i], sumCol);
		createHessianBox(dy_s, Dy[i], NY, 9, size[i], sumCol);
		createHessianBox(dxy_s, Dxy[i], NXY, 9, size[i], sumCol);
	}

	static int sumBuf[sumBufRow][sumCol];
	ap_uint< sumBufRow << 3 > sumBufIndex;
	static ap_uint<sumBufRow << 3> MSB = 0;

#ifdef DEBUG
	fout_sumBufIndex << "====================================��ʼ����Ϊ====================================" << std::endl;
#endif
	calcLayerDetAndTrace_initbufIndex:for(int i = 0; i < sumBufRow; i++)
	{
		sumBufIndex.range(((i + 1) << 3) - 1, i << 3) = i;
#ifdef DEBUG
		fout_sumBufIndex << "��" << i <<"������Ϊ��" << sumBufIndex.range(((i + 1) << 3) - 1, i << 3) << std::endl;
#endif
	}
#ifdef DEBUG
	fout_sumBufIndex << "==================================================================================" << std::endl;
#endif

	static int rIndex = 0;
	calcLayerDetAndTrace_row:for (int r = 0; r < sumRow; r++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=567
		rIndex = r % sumBufRow;
#ifdef DEBUG
		fout_sumBufIndex << "����" << r <<"��д��sumBuf��" << rIndex << "��"<< std::endl;
#endif
		MSB.range((sumBufRow << 3) - 1, (sumBufRow << 3) - 8) = sumBufIndex.range(7, 0);
		sumBufIndex = (sumBufIndex >> 8) | MSB;
#ifdef DEBUG
		for(int i = 0; i < sumBufRow; i++)
		{
			fout_sumBufIndex << "��" << i <<"������Ϊ��" << sumBufIndex.range(((i + 1) << 3) - 1, i << 3) << std::endl;
		}
#endif
		calcLayerDetAndTrace_col:for (int c = 0; c < sumCol; c++)
		{
			static float dx = 0, dy = 0, dxy = 0, dt = 0, tt = 0;
#pragma HLS LOOP_TRIPCOUNT min=1 max=866
			sum >> sumBuf[rIndex][c];
#ifdef DEBUG
			fout_sum2buf << sumBuf[rIndex][c] << " ";
#endif
			/*���гߴ��ģ���������ͼ*/

			for(int k = 0; k < nTotalLayers; k++)
			{
				/*���Ǵ�ģ��Ĳ�������*/
				if((r & (((int)1 << sampleStep[k]) - 1)) == 0 && (c & (((int)1 << sampleStep[k]) - 1)) == 0)
				{
					int cOffset = c - Dx[k][3].x2;

					if(r >= Dy[k][3].y2 && cOffset >= 0)
					{
						dx  = calcHaarPattern(sumBuf, Dx[k], NX, sumBufIndex, cOffset);
						dy  = calcHaarPattern(sumBuf, Dy[k], NY, sumBufIndex, cOffset);
						dxy = calcHaarPattern(sumBuf, Dxy[k], NXY , sumBufIndex, cOffset);
						dt  = dx * dy - 0.9 * dxy * dxy;

						switch (k) {
							case 0:
								det0 << dt;
								break;
							case 1:
								det1 << dt;
								break;
							case 2:
								det2 << dt;
								break;
							case 3:
								det3 << dt;
								break;
							case 4:
								det4 << dt;
								break;
							case 5:
								det5 << dt;
								break;
							case 6:
								det6 << dt;
								break;
							case 7:
								det7 << dt;
								break;
							case 8:
								det8 << dt;
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
}

void SURF::findCharacteristicPoint(
		int size[nTotalLayers],
		int sampleStep[nTotalLayers],
		int middleIndices[nMiddleLayers],
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
		float hessianThreshold,
		int pointNumber)
{

	static KeyPoint point;
	static float N1[3][3][858];
	static float N2[3][3][426];
	static float N3[3][3][211];

	/*��������ԭͼ�е�����*/
	static int center_r = 0;
	static int center_c = 0;
	int rIndex = 0;

	static ap_uint<2> bRow[3] = {0};
	static int midIndex = 0;
	/*�м���һ�л��һ��Ԫ����ԭͼ�е�����ƫ����*/
	static int iSOffset = 0;
	findCharacteristicPoint_layer:for(int ly = 0; ly < nMiddleLayers; ly++)
	{
		ap_uint<6>RowIndex = 0x24;
		static ap_uint<6> MSB = 0;

#ifdef DEBUG
		fout_NIndex << "====================================��ʼ����Ϊ====================================" << std::endl;
	for(int Nr = 0; Nr < 3; Nr++)
	{
		fout_NIndex << "��" << Nr <<"������Ϊ��" << RowIndex.range(((Nr + 1) << 1) - 1, Nr << 1) << std::endl;
	}
	fout_NIndex << "==================================================================================" << std::endl;
#endif

		midIndex = middleIndices[ly];

		iSOffset = ((size[midIndex - 1] - 1) >> 1);

		findCharacteristicPoint_r0:for(int r = 0; r < detRow[midIndex - 1]; r++)
			{
				rIndex = r % 3;
//			if(rIndex < 3)
//			{
//				rIndex++;
//			}
//			else{
//				rIndex = 0;
//			}
#ifdef DEBUG
				fout_NIndex << "����" << r <<"��д��N�ĵ�" << rIndex << "��"<< std::endl;
#endif
				MSB.range(5,4) = RowIndex.range(1,0);
				RowIndex = (RowIndex >> 2) | MSB;
#ifdef DEBUG
		for(int Nr = 0; Nr < 3; Nr++)
		{
			fout_NIndex << "��" << Nr <<"������Ϊ��" << RowIndex.range(((Nr + 1) << 1) - 1, Nr << 1) << std::endl;
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
							break;
						case 1:
							det3 >> N2[0][rIndex][c];
							break;
						case 2:
							det6 >> N3[0][rIndex][c];
							break;
						default:
							break;
					}

					if(c >= margin[ly] && c < detCol[midIndex] + margin[ly])
					{
						switch (ly) {
							case 0:
								det1 >> N1[1][rIndex][c];
								break;
							case 1:
								det4 >> N2[1][rIndex][c];
								break;
							case 2:
								det7 >> N3[1][rIndex][c];
								break;
							default:
								break;
						}
					}

					static int maxMargin = (margin[ly] << 1);
					if(c >= maxMargin && c < detCol[midIndex + 1] + maxMargin)
					{
						switch (ly) {
							case 0:
								det2 >> N1[2][rIndex][c];
								break;
							case 1:
								det5 >> N2[2][rIndex][c];
								break;
							case 2:
								det8 >> N3[2][rIndex][c];
								break;
							default:
								break;
						}
					}

					static int firstIndex = maxMargin + 2;
					static float val0  = 0;
					static int cOffset = -2;

					/*��ʼѰ��������*/
					if(r >= 3 && c >= firstIndex && c < detCol[midIndex + 1] + maxMargin)
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
										/*������ת����ԭͼ���е�����*/
										point.y = (r << sampleStep[midIndex]) + iSOffset;
										point.x = (c << sampleStep[midIndex]) + iSOffset;
#ifdef DEBUG
										fout_keyPoint << "(" << point.y << "," << point.x << ")" << std::endl;
#endif
										pointNumber++;
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
										point.y = (r << sampleStep[midIndex]) + iSOffset;
										point.x = (c << sampleStep[midIndex]) + iSOffset;
#ifdef DEBUG
										fout_keyPoint << "(" << point.y << "," << point.x << ")" << std::endl;
#endif
										pointNumber++;
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
										point.y = (r << sampleStep[midIndex]) + iSOffset;
										point.x = (c << sampleStep[midIndex]) + iSOffset;
#ifdef DEBUG
										fout_keyPoint << "(" << point.y << "," << point.x << ")" << std::endl;
#endif
										pointNumber++;
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
		std::cout << "��⵽" << pointNumber << "��������" << std::endl;
#endif
}

void SURF::HessianDetector(hls::stream<int>& sum,  int pointNumber, int nOctaves, int nOctaveLayers, float hessianThreshold)
{
	static const int SAMPLE_STEP = 0;

	hls::stream<float> dets[nTotalLayers];		// ÿһ��ͼ�� ��Ӧ�� Hessian����ʽ��ֵ
	hls::stream<float> traces[nTotalLayers];	// ÿһ��ͼ�� ��Ӧ�� Hessian����ļ���ֵ

	static int sizes[nTotalLayers];						// ÿһ���õ� Harrģ��Ĵ�С
	static int sampleSteps[nTotalLayers];				// ÿһ���õĲ�������
	static int middleIndices[nMiddleLayers];			// �м�������ֵ

//	static int detCols[nTotalLayers];
	static int index = 0, middleIndex = 0, step = SAMPLE_STEP;

	HessianDetector_octave:for (int octave = 0; octave < nOctaves; octave++)
	{

		for (int layer = 0; layer < nOctaveLayers + 2; layer++)
		{
//
//			static int dr = (sumRow - 1) / step;
//			static int dc = (sumCol - 1) / step;

//			detCols[index] = dc;
			sizes[index] = ((2 << octave) * (layer + 1) + 1) * 3;

			sampleSteps[index] = step;

			if (0 < layer && layer <= nOctaveLayers)
			{
				middleIndices[middleIndex++] = index;
#ifdef DEBUG
				fout_middleIndices << middleIndices[middleIndex - 1] << std::endl;
#endif
			}

#ifdef DEBUG
			fout_size_sampleSteps << sizes[index] <<" "<< ((int)1 << sampleSteps[index]) << std::endl;
#endif
			index++;
		}
		step += 1;
	}

	calcLayerDetAndTrace(
			sum,
			sizes,
			sampleSteps,
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
			sizes,
			sampleSteps,
			middleIndices,
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
			pointNumber);

}

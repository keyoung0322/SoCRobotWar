#include "HSIwithBinary.h"	

void HSI(U16 *buf, unsigned char *sImg, unsigned char *iImg, U16 *hImg)
{
	int i;
	int imageSize = WIDTH * HEIGHT;
	unsigned short lookValue;

	const int outSize = WIDTH * HEIGHT;
		
	memset(hImg, 0, outSize * sizeof(unsigned short));		//메모리 초기화
	memset(sImg, 0, outSize);
	memset(iImg, 0, outSize);

	for(i = imageSize - 1; i >= 0; --i) {		//최적화된 코드
		lookValue = buf[i];						//룩업테이블에서 값을 미리 뽑아와준다

		hImg[i] = h_lookup[lookValue];
		sImg[i] = s_lookup[lookValue];
		iImg[i] = i_lookup[lookValue];
	}
}

void binary(U16 *hImg, U8 *sImg, U8 *iImg, U8 *modify, const int width, const int height/*, int *global_white*/)
{
	int margin = 20;
	int black_th = 25;
	//int white_th = 230;
	int i, j;
	int index;

	int green_h_max = 160 + margin;		//프로그램 최적화를 위해 추출값을 미리 계산한다
	int green_h_min = 110 - margin;		//						"
	//int green_s_max = 200 + margin;		//						"
	int green_s_min = 70 - margin;		//						"

	//int imgSize = width * height;

	for(i = 0; i < height; i++) {
		index = i * WIDTH;

		for(j = 0; j < width; j++) {		// 초록색은 white
			if( (sImg[index + j] > green_s_min /*&& sImg[index + j] < green_s_max*/) &&
				(hImg[index + j] > 85 && hImg[index + j] < green_h_max))
				modify[index + j] = 255;
			else
				modify[index + j] = 0;
		}
		for(j = 0; j < width; j++) {		// 검은색은 gray
			if(iImg[index + j] < black_th)
				modify[index + j] = 128;
		}
		/*for(j = 30; j < width; j++) {
			if(iImg[index + j] > white_th)
				++(*global_white);
		}*/
	}
}

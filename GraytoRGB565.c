#include "GraytoRGB565.h"



//Gray ������ RGB565�� ��ȯ - LCD��¿�
int GraytoRGB565(unsigned char *gray, unsigned short *rgb565, int inWidth, int inHeight, int outWidth, int outHeight)
{
	if( !gray || !rgb565 )
	{
		printf("%s: invaild pointer\n", __func__);
		return -1;
	}

	memset(rgb565, 0, outWidth * outHeight * sizeof(unsigned short));

	int row, col;
	int inIndex, outIndex;
	unsigned char grayData;
	unsigned short rgbData;

	for(row = 0; row < inHeight; row++)
	{
		inIndex = row * inWidth;//gray �ε���
		outIndex = row * outWidth;//RGB565 �ε���
		//printf("%d, %d\n", inIndex, outIndex);

		for(col = 0; col < inWidth; col++)
		{
			grayData = gray[inIndex + col];
			
			rgbData = MAKE_RGB565(grayData, grayData, grayData);
			//rgbData = MAKE_RGB565(255, 0, 0);
			rgb565[outIndex + col] = rgbData;
		}
	}

	return 0;
}

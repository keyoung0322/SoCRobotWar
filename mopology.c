#include "mopology.h"

void binaryDilation(unsigned char *InImg, unsigned char *OutImg, int height, int width, struct structElement *pSE)
{
	int flagPassed;
	int row, col;
	int index;
	int i, j, k;

	int white = 0;
	int black = 0;

	for(i = 0; i < height; i++)
	{
		index = i * WIDTH;//�ӵ� ������ ���� j for�� ���� �̸� ���
		for(j = 0; j < width; j++)
		{
			flagPassed = 0;

			for(k = 0; k < pSE[0].count; k++)
			{
				row = i + pSE[k].r;
				col = j + pSE[k].c;

				if( row >= 0 && row < height && col >= 0 && col < width )
				{
					//�ϳ��� ���ǿ� ���� ������ ����
					if( *(InImg + row*WIDTH + col) == WHITE ){
						flagPassed++;
						white++;
					}
					else if( *(InImg + row*WIDTH + col) == 128 ){
						flagPassed++;
						black++;
					}
				}//if
				else{
					break;
				}
			}//for

			if( flagPassed > 0 ){
				if(white > black) {
					*(OutImg + index + j) = WHITE;
				}
				else {
					*(OutImg + index + j) = 128;
				}
			}
			else {
				*(OutImg + index + j) = BLACK;
			}

			white = 0;
			black = 0;
		}//for
	}//for
}

void binaryErosion(unsigned char *InImg, unsigned char *OutImg, int height, int width, struct structElement *pSE)
{
	int flagPassed;
	int row, col;
	int index;
	int i, j, k;

	int white = 0;
	int black = 0;

	//ħ��
	for(i = 0; i < height; i++)
	{
		index = i * WIDTH;//�ӵ� ������ ���� j for�� ���� �̸� ���
		for(j = 0; j < width; j++)
		{
			//��� Ž�� ����
			flagPassed = 1;
			for(k = 0; k < pSE[0].count; k++)
			{
				row = i + pSE[k].r;
				col = j + pSE[k].c;

				if( row>= 0 && row<height && col>= 0 && col<width )
				{
					if( *(InImg + row*WIDTH + col) == BLACK){
						flagPassed = 0;
						break;
					}

					if(*(InImg + row*WIDTH + col) == WHITE) {
						white++;
					}
					else if(*(InImg + row*WIDTH + col) == 128) {
						black++;
					}
				}//if
				else
				{
					flagPassed = 0;
					break;
				}//else
			}//for

			if( flagPassed ){
				if(white > black) {
					*(OutImg + index + j) = WHITE;
				}
				else {
					*(OutImg + index + j) = 128;//(128 == GREY(������))
				}
			}
			else{
				*(OutImg + index + j) = BLACK;
			}

			white = 0;	//�ʱ�ȭ
			black = 0;	//�ʱ�ȭ
		}//for
	}//for
}

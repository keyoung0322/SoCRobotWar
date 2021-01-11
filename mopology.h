#include "amazon2_sdk.h"

struct structElement
{
	char r;
	char c;
	short count;
};
static struct structElement gSE[9] = {	{0,0,9}, {0,1,9},{0,2,9},
	{1,0,9}, {1,1,9}, {1,2,9},
	{2,0,9}, {2,1,9}, {2,2,9} };

void binaryDilation(unsigned char *InImg, unsigned char *OutImg, int height, int width, struct structElement *pSE);
void binaryErosion(unsigned char *InImg, unsigned char *OutImg, int height, int width, struct structElement *pSE);

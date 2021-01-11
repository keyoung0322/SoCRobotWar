#include <stdio.h>
#include <stdlib.h>

#include "amazon2_sdk.h"
#include "hImg.h"
#include "iImg.h"
#include "sImg.h"

void HSI(U16 *buf, unsigned char *s, unsigned char *i, U16 *h);		//영상을 HSI변환한다(전체영역)

//영상을 이진화 한다
void binary(U16 *hImg, U8 *sImg, U8 *iImg, U8 *modify, const int width, const int height/*, int *global_white*/);
//이진화 할 때 영역을 제한한다(파라미터로 가로세로를 받는다...?)

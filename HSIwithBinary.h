#include <stdio.h>
#include <stdlib.h>

#include "amazon2_sdk.h"
#include "hImg.h"
#include "iImg.h"
#include "sImg.h"

void HSI(U16 *buf, unsigned char *s, unsigned char *i, U16 *h);		//������ HSI��ȯ�Ѵ�(��ü����)

//������ ����ȭ �Ѵ�
void binary(U16 *hImg, U8 *sImg, U8 *iImg, U8 *modify, const int width, const int height/*, int *global_white*/);
//����ȭ �� �� ������ �����Ѵ�(�Ķ���ͷ� ���μ��θ� �޴´�...?)

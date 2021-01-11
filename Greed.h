
#include "amazon2_sdk.h"
#include "graphic_api.h"

#define FPGA_IMAGE_R	180	//���� ����ũ��
#define FPGA_IMAGE_C	120	//���� ����ũ��

/*
#define G_R_NUMBER		10	//�׸��� ���ΰ���
#define G_C_NUMBER		10	//�׸��� ���ΰ���

#define G_WIDTH			18	//�׸��� ���α���
#define G_HEIGHT		12	//�׸��� ���α���
*/

#define G_C_NUM			6
#define G_C_WIDTH		20

#define WIDTH_MODIFY	105

#define element			int
#define element_num		16

#define	GREY			128

#define GREED_HISTORY 10;

struct greed{
	element green_count;	//�ʷ� ���� ����
	element blue_count;		//�Ķ� ���� ����
	element red_count;		//���� ���� ����

	element green_x;		//�ʷ��� X��ǥ ��
	element green_y;		//�ʷ��� Y��ǥ ��
	element blue_x;			//�Ķ��� X��ǥ ��
	element blue_y;			//�Ķ��� Y��ǥ ��
	element red_x;			//������ X��ǥ ��
	element red_y;			//������ Y��ǥ ��

	element green_center_x;	//�ʷ��� X�߽�
	element green_center_y;	//�ʷ��� Y�߽�
	element blue_center_x;	//�Ķ��� X�߽�
	element blue_center_y;	//�Ķ��� Y�߽�
	element red_center_x;	//������ X�߽�
	element red_center_y;	//������ Y�߽�

	element black_count;

	float green_ratio;		//�ʷϻ� �߽� ����(���� ���� / �˻��ϴ� ��ü ����)
	float blue_ratio;		//�Ķ���					"
	float red_ratio;		//������	
};
//�Ǽ� �ڷ���: �� �׸����� �� R, G, B�� �߽� ���� ����

struct greed_count{			//�׸��� ����(�� ���� ����) ���� �� �迭�� ���� ����ü <- ��ɷ� ������ ���ش�
	int idx;				//�׸��� �ε���
	int count;				//�׸��� ���� ������� ����
};

struct greed_history {
	int last_index;
	int green_count;
	int black_count;
};
void greed_fill(U16* hImg, U8* sImg, U8*iImg, U8* modify, struct greed *cube, struct greed_count *r, struct greed_count *g, struct greed_count *b, int width, int height, int *global_green, int *global_black);
void sorting(struct greed_count *info, int size);
void green_position(U8 *img, struct greed g, int centerX, int centerY, float *distribute);
// �� ������ static�Լ���, ����ó���� greed_fill �Լ� ���Ŀ��� �� �� �־ �ٱ����� ������. *distribute �Ķ���ʹ� 

void merging_green_black(U8 *binary, int greed_index);

int front_side_analyze(U8* img, struct greed *maximumGreed, int max_idx);

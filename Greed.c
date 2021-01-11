#include "Greed.h"
#include <stdio.h>

//Greed�� R, G, B �߽������� ����Ѵ�
static void center_caculate_cent(struct greed *g)
{
	int i;

	for(i=0; i<G_C_NUM; ++i){
		g[i].green_center_x = (element)(1.0*g[i].green_x/g[i].green_count);
		g[i].green_center_y = (element)(1.0*g[i].green_y/g[i].green_count);

		g[i].blue_center_x = (element)(1.0*g[i].blue_x/g[i].blue_count);
		g[i].blue_center_y = (element)(1.0*g[i].blue_y/g[i].blue_count);

		g[i].red_center_x = (element)(1.0*g[i].red_x/g[i].red_count);
		g[i].red_center_y = (element)(1.0*g[i].red_y/g[i].red_count);
	}
}

//Green���� ���� ���̳��� Greed�� �� ��Ȯ�� ��ġ�� ����Ѵ�
//idx�� Greed ����ü�� �ε���
//centerX, centerY�� �� �׸��忡�� ���Ϸ� �ϴ� R or G or B�� �������� ����� �� ���Ϸ� �ϴ� ������ �߽��� �־��ش�
//G�� ã�� R, B(����� �߽�)�� �� �����߽����� ������(�Ʒ� �Լ� �Ⱦ���)
void green_position(U8 *img, struct greed g, int centerX, int centerY, float *distribute)		//*distribute = &cube[max].green_ratio
{
	//�˻� ���� -> 40 * 40 or 30 * 30
	const int checking_area = 15;		//40 by 40�̸� 20����, 60 by 60�̸� 30���� �������ش�
	//const int checking_area_full = checking_area * 2;
	//���� ����� ���� ������ �ΰ��� ������
	
	int index;
	int i;
	int j;
	int count = 0;
	int size = 1;

	//		�˻翵���� ���� ����
	int start_x = centerX - checking_area;
	int start_y = centerY - checking_area;
	int end_x = centerX + checking_area;
	int end_y = centerY + checking_area;
	
	if(start_x < 0) start_x = 0;
	if(start_y < 0) start_y = 0;
	if(end_x > WIDTH_MODIFY) end_x = WIDTH_MODIFY;
	if(end_y > HEIGHT) end_y = HEIGHT;
	//		�˻翵���� ���� ����

	for(i=start_y; i<end_y; ++i) {	//for���� ���鼭 �ʷϻ��� ������ �����Ѵ�
		index = i * WIDTH;
		for(j=start_x; j<end_x; ++j) {
			if(img[index + j] == WHITE)
				count++;
		}
	}
	size = (end_x - start_x) * (end_y - start_y);		//������: �˻翵�� ũ��

	*distribute = 1.0 * count;
	*distribute /= size;
}

//����ȭ(�ʷ�-����-������)�� ������ ������ �ʷϻ��� �����ش�(BLACK to WHITE)
//greed_index�� �����ϰ��� �ϴ� �׸����� �ε���
void merging_green_black(U8 *binary, int greed_index)
{
	int i, j, idx;
	int start = greed_index * G_C_WIDTH;

	for(i=start; i<start+G_C_WIDTH; ++i) {
		idx = i * FPGA_IMAGE_R;
		for(j=15; j<WIDTH_MODIFY; j++) {
			if(binary[idx + j] > BLACK)
				binary[idx + j] = WHITE;
		}
	}
}

//�� �׸��带 R, G, B���� ī��Ʈ �ؼ� �����Ѵ�
void sorting(struct greed_count *info, int size)
{
	int i, j;
	struct greed_count temp;

	for(i=0; i<size; ++i) {
		for(j=size-1; j>i; --j) {
			if(info[j].count > info[j-1].count) {
				temp = info[j];
				info[j] = info[j-1];
				info[j-1] = temp;
			}
		}
	}
}

//img = ���� ħ��, ��â�� ����
//maximumGreed = �ʷϻ� ���帹�� �׸���
//�Լ��� ������ �����κ��� ����˻��ؼ� �ʷϻ� ������ ���� �� �κ��� ��������, �������� ����� ��ȯ�Ѵ�
int front_side_analyze(U8* img, struct greed *maximumGreed, int max_idx)
{
	int i, j, k;
	int idx1 = G_C_WIDTH * WIDTH;
	int idx2;
	int green_count[G_C_NUM] = {0};

	int range = 10;
	int start = maximumGreed[max_idx].green_center_x - range;
	int end = maximumGreed[max_idx].green_center_x + range;

	//int max_greed_green;			//�ʷϻ� ���� ���� �׸����� �ʷ� ī��Ʈ
	int total_greed_green = 0;		//��ü ������ �׸� ī��Ʈ

	float ratio;

	if( start < 0 ) start = 0;
	if( end > 105 ) end = 105;

	for(k=0; k<G_C_NUM; ++k) { 
	//���� �׸��� ���� for��
		idx1 *= k;

		for(i=0; i<G_C_WIDTH; ++i) {
			idx2 = idx1 + (i * WIDTH);

			for(j=start; j<end; ++j) {
				if(img[idx2 + j] == WHITE) {
					green_count[k]++;
				}
			}
		}
		total_greed_green += green_count[k];

		idx1 = G_C_WIDTH * WIDTH;
	}

	ratio = (1.0 * green_count[max_idx]) / total_greed_green;
	printf("Greed: %d\tAll: %d\n", green_count[max_idx], total_greed_green);
	printf("Ratio: %f\n\n", ratio);

	/*
	 *
	 *	�˰��� ����ֱ�
	 *
	 */

	if(ratio > 0.3) return 1;
	else return 2;
}

//Greed ������ ä���ִ´�.
void greed_fill(U16* hImg, U8* sImg, U8*iImg, U8* modify, struct greed *cube, struct greed_count *r, struct greed_count *g, struct greed_count *b, int width, int height, int *global_green, int *global_black)
{
	int i, j, k;					//������ ��ȸ�ϴ� �ε���(k=�׸��� �ε���, i=�׸��� ���� ��ȸ, j=�׸��� ���� ��ȸ)
	int idx1 = G_C_WIDTH * WIDTH;	//��������� ���� �ε��� ����
	int idx2;						//���� ��������� ����
	int idx_y;						//�׸��� ���� ���� �ε���
	int margin = 30;				//������
	int max;						//Green Count�� ���� ū �׸����� �ε���

	int blue_h_max = 360 + margin;	//���α׷� ����ȭ�� ���� ���Ⱚ�� �̸� ������ش�
	int blue_h_min = 345 - margin;	//						"
	int red_h_max = 215 + margin;	//						"
	int red_h_min = 190 - margin;	//						"

	for(k=0; k<G_C_NUM; ++k) { 
	//���� �׸��� ���� for��
		idx1 *= k;
		r[k].idx = g[k].idx = b[k].idx = k;

		for(i=0; i<G_C_WIDTH; ++i) {
			idx2 = idx1 + (i * WIDTH);
			idx_y = idx2 / 180;

			for(j=0; j<width; ++j) {
				if(modify[idx2 + j] == WHITE) {
					++cube[k].green_count;
					cube[k].green_x += j;
					cube[k].green_y += idx_y;
					++(*global_green);
				}
				else if(modify[idx2 + j] == 128){
					++cube[k].black_count;
					++(*global_black);
				}

				//�̺κ��� R/B HSI ���� �����ؼ� �־������
				//�밡���� �پ���� �ҵ�...
				//��/�� ã�Ƽ� Greed�� ����־�����
				if(hImg[idx2 + j] < blue_h_max && hImg[idx2 + j] > blue_h_min) {
					++cube[k].blue_count;
					cube[k].blue_x += j;
					cube[k].blue_y += idx_y;
				}
				else if(hImg[idx2 + j] < red_h_max && hImg[idx2 + j] > red_h_min) {
					++cube[k].red_count;
					cube[k].red_x += j;
					cube[k].red_y += idx_y;
				}
			}
		}
		//

		r[k].count = cube[k].red_count;
		g[k].count = cube[k].green_count;
		b[k].count = cube[k].blue_count;

		idx1 = G_C_WIDTH * WIDTH;
	}

	sorting(r, G_C_NUM);	//������ ���� ������ �׸��� �ε��� ��迭�Ѵ�
	sorting(g, G_C_NUM);	//�ʷϻ� ���� ������ �׸��� �ε��� ��迭�Ѵ�
	sorting(b, G_C_NUM);	//�Ķ��� ���� ������ �׸��� �ε��� ��迭�Ѵ�

	max = g[0].idx;									//�ʷϼ��� �ִ� �׸����� �ε��� [0 ~ 5]
	center_caculate_cent(cube);						//�� �׸����� R, G, B �����߽��� ����ش�
}

/*
						180

|---------------------------------------------------|
|			Greed #1			|					|
|-------------------------------|					|
|			Greed #2			|					|
|-------------------------------|					|
|			Greed #3			| �̺κ��� ����ó��	|	120
|-------------------------------	|		���Ѵ�		|
|			Greed #4			|					|
|-------------------------------|					|
|			Greed #5			|					|
|-------------------------------|					|
|			Greed #6			|					|
|---------------------------------------------------|

�� �׸��忡 ���� ����: R, G, B ī��Ʈ, (������)�����߽�

*/

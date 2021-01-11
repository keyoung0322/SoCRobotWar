#include "Greed.h"
#include <stdio.h>

//Greed의 R, G, B 중심정보를 계산한다
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

//Green값이 가장 많이나온 Greed의 더 정확한 위치를 계산한다
//idx는 Greed 구조체의 인덱스
//centerX, centerY는 각 그리드에서 구하려 하는 R or G or B의 밀집도를 계산할 때 구하려 하는 색상의 중심을 넣어준다
//G만 찾고 R, B(경기장 중심)는 걍 무게중심으로 구하자(아래 함수 안쓴다)
void green_position(U8 *img, struct greed g, int centerX, int centerY, float *distribute)		//*distribute = &cube[max].green_ratio
{
	//검사 영역 -> 40 * 40 or 30 * 30
	const int checking_area = 15;		//40 by 40이면 20으로, 60 by 60이면 30으로 설정해준다
	//const int checking_area_full = checking_area * 2;
	//성능 향상을 위해 변수를 두개로 나눈다
	
	int index;
	int i;
	int j;
	int count = 0;
	int size = 1;

	//		검사영역을 위한 변수
	int start_x = centerX - checking_area;
	int start_y = centerY - checking_area;
	int end_x = centerX + checking_area;
	int end_y = centerY + checking_area;
	
	if(start_x < 0) start_x = 0;
	if(start_y < 0) start_y = 0;
	if(end_x > WIDTH_MODIFY) end_x = WIDTH_MODIFY;
	if(end_y > HEIGHT) end_y = HEIGHT;
	//		검사영역을 위한 변수

	for(i=start_y; i<end_y; ++i) {	//for문을 돌면서 초록색의 갯수를 조사한다
		index = i * WIDTH;
		for(j=start_x; j<end_x; ++j) {
			if(img[index + j] == WHITE)
				count++;
		}
	}
	size = (end_x - start_x) * (end_y - start_y);		//사이즈: 검사영역 크기

	*distribute = 1.0 * count;
	*distribute /= size;
}

//삼진화(초록-검정-나머지)된 영상의 검정을 초록색과 합쳐준다(BLACK to WHITE)
//greed_index는 병합하고자 하는 그리드의 인덱스
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

//각 그리드를 R, G, B따로 카운트 해서 정렬한다
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

//img = 삼진 침식, 팽창된 영상
//maximumGreed = 초록색 가장많은 그리드
//함수는 영상의 일정부분을 수평검사해서 초록색 비율을 따진 후 로봇이 정면인지, 측면인지 결과를 반환한다
int front_side_analyze(U8* img, struct greed *maximumGreed, int max_idx)
{
	int i, j, k;
	int idx1 = G_C_WIDTH * WIDTH;
	int idx2;
	int green_count[G_C_NUM] = {0};

	int range = 10;
	int start = maximumGreed[max_idx].green_center_x - range;
	int end = maximumGreed[max_idx].green_center_x + range;

	//int max_greed_green;			//초록색 가장 많은 그리드의 초록 카운트
	int total_greed_green = 0;		//전체 영역의 그린 카운트

	float ratio;

	if( start < 0 ) start = 0;
	if( end > 105 ) end = 105;

	for(k=0; k<G_C_NUM; ++k) { 
	//세로 그리드 정보 for문
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
	 *	알고리즘 집어넣기
	 *
	 */

	if(ratio > 0.3) return 1;
	else return 2;
}

//Greed 정보를 채워넣는다.
void greed_fill(U16* hImg, U8* sImg, U8*iImg, U8* modify, struct greed *cube, struct greed_count *r, struct greed_count *g, struct greed_count *b, int width, int height, int *global_green, int *global_black)
{
	int i, j, k;					//영상을 순회하는 인덱스(k=그리드 인덱스, i=그리드 새로 순회, j=그리드 가로 순회)
	int idx1 = G_C_WIDTH * WIDTH;	//성능향상을 위한 인덱스 계산식
	int idx2;						//역시 성능향상을 위해
	int idx_y;						//그리드 안의 새로 인덱스
	int margin = 30;				//마진값
	int max;						//Green Count가 가장 큰 그리드의 인덱스

	int blue_h_max = 360 + margin;	//프로그램 최적화를 위해 추출값을 미리 계산해준다
	int blue_h_min = 345 - margin;	//						"
	int red_h_max = 215 + margin;	//						"
	int red_h_min = 190 - margin;	//						"

	for(k=0; k<G_C_NUM; ++k) { 
	//세로 그리드 정보 for문
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

				//이부분은 R/B HSI 직접 추출해서 넣어줘야함
				//노가다좀 뛰어워랴 할듯...
				//빨/파 찾아서 Greed에 집어넣어주자
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

	sorting(r, G_C_NUM);	//빨강색 갯수 순으로 그리드 인덱스 재배열한다
	sorting(g, G_C_NUM);	//초록색 갯수 순으로 그리드 인덱스 재배열한다
	sorting(b, G_C_NUM);	//파랑색 갯수 순으로 그리드 인덱스 재배열한다

	max = g[0].idx;									//초록성분 최대 그리드의 인덱스 [0 ~ 5]
	center_caculate_cent(cube);						//각 그리들의 R, G, B 무게중심을 잡아준다
}

/*
						180

|---------------------------------------------------|
|			Greed #1			|					|
|-------------------------------|					|
|			Greed #2			|					|
|-------------------------------|					|
|			Greed #3			| 이부분은 영상처리	|	120
|-------------------------------	|		안한다		|
|			Greed #4			|					|
|-------------------------------|					|
|			Greed #5			|					|
|-------------------------------|					|
|			Greed #6			|					|
|---------------------------------------------------|

각 그리드에 들어가는 정보: R, G, B 카운트, (각각의)무게중심

*/

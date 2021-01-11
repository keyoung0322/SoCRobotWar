#ifndef __LABELING_H
#define __LABELING_H

#define LABELING_THRESHOLD	30//5*5이상

#define BLACK 0
#define WHITE 255

struct center
{
	int x_center;
	int y_center;
	int top_x;
	int top_y;
	int bot_x;
	int bot_y;
	int left_x;
	int left_y;
	int right_x;
	int right_y;
	int mark;//마킹된 라벨
	int pixel_count;//영역내의 픽셀개수
	///상황에 따른 변수 - 라벨링 함수에서는 처리하지 않음
	int width;//가로길이 - 게이트에서 쓰임
	int height_width;//세로/가로 비율 - 게이트에서 쓰임
	int robot_g_count;
	int robot_b_count;
	int robot_y_count;
	int robot_r_count;
	int robot_count;
};

#define	BLOB_INFO_CNT	20
int BlobColoring(unsigned char*, int, int, struct center[BLOB_INFO_CNT], int*, int);
int BlobColoring2(unsigned char*, int, int, struct center[BLOB_INFO_CNT], int*, int, short *, short *, short *);
int blob_sort_re(const void *a, const void *b);

#endif //ifndef __LABELING_H

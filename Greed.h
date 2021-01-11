
#include "amazon2_sdk.h"
#include "graphic_api.h"

#define FPGA_IMAGE_R	180	//영상 가로크기
#define FPGA_IMAGE_C	120	//영상 새로크기

/*
#define G_R_NUMBER		10	//그리드 가로갯수
#define G_C_NUMBER		10	//그리드 새로갯수

#define G_WIDTH			18	//그리드 가로길이
#define G_HEIGHT		12	//그리드 새로길이
*/

#define G_C_NUM			6
#define G_C_WIDTH		20

#define WIDTH_MODIFY	105

#define element			int
#define element_num		16

#define	GREY			128

#define GREED_HISTORY 10;

struct greed{
	element green_count;	//초록 성분 갯수
	element blue_count;		//파랑 성분 갯수
	element red_count;		//빨강 성분 갯수

	element green_x;		//초록점 X좌표 합
	element green_y;		//초록점 Y좌표 합
	element blue_x;			//파랑점 X좌표 합
	element blue_y;			//파랑점 Y좌표 합
	element red_x;			//빨강점 X좌표 합
	element red_y;			//빨강점 Y좌표 합

	element green_center_x;	//초록점 X중심
	element green_center_y;	//초록점 Y중심
	element blue_center_x;	//파랑색 X중심
	element blue_center_y;	//파랑색 Y중심
	element red_center_x;	//빨강색 X중심
	element red_center_y;	//빨강색 Y중심

	element black_count;

	float green_ratio;		//초록색 중심 비율(색상 넓이 / 검사하는 전체 넓이)
	float blue_ratio;		//파랑색					"
	float red_ratio;		//빨강색	
};
//실수 자료형: 각 그리드의 각 R, G, B의 중심 차지 비율

struct greed_count{			//그리드 정보(각 색상 갯수) 저장 및 배열을 위한 구조체 <- 요걸로 재정렬 해준다
	int idx;				//그리드 인덱스
	int count;				//그리드 안의 검출색상 갯수
};

struct greed_history {
	int last_index;
	int green_count;
	int black_count;
};
void greed_fill(U16* hImg, U8* sImg, U8*iImg, U8* modify, struct greed *cube, struct greed_count *r, struct greed_count *g, struct greed_count *b, int width, int height, int *global_green, int *global_black);
void sorting(struct greed_count *info, int size);
void green_position(U8 *img, struct greed g, int centerX, int centerY, float *distribute);
// ㄴ 원래는 static함순대, 영상처리를 greed_fill 함수 이후에도 할 수 있어서 바깥으로 빼줬음. *distribute 파라미터는 

void merging_green_black(U8 *binary, int greed_index);

int front_side_analyze(U8* img, struct greed *maximumGreed, int max_idx);

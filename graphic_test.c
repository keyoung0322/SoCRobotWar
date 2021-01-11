#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "amazon2_sdk.h"
#include "graphic_api.h"
#include "GraytoRGB565.h"
#include "HSIwithBinary.h"
#include "mopology.h"
#include "labeling.h"
#include "Greed.h"

#define AMAZON2_GRAPHIC_VERSION		"v0.3"

void what_to_do(struct greed *cube, struct greed_count *r, struct greed_count *g, struct greed_count *b, int global_black, int global_green)
{
	const int limit = 300;
	static int number = 0;

	static int greed_green_[300];
	static int global_green_[300];
	static int greed_black_[300];

	int i;
	int total;

	if(number == limit) { 
		total = 0;

		for(i=0; i<limit; ++i)
			total += greed_green_[i];

		total = (int)(1.0 * total / limit);
		printf("greed_green_avg = %d", total);


		total = 0;

		for(i=0; i<limit; ++i)
			total += global_green_[i];

		total = (int)(1.0 * total / limit);
		printf("global_green_avg = %d", total);



		total = 0;

		for(i=0; i<limit; ++i)
			total += greed_black_[i];

		total = (int)(1.0 * total / limit);
		printf("greed_black_avg = %d", total);
		
		exit(1);
	}
	/*
	int global_black_th = 850;
	int global_green_th = 650;
	int greed_black_th = 400;
	//int greed_green_th = 300;
	float ratio = 0.0;
	int max_idx = g[0].idx;

	//printf("global_g: %d\tglobal_b: %d\tgreed_g: %d\tgreed_b: %d\n", global_green, global_black, cube[max_idx].green_count,cube[max_idx].black_count);
	//ratio = 1.0 * cube[max_idx].black_count / global_black;
	//printf("Greed/Global black raio : %f\n", ratio);
	//global_green > global_green_th -> 전체 초록 갯수가 th 이상
	if(global_green > global_green_th) {
		if(cube[max_idx].black_count > greed_black_th || global_black > global_black_th) { //그리드 블랙 갯수나 전체 블랙 갯수중 하나라도 th보다 크다
			ratio = 1.0 * cube[max_idx].black_count / global_black;

			printf("Greed/Global black raio : %f\n", ratio);
			if(ratio > 0.21) printf("Close: Side of the robot\n");
			else printf("Close: Robot is facing me\n");
		}
		else printf("Close: Low possibility of robot. look aroud\n");
	}
	else {
		if(global_green > 200 && global_green < 270) {
			if(max_idx == 0 || max_idx == 1) printf("Far: Turn left and move\n");
			else if(max_idx == 2 || max_idx == 3) printf("Far: Move to center\n");
			else printf("Far: Turn right and move\n");
		}
		else printf("Far: No robot. Look aroud.\n");
	}
	*/
	int max = g[0].idx;

	greed_green_[number] = cube[max].green_count;
	global_green_[number] = global_green;
	greed_black_[number] = cube[max].black_count;

	printf("%d\n", number);
	printf("greed_green_maximum: %d\n", cube[max].green_count);
	printf("global_green_maximum: %d\n", global_green);
	printf("greed_black_maximum: %d\n\n", cube[max].black_count);

	number++;
}

void graphic_test(void)
{
	//float accuracy;
	int idx;

	//Greed 구조체 -> 새로 그리드로 나눈다(1 * 6)
	struct greed cube[G_C_NUM];
	struct greed_count gc[G_C_NUM];
	struct greed_count rc[G_C_NUM];
	struct greed_count bc[G_C_NUM];

	int count_size = sizeof(int) * 3;
	int size = (sizeof(element) * element_num + sizeof(float) * 3) * G_C_NUM;

	//global -> 화면상의 모든 초록, 검정 갯수(밀집도 대신 쓸 까...?)
	int global_green = 0;
	int global_black = 0;

	memset(cube, 0, size);
	memset(gc, 0, count_size);		//그리드 count 정보 초기화
	memset(rc, 0, count_size);
	memset(bc, 0, count_size);
	//size, memset == 구조체 초기화를 위한 모듈(매 프레임마다 넣어주자)

	U16* fpga_videodata = (U16*)malloc(180 * 120 * 2);	//카메라 데이타
	U16* out_videodata = (U16*)malloc(180 * 120 * 2);
	U16* out_videodata2 = (U16*)malloc(180 * 120 * 2);
	U16* h_videodata = (U16*)malloc(180 * 120 * 2);		//H영상
	U8* s_videodata = (U8*)malloc(180 * 120);			//S영상
	U8* i_videodata = (U8*)malloc(180 * 120);			//I영상

	U8* modify_videodata1 = (U8*)malloc(180 * 120);	//처리 데이타 #1
	U8* modify_videodata2 = (U8*)malloc(180 * 120);	//처리 데이타 #2
	U8* after_mopology = (U8*)malloc(180 * 120);
	//요거 두개가지고 처리한다

	while(1) {
		read_fpga_video_data(fpga_videodata);			//fpga에서 영상을 받아온다
		memset(cube, 0, size);		//Greed 구조체 초기화
		memset(gc, 0, count_size);		//그리드 count 정보 초기화
		memset(rc, 0, count_size);
		memset(bc, 0, count_size);

		HSI(fpga_videodata, s_videodata, i_videodata, h_videodata);
		binary(h_videodata, s_videodata, i_videodata, modify_videodata1, NEW_HEIGHT, NEW_WIDTH/*, &global_white*/);

		binaryErosion(modify_videodata1, modify_videodata2, NEW_WIDTH, NEW_HEIGHT, gSE);
		binaryDilation(modify_videodata2, after_mopology, NEW_WIDTH, NEW_HEIGHT, gSE);

		/* 그리드 함수 */
		greed_fill(h_videodata, s_videodata, i_videodata, after_mopology, cube, rc, gc, bc, NEW_HEIGHT, NEW_WIDTH, &global_green, &global_black);
		idx = gc[0].idx;
		green_position(modify_videodata1, cube[idx], cube[idx].green_center_x, cube[idx].green_center_y, &cube[idx].green_ratio);
		merging_green_black(after_mopology, idx);	//->초록 + 검정 병합
		//printf("greed idx: %d\tgreen count: %d\tblack count: %d\n", idx, cube[idx].green_count, cube[idx].black_count);
		//printf("x: %d\ty: %d\n", cube[idx].green_center_x, cube[idx].green_center_y);
		//printf("------------------------------------------------------\n");
		//printf("greed idx: %d\tgreen count: %d\tblack count: %d\n", idx, cube[idx].green_count, global_black);
		//printf("Global green: %d\tGlobal black: %d\n", global_green, global_black);

		//front_side_analyze(after_mopology, cube, idx);
		what_to_do(cube, rc, gc, bc, global_black, global_green);

		GraytoRGB565(after_mopology, out_videodata, 180, 120, 180, 120);
		drawCross(out_videodata, cube[idx].green_center_x, cube[idx].green_center_y, 10, 2, WIDTH, HEIGHT);
		drawCross(fpga_videodata, cube[idx].green_center_x, cube[idx].green_center_y, 10, 2, WIDTH, HEIGHT);
		draw_img_from_buffer_separator(out_videodata, LEFT_SCREEN);		// 이진화된영상
		usleep(1);
		draw_img_from_buffer_separator(fpga_videodata, RIGHT_SCREEN);	// 원영상
		usleep(1);
		
		flip();
		
		global_green = 0;
		global_black = 0;
	}

	free(fpga_videodata);
	free(out_videodata);
	free(out_videodata2);
	free(h_videodata);
	free(s_videodata);
	free(i_videodata);
	free(modify_videodata1);
	free(modify_videodata2);
	free(after_mopology);
}

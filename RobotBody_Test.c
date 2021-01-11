/*#include "amazon2_sdk.h"
#include "uart_api.h"
#include "robot_protocol.h"
#include "graphic_api.h"
#include "GraytoRGB565.h"
#include "HSIwithBinary.h"
#include "mopology.h"
#include "labeling.h"
#include "Greed.h"

#include <termios.h>
static struct termios inittio, newtio;

U16* fpga_videodata;
U16* out_videodata;
U16* h_videodata;	
U8*	s_videodata;		
U8*	i_videodata;			

U8* modify_videodata1;
U8* modify_videodata2;
U8* after_mopology;

int find_center = 0;
int global_green = 0;
int global_black = 0;
int global_white = 0;

struct greed g[6];
struct greed_count gc[6];
struct greed_count rc[6];
struct greed_count bc[6];

void init_console(void)
{
	tcgetattr(0, &inittio);
	newtio = inittio;

	// 인터럽트 막아서 지움
	//newtio.c_lflag &= ~ICANON;	
	//newtio.c_lflag &= ~ECHO;
	//newtio.c_lflag &= ~ISIG;

	newtio.c_cc[VMIN] = 1;
	newtio.c_cc[VTIME] = 0;

	cfsetispeed(&newtio, B115200);

	tcsetattr(0, TCSANOW, &newtio);
}

int taekwon()
{
	int mis;

	//영상처리를 위한 greed 구조체

	init_console();

	if (uart_open() < 0) 
		return EXIT_FAILURE;
	
	memory_allocate();

	init_robot();					// 토크걸기
	Motion(ATTACK_STAND);	
	Motion(HEAD);					// 머리 정면
	Motion(HELLO);					// 인사
	Motion(ATTACK_STAND);			// 기본자세 취하기
	turn_left_80();					// 왼쪽으로 회전하여 공격자세 취하기
	usleep(400000);					// 0.4초 대기

	Motion(FOUR_STEP);

	while(1) {
		mis = find_robot();
		switch(mis) {
			case ROBOT_FOUND_GENUIN_LEFT:
			case ROBOT_FOUND_GENUIN_CENTER:
			case ROBOT_FOUND_GENUIN_RIGHT:
				printf("robot find!\n");
				move_to_robot(mis);
				break;

			case ROBOT_FOUND_FAR_LEFT:
			case ROBOT_FOUND_FAR_CENTER:
			case ROBOT_FOUND_FAR_RIGHT:
				printf("robot is far!\n");
				move_to_robot(mis);
				break;

			case ROBOT_FOUND_FACING:
				printf("robot facing!\n");
				move_to_robot(mis);
				break;

			case ROBOT_NOT_FOUND:
				printf("robot not found!\n");
				//move_to_center(rc, bc);
				break;
			default:
				break;
		}
	}
	
	memory_free();

	uart_close();

	return 0;
}

int find_robot(void)
{
	char mis = 0;		// 리턴값
	int head_turn = 0;
	static int history_idx = 6;

	int global_black_th = 900;		
	int global_green_th = 430;		
	int greed_black_th = 700;		
	int greed_green_th = 100;
	int global_white_th = 2500;

	float ratio;
	int max_idx = 0;int r_max_idx = 0;int b_max_idx =0;
	int size = (sizeof(element) * element_num + sizeof(float) * 3) * G_C_NUM;		// sizeof(float) * 3 = 추가해준 R, G, B의 밀집도
	int count_size = sizeof(int) * 3;


HEAD_TURN:
	read_fpga_video_data(fpga_videodata);

	memset(g, 0, size);			
	memset(gc, 0, count_size);
	memset(rc, 0, count_size);
	memset(bc, 0, count_size);

	HSI(fpga_videodata, s_videodata, i_videodata, h_videodata);
	binary(h_videodata, s_videodata, i_videodata, modify_videodata1, 105, 120, &global_white);

	greed_fill(h_videodata, s_videodata, i_videodata, modify_videodata1, g, rc, gc, bc, 105, 120, &global_green, &global_black);

	max_idx = gc[0].idx;
	r_max_idx = rc[0].idx;
	b_max_idx = bc[0].idx;

	green_position(modify_videodata1, g[max_idx], g[max_idx].green_center_x, g[max_idx].green_center_y, &g[max_idx].green_ratio);
	merging_green_black(modify_videodata1, max_idx);	//->초록 + 검정 병합

	memcpy(after_mopology, modify_videodata1, 180 * 120);
	binaryErosion(after_mopology, modify_videodata2, 120, 105, gSE);
	binaryDilation(modify_videodata2, after_mopology, 120, 105, gSE);

	GraytoRGB565(after_mopology, out_videodata, 180, 120, 180, 120);
	drawCross(out_videodata, g[max_idx].green_center_x, g[max_idx].green_center_y, 10, 2, 180, 120);
	drawCross(fpga_videodata, g[max_idx].green_center_x, g[max_idx].green_center_y, 10, 2, 180, 120);
	draw_img_from_buffer_separator(out_videodata, LEFT_SCREEN);
	draw_img_from_buffer_separator(fpga_videodata, RIGHT_SCREEN);
	flip();

	if(global_green > 200 && head_turn > 0) {
		if(head_turn == 1) {			// 머리 왼쪽 회전
			if(global_green < 1100) {
				Motion(NEW_KICK);
				turn_left_30();
			}
			else
				turn_left_30();
		}
		else if(head_turn == 2) {	// 머리 정면

			two_step();
			Motion(WALK_RIGHT_PUNCH);
		}
		else {						// 머리 오른쪽
			turn_right_30();
		}
	}

	if(global_green > global_green_th) {
		//printf("close: global_green: %d, greed_green = %d\n", global_green, g[max_idx].green_count);
		if( (g[max_idx].black_count > greed_black_th) || (global_black > global_black_th) ) {
			ratio = 1.0 * g[max_idx].black_count / global_black;	
			if(ratio < 0.1)			// 로봇 정면인 경우
				mis = ROBOT_FOUND_FACING;
			else
				mis = find_robot_direction(max_idx, ROBOT_FOUND);
		}
		history_idx = max_idx;
	}
	else if(global_green > 400 && g[max_idx].green_count > greed_green_th)	{		// 로봇 멀리 있다
		//printf("far: global_green: %d, greed_green = %d\n", global_green, g[max_idx].green_count);
		mis = find_robot_direction(max_idx, ROBOT_IS_FAR);
		 
		history_idx = max_idx;
	}
	else if(global_green > 200)
		two_step();
	else {
		if(history_idx < 6) {
			if(history_idx == 0 || history_idx == 1 || history_idx == 2) {
				mis = find_robot_direction(max_idx, ROBOT_HISTORY_LEFT);
				history_idx = 6;
			}
			else if(history_idx == 3 || history_idx == 4 || history_idx == 5) {
				mis = find_robot_direction(max_idx, ROBOT_HISTORY_RIGHT);
				history_idx = 6;
			}
			else 
				history_idx = 6;
		}
		else {
			//printf("red: %d, blue: %d\n", rc[r_max_idx].count, bc[b_max_idx].count);
			if(head_turn == 0) {		// 먼저 왼쪽 확인
				Motion(HEAD_LEFT);
				++head_turn;
				if(global_white >= global_white_th) {
					if(rc[r_max_idx].count > 600 && bc[b_max_idx].count > 600 && abs(b_max_idx - r_max_idx) < 2)
						find_center = 1;		// 태극무늬가 왼쪽에 있다
				}
				goto HEAD_TURN;
			}
			else if(head_turn == 1) {	// 없으면 정면 확인
				Motion(HEAD);
				++head_turn;
				if(global_white >= global_white_th) {
					if(rc[r_max_idx].count > 600 && bc[b_max_idx].count > 600 && abs(b_max_idx - r_max_idx) < 2)
						find_center = 2;		// 태극무늬가 가운데 있다
				}
				goto HEAD_TURN;
			}
			else if(head_turn == 2) {	// 없으면 오른쪽 확인
				Motion(HEAD_RIGHT);
				++head_turn;
				if(global_white >= global_white_th) {
					if(rc[r_max_idx].count > 600 && bc[b_max_idx].count > 600 && abs(b_max_idx - r_max_idx) < 2)
						find_center = 3;	// 태극무늬가 오른쪽에 있다
				}
				goto HEAD_TURN;
			}
			else {						// 또 없으면 태극무늬 찾기
				Motion(HEAD);			// 일단 정면을 바라본다
				//if(find_center > 0)		// 장내를 바라보고 태극무늬 발견
				//	mis = ROBOT_NOT_FOUND;

				Motion(LEFT_BACK_STEP);Motion(LEFT_BACK_STEP);
				turn_right_80();
				mis = ROBOT_NOT_FOUND;
			}
		}
	}

	global_green = 0;
	global_black = 0;
	head_turn = 0;

	return mis;
}

int find_robot_direction(int max_idx, int dis)
{
	int mis = 0;
	switch(dis) {
		case ROBOT_FOUND:
			if( max_idx == 0 || max_idx == 1)
				mis = ROBOT_FOUND_GENUIN_LEFT;
			else if(max_idx == 2)
				mis = ROBOT_FOUND_GENUIN_CENTER;
			else if(max_idx == 3) {
				Motion(TURN_RIGHT_9);
				mis = ROBOT_FOUND_GENUIN_CENTER;
			}
			else
				mis = ROBOT_FOUND_GENUIN_RIGHT;
			break;
		
		case ROBOT_FOUND_FACING:
			mis = ROBOT_FOUND_FACING;
			break;
		
		case ROBOT_IS_FAR:
			if( max_idx == 0 || max_idx == 1)
				mis = ROBOT_FOUND_FAR_LEFT;
			else if(max_idx == 2)
				mis = ROBOT_FOUND_FAR_CENTER;
			else if(max_idx == 3) {
				Motion(TURN_RIGHT_9);
				mis = ROBOT_FOUND_FAR_CENTER;
			}
			else
				mis = ROBOT_FOUND_FAR_RIGHT;
			break;
		
		case ROBOT_HISTORY_LEFT:
			Motion(TURN_LEFT_10_H_RIGHT);
			Motion(TURN_LEFT_10_H_RIGHT);
			one_step();
			Motion(NEW_KICK);
			//Motion(LEFT_BACK_STEP);
			mis = 0;
			break;

		case ROBOT_HISTORY_RIGHT:
			Motion(TURN_RIGHT_9);
			Motion(TURN_RIGHT_9);
			one_step();
			Motion(WALK_RIGHT_PUNCH);
			Motion(LEFT_BACK_STEP);
			mis = 0;
			break;
	}

	return mis;
}

int move_to_robot(int mis)
{
	switch(mis) {
		case ROBOT_FOUND_GENUIN_LEFT:
			two_step();
			Motion(TURN_LEFT_10_H_RIGHT);
			Motion(NEW_KICK);
			Motion(LEFT_BACK_STEP);
			break;
	
		case ROBOT_FOUND_GENUIN_CENTER:
			two_step();
			Motion(SIDE_KICK); 
			//Motion(LEFT_BACK_STEP);
			break;
		
		case ROBOT_FOUND_GENUIN_RIGHT:
			two_step();
			Motion(TURN_RIGHT_9);
			Motion(WALK_UPDOWN_PUNCH);
			Motion(LEFT_BACK_STEP);
			break;
		
		case ROBOT_FOUND_FAR_LEFT:
			Motion(TURN_LEFT_10_H_RIGHT);
			two_step();
			one_step();
			one_step();
			Motion(NEW_KICK);
			Motion(LEFT_BACK_STEP);
			break;
		
		case ROBOT_FOUND_FAR_CENTER:
			two_step();
			one_step();
			one_step();
			Motion(THREE_STEP_PUNCH);
			Motion(LEFT_BACK_STEP);
			break;
		
		case ROBOT_FOUND_FAR_RIGHT:
			Motion(TURN_RIGHT_9);
			two_step();
			one_step();
			one_step();
			Motion(WALK_RIGHT_PUNCH);
			Motion(LEFT_BACK_STEP);
			break;

		case ROBOT_FOUND_FACING:
			two_step();
			Motion(SIDE_KICK);
			Motion(UPDOWN_RIGHT_PUNCH);
			break;
	}
	return 0;
}

void move_to_center(struct greed_count *rc, struct greed_count *bc)
{
	int r_max_idx = rc[0].idx;
	int b_max_idx = bc[0].idx;

	float blue_ratio = 1.0 * bc[b_max_idx].count / (bc[b_max_idx].count + rc[r_max_idx].count);
	float red_ratio = 1.0 * rc[r_max_idx].count / (bc[b_max_idx].count + rc[r_max_idx].count);

	if(find_center == 1) {		// 왼쪽에 있다
		turn_left_30();
		two_step();
	}
	else if(find_center == 2) {	// 정면에 있다
		two_step();
		two_step();
	}			
	else {						// 오른쪽에 있다
		turn_right_30();
		two_step();
	}
}

void repeat_motion(unsigned char motionNum, int num)
{
	int i;
	for(i = 0; i < num; i++) {
		Motion(motionNum);
	}
}

void memory_allocate(void)
{
	fpga_videodata = (U16*)malloc(180 * 120 * 2);	
	out_videodata = (U16*)malloc(180 * 120 * 2);
	h_videodata = (U16*)malloc(180 * 120 * 2);		
	s_videodata = (U8*)malloc(180 * 120);			
	i_videodata = (U8*)malloc(180 * 120);			

	modify_videodata1 = (U8*)malloc(180 * 120);
	modify_videodata2 = (U8*)malloc(180 * 120);
	after_mopology = (U8*)malloc(180 * 120);
}

void memory_free(void)
{
	free(fpga_videodata);
	free(out_videodata);
	free(h_videodata);
	free(s_videodata);
	free(i_videodata);
	free(modify_videodata1);
	free(modify_videodata2);
	free(after_mopology);
}
*/

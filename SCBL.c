#include "amazon2_sdk.h"
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

/* �������ͺ��� ���� */
U16* fpga_videodata;
U16* out_videodata;
U16* h_videodata;	
U8*	s_videodata;		
U8*	i_videodata;			

U8* modify_videodata1;
U8* modify_videodata2;
U8* after_mopology;

/* �������� ���� */
int max_idx;
int history_idx = 6;
int head_turn;
int head_state = 0;
int global_green = 0;
int global_black = 0;

struct greed g[6];
struct greed_count gc[6];
struct greed_count rc[6];
struct greed_count bc[6];

void init_console(void)
{
	tcgetattr(0, &inittio);
	newtio = inittio;

	// ���ͷ�Ʈ ���Ƽ� ����
	//newtio.c_lflag &= ~ICANON;	
	//newtio.c_lflag &= ~ECHO;
	//newtio.c_lflag &= ~ISIG;

	newtio.c_cc[VMIN] = 1;
	newtio.c_cc[VTIME] = 0;

	cfsetispeed(&newtio, B115200);

	tcsetattr(0, TCSANOW, &newtio);
}

int taekwon(void)
{
	int mis = 0;

	init_console();

	if (uart_open() < 0) 
		return EXIT_FAILURE;

	/* �޸� �Ҵ� */
	memory_allocate();

	/* ó�� ���� ��� */
	init_robot();				// ��ũ�ɱ�
	Motion(STAND_2);			// �⺻�ڼ����ϱ�
	Motion(HELLO_HOVIS);		// �λ�
	Motion(STAND_2);			// �⺻�ڼ����ϱ�
	Motion_Ack(TURN_LEFT_90);
	Motion_Ack(SIX_STEP);
	Motion_Ack(SIDE_KICK);
	usleep(100000);

	while(1) {
		mis = find_robot();
		
		/* �Ӹ��� ������ ���� */
		if(head_state == 1 /*&& ((mis == ROBOT_BACK) || (g[max_idx].green_count > 150))*/) {
			head_state = 0;
			Motion_Ack(FRONT_KICK_TURN_18);
		}
		/* �κ��� ���� �����ӿ� �־���. */
		if(mis == ROBOT_NOT_FOUND && history_idx != 6) {
			if(history_idx == 0 ) {
				Motion(TURN_LEFT_30);
				history_idx = 6;
			}
			else if(history_idx == 1 || history_idx == 2) {
				Motion(TURN_LEFT_18);
				history_idx = 6;
			}
			else if(history_idx == 3 || history_idx == 4) {
				Motion(TURN_RIGHT_18);
				history_idx = 6;
			}
			else {
				Motion(TURN_RIGHT_30);
				history_idx = 6;
			}
		}
		/* �κ��� �����ϱ� �Ӹ��� ������ */
		else if(mis == ROBOT_NOT_FOUND) {
			head_turn++;
			if(head_turn == 1)
				Motion(HEAD_RIGHT_45);
			else if(head_turn == 2)
				Motion(STAND_2);
			else if(head_turn == 3)
				Motion(HEAD_LEFT);
			else if(head_turn == 4) {		// ��û ������ ������ ��ã���ϱ� �ΰ��� ��������
				Motion(STAND_2);
				Motion(TWO_BACK_STEP);
			}
			else if(head_turn == 5)
				Motion(HEAD_RIGHT_45);
			else if(head_turn == 6)
				Motion(HEAD_LEFT);
			else if(head_turn < 20) {
				Motion_Ack(TURN_LEFT_30);
			}
			else if(head_turn >= 20)
				head_turn = 0;
		}
		
		/* �Ӹ��� ���ư� �������� �������� */
		else if(head_turn == 1 || head_turn == 5) {		// �Ӹ����� : ������
			head_turn = 0;
			if(max_idx == 0 || max_idx == 1) {
				if(mis == ROBOT_IS_FAR)
					Motion(TWO_STEP);
				Motion(TWO_STEP);
				Motion_Ack(TURN_RIGHT_13_TEMP);
				Motion(BACK_STEP);
			}
			else if(max_idx == 2 || max_idx == 3) {
				if(mis == ROBOT_IS_FAR) {
					Motion(TWO_STEP);
				}
				Motion(TWO_STEP);
				Motion_Ack(TURN_RIGHT_18_TEMP);
				Motion(BACK_STEP);
			}
			else {
				if(mis == ROBOT_IS_FAR)
					Motion(TWO_STEP);
				Motion(TWO_STEP);
				Motion_Ack(TURN_RIGHT_30_TEMP);
				Motion(BACK_STEP);
			}
		}
		else if(head_turn == 2 || head_turn == 4) {		// �Ӹ����� : ����
			head_turn = 0;
			if(mis == ROBOT_IS_FAR)
				Motion(TWO_STEP);
			Motion(TWO_STEP);
			Motion_Ack(UP_PUNCH);
			Motion(BACK_STEP);
		}
		else if(head_turn == 3 || head_turn == 6) {		// �Ӹ����� : ����
			head_turn = 0;
			if(max_idx == 0 || max_idx == 1) {	// ����
				Motion(HEAD_FRONT);
				head_state = 1;
			}
			else if(max_idx == 2 || max_idx == 3) {	// ����
				if(mis == ROBOT_IS_FAR) {
					Motion(TWO_STEP);
				}
				Motion(TWO_STEP);
				Motion_Ack(PUSH_PUNCH);
				Motion(BACK_STEP);
			}
			else {									// ������
				if(mis == ROBOT_IS_FAR) {
					Motion(TWO_STEP);
				}
				Motion(TWO_STEP);
				Motion_Ack(TURN_LEFT_13_PUSH);
				Motion(BACK_STEP);
			}
		}
		else if(mis == ROBOT_BACK) {
			Motion(TWO_STEP);
			Motion(SIDE_KICK2_2times);
		}
		/* �κ��� �����ϱ� �������� */
		else {
			/* �κ��� �ִ� */
			if(mis == ROBOT_IS_FOUND) {
				head_turn = 0;
				if(max_idx == 4 || max_idx == 5) {
					Motion(TWO_STEP);
					Motion_Ack(TURN_LEFT_18_SIDE);
					usleep(100000);
					Motion(BACK_STEP);
				}
				else if(max_idx == 1 || max_idx == 3) {	
					Motion(TWO_STEP);
					Motion_Ack(SIDE_KICK2);
					Motion(BACK_STEP);
				}
				else if(max_idx == 2) {
					Motion(TWO_STEP);
					Motion_Ack(SIDE_KICK);
					Motion(BACK_STEP);
				}
				else if(max_idx == 0) {	
					Motion(TWO_STEP);
					Motion(TURN_LEFT_13);
					Motion_Ack(PUNCH_SET);
					usleep(100000);
					Motion(BACK_STEP);
				}
			}
			/* �κ��� �ָ� �ִ� */
			else if(mis == ROBOT_IS_FAR) {
				head_turn = 0;
				if(max_idx == 0 ) {		// ����
					Motion(TURN_LEFT_30);
					Motion(TWO_STEP);
				}
				else if(max_idx == 1) {
					Motion(TURN_LEFT_18);
					Motion(TWO_STEP);
				}
				else if(max_idx == 2 || max_idx == 3) {	// �߾�
					Motion(TWO_STEP);
				}
				else if(max_idx == 4) {					// ������
					Motion(TURN_RIGHT_18);
					Motion(TWO_STEP);
				}
				else {
					Motion(TURN_RIGHT_30);
					Motion(TWO_STEP);
				}
			}
		}
	}

	/* �޸� ���� */
	memory_free();

	uart_close();

	return 0;
}

int find_robot(void)
{
	static int found = 0;		//�������� -> �α׸� ����Ѵ�
	static int notFound = 0;	//�������� -> �α׸� ����Ѵ�
	const int th = 5;		//5�����ӵ��� ��ã���� �κ� �ָ��ִٰ� �Ǵ�

	int mis = 0;
	int size = (sizeof(element) * element_num + sizeof(float) * 3) * G_C_NUM;		// sizeof(float) * 3 = �߰����� R, G, B�� ������
	int count_size = sizeof(int) * 3;
	float temp = 0;
	//int history_idx = 6;
	//int global_green_th; int greed_green_th; int greed_black_th;
	//history_idx = 6;

	/* ���� �ޱ� */
	read_fpga_video_data(fpga_videodata);

	/* �޸� �ʱ�ȭ */
	memset(g, 0, size);			
	memset(gc, 0, count_size);
	memset(rc, 0, count_size);
	memset(bc, 0, count_size);

	/* ��ó�� */
	HSI(fpga_videodata, s_videodata, i_videodata, h_videodata);
	binary(h_videodata, s_videodata, i_videodata, modify_videodata1, NEW_HEIGHT, NEW_WIDTH/*, &global_white*/);
	binaryErosion(modify_videodata1, modify_videodata2, NEW_WIDTH, NEW_HEIGHT, gSE);
	binaryDilation(modify_videodata2, after_mopology, NEW_WIDTH, NEW_HEIGHT, gSE);

	/* �׸��� �Լ� */
	greed_fill(h_videodata, s_videodata, i_videodata, after_mopology, g, rc, gc, bc, NEW_HEIGHT, NEW_WIDTH, &global_green, &global_black);
	max_idx = gc[0].idx;
	green_position(modify_videodata1, g[max_idx], g[max_idx].green_center_x, g[max_idx].green_center_y, &temp);
	//green_position(modify_videodata1, g[max_idx], g[max_idx].green_center_x, g[max_idx].green_center_y, &g[max_idx].green_ratio);
	merging_green_black(after_mopology, max_idx);	//->�ʷ� + ���� ����

	/* ���� �ʷϻ��� �ִ� */
	if(global_green > 100) {

		if(found == 0) { // -> �κ��� �ָ� ������
			/////////////
			/* �κ��̴� -> found ����*/
			if(global_green > 260 && g[max_idx].black_count > 70) {
				found = 1;
				notFound = 0;//

				/* �κ��� ������ �ִ� */
				if(global_green > 550) {
					if(g[max_idx].green_count > 800) {
						printf("robot back\n");
						mis = ROBOT_BACK;
					}
					else {
						printf("ROBOT_IS_FOUND\n");
						mis = ROBOT_IS_FOUND;
						history_idx = max_idx;
					}
				}
				/* �κ��� �ָ� �ִ� */
				else {
					printf("ROBOT_IS_FAR\n");
					mis = ROBOT_IS_FAR;
					history_idx = max_idx;
				}	
			}
			/* �κ��� �ƴϴ� -> notFound ���� */
			else {
				notFound++;
				mis = ROBOT_NOT_FOUND;
				printf("ROBOT_NOT_FOUND\n");
			}
			/////////////
		}
		else if(found == 1) { // -> �κ��� ������ ������
			/////////////
			/* �κ��̴� -> found ����*/
			if(global_green > 260 * 1 / 2 && g[max_idx].black_count > 70 * 1 / 2) {
				notFound = 0;//

				/* �κ��� ������ �ִ� */
				if(global_green > 550 * 1 / 2) {		
					if(g[max_idx].green_count > 800 * 1 / 2) {
						printf("robot back\n");
						mis = ROBOT_BACK;
					}
					else {
						printf("ROBOT_IS_FOUND\n");
						mis = ROBOT_IS_FOUND;
						history_idx = max_idx;
					}
				}
				/* �κ��� �ָ� �ִ� */
				else {
					printf("ROBOT_IS_FAR\n");
					mis = ROBOT_IS_FAR;
					history_idx = max_idx;
				}	
			}
			/* �κ��� �ƴϴ� -> notFound ���� */
			else {
				notFound++;
				mis = ROBOT_NOT_FOUND;
				printf("ROBOT_NOT_FOUND\n");
			}
			/////////////
		}
	}
	/* ���� �ʷϻ��� ���� -> notFound ����*/
	else {
		notFound++;
		mis = ROBOT_NOT_FOUND;
		printf("ROBOT_NOT_FOUND\n");
	}
	
	if(notFound > th) {
		found = 0;
		notFound = 0;
	}

	printf("%d\n", mis);
	global_green = 0;

	/* ���ڰ��׸��� & ������� */
	GraytoRGB565(after_mopology, out_videodata, 180, 120, 180, 120);
	drawCross(out_videodata, g[max_idx].green_center_x, g[max_idx].green_center_y, 10, 2, WIDTH, HEIGHT);
	drawCross(fpga_videodata, g[max_idx].green_center_x, g[max_idx].green_center_y, 10, 2, WIDTH, HEIGHT);
	draw_img_from_buffer_separator(out_videodata, LEFT_SCREEN);		// ����ȭ�ȿ���
	usleep(1);
	draw_img_from_buffer_separator(fpga_videodata, RIGHT_SCREEN);	// ������
	usleep(1);

	flip();

	return mis;
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

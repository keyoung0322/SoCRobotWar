/* ------------------------------------------------------------------------ */
/*                  KAIST BATTLE ROBOT CONTROL PROGRAM						*/
/*                   														*/
/*          Coding by YKH      												*/
/*          file : robot_protocol.h    										*/
/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */
/*                  KAIST BATTLE ROBOT CONTROL PROGRAM            			*/
/*                   														*/
/*          Modified by Goon-Ho Choi, HANOOL ROBOTICS 2003.07.16     		*/
/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef __ROBOT_PROTOCOL_H__
#define __ROBOT_PROTOCOL_H__

#define Header0	0xff
#define Header1 0xff
#define Packet	0x09
#define pID	0xfd				// DRC 253
#define CMD	0x16		
#define DRC_Packet 
#define RAM_READ 0x14
#define MOTION_ADDRESS 0x44		// Address 68

#define ROBOT_NOT_FOUND 100
#define ROBOT_IS_FOUND 101
#define ROBOT_FACING 102
#define ROBOT_BACK 103
#define ROBOT_IS_FAR 104
#define ROBOT_HISTORY_LEFT1 105
#define ROBOT_HISTORY_LEFT2 106
#define ROBOT_HISTORY_RIGHT1 107
#define ROBOT_HISTORY_RIGHT2 108

/* 모션목록 */
#define HELLO_HOVIS 0
#define STAND_1 1
#define STAND_2 2
#define STAND_3 3
#define HEAD_LEFT 4
#define HEAD_RIGHT_45 5
#define ONE_STEP 6
#define TWO_STEP 7
#define BACK_STEP 8
#define TWO_BACK_STEP 9
#define TURN_LEFT_18 10
#define TURN_LEFT_13 11
#define TURN_LEFT_30 12
#define TURN_RIGHT_18 13
#define TURN_RIGHT_13 14
#define TURN_RIGHT_30 15
#define UP_PUNCH 16
#define PUSH_PUNCH 17
#define SIDE_PUNCH 18
#define FRONT_PUNCH 19
#define FRONT_KICK1 20
#define SIDE_KICK 21
#define DOWN_KICK 22
#define TURN_LEFT_90 23
#define SIDE_KICK2_2times 24
#define HEAD_FRONT 25
#define	TEMP_PUNCH 26
#define SIX_STEP 27
#define TURN_LEFT_18_SIDE 28
#define TURN_LEFT_13_PUSH 29
#define TURN_RIGHT_18_TEMP 30
#define FRONT_KICK_TURN_18 31
#define TURN_RIGHT_13_TEMP 32
#define TURN_RIGHT_30_TEMP 33
#define TURN_LEFT_30_SIDE 34
#define SIDE_KICK2 35
#define PUNCH_SET 36

///////////////////////////////////////////////////////////////////////////////
void DelayLoop(int delay_time);
int taekwon(void);
void init_console(void);
void Receive_Ack(void);
void Motion_Ack(unsigned char motionNum);
void Motion(unsigned char motionNum);
void init_robot(void);

int find_robot(void);		//영상정보(R, G, B 중심, count 갯수)를 담고있는 구조체
void memory_allocate(void);
void memory_free(void);
////////////////////////////////////////////////////////////////////////////////

#endif // __ROBOT_PROTOCOL_H__


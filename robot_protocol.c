/************************************************************************
  Title     : Robot Body Protocol Source File
  File name : robot_protocol.c    

  Author    : adc inc. (oxyang@adc.co.kr)
  History
		+ v0.0  2007/2/14
		+ v1.0  2008/8/6
************************************************************************/
#include <stdio.h>
#include <string.h>
#include "robot_protocol.h"
#include "uart_api.h"
//////////////////////////////////////////////////// Protocol Test

void DelayLoop(int delay_time)
{
	while(delay_time)
		delay_time--;
}
void init_robot()
{
	unsigned char Init_Buffer[10] = {0,};
	Init_Buffer[0] = 0xff;
	Init_Buffer[1] = 0xff;
	Init_Buffer[2] = 0x0a;
	Init_Buffer[3] = 0xfe;
	Init_Buffer[4] = 0x03;
	Init_Buffer[5] = 0xa2;
	Init_Buffer[6] = 0x5c;
	Init_Buffer[7] = 0x34;
	Init_Buffer[8] = 0x01;
	Init_Buffer[9] = 0x60;
	uart1_buffer_write(Init_Buffer, 10);
}

#define ERROR 0
#define OK	1

/* Command Function */

void Motion(unsigned char motionNum)
{
	unsigned char CS1, CS2;
	unsigned char Command_Buffer[9] = {0,};
	
	CS1 = (Packet^pID^CMD^motionNum^0x00) & 0xfe;
	CS2 = (~(Packet^pID^CMD^motionNum^0x00)) & 0xfe;

	printf("Motion(%d) start\n", motionNum);

	Command_Buffer[0] = Header0;	
	Command_Buffer[1] = Header1;
	Command_Buffer[2] = Packet;
	Command_Buffer[3] = pID;
	Command_Buffer[4] = CMD;
	Command_Buffer[5] = CS1;
	Command_Buffer[6] = CS2;
	Command_Buffer[7] = motionNum;
	Command_Buffer[8] = 0x00;
	
	uart1_buffer_write(Command_Buffer, 9);

	usleep(500000);
	//printf("CS1=%x\n", CS1);
	//printf("CS2=%x\n", CS2);
}

void Motion_Ack(unsigned char motionNum)
{
	unsigned char CS1, CS2;
	unsigned char Command_Buffer[9] = {0,};
	
	CS1 = (Packet^pID^CMD^motionNum^0x00) & 0xfe;
	CS2 = (~(Packet^pID^CMD^motionNum^0x00)) & 0xfe;

	printf("Motion(%d) start\n", motionNum);

	Command_Buffer[0] = Header0;	
	Command_Buffer[1] = Header1;
	Command_Buffer[2] = Packet;
	Command_Buffer[3] = pID;
	Command_Buffer[4] = CMD;
	Command_Buffer[5] = CS1;
	Command_Buffer[6] = CS2;
	Command_Buffer[7] = motionNum;
	Command_Buffer[8] = 0x00;
	
	uart1_buffer_write(Command_Buffer, 9);

	usleep(100000);
	//printf("CS1=%x\n", CS1);
	//printf("CS2=%x\n", CS2);
	Receive_Ack();
}

void Receive_Ack(void)
{
	int count = 0;
	unsigned char CS1, CS2;
	unsigned char buf[12];
	unsigned char Command_Buffer[9] = {0,};

	CS1 = (Packet^pID^RAM_READ^MOTION_ADDRESS^0x01) & 0xfe;
	CS2 = (~(Packet^pID^RAM_READ^MOTION_ADDRESS^0x01)) & 0xfe;

	Command_Buffer[0] = Header0;	
	Command_Buffer[1] = Header1;
	Command_Buffer[2] = Packet;
	Command_Buffer[3] = pID;
	Command_Buffer[4] = RAM_READ;
	Command_Buffer[5] = CS1;
	Command_Buffer[6] = CS2;
	Command_Buffer[7] = MOTION_ADDRESS;
	Command_Buffer[8] = 0x01;
	
	printf("Ack receive start!!\n");
	
	while(1) {
		int boolean;

		count++;
		uart1_buffer_write(Command_Buffer, 9);
		//memset(buf, 1, 12);
		buf[9] = 1;
		uart1_buffer_read(buf, 0x0C);

		boolean = buf[9];
		
		if(count == 1 && boolean == 0)
			boolean = 1;

		if(boolean == 0) {
			printf("Receive Ack!!\n");
			break;
		}

		else if(count > 70)	{	// е╦юс╬ф©Т
			printf("Time Out!!\n");	
			break;
		}
		usleep(50000);
	}
	usleep(50000);
}

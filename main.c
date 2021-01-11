#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <math.h>

#include "amazon2_sdk.h"
#include "graphic_api.h"
#include "uart_api.h"
#include "robot_protocol.h"

int main(int argc, char **argv)
{
	if (open_graphic() < 0) {
		return -1;
	}
	SURFACE* bmpsurf = 0;

	direct_camera_display_off();
	if( direct_camera_display_stat() )
	{
		return -1;
	}
	else
	{
		printf("camera open\n");
		usleep(100000);//카메라 드라이버를 open한 다음에 일정시간동안 sleep를 해야한다. - 왠지는 모름
		clear_screen();//배경을 흰색으로 채운다
		flip();
		clear_screen();//배경을 흰색으로 채운다
	}

	//graphic_test();
	taekwon();

	printf("Finish\n");
	
	if (bmpsurf != 0)
		release_surface(bmpsurf);
	close_graphic();

	return 0;
}

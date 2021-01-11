#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include "labeling.h"

typedef struct blob_area
{
	int	mark;//마킹한 숫자
	int	area;//넓이
}blob_area;

//라벨링 검출한 영역을 넓이 기준으로 내림차순으로 정렬
static int blob_sort(const void *a, const void *b)
{
	blob_area *one = (blob_area*)a;
	blob_area *two = (blob_area*)b;

	if(one->area < two->area)
	{
		return 1;
	}
	else if(one->area > two->area)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

static int push(short *stackx, short *stacky, int arr_size, int vx, int vy, int *top)
{
	if(*top>=arr_size)
		return(-1);

	(*top)++;
	stackx[*top]=vx;
	stacky[*top]=vy;

	return(1);
}
static int pop(short *stackx, short *stacky, int *vx, int *vy, int *top)
{
	if(*top==0)
		return(-1);

	*vx = stackx[*top];
	*vy = stacky[*top];
	(*top)--;
	
	return(1);
}

// GlassFire 알고리즘을 이용한 라벨링 함수
int BlobColoring(unsigned char* CutImage, int height, int width, struct center center[BLOB_INFO_CNT], int* blob_count, int p_threshhold)
{
	int ret;

	const int arr_size = height * width;
	short* stackx = (short*)malloc(sizeof(short) * arr_size);
	short* stacky = (short*)malloc(sizeof(short) * arr_size);
	short* coloring = (short*)malloc(sizeof(short) * arr_size);

	ret = BlobColoring2(CutImage, height, width, center, blob_count, p_threshhold, stackx, stacky, coloring);

	free(stackx);
	free(stacky);
	free(coloring);

	return ret;
}

int BlobColoring2(unsigned char* CutImage, int height, int width, struct center center[BLOB_INFO_CNT], int* blob_count, int p_threshhold, short *stackx, short *stacky, short *coloring)
{
	if( !center || !CutImage ){
		return 0;
	}

	const int arr_size = height * width;
	const int mark_step = 255 / BLOB_INFO_CNT;//픽셀 증분값

	int i, j, m, n, r, c, top, area, index;
	
	short curColor = 255;//마크할 픽셀번호
	short curColorIndex = 0;//마크를 저장할 배열인덱스

	//라벨링된 영역
	blob_area blob[BLOB_INFO_CNT];
	memset(blob, 0, sizeof(blob_area) * BLOB_INFO_CNT);

	// 스택으로 사용할 메모리 할당
	memset(stackx, 0, sizeof(short)*arr_size);//메모리 초기화
	memset(stacky, 0, sizeof(short)*arr_size);//메모리 초기화
	
	// 라벨링된 픽셀을 저장하기 위해 메모리 할당
	memset(coloring, 0, sizeof(short)*arr_size);//메모리 초기화

	for(i=0; i<height; i++)
	{
		index = i*width;
		for(j=0; j<width; j++)
		{
			// 이미 방문한 점이거나 픽셀값이 255가 아니라면 처리 안함
			if(coloring[index+j] != 0 || CutImage[index+j] != WHITE)
				continue;

			r=i; c=j; top=0; area=1;
			curColor -= mark_step;
			curColorIndex++;

			while(1)
			{
GRASSFIRE:
				for(m=r-1; m<=r+1; m++)
				{
					index = m*width;
					for(n=c-1; n<=c+1; n++)
					{
						//관심 픽셀이 영상경계를 벗어나면 처리 안함
						if(m<0 || m>=height || n<0 || n>=width)
							continue;

						if(CutImage[index+n]==WHITE && coloring[index+n]==0)
						{
							coloring[index+n] = curColor; // 현재 라벨로 마크

							if(push(stackx, stacky, arr_size, (short)m, (short)n, &top) == -1)
								continue;

							r=m; c=n; area++;
							goto GRASSFIRE;
						}
					}
				}
				if(pop(stackx, stacky, &r, &c, &top) == -1)
					break;
			}//while

			//라벨링 됐음
			if( curColorIndex < BLOB_INFO_CNT
				&& area > p_threshhold )
			{
				blob[curColorIndex].mark = curColor;
				blob[curColorIndex].area = area;
			}
		}//for
	}//for

	//라벨링 영역을 면적 내림차순으로 정렬
	qsort((void*)blob, BLOB_INFO_CNT, sizeof(blob_area), blob_sort);

	// CutImage 배열 클리어~
	memset(CutImage, BLACK, arr_size);

	// Center 배열 클리어
	memset(center, 0, sizeof(struct center) * BLOB_INFO_CNT);

	// coloring에 저장된 라벨링 결과중 상위 몇개 영역만 저장 
	// 무게 중심도 같이 구하기
	for(i = 0; i < BLOB_INFO_CNT; i++)
	{
		if(blob[i].area == 0){
			break;
		}

		//center[i].x_center = 0;
		//center[i].y_center = 0;
		center[i].left_x = width;
		//center[i].right_x = 0;
		//center[i].top_x = 0;
		//center[i].top_y = 0;
		//center[i].bot_y = 0;

		center[i].pixel_count = blob[i].area;
		center[i].mark = blob[i].mark;

		for(r = 0; r < height; r++)
		{
			index = r * width;
			for(c = 0; c < width; c++)
			{
				if( coloring[index + c] == blob[i].mark )
				{
					CutImage[index + c] = blob[i].mark;

					//무게중심 찾기
					center[i].x_center += c;
					center[i].y_center += r;
					
					//위쪽픽셀 찾기 - 맨 위에 있는 픽셀 찾기
					if(center[i].top_x == 0)
					{
						center[i].top_x = c;
						center[i].top_y = r;
					}

					//왼쪽픽셀 찾기
					if(center[i].left_x > c)
					{
						center[i].left_x = c;
						center[i].left_y = r;
					}

					//오른쪽픽셀 찾기
					if(center[i].right_x < c)
					{
						center[i].right_x = c;
						center[i].right_y = r;
					}
					
					//아래쪽픽셀 찾기
					if(center[i].bot_y < r)
					{
						center[i].bot_x = c;
						center[i].bot_y = r;
					}
				}
			}
		}
	
		center[i].x_center /= center[i].pixel_count;
		center[i].y_center /= center[i].pixel_count;
	
		//맨 아래에 있는 픽셀 찾기
#if 0
		for(r = height-1; r >= 0; r--)
		{
			index = r * width;
			for(c = width-1; c >= 0; c--)
			{
				if(coloring[index + c] == blob[i].mark)
				{
					center[i].bot_x = c;
					center[i].bot_y = r;
					break;
				}
			}
			if(c >= 0)
			{
				break;
			}
		}
#endif
	}//for

	//라벨링 영역 개수 저장
	*blob_count = i;

	return 1;
}

//라벨링 영역을 조작 후 다시 정렬할 때 사용, 내림차순
int blob_sort_re(const void *a, const void *b)
{
	struct center *one = (struct center*)a;
	struct center *two = (struct center*)b;

	if(one->pixel_count < two->pixel_count)
	{
		return 1;
	}
	else if(one->pixel_count > two->pixel_count)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}


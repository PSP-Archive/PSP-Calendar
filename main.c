#include "types.h"
#include "file.h"
#include "pg.h"
#include "string.h"
#include "power.h"
#include "japanese.h"
#include "controller.h"
#include "psptime.c"
#include "inifile.h"
#include "resource.h"
#include "num16.h"

//#define	NO_TIME_CALL

#define	OFFSET_X	16
#define	OFFSET_Y	64
#define	SIZE_X		64
#define	SIZE_Y		32

#define	N_FARAME	2

#define	RGB(r,g,b)	((b<<10)+(g<<5)+(r)+0x8000)

#define	COLOR_BACK		RGB(31,31,31)
#define	COLOR_LINE		RGB(12,12,12)
#define	COLOR_CUR		RGB(31,31, 0)
#define	COLOR_TODAY		RGB( 0,31,31)
#define	COLOR_TIME		RGB( 0, 0, 0)
#define	COLOR_HOLIDAY	RGB(12,12,12)

#define	COLOR_IX_BACK		0
#define	COLOR_IX_SUN		1
#define	COLOR_IX_HOLIDAY	1
#define	COLOR_IX_SAT		2
#define	COLOR_IX_CUR		3
#define	COLOR_IX_TODAY		4
#define	COLOR_IX_SPECIAL	5
#define	COLOR_IX_DIS		6

u8	bUseBack = FALSE;
u16	BackBmp[SCREEN_WIDTH*SCREEN_HEIGHT];
static	u8	nTranceparent = 95;

unsigned short	FrameCol[][4] =
	{
		{	COLOR_LINE   , COLOR_BACK   , COLOR_BACK	},	/* BACK */
		{	RGB(31, 8, 8), RGB(31,16,16), RGB(31,24,24)	},	/* RED */
		{	RGB( 8, 8,31), RGB(16,16,31), RGB(24,24,31)	},	/* BLUE */
		{	RGB(31,31, 8), RGB(31,31,16), RGB(31,31,24)	},	/* YELLOW */
		{	RGB( 8,31,31), RGB(16,31,31), RGB(24,31,31)	},	/* CYAN */
		{	RGB( 8,31, 8), RGB(16,31,16), RGB(24,31,24)	},	/* GREEN */
		{	COLOR_LINE   , RGB(24,24,24), RGB(24,24,24)	},	/* DISABLE */
	};

#define	FRAME_LINE_LEN	12
char wtbl[7][FRAME_LINE_LEN-2+1] = RES_WEEKS;
char date_week[7][4+1] = RES_DATE_WEEKS;

#define	MSG_LEN		63
#define	MSG_LINE	3
char	day_msg[31][MSG_LINE][MSG_LEN+1];

static	struct	tagRESOURCE
	{
		char	Menu[4][40];
	}	res;

static	int	dwLocalTimeAdj = (3600*9);	// GMT+9 Tokyo Japan

static	unsigned short	cur_year;
static	unsigned short	cur_mon;
static	unsigned short	cur_day;
static	unsigned short	cur_offset;
static	unsigned short	cur_max_day;
static	long			cur_dtime;

struct	tm	now_tim;

static	unsigned short	cur_x;
static	unsigned short	cur_y;

char	lpszCurrent[MAX_PATH];
char	lpszSchedulePath[MAX_PATH];
static	char	lpszReadBuff[1024*16];


#define	MAX_HOLIDAY			512
#define	MAX_HOLIDAY_TEXT	10
typedef	struct	tagHOLIDAY
	{
		u16		year;
		u16		mon;
		u16		day;
		u16		col_ix;
		char	lpszText[MAX_HOLIDAY_TEXT+1];
	}	HOLIDAY;
u16	nHoliday;
HOLIDAY	hol[MAX_HOLIDAY];

struct	tm
	{
	unsigned short	tm_year;
	unsigned short	tm_mon;
	unsigned short	tm_mday;
	unsigned short	tm_hour;
	unsigned short	tm_min;
	unsigned short	tm_sec;
	unsigned short	tm_wday;
	unsigned short	tm_max_day;
	};
static	struct	tm	gtm;
const	unsigned char	mtbl1[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const	unsigned char	mtbl2[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static	struct	tm	gtm;
struct	tm* ltime(unsigned long *dtime)
{
	unsigned char	*mtbl;
	unsigned short	i;
	unsigned long	days;
	unsigned long	times;
	
	days  = ((unsigned long)*dtime) / (24*60*60);
	times = ((unsigned long)*dtime) % (24*60*60);

	gtm.tm_wday = (days+4) % 7;

	gtm.tm_year = 70;
	while(days > (365+365+366+365))
	  {
	  gtm.tm_year += 4;
	  days -= (365+365+366+365);
	  }
//	if (gtm.tm_year > 100)
//	  days++;

	if (days >= 365)
	  {
	  gtm.tm_year ++;
	  days -= 365;
	  }
	if (days >= 365)
	  {
	  gtm.tm_year ++;
	  days -= 365;
	  }
	if (days >= 366)
	  {
	  gtm.tm_year ++;
	  days -= 366;

	  if (days >= 365)
	    {
	    gtm.tm_year ++;
	    days -= 365;
	    }
	  }
	
	if ((gtm.tm_year != 200)&&((gtm.tm_year % 4) == 0))
	  mtbl = (unsigned char*)mtbl2;
	else
	  mtbl = (unsigned char*)mtbl1;
	gtm.tm_mon = 0;
	
	for(i=0; i<12; i++)
	  {
	  if (mtbl[i] >= (days+1))
	    break;
	  days -= mtbl[i];
	  }
	
	gtm.tm_mon  = i;
	gtm.tm_mday = days+1;
	gtm.tm_max_day = mtbl[i];

	gtm.tm_hour = times / (60*60);
	gtm.tm_min  = (times % (60*60)) / 60;
	gtm.tm_sec  = times % 60;
	
	return(&gtm);
}
void DotToAsc1(char *lpszText, struct tm *tim)
	{
	lpszText[ 0] = '0' + ((tim->tm_year+1900) / 1000);
	lpszText[ 1] = '0' + (((tim->tm_year+1900)%1000) / 100);
	lpszText[ 2] = '0' + (((tim->tm_year+1900)% 100) /  10);
	lpszText[ 3] = '0' + (((tim->tm_year+1900)%  10) /   1);
	lpszText[ 4] = '/';
	lpszText[ 5] = '0' + (((tim->tm_mon+1)% 100) /  10);
	lpszText[ 6] = '0' + (((tim->tm_mon+1)%  10) /   1);
	lpszText[ 7] = '/';
	lpszText[ 8] = '0' + ((tim->tm_mday% 100) /  10);
	lpszText[ 9] = '0' + ((tim->tm_mday%  10) /   1);
	lpszText[10] = ' ';
	lpszText[11] = date_week[tim->tm_wday][0];
	lpszText[12] = date_week[tim->tm_wday][1];
	lpszText[13] = date_week[tim->tm_wday][2];
	lpszText[14] = date_week[tim->tm_wday][3];
	lpszText[15] = 0x00;;
	}
	
void DotToAsc2(char *lpszText, struct tm *tim)
	{
	lpszText[ 0] = '0' + (((tim->tm_hour)% 100) /  10);
	lpszText[ 1] = '0' + (((tim->tm_hour)%  10) /   1);
	lpszText[ 2] = ':';
	lpszText[ 3] = '0' + (((tim->tm_min)% 100) /  10);
	lpszText[ 4] = '0' + (((tim->tm_min)%  10) /   1);
	lpszText[ 5] = ':';
	lpszText[ 6] = '0' + (((tim->tm_sec)% 100) /  10);
	lpszText[ 7] = '0' + (((tim->tm_sec)%  10) /   1);
	lpszText[ 8] = 0x00;
	}


int GetLocalTime()
{
	struct timeval {
		long	tv_sec;/* seconds */
		long	suseconds_ttv_usec;/* microseconds */
	}	tv;
	struct timezone {
		int		tz_minuteswest; /* minutes W of Greenwich */
		int		tz_dsttime;     /* type of dst correction */
		int		a;
		int		b;
	} tz;

//	sceKernelLibcGettimeofday(&tv, &tz);	/* Bug? tz.tz_minuteswest,tz.tz_dsttime == 0 */
	return(dwLocalTimeAdj);
}

unsigned long GetNowTime(struct tm *tim_ret)
{
	unsigned long	tempy;
	struct	tm	*tim;

#ifndef	NO_TIME_CALL
	tempy = (unsigned long)sceKernelLibcTime((void *) 0);
#else
	tempy = 1116117184;
#endif
	tempy += GetLocalTime();

	if (tim_ret)
	{
		tim = ltime(&tempy);
		tim_ret->tm_year = tim->tm_year;
		tim_ret->tm_mon  = tim->tm_mon;
		tim_ret->tm_mday = tim->tm_mday;
		tim_ret->tm_hour = tim->tm_hour;
		tim_ret->tm_min  = tim->tm_min;
		tim_ret->tm_sec  = tim->tm_sec;
		tim_ret->tm_wday = tim->tm_wday;
		tim_ret->tm_max_day = tim->tm_max_day;
	}

	return(tempy);
}

unsigned long WaitInput(void)
{
	unsigned long key;
	int i;

	// wait key
	while(1) {
		key = Read_Key(REPEAT_TIME);
		if (key != 0) break;
		pgWaitV();	break;
	}
	return(key);
}

#define	TIME_X1	(480-5*24 -2)
#define	TIME_X2	(480-5* 8 -2)
#define	TIME_Y	1
void DrawTime()
{
	unsigned char	lpszText[24];

	GetNowTime(&now_tim);
	
	pgBoxFillAlpha(TIME_X1, TIME_Y, TIME_X1+5*24, TIME_Y+10, COLOR_BACK, 0);
	DotToAsc1(lpszText, &now_tim);
	print_japanese(TIME_X1,TIME_Y,lpszText,COLOR_TIME,0,0);
	DotToAsc2(lpszText, &now_tim);
	print_japanese(TIME_X2,TIME_Y,lpszText,COLOR_TIME,0,0);
}

void DrawFrame(int ix, int iy, unsigned short *col)
{
	pgBox(OFFSET_X+ix*SIZE_X  , OFFSET_Y+iy*SIZE_Y  , OFFSET_X+ix*SIZE_X+SIZE_X-1,OFFSET_Y+iy*SIZE_Y+SIZE_Y-1, col[0]);
	pgBox(OFFSET_X+ix*SIZE_X+1, OFFSET_Y+iy*SIZE_Y+1, OFFSET_X+ix*SIZE_X+SIZE_X-2,OFFSET_Y+iy*SIZE_Y+SIZE_Y-2, col[1]);
//	pgBoxFill(OFFSET_X+ix*SIZE_X+2, OFFSET_Y+iy*SIZE_Y+2, OFFSET_X+ix*SIZE_X+SIZE_X-3,OFFSET_Y+iy*SIZE_Y+SIZE_Y-3, col[2]);
	pgBoxFillAlpha(OFFSET_X+ix*SIZE_X+2, OFFSET_Y+iy*SIZE_Y+2, OFFSET_X+ix*SIZE_X+SIZE_X-3,OFFSET_Y+iy*SIZE_Y+SIZE_Y-3, col[2], nTranceparent);
}

HOLIDAY *GetHoliday(u16 year, u16 mon, u16 day)
{
	int	i;
	for(i=0; i<nHoliday; i++)
	{
		if (((hol[i].year==year)||(hol[i].year==0))&&
		    (hol[i].mon==mon)&&
		    (hol[i].day==day))
		{
			return(&hol[i]);
		}
	}
	return(NULL);
}

int GetFrameColor(int x, int y, char *lpszText)
{
	int	ret;
	
	lpszText[0] = 0x00;
	if (((x+y*7)<cur_offset) || (((x+y*7)-cur_offset)>=cur_max_day))
	{
		ret = COLOR_IX_DIS;
	}
	else
	{
		u16	day;
		HOLIDAY	*lpHol;
		
		day = (x+y*7-cur_offset+1);

		lpHol = GetHoliday(cur_year+1900, cur_mon, day);
		if (lpHol)
		{
			strcpy(lpszText,lpHol->lpszText);
		}
		
		if ((cur_x==x)&&(cur_y==y))
		{
			ret = COLOR_IX_CUR;
		}
		else if ((now_tim.tm_year==cur_year)&&(now_tim.tm_mon==cur_mon)&&(now_tim.tm_mday==day))
		{
			ret = COLOR_IX_TODAY;
		}
		else
		{
			
			if (lpHol)
			{
				ret = lpHol->col_ix;
			}
			else if (x==0)
			{
				ret = COLOR_IX_SUN;
			}
			else if (x==6)
			{
				ret = COLOR_IX_SAT;
			}
			else
			{
				ret = COLOR_IX_BACK;
			}
		}
	}

	return(ret);
}

void DrawBack()
{
	int	x, y;
	int	i;
	int	ix;
	char	lpszText[24];
	char	lpszHoliday[MAX_HOLIDAY_TEXT+1];
    
	if (bUseBack)
		pgBitBlt(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, BackBmp);
	else
		pgFillvram(COLOR_BACK);
	
	dec2asc(cur_year+1900, lpszText, 4);
	lpszText[4] = ' ';
	dec2asc(cur_mon+1, &lpszText[5], 2);
	print_num16(198,20,lpszText,COLOR_TIME,0,0);

	pgBox(OFFSET_X -1, OFFSET_Y-12-1, OFFSET_X + 7*SIZE_X, OFFSET_Y, COLOR_LINE);
	for (x=0;x<7;x++)
	{
		unsigned short	col;
		
		pgBox(OFFSET_X + x*SIZE_X, OFFSET_Y-12, OFFSET_X + x*SIZE_X+SIZE_X-1, OFFSET_Y-1, COLOR_LINE);
		if (bUseBack)
		{
			pgBoxFillAlpha(OFFSET_X + x*SIZE_X+1, OFFSET_Y-12+1, OFFSET_X + x*SIZE_X+SIZE_X-2, OFFSET_Y-2, COLOR_BACK, nTranceparent);
		}
		if (x==0)
			col = FrameCol[COLOR_IX_SUN][0];
		else if (x==6)
			col = FrameCol[COLOR_IX_SAT][0];
		else
			col = RGB(0,0,0);
		print_japanese(OFFSET_X + x*SIZE_X+1, OFFSET_Y-(12-1),(char*)wtbl[x],col,0,0);
	}
	
	for (y=0;y<6;y++)
	{
		for (x=0;x<7;x++)
		{
			if ((y==5)&&(x>=2))
			{
				if (bUseBack)
				{
					pgBoxFillAlpha(OFFSET_X + x*SIZE_X+1, OFFSET_Y + y*SIZE_Y+1, OFFSET_X + (x+4)*SIZE_X+SIZE_X-1, OFFSET_Y + y*SIZE_Y+SIZE_Y-1, COLOR_BACK, nTranceparent);
				}
				break;
			}
			
			ix = GetFrameColor(x, y, lpszHoliday);
			
			pgBox(OFFSET_X + x*SIZE_X, OFFSET_Y + y*SIZE_Y, OFFSET_X + x*SIZE_X+SIZE_X-1, OFFSET_Y + y*SIZE_Y+SIZE_Y-1, COLOR_LINE);
			DrawFrame(x,y,FrameCol[ix]);
			if (lpszHoliday[0])
			{
				print_japanese(OFFSET_X+x*SIZE_X+1+10, OFFSET_Y+y*SIZE_Y+1,lpszHoliday,COLOR_HOLIDAY,0,0);
			}
		}
	}

	pgBox(OFFSET_X + 2*SIZE_X, OFFSET_Y + 5*SIZE_Y, OFFSET_X + 6*SIZE_X+SIZE_X-1, OFFSET_Y + 5*SIZE_Y+SIZE_Y-1, COLOR_LINE);

	pgBox(OFFSET_X-1, OFFSET_Y-1, OFFSET_X+7*SIZE_X, OFFSET_Y+6*SIZE_Y, COLOR_LINE);

//	DrawFrame(cur_x, cur_y, FrameCol[COLOR_IX_CUR]);

	/* Draw Date*/
	for(i=0;i<cur_max_day;i++)
	{
		char	lpszText[3];
		
		lpszText[0] = ((i+1)<10)?' ':'0'+((i+1)/10);
		lpszText[1] = '0'+((i+1)%10);
		lpszText[2] = 0x00;
		x = (cur_offset+i)%7;
		y = (cur_offset+i)/7;
		print_japanese(OFFSET_X + x*SIZE_X+1, OFFSET_Y + y*SIZE_Y+1,lpszText,RGB(0,0,0),0,0);
	}
}

void DrawText()
{
	int	x,y;
	int	i;
	int	j;
	
	for (i=0; i<cur_max_day; i++)
	{
		y = (cur_offset + i) / 7;
		x = (cur_offset + i) % 7;

		for (j=0; j<MSG_LINE; j++)
		{
			char	lpszText[FRAME_LINE_LEN+1];
			
			if (j<MSG_LINE-1)
			{
				getline(day_msg[i][j], lpszText, FRAME_LINE_LEN);
				print_japanese(OFFSET_X + x*SIZE_X+1, OFFSET_Y + y*SIZE_Y+1+(j+1)*10,lpszText,RGB(0,0,0),0,0);
			}
			
			if (cur_day == (i+1))
			{
				print_japanese(OFFSET_X + 2*SIZE_X+1, OFFSET_Y + 5*SIZE_Y+1+j*10,day_msg[i][j],RGB(0,0,0),0,0);
			}
		}
	}
}

void ReadMsg(u16 year, u16 mon, u16 max_day)
{
	char	lpszFile[MAX_PATH];
	char	lpszPath[16];
	int		fd;
	int		len;
	char	*p;
	char	*d;
	int		i;
	int		j;

	strcpy(lpszFile, lpszSchedulePath);
	p = lpszFile + strlen(lpszFile);
	
	*p = '/';	p++;
	p += itoan((int)year, p, 4);
	
	*p = '/';	p++;
	p += itoan((int)mon , p, 2);
	
	d = p+1;
	strcat(p, "/01.txt");
	
	for (i=0; i<max_day; i++)
	{
		d[0] = '0'+((i+1)/10);
		d[1] = '0'+((i+1)%10);

		fd = fopen(lpszFile, "r");
		if (fd>=0)
		{
			char	w[(MSG_LEN+2)*MSG_LINE+1];
			
			len = fread(lpszReadBuff, (MSG_LEN+2)*MSG_LINE, 1, fd);
			fclose(fd);
			
			lpszReadBuff[len] = 0x00;
			
			p = lpszReadBuff;
			for (j=0; j<MSG_LINE; j++)
			{
				p += getline(p, w, (MSG_LEN+2)*MSG_LINE);
				getline(w, day_msg[i][j], MSG_LEN);
/*
//debug
{
print_japanese(0, 0,day_msg[i][j],RGB(31,0,0),0,1);
pgScreenFlipV();
while(WaitInput()==0);
}
*/
			}
		}
		else
		{
			for (j=0; j<MSG_LINE; j++)
			{
			day_msg[i][j][0] = 0x00;
			}
		}
	}
}

void SetCurrent(struct tm *tim)
{
	char	bMsgRead;
	
	if (cur_mon != tim->tm_mon)
		bMsgRead = TRUE;
	else
		bMsgRead = FALSE;
		
	cur_year   = tim->tm_year;
	cur_mon    = tim->tm_mon;
	cur_day    = tim->tm_mday;
	cur_offset = (tim->tm_wday+8 - (tim->tm_mday%7)) % 7;
	cur_max_day= tim->tm_max_day;
	
	if (bMsgRead)
		ReadMsg(cur_year+1900, cur_mon+1, cur_max_day);
}

void MoveDay(int move_day)
{
	struct tm	*tim;
	
	(int)cur_dtime = (int)cur_dtime + (24*60*60*move_day);
	
	tim = ltime(&cur_dtime);
	SetCurrent(tim);
	
	cur_x = (cur_day-1 + cur_offset) % 7;
	cur_y = (cur_day-1 + cur_offset) / 7;
}

void MoveMon(int move_mon)
{
	u16	old_mon;
	
	old_mon = cur_mon;
	
	while(old_mon == cur_mon)
	{
		MoveDay(7*move_mon);
	}
}

void holiday_callback(char *lpszSec, char *lpszKey, char *lpszVal)
{
	if (nHoliday >= MAX_HOLIDAY)
		return;
	
/*
//debug
{
print_japanese(0, 0,lpszSec,RGB(31,0,0),0,0);
print_japanese(0,10,lpszKey,RGB(31,0,0),0,0);
print_japanese(0,20,lpszVal,RGB(31,0,0),0,0);
pgScreenFlipV();
while(WaitInput()==0);
}
*/
	if (strlen(lpszKey) < 4)
		return;
	
	if (strcmp(lpszSec, "ALL")==0)
	{
		hol[nHoliday].year = 0;
	}
	else
	{
		hol[nHoliday].year = (lpszSec[0]-'0')*1000 + (lpszSec[1]-'0')*100 + (lpszSec[2]-'0')*10 + (lpszSec[3]-'0');
	}

	hol[nHoliday].mon    = (lpszKey[0]-'0')*10 + (lpszKey[1]-'0') -1;
	hol[nHoliday].day    = (lpszKey[2]-'0')*10 + (lpszKey[3]-'0');
	hol[nHoliday].col_ix = lpszKey[4]=='S' ? COLOR_IX_SPECIAL:COLOR_IX_HOLIDAY;
	getline(lpszVal, hol[nHoliday].lpszText, MAX_HOLIDAY_TEXT);
	
	nHoliday++;
}

void read_holiday(char *lpszFile)
{
	int	fd;
	int	len;

	nHoliday = 0;
	
	fd = fopen(lpszFile, "r");
	if (fd>=0)
	{
		len = fread(lpszReadBuff, sizeof(lpszReadBuff), 1, fd);
		fclose(fd);
		lpszReadBuff[len] = 0x00;
		inifile(lpszReadBuff, holiday_callback);
	}
	
}

void config_callback(char *lpszSec, char *lpszKey, char *lpszVal)
{
/*
//debug
{
print_japanese(0, 0,lpszSec,RGB(31,0,0),RGB(1,1,1),1);
print_japanese(0,10,lpszKey,RGB(31,0,0),RGB(1,1,1),1);
print_japanese(0,20,lpszVal,RGB(31,0,0),RGB(1,1,1),1);
pgScreenFlipV();
while(WaitInput()==0);
}
*/
	if (stricmp(lpszSec, "Env")==0)
	{
		if (stricmp(lpszKey, "TZ")==0)
		{
			dwLocalTimeAdj = atoi(lpszVal) * 3600;
		}
		else if (stricmp(lpszKey, "LANG")==0)
		{
//			if (stricmp(lpszVal, "JP")==0)	memcpy(&res, &res_jp, sizeof(res));
		}
		else if (stricmp(lpszKey, "Schedule")==0)
		{
			strcpy(lpszSchedulePath, lpszVal);
		}
		else if (stricmp(lpszKey, "Tranceparent")==0)
		{
			nTranceparent = atoi(lpszVal) % 101;
		}
	}
	else if (stricmp(lpszSec, "WEEK")==0)
	{
		if (stricmp(lpszKey, "SUN")==0)	getline(lpszVal, wtbl[0], FRAME_LINE_LEN-2);
		if (stricmp(lpszKey, "MON")==0)	getline(lpszVal, wtbl[1], FRAME_LINE_LEN-2);
		if (stricmp(lpszKey, "TUE")==0)	getline(lpszVal, wtbl[2], FRAME_LINE_LEN-2);
		if (stricmp(lpszKey, "WED")==0)	getline(lpszVal, wtbl[3], FRAME_LINE_LEN-2);
		if (stricmp(lpszKey, "THU")==0)	getline(lpszVal, wtbl[4], FRAME_LINE_LEN-2);
		if (stricmp(lpszKey, "FRI")==0)	getline(lpszVal, wtbl[5], FRAME_LINE_LEN-2);
		if (stricmp(lpszKey, "SAT")==0)	getline(lpszVal, wtbl[6], FRAME_LINE_LEN-2);
	}
	else if (stricmp(lpszSec, "TIME")==0)
	{
		if (stricmp(lpszKey, "SUN")==0)	getline(lpszVal, date_week[0], 4);
		if (stricmp(lpszKey, "MON")==0)	getline(lpszVal, date_week[1], 4);
		if (stricmp(lpszKey, "TUE")==0)	getline(lpszVal, date_week[2], 4);
		if (stricmp(lpszKey, "WED")==0)	getline(lpszVal, date_week[3], 4);
		if (stricmp(lpszKey, "THU")==0)	getline(lpszVal, date_week[4], 4);
		if (stricmp(lpszKey, "FRI")==0)	getline(lpszVal, date_week[5], 4);
		if (stricmp(lpszKey, "SAT")==0)	getline(lpszVal, date_week[6], 4);
	}
}

void read_config(char *lpszFile)
{
	int	fd;
	int	len;

	nHoliday = 0;
	
	fd = fopen(lpszFile, "r");
	if (fd>=0)
	{
		len = fread(lpszReadBuff, sizeof(lpszReadBuff), 1, fd);
		fclose(fd);
		lpszReadBuff[len] = 0x00;
		inifile(lpszReadBuff, config_callback);
	}
	
}

u8	TmpBmp[SCREEN_WIDTH*3];
void LoadBackBmp(char *lpszFile)
{
	int	fd;
	int	len;
	int	x, y;
	u8	*p;

	fd = fopen(lpszFile, "r");
	if (fd>=0)
	{
		len = fread(TmpBmp, 0x36, 1, fd);		/* 0x36 = BMP Header size */
		for (y=SCREEN_HEIGHT-1; y>=0; y--)
		{
			p = TmpBmp;
			len = fread(TmpBmp, sizeof(TmpBmp), 1, fd);
			for (x=0; x<SCREEN_WIDTH; x++)
			{
				BackBmp[x+y*SCREEN_WIDTH] = RGB((p[2]>>3), (p[1]>>3), (p[0]>>3));
				p+=3;
			}
		}
		fclose(fd);

		bUseBack = TRUE;
		return;
	}

	memset(BackBmp, COLOR_BACK, sizeof(BackBmp));
}

/*****************************************************/
#define	MAX_SCHEDULE_TEXT	(1024*64)
#define	MAX_SCHEDULE_LINE	(MAX_SCHEDULE_TEXT/4)
#define	MAX_SCHEDULE_1LINE	((480-10)/5)
#define	MAX_SCHEDULE_LN		((272-12)/10)
#define	SCROLL_BAR_TOP_X	(MAX_SCHEDULE_1LINE*5)
#define	SCROLL_BAR_TOP_Y	(12)
char	ScheduleText[MAX_SCHEDULE_TEXT];
u32		nln;
char	*ln[MAX_SCHEDULE_LINE];
char	*sc_p[MAX_SCHEDULE_LINE];

void ViewSchedule(u16 year, u16 mon, u16 day)
{
	unsigned short	key;
	char	lpszFile[MAX_PATH];
	int		fd;
	int		len;
	char	*p;
	int		i;
	char	w[MAX_SCHEDULE_1LINE];
	char	EraceDraw;
	u32		ViewTop;
	u32		ScSize;
	u32		ScPos;
	char	lpszTitle[] = "0000/00/00";

	strcpy(lpszFile, lpszSchedulePath);
	p = lpszFile + strlen(lpszFile);
	
	*p = '/';	p++;
	p += itoan((int)year, p, 4);
	
	*p = '/';	p++;
	p += itoan((int)mon , p, 2);
	
	*p = '/';	p++;
	p += itoan((int)day , p, 2);

	strcat(p, ".txt");

	itoan((int)year, &lpszTitle[0], 4);	lpszTitle[4] = '/';
	itoan((int)mon , &lpszTitle[5], 2);	lpszTitle[7] = '/';
	itoan((int)day , &lpszTitle[8], 2);
	
	nln = 0;
	ScheduleText[0] = 0x00;
	ViewTop = 0;
	ScPos = 0;
	p = ScheduleText;
	sc_p[0] = p;

	fd = fopen(lpszFile, "r");
	if (fd>=0)
	{
		len = fread(ScheduleText, sizeof(ScheduleText), 1, fd);
		fclose(fd);
		ScheduleText[len] = 0x00;
		
		for (i=1; i<MAX_SCHEDULE_LINE; i++)
		{
			len = getline(p, w, sizeof(w));
			if (len == 0)
				break;
			
			p += len;
			sc_p[i] = p;
		}
		nln = i;
	}
	
	if (nln <= MAX_SCHEDULE_LN)
	{
		ScSize = MAX_SCHEDULE_LN * 10 -1;
	}
	else
	{
		ScSize = (MAX_SCHEDULE_LN*10*MAX_SCHEDULE_LN) / nln;
		if (ScSize<6)
			ScSize = 6;
	}
	
	EraceDraw = N_FARAME;
	for(;;)
	{
		key = Read_Key(REPEAT_TIME);
		if (key & CTRL_CROSS)
		{
			break;
		}
		else if (key & CTRL_UP)
		{
			if (ViewTop)
			{
				ViewTop--;
				EraceDraw = N_FARAME;
			}
		}
		else if (key & CTRL_DOWN)
		{
			if (nln>MAX_SCHEDULE_LN)
			{
				if ((ViewTop+MAX_SCHEDULE_LN)<nln)
				{
					ViewTop++;
					EraceDraw = N_FARAME;
				}
			}
		}
		else if (key & CTRL_LTRIGGER)
		{
			if (ViewTop)
			{
				if (ViewTop<=MAX_SCHEDULE_LN)
					ViewTop = 0;
				else
					ViewTop -= MAX_SCHEDULE_LN;
				EraceDraw = N_FARAME;
			}
		}
		else if (key & CTRL_RTRIGGER)
		{
			if (nln>MAX_SCHEDULE_LN)
			{
				if ((ViewTop+MAX_SCHEDULE_LN)<nln)
				{
					ViewTop += MAX_SCHEDULE_LN;
					if ((ViewTop+MAX_SCHEDULE_LN)>=nln)
						ViewTop = nln - MAX_SCHEDULE_LN;
					EraceDraw = N_FARAME;
				}
			}
		}

		if (EraceDraw)
		{
			if (bUseBack)
			{
				pgBitBlt(0, 0, SCREEN_WIDTH, 12, 1, BackBmp);
				pgBoxFillAlpha(0, SCROLL_BAR_TOP_Y, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, COLOR_BACK, nTranceparent);
			}
			else
			{
				pgFillvram(COLOR_BACK);
			}
			print_japanese(0,TIME_Y,lpszTitle,COLOR_TIME,0,0);
			
//			pgLine(0, SCROLL_BAR_TOP_Y-1, SCREEN_WIDTH-1, SCROLL_BAR_TOP_Y-1,COLOR_LINE);
			
			// Scroll Bar
			pgBox(SCROLL_BAR_TOP_X, SCROLL_BAR_TOP_Y-1, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, COLOR_LINE);
			if (bUseBack)
				pgBoxFillAlpha(SCROLL_BAR_TOP_X+1, SCROLL_BAR_TOP_Y-1+1, SCREEN_WIDTH-1-1, SCREEN_HEIGHT-1-1, COLOR_LINE, nTranceparent);
			else
				pgBoxFill(SCROLL_BAR_TOP_X+1, SCROLL_BAR_TOP_Y-1+1, SCREEN_WIDTH-1-1, SCREEN_HEIGHT-1-1, COLOR_LINE);

			if (nln)
				ScPos = ((MAX_SCHEDULE_LN*10)*ViewTop)/nln;
			
			if (bUseBack)
			{
				pgBox(SCROLL_BAR_TOP_X+1, SCROLL_BAR_TOP_Y+ScPos, SCREEN_WIDTH-2, SCROLL_BAR_TOP_Y + ScPos + ScSize-1, COLOR_CUR);
				pgBoxFillAlpha(SCROLL_BAR_TOP_X+2, SCROLL_BAR_TOP_Y+ScPos+1, SCREEN_WIDTH-3, SCROLL_BAR_TOP_Y + ScPos + ScSize-2, COLOR_CUR, nTranceparent);
			}
			else
			{
				pgBoxFill(SCROLL_BAR_TOP_X+1, SCROLL_BAR_TOP_Y+ScPos, SCREEN_WIDTH-2, SCROLL_BAR_TOP_Y + ScPos + ScSize-1, COLOR_CUR);
			}

			for (i=0;i<MAX_SCHEDULE_LN; i++)
			{
				if ((ViewTop+i)>=nln)
					break;
				getline(sc_p[ViewTop+i], w, sizeof(w));
				print_japanese(0,i*10+12,w,COLOR_TIME,0,0);
			}
			EraceDraw--;
		}
		DrawTime();
		pgLine(0, SCROLL_BAR_TOP_Y-1, SCREEN_WIDTH-1, SCROLL_BAR_TOP_Y-1,COLOR_LINE);
		pgScreenFlipV();
	}
}
/*****************************************************/

int xmain(int argc, char *argv)
{
	struct	tm	tim;
	char	*p;
	char	lpszFile[MAX_PATH];
	char	EraceDraw;

	strcpy(lpszCurrent, argv);
	p = strrchr(lpszCurrent, '/');
	*p = 0x00;

	// Initialize
	pgInit();
	SetupCallbacks();
	pgScreenFrame(N_FARAME,0);
	sceCtrlInit(0);
	sceCtrlSetAnalogMode(0);

	strcpy(lpszSchedulePath, lpszCurrent);
	
	strcpy(lpszFile, lpszCurrent);
	strcat(lpszFile, "/Calendar.ini");
	read_config(lpszFile);
	
	strcpy(lpszFile, lpszSchedulePath);
	strcat(lpszFile, "/Holiday.ini");
	read_holiday(lpszFile);
	
	strcpy(lpszFile, lpszCurrent);
	strcat(lpszFile, "/BACK.BMP");
	LoadBackBmp(lpszFile);

	cur_dtime = GetNowTime(&tim);
	SetCurrent(&tim);

//debug
/*
{
char	lpszText[3] = "00";
int	i;
u16	c = 0x0001;
for (i=0;i<16;i++)
{
	lpszText[0]='0'+i/10;
	lpszText[1]='0'+i%10;
	
	print_japanese(0, 0,lpszText,RGB(31,0,0),RGB(1,1,1),1);
	pgBoxFill(10, 10, 20, 20, c);
	pgBoxFillAlpha(20, 10, 30, 20, c,0);
	pgBoxFillAlpha(30, 10, 40, 20, c,8);
	pgBoxFillAlpha(40, 10, 50, 20, c,16);
	pgBoxFillAlpha(50, 10, 60, 20, c,24);
	pgBoxFillAlpha(60, 10, 70, 20, c,31);
	pgScreenFlipV();
	while(WaitInput()==0);
	c<<=1;
}
}
*/
	EraceDraw = N_FARAME;
	for(;;) {
			unsigned short	key;
			
			key = Read_Key(REPEAT_TIME);
			
			if ( key & (CTRL_UP|CTRL_DOWN|CTRL_LEFT|CTRL_RIGHT) )
			{
				if (key&CTRL_LEFT)	MoveDay(-1);
				if (key&CTRL_RIGHT)	MoveDay( 1);
				if (key&CTRL_UP)	MoveDay(-7);
				if (key&CTRL_DOWN)	MoveDay( 7);
				EraceDraw = N_FARAME;
			}
			else if ( key & CTRL_LTRIGGER)
			{
				MoveMon(-1);
				EraceDraw = N_FARAME;
			}
			else if ( key & CTRL_RTRIGGER)
			{
				MoveMon( 1);
				EraceDraw = N_FARAME;
			}
			else if ( key & CTRL_TRIANGLE )
			{
				cur_dtime = GetNowTime(&tim);
				SetCurrent(&tim);
				EraceDraw = N_FARAME;
			}
			else if ( key & CTRL_CIRCLE )
			{
				int	i;
				
				for (i=0; i<MSG_LINE; i++)
				{
					if (day_msg[cur_day-1][i][0])
					{
						ViewSchedule(cur_year+1900, cur_mon+1, cur_day);
						EraceDraw = N_FARAME;
						break;
					}
				}
			}

			if (EraceDraw)
			{
				MoveDay(0);
				DrawBack();
				DrawText();
				EraceDraw--;
			}
			DrawTime();
			pgScreenFlipV();
	}
	return(0);
}


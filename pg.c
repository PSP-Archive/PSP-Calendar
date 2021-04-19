// primitive graphics for Hello World PSP

#include "pg.h"
#include "font.c"

//system call
void pspDisplayWaitVblankStart();
void pspDisplaySetMode(long,long,long);
void pspDisplaySetFrameBuf(char *topaddr,long linesize,long pixelsize,long);


//constants

//480*272 = 60*38
#define CMAX_X 60
#define CMAX_Y 38
#define CMAX2_X 30
#define CMAX2_Y 19
#define CMAX4_X 15
#define CMAX4_Y 9


//variables
char *pg_vramtop=(char *)0x04000000;
long pg_screenmode;
long pg_showframe;
long pg_drawframe;



void pgWaitVn(unsigned long count)
{
	for (; count>0; --count) {
		pspDisplayWaitVblankStart();
	}
}


void pgWaitV()
{
	pspDisplayWaitVblankStart();
}


char *pgGetVramAddr(unsigned long x,unsigned long y)
{
	return pg_vramtop+(pg_drawframe?FRAMESIZE:0)+x*PIXELSIZE*2+y*LINESIZE*2+0x40000000;
}


void pgInit()
{
	pspDisplaySetMode(0,SCREEN_WIDTH,SCREEN_HEIGHT);
	pgScreenFrame(0,0);
}


void pgPrint(unsigned long x,unsigned long y,unsigned long color,const char *str)
{
	while (*str!=0 && x<CMAX_X && y<CMAX_Y) {
		pgPutChar(x*8,y*8,color,0,*str,1,0,1);
		str++;
		x++;
		if (x>=CMAX_X) {
			x=0;
			y++;
		}
	}
}

/*
void pgPrint2(unsigned long x,unsigned long y,unsigned long color,const char *str)
{
	while (*str!=0 && x<CMAX2_X && y<CMAX2_Y) {
		pgPutChar(x*16,y*16,color,0,*str,1,0,2);
		str++;
		x++;
		if (x>=CMAX2_X) {
			x=0;
			y++;
		}
	}
}


void pgPrint4(unsigned long x,unsigned long y,unsigned long color,const char *str)
{
	while (*str!=0 && x<CMAX4_X && y<CMAX4_Y) {
		pgPutChar(x*32,y*32,color,0,*str,1,0,4);
		str++;
		x++;
		if (x>=CMAX4_X) {
			x=0;
			y++;
		}
	}
}
*/

void pgFillvram(unsigned long color)
{
	unsigned char *vptr0;		//pointer to vram
	unsigned long i;

	vptr0=pgGetVramAddr(0,0);
	for (i=0; i<FRAMESIZE/2; i++) {
		*(unsigned short *)vptr0=color;
		vptr0+=PIXELSIZE*2;
	}
}

void pgBitBlt(unsigned long x,unsigned long y,unsigned long w,unsigned long h,unsigned long mag,const unsigned short *d)
{
	unsigned char *vptr0;		//pointer to vram
	unsigned char *vptr;		//pointer to vram
	unsigned long xx,yy,mx,my;
	const unsigned short *dd;
	
	vptr0=pgGetVramAddr(x,y);
	for (yy=0; yy<h; yy++) {
		for (my=0; my<mag; my++) {
			vptr=vptr0;
			dd=d;
			for (xx=0; xx<w; xx++) {
				for (mx=0; mx<mag; mx++) {
					*(unsigned short *)vptr=*dd;
					vptr+=PIXELSIZE*2;
				}
				dd++;
			}
			vptr0+=LINESIZE*2;
		}
		d+=w;
	}
	
}


void pgPutChar(unsigned long x,unsigned long y,unsigned long color,unsigned long bgcolor,unsigned char ch,char drawfg,char drawbg,char mag)
{
	unsigned char *vptr0;		//pointer to vram
	unsigned char *vptr;		//pointer to vram
	const unsigned char *cfont;		//pointer to font
	unsigned long cx,cy;
	unsigned long b;
	char mx,my;

	if (ch>255) return;
	cfont=font+ch*8;
	vptr0=pgGetVramAddr(x,y);
	for (cy=0; cy<8; cy++) {
		for (my=0; my<mag; my++) {
			vptr=vptr0;
			b=0x80;
			for (cx=0; cx<8; cx++) {
				for (mx=0; mx<mag; mx++) {
					if ((*cfont&b)!=0) {
						if (drawfg) *(unsigned short *)vptr=color;
					} else {
						if (drawbg) *(unsigned short *)vptr=bgcolor;
					}
					vptr+=PIXELSIZE*2;
				}
				b=b>>1;
			}
			vptr0+=LINESIZE*2;
		}
		cfont++;
	}
}


void pgScreenFrame(long mode,long frame)
{
	pg_screenmode=mode;
	frame=(frame?1:0);
	pg_showframe=frame;
	if (mode==0) {
		//screen off
		pg_drawframe=frame;
		pspDisplaySetFrameBuf(0,0,0,1);
	} else if (mode==1) {
		//show/draw same
		pg_drawframe=frame;
		pspDisplaySetFrameBuf(pg_vramtop+(pg_showframe?FRAMESIZE:0),LINESIZE,PIXELSIZE,1);
	} else if (mode==2) {
		//show/draw different
		pg_drawframe=(frame?0:1);
		pspDisplaySetFrameBuf(pg_vramtop+(pg_showframe?FRAMESIZE:0),LINESIZE,PIXELSIZE,1);
	}
}


void pgScreenFlip()
{
	pg_showframe=(pg_showframe?0:1);
	pg_drawframe=(pg_drawframe?0:1);
	pspDisplaySetFrameBuf(pg_vramtop+(pg_showframe?FRAMESIZE:0),LINESIZE,PIXELSIZE,0);
}


void pgScreenFlipV()
{
	pgWaitV();
	pgScreenFlip();
}

void print_hex(unsigned int val)
 {
 	char str[9];
 	int pos;
 	str[8] = 0;
 	pos = 7;
 	while(pos >= 0)
 	{
 		int nibble;
 		nibble = val & 0xF;
 		if(nibble < 0xA)
 		{
 			str[pos--] = nibble + '0';
 		}
 		else
 		{
 			str[pos--] = nibble + 'A' - 0xA;
 		}
 		val >>= 4;
 	}
		pgFillvram(0);
		pgPrint(1,7,0xffff,str);			
		pgScreenFlipV();
 }

unsigned long AddBlend(unsigned long c0, unsigned long c1)
{
   unsigned long c;
   c = (((c0 & c1) << 1) + ((c0 ^ c1) & 0xFEFEFE)) & 0x1010100;
   c = ((c >> 8) + 0x7f7f7f) ^ 0x7f7f7f;
   return (c0 + c1 - c) | c;
}

unsigned long SubBlend(unsigned long c0, unsigned long c1)
{
   unsigned long c;
   c = (((~c0 & c1) << 1) + ((~c0 ^ c1) & 0xFEFEFE)) & 0x1010100;
   c = (( c >> 8) + 0x7f7f7f) ^ 0x7f7f7f;
   return (c0 | c) - (c1 | c);
}

unsigned long P_INC_US_B(unsigned long x)
{
   unsigned long dwNum = ((~(x & ((x & 0x7f7f7f7f) + 0x01010101))) & 0x80808080) >> 7;
   return x + dwNum;
}
/*
unsigned long P_DEC_US_B(unsigned long x)
{
    unsigned long dwNum = ((x | ((x | 0x80808080) - 0x01010101)) & 0x80808080) >> 7;
    return x - dwNum;
}

unsigned long P_SLL_W(unsigned long x, unsigned long s)
{
    unsigned long m = 0xffff0000 ^ (0x0000ffff << s);
    return (x << s) & m;
}
*/
void pgLine(long x0, long y0, long x1, long y1, unsigned short col)
{
	unsigned short	*vptr0;		//pointer to vram
	long	E;
	long	x, y;
	long	dx, dy;
	long	sx, sy;
	long	i;

	sx = (x1 > x0) ? 1 : -1;
	sy = (y1 > y0) ? 1 : -1;
	dx = (x1 > x0) ? x1-x0 : x0-x1;
	dy = (y1 > y0) ? y1-y0 : y0-y1;

	x = x0;
	y = y0;

	vptr0 = (unsigned short*)pgGetVramAddr(x,y);

	if (dx >= dy)
	{
		E = -dx;
		for (i=0; i<=dx; i++)
		{
			*vptr0 = col;
			x += sx;
			E += dy;
			if( E >= 0 )
			{
				y += sy;
				E -= dx;
				vptr0 += (sy*512);
			}
			vptr0 += sx;
		}
	}
	else
	{
		E = -dy;
		for (i=0; i<=dy; i++)
		{
			*vptr0 = col;
			y += sy;
			E += dx;
			if (E >= 0)
			{
		    	x += sx;
		    	E -= dy;
				vptr0 += sx;
			}
			vptr0 += (sy*LINESIZE);
		}
	}
}

void pgBox(long x0, long y0, long x1, long y1, unsigned short col)
{
	pgLine(x0,y0,x1,y0,col);
	pgLine(x0,y1,x1,y1,col);
	pgLine(x0,y0,x0,y1,col);
	pgLine(x1,y0,x1,y1,col);
}

void pgBoxFill(long x0, long y0, long x1, long y1, unsigned short col)
{
	unsigned short *vptr0;		//pointer to vram
	unsigned short *vptr_back;	//pointer to vram
	unsigned long i;
	long	x, y;
	long	dx, dy;
	long	sx, sy;

	sx = (x1 > x0) ? 1 : -1;
	sy = (y1 > y0) ? 1 : -1;
	dx = (x1 > x0) ? x1-x0 : x0-x1;
	dy = (y1 > y0) ? y1-y0 : y0-y1;

	vptr0=(unsigned short*)pgGetVramAddr(x0,y0);
	for (y=0; y<=dy; y+=sy)
	{
		vptr_back = vptr0;
		for (x=0; x<=dx; x+=sx)
		{
			*vptr0 = col;
			vptr0++;
		}
		vptr0 = vptr_back + LINESIZE;
	}
}

#define	RGBx(r,g,b)	((((b)&0x1F)<<10)+(((g)&0x1F)<<5)+((r)&0x1F)+0x8000)
extern	unsigned short	BackBmp[];
void pgBoxFillAlpha(long x0, long y0, long x1, long y1, unsigned short col, unsigned short alpha)
{
	unsigned short *vptr0;		//pointer to vram
	unsigned short *vptr_back;	//pointer to vram
	unsigned long i;
	long	x, y;
	long	dx, dy;
	long	sx, sy;

	sx = (x1 > x0) ? 1 : -1;
	sy = (y1 > y0) ? 1 : -1;
	dx = (x1 > x0) ? x1-x0 : x0-x1;
	dy = (y1 > y0) ? y1-y0 : y0-y1;

	vptr0=(unsigned short*)pgGetVramAddr(x0,y0);
	for (y=0; y<=dy; y+=sy)
	{
		vptr_back = vptr0;
		for (x=0; x<=dx; x+=sx)
		{
			unsigned short	w1,w2;
			unsigned short	R1,R2;
			unsigned short	G1,G2;
			unsigned short	B1,B2;
			
//			w1 = (unsigned short)SubBlend(*vptr0, alpha);
//w1=(*vptr0&alpha);
//			w2 = (unsigned short)SubBlend(col   , alpha^0xFFFF);
//w2=(col&(alpha^0xFFFF));
//			*vptr0 = (unsigned short)AddBlend(w1, w2);
R1=((BackBmp[(x0+x)+(y0+y)*480])    )&0x1F;
G1=((BackBmp[(x0+x)+(y0+y)*480])>> 5)&0x1F;
B1=((BackBmp[(x0+x)+(y0+y)*480])>>10)&0x1F;

R1=BackBmp[(x0+x)+(y0+y)*480]&0x7FFF;	R1 >>=0;  R1 &= 0x1F;
G1=BackBmp[(x0+x)+(y0+y)*480]&0x7FFF;	G1 >>=5;  G1 &= 0x1F;
B1=BackBmp[(x0+x)+(y0+y)*480]&0x7FFF;	B1 >>=10; B1 &= 0x1F;

alpha &= 100;
R1 = (R1 * (100-alpha)) / 100;
G1 = (G1 * (100-alpha)) / 100;
B1 = (B1 * (100-alpha)) / 100;

R2=((col)>> 0)&0x1F;
G2=((col)>> 5)&0x1F;
B2=((col)>>10)&0x1F;

R2 = (R2 * (alpha)) / 100;
G2 = (G2 * (alpha)) / 100;
B2 = (B2 * (alpha)) / 100;
/*
alpha = 0;
R1 = (R1*(31-alpha))/31;
G1 = (G1*(31-alpha))/31;
B1 = (B1*(31-alpha))/31;
*/
//R1=G1=B1=0;
/*
R2 = (R2*alpha)/31;
G2 = (G2*alpha)/31;
B2 = (B2*alpha)/31;
*/
//R1=0;G1=0;B1=0;
//R2=0;G2=0;B2=0;
{
unsigned short c;
unsigned short Rx,Gx,Bx;
Rx = (R1)+(R2);
Gx = (G1)+(G2);
Bx = (B1)+(B2);
c = (Bx<<10)|(Gx<<5)|(Rx<<0)|0x8000;
*vptr0 =c;
}
//*vptr0 =BackBmp[(x0+x)+(y0+y)*480];
			vptr0++;
		}
		vptr0 = vptr_back + LINESIZE;
	}
}


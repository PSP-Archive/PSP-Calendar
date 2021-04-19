// primitive graphics for Hello World PSP

void pgInit();
void pgWaitV();
void pgWaitVn(unsigned long count);
void pgScreenFrame(long mode,long frame);
void pgScreenFlip();
void pgScreenFlipV();

void pgPrint(unsigned long x,unsigned long y,unsigned long color,const char *str);
/*
void pgPrint2(unsigned long x,unsigned long y,unsigned long color,const char *str);
void pgPrint4(unsigned long x,unsigned long y,unsigned long color,const char *str);
*/
void pgFillvram(unsigned long color);
void pgBitBlt(unsigned long x,unsigned long y,unsigned long w,unsigned long h,unsigned long mag,const unsigned short *d);
void pgPutChar(unsigned long x,unsigned long y,unsigned long color,unsigned long bgcolor,unsigned char ch,char drawfg,char drawbg,char mag);
void print_hex(unsigned int val);
void pgLine(long x0, long y0, long x1, long y1, unsigned short col);
void pgBox(long x0, long y0, long x1, long y1, unsigned short col);
void pgBoxFill(long x0, long y0, long x1, long y1, unsigned short col);
void pgBoxFillAlpha(long x0, long y0, long x1, long y1, unsigned short col, unsigned short alpha);

#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 272

#define		PIXELSIZE	1				//in short
#define		LINESIZE	512				//in short
#define		FRAMESIZE	0x44000			//in byte

#include "japanese.c"

unsigned short num2elisa(unsigned short c);

void Draw_Char_Hankaku(int x,int y,unsigned char ch,int col,int backcol,int fill);

void Draw_Char_Zenkaku(int x,int y,unsigned char u,unsigned char d,int col,int backcol,int fill);

void print_japanese(int x,int y,unsigned char *str,int col,int backcol,int fill);

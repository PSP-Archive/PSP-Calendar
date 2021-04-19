#ifndef FILE_H
#define FILE_H

#include "types.h"

#define	MAX_PATH	256

enum { 
    TYPE_DIR=0x10, 
    TYPE_FILE=0x20 
}; 

#define O_RDONLY    0x0001 
#define O_WRONLY    0x0002 
#define O_RDWR      0x0003 
#define O_NBLOCK    0x0010 
#define O_APPEND    0x0100 
#define O_CREAT     0x0200 
#define O_TRUNC     0x0400 
#define O_NOWAIT    0x8000 


typedef struct dirent { 
    unsigned long unk0; 
    unsigned long type; 
    unsigned long size; 
    unsigned long unk[19]; 
    char name[0x108]; 
	unsigned char dmy[128];
} dirent_t;

int  sceIoOpen(const char* file, int mode, int opt); 
void sceIoClose(int fd); 
int  sceIoRead(int fd, void *data, int size); 
int  sceIoWrite(int fd, void *data, int size); 
int  sceIoLseek(int fd, int offset, int whence); 
int  sceIoRemove(const char *file); 
int  sceIoMkdir(const char *dir, int mode); 
int  sceIoRmdir(const char *dir); 
int  sceIoRename(const char *oldname, const char *newname); 
//int sceIoDevctl(const char *name int cmd, void *arg, size_t arglen, void *buf, size_t *buflen); 
int  sceIoDopen(const char *fn); 
int  sceIoDread(int fd, dirent_t *de); 
void sceIoDclose(int fd); 

#define	fopen(file, mode)			sceIoOpen(file, mode[0]=='r'?O_RDONLY:O_CREAT|O_WRONLY, 0777)
#define	fclose(fd)					sceIoClose(fd)
#define	fread(data, size, x, fd)	sceIoRead(fd, data, size)
#define	fwrite(data, size, x, fd)	sceIoWrite(fd, data, size)
#define	fseek(fd, offset, whence)	sceIoLseek(fd, offset, whence)

#endif

#include "types.h"

int memcmp(const void *buf1, const void *buf2,size_t n)
{
	int ret;
	size_t i;
	
	for(i=0; i<n; i++){
		ret = ((unsigned char*)buf1)[i] - ((unsigned char*)buf2)[i];
		if(ret!=0)
			return ret;
	}
	return 0;
}

void* memcpy(void *buf1, const void *buf2, size_t n)
{
	while(n-->0)
		((unsigned char*)buf1)[n] = ((unsigned char*)buf2)[n];
	return buf1;
}

void* memset(void *buf, int ch, size_t n)
{
	unsigned char *p = buf;
	
	while(n>0)
		p[--n] = ch;
	
	return buf;
}

size_t strlen(const char *s)
{
	size_t ret;
	
	for(ret=0; s[ret]; ret++)
		;
	
	return ret;
}

char* strcpy(char *dest, const char *src)
{
	int i;
	
	for(i=0; src[i]; i++)
		dest[i] = src[i];
	dest[i] = 0;
	
	return dest;
}

char* strchr(const char *src, int c)
{
	while(*src){
		if(*src == c)
			return ((char*)src);
		src++;
	}
	
	return NULL;
}

char* strrchr(const char *src, int c)
{
	size_t len;
	
	len=strlen(src);
	while(len>0){
		len--;
		if(*(src+len) == c)
			return (char*)(src+len);
	}
	
	return NULL;
}

char* strcat(char *dest, const char *src)
{
	int i;
	size_t len;
	
	len=strlen(dest);
	for(i=0; src[i]; i++)
		dest[len+i] = src[i];
	dest[len+i] = 0;
	
	return dest;
}

int strcmp(const char *str1, const char *str2)
{
	char c1, c2;
	for(;;){
		c1 = *str1;
		c2 = *str2;
		
		if(c1!=c2)
			return 1;
		else if(c1==0)
			return 0;
		
		str1++; str2++;
	}
}

int stricmp(const char *str1, const char *str2)
{
	char c1, c2;
	for(;;){
		c1 = *str1;
		if(c1>=0x61 && c1<=0x7A) c1-=0x20;
		c2 = *str2;
		if(c2>=0x61 && c2<=0x7A) c2-=0x20;
		
		if(c1!=c2)
			return 1;
		else if(c1==0)
			return 0;
		
		str1++; str2++;
	}
}

int atoi(char *c)
{
	int	ret = 0;
	int	m = 1;
	
	while(*c==' ')	c++;
	
	if (*c=='-')
	{
		m = -1;
		c++;
	}
	if (*c=='+')
		c++;
	
	while((*c>='0')&&(*c<='9'))
	{
		ret*=10;
		ret+=(*c-'0');
		c++;
	}
	
	return(ret*m);
}

int itoan(int num, char *c, int keta)
{
	int	ret;
	int	i;
	int	n = num;
	
	ret = keta;
	if (n < 0)
	{
		*c = '-';
		ret++;
	}
	
	for(i=0;i<keta;i++)
	{
		c[keta-1-i] = '0'+(n%10);
		n /= 10;
	}
	c[keta] = 0x00;
	
	return(ret);
}

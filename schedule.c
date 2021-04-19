#include "pg.h"
#include "file.h"

#define	TEXT_MAX_LEN	52
char	MonText[31][3][TEXT_MAX_LEN];


void dec2asc(int n, char *c, int len)
{
	int	i;

	c[len] = 0x00;
	for (i=len-1; i>=0; i--)
	{
		if (n)
		{
			c[i] = '0'+n%10;
			n /= 10;
		}
		else
		{
			c[i] = '0'+n%10;
		}
	}
}

void ScheduleRead(unsigned short year, unsigned short month, int nmax)
{
	int	i;
	
	for (i=0; i<nmax; i++)
	{
//		strcpy(MonText[i][1], "----+----1----+----2----+----3----+----4----+----51");
//		strcpy(MonText[i][1], "‚Ä‚·‚Æ•¶Žš—ñ");
//		strcpy(MonText[i][2], "ABCDEFG");
	}
}


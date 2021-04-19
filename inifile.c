#include "types.h"
#include "string.h"
#include "inifile.h"

int getline(char *src, char *dst, int maxlen)
{
	int		i;

	for (i=0; i<maxlen; i++)
	{
		if (src[i] == 0x00)
			break;

		if (((unsigned char)src[i] >= 0x81)&&((unsigned char)src[i] <= 0x9F)||((unsigned char)src[i] >= 0xE0)&&((unsigned char)src[i] <= 0xFC))		/* 漢字か？ */
		{
			if (i>=maxlen-1)
				break;

			*dst = src[i++];	/* 2バイトコピー */
			dst++;
			*dst = src[i];
			dst++;
			continue;
		}

		if (src[i] == '\r')		// for DOS / Mac
		{
			break;
		}
		if (src[i] == '\n')		// for UNIX
		{
			break;
		}

		*dst = src[i];
		dst++;
	}

	*dst = 0x00;

	if (src[i] == '\r')		// for DOS / Mac
	{
		i++;
		if (src[i] == '\n')
			i++;
	}
	if (src[i] == '\n')		// for UNIX
	{
		i++;
	}

	return(i);
}


int inifile(char *file, INIFILECALLBACK *func)
{
	int		ix;
	int		l;
	char	lpszSec[32];
	char	lpszKey[256];
	char	*b,*p;

	lpszSec[0] = 0x00;
	ix = 0;
	while((l=getline(&file[ix], lpszKey, sizeof(lpszKey))))
	{
		if ((lpszKey[0] != ';')&&(lpszKey[0] != '#'))
		{
			if (lpszKey[0] == '[')
			{
				p = strchr(lpszKey, ']');
				if (p!=NULL)
				{
					*p = 0x00;
					strcpy(lpszSec, &lpszKey[1]);
				}
			}
			else
			{
				b = p = strchr(lpszKey, '=');
				if (p)
				{
					b--;
					while(*b==' '||*b=='\t')	b--;
					*(b+1) = 0x00;

					p++;
					if (*p == '\"')
					{
						char	*x;
						
						p++;
						x = strrchr(p, '\"');
						if (x)	*x = 0x00;
					}
					else
					{
						while((*p)&&(*p==' '||*p=='\t'))	p++;
					}
					func(lpszSec,lpszKey,p);
				}
			}
		}

		ix += l;
	}

	return 0;
}

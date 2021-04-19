#ifndef	_INIFILE_H
#define	_INIFILE_H

typedef void (INIFILECALLBACK)(char*,char*,char*);

int getline(char *src, char *dst, int maxlen);
int inifile(char *file, INIFILECALLBACK *func);

#endif	/* _INIFILE_H */

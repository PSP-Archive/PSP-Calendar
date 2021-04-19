#ifndef	RESOURCE_H
#define	RESOURCE_H

#define	RES_ENGLISH
//#define	RES_JAPANESE

#define	RES_WEEKS_EN	{\
"SUN",\
"MON",\
"TUE",\
"WED",\
"THU",\
"FRI",\
"SAT"\
}

#define	RES_WEEKS_JP	{\
"ì˙",\
"åé",\
"âŒ",\
"êÖ",\
"ñÿ",\
"ã‡",\
"ìy"\
}

#define	RES_DATE_WEEKS_JP	{\
"(ì˙)",\
"(åé)",\
"(âŒ)",\
"(êÖ)",\
"(ñÿ)",\
"(ã‡)",\
"(ìy)"\
}

#ifdef	RES_ENGLISH
	#define	RES_WEEKS		RES_WEEKS_EN
	#define	RES_DATE_WEEKS	RES_WEEKS_EN
#endif

#ifdef	RES_JAPANESE
	#define	RES_WEEKS	RES_WEEKS_JP
	#define	RES_DATE_WEEKS	RES_DATE_WEEKS_JP
#endif


#endif

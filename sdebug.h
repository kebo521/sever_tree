#ifndef _S_DEBUG_
#define _S_DEBUG_

typedef enum
{
	ZEROPAD	=0x01,	/* pad with zero */
	SIGN	=0x02,	/* unsigned/signed long */
	PLUS	=0x04,	/* show plus */
	SPACE	=0x08,	/* space if plus */
	LEFT	=0x10,	/* left justified */
	SPECIAL	=0x20,	/* 0x */
	LARGE	=0x40,	/* use 'ABCDEF' instead of 'abcdef' */
	ALIGN	=0x80,	/* align the space */
}EXP_DEF_STRING_TYPE;


typedef char* va_list;
#define _va_rounded_size(TYPE) ((sizeof(TYPE)+sizeof(int)-1)&(~(sizeof(int)-1)))
#define _va_start(AP, LASTARG)   (AP = ((char *)& (LASTARG) + _va_rounded_size(LASTARG)))
#define _va_arg(AP, TYPE)        (AP += _va_rounded_size(TYPE), *((TYPE *)(AP - _va_rounded_size(TYPE))))
#define _va_end(AP)              (AP = (va_list)0 )

extern int _vsprintf(char *buf, const char *fmt, va_list args);
extern int gBcdtoStr_n(u8 *pBcd, int len,char *pOut,int outSize);

extern void gTrace(const char *format,...);
extern void gTraceHex(char *pTitle,u8 *pBcd, int len);

#define TRACE(...) 				gTrace(__VA_ARGS__)
#define TRACE_HEX(Msg,buf,len) 	gTraceHex(Msg,buf,len)

#endif

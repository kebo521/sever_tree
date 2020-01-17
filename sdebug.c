/************************************************************ 
 调试信息
 ************************************************************/
#include "comm.h"


#if(0)

#define abs(a)  ((a) < 0 ?  -(a) :(a))
#define is_digit(c) ((c) >= '0' && (c) <= '9')
/////////////////////////////////////////////////////////////////////////////

static char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
static char *upper_digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/*
static unsigned long strnlen(const char *s, int count)
{
    const char *sc;
    for (sc = s; *sc != '\0' && count--; ++sc);
    return sc - s;
}
*/
static int skip_atoi(const char **s)
{
    int i = 0;
    while (is_digit(**s))
    {
        i = i*10 + *((*s)++) - '0';
    }
    return i;
}

static char * number(char *str, long num, int base, int size, int precision, int type)
{
    char c, sign, tmp[66];
    char *dig = digits;
    int i;

    if (type & LARGE)  dig = upper_digits;
    if (type & LEFT) type &= ~ZEROPAD;
    if (base < 2 || base > 36) return 0;

    c = (type & ZEROPAD) ? '0' : ' ';
    sign = 0;
    if (type & SIGN)
    {
        if (num < 0)
        {
            sign = '-';
            num = -num;
            size--;
        }
        else if (type & PLUS)
        {
            sign = '+';
            size--;
        }
        else if (type & SPACE)
        {
            sign = ' ';
            size--;
        }
    }

    if (type & SPECIAL)
    {
        if (16 == base)
            size -= 2;
        else if (8 == base)
            size--;
    }

    i = 0;

    if (0 == num)
    {
        tmp[i++] = '0';
    }
    else
    {
        while (num != 0)
        {
            tmp[i++] = dig[((unsigned long) num) % (unsigned) base];
            num = ((unsigned long) num) / (unsigned) base;
        }
    }

    if (i > precision) precision = i;
    size -= precision;
    if (!(type & (ZEROPAD | LEFT)))
    {
        while(size-- > 0) *str++ = ' ';
    }
    if (sign) *str++ = sign;

    if (type & SPECIAL)
    {
        if (8 == base)
        {
            *str++ = '0';
        }
        else if (16 == base)
        {
            *str++ = '0';
            *str++ = digits[33];
        }
    }

    if(!(type & LEFT))
    {
        while(size-- > 0) *str++ = c;
    }
    while(i < precision--) *str++ = '0';
    while(i-- > 0) *str++ = tmp[i];
    while(size-- > 0) *str++ = ' ';

    return str;
}

static char * eaddr(char *str, unsigned char *addr, int size, int precision, int type)
{
    char tmp[24];
    char *dig = digits;
    int i,len = 0;
    if (type & LARGE)  dig = upper_digits;
    for(i = 0; i < 6; i++)
    {
        if (i != 0) tmp[len++] = ':';
        tmp[len++] = dig[addr[i] >> 4];
        tmp[len++] = dig[addr[i] & 0x0F];
    }

    if (!(type & LEFT))
    {
        while (len < size--) *str++ = ' ';
    }
    
    for (i = 0; i < len; ++i)
    {
        *str++ = tmp[i];
    }
    
    while (len < size--) *str++ = ' ';

    return str;
}

static char * iaddr(char *str, unsigned char *addr, int size, int precision, int type)
{
    char tmp[24];
    int i,len = 0;
    for (i = 0; i < 4; i++)
    {
        int n = addr[i];
        if (i != 0) tmp[len++] = '.';
        
        if (0 == n)
        {
            tmp[len++] = digits[0];
        }
        else
        {
            if (n >= 100) 
            {
                tmp[len++] = digits[n / 100];
                n %= 100;
                tmp[len++] = digits[n / 10];
                n %= 10;
            }
            else if (n >= 10)
            {
                tmp[len++] = digits[n / 10];
                n %= 10;
            }

            tmp[len++] = digits[n];
        }
    }

    if (!(type & LEFT))
    {
        while(len < size--) *str++ = ' ';
    }
    
    for (i = 0; i < len; ++i)
    {
        *str++ = tmp[i];
    }
    
    while (len < size--) *str++ = ' ';
    
    return str;
}

int _vsprintf(char *buf, const char *fmt, _va_list args)
{
    char *str;
    int field_width;      /* Width of output field */

    for(str = buf; *fmt; fmt++)
    {
        long num;
        int base = 10;
        int flags = 0; /* Flags to number()    Process flags */
        int qualifier = -1;        /* 'h', 'l', or 'L' for integer fields */
        int precision = -1;     /* Min. # of digits for integers; max number of chars for from string */
        BOOL bFmt = TRUE;
        if (*fmt != '%')
        {
            *str++ = *fmt;
            continue;
        }
        
        bFmt = TRUE;
        while(bFmt)
        {
            fmt++; /* This also skips first '%' */
            switch (*fmt)
            {
                case '-': flags |= LEFT; break;
                case '+': flags |= PLUS; break;
                case ' ': flags |= SPACE; break;
                case '#': flags |= SPECIAL; break;
                case '0': flags |= ZEROPAD; break;
                default:  bFmt = FALSE;
            }
        }
        
        /* Get field width */
        field_width = -1;
        if (is_digit(*fmt))
        {
            field_width = skip_atoi(&fmt);
        }
        else if ('*' == *fmt)
        {
            fmt++;
            field_width = va_arg(args, int);
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* Get the precision */
        precision = -1;
        if ('.' == *fmt)
        {
            ++fmt;
            if (is_digit(*fmt))
            {
                precision = skip_atoi(&fmt);
            }
            else if ('*' == *fmt)
            {
                ++fmt;
                precision = va_arg(args, int);
            }
            if (precision < 0) precision = 0;
        }

        /* Get the conversion qualifier */
        qualifier = -1;
        if ('h' == *fmt || 'l' == *fmt || 'L' == *fmt)
        {
            qualifier = *fmt;
            fmt++;
        }

        /* Default base */
        base = 10;
        switch (*fmt)
        {
            case 'c':
            {
                if (!(flags & LEFT)) while (--field_width > 0) *str++ = ' ';
                *str++ = (unsigned char) va_arg(args, int);
                while (--field_width > 0) *str++ = ' ';
                continue;
            }
            case 's':
            {
                int i,len;
                char * s = va_arg(args, char *);
                if (!s) s = "<NULL>";
                len = strnlen(s, precision);
                if (!(flags & LEFT)) while (len < field_width--) *str++ = ' ';
                for (i = 0; i < len; ++i) *str++ = *s++;
                while (len < field_width--) *str++ = ' ';
                continue;
            }
			case 'B':	//char*,int
            {
                int i,len;
				u8* pBCD = va_arg(args, u8*);
				len=	va_arg(args, int);
                if (!pBCD) len	=0;
				for(i=0;i<len;i++)
				{
					*str++ = upper_digits[pBCD[i]>>4];
					*str++ = upper_digits[pBCD[i]&0x0f];
					*str++ = ' ';
				}
				if(i) str--;	//去掉最后一个空格。
                continue;
            }
            case 'p':
            {
                if (-1 == field_width)
                {
                    field_width = 2 * sizeof(void *);
                    flags |= ZEROPAD;
                }
                str = number(str, (unsigned long) va_arg(args, void *), 16, field_width, precision, flags);
                continue;
            }
            case 'n':
            {
                if ('l' == qualifier)
                {
                    long *ip = va_arg(args, long *);
                    *ip = (str - buf);
                }
                else
                {
                    int *ip = va_arg(args, int *);
                    *ip = (str - buf);
                }
                continue;
            }
            case 'A':
            {
                flags |= LARGE; /* no break */
            }
            case 'a':
            {
                if ('l' == qualifier)
                {
                    str = eaddr(str, va_arg(args, unsigned char *), field_width, precision, flags);
                }
                else
                {
                    str = iaddr(str, va_arg(args, unsigned char *), field_width, precision, flags);
                }
                continue;
            }
                /* Integer number formats - set up the flags and "break" */
            case 'o':
            {
                base = 8;
                break;
            }
            case 'X':
            {
                flags |= LARGE; /* no break */
            }
            case 'x':
            {
                base = 16;
                break;
            }
            case 'd':
            case 'i':
            {
                flags |= SIGN; /* no break */
            }
            case 'u':
            {
                break;
            }
            default:
            {
                if (*fmt != '%') *str++ = '%';
                if (*fmt)
                {
                    *str++ = *fmt;
                }
                else
                {
                    --fmt;
                }
                continue;
            }
        }  /* end of switch (*fmt) */

        if (qualifier == 'l')
        {
            num = _va_arg(args, unsigned long);
        }
        else if (qualifier == 'h')
        {
            if (flags & SIGN)
                num = _va_arg(args, short);
            else
                num = _va_arg(args, unsigned short);
        }
        else if (flags & SIGN)
        {
            num = (unsigned long)_va_arg(args, int);
        }
        else
        {
            num = (unsigned long)_va_arg(args, unsigned long);
        }
        
        str = number(str, num, base, field_width, precision, flags);
    } /* end of for (str = buf; *fmt; fmt++) */

    *str = '\0';
    return str - buf;
}
#endif

void gTrace(const char *format,...)
{	
#if(1)
	va_list arg;	
	char str[4096];
//	SYSTEMTIME systm;
//	GetLocalTime(&systm);
	//sprintf_s(sMsec,"%08d:",systm.wMilliseconds); 
	va_start(arg, format );
	memcpy(str,"TRACE->>",8);
	vsprintf(str+8, format,arg);
	va_end(arg);
	puts(str);
#else
	va_list arg;
	va_start(arg, format);
	vprintf(format,arg);
	va_end(arg);	
	puts(" ");
#endif
}

int gBcdtoStr_n(u8 *pBcd, int len,char *pOut,int outSize)
{
	const char sHexData[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	int i,nMax;
	char *pSave,*pEnd;
	pSave = pOut;
	pEnd = pOut+outSize;
	nMax = 32;
	while(len)
	{
		if(nMax > len) nMax=len;
		if((pSave+(3*nMax + 2)) >= pEnd) break;
		for(i=0;i<nMax;i++)
		{
			*pSave++ = sHexData[*pBcd >> 4];
			*pSave++ = sHexData[*pBcd & 0x0f];
			*pSave++ = ' ';
			pBcd++;
		}
		*pSave++ = '\r';
		*pSave++ = '\n';
		len -= nMax;
	}
	*pSave = '\0';
	return pSave-pOut;
}

//=================================================================
void gTraceHex(char *pTitle,u8 *pBcd, int len)
{
/*
	int i;
	if(pBcd == NULL )
	{
		printf("TRACE_HEX-->> %s:Data=NULL\r\n",pTitle);
		return;
	}
	printf("TRACE_HEX-->> %s[%d]:\r\n",pTitle,len);
	for(i=0;i<len;i++)
	{
		printf("%02X ",pBcd[i]);
		if((i%40)==39)
		{
			printf("\r\n");
		}
	}
	if(i%40)
	{
		printf("\r\n");
	}
*/
	char sbuff[2048];
	int len1,len2;
	if(pBcd == NULL )
	{
		printf("TRACE_HEX-->> %s:Data=NULL\r\n",pTitle);
		return;
	}
	len1 = sprintf(sbuff,"%s[%d]:\r\n",pTitle,len);
	len2 = gBcdtoStr_n(pBcd,len,sbuff+len1,sizeof(sbuff)-len1);
	//*pSave = '\0'; fputs(sbuff, stdout);
	fwrite(sbuff,1,len1+len2,stdout);
}


int gLog(char* sbuff,int size,const char *format,...)
{	
	int ret;
	va_list arg;
	va_start(arg, format);
	ret=vsnprintf(sbuff,size,format,arg);
	va_end(arg);
	return ret;
}

int gLogHex(char* sbuff,int size,char *pTitle,u8 *pBcd, int len)
{
	int len1,len2;
	if(pBcd == NULL )
	{
		return snprintf(sbuff,size,"TRACE_HEX-->> %s:Data[%d]=NULL\r\n",pTitle,len);
	}
	len1 = sprintf(sbuff,"%s[%d]:",pTitle,len);
	if(len > 20)
	{
		sbuff[len1++]='\r';
		sbuff[len1++]='\n';
	}
	len2 = gBcdtoStr_n(pBcd,len,sbuff+len1,size-len1);
	return (len1+len2);
}




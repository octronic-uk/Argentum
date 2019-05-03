#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int vsprintf(char *s0, const char *fmt, va_list args)
{
	char Num[80];
	char *s;

	for (s=s0;*fmt;++fmt)
	{
		if (fmt[0]=='%' && fmt[1])
		{
			const char *str;
			bool Left = 0;
			bool Sign = 0;
			bool Large = 0;
			bool ZeroPad = 0;
			int Width = -1;
			int Type = -1;
			int Base = 10;
			char ch,cs;
			int Len;
			long n;

			for (;;)
			{
				switch (*(++fmt)) 
				{
				case '-': Left = 1; continue;
				case '0': ZeroPad = 1; continue;
				default: break;
				}
				break;
			}

			if (*fmt>='0' && *fmt<='9')
			{
				Width = 0;
				for (;*fmt>='0' && *fmt<='9';++fmt)
					Width = Width*10 + (*fmt-'0');
			}
			else
			if (*fmt == '*')
			{
				++fmt;
				Width = va_arg(args, int);
				if (Width < 0)
				{
					Left = 1;
					Width = -Width;
				}
			}

			if (*fmt == 'h' || 
				*fmt == 'L' ||
				*fmt == 'l')
				Type = *(fmt++);

			switch (*fmt)
			{
			case 'c':
				for (;!Left && Width>1;--Width)
					*(s++) = ' ';
				*(s++) = (char)va_arg(args,int);
				for (;Width>1;--Width)
					*(s++) = ' ';
				continue;
			case 's':
				str = va_arg(args,const char*);
				if (!s)
					str = "<NULL>";
				Len = strlen(str);
				for (;!Left && Width>Len;--Width)
					*(s++) = ' ';
				for (;Len>0;--Len,--Width)
					*(s++) = *(str++);
				for (;Width>0;--Width)
					*(s++) = ' ';
				continue;
			case 'o':
				Base = 8;
				break;
			case 'X':
				Large = 1;
			case 'x':
				Base = 16;
				break;
			case 'i':
			case 'd':
				Sign = 1;
			case 'u':
				break;
			default:
				if (*fmt != '%')
					*(s++) = '%';
				*(s++) = *fmt;
				continue;
			}

			if (Type == 'l')
				n = va_arg(args,unsigned long);
			else
			if (Type == 'h')
				if (Sign)
					n = (short)va_arg(args,int);
				else
					n = (unsigned short)va_arg(args,unsigned int);
			else
			if (Sign)
				n = va_arg(args,int);
			else
				n= va_arg(args,unsigned int);

			if (Left)
				ZeroPad = 0;
			if (Large)
				str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
			else
				str = "0123456789abcdefghijklmnopqrstuvwxyz";

			ch = ' ';
			if (ZeroPad)
				ch = '0';
			cs = 0;

			if (n<0 && Sign)
			{
				cs = '-';
				n=-n;
			}

			Len = 0;
			if (n==0)
				Num[Len++] = '0';
			else
			{
				unsigned long un = n;
				while (un != 0)
				{
					Num[Len++] = str[un%Base];
					un /= Base;
				}
			}

			if (cs)
				++Len;

			for (;!Left && Width>Len;--Width)
				*(s++) = ch;

			if (cs)
			{
				*(s++) = cs;
				--Len;
				--Width;
			}

			for (;Len;--Width)
				*(s++) = Num[--Len];

			for (;Width>0;--Width)
				*(s++) = ' ';
		}
		else
			*(s++) = *fmt;
	}
	*(s++) = 0;
	return s-s0;
}
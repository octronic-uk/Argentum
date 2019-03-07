#include "Utilities.h"

// inline function to swap two numbers
void swap(char *x, char *y) 
{
	char t = *x; 
	*x = *y; 
	*y = t;
}

// function to reverse buffer[i..j]
void reverse(char *buffer, int i, int j)
{
	while (i < j)
	{
		swap(&buffer[i++], &buffer[j--]);
	}
}

// Iterative function to implement itoa() function in C
void itoa(int value, char* buffer, int base)
{
	// invalid input
	if (base < 2 || base > 32)
  {
		return;
  }

	// consider absolute value of number
	int n = value < 0 ? -value : value;

	int i = 0;
	while (n)
	{
		int r = n % base;

		if (r >= 10) 
		{
			buffer[i++] = 65 + (r - 10);
		}
		else
		{
			buffer[i++] = 48 + r;
		}
		n = n / base;
	}

	// if number is 0
	if (i == 0)
	{
		buffer[i++] = '0';
	}

	// If base is 10 and value is negative, the resulting string 
	// is preceded with a minus sign (-)
	// With any other base, value is always considered unsigned
	if (value < 0 && base == 10)
  {
		buffer[i++] = '-';
  }


	buffer[i] = '\0'; // null terminate string

	// reverse the string and return it
	reverse(buffer, 0, i - 1);
}

// Iterative function to implement itoa() function in C
void ultoa(unsigned long value, char* buffer, int base)
{
	// invalid input
	if (base < 2 || base > 32)
  {
		return;
  }

	// consider absolute value of number
	unsigned long n = value < 0 ? -value : value;

	int i = 0;
	while (n)
	{
		unsigned long r = n % base;

		if (r >= 10) 
		{
			buffer[i++] = 65 + (r - 10);
		}
		else
		{
			buffer[i++] = 48 + r;
		}

		n = n / base;
	}

	// if number is 0
	if (i == 0)
	{
		buffer[i++] = '0';
	}

	// If base is 10 and value is negative, the resulting string 
	// is preceded with a minus sign (-)
	// With any other base, value is always considered unsigned
	if (value < 0 && base == 10)
  {
		buffer[i++] = '-';
  }


	buffer[i] = '\0'; // null terminate string

	// reverse the string and return it
	reverse(buffer, 0, i - 1);
}

void* memset( void* dst, int ch, unsigned long count )
{
	int i;
	char* dest = (char*)dst;
	for (i=0; i<count; i++) 
	{
		dest[i] = ch; 
	}
	return dst;
}

unsigned long strlen(const char* str)
{
	unsigned long i;
	for (i=0;str[i] != 0; i++) {}
	return i;
}

int memcpy(void* dst, void* src, int size)
{
	unsigned long i;
	for (i=0; i<size; i++)
	{
		char* nextDst = (char*)dst+i;
		char* nextSrc = (char*)src+i;
		*nextDst = *nextSrc;
	}
	return size;
}
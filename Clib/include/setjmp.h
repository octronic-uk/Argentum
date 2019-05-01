#pragma once

/*
	http://vmresu.me/blog/2016/02/09/lets-understand-setjmp-slash-longjmp/
*/

typedef unsigned long __jmp_buf[6];
typedef struct __jmp_buf_tag 
{
    __jmp_buf __jb;
    unsigned long __fl;
    unsigned long __ss[128/sizeof(long)];
} jmp_buf[1];

extern int setjmp(jmp_buf env);
extern void longjmp(jmp_buf env, int val);


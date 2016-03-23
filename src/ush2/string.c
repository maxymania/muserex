/*
 * Copyright (c) 2016 Simon Schmidt
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ush.h"

typedef char* chrp;

#define caseof(v,c) case v:c;break;
#define casedef(c) default:c;break;

#define nilf(x) if(!(x)) return NULL

string_t shs_cat_(string_t first,...){
	size_t dest = 1;
	string_t elem;
	chrp c;
	va_list ap;
	va_start(ap,first);
	while(elem=va_arg(ap,string_t))
		dest +=strlen(elem);
	va_end(ap);
	c = malloc(dest);
	nilf(c);
	*c=0;
	va_start(ap,first);
	while(elem=va_arg(ap,string_t))
		strcat(c,elem);
	va_end(ap);
	return c;
}

string_t shs_dup(string_t str){
	chrp c = malloc(strlen(str)+1);
	nilf(c);
	strcpy(c,str);
	return c;
}

string_t shs_decode(string_t *str,char* buf){
	char e;
	chrp d;
	d = buf;
	while(*(*str) && *(*str)<=' '){
		(*str)++;
	}
	nilf(*(*str));
	while(e=*(*str)){
		if(e<=' ')break;
		switch(e){
		caseof('\\',*d++=*(++(*str)))
		casedef(*d++=e);
		}
		(*str)++;
	}
	*d++=0;
	if(!*buf) return NULL; /* zero string */
	return shs_dup(buf);
}


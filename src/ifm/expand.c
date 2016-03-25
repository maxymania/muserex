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
#include "ifm.h"

static int sh_skip_word(string_t var){
	int n=0;
	char c;
	for(;;n++,var++){
		c = *var;
		if((c>='0')&&(c<='9')) continue;
		if((c>='A')&&(c<='Z')) continue;
		if((c>='a')&&(c<='z')) continue;
		return n;
	}
	return n;
}

static int sh_skip_novar(string_t var){
	int n=0;
	for(;;n++,var++){
		switch(*var){
		case 0:
		case '$':
		case '~':
			return n;
		case '\\':
			n++;var++;
			break;
		}
	}
	return n;
}

string_t sh_getvar(string_t var){
	string_t val = getenv(var);
	return val;
}

static string_t sh_getvarOr(string_t var,string_t ors){
	string_t val = sh_getvar(var);
	if(!val)return ors;
	return val;
}

sds sh_expand_string(string_t src){
	sds dest = sdsempty();
	sds temp;
	int n;
	#define INSERTVAR                         \
	if(n){                                    \
	temp = sdsnewlen(src,n);                  \
	dest = sdscat(dest,sh_getvarOr(temp,"")); \
	sdsfree(temp);                            \
	src += n;                                 \
	}
	for(;;) {
		n = sh_skip_novar(src);
		if(n) {
			dest = sdscatlen(dest,src,n);
			src += n;
		}
		if(!*src) break;
		if(*src=='$') {
			src++;
			if(*src=='{') {
				src++;
				n = sh_skip_word(src);
				INSERTVAR;
				while(*src&&*src!='}') src++;
				if(*src=='}') src++;
			} else {
				n = sh_skip_word(src);
				INSERTVAR;
			}
		} else if(*src=='~'){
			src++;
			dest = sdscat(dest,sh_getvarOr("HOME",""));
		}
	}
	return dest;
}


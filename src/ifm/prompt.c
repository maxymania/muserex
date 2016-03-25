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
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <libgen.h>
#include "ifm.h"

#define nilf(x) if(!(x)) return NULL
#define fail(x) if(!(x)) return 
#define caseof(v,c) case v:c;break;
#define casedef(c) default:c;break;

typedef char* chrp;
typedef const chrp* cocp;

#define ccp (chrp)

static string_t PS1;
static char buffer[4096];
static char pathbuf[4096];
#define bufsiz (sizeof(buffer)-1)

static string_t getenvOr(string_t name,string_t o){
	/* string_t r = getenv(name); */
	string_t r = sh_getvar(name);
	if(r)return r;
	return o;
}

void sh_prompt_update(){
	int i;
	char isUid = (geteuid()==0)?'#':'$';
	PS1 = getenvOr("IFM_PS1","\\u@\\h \\$ ");
	string_t cwd = getcwd(pathbuf,sizeof pathbuf);
	if(!cwd)cwd = "~";
	string_t ps1 = PS1;
	char part[256];
	char c;
	*buffer = 0;
	while(*ps1){
		if(*ps1!='\\'){
			snprintf(part,sizeof part,"%c",*ps1++);
			strncat(buffer,part,bufsiz);
			continue;
		}
		ps1++;
		switch(c = *ps1++){
		caseof('v',strncat(buffer,"1.0",bufsiz); continue)
		caseof('V',strncat(buffer,"1.0",bufsiz); continue)
		caseof('s',strncat(buffer,"ush",bufsiz); continue)
		caseof('u',strncat(buffer,getenvOr("USER","me"),bufsiz); continue)
		caseof('W',strncat(buffer,basename(ccp cwd),bufsiz); continue)
		caseof('w',strncat(buffer,cwd,bufsiz); continue)
		caseof('H',i = gethostname(part,sizeof part); if(i<0)*part=0)
		caseof('h',i = gethostname(part,sizeof part); if(i<0)*part=0)
		caseof('n',snprintf(part,sizeof part,"\r\n"))
		caseof('r',snprintf(part,sizeof part,"\r"))
		caseof('e',snprintf(part,sizeof part,"\e"))
		caseof('a',snprintf(part,sizeof part,"\07"))
		caseof('$',snprintf(part,sizeof part,"%c",isUid))
		caseof('[',continue)
		caseof(']',continue)
		caseof('@',continue)
		caseof('t',continue)
		caseof('T',continue)
		caseof('A',continue)
		caseof('d',continue)
		/*casedef(snprintf(part,sizeof part,"%c",c))*/
		}
		strncat(buffer,part,bufsiz);
	}
	snprintf(part,sizeof part,"%s\n");
}

string_t sh_prompt(){
	return buffer;
}



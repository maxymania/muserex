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
#include <libgen.h>
#define __noptarg__def
#include <ngetopt.h>

static int argi = 1;
static const char* arg = "";
static char argbeg='-';
static char buf[2]={' ',0};
static const char* progname = "";

int ngetopt(int argc,argv_t argv,const char* desc){
	noptarg = NULL;
	int C,D;
	while(!*arg){
		if(argbeg!='-') { noptind = argi-1; return -1; }
		if(argi>=argc) return -1;
		arg = argv[argi++];
		noptind = argi;
		argbeg = *arg;
		arg++;
	}
	if(argbeg!='-') { noptind = argi-1; return -1; }
	C = *arg++;
	while(D = *desc++){
		if(C==D){
			if(*desc==':'){
				desc++;
				if(argi<argc){
					noptarg = argv[argi++];
					noptind = argi;
				}
			}
			return C;
		}else if(*desc==':')
			desc++;
	}
	if(!*progname) progname = basename((char*)*argv);
	if(!progname) progname = "";
	*buf=C;
	fprintf(stderr,"%s: unknown option -- %s\n",progname,buf);
	return '?';
}


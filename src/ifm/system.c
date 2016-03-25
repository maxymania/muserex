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
#include <glob.h>
#include "ifm.h"

enum { MAXARGS = 4096 };
static string_t args[MAXARGS+1];
static int raw;

#define nilf(x) if(!(x)) return NULL
#define fail(x) if(!(x)) return 
#define vp (void*)

static void sh_check_cmd(){
	raw = (!strcmp("list",args[0]))
	;
}

static int sh_globit(int argi){
	int i;
	argi--;
	glob_t G;
	switch(glob(args[argi], 0, NULL, &G)){
	case 0:
		for(i=0; (argi < MAXARGS) &&
			G.gl_pathv[i];++i,argi++)
			args[argi] = shs_dup(G.gl_pathv[i]);
		globfree(&G);
		return argi;
	case GLOB_NOSPACE:
		args[0] = "echo";
		args[1] = "Out of memory!";
		args[2] = 0;
		return 2;
	default:
		return argi+1;
	}
}

static string_t sh_getvarOr(string_t var,string_t ors){
	string_t val = sh_getvar(var);
	if(!val)return ors;
	return val;
}

void sh_system(string_t cmd){
	int argc=0,i;
	string_t cur=cmd;
	string_t temp;
	sds SWork;
	char* buf = malloc(strlen(cmd)+100);
	fail(buf);
	while(argc<MAXARGS){
		args[argc] = shs_decode(&cur,buf);
		if(!args[argc])break;
		if(!argc)sh_check_cmd();
		argc++;
		if(!raw){
			if(shs_wasVar()){
				argc--;
				temp = args[argc];
				if(*temp=='$') {
					args[argc] = shs_dup(sh_getvarOr(temp+1,""));
					free(vp temp);
				} else if(*temp=='~') {
					if(temp[1]){
						SWork = sdsnew(sh_getvarOr("HOME","~"));
						SWork = sdscat(SWork,temp+1);
						args[argc] = shs_dup(SWork);
						sdsfree(SWork);
					} else
						args[argc] = shs_dup(sh_getvarOr("HOME","~"));
					free(vp temp);
				}
				argc++;
			}
			if(shs_wasGlob())
				argc = sh_globit(argc);
		}
	}
	args[argc]=NULL;
	if(argc)
		sh_docmd(argc,args);
	for(i=0;i<argc;++i)
		free(vp args[i]);
	free(buf);
}


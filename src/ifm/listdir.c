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
#include <sys/stat.h>
#include <glob.h>
#include <time.h>
#include "ifm.h"

#define aif(x) if(x)
#define bif(x) else if(x)
#define cont(x) if(x)continue

static char buffer [1000];


static void sh_i2s(char** pptr,int I){
	int i;
	char* t;
	/* begin */
	t = *pptr;
	for(i=0;t[i]=='0';++i);
	*pptr = t+i;
	while(i--){
		t[i] = (I%10)+'0';
		I/=10;
	}
}
static char* sh_copystrto(char* dest,string_t src){
	size_t sz = strlen(src);
	strcpy(dest,src);
	return dest+sz;
}

static string_t sh_build(struct stat *pstb){
	struct stat stb;
	time_t tmv;
	struct tm *tmp;
	sds t1,t2;
	char* ptr;
	stb = *pstb;
	t1 = buffer;
	t2 = sh_copystrto(t1,"---------- ");
	     sh_copystrto(t2,"0000/00/00-00:00:00 ");
	//
	
	aif(S_ISDIR (stb.st_mode)) *t1='d';
	bif(S_ISSOCK(stb.st_mode)) *t1='s';
	bif(S_ISFIFO(stb.st_mode)) *t1='p';
	bif(S_ISBLK (stb.st_mode)) *t1='b';
	bif(S_ISCHR (stb.st_mode)) *t1='c';
	aif(stb.st_mode&0400)       t1[1]='r';
	aif(stb.st_mode&0200)       t1[2]='w';
	aif(stb.st_mode&0100)       t1[3]='x';
	aif(stb.st_mode&0040)       t1[4]='r';
	aif(stb.st_mode&0020)       t1[5]='w';
	aif(stb.st_mode&0010)       t1[6]='x';
	aif(stb.st_mode&0004)       t1[7]='r';
	aif(stb.st_mode&0002)       t1[8]='w';
	aif(stb.st_mode&0001)       t1[9]='x';
	
	tmv = stb.st_mtime;
	tmp = localtime(&tmv);
	ptr = t2;
	sh_i2s(&ptr,tmp->tm_year+1900); ptr++;
	sh_i2s(&ptr,tmp->tm_mon);       ptr++;
	sh_i2s(&ptr,tmp->tm_mday);      ptr++;
	sh_i2s(&ptr,tmp->tm_hour);      ptr++;
	sh_i2s(&ptr,tmp->tm_min);       ptr++;
	sh_i2s(&ptr,tmp->tm_sec);
	
	return t1;
}

static void sh_showfile(string_t fname){
	struct stat stb;
	string_t line;
	memset(&stb,0,sizeof(stb));
	if(stat(fname,&stb)){
		stb.st_mode = S_IFREG;
	}
	line = sh_build(&stb);
	printf("%s%s\n",line,fname);
}

static string_t sh_getvarOr(string_t var,string_t ors){
	string_t val = sh_getvar(var);
	if(!val)return ors;
	return val;
}

void sh_show(int argc,const string_t *argv){
	int i,j;
	glob_t G;
	sds Temp;
	
	if(argc<2) return;
	for(i=1;i<argc;++i){
		if(*(argv[i])=='~'){
			Temp = sdsnew(sh_getvarOr("HOME","~"));     cont(!Temp);
			Temp = sdscat(Temp,argv[i]+1);
		}else Temp = sdsnew(argv[i]);
		cont(!Temp);
		cont(glob(Temp, 0, NULL, &G));
		for(j=0;G.gl_pathv[j];++j)
			sh_showfile(G.gl_pathv[j]);
		globfree(&G);
		sdsfree(Temp);
	}
}



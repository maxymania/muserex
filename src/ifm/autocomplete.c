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
#include <linenoise.h>
#include <libgen.h>
#include <sys/types.h>
#include <dirent.h>
#include "ifm.h"
#include <glob.h>

#define of(c) case c:



static inline int wschar(char c){
	switch(c){
	of(' ')
	of('\t')
	of('\\')
	return 1;
	}
	return 0;
}

#if 0
static inline int badchar(char c){
	switch(c){
	of(' ')
	of('\t')
	of('$')
	of('~')
	of('\\')
	return 1;
	}
	return 0;
}
#endif

static size_t sh_walkback_ws(string_t buf,size_t s){
	while(s--){
		if(!wschar(buf[s])) return s+1;
	}
	return 0;
}

static size_t sh_walkback(string_t buf,size_t s){
	while(s--){
		if(wschar(buf[s])) return s+1;
	}
	return 0;
}

static int sh_find(string_t buf,char c){
	while(*buf)
		if(c==*buf++)return 1;
	return 0;
}

#define fail(x) if(!(x)) return 
#define cont(x) if(!(x)) continue 

static void sh_completion(const char *buf, linenoiseCompletions *lc) {
	glob_t G;
	sds base,pref,Comp;
	size_t bl,sh;
	int i;
	
	bl = strlen(buf);
	sh = sh_walkback(buf,bl);
	
	base = sdsnewlen(buf,sh);      fail(base);
	pref = sdsnew(buf+sh);         fail(pref);
	if(!(
		sh_find(pref,'*')
		)){
		pref = sdscat(pref,"*");   fail(pref);
	}
	
	if(glob(pref, 0, NULL, &G))goto cleanup;
	
	for(i=0;G.gl_pathv[i];++i){
		Comp = sdsdup(base);                 cont(Comp);
		Comp = sdscat(Comp,G.gl_pathv[i]);   cont(Comp);
		linenoiseAddCompletion(lc,Comp);
		sdsfree(Comp);
	}
	globfree(&G);
	
cleanup:
	sdsfree(base);
	sdsfree(pref);
}

void sh_initAutoComplete(){
	linenoiseSetCompletionCallback(sh_completion);
}


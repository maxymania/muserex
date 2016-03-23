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
#include "ush.h"

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

static int sh_prefix(string_t prefix,string_t str){
startloop:
	if(!*prefix)return 1;
	if(*prefix!=*str)return 0;
	prefix++;
	str++;
	goto startloop;
}

static size_t sh_walkback(string_t buf,size_t s){
	while(s--){
		if(badchar(buf[s])) return s+1;
	}
	return 0;
}

#define fail(x) if(!(x)) return 
#define cont(x) if(!(x)) continue 

static void sh_completion(const char *buf, linenoiseCompletions *lc) {
	sds base,pref,pdup,Comp;
	char* badd;
	DIR* dir;
	struct dirent* ent;
	size_t bl,sh;
	
	bl = strlen(buf);
	sh = sh_walkback(buf,bl);
	
	base = sdsnewlen(buf,sh);      fail(base);
	pref = sdsnew(buf+sh);         fail(pref);
	pdup = sdsdup(pref);           fail(pdup);
	badd = dirname(pdup);
	
	if(strcmp(".",badd)){ /* not "." */
		if(strcmp("/",badd)) /* not "/" */
			base = sdscat(base,badd);
			fail(base);
		base = sdscat(base,"/");
		fail(base);
	}
	dir = opendir(badd);           fail(dir);
	sdsfree(pdup);
	
	if(*pref){
		pdup = sdsnew(basename(pref)); fail(pdup);
		sdsfree(pref);
		pref = pdup;
	}
	
	while(ent = readdir(dir)){
		cont(sh_prefix(pref, ent->d_name));
		cont(*pref||!sh_prefix(".", ent->d_name));
		Comp = sdsdup(base);
		cont(Comp);
		Comp = sdscat(Comp,ent->d_name);
		cont(Comp);
		linenoiseAddCompletion(lc,Comp);
		sdsfree(Comp);
	}
}

void sh_initAutoComplete(){
	linenoiseSetCompletionCallback(sh_completion);
}


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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include <ngetopt.h>

#define BMASK_USER  05700
#define BMASK_GROUP 02070
#define BMASK_OTHER 00007
/* All users, except setuid bit */
#define BMASK_ALL   01777

#define BIT_READ   00444
#define BIT_WRITE  00222
#define BIT_EXEC   00111
#define BIT_SETID  06000
#define BIT_STICKY 01000

typedef const char* string_t;
#define caseof(v,c) case v:c;break;

string_t oarg(){
	if(noptarg)return noptarg;
	fprintf(stderr,"Program crash! Cause: omitted optarg\n");
	exit(1);
	return NULL;
}
void* assertAlloc(size_t sz){
	void* p=malloc(sz);
	if(p)return p;
	perror("cpaste");
	exit(1);
	return NULL;
}

string_t destDir = ".";
enum {
	wCOPY,
	wCUT,
	wSYML,
	wHRDL,
};
int way = wCOPY;
#define W(x) way=w##x

int exvr = 0;

void setDir();
void cpaste(string_t file);

char buffer[4096];

int main(int argc,const string_t* argv){
	int c;
	while ( (c = ngetopt(argc, argv, "-d:csh")) != -1) {
		switch(c){
		caseof('-',goto end)
		caseof('d',setDir())
		caseof('c',W(CUT))
		caseof('s',W(SYML))
		caseof('h',W(HRDL))
		}
	}
end:
	
	while(noptind<argc){
		cpaste(argv[noptind++]);
	}
	return exvr;
}

void notifyError(int rv){
	if(rv==0) return; /* no error! */
	perror("create");
	exvr = 1;
}
#define _(x) notifyError(x)

void setDir(){
	char *dest;
	destDir = oarg();
	size_t sz = strlen(destDir);
	if(destDir[sz-1]=='/'){
		dest = assertAlloc(sz+1);
		strcpy(dest,destDir);
		destDir=dest;
		while(sz--){
			if(dest[sz]!='/')break;
			dest[sz] = 0;
		}
	}
}

int copyFile(string_t from, string_t to){
	struct stat stb;
	int n,src,dst;
	if(stat(from,&stb)) return 1;
	if(S_ISDIR(stb.st_mode)){
		errno=EISDIR;
		return 1;
	}
	if(!(S_ISREG(stb.st_mode))){
		return mknod(to,stb.st_mode,stb.st_rdev);
	}
	src = open(from,O_RDONLY);
	if(src<0) return 1;
	dst = open(to,O_WRONLY|O_CREAT|O_TRUNC,stb.st_mode&07777);
	if(dst<0){
		close(src);
		return 1;
	}
	for(;;){
		n = read(src,buffer,sizeof buffer);
		if(n<1) break;
		write(dst,buffer,n);
	}
	close(src);
	close(dst);
	return 0;
}

void cpaste(string_t file){
	string_t fn = basename((char*)file);
	size_t szt = strlen(fn)+strlen(destDir)+16;
	char buf[szt];
	sprintf(buf,"%s/%s",destDir,fn);
	switch(way){
	caseof(wCOPY,_(copyFile(file,buf)))
	caseof(wCUT ,_(rename(file,buf)))
	caseof(wSYML,_(symlink(file,buf)))
	caseof(wHRDL,_(link(file,buf)))
	}
}


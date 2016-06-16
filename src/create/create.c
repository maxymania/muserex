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

void getMode();
void setRights(int c);
void createObj(string_t arg);
int way  = 'm';
int type = 'f';
int exvr = 0;
mode_t mode = S_IFREG;
mode_t perm  = 0644;
mode_t permd = 0755;
dev_t dev = 0;
unsigned int major=0,minor=0;
string_t sourceFile = NULL;

string_t oarg(){
	if(noptarg)return noptarg;
	fprintf(stderr,"Program crash! Cause: omitted optarg\n");
	exit(1);
	return NULL;
}

#define noptarg oarg()

int main(int argc,const string_t* argv){
	int c;
	while ( (c = ngetopt(argc, argv, "-bcdfspMA:I:u:g:o:a:l:h:m:")) != -1) {
		switch(c){
		caseof('-',goto end)
		case 'l': /* symlink */
		case 'h': /* hardlinks */
		case 'm': /* stat-mknod */
			sourceFile = noptarg;
		case 'd': way='M'; /* directory */
		case 'b': /* block device */
		case 'c': /* character device */
		case 'f': /* regular file */
		case 's': /* socket */
		case 'p': /* pipe */
		type = c;
		getMode();
		break;
		caseof('I',minor = atoi(noptarg))
		caseof('A',major = atoi(noptarg))
		caseof('M',way = 'M')
		case 'u':
		case 'g':
		case 'o':
		case 'a':
			setRights(c);
			break;
		}
	}
end:
	dev = makedev(major,minor);
	while(noptind<argc){
		createObj(argv[noptind++]);
	}
	return exvr;
}

void setRights(int c){
	mode_t where = 0;
	mode_t what  = 0;
	string_t rtv = noptarg;
	switch(c){
	caseof('u',where|=BMASK_USER)
	caseof('g',where|=BMASK_GROUP)
	caseof('o',where|=BMASK_OTHER)
	caseof('a',where|=BMASK_ALL)
	}
	while(c = *rtv++)switch(c){
	caseof('r',what|=BIT_READ)
	caseof('w',what|=BIT_WRITE)
	caseof('x',what|=BIT_EXEC)
	caseof('s',what|=BIT_SETID)
	caseof('t',what|=BIT_STICKY)
	}
	perm  &= ~where;
	perm  |= (what&where);
	permd &= ~where;
	permd |= (what&where);
}

void getMode(){
	switch(type){
	caseof('b',mode=S_IFBLK)
	caseof('c',mode=S_IFCHR)
	caseof('f',mode=S_IFREG)
	caseof('s',mode=S_IFSOCK)
	caseof('p',mode=S_IFIFO)
	}
}

int notifyError(int rv){
	if(rv==0) return 0; /* no error! */
	perror("create");
	exvr = 1;
	return 1;
}
#define _(x) notifyError(x)

int mkFile(string_t arg){
	int fd = open(arg,O_RDONLY|O_CREAT,perm);
	if(fd<0)return -1;
	close(fd);
	return 0;
}

void createObj(string_t arg){
	struct stat stb;
	switch(way){
	caseof('m', _(mknod(arg,mode|perm,dev)) )
	caseof('M',
		switch(type){
		caseof('l', _(symlink(sourceFile,arg)) )
		caseof('h', _(link(sourceFile,arg)) )
		caseof('m', if(!_(stat(sourceFile,&stb)))
						_(mknod(arg,stb.st_mode,stb.st_rdev))  )
		caseof('b', _(mknod(arg,S_IFBLK|perm,dev)) )
		caseof('c', _(mknod(arg,S_IFCHR|perm,dev)) )
		caseof('d', _(mkdir(arg,permd)) )
		caseof('f', _(mkFile(arg)) )
		caseof('p', _(mkfifo(arg,perm)) )
		caseof('s', _(mknod(arg,S_IFSOCK|perm,dev)) )
		}
	)
	}
}


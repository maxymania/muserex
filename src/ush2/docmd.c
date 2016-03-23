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
#include "ush.h"

#define nilf(x) if(!(x)) return NULL
#define fail(x) if(!(x)) return 
typedef char* chrp;
typedef const chrp* cocp;

static void sh_cd(int argc,const string_t *argv){
	string_t dest;
	if(argc==1)
		dest = getenv("HOME");
	else
		dest = argv[1];
	fail(dest);
	chdir(dest);
}

static void sh_spawn(int argc,const string_t *argv){
	int status;
	pid_t p;
	p = fork();
	if(p<0) { perror("fork"); return; }
	if(p==0){
		execvp(argv[0],(cocp)argv);
		exit(1); // fail!
	}
	waitpid(p,&status,0);
}

void sh_docmd(int argc,const string_t *argv){
	if(argc<1)return;
	if(!strcmp("cd",argv[0])){
		sh_cd(argc,argv);
	}else{
		sh_spawn(argc,argv);
	}
}



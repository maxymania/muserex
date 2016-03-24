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
#include <unistd.h>
#include <ngetopt.h>
#include <sds.h>
#include "nw.h"

typedef const char* string_t;
#define caseof(v,c) case v:c;break;

int
useIPv4 = 0,
useIPv6 = 0,
useUNIX = 0,
port = 0
;
sds addr = NULL;
string_t l4p = "TCP";

static string_t oarg(){
	if(noptarg)return noptarg;
	fprintf(stderr,"Program crash! Cause: omitted optarg\n");
	exit(1);
	return NULL;
}

int main(int argc,const string_t* argv){
	scka socket;
	struct sock_names names;
	pid_t fp;
	int c;
	while ( (c = ngetopt(argc, argv, "-46ui:p:")) != -1) {
		switch(c){
		caseof('-',goto end)
		caseof('4',useIPv4 = 1)
		caseof('6',useIPv6 = 1)
		caseof('u',useUNIX = 1; l4p="UNIX")
		caseof('p',port    = atoi  (oarg())  )
		caseof('i',addr    = sdsnew(oarg())  )
		}
	}
end:
	if(useIPv4 && useIPv6){
		fprintf(stderr,"Only one of IPv4 and IPv6 is allowed\n");
		exit(1);
	}
	if((useIPv4||useIPv6)&&useUNIX){
		fprintf(stderr,"Only one of TCP/IP and UNIX is allowed\n");
		exit(1);
	}
	
	/**/ if(useIPv4) socket = nw_listen_in4 (addr,port);
	else if(useIPv6) socket = nw_listen_in6 (addr,port);
	else if(useUNIX) socket = nw_listen_unix(addr);
	
	for(;;){
		c = nw_accept_name(socket,&names);
		if(c<0){ sleep(1); continue; }
		fp = fork();
		if(fp<0){
			perror("fork");
		}else if(fp==0){
			setenv("PROTOL4",l4p,1);
			setenv("LOCALIP",names.local,1);
			setenv("REMOTEIP",names.remote,1);
			dup2(c,fileno(stdin));
			dup2(c,fileno(stdout));
			close(c);
			execvp(argv[noptind],(char**)argv+noptind);
			perror("execvp");
			exit(1);
		}
		close(c);
	}
}



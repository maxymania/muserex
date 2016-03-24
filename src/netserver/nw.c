/*
 * Copyright (c) 2014, Simon Schmidt
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

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "nw.h"

int nw_accept(scka s){
	char addr[s.addrl];
	int i=s.addrl;
	return accept(s.sock, (struct sockaddr *) addr, &i);
}

int nw_accept_name(scka s,struct sock_names* names){
	struct sockaddr *sad;
	char addr[s.addrl];
	int i=s.addrl;
	int r=accept(s.sock, (struct sockaddr *) addr, &i);
	if(r<0)return r;
	sad=(struct sockaddr *)addr;
	i=s.addrl;
	inet_ntop(sad->sa_family,sad,names->remote,255);
	getsockname(r,sad,&i);
	inet_ntop(sad->sa_family,sad,names->local,255);
	names->remote[255]=0;
	names->local[255]=0;
	return r;
}




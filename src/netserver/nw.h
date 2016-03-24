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
#ifndef lib2_nw_h
#define lib2_nw_h

struct scka_s{
	int sock;
	int addrl;
};
typedef struct scka_s scka;
scka nw_listen_in4(const char* host,int port);
int nw_connect_in4(const char* host,int port);
scka nw_listen_in6(const char* host,int port);
int nw_connect_in6(const char* host,int port);
scka nw_listen_unix(const char* sckf);
int nw_connect_unix(const char* sckf);

int nw_accept(scka s);

struct sock_names{
	char local[256];
	char remote[256];
};

int nw_accept_name(scka s,struct sock_names* names);

#endif


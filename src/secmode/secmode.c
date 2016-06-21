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
#include <stdint.h>
#include <errno.h>
#include <seccomp.h>
#include <sched.h>

typedef const char* string_t;
#define caseof(v,c) case v:c;break;

#define HW_SPECIFIC 0x001
#define SYS_CHMODS  0x002
#define SYS_CHROOT  0x004
#define SYS_CHDIR   0x008
#define SYS_VMRW    0x010
#define SYS_VSPLC   0x020
#define SYS_SPLC    0x040

int main(int argc,const string_t* argv){
	uint32_t flags = 0;
	scmp_filter_ctx ctx;
	if(argc<2)return 1;
	string_t str = argv[1];
	for(;*str;str++)switch(*str){
	caseof('H',flags|=HW_SPECIFIC)
	caseof('m',flags|=SYS_CHMODS)
	caseof('p',flags|=SYS_CHROOT|SYS_CHDIR)
	caseof('r',flags|=SYS_CHROOT)
	caseof('v',flags|=SYS_VMRW)
	caseof('s',flags|=SYS_SPLC|SYS_VSPLC)
	caseof('S',flags|=SYS_VSPLC)
	}
	
	ctx = seccomp_init(SCMP_ACT_KILL);

	if(flags&HW_SPECIFIC){
		#include "syscalls.h"
	}
	if(flags&SYS_CHMODS){
		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(chmod), 0);
		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(fchmod), 0);
		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(fchmodat), 0);

		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(chown), 0);
		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(fchown), 0);
		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(lchown), 0);
		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(fchownat), 0);

		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(chown32), 0);
		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(fchown32), 0);
		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(lchown32), 0);
	}
	if(flags&SYS_CHROOT)
		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(chroot), 0);
	if(flags&SYS_CHDIR){
		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(chdir), 0);
		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(fchdir), 0);
	}
	if(flags&SYS_VMRW){
		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(process_vm_readv), 0);
		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(process_vm_writev), 0);
		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(ptrace), 0);
	}
	if(flags&SYS_VSPLC)
		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(vmsplice), 0);
	if(flags&SYS_SPLC)
		seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(splice), 0);

	seccomp_load(ctx);
	execvp(argv[2],(char *const*)&argv[2]);
	return 0;
}



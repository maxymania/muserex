# NAME
secmode - run programs in secure mode

# SYNOPSIS

secmode [HmprvSs] cmd args...


# DESCRIPTION

Runs a program in an seccomp-jail

H : disable arch specific systemcalls such as vm86 or ppc_rtas
m : disable chmod, chown etc.
p : lock the current work directory, eg. no chdir and no chroot
r : lock the current root, eg. no chroot (if compined with 'p' this 'r' is redundant)
v : ban ptrace and process_vm_readv/process_vm_writev
S : disable vmsplice (if compined with 's' this 'S' is redundant)
s : disable splice and vmsplice

# EXAMPLES

```sh
secmode s apache-httpd
# runs apache webserver without splice and vmsplice

```


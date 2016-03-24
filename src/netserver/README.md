# NAME
netserver - uscpi-tcp like complaint server

# SYNOPSIS

```
netserver [-46u] [-i <ip-addr>] [-p <port-num>] program [args...]

```

# DESCRIPTION

* -4 : use TCP/IPv4
* -6 : use TCP/IPv6
* -u : UNIX domain sockets
* -i <addr> : IP-address or UNIX domain socket path.
* -p <port> : TCP port

the environment variables being set:

* `PROTOL4`: the string TCP or UDP
* `PROTOL3`: the string IPv4 or IPv6
* `LOCALIP`: the local IP address
* `LOCALPORT`: the local port number (not implemented)
* `REMOTEIP`: the remote IP address
* `REMOTEPORT`: the remote port number (not implemented)

```
netserver -4ip 192.168.1.1 80 http-srv -rug /var/www www www
```

# REMARKS:
Not tested yet.

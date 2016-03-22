# NAME
create - a tool that create file, directories, named pipes, sockets and devices.

# SYNOPSIS

create [-bcdfspM] [-A major] [-I minor] [-[ugoa] rights] Name1 ... NameN

# DESCRIPTION

## File Type options
* -l target : symbolic link
* -h target : hard link
* -m target : do a stat()->mknod() copy.
* -b : block device
* -c : character device
* -d : directory
* -f : regular file (default)
* -s : socket file
* -p : named pipe/fifo

## Devices:
* -A major: major device number
* -I minor: minor device number

## Rights:
* -u [rwxst]: rights the user will have (default is rw; rwx on dirs)
* -g [rwxst]: rights the group will have (default is r; rx on dirs)
* -o [rwxst]: rights others will have (default is r; rx on dirs)
* -a [rwxst]: rights all will have

## Other
* -M : prefer mkfifo and open(O_CREAT) rather than mknod.

# EXAMPLES

```sh
# Howto create the file `File1'
create File1

# Howto create the file `File1' with the permissions 642

create -ugo rw w r File1

# Howto create a unix domain socket file

create -s TheSocket

# mkdir Dir1
create -d Dir1

# mknod -m 622 /dev/console c 5 1

create -cAIugo 5 1 rw w w /dev/console

# mknod -m 660 /dev/hda b 3 0

create -bAIugo 3 0 rw rw - /dev/hda

# mkfifo Fifo
create -p Fifo

```


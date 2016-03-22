# NAME
cpaste - copy and paste

# SYNOPSIS

cpaste -d DestinationDirectory File1 File2 File3

# DESCRIPTION

Copies a file and pastes it into an directory.

## Destination
* -d dest: destination directory (default is .)

## Operations Types
* -c : cut and paste
* -s : pastes a symlink
* -h : pastes a hardlink (will fail with directories on most filesystems)

# EXAMPLES

```sh

# Move a file into /temp

cpaste -dc /temp MyFile

# Symlink a file into the actual directory

cpaste -s /path/to/file

# Hardlink a file into the actual directory

cpaste -h /var/logs.apache

# Simply copies files

cpaste -d myPlace file1 file2 file3

```


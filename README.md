# Linux-daemon
The daemon which gathers bytes of information from presumably random sources. 

Every 5 seconds the daemon checks size of the special file ~/random/buffer. If size is less than 5 mb (or file doesn't exist), the daemon fills it with random data.

Run daemon:
-make
-bash test.sh

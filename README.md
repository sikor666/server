# Server

Error in accept: Too many open files
``` bash
olive@slimcase-666:~/Workspace/git/server/build$ ulimit -a
-t: cpu time (seconds)              unlimited
-f: file size (blocks)              unlimited
-d: data seg size (kbytes)          unlimited
-s: stack size (kbytes)             8192
-c: core file size (blocks)         0
-m: resident set size (kbytes)      unlimited
-u: processes                       63036
-n: file descriptors                1024
-l: locked-in-memory size (kbytes)  2027068
-v: address space (kbytes)          unlimited
-x: file locks                      unlimited
-i: pending signals                 63036
-q: bytes in POSIX msg queues       819200
-e: max nice                        0
-r: max rt priority                 0
-N 15: rt cpu time (microseconds)   unlimited
olive@slimcase-666:~/Workspace/git/server/build$ ulimit -n 16384
```

Error in connect: Cannot assign requested address
``` bash
# IP Sysctl: https://www.kernel.org/doc/html/latest/networking/ip-sysctl.html
#
# tcp_tw_reuse - INTEGER
#
#    Enable reuse of TIME-WAIT sockets for new connections when it is safe from protocol
#    viewpoint.
#        0 - disable
#        1 - global enable
#        2 - enable for loopback traffic only
#
#    It should not be changed without advice/request of technical experts.
#
#    Default: 2

sysctl net.ipv4.tcp_tw_reuse
# net.ipv4.tcp_tw_reuse = 2
sudo sysctl -w net.ipv4.tcp_tw_reuse=1
# net.ipv4.tcp_tw_reuse = 1
```

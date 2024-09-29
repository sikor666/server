# Server

IP Sysctl: https://www.kernel.org/doc/html/latest/networking/ip-sysctl.html

``` bash
# ip_local_port_range - 2 INTEGERS
#
#    Defines the local port range that is used by TCP and UDP to choose the local port.
#    The first number is the first, the second the last local port number. If possible,
#    it is better these numbers have different parity (one even and one odd value).
#    Must be greater than or equal to ip_unprivileged_port_start.
#
#    The default values are 32768 and 60999 respectively.

sysctl net.ipv4.ip_local_port_range
# net.ipv4.ip_local_port_range = 32768    60999
sudo sysctl -w net.ipv4.ip_local_port_range="1024 65535"
# net.ipv4.ip_local_port_range = 1024     65535


# tcp_fin_timeout - INTEGER
#
#    The length of time an orphaned (no longer referenced by any application) connection
#    will remain in the FIN_WAIT_2 state before it is aborted at the local end. While a
#    perfectly valid “receive only” state for an un-orphaned connection, an orphaned
#    connection in FIN_WAIT_2 state could otherwise wait forever for the remote to close
#    its end of the connection.
#
#    Cf. tcp_max_orphans
#
#    Default: 60 seconds

sysctl net.ipv4.tcp_fin_timeout
# net.ipv4.tcp_fin_timeout = 60
sudo sysctl -w net.ipv4.tcp_fin_timeout=30
# net.ipv4.tcp_fin_timeout = 30


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

Error in accept: Too many open files:
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

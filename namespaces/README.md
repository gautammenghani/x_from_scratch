A small program to explore how namespaces work by running the given command in a network namespace with no network.

```
$ ./namespace ping 1.1.1.1
child pid: 42231
Running the command /usr/bin/ping
ping: connect: Network is unreachable
$
```

Namespaces can be created in 2 ways - the code in this dir explores both ways:
1. Clone syscall
2. unshare syscall

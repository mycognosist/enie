# enie

Emit Network Interface Events (ENIE).

`enie` utilises RTNETLINK to subscribe to network interface state-change events from the kernel. `enie` outputs events on `stdout`, allowing it to be coupled with other systems.

### Usage

Compile:

`gcc enie.c -o enie`

Run:

`./enie`

### Output

Events are prefixed by an integer denoting event type and followed by the interface name and any other relevant variables (ie. state or IP address).

States:

```
 0 - UP
 1 - DOWN
 2 - IP_ADD
 3 - IP_DEL
```

Examples:

`0_wlan0_UP_RUNNING` or `0_wlan0_UP_NOT_RUNNING`  
`1_wlan0`  
`2_wlan0_192.168.1.12`  
`3_wlan0`

### Rationale

I've been thinking about network interface state and the most efficient way to share it with a remote client. This is my attempt at experimentation with a push-based system, as well as a means of gaining deeper understanding of the Linux kernel and subsystems.

### Resources

 - [Monitoring Linux networking state using netlink](https://olegkutkov.me/2018/02/14/monitoring-linux-networking-state-using-netlink/)
 - [Manipulating the Networking Environment Using RTNETLINK](https://www.linuxjournal.com/article/8498)
 - [rtnetlink - Linux IPv4 routing socket (manual)](http://man7.org/linux/man-pages/man7/rtnetlink.7.html)

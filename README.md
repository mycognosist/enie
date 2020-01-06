# enie

Emit Network Interface Events (ENIE).

`enie` utilises RTNETLINK to subscribe to network interface state-change events from the kernel. Events matching the user-specified interface (for example, `wlan0`) are printed to `stdout`, allowing `enie` to be coupled with other systems.

### Usage

Compile:

`gcc enie.c -o enie`

Run:

`./enie wlan0`

### Output

Events are prefixed by an integer denoting event type and followed by the interface name and any other relevant variables (ie. state or IP address).

States:

```
 0 - UP_CONNECTED
 1 - UP_DISCONNECTED
 2 - DOWN
 3 - IP_ASSIGNED
 4 - IP_DELETED
```

Examples:

`0_wlan0_UP_CONNECTED`  
`1_wlan0_UP_DISCONNECTED`  
`2_wlan0_DOWN`  
`3_wlan0_IP_ASSIGNED_192.168.1.12`  
`4_wlan0_IP_DELETED`

### Rationale

I've been thinking about network interface state and the most efficient way to access and share it. This is my attempt at experimentation with a push-based system, as well as a means of gaining deeper understanding of the Linux kernel and subsystems.

### Resources

 - [Monitoring Linux networking state using netlink](https://olegkutkov.me/2018/02/14/monitoring-linux-networking-state-using-netlink/)
 - [Manipulating the Networking Environment Using RTNETLINK](https://www.linuxjournal.com/article/8498)
 - [rtnetlink - Linux IPv4 routing socket (manual)](http://man7.org/linux/man-pages/man7/rtnetlink.7.html)

# enie

Emit Network Interface Events (ENIE).

`enie` utilises RTNETLINK to subscribe to network interface state-change events from the kernel. The user specifies an interface to listen to (for example, `wlan0`) as an argument. `enie` then outputs events on stdout, allowing it to be coupled with other systems.

### Usage

Compile:

`gcc enie.c -o enie`

Run:

`./enie wlan0`

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
`3_wlan`

### Why?

I've been thinking about network interface state and the most efficient way to share it with a remote client. This is my attempt at experimentation with a push-based system, as well as a means of gaining deeper understanding of the Linux kernel and subsystems.

# Systemd

## VRF

### 20-vrf-11.netdev

~~~
[NetDev]
Name=BSA-PROD
Kind=vrf

[VRF]
TableId=11
~~~

### cat 10-en01.network 

~~~
[Match]
MACAddress=00:16:3E:00:11:02
a0r 15:03:51 network # cat 10-en02.network 
[Match]
MACAddress=00:16:3E:00:11:03

[Network]
VRF=AGI-MGT
~~~


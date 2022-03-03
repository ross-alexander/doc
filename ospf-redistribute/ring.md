
# Connected to OSPF (dalet)

~~~
route-map ospf-import permit 1
  set tag 65101
exit

router ospf vrf prod
  redistribute connected metric 20 route-map ospf-import
exit
~~~

# BGP export (aleph)

~~~
route-map bgp-export permit 1
  match tag 65101
exit

router bgp 65101 vrf prod
  address-family ipv4 unicast
    redistribute ospf route-map bgp-export
  exit-address-family
exit
~~~


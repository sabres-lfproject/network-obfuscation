from scapy.all import *

payload="HelloWorld"*3
p=IP(proto=4, dst='10.1.1.3')
p1=IP(dst='10.1.1.3')/ICMP(type="echo-request")/payload
r=p1.build()*3
send(p/r, iface='eth1')

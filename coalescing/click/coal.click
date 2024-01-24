c0 :: Classifier(12/0800, 12/0806, -);
c1 :: Classifier(12/0800, 12/0806, -);

vls :: VlanSwitch(VLAN 126, VLAN 128);
vls[1] -> c1;
vls[0] -> c0;

// Packet Arrival
FromDPDKDevice(0000:03:00.0) -> VLANDecap() -> vls;

// Packet Departure
out_eth1 :: ToDPDKDevice(0000:03:00.0);
out_eth1_126 :: VLANEncap(VLAN_ID 126) -> out_eth1;
out_eth1_128 :: VLANEncap(VLAN_ID 128) -> out_eth1;

// ARP Forwarder
c0[1] -> out_eth1_128
c1[1] -> out_eth1_126
 

// Packet Processing
c0[0] -> Strip(14) -> Coalese() -> EtherEncap(0x0800, 00:08:a2:0d:db:3d, 00:08:a2:0d:e2:91) -> out_eth1_128
c1[0] -> Strip(14) -> Coalese() -> EtherEncap(0x0800, 00:08:a2:0d:e2:91, 00:08:a2:0d:db:3d) -> out_eth1_126
// Strip required ??

// Discard Unknown Packets
c0[2] -> Discard();
c1[2] -> Discard();

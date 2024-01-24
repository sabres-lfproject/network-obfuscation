c0 :: Classifier(12/0800, 12/0806, -);
c1 :: Classifier(12/0800, 12/0806, -);

vls :: VlanSwitch(VLAN 170, VLAN 167);
vls[1] -> c1;
vls[0] -> c0;

// Packet Arrival
FromDPDKDevice(0000:03:00.0) -> VLANDecap() -> vls;

// Packet Departure
out_eth1 :: ToDPDKDevice(0000:03:00.0);
out_eth1_170 :: VLANEncap(VLAN_ID 170) -> out_eth1;
out_eth1_167 :: VLANEncap(VLAN_ID 167) -> out_eth1;

// ARP Forwarder
c0[1] -> out_eth1_167
c1[1] -> out_eth1_170
 

// Packet Processing
c0[0] -> Strip(14) -> Print (C0IN) -> Dcoalese() -> Print(C0UT) -> EtherEncap(0x0800, 00:08:a2:0d:db:4b, 00:08:a2:0d:e1:b1) -> out_eth1_167
c1[0] -> Strip(14) -> Print (C1IN) -> Coalese() -> Print(C1UT) -> EtherEncap(0x0800, 00:08:a2:0d:e1:b1, 00:08:a2:0d:db:4b) -> out_eth1_170
// Strip required ??

// Discard Unknown Packets
c0[2] -> Discard();
c1[2] -> Discard();

c0 :: Classifier(12/0800, 12/0806, -);
c1 :: Classifier(12/0800, 12/0806, -);

vls :: VlanSwitch(VLAN 169, VLAN 170);
vls[1] -> c1;
vls[0] -> c0;

// Packet Arrival
FromDPDKDevice(0000:03:00.0) -> VLANDecap() -> vls;

// Packet Departure
out_eth1 :: ToDPDKDevice(0000:03:00.0);
out_eth1_170 :: VLANEncap(VLAN_ID 170) -> out_eth1;
out_eth1_169 :: VLANEncap(VLAN_ID 169) -> out_eth1;

// ARP Forwarder
c0[1] -> out_eth1_170
c1[1] -> out_eth1_169


// Packet Processing
c0[0] -> Print(A2B,64) -> out_eth1_170
c1[0] -> Print(B2A,64) -> out_eth1_169
// Strip required ??

// Discard Unknown Packets
c0[2] -> Discard();
c1[2] -> Discard();

c0 :: Classifier(12/0800, 12/0806, -);
c1 :: Classifier(12/0800, 12/0806, -);

vls :: VlanSwitch(VLAN 168, VLAN 169);
vls[1] -> c1;
vls[0] -> c0;

// Packet Arrival
FromDPDKDevice(0000:03:00.0) -> VLANDecap() -> vls;

// Packet Departure
out_eth1 :: ToDPDKDevice(0000:03:00.0);
out_eth1_168 :: VLANEncap(VLAN_ID 168) -> out_eth1;
out_eth1_169 :: VLANEncap(VLAN_ID 169) -> out_eth1;

// ARP Forwarder
c0[1] -> out_eth1_169
c1[1] -> out_eth1_168
 

// Packet Processing
c0[0] -> Strip(14) -> Print(C0IN,64) -> Coalese() ->Print(C0UT,64) ->  EtherEncap(0x0800, 00:08:a2:0d:db:4b, 00:08:a2:0d:e1:b1) -> out_eth1_169
c1[0] -> Strip(14) ->Print(C1IN,64) ->  Dcoalese() ->Print(C1UT,64) ->  EtherEncap(0x0800, 00:08:a2:0d:e1:b1, 00:08:a2:0d:db:4b) -> out_eth1_168
// Strip required ??

// Discard Unknown Packets
c0[2] -> Discard();
c1[2] -> Discard();

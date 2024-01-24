c0 :: Classifier(12/0800, 12/0806, -);
c1 :: Classifier(12/0800, 12/0806, -);

// Packet Arrival
in_eth1 :: FromDPDKDevice(0000:02:00.0, N_QUEUES 1, MODE none, MAXTHREADS 1) 
	-> c0;
in_eth2 :: FromDPDKDevice(0000:03:00.0, N_QUEUES 1, MODE none, MAXTHREADS 1) 
	-> c1;

out_eth1 :: ToDPDKDevice(0000:02:00.0, MAXQUEUES 1, N_QUEUES 1);
out_eth2 :: ToDPDKDevice(0000:03:00.0, MAXQUEUES 1, N_QUEUES 1);

// Packet Departure
cap_eth1 :: EtherEncap(0x0800, 04:70:00:00:00:01, 04:70:00:00:02:01) 
	->  out_eth1
cap_eth2 :: EtherEncap(0x0800, 04:70:00:00:00:11, 04:70:00:00:03:11) 
	-> out_eth2

// ARP Forwarder
c0[1] 
	-> out_eth2
c1[1] 
	-> out_eth1
 
ip_cl0 :: IPClassifier(syn or fin, -);
ip_cl1 :: IPClassifier(syn or fin, -);

// Packet Processing
c0[0] 
	-> Strip(14) 
	-> CheckIPHeader() 
	-> ip_cl0[1] 
	-> ThreadSafeQueue() 
	-> Coalesq(LENGTH 1400, NPKTS 1, PAD true, OBFUSCATE false, DEBUG false, TIMER 5000) 
	->  cap_eth2
c1[0] 
	-> Strip(14) 
	-> CheckIPHeader() 
	-> ip_cl1[1] 
	->  Dcoales(OBFUSCATE false) 
	->  cap_eth1
// Strip required ??

ip_cl0[0] 
	->  cap_eth2
ip_cl1[0] 
	->  cap_eth1

// Discard Unknown Packets
c0[2] 
	-> Discard();
c1[2] 
	-> Discard();

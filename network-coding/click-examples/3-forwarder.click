/*
* This is an example of forwarding between 3 interfaces in a manner of
* north-southband interfaces using the following topology:
*
*      a   b
*       \ /
*        c   ( this node)
*        |
*        d
*
* Forwarding is allow from a->c->d and b->c->d and vise versa,
* a <-> b forwarding is not directly allowed.
*/

// address info for each of the interfaces
AddressInfo(
        eth1-dev        10.0.0.11       10.0.0.0/24     04:70:00:00:00:11,
        eth2-dev        10.0.1.12       10.0.1.0/24     04:70:00:00:00:12,
        eth3-dev        10.0.2.13       10.0.2.0/24     04:70:00:00:00:13,

        eth1-neigh      10.0.0.1        10.0.0.0/24     04:70:00:00:00:01,
        eth2-neigh      10.0.1.2        10.0.1.0/24     04:70:00:00:00:02,
        eth3-neigh      10.0.2.3        10.0.2.0/24     04:70:00:00:00:03,
);


/* Classifier per interface to handle ARP Requests */
classifier0      :: Classifier(
                        12/0806 20/0001, /* arp requests */
                        -,
                );

classifier1      :: Classifier(
                        12/0806 20/0001, /* arp requests */
                        -,
                );

classifier2      :: Classifier(
                        12/0806 20/0001, /* arp requests */
                        -,
                );

/* IPClassifier per interface to handle proper forwarding */
ipclassifier1   :: IPClassifier(
                        dst host ip eth3-neigh,
                );

ipclassifier2   :: IPClassifier(
                        dst host eth3-neigh,
                );

ipclassifier3   :: IPClassifier(
                        dst host ip eth1-neigh,
                        dst host ip eth2-neigh,
                );

/* Instantiation of each From/To interface */
left_in_device          ::      FromDevice(eth1);
right_in_device         ::      FromDevice(eth2);
south_in_device         ::      FromDevice(eth3);

left_out_device         ::      ToDevice(eth1);
right_out_device        ::      ToDevice(eth2);
south_out_device        ::      ToDevice(eth3);

/* Queue per out-going interface */
q1              :: ThreadSafeQueue(200);
q2              :: ThreadSafeQueue(200);
q3              :: ThreadSafeQueue(200);

/* Marking IP Header per ipclassifier (required before ipclassifier) */
chip            :: MarkIPHeader(14);
chip1           :: MarkIPHeader(14);
chip2           :: MarkIPHeader(14);


/* handle the arp requests */
left_in_device  ->      classifier0[0]  ->      ARPResponder(10.0.1.2 10.0.1.0/24 04:70:00:00:00:02, 10.0.2.3 10.0.2.0/24 04:70:00:00:00:03)    ->      q1;
right_in_device ->      classifier1[0]  ->      ARPResponder(10.0.0.1 10.0.0.0/24 04:70:00:00:00:01, 10.0.2.3 10.0.2.0/24 04:70:00:00:00:03)    ->      q2;
south_in_device ->      classifier2[0]  ->      ARPResponder(10.0.0.1 10.0.0.0/24 04:70:00:00:00:01, 10.0.1.2 10.0.1.0/24 04:70:00:00:00:02)    ->      q3;

/* forwarding rules to ip classifier */
// left interface going southbound
classifier0[1]  ->      chip    ->     ipclassifier1   ->      EtherRewrite(04:70:00:00:00:11, 04:70:00:00:00:03)      ->      q3;
// right interface going southbound
classifier1[1]  ->      chip1   ->      ipclassifier2[0]   ->      EtherRewrite(04:70:00:00:00:12, 04:70:00:00:00:03)      ->      q3; 
// south interface going northbound (left)
classifier2[1]  ->      chip2   ->      ipclassifier3[0]   ->      EtherRewrite(04:70:00:00:00:13, 04:70:00:00:00:01)      ->      q1;
// south interface going northbound (right)
ipclassifier3[1]   ->      EtherRewrite(04:70:00:00:00:13, 04:70:00:00:00:02)      ->      q2;


/* From queue to out interface */
q1      ->      left_out_device;
q2      ->      right_out_device;
q3      ->      south_out_device;

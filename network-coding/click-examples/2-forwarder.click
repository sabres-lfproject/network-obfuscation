/*
* This file is an example of straight forwarding through this device.
* It does not require ip_forwarding to be enabled.
*
*
*/

// address info for each of the interfaces
AddressInfo(
        eth1-dev        10.0.0.11       10.0.0.0/24     04:70:00:00:00:11,
        eth2-dev        10.0.1.12       10.0.1.0/24     04:70:00:00:00:12,

        eth1-neigh      10.0.0.1        10.0.0.0/24     04:70:00:00:00:01,
        eth2-neigh      10.0.1.2        10.0.1.0/24     04:70:00:00:00:02,
);

left_in_device          ::      FromDevice(eth1);
south_in_device         ::      FromDevice(eth3);

left_out_device         ::      ToDevice(eth1);
south_out_device        ::      ToDevice(eth3);

q1              :: Queue(200);
q2              :: Queue(200);

/* handle the arp requests */
left_in_device  ->      q1      ->      EtherRewrite(04:70:00:00:00:11, 04:70:00:00:00:13)      ->      south_out_device;
south_in_device ->      q2      ->      EtherRewrite(04:70:00:00:00:03, 04:70:00:00:00:01)      ->      left_out_device;

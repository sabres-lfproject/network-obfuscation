// address info for each of the interfaces
classifier0     :: Classifier(
        12/0806 20/0001, /* arp requests */
        12/0800,
        -,
        );

classifier1     :: Classifier(
        12/0806 20/0001, /* arp requests */
        12/0800,
        -,
        );

classifier2     :: Classifier(
        12/0806 20/0001, /* arp requests */
        12/0800,
        -,
        );

classifier3     :: Classifier(
        12/0806 20/0001, /* arp requests */
        12/0800,
        -,
        );


ipclassifier    :: IPClassifier(
        dst host 192.168.2.2
        );

data_in                         ::      FromDPDKDevice( 0000:02:02.0, MTU 2500, JUMBO true, N_QUEUES 1, MAXTHREADS 1, MODE none);
left_in_device                  ::      FromDPDKDevice( 0000:02:03.0, MTU 2500, JUMBO true, N_QUEUES 1, MAXTHREADS 1, MODE none);
center_in_device                ::      FromDPDKDevice( 0000:02:04.0, MTU 2500, JUMBO true, N_QUEUES 1, MAXTHREADS 1, MODE none);
right_in_device                 ::      FromDPDKDevice( 0000:02:05.0, MTU 2500, JUMBO true, N_QUEUES 1, MAXTHREADS 1, MODE none);

data_out                        ::      ToDPDKDevice( 0000:02:02.0, N_QUEUES 1);
left_out_device                 ::      ToDPDKDevice( 0000:02:03.0, N_QUEUES 1);
center_out_device               ::      ToDPDKDevice( 0000:02:04.0, N_QUEUES 1);
right_out_device                ::      ToDPDKDevice( 0000:02:05.0, N_QUEUES 1);

chip    :: MarkIPHeader(14);
chip1   :: MarkIPHeader(14);

encode :: SSSMsg(3,2,0,10,1500);
decode :: SSSMsg(3,2,1,10,1500);


/* handle the arp requests */

data_in         ->      classifier0[0]  ->      Print("a")      ->      ARPResponder(192.168.2.2 192.168.2.0/24 04:70:00:00:00:01)        ->      data_out;

// no-ops
left_in_device  ->      classifier1[0]  ->      Print("b")      ->      ARPResponder(192.168.0.1 192.168.0.0/24 04:70:00:00:00:10)        ->     left_out_device;
center_in_device ->     classifier2[0]  ->      Print("c")      ->      ARPResponder(192.168.0.1 192.168.0.0/24 04:70:00:00:00:20)        ->     center_out_device;
right_in_device ->      classifier3[0]  ->      Print("d")      ->      ARPResponder(192.168.0.1 192.168.0.0/24 04:70:00:00:00:30)        ->     right_out_device;




// if this is an ip packet
// and it is dest host y -> rewrite the eth header now
// then send it to SSS
classifier0[1]  ->       chip    ->      ipclassifier[0] ->      encode;

// then these will be the encoded chunks
encode[0]      ->       EtherRewrite(04:70:00:00:00:10, 04:70:00:00:03:10)      ->      left_out_device;
encode[1]      ->       EtherRewrite(04:70:00:00:00:20, 04:70:00:00:03:20)      ->      center_out_device;
encode[2]      ->       EtherRewrite(04:70:00:00:00:30, 04:70:00:00:03:30)      ->      right_out_device;


// if the packet is coming over one of or other links, it means its already encodeed and ready to be decodeed.
classifier1[1]  ->      EtherRewrite(04:70:00:00:00:10, 04:70:00:00:03:10)      ->      chip1   ->      decode;
classifier2[1]  ->      EtherRewrite(04:70:00:00:00:20, 04:70:00:00:03:20)      ->      chip1   ->      decode;
classifier3[1]  ->      EtherRewrite(04:70:00:00:00:30, 04:70:00:00:03:30)      ->      chip1   ->      decode;

classifier0[2]  ->      Discard;
classifier1[2]  ->      Discard;
classifier2[2]  ->      Discard;
classifier3[2]  ->      Discard;

// swapped dst and src mack because once the packet is prepped, we want it to have
// our neighbors mac address as the dstination.
decode ->      EtherRewrite(04:70:00:00:02:01, 04:70:00:00:01:01)->      data_out;


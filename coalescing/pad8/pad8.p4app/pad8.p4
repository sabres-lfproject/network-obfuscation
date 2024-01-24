#include <core.p4>
#include <v1model.p4> /* either v1 or psa */

typedef bit<9>  egressSpec_t;

typedef bit<48> macAddr_t;

header ethernet_t {
    macAddr_t dstAddr;
    macAddr_t srcAddr;
    bit<16>   etherType;
}
#define ETH_SZ 14

#define DATA_LEN 100
// should be 1500
#define DATA_BITS 800
// DATA_LEN*8
header data_t {
	varbit<DATA_BITS> data;
}

header pad_t {
	//varbit<DATA_BITS> pad;
	bit<64> pad;
}

struct headers {
    ethernet_t   ethernet;
	data_t data;
	pad_t pad;
}

struct metadata {}

#define EIGHT 8

parser MyParser(packet_in packet,
                out headers hdr,
                inout metadata meta,
                inout standard_metadata_t standard_metadata) {

    state start {
        transition parse_ethernet;
    }
    state parse_ethernet {
        packet.extract(hdr.ethernet);
	packet.extract(hdr.data, EIGHT*(standard_metadata.packet_length - ETH_SZ));
	transition accept;
    }
}

control MyVerifyChecksum(inout headers hdr, inout metadata meta) {
    apply {  }
}

control MyIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {
	apply {
		standard_metadata.egress_spec = 0x03-standard_metadata.ingress_port;
		if (standard_metadata.packet_length < DATA_LEN) {
			hdr.pad.setValid();
			hdr.pad = {0x0102030405060708};
		}
	}
}

control MyEgress(inout headers hdr,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata) {

    action drop() {
        mark_to_drop(standard_metadata);
    }
	
    apply {
    }
}

control MyComputeChecksum(inout headers hdr, inout metadata meta) {
     apply {

    }
}

control MyDeparser(packet_out packet, in headers hdr) {
    apply {
        packet.emit(hdr.ethernet);
		packet.emit(hdr.data);
		packet.emit(hdr.pad);
    }
}

V1Switch(
	MyParser(),
	MyVerifyChecksum(),
	MyIngress(),
	MyEgress(),
	MyComputeChecksum(),
	MyDeparser()
) main;

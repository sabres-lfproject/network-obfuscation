#include <click/config.h>
// #include <click/TODO.hh>
#include "coalese.hh"

CLICK_DECLS

Coalese::Coalese() { };
Coalese::~Coalese() { };
#define ETH_HDR 14
#define IP_HDR 20

//WritablePacket *init_header(Packet *p) {
//	//q = Packet::make(p->headroom() + p->ip_header_offset(), 0, MTU, 0);
//	//q->set_ip_header((click_ip *)q->data(), 20);
//	memcpy(buff, p->ip_header(), 20); // changed v.1
//	//click_ip *q_iph = q->ip_header();
//	//q_iph->ip_p= IP_PROTO_IPIP;
//	return q;
//}

Packet *Coalese::simple_action(Packet *p) {
	uint32_t in_packet_length = p->network_length();
	WritablePacket *q;
	if (in_packet_length + out_packet_length >= MTU && out_packet_length > IP_HDR + ETH_HDR) {
		// before we ship
		// clear the unused portion of q
		q = Packet::make(p->headroom() + p->ip_header_offset(), buff, out_packet_length, 0); // changed v.1
		//q->take(MTU - out_packet_length);
		q->set_ip_header((click_ip *)q->data(), 20); // changed v.1
		click_ip *q_iph = q->ip_header(); // changed v.1
		//memcpy(q_iph, p->ip_header(), out_packet_length); // changed v.1
		q_iph->ip_p= IP_PROTO_IPIP; // changed v.1
		q_iph->ip_len = htons(q->network_length()); // TODO check if this is based on out_packet_length
		//click_chatter ("Coalese length %d %d", out_packet_length, q->network_length() );
		q_iph->ip_sum = 0;
		q_iph->ip_sum = click_in_cksum((const unsigned char *)q_iph, q_iph->ip_hl << 2);
		output(0).push(q); // ship q
		out_packet_length =0; // reset output length
		buff = new unsigned char[1500];
		memset(buff, 0, MTU); // changed v.1
	}

	if (out_packet_length ==0) { // not initialized
		out_packet_length = IP_HDR;
		memcpy(buff, p->ip_header(), 20); 
		// q=init_header(p); // add eth header and IP header
	}

	if (in_packet_length + out_packet_length < MTU) {
		memcpy(buff+out_packet_length, p->network_header() ,in_packet_length); // changed v.1
		out_packet_length += in_packet_length;
		p->kill();
		return 0;
	} else { // situation where incoming packet is of MTU length
		return p;
	}

	// TODO - include timer aspect
	// if timer expired run_timer will be called
		// run_timer will send packet q and 
		// will @call reschedule_after

	// otherwise within simple_action @call schedule_now when packet length limit is reached
	// reinitialize packet pointer q and out_packet_length
	// add incoming packet p to q as needed; drop p

};

CLICK_ENDDECLS
EXPORT_ELEMENT(Coalese)

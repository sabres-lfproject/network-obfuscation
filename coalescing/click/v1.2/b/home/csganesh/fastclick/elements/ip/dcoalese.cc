#include <click/config.h>
// #include <click/TODO.hh>
#include "dcoalese.hh"

CLICK_DECLS

Dcoalese::Dcoalese() { };
Dcoalese::~Dcoalese() { };
#define ETH_HDR 14
#define IP_HDR 20
#define MTU 1500

Packet *Dcoalese::simple_action(Packet *p) {
    WritablePacket *q;
    const click_ip *iph = p->ip_header();
    uint32_t in_packet_length = ntohs(iph->ip_len);
    bool ip_ip =  iph->ip_p== IP_PROTO_IPIP;
    click_chatter ("Dcoalese Type: %d %d,%d", ip_ip , iph->ip_len, in_packet_length);
    if (! ip_ip) return p;
    // strip ip header
    uint32_t offset = IP_HDR;
    uint32_t packet_length = 0;
    while (in_packet_length > offset) {
        // move to the next ip header
        //p->set_ip_header((click_ip *)iph+offset, IP_HDR);
        // extract length
        click_ip* iphf = (click_ip *) ((void*) iph+offset);
        packet_length = ntohs(iphf->ip_len);
    	click_chatter ("Dcoalese Offset: %d %d %p,%p,%p", offset, IP_HDR, iph, iphf, p->end_buffer());
        click_chatter ("Dcoalese Packet %d Type: %d %d", iphf, iphf->ip_p , packet_length);
	if (iphf->ip_p ==0 || packet_length ==0)
		break;
        q = Packet::make(ETH_HDR, 0, packet_length, 0);
	//q->set_mac_header(q->buffer()+q->headroom(),ETH_HDR);
	//q->set_ip_header((click_ip*)q->data()+ETH_HDR,IP_HDR);
        click_chatter ("Dcoalese headroom %p offest: %p %p", q->buffer(), q->mac_header(),q->ip_header());
	//memcpy(q->data(),p->mac_header(),ETH_HDR);
	memcpy(q->data(),iphf,packet_length);
        output(0).push(q);
        offset += packet_length;
    }

    p->kill();
    return 0;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(Dcoalese)

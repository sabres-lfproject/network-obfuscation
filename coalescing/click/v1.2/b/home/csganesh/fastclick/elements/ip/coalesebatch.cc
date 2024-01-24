#include <click/config.h>
// #include <click/TODO.hh>
#include "coalesebatch.hh"

CLICK_DECLS

CoaleseBatch::CoaleseBatch() { };
CoaleseBatch::~CoaleseBatch() { };
#define ETH_HDR 14
#define IP_HDR 20
#define PACKET_END 128
#define PACKET_START 64

void CoaleseBatch::process_packet (Packet *p) {
    if ((PAINT_ANNO(p) & (PACKET_START|PACKET_END)) ==(PACKET_START|PACKET_END)) {
        click_chatter ("CB: start and end %d", out_packet_length);
        if (p) output(0).push(p);
        return;
    }
    if ((PAINT_ANNO(p) & PACKET_START) >0) {
        out_packet_length = IP_HDR;
        //buff = new unsigned char[1500];
        memset(buff, 0, MTU);
        click_chatter ("CB: start and copy %d", out_packet_length);
        //memset(buff, 0, MTU);
        memcpy(buff, p->ip_header(), 20);
    }
    memcpy(buff+out_packet_length, p ,p->length());
    out_packet_length += p->length();
    click_chatter ("CB: copy payload %d", out_packet_length);
    if ((PAINT_ANNO(p) & PACKET_END) >0) {
        WritablePacket *q = Packet::make(ETH_HDR+4, buff, out_packet_length, 0);
        q->set_ip_header((click_ip *)q->data(), 20);
        click_ip *q_iph = q->ip_header();
        q_iph->ip_len = htons(q->network_length());
        q_iph->ip_sum = 0;
        q_iph->ip_sum = click_in_cksum((const unsigned char *)q_iph, q_iph->ip_hl << 2);
        click_chatter ("CB: end send %d", out_packet_length);
        if (q) output(0).push(q); 
        out_packet_length = IP_HDR;
        //buff = new unsigned char[1500];
        memset(buff, 0, MTU);
        click_chatter ("CB: start and copy %d", out_packet_length);
        //memset(buff, 0, MTU);
        memcpy(buff, p->ip_header(), 20);
    }
    //p->kill();
    return;
}

Packet *CoaleseBatch::simple_action(Packet *p) {
    process_packet(p);
    return 0;
}

PacketBatch* CoaleseBatch::simple_action_batch(PacketBatch *batch) {
    FOR_EACH_PACKET_SAFE(batch, p) {
        process_packet (p);
    }
    return 0;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CoaleseBatch)

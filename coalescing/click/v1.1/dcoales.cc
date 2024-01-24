#include <click/config.h>
// #include <click/TODO.hh>
#include "dcoales.hh"
#include <click/args.hh>

CLICK_DECLS

#define SEED 13
#define data_length 4
Dcoales::Dcoales()
{
    prev=113;
    click_srandom(_seed);
}

Dcoales::~Dcoales () { };
#define ETH_HDR 14
#define IP_HDR 20
#define MTU 1500

int
Dcoales::configure(Vector<String> &conf, ErrorHandler *errh) // TODO consolidate functionality
{
    DEBUG_CHATTER ("DC: IN configure");

    if (Args(conf, this, errh)
        .read_p("OBFUSCATE", _obfus)
        .read_p("DEBUG", _debug)
        .read_p("OBFUSCATE_LENGTH", _data_length)
        .read_p("SEED", _seed)
        .complete() < 0)
        return -1;
    return 0;
}

Packet *
Dcoales::obfuscate(Packet *p, uint32_t obfuscate_offset)
{
  if (!_obfus) return p;
  WritablePacket *q = p->uniqueify();
  uint32_t data = 0;
  memcpy(&data,q->data()+obfuscate_offset,_data_length);
  // check if ntoh is needed
  prev = prev * _seed;
  uint32_t rnd = prev;
  uint32_t data1=data ^ rnd;
  DEBUG_CHATTER ("data %d rnd %d d1 %d ", data, rnd, data1);
  memcpy(q->data()+obfuscate_offset,&data1,_data_length);
  return q;
}

Packet *Dcoales::simple_action(Packet *p) {
    WritablePacket *q;
    Packet *dobfus;
    const click_ip *iph = p->ip_header();
    uint32_t in_packet_length = ntohs(iph->ip_len);
    bool ip_ip =  iph->ip_p== IP_PROTO_IPIP;
    DEBUG_CHATTER ("Dcoales  Type: %d %d,%d", ip_ip , iph->ip_len, in_packet_length);
    // strip ip header
    uint32_t offset = IP_HDR;
    uint32_t packet_length = 0;
    while (in_packet_length > offset) {
        // move to the next ip header
        //p->set_ip_header((click_ip *)iph+offset, IP_HDR);
        // extract length
        if (in_packet_length-offset >=_data_length)
            dobfus = obfuscate(p,offset); // deobfuscate
        else
            break; // must be padding
        click_ip* iphf = (click_ip *) ((void*) dobfus->ip_header()+offset);
        packet_length = ntohs(iphf->ip_len);
    	DEBUG_CHATTER ("Dcoales  Offset: %d %d %p,%p,%p", offset, IP_HDR, iph, iphf, p->end_buffer());
        DEBUG_CHATTER ("Dcoales  Packet %d Type: %d %d", iphf, iphf->ip_p , packet_length);
        if (iphf->ip_p ==0 || iphf->ip_v ==0 || packet_length ==0)
            break;
        q = Packet::make(0, 0, packet_length, 0);
	//q->set_mac_header(q->buffer()+q->headroom(),ETH_HDR);
	//q->set_ip_header((click_ip*)q->data()+ETH_HDR,IP_HDR);
        DEBUG_CHATTER ("Dcoales  headroom %p offest: %p %p", q->buffer(), q->mac_header(),q->ip_header());
	//memcpy(q->data(),p->mac_header(),ETH_HDR);
	memcpy(q->data(),iphf,packet_length);
        output(0).push(q);
        offset += packet_length;
    }

    p->kill();
    return 0;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(Dcoales)

#ifndef CLICK_COALESEBATCH__HH
#define CLICK_COALESEBATCH__HH

#include <click/batchelement.hh>

CLICK_DECLS

/*
=c

CoaleseBatch(TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
#define MTU 1500
class CoaleseBatch : public BatchElement {
	//TODO: Add private attributes
	unsigned char* buff = new unsigned char[MTU];
    uint32_t out_packet_length=0;

	public:
		CoaleseBatch();
		~CoaleseBatch();

		const char *class_name() const { return "CoaleseBatch"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return PUSH; }


    PacketBatch *simple_action_batch(PacketBatch *);
    Packet *simple_action(Packet *);
	void process_packet (Packet *p);

};

CLICK_ENDDECLS

#endif

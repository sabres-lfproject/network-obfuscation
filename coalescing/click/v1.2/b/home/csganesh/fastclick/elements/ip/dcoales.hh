#ifndef CLICK_DCOALES__HH
#define CLICK_DCOALES__HH

#include <click/element.hh>

CLICK_DECLS

/*
=c

Dcoales (TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
class Dcoales  : public Element {
	//TODO: Add private attributes
	uint32_t prev=113;
	bool _obfus=true, _pad=true;
	bool _debug=false;
	uint32_t _data_length=4, _seed=13;

	public:
		Dcoales ();
		~Dcoales ();

		const char *class_name() const { return "Dcoales "; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return PUSH; }
		Packet *obfuscate(Packet *p, uint32_t obfuscate_offset);
		int configure(Vector<String> &conf, ErrorHandler *errh);

		Packet *simple_action(Packet *p);
};
#define DEBUG_CHATTER if (_debug) click_chatter

CLICK_ENDDECLS

#endif

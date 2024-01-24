#ifndef CLICK_DCOALESE__HH
#define CLICK_DCOALESE__HH

#include <click/element.hh>

CLICK_DECLS

/*
=c

Dcoalese(TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
class Dcoalese : public Element {
	//TODO: Add private attributes

	public:
		Dcoalese();
		~Dcoalese();

		const char *class_name() const { return "Dcoalese"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return PUSH; }

		Packet *simple_action(Packet *p);
};

CLICK_ENDDECLS

#endif

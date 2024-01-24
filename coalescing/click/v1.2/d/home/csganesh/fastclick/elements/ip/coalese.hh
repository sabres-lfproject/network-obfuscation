#ifndef CLICK_COALESE__HH
#define CLICK_COALESE__HH

#include <click/element.hh>

CLICK_DECLS

/*
=c

Coalese(TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
#define MTU 1500
class Coalese : public Element {
	//TODO: Add private attributes
	unsigned char* buff = new unsigned char[MTU];

	public:
		Coalese();
		~Coalese();

		const char *class_name() const { return "Coalese"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return PUSH; }

		Packet *simple_action(Packet *p);
};

CLICK_ENDDECLS

#endif

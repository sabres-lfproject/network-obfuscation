#ifndef CLICK_PADDING_HH
#define CLICK_PADDING_HH

#include <click/element.hh>
#include <click/glue.hh>

CLICK_DECLS

/* Padding */

#define MTU 1500
class Padding : public Element {
    unsigned char* buff = new unsigned char[2*MTU];
    unsigned _nbytes;
    public:
        Padding();
        ~Padding();
        int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;
		const char *class_name() const { return "Padding"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return PUSH; }
    Packet *simple_action(Packet *);
    int initialize(ErrorHandler *);
};

CLICK_ENDDECLS

#endif

#ifndef CLICK_OBFUSCAT_HH
#define CLICK_OBFUSCAT_HH
#include <click/element.hh>
CLICK_DECLS


class Obfuscat : public Element { public:

  Obfuscat() CLICK_COLD;

  const char *class_name() const override	{ return "Obfuscat"; }
  const char *port_count() const override	{ return PORTS_1_1; }

  Packet *simple_action(Packet *);
  uint32_t prev=113;

};


CLICK_ENDDECLS
#endif

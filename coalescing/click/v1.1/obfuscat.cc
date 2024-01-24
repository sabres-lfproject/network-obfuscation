#include <click/config.h>
#include "obfuscat.hh"
CLICK_DECLS

#define SEED 13
Obfuscat::Obfuscat()
{
    prev=113;
    click_srandom(SEED);
}

#define offset 0
#define data_length 4
Packet *
Obfuscat::simple_action(Packet *p)
{
  WritablePacket *q = p->uniqueify();
  uint32_t data = 0;
  memcpy(&data,q->data()+offset,data_length);
  // check if ntoh is needed
  prev = prev * SEED;
  uint32_t rnd = prev;
  uint32_t data1=data ^ rnd;
  click_chatter ("data %d rnd %d d1 %d ", data, rnd, data1);
  memcpy(q->data()+offset,&data1,data_length);
  return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(Obfuscat)

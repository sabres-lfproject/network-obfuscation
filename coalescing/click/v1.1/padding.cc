#include <click/config.h>
#include <click/args.hh>
#include <fcntl.h>
// #include <click/TODO.hh>
#include "padding.hh"

CLICK_DECLS

Padding::Padding() {};
Padding::~Padding() {};

int
Padding::configure(Vector<String>& conf, ErrorHandler* errh)
{
    _nbytes = 0;
    return Args(conf, this, errh)
        .read_p("LENGTH", _nbytes)
        .complete();

    return 0;
}

int Padding::initialize (ErrorHandler *) {
    int fd = open("/dev/random", O_RDONLY);
    read(fd, buff, 2*MTU);
    return 0;
}
#define PHEADROOM 18
Packet *Padding::simple_action(Packet *p) {
    WritablePacket *q = nullptr;
    uint32_t fill_len = _nbytes - (PHEADROOM + p->length());
    const char zero = 0;
    click_chatter("Padding nb %d fill %d hd %d len %d", _nbytes, fill_len, PHEADROOM, p->length());
    //uint32_t tail_len = MTU - (PHEADROOM + p->length() + p->tailroom());
    uint32_t offset = p->length();
    if (p->length()+ PHEADROOM < _nbytes) {
        if (fill_len > 0)
            q = p->put(fill_len);
        //else
            //*q = p->uniqueify();
        // pad required
        uint32_t rnd = click_random(0,_nbytes);
        memcpy(q->data()+offset,buff+rnd,fill_len);
        if (fill_len >= 1) // set ip version field to 0 to remove padding
            memcpy(q->data()+offset,&zero,1);
        //q->set_ip_header((click_ip *)q->data(), 20);
        //click_ip *q2 = q->data()+offset;
        //q2->ip_v =0; // so that we can remove the padding
        q->set_ip_header((click_ip *)q->data(), 20);
        click_ip *q1 = q->ip_header();
        q1->ip_len = htons(q->length());
        q1->ip_sum = 0;
        q1->ip_sum = click_in_cksum((const unsigned char *)q1, q1->ip_hl << 2);

    } else
        return p;
    return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(Padding)

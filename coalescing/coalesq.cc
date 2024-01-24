#include <click/config.h>
#include <fcntl.h>
#include <click/args.hh>
#include "coalesq.hh"
CLICK_DECLS

#define SEED 13
#define PHEADROOM 18
Coalesq::Coalesq()
{
    prev=113;
    click_srandom(_seed);
}

Coalesq::~Coalesq() { };


/*#define obfuscate_offset 0*/
#define data_length 4
int Coalesq::initialize (ErrorHandler *) {
    /* From Padding */
    int fd = open("/dev/random", O_RDONLY);
    read(fd, rbuff, 2*MTU);
    /* From Timed Batch */
    DEBUG_CHATTER ("CL: IN init");
    _timer.initialize(this);
    if (_interval >0) {
        _timer.schedule_after(_stamp);
        DEBUG_CHATTER ("timeout configured delta %d us time:%d", _stamp.usec(), Timestamp::now().usec());
    }
    _tsq = static_cast<ThreadSafeQueue *>(input(0).element());
    DEBUG_CHATTER ("CL: OUT init");

    return 0;
}

int
Coalesq::configure(Vector<String> &conf, ErrorHandler *errh) // TODO consolidate functionality
{
    DEBUG_CHATTER ("CL: IN configure");
    _nbytes = 0;

    if (Args(conf, this, errh)
        .read_p("LENGTH", _nbytes)
        .read_p("NPKTS", _npkts)
        .read_p("BURST", _burst)
        .read_p("TIMER", _interval)
        .read_p("PAD", _pad)
        .read_p("OBFUSCATE", _obfus)
        .read_p("OBFUSCATE_LENGTH", _data_length)
        .read_p("DEBUG", _debug)
        .read_p("SEED", _seed)
        .complete() < 0)
        return -1;
    _stamp = Timestamp::make_usec(0,_interval);
    DEBUG_CHATTER ("CL: OUT configure b=%d i=%d", _burst, _stamp.usec());
    return 0;
}

#define ETH_HDR 14
#define IP_HDR 20
#define PACKET_END 128
#define PACKET_START 64

void Coalesq::process_packet (Packet *p, bool start, bool end) {
    DEBUG_CHATTER ("CL : proc pkt: pkt=%p start=%d end=%d",p,start,end);
    if (_npkts == 1) { // for just padding of packets one by one
	    start =true;
	    end = true;
    }
/*     if (start && end) {
        DEBUG_CHATTER ("CB: start and end %d", out_packet_length);
        // do we need to pad ?? this is causing obfuscation desynchronization
        output(0).push(padding(p));
        return;
    } */
    if (start) {
        out_packet_length = IP_HDR;
        //buff = new unsigned char[1500];
        memset(buff, 0, MTU);
        DEBUG_CHATTER ("CB: start and copy %d", out_packet_length);
        //memset(buff, 0, MTU);
        memcpy(buff, p->ip_header(), 20);
        DEBUG_CHATTER("after memcpy 5 %x", p->ip_header());
    }
    uint32_t plen = p->length();
    memcpy(buff+out_packet_length, obfuscate(p,0)->data() ,plen);
    //memcpy(buff+out_packet_length, p->data() ,plen);
    DEBUG_CHATTER("after memcpy 6 %x", buff+out_packet_length);
    out_packet_length += plen;
    DEBUG_CHATTER ("CB: copy payload %d", out_packet_length);
    if (end) {
        WritablePacket *q = Packet::make(PHEADROOM, buff, out_packet_length, MTU-out_packet_length);
        q->set_ip_header((click_ip *)q->data(), 20);
        click_ip *q_iph = q->ip_header();
        q_iph->ip_len = htons(out_packet_length);
        
        q_iph->ip_sum = 0;
        q_iph->ip_sum = click_in_cksum((const unsigned char *)q_iph, q_iph->ip_hl << 2);
        DEBUG_CHATTER ("CB: end send %d", out_packet_length);
        output(0).push(padding(q));
        out_packet_length = IP_HDR;
        //buff = new unsigned char[1500];
        memset(buff, 0, MTU);
        DEBUG_CHATTER ("CB: start and copy %d", out_packet_length);
        //memset(buff, 0, MTU);
        memcpy(buff, q->ip_header(), 20);
        DEBUG_CHATTER("after memcpy 7 %x", buff);
    }
    p->kill();
    return;
}

Packet *
Coalesq::obfuscate(Packet *p, uint32_t obfuscate_offset)
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
  DEBUG_CHATTER("after memcpy 1 %x", q->data()+obfuscate_offset);
  return q;
}


Packet *Coalesq::padding(Packet *p) {
    if (!_pad) return p;
    WritablePacket *q = nullptr;
    uint32_t fill_len = _nbytes - (PHEADROOM + p->length());
    const char zero = 0;
    DEBUG_CHATTER("Padding nb %d fill %d hd %d len %d", _nbytes, fill_len, PHEADROOM, p->length());
    //uint32_t tail_len = MTU - (PHEADROOM + p->length() + p->tailroom());
    uint32_t offset = p->length();
    if (p->length()+ PHEADROOM < _nbytes) {
        if (fill_len > 0)
            q = p->put(fill_len);
        //else
            //*q = p->uniqueify();
        // pad required
        uint32_t rnd = click_random(0,_nbytes);
        memcpy(q->data()+offset,rbuff+rnd,fill_len);
        DEBUG_CHATTER("after memcpy 2 %x", q->data()+offset);
        if (fill_len >= 1) // set ip version field to 0 to remove padding
            memcpy(q->data()+offset,&zero,1);
        DEBUG_CHATTER("after memcpy 3 %x", q->data()+offset);
        //q->set_ip_header((click_ip *)q->data(), 20);
        //click_ip *q2 = q->data()+offset;
        //q2->ip_v =0; // so that we can remove the padding
        q->set_ip_header((click_ip *)q->data(), 20);
        click_ip *q1 = q->ip_header();
        q1->ip_len = htons(q->length());
        q1->ip_sum = 0;
        q1->ip_sum = click_in_cksum((const unsigned char *)q1, q1->ip_hl << 2);
        if (fill_len >= _data_length)
          obfuscate(q,offset);

        DEBUG_CHATTER("after memupd 4 %x", q1);

    } else
        return p;
    return q;
}

#define PACKET_END 128
#define PACKET_START 64
#define ETH_SZ 14
#define BASE_SZ 20
#define MTU 1500
/* SET_PAINT_ANNO(p,v) - SET*/

void Coalesq::run_timer(Timer* ) {
    int pcount = _tsq->size();
    DEBUG_CHATTER ("In CL::Runtask batch size=%d",pcount);
    if (pcount ==0 && _interval > 0) {
        _timer.schedule_after(_stamp);
        DEBUG_CHATTER ("timeout configured delta %d us time:%d", _stamp.usec(), Timestamp::now().usec());
        return;
    } // TODO optimization by listening to notification

    // send each packet FOR_EACH_PACKET_SAFE
    bool start =true, sendflag = false;
    uint32_t len =0, plen =0;
    Packet *prev = nullptr;
    int popcount =0;
    bool prevstart = true, prevend = false;
    DEBUG_CHATTER("CL:: run timer:: packet count %d \n",pcount);
    DEBUG_CHATTER("CL:: run timer:: 10\n");
    for(Packet *pkt = nullptr;pcount>0;pcount--) { // this always sends prev packet
        pkt=input(0).pull();
        plen = pkt->length();
        if (len + plen < MTU) {
            len += plen;
        } else {
            sendflag = true;
        } 

        if (sendflag || plen == MTU) {
            //SET_PAINT_ANNO(prev,PAINT_ANNO(prev) | PACKET_END); // Todo add the flag 
            prevend=sendflag;
            DEBUG_CHATTER ("send count=%d pkt==%p send=%d, computed len=%d, start=%d anno=%d\n", pcount, prev,sendflag,len,start, PAINT_ANNO(prev));
            process_packet(prev, prevstart, sendflag);
            sendflag = (plen == MTU)? true:false;
            len =BASE_SZ+plen; start = true;
            DEBUG_CHATTER("CL:: run timer:: 11 count=%d \n",pcount);
        } else {
            if (prev != nullptr && prev->length() > BASE_SZ) {
                process_packet(prev, prevstart, sendflag);
                DEBUG_CHATTER("CL:: run timer:: 12 count=%d \n",pcount);
            }
        }
        DEBUG_CHATTER("CL:: run timer:: 13 count=%d \n",pcount);
        if (start) {
            //SET_PAINT_ANNO(pkt,PACKET_START);
            prevstart=true;
            start = false;
        } else
            prevstart=false;
        DEBUG_CHATTER("CL:: run timer:: 14 count=%d \n",pcount);

        if (len >= MTU) { // check if this is needed
            // mark end
            len =BASE_SZ; start = true; sendflag = true;
        }
        DEBUG_CHATTER("CL:: run timer:: 15 count=%d \n",pcount);
        prev = pkt; 
        DEBUG_CHATTER ("context count=%d pkt==%p send=%d, computed len=%d, start=%d anno=%d\n", pcount, prev,sendflag,len,start, PAINT_ANNO(prev));
    }
    //SET_PAINT_ANNO(prev,PAINT_ANNO(prev) | PACKET_END);
    prevend=true;
    if (prev) {
        DEBUG_CHATTER("CL:: run timer:: 16.1 count=%d \n",pcount);
        process_packet(prev, prevstart, true);
        DEBUG_CHATTER("CL:: run timer:: 16.2 count=%d \n",pcount);
        
    }

    if (_interval > 0) {
        _timer.schedule_after(_stamp);
        DEBUG_CHATTER ("timeout configured delta %d us time:%d", _stamp.usec(), Timestamp::now().usec());
    }
    return;
}

Packet* Coalesq::simple_action(Packet *p) {
    DEBUG_CHATTER ("In CL::simple action");
/*     if (last_batch == nullptr) {
        DEBUG_CHATTER ("In CL::simple action null");
        last_batch =  PacketBatch::make_from_packet(p);;
    } else {
        DEBUG_CHATTER ("In CL::simple action !null");
        last_batch->append_packet(p);
    } */
    DEBUG_CHATTER ("Out CL::simple action");
    return 0;
}
CLICK_ENDDECLS
EXPORT_ELEMENT(Coalesq)

/*
 * minbatch.{cc,hh}
 */
#include <click/config.h>
#include "timedbatch.hh"
#include <click/glue.hh>
#include <click/args.hh>
#include <click/packet.hh>
#include <click/packet_anno.hh>
#include <click/master.hh>


CLICK_DECLS

TimedBatch::TimedBatch() : _burst(32),_interval(1000000000) {
    //in_batch_mode = BATCH_MODE_NEEDED;
    click_chatter ("TB: IN ctor");
}

#define NANO2SEC 1000000000

int
TimedBatch::configure(Vector<String> &conf, ErrorHandler *errh)
{
    click_chatter ("TB: IN configure");
    if (Args(conf, this, errh)
        .read_p("BURST", _burst)
        .read_p("TIMER", _interval)
        .complete() < 0)
        return -1;
    _stamp = Timestamp::make_usec(0,_interval);
    click_chatter ("TB: OUT configure b=%d i=%d", _burst, _stamp.usec());

/*     if (_interval >= 0) {
        //for (unsigned i = 0; i < click_max_cpu_ids(); i++) {
            State &s = _state;
            Task* task = new Task(this);
            task->initialize(this,true);
            //task->move_thread(i);
            timer = new Timer(task);
            timer->initialize(this);
            timer->reschedule_after_msec(_interval);
            click_chatter ("timeout configured delta %d ms", _interval);
            //timer->move_thread(i);
        //}
    } */

    return 0;
}

int
TimedBatch::initialize(ErrorHandler *)
{
    click_chatter ("TB: IN init");
    _timer.initialize(this);
    if (_interval >0) {
        _timer.schedule_after(_stamp);
        click_chatter ("timeout configured delta %d us time:%d", _stamp.usec(), Timestamp::now().usec());
    }
    click_chatter ("TB: OUT init");
  return 0;
}

// using top two bits for packet start and packet end annotations from paint
#define PACKET_END 128
#define PACKET_START 64
#define ETH_SZ 14
#define BASE_SZ 20
#define MTU 1500
/* SET_PAINT_ANNO(p,v) - SET*/

void TimedBatch::run_timer(Timer* ) {
    click_chatter ("In TB::Runtask");
     if (!last_batch) {
        if (_interval > 0) {
            _timer.schedule_after(_stamp);
            click_chatter ("timeout configured delta %d us time:%d", _stamp.usec(), Timestamp::now().usec());
        }
        return;
    }

    PacketBatch* p = last_batch;
    // send each packet FOR_EACH_PACKET_SAFE
    bool start =true, sendflag = false;
    uint32_t len =0, plen =0;
    Packet *prev = nullptr;
    FOR_EACH_PACKET_SAFE(p, pkt) { // this always sends prev packet
        plen = pkt->length();
        if (len + plen < MTU) {
            len += plen;
        } else {
            sendflag = true;
        } 

        if (sendflag || plen == MTU) {
            SET_PAINT_ANNO(prev,PAINT_ANNO(prev) | PACKET_END); // Todo add the flag 
            click_chatter ("send pkt==%p send=%d, computed len=%d, start=%d anno=%d\n", prev,sendflag,len,start, PAINT_ANNO(prev));
            output(0).push(prev);
            p->pop_front();
            sendflag = (plen == MTU)? true:false;
            len =BASE_SZ+plen; start = true;
        } else {
            if (prev != nullptr && prev->length() > BASE_SZ) {
                output(0).push(prev); // any way send
                p->pop_front();
            }
        }

        if (start) {
            SET_PAINT_ANNO(pkt,PACKET_START);
            start = false;
        }

        if (len >= MTU) { // check if this is needed
            // mark end
            len =BASE_SZ; start = true; sendflag = true;
        }
        prev = pkt;
        click_chatter ("context pkt==%p send=%d, computed len=%d, start=%d anno=%d\n", prev,sendflag,len,start, PAINT_ANNO(prev));
        

    }
    SET_PAINT_ANNO(prev,PAINT_ANNO(prev) | PACKET_END);
    if (prev) {
        p->pop_front();
        output(0).push(prev);
    }

    //output_push_batch(0,p);
    //last_batch = nullptr;
    if (_interval > 0) {
        _timer.schedule_after(_stamp);
        click_chatter ("timeout configured delta %d us time:%d", _stamp.usec(), Timestamp::now().usec());
    }
    return;
}

Packet* TimedBatch::simple_action(Packet *p) {

    click_chatter ("In TB::simple action");
     if (last_batch == nullptr) {
        click_chatter ("In TB::simple action null");
        last_batch =  PacketBatch::make_from_packet(p);;
    } else {
        click_chatter ("In TB::simple action !null");
        last_batch->append_packet(p);
    }
    click_chatter ("Out TB::simple action");
    return 0;
}

void TimedBatch::push_batch(int port, PacketBatch *p) {

    click_chatter ("In TB::push batch");
     if (last_batch == nullptr) {
        last_batch = p;
    } else {
        last_batch->append_batch(p);
    }

/*    if (last_batch->count() < _burst) {
        if (_interval >= 0) {
            timer->schedule_after(Timestamp::make_usec(_interval));
        }
     } else {
        if (_interval >= 0)
            timer->unschedule();

        p = last_batch;
        last_batch = nullptr;       
        output_push_batch(0,p);
    } */
}

CLICK_ENDDECLS
//ELEMENT_REQUIRES(batch)
EXPORT_ELEMENT(TimedBatch)

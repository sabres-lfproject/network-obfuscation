#ifndef CLICK_TIMEDBATCH_HH
#define CLICK_TIMEDBATCH_HH

#include <click/string.hh>
#include <click/timer.hh>
#include <click/batchelement.hh>
#include <vector>

CLICK_DECLS

/**
 * Ensure that patch going through have a minimal size, if not
 * it waits for more packet, using a timeout to limit the amount of waiting time.
 */
class TimedBatch: public BatchElement { public:

    TimedBatch() CLICK_COLD;

    const char *class_name() const override		{ return "TimedBatch"; }
    const char *port_count() const override		{ return "1/1"; }
    const char *processing() const override		{ return PUSH; }

    int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;
    bool can_live_reconfigure() const		{ return false; }
    Packet* simple_action(Packet *p);

    //bool run_task(Task *task);
    void run_timer(Timer* );
    int initialize(ErrorHandler *);

    void push_batch(int port, PacketBatch *p);

 private:

/*     class State {
    public:
        State() : last_batch(0), timers(0) {};

        PacketBatch* last_batch;
        Timer*  timers;
    };

    State _state;
 */
    PacketBatch* last_batch = nullptr;
    unsigned _burst;
    uint32_t _interval;
    Timer _timer;
    Timestamp _stamp;

};
#endif
CLICK_ENDDECLS

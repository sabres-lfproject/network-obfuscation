#ifndef CLICK_COALESQ_HH
#define CLICK_COALESQ_HH
#include <click/element.hh>
#include <elements/standard/threadsafequeue.hh>
#include <click/timer.hh>
CLICK_DECLS

#define MTU 1500
class Coalesq : public Element { public:

  Coalesq() CLICK_COLD;
	~Coalesq();


  const char *class_name() const override	{ return "Coalesq"; }
  const char *port_count() const override	{ return PORTS_1_1; }
  const char *processing() const override		{ return "l/h"; }
  bool can_live_reconfigure() const		{ return false; }
  void run_timer(Timer* );
  int initialize(ErrorHandler *);

  int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;

  Packet *simple_action(Packet *);
  Packet *obfuscate(Packet *p, uint32_t obfuscate_offset);
  Packet *padding(Packet *);
  void process_packet(Packet *, bool , bool ); // from coalese batch
  
  
  private:
    ThreadSafeQueue* _tsq;
    uint32_t prev=113; // obfuscate
    unsigned char* rbuff = new unsigned char[2*MTU]; // padding
    unsigned _nbytes=MTU; // padding

    unsigned char* buff = new unsigned char[MTU]; // coalesebatch
    uint32_t out_packet_length=0; // coalesebatch

    /* Timed batch */
    PacketBatch* last_batch = nullptr; 
    unsigned _burst;
    uint32_t _interval;
    Timer _timer;
    Timestamp _stamp;
    bool _obfus=true, _pad=true;
    bool _debug=false;
    uint32_t _data_length=4, _seed=13;

};

#define DEBUG_CHATTER if (_debug) click_chatter

CLICK_ENDDECLS
#endif

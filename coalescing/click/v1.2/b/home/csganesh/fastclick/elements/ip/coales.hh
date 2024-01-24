#ifndef CLICK_COALES_HH
#define CLICK_COALES_HH
#include <click/element.hh>
#include <click/timer.hh>
#include <click/sync.hh>
CLICK_DECLS

#define MTU 1400
class Coales : public Element { public:

  Coales() CLICK_COLD;
	~Coales();


  const char *class_name() const override	{ return "Coales"; }
  const char *port_count() const override	{ return PORTS_1_1; }
  const char *processing() const override		{ return PUSH; }
  bool can_live_reconfigure() const		{ return false; }
  void run_timer(Timer* );
  int initialize(ErrorHandler *);

  int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;

  Packet *simple_action(Packet *);
  Packet *obfuscate(Packet *p, uint32_t obfuscate_offset);
  Packet *padding(Packet *);
  void process_packet(Packet *, bool , bool ); // from coalese batch
  
  
  private:
    uint32_t prev=113; // obfuscate
    unsigned char* rbuff = new unsigned char[2*MTU]; // padding
    unsigned _nbytes=MTU; // padding
    Spinlock* _lock;

    unsigned char* buff = new unsigned char[2*MTU]; // coalesebatch
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

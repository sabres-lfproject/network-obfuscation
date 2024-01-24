InfiniteSource(LIMIT 12, STOP false, LENGTH 400)
  -> r :: RoundRobinSwitch
  -> ICMPIPEncap(1.0.0.1, 1.0.0.1, TYPE 8, CODE 0) -> Print(IN, 640)
  -> p :: Coales (LENGTH 1500, TIMER 100,  PAD true, OBFUSCATE true) -> Print(COAL, 640)
  -> Dcoales() -> Print(DCOAL, 640) -> CheckIPHeader()
  -> ToIPSummaryDump(-, FIELDS ip_src ip_dst ip_proto icmp_type icmp_code ip_len);
r[1] -> ICMPIPEncap(1.0.0.2, 1.0.0.2, TYPE 3, CODE 1) -> Print(IN, 640) -> p;
r[2] -> ICMPIPEncap(1.0.0.3, 1.0.0.3, TYPE 5, CODE 3) -> Print(IN, 640) -> p;
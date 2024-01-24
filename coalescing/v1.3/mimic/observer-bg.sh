#export yMIMIC_FILE_LIST="/click/NonVPN/test-pcap-list"
export yMIMIC_FILE_LIST="/click/NonVPN/pcap-list"
export yMIMIC_IN_DIR="/click/NonVPN/mimic-extract/"
export yMIMIC_OUT_DIR="/click/coalescing/pcap"
export yMIMIC_INFORM_HOSTS="x,y"
export yMIMIC_MONITOR_HOST="v"
export yMIMIC_INFORM_PORTS="3333,3333"
export yMIMIC_MONITOR_PORT=3333
export yMIMIC_LISTEN_PORTS="3333,3334"
echo arguments $@ aa $*
./mimic-tcpdump.py $@

#export yMIMIC_FILE_LIST="/click/NonVPN/test-pcap-list"
export yMIMIC_FILE_LIST="/click/NonVPN/pcap-list"
export yMIMIC_IN_DIR="/click/NonVPN/mimic-extract/"
export yMIMIC_OUT_DIR="/tmp"
export yMIMIC_INFORM_HOSTS="t"
export yMIMIC_MONITOR_HOST="v"
export yMIMIC_INFORM_PORT=3333
export yMIMIC_MONITOR_PORT=3333
export yMIMIC_LISTEN_PORTS=3333
export yMIMIC_END_ARGS=${*:2}
echo mimic end args $yMIMIC_END_ARGS

./mimic-server.py $@

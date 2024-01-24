#!/bin/bash

topo=`cat .rvn/topo.json | jq '.name' | cut -f 2 -d '"'`
nodes=`cat .rvn/topo.json | jq '.nodes[].name'`

vir_nodes=()
for node in $nodes; do
	tmp=`echo $node | cut -f 2 -d '"'`;
	vir_nodes+=(`echo "$topo"_"$tmp"`);
done

#echo "${vir_nodes[@]}"

net_devs=()
for vr in ${vir_nodes[@]}; do
	net_devs+=(`sudo virsh dumpxml $vr | /usr/local/bin/xq '.domain.devices.interface | .[].source | select(."@network" | endswith("infra") | not) | ."@bridge"'`)
	#echo "$vr ${vir_nets[@]}"
	net_devs+=(`sudo virsh dumpxml $vr | /usr/local/bin/xq | jq '.domain.devices.interface | del(.[0]) | .[].target."@dev"'`)
done

for nd in ${net_devs[@]}; do
	iface=`echo $nd | cut -d '"' -f 2`
	sudo ethtool -K $iface tso off ufo off gso off gro off lro off sg off rx off tx off
done

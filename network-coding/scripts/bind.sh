#!/bin/bash

for x in /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages /sys/devices/system/node/node?/hugepages/hugepages-2048kB/nr_hugepages
do
  echo 400 > $x
done

[ ! -d /mnt/huge ] && mkdir -p /mnt/huge
mount -t hugetlbfs nodev /mnt/huge

modprobe vfio-pci
echo 1 > /sys/module/vfio/parameters/enable_unsafe_noiommu_mode

let c=$(ip link | grep eth[0-9] |wc -l)
let i=1

while [[ i -lt c ]]; do
        echo "configure eth$i"
        ip link set dev eth$i down
        let i=$i+1
done

let i=1

while [[ i -lt c ]]; do
        businfo=`ethtool -i eth$i | grep bus-info | cut -f 3-4 -d ":"`
        echo "configuring ${businfo}"
        python dpdk-devbind.py -b vfio-pci "${businfo}"
        let i=$i+1
done

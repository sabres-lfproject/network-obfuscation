#!/bin/bash

let i=1

while [[ i -lt c ]]; do
        businfo=`ethtool -i eth$i | grep bus-info | cut -f 3-4 -d ":"`
        echo "unbinding ${businfo} from dpdk back to linux e1000 driver"
        python dpdk-devbind.py -b e1000 "${businfo}"
        let i=$i+1
done


pcibuses=`python dpdk-devbind.py --status | grep -B 10 "Network devices using kernel driver" | grep drv | cut -d " " -f 1`
for bus in ${pcibuses[@]}; do
  echo "unbinding ${businfo} from dpdk back to linux e1000 driver"
  python dpdk-devbind.py -u "${businfo}"
  python dpdk-devbind.py -b e1000 "${businfo}"
done

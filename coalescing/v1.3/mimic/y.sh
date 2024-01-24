sudo ip addr add dev eth1 192.168.2.20/24
sudo ip addr add dev eth1 192.168.2.21/24
sudo ip addr add dev eth1 192.168.2.22/24
sudo ip addr add dev eth1 192.168.2.23/24
sudo ip addr add dev eth1 192.168.2.24/24
sudo ip addr add dev eth1 192.168.2.25/24
sudo ip addr add dev eth1 192.168.2.26/24
sudo ip addr add dev eth1 192.168.2.27/24
sudo ip addr add dev eth1 192.168.2.28/24
sudo ip addr add dev eth1 192.168.2.29/24

sudo ip neigh add 192.168.2.20 dev eth1 lladdr 04:70:00:00:00:01
sudo ip neigh add 192.168.2.21 dev eth1 lladdr 04:70:00:00:00:01
sudo ip neigh add 192.168.2.22 dev eth1 lladdr 04:70:00:00:00:01
sudo ip neigh add 192.168.2.23 dev eth1 lladdr 04:70:00:00:00:01
sudo ip neigh add 192.168.2.24 dev eth1 lladdr 04:70:00:00:00:01
sudo ip neigh add 192.168.2.25 dev eth1 lladdr 04:70:00:00:00:01
sudo ip neigh add 192.168.2.26 dev eth1 lladdr 04:70:00:00:00:01
sudo ip neigh add 192.168.2.27 dev eth1 lladdr 04:70:00:00:00:01
sudo ip neigh add 192.168.2.28 dev eth1 lladdr 04:70:00:00:00:01
sudo ip neigh add 192.168.2.29 dev eth1 lladdr 04:70:00:00:00:01

sudo ethtool -K eth1 rx off tx off

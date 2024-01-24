sudo ip addr add dev eth1 192.168.0.10/24
sudo ip addr add dev eth1 192.168.0.11/24
sudo ip addr add dev eth1 192.168.0.12/24
sudo ip addr add dev eth1 192.168.0.13/24
sudo ip addr add dev eth1 192.168.0.14/24
sudo ip addr add dev eth1 192.168.0.15/24
sudo ip addr add dev eth1 192.168.0.16/24
sudo ip addr add dev eth1 192.168.0.17/24
sudo ip addr add dev eth1 192.168.0.18/24
sudo ip addr add dev eth1 192.168.0.19/24

sudo ip neigh add 192.168.0.10 dev eth1 lladdr 04:70:00:00:00:02
sudo ip neigh add 192.168.0.11 dev eth1 lladdr 04:70:00:00:00:02
sudo ip neigh add 192.168.0.12 dev eth1 lladdr 04:70:00:00:00:02
sudo ip neigh add 192.168.0.13 dev eth1 lladdr 04:70:00:00:00:02
sudo ip neigh add 192.168.0.14 dev eth1 lladdr 04:70:00:00:00:02
sudo ip neigh add 192.168.0.15 dev eth1 lladdr 04:70:00:00:00:02
sudo ip neigh add 192.168.0.16 dev eth1 lladdr 04:70:00:00:00:02
sudo ip neigh add 192.168.0.17 dev eth1 lladdr 04:70:00:00:00:02
sudo ip neigh add 192.168.0.18 dev eth1 lladdr 04:70:00:00:00:02
sudo ip neigh add 192.168.0.19 dev eth1 lladdr 04:70:00:00:00:02

sudo ethtool -K eth1 rx off tx off

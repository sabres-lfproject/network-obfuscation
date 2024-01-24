# Sabres Network Coding

This repository contains code for implementing Shamir Secret Sharing, XOR, and
Random Linear Network Coding implemented in click.  There are caveats for each
implementation as the design was for networks with multiple disjoint paths.

The click userlevel code can be implemented in both DPDK and userspace.

## Software Dependencies and Submodules

Our implementations are built on fastclick and our Shamir Secret Sharing uses
crypto++, while our RLNC implementation uses gferasure.  We use raven for
building libvirt topologies for network testing.

[fastclick](https://github.com/tbarbette/fastclick)
[Crypto++](https://github.com/weidai11/cryptopp)
[gferasure](https://bitbucket.org/ssrc/gferasure)
[raven](https://gitlab.com/mergetb/tech/raven)

## Source Code

The source code which implements these functions is a submodule in `src`.

## Topology for testing

```
          x      | send packet
          |      
          a      | create secrets, forward over 3 interfaces
         /|\
        t u v    | forward secrets
         \|/
          b      | recv secrets, decode and forward to dest
          |      
          y      | recv packet
```

## Running a test

The following list of scripts will go from nothing to a set of configured
libvirt nodes with dpdk and click installed.

```
./run.sh
./update_hosts.sh
./install.sh
./deploy.sh
./offloading.sh
./modify_libvirt.sh
./configure_dpdk.yml
```

Once all the nodes have been created, configured, and offloading has been
disabled, we are readint to run our click code.  The final step is to build
the click code on the end nodes (a, b) and run the network coding suite:

```
xor
nca
sss
```

## Click Nodes

```
rvn@a:~/fastclick$ make && sudo ./bin/click --dpdk -l 0 -- ../a-xor-dpdk.click
rvn@b:~/fastclick$ make && sudo ./bin/click --dpdk -l 0 -- ../b-xor-dpdk.click
```

### DPDK hugetables debugging memory usages

```
PID=`sudo grep "KernelPageSize:     2048 kB" /proc/[[:digit:]]*/smaps | awk {'print $1'} | cut -d "/" -f3 | sort | uniq`
sudo grep -B 11 'KernelPageSize:     2048 kB' /proc/$PID/smaps | grep "^Size:" | awk 'BEGIN{sum=0}{sum+=$2}END{print sum/1024}' 
```

### RLNC / Gferasure code

For the RLNC code, we need to add to the `LD_LIBRARY_PATH` environment variable
where it can find the gferasure library.

```
sudo LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/rvn/fastclick/include/gferasure/src/ ./bin/click --dpdk -l 0-3 -n 1 -- ../a-nca-dpdk.click
sudo LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/rvn/fastclick/include/gferasure/src/ ./bin/click --dpdk -l 0-3 -n 1 -- ../b-nca-dpdk.click
```

## Acknowledgements

This research is supported by DARPA under grant number HR001120C0157. The views,
opinions, and/or findings expressed are those of the author(s) and should not be
interpreted as representing the official views or policies of the sponsoring
organizations, agencies, or the U.S. Government.

## LICENSE

The 3-Clause BSD License

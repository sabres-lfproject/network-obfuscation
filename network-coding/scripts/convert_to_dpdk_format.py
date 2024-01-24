#!/usr/bin/python3

import os
import subprocess
import re


files = os.listdir("/home/rvn")
template = ""
for f in files:
    if ".click" in f:
        template = f

dpdk_template = ""
with open(template, "r") as fi:
    for line in fi:
        #print(line)
        variable = line.split("::")[0]
        if "FromDevice" in line or "ToDevice" in line:
            new_line = ""
            p = re.compile(r'eth[0-9]')
            ethMatch = all_match = re.findall(p, line)
            ethNum = ethMatch[0]
            p = subprocess.run('ethtool -i %s' % ethNum, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, encoding='utf-8')
            for oline in p.stdout.split("\n"):
                print(oline)
                if "bus-info" in str(oline):
                    pcibus = ':'.join(oline.split(":")[1:])
                    print(pcibus)
                    if "FromDevice" in line:
                        new_line = "FromDPDKDevice(%s, MTU 2500, JUMBO true, N_QUEUES 1, MAXTHREADS 1, MODE none);" % pcibus
                    if "ToDevice" in line:
                        new_line = "ToDPDKDevice(%s, N_QUEUES 1);" % pcibus
            dpdk_template += "%s\t::\t%s\n" % (variable, new_line)
        else:
            dpdk_template += line


new_template = template.split(".")
new_template[-2] += "-dpdk"
nt = ".".join(new_template)

with open(nt, "w") as fi:
    fi.write(dpdk_template)

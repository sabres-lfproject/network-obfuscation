#!/usr/bin/env python3

import re
import os, sys, shutil
from time import sleep
fileList = os.getenv('yMIMIC_FILE_LIST','/click/NonVPN/pcap-list')
outDir = os.getenv('yMIMIC_OUT_DIR','/tmp/')

informHosts = os.getenv('yMIMIC_INFORM_HOSTS', 'x,y').split(',')
monitorHost = os.getenv('yMIMIC_MONITOR_HOST', 'v')

informPorts = os.getenv('yMIMIC_INFORM_PORT', '3333,3333').split(',')
monitorPort = os.getenv('yMIMIC_MONITOR_PORT','3333')
listenPorts = os.getenv('yMIMIC_LISTEN_PORTS','3333,3334').split(',')

text_file = open(fileList, "r")
lines = text_file.readlines()
fList = [ln.rstrip() for ln in lines]

##4 test pcaps
start=sys.argv[1]
if str(start).isdigit():
    start=int(start)
    fList = fList[start:]
else:
    idx = fList.index(start)
    fList = fList[idx:]

CMD='tcpdump -i eth1 -c 40000 -w'.split()
#CMD='tcpdump -i eth1 -c 40000 -s 64 -w'.split()
INFORM='echo $(date +"%T"),{},{} | nc {} {}' #command, file, host, port
MONITOR='echo $(date +"%T"),{},{} | nc {} {}' #command, file, host, port
LISTEN='nc -l {} -W 1' # wait for one message
fileArg='/tmp/{}.pcap'
outfileArg='{}/{}.pcap'
for fl in fList:
    filen = fileArg.format(os.path.splitext(os.path.basename(fl))[0])
    print ("ECHO CMD", os.P_NOWAIT, CMD[0], *CMD, filen)
    pid = os.spawnlp(os.P_NOWAIT, CMD[0], *CMD, filen)
    sleep(2)
    for i,host in enumerate(informHosts):
        informcmd = INFORM.format("start", filen, host, informPorts[i])
        print ("ECHO", host, informcmd)
        while (rc := os.system(informcmd)) !=0:
            print ("RC %d \r", rc, end="")
            sleep(1)
    monitorcmd = MONITOR.format("start", filen, monitorHost, monitorPort)
    os.system(monitorcmd) # execute only once
    ## monitor to listen with -W 1 and restart connection
    for i,port in enumerate(listenPorts):
        os.system(LISTEN.format(port))
    # exit out of all listen - ready to end
    print("Killing", "kill -9 %d"%(pid))
    os.system("kill -9 %d"%(pid))
    #pid.terminate()
    #shutil.copy(filen, outfileArg.format(outDir,fl))

        
##py (t) start capture

## inform a,b on start;
## wait for done from a,b;
## kill capture;
## copy out pcap to specified location

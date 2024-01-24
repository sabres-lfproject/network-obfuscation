#!/usr/bin/env python3

import re
import os, sys, shutil, shlex
from time import sleep
fileList = os.getenv('yMIMIC_FILE_LIST','/click/NonVPN/pcap-list')
bgfilen = os.getenv('yMIMIC_BACKGROUND_TRAFFIC','/click/NonVPN/pcap/AIMchat1.csv')
inDir = os.getenv('yMIMIC_IN_DIR','/click/NonVPN/mimic-extract/') # check the directory
outDir = os.getenv('yMIMIC_OUT_DIR','/tmp/')
(mePercent, meTime) = os.getenv('yMIMIC_END_ARGS','1 3600').split() # mimicEndArgs 

isClient=False
if "client" in sys.argv[0]:
    isClient=True

informHosts = os.getenv('yMIMIC_INFORM_HOSTS', 't').split(',')
monitorHost = os.getenv('yMIMIC_MONITOR_HOST', 'v')

if isClient:
    informPorts = os.getenv('yMIMIC_INFORM_PORT', '3334').split(',')
else:
    informPorts = os.getenv('yMIMIC_INFORM_PORT', '3333').split(',')
monitorPort = os.getenv('yMIMIC_MONITOR_PORT','3333')
listenPorts = os.getenv('yMIMIC_LISTEN_PORTS','3333').split(',')

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

CMD=''
BGCMD=''
if not isClient:
    #CMD='mimic-replay -s -i client.ips -e {}/{} | /click/mimic/mimic-end.py {} {}'
    CMD='/click/mimic/mimic-rs'
    BGCMD='/click/mimic/mimic-rsm'
else:
    #CMD='mimic-replay -c 192.168.2.2 -i server.ips -n5 -e {}/{} | /click/mimic/mimic-end.py {} {}'
    CMD='/click/mimic/mimic-rc'
    BGCMD='/click/mimic/mimic-rcm'

INFORM='echo $(date +"%T"),{},{} | nc {} {}' #command, file, host, port
MONITOR='echo $(date +"%T"),{},{},{} | nc {} {}' #command, file, host, port
LISTEN='nc -l {} -W 1' # wait for one message


for fl in fList:
    for i,port in enumerate(listenPorts):
        print ("Listening on ", LISTEN.format(port))
        os.system(LISTEN.format(port))
    # listen to py (t)
    fname=re.sub('.pcap.*','.csv',os.path.basename(fl))
    #mCmd=CMD.format(inDir,fname,mePercent,meTime).split()
    print(os.P_NOWAIT, "bash", "bash", BGCMD, bgfilen)
    pid = os.spawnlp(os.P_NOWAIT, "bash" , "bash", BGCMD, bgfilen)
    print(os.P_NOWAIT, "bash", "bash", BGCMD, bgfilen, pid)
    os.system("ps -p {}".format(pid))
    # spawn background traffic 
    print(os.P_WAIT, CMD, CMD, inDir,fname,mePercent,meTime)
    #print(os.P_WAIT, mCmd[0], *mCmd)    
    os.system(shlex.join([CMD, inDir, fname, mePercent, meTime]))
    #pid = os.spawnlp(os.P_WAIT, CMD, CMD, inDir,fname,mePercent,meTime)
    # start replay
    print("Killing", "kill -9 %d"%(pid))
    os.system("kill -9 %d"%(pid))

    for i,host in enumerate(informHosts):
        informcmd = INFORM.format("end", fname, host, informPorts[i])
        while os.system(informcmd) !=0:
            sleep(1)
        monitorcmd = MONITOR.format("end", fname, sys.argv[0], monitorHost, monitorPort)
        os.system(monitorcmd) # execute only once
    

    # inform when it terminates

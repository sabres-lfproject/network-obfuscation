import re
import os, sys, shutil, shlex
from time import sleep

BGCMD='/click/mimic/mimic-rsm'
bgfilen = os.getenv('yMIMIC_BACKGROUND_TRAFFIC','/click/NonVPN/mimic-extract/AIMchat1.csv')
print(os.P_NOWAIT, "bash", "bash", BGCMD, bgfilen)
pid = os.spawnlp(os.P_NOWAIT, "bash", "bash", BGCMD, bgfilen)
print(os.P_NOWAIT, BGCMD, BGCMD, bgfilen, pid)
print("ps -p {}".format(pid))
os.system("ps -p {}".format(pid))

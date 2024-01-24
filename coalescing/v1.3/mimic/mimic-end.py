#!/usr/bin/env python3
import sys, re, os, time
frac=float(sys.argv[1])
tdiff=float(sys.argv[2])

reg=re.compile("^Successfully completed (\d+)/(\d+) avg")

t0 =0
for line in sys.stdin:
    line.rstrip()
    #print ("DBG", line)
    m=reg.match(line)
    if m:
        if t0 == 0:
            t0 = time.time()
        td = time.time() - t0
        print ("outp %s of %s expecting %f completion or time %f elapsed: %f\r"%(m.group(1), m.group(2), frac, tdiff, td), end="")
        m2 = float(m.group(2))
        if m2 > 0 and (td > tdiff or float(m.group(1)) >= frac * m2):
            print ("exiting", td, m.group(1), m2)
            os.system("pkill -TERM mimic-replay")
            exit(0)

#print("Exit")

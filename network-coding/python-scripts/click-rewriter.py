#!/usr/bin/python3

### XXX: this requires that the raven model use host names that are ordered in
### a manner where the previous node is in a total ordering and is of a lesser
### value than the current node.

# Takes in args from user with the source and desination name's (e.g., "a b")
# Will then run the jq script:
# cat .rvn/topo.json | jq  ".links[] | [.endpoints[] | .port], .props.mac" | jq -n '[inputs]' > links.map
#  [
#    2,
#    3
#  ],
#  {
#    "g": "04:70:00:00:00:62",
#    "h": "04:70:00:00:00:73"
#  }

# This script will see this as host 'g' port 'eth2' and assosiated mac address.

import json
import re
import subprocess
from pprint import pprint
from collections import OrderedDict
from os.path import exists

# create the file
# we need to always recreate as we dont know if the links.map is stale
# from another raven experiment/model
x = subprocess.Popen("./create-links.sh")
x.wait()

# open links file
f = open('links.map')
data = json.load(f)
f.close()

## TODO
# Need to aggregate the links map, so that each node only has
# a single entry, and the entry has four values for the neighbors
# and local interfaces in order to do forwarding correctly.

p = 0

src = ""
dst = ""
srcdev = ""
dstdev = ""
srcmac = ""
dstmac = ""
zeList = {}


for i in data:
	if p % 2 == 0:
		srcdev = i[0]
		dstdev = i[1]
	if p % 2 == 1:
		x = list(i)
		src = x[0]
		dst = x[1]
		# NOTE: this is because python dicts are not ordered
		# and we are enforcing that our raven model is
		# written from a left to righ manner where the
		# src node is always a smaller value than the
		# next node in the topology
		if dst < src:
			t = dst
			dst = src
			src = t
		srcmac = i[src]
		dstmac = i[dst]

	p += 1
	if p % 2 == 0:
		src = str(src)
		dst = str(dst)
		srcmac = str(srcmac)
		dstmac = str(dstmac)
		if src not in zeList:
			zeList[src] = [(src,srcdev,srcmac,dst, "dst")]
		else:
			zeList[src].append((src, srcdev, srcmac,dst, "dst"))
		if dst not in zeList:
			zeList[dst] = [(dst,dstdev,dstmac,src, "src")]
		else:
			zeList[dst].append((dst, dstdev, dstmac,src, "src"))

pprint(zeList)
zeList = OrderedDict(zeList)
asList = list(zeList.keys())

ready = {}
for srchost in zeList:
	# ignore the endpoints, this is for forwarding nodes
#	if srchost == asList[0] or srchost == asList[-1]:
#		print("1 skipping %s" % srchost)
#		continue
	if len(zeList[srchost]) == 1:
		print("2 skipping %s" % srchost)
		continue

	order = {}
	# this node
	w = zeList[srchost][0]
	x = zeList[srchost][1]

	# neighbor nodes
	a = zeList[w[-2]]
	b = zeList[x[-2]]
	y = ""
	z = ""
	# we need to then find 'this' node in neighbor node list
	# to match the correct lists together.  So the end result
	# is that we have 'this' node and 'neighbor' node matched.
	for t in a:
		if t[-2] == w[0]:
			y = t
	for t in b:
		if t[-2] == x[0]:
			z = t
	
	# create the ordering
	# note that y is before w, because we have to respect the
	# previous neighbor ordering.
	if w[-1] == "src":
		order["first"] = [y[2], w[2]]
		order["second"] = [x[2], z[2]]
		order["left"] = "eth%s" % w[1]
		order["right"] = "eth%s" % x[1]
	else:
		order["first"] = [x[2], z[2]]
		order["second"] = [y[2],w[2]]
		order["left"] = "eth%s" % x[1]
		order["right"] = "eth%s" % w[1]
			
	ready[srchost] = order

# now we are ready to create our forwarding files and replace
# 2-forwarding template with the correct values on the nodes.
#pprint(ready)

macaddr = re.compile(r'(?:[0-9a-fA-F]:?){12}')
for node in ready:
	with open("../click-templates/xor.template") as fr:
		with open("../click-templates/%s.click" % node, "w") as fw:
			first = True
			for line in fr:
				## TODO replace here
				if "EtherRewrite" in line:
					z = re.findall(macaddr, line)
					if first:
						print("in first: %s" % ready[node])
						print("z: %s" % z)
						line1 = line.replace(z[0],ready[node]['first'][0])
						line2 = line1.replace(z[1],ready[node]['first'][1])
						fw.write(line2)
						first = False
					else:
						line1 = line.replace(z[0],ready[node]['second'][0])
						line2 = line1.replace(z[1],ready[node]['second'][1])
						fw.write(line2)
				elif "Device" in line:
					if "eth1" in line:
						t = line.replace("eth1", ready[node]['left'])
						fw.write(t)
					else:
						t = line.replace("eth3", ready[node]['right'])
						fw.write(t)
				else:
					fw.write(line)

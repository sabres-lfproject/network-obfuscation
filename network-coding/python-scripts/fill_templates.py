#!/usr/bin/python3

import json
import yaml
from pprint import pprint
from collections import OrderedDict

template_dir = "click-templates"
ends = ["x", "y"]

hostMap = {}

p = 0
src = ""
dst = ""
srcdev = ""
dstdev = ""
srcmac = ""
dstmac = ""
zeList = {}
data = {}

with open('links.map') as f:
    data = json.load(f)


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
            zeList[src] = [(src,srcdev,srcmac,dst)]
        else:
            zeList[src].append((src, srcdev, srcmac, dst))
        if dst not in zeList:
            zeList[dst] = [(dst,dstdev,dstmac, src)]
        else:
            zeList[dst].append((dst, dstdev, dstmac, src))

pprint(zeList)
oList = OrderedDict(zeList)
asList = list(zeList.keys())


yamlMap = {}
with open('../ansible/vars.yml', 'r') as outfile:
        yamlMap = yaml.load(outfile, Loader=yaml.Loader)

pprint(yamlMap)

hostMap = {}

for k in ends:
    if k not in hostMap:
        hostMap[k] = {}
    hostMap[k]['srcip'] = yamlMap['config'][k]['interfaces'][0]['ip']
    hostMap[k]['dstip'] = yamlMap['config'][ends[((ends.index(k)+1)%len(ends))]]['interfaces'][0]['ip']
    hostMap[k]['dstroute'] = yamlMap['config'][k]['routes'][0]['addr']
    hostMap[k]['srcroute'] = yamlMap['config'][ends[((ends.index(k)+1)%len(ends))]]['routes'][0]['addr']
    hostMap[k]['neigh'] = [x[-1] for x in zeList[k]]

pprint(hostMap)

endFi = open('../%s/xor.template' % template_dir).readlines()

xorers = []
for k in ends:
    for neigh in hostMap[k]["neigh"]:
        xorers.append(neigh)

tempMap = hostMap
for k in xorers:
    tempMap[k] = {}
    for v in ends:
        print(k, v)
        for x in hostMap[v]['neigh']:
            if x == k:
                tempMap[k]["DSTIP"] =  hostMap[v]['dstip']
                tempMap[k]["DSTROUTE"] = hostMap[v]['dstroute']
                tempMap[k]["SRCIP"] =  hostMap[v]['srcip']
                tempMap[k]["SRCROUTE"] = hostMap[v]['srcroute']
                print(zeList[v])
                tempMap[k]["SRCMAC"] = zeList[v][0][2]
                tempMap[k]["DSTMAC"] = zeList[ends[((ends.index(v)+1)%len(ends))]][0][2]
    # configure eth device names
    for num in range(0, len(oList[k])):
        tempMap[k]["THISMAC%s" % str(num+1)] = oList[k][num][2]
        tempMap[k]["ETH%s" % str(num+1)] = "eth%s" % (oList[k][num][1]) # raven mgmt is 0
        for x in oList[oList[k][num][-1]]:
            if x[-1] == k:
                tempMap[k]["NEIGHMAC%s" % str(num+1)] = x[2]
        print(tempMap[k])

pprint(tempMap)

for f in xorers:
    temp = endFi
    with open('../'+template_dir+'/'+f+'-xor.click', 'w') as outFi:
        for line in temp:
            templateLine = False
            templateString = line
            skipLine = False
            for template in tempMap[f]:
                if template in templateString:
                    templateString = templateString.replace(template, tempMap[f][template])
            outFi.write(templateString)

endFi = open('../%s/sss.template' % template_dir).readlines()
templates = ["ETH", "DST", "SRC"]

for f in xorers:
    temp = endFi
    with open('../'+template_dir+'/'+f+'-sss.click', 'w') as outFi:
        for line in temp:
            templateLine = False
            templateString = line
            skipLine = False
            for template in tempMap[f]:
                if template in templateString:
                    templateString = templateString.replace(template, tempMap[f][template])
            outFi.write(templateString)

endFi = open('../%s/nca.template' % template_dir).readlines()
templates = ["ETH", "DST", "SRC"]

for f in xorers:
    temp = endFi
    with open('../'+template_dir+'/'+f+'-nca.click', 'w') as outFi:
        for line in temp:
            templateLine = False
            templateString = line
            skipLine = False
            for template in tempMap[f]:
                if template in templateString:
                    templateString = templateString.replace(template, tempMap[f][template])
            outFi.write(templateString)


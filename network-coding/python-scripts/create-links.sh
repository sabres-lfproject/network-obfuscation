#!/bin/bash

cat ../.rvn/topo.json | jq  ".links[] | [.endpoints[] | .port], .props.mac" | jq -n '[inputs]' > links.map

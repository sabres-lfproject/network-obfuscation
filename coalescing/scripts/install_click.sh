#!/bin/bash

set -e

if [[ $EUID -ne 0 ]]; then
	echo "requires root access to run ansible playbook"
	exit 1
fi

ROOTDIR="."
HOSTFILE=$ROOTDIR/.rvn/ansible-hosts
ANSIBLEDIR=$ROOTDIR/ansible
OPTIONAL=$ANSIBLEDIR/vars/hosts.ini
VARSFILE=$ANSIBLEDIR/vars/vars.yml

ANSIBLE_HOST_KEY_CHECKING=False ansible-playbook \
	-i $HOSTFILE -i $OPTIONAL \
	-e 'ansible_python_interpreter=/usr/bin/python3' \
	--extra-vars "@${VARSFILE}" \
	$ANSIBLEDIR/coales.yml

#	$ANSIBLEDIR/install_click.yml


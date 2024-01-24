#!/bin/bash

set -e

if [[ $EUID -ne 0 ]]; then
	echo "requires root access to run ansible playbook"
	exit 1
fi

ANSIBLE_HOST_KEY_CHECKING=False ansible-playbook -i .rvn/ansible-hosts -i ansible/hosts.ini -e 'ansible_python_interpreter=/usr/bin/python3' --extra-vars "@ansible/vars.yml" ansible/addresses.yml
ANSIBLE_HOST_KEY_CHECKING=False ansible-playbook -i .rvn/ansible-hosts -i ansible/hosts.ini -e 'ansible_python_interpreter=/usr/bin/python3' --extra-vars "@ansible/vars.yml" ansible/routes.yml

# TODO: dcomptb this has to use other means
pushd python-scripts
./create-links.sh
python3 fill_templates.py
popd

ANSIBLE_HOST_KEY_CHECKING=False ansible-playbook -i .rvn/ansible-hosts -i ansible/hosts.ini -e 'ansible_python_interpreter=/usr/bin/python3' --extra-vars "@ansible/vars.yml" ansible/copy-templates.yml

#!/bin/bash

if test "$1" == "ASROOT"
then
	echo "Install libraries and tools..."
	make dev-install
	echo "Create /usr/bin/lc...."
	echo '#!/bin/bash
valgrind --leak-check=full --leak-resolution=high --show-reachable=yes --track-origins=yes --track-fds=yes "$1"
' > /usr/bin/lc
	chmod +rx /usr/bin/lc
	echo "Done."
	exit
fi

if test -n "`which sudo`" -a -n "`which apt-get`"
then
	echo "Start $9 as root...."
	sudo bash -c "$0 ASROOT"
else
	echo "sudo or apt-get not found."
fi



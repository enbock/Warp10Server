#!/bin/bash

echo "Create /usr/bin/lc...."
echo '#!/bin/bash
valgrind --leak-check=full --leak-resolution=high --show-reachable=yes --track-origins=yes --track-fds=yes "$1"
' > /usr/bin/lc
chmod +rx /usr/bin/lc
echo "Done."




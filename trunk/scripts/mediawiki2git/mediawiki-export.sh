#!/bin/bash

for i in "$@"; do
    curl \
        "http://windstille.berlios.de/wiki/index.php?title=Special:Export&action=submit" \
    -F "action=submit" \
    -F "pages=$i"      
done

# EOF #

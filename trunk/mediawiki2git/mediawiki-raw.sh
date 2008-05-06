#!/bin/bash

for i in "$@"; do
    i=$(urlesc $i)
    curl "http://windstille.berlios.de/wiki/index.php?title=${i}&action=raw"
done

# EOF #

#!/bin/sh

if [ $# -eq 0 ]; then
  echo "Usage $0 DIRECTORY..."
else
  for i in "$@"; do
    # XCF
    find "$i" -type f -a -name "*.xcf" -print0 | \
    while read -d $'\0' -r file; do
      if svn ls "$file" && [ ! "$(svn propget svn:mime-type "$file")" = "application/x-xcf" ]; then
         echo "$file"
	 svn propset svn:mime-type "application/x-xcf" "$file"
      fi
    done

    # PNG
    find "$i" -type f -a -name "*.png" -print0 | \
    while read -d $'\0' -r file; do
      if svn ls "$file" && [ ! "$(svn propget svn:mime-type "$file")" = "image/png" ]; then
         echo "$file"
	 svn propset svn:mime-type "image/png" "$file"
      fi
    done

done
fi

# EOF #

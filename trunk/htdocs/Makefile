OUTPUTFILES = \
  index.html news.html screenshots.html development.html download.html contact.html \
  artworks.html

all : $(OUTPUTFILES)

clean :
	rm -vf $(OUTPUTFILES)

%.html :: %.xml default.xsl Makefile menu.xml
	FILENAME=$<; \
	LASTCHANGE=`date -I`; \
	echo $${FILENAME%%.xml}; \
	xalan \
          -param filename   "'$${FILENAME%%.xml}'" \
          -param lastchange "'$${LASTCHANGE}'" \
          -in $< \
          -out $@ \
          -xsl default.xsl

upload: berlios

commit: berlios

berlios: all
	svn update && \
	svn commit -m "--- some unknown new stuff (automatically inserted by the upload script) ---" && \
	rsync -Crv . grumbel@shell.berlios.de:/home/groups/windstille/htdocs/

.PHONY: all clean upload

# EOF #

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
          -PARAM filename   "'$${FILENAME%%.xml}'" \
          -PARAM lastchange "'$${LASTCHANGE}'" \
          -IN $< \
          -OUT $@ \
          -XSL default.xsl

commit: all
	cvs commit -m "automatic webpage update"

berlios:
	rsync -Ctrv . grumbel@shell.berlios.de:/home/groups/windstille/htdocs/

.PHONY: all clean upload

# EOF #

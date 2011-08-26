OUTPUTFILES = \
  build/index.html \
  build/news.html \
  build/screenshots.html \
  build/development.html \
  build/download.html \
  build/contact.html \
  build/artworks.html

all : $(OUTPUTFILES)

clean :
	rm -vf $(OUTPUTFILES)

build/:
	mkdir build/

build/%.html :: %.xml default.xsl Makefile menu.xml build/
	@FILENAME=$<; \
	LASTCHANGE=`date -I`; \
	echo $${FILENAME%%.xml}; \
	xsltproc \
          -stringparam filename   "'$${FILENAME%%.xml}'" \
          -stringparam lastchange "'$${LASTCHANGE}'" \
          -o $@ \
          default.xsl $<

upload: berlios

commit: berlios

berlios: all
	# svn update && \
	# svn commit -m "--- some unknown new stuff (automatically inserted by the upload script) ---" && \
	rsync -LCrtv build/ grumbel@shell.berlios.de:/home/groups/windstille/htdocs/

.PHONY: all clean upload

# EOF #

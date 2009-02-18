#!/usr/bin/env python

import os.path
import codecs
import re
from genshi.template import MarkupTemplate
import genshi.builder as bldr
import creoleparser
import sys

# Creoleparser chokes on long documents
sys.setrecursionlimit(2000)

token_pat = re.compile('\s*([A-Za-z_]+) *= *"([^"]+)"\s*')

def parse_arg_string(str):
    dict = {}  
    while str:
        m = token_pat.match(str)
        if not m:
            raise Exception("Couldn't match '%s'" % str)
        dict[m.group(1)] = m.group(2)
        str = str[m.end():]
    return dict

def file2string(filename):
    f = codecs.open(filename, encoding='utf-8')
    s = f.read()
    f.close()
    return s

all_pages = ['Windstille',
             'News',
             'Rebirth',
             'Backstory',
             'Characters',
             'Vehicles',
             'Locations',
             'Particles',
             'Navigation Mesh',
             'Scripting',
             'Drawing Model',
             'Drawing Primitives',
             'Editor',
             'Actions',
             'PDA',
             'Weapons',
             'Fighting',
             'Blender',
             'BlenderToSprite3D',
             'Sprite3D',
             'Sprite',
             'About']

def find_next_page(str):
    for i in range(len(all_pages)):
        if all_pages[i] == str:
            if i == len(all_pages)-1:
                return "Windstille"
            else:
                return all_pages[i+1]
    raise Exception("Couldn't find next page for " + str)

def find_prev_page(str):
    for i in range(len(all_pages)):
        if all_pages[i] == str:
            if i == 0:
                return "Windstille"
            else:
                return all_pages[i-1]
    raise Exception("Couldn't find prev page for " + str)

class Wiki2HTML:
    def macro_func(self, name, arg_string, body, block_type):
        if name == "comment":
            body = self.creole_parser.generate(body)
            return bldr.tag.div(body, class_= 'comment')

        elif name == "note":
            body = self.creole_parser.generate(body)
            return bldr.tag.div(body, class_='note')

        elif name == "thumbnail":
            dict = parse_arg_string(arg_string)
            if not dict.has_key('src'):
                raise Exception("Source argument missing from <<img>>")

            if dict.has_key('alt'):
                alt = dict['alt']
            else:
                alt = dict['src']

            if dict.has_key('title'):
                title = dict['title']            
            else:
                title = None

            return bldr.tag.a(bldr.tag.img(None, src="thumbnails/" + dict['src'], alt=alt, title=title, class_="thumbnail"),
                              href="images/" + dict['src'])
        elif name == "img":
            dict = parse_arg_string(arg_string)
            if not dict.has_key('src'):
                raise Exception("Source argument missing from <<img>>")

            return bldr.tag.img(None, src="images/" + dict['src'], alt=dict['src'], title=dict['title'], class_= 'comment')

        elif name == "ind":
            body = self.creole_parser.generate(body)
            return bldr.tag.div(body, class_='indent' + arg_string[1:])
        else:
            raise Exception("Invalid Macro name: %s" % name)

    def talk_links_funcs(self, str):
        return "Talk%3a" + str + ".html"

    def main(self, args):
        if args == []:
            print "Usage: wiki2html.py FILENAME..."
        else:
            self.creole_parser = creoleparser.Parser(dialect=creoleparser.Creole10(
                    wiki_links_base_url   = '',
                    wiki_links_path_func  = lambda pagename: pagename + ".html",
                    wiki_links_space_char = ' ',
                    # no_wiki_monospace   = True,
                    use_additions         = True,
                    macro_func            = self.macro_func,
                    interwiki_links_funcs={'Talk' : self.talk_links_funcs }
                    ))

            try:
                for filename in args:
                    tmpl = MarkupTemplate(file2string("template.xml"))
                    print tmpl.generate(next = find_next_page(os.path.basename(filename)[:-5]),
                                        prev = find_prev_page(os.path.basename(filename)[:-5]),
                                        body = self.creole_parser.generate(file2string(filename)),
                                        title = filename.replace(".wiki", "")).render(method='xhtml', 
                                                                                      strip_whitespace=True)
            except Exception, err:
                sys.stderr.write(str(err) + "\n")

            
if __name__ == "__main__":
    wiki2html = Wiki2HTML()
    sys.exit(wiki2html.main(sys.argv[1:]))

# EOF #

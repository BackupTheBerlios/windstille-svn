#! /usr/bin/env python

import os, sys
import pygame
import _fam
from pygame.locals import *
from threading import Thread
from stat import *

def do_continue():
    global mtime, filename
    new_mtime = os.stat(filename)[ST_MTIME]
    if mtime != new_mtime:
        mtime = new_mtime
        return False
    else:
        return True

def main(args):   
    if len(args) != 1:
        print "Usage rungame.py FILENAME"
    else:
        global filename, mtime, screen
        
        pygame.init()
        screen   = pygame.display.set_mode((512,512))
        filename = args[0]
        mtime    = os.stat(filename)[ST_MTIME]

        quit = False
        while not quit:
            try:
                print "Restarting:", filename
                execfile(filename)
            except SyntaxError, err:
                print "SyntaxError:", err                
            except (KeyboardInterrupt, SystemExit):
                print "Interrupt recieved, going to quit"
                quit = True
            except:
                print "Unexpected error:", sys.exc_info()[0]
                
            pygame.time.wait(500)


filename = None
mtime    = None
screen   = None

if __name__ == "__main__":
    main(sys.argv[1:])
    
# EOF #

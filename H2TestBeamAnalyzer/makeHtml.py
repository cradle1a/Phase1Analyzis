#!/usr/bin/env python

import sys
import optparse
import commands
import os
import glob
import subprocess
import time

#######################
# Get options
#######################

parser = optparse.OptionParser("usage: %prog [options]\
<input directory> \n")

parser.add_option ('--t', dest='title', type='string',
                   default = 'Plots',
                   help="Title of html page. (Default: %default)")
parser.add_option ('--s', dest='size', type='int',
                   default = 400,
                   help="Size of displayed plots. Ignored for now.")
parser.add_option ('--rw', dest='rowWidth', type='int',
                   default = 3,
                   help="Number of plots per row. (Default: %default)")
parser.add_option ('--ext', type="string",
                   dest="ext", default="pdf",
                   help="Format of linked image. (Default: %default)")
parser.add_option ('--img', type='string',
                   dest="img", default="gif",
                   help="Format of displayed image. (Default: %default)")
options, args = parser.parse_args()


if len(args) != 1:
    print "Please specify input dir.  Exiting."
    sys.exit(1)

indir  = args[0]+"/"
title = options.title
size = options.size
ext = options.ext
img = options.img
rowWidth = options.rowWidth

##############################################
# Check dir
##############################################
if not os.path.isdir(indir) :
    print "Cannot find %s.  Exiting." % infile
    sys.exit(1)

print "Input directory: " + indir
os.chdir(indir)

#if os.path.isfile("index.html") :
#    print "%s already exists.  Exiting." % indir+"index.html"

if title == "make":
    print indir
    sindir = indir.split("/")
    # remove white spaces
    while '' in sindir: sindir.remove('')
    print sindir
    title = sindir[len(sindir)-1]
    print len(sindir)-1
    print title

procs = []

# wait until number of active processes is less than 'maxproc'
def wait_nproc(maxproc, poll_interval = 0.1):
    while True:
        # get a number of active subprocesses
        nact = 0
        for p in procs:
            if p.poll() is None: nact += 1
        
        if nact <= maxproc: break
        else: time.sleep(poll_interval)

# generate HTML code and prepare thumbnails
html = ""
html += "<html>\n"
html += "<head>\n"
html += "<title>" + title +"</title>\n"
html += "</head>\n"
html += "<body>\n"
html += "<h1>" + title + "</h1>\n"
html += "<hr>\n"

files = glob.glob('*' + ext)
files.sort()

for file in files:
    #print "Processing", file
    #sys.stdout.write(".")
    #sys.stdout.flush()
    
    base = os.path.splitext(file)[0]
    fname = "%s.%s" % (base, img)
    #fsmall = "%s_small.%s" % (base, img)
    flink = "%s.%s" % (base, ext)
    
    # It appears that the ROOT made gifs are already pretty small, so no need to create them. 
    #wait_nproc(10)
    #p = subprocess.Popen(['convert', fname, "-resize", "%sx%s" % (size, size), fsmall])
    #p = subprocess.call(['convert', fname, "-resize", "%sx%s" % (size, size), fsmall])
    #procs.append(p)
    html += '<a href="%s"><img src="%s"></a>\n' % (flink, fname)

sys.stdout.write("\n")

# wait until all background jobs are finished
#wait_nproc(0)

html += "</body>\n"
html += "</html>\n"

# write out the index.html content
findex = open("index.html", "w")
findex.truncate()
findex.write(html)
findex.close()


#!/usr/bin/env python

import os
import sys
import subprocess
import optparse
import re

from runlists import getEmapFromRun, getShuntFromRun

parser = optparse.OptionParser("usage: %prog <input filename> [options]")

parser.add_option('-v', dest="verbosity", default="0", help="Verbosity level for cmsRun (integer >= 0)")
parser.add_option('-n', dest="nevents", default="-1", help="Number of events to process.")
parser.add_option('-m', '-e', dest="emap", default=None, help="EMAP file")
parser.add_option('-s', dest="shunt", default=None, help="QIE 11 shunt setting (float, default = 1.0)")
parser.add_option('-a', dest="adcHistOnly", action="store_true", default=False, help="Only run adcHists analyzer")

options, args = parser.parse_args()

numargs = len(args)
if len(args) != 1:
    print "Usage: h2-tb-analyzer.py <input filename> [options]"
    sys.exit(1)

inputFile = args[0]
inputPattern = re.compile("([0-9]+).root")
m = inputPattern.search(inputFile)
if m:
   runNumber = m.group(1)
else:
   print "### ERROR: Could not extract run number from filename."
   print "### The filename must end in:  xxxxxx.root where xxxxxx is the run number."
   sys.exit(1)

rn = int(runNumber)
prefix = "UserCode/H2TestBeamAnalyzer/"
if options.emap:
    emapFile = prefix + options.emap.split('/')[-1]
    print "Using EMAP: ",emapFile
else:
    emapFile = prefix + getEmapFromRun(int(runNumber))
    print "No EMAP Specified.  Using EMAP ",emapFile

shunt = 1.0
if options.emap:
    shunt = options.shunt
    print "Using shunt: ",shunt
else:
    shunt = getShuntFromRun(int(runNumber))
    print "No shunt Specified.  Using shunt ",shunt

numEvents = 0
if options.nevents.isdigit():
   numEvents = int(options.nevents)
   if numEvents == 0:
        print "Specificying -n 0 processes all events."
   else:
        print "User Limit on number of events to process: ",numEvents   

setVerbosity = 0
if options.verbosity.isdigit(): setVerbosity = options.verbosity
print "Using verbosity level",setVerbosity
 
command = ["cmsRun","h2-tb-analyzer-run.py",inputFile,emapFile,runNumber,str(numEvents),str(setVerbosity),str(shunt),str(int(options.adcHistOnly))]
print "Executing \"%s\"" % " ".join(command) 
subprocess.check_call(command)

print "Finished!  Finalizing output file..."
emapFileShort = emapFile.rsplit('.',1)[0].rsplit('/')[-1]
os.rename("ana_h2_tb_run"+runNumber+"_"+emapFileShort+"_processing.root","ana_h2_tb_run"+runNumber+"_"+emapFileShort+".root")


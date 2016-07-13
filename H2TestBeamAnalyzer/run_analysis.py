#!/usr/bin/python
#run_analysis.py

import subprocess
import sys
import optparse
import os

#Options

parser = optparse.OptionParser("usage: %prog [options]")

parser.add_option ('--r', type='string',
                   dest="runs", default = "-999",
                   help="Pick a specific run number or range of numbers with Unix globbing - may need to use single quotes or -f before running. NOTE - MUST USE 6 DIGIT RUN NUMBER, add leading zeros as needed.")
parser.add_option ('-d',
                   dest="delete", action="store_true",
                   default=False, help="Delete files after moving to destination")
parser.add_option ('--dest',
                   type='string', dest="destination",
                   default="/hcalTB/Analysis/", help="Destination directory for run results/html. Remote locations ok")
defRunDest = "/home/daq/Analysis/HcalTestBeam/data_spool_mirror"
parser.add_option ('--runDest',
                   type='string', dest="runDest", default=defRunDest, help="Where the run files HTB*.root are to be stored during processing. '.' for working directory, but will be removed if '.' is used")
parser.add_option ('-v', dest="verbose", action="store_true",
                   default=False, help="Runs the analysis in verbose mode. Not recommended on large runs or batches of runs, as verbose output can be quite massive.")
parser.add_option ('-q', dest="mute", action="store_true", default=False, help="Further decreases verbosity.")
parser.add_option ('--all', dest="all", action="store_true", default=False, help="Use --all to run on all files in spool")
parser.add_option ('-f', dest="force", action="store_true", default=False, help="Ignore warnings about run(s) already being staged and proceed with processing run(s)")
parser.add_option ('-u', dest="doUndone", action="store_true", default=False, help="Run analysis on all runs which have not been processed into the destination directory")
options, args = parser.parse_args()


all = options.all
delete = options.delete
runDest = options.runDest
runs = options.runs
destination = options.destination
verbose = options.verbose
mute = options.mute
force = options.force
doUndone = options.doUndone
dataLoc = '/data/spool/'

if len(sys.argv) == 1:
    parser.print_help()
    sys.exit(1)
if all:
    runs = '*'
if runs == "-999":
    print "No runs specified. Use --r or --all"
    sys.exit(1)

########

#Get files from cmshcaltb02
lsCom = 'ls ' + dataLoc + 'HTB_' + runs + '.root'

ls = subprocess.Popen(['ssh','daq@cmshcaltb02', lsCom,], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
ls.wait()
out, err =  ls.communicate()
fileList = out.split()
fileList.sort(reverse=True)
if doUndone:
    ls2 = subprocess.Popen(['ls', "/hcalTB/Analysis/"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    ls2.wait()
    out, err = ls2.communicate()
    done = out.split()
    for run in done:
        runNum1 = run[12:]
        runNum1 = runNum1.zfill(6)
        runName = dataLoc + "HTB_" + runNum1 + ".root"
        if runName in fileList:
            fileList.remove(runName)
            print "Run %s has already been processed. Run without -u to process anyway" % runNum1
for fileName in fileList:
    name = fileName[12:]
    runNum = fileName[16:-5]
    if len(runNum) == 6:
        print "Getting run number %s" % runNum
        rsyncPath = "daq@cmshcaltb02:%s" % fileName
        if verbose:
            subprocess.call(["rsync", "-av", rsyncPath, runDest])
        else:
            subprocess.call(["rsync", "-aq", rsyncPath, runDest])
        symLinkPath = runDest + '/' + name
        if runDest != '.':
            link = subprocess.Popen(["ln", "-s", symLinkPath, "."], stdout=open(os.devnull, 'wb'), stderr=subprocess.PIPE)
            out, err = link.communicate()
            if err[:2] == "ln":
                if force:
                    print "Warning, run %s has already been staged for processing. -f used, proceeding anyway..." % runNum
                else:
                    print "Warning, run %s has already been staged for processing, skipping." % runNum
                    fileList.remove(fileName)
###########################
#Run analysis
for fileName in fileList:
    print "==> " + fileName
    name = os.path.basename(fileName)
    
    #Check if file is an HTB*.root file
    if not (len(name) == 15 and name[:3] == "HTB" and name[-5:] == ".root"):
        continue
    
    stdoutf = open(os.devnull, 'wb') if mute else None
    runNum = fileName[16:-5]
    ana = "ana_h2_tb_run%s.root" % runNum
    ana2 = "ana_tb_out_run%s.root" % str(int(runNum))
    plotsDir = "tb_plots_run%s" % str(int(runNum))
    
    if verbose:
        subprocess.check_call(["cmsRun", "h2testbeamanalyzer_cfg_verbose.py", runNum], stdout=stdoutf)
    else:
        subprocess.check_call(["cmsRun", "h2testbeamanalyzer_cfg.py", runNum], stdout=stdoutf)
    subprocess.check_call(["./tb_ana.py", "--i", ana, "--o", ana2, "--r", str(int(runNum))], stdout=stdoutf)
    subprocess.call(["rm", "-rf", plotsDir])
    print "Generating plots for run " + runNum
    subprocess.check_call(["./tb_plots.py", "--i", ana2, "--o", plotsDir, "--r", str(int(runNum))], stdout=stdoutf)
    print "Generating html for run " + runNum
    subprocess.check_call(["./makeHtml.py", plotsDir], stdout=stdoutf)
    subprocess.check_call(["./makeMenu.sh", plotsDir], stdout=stdoutf)
    print "Moving results of run " + runNum
    subprocess.check_call(["rsync", "-aq", "--delete", plotsDir, destination], stdout=stdoutf)
    #subprocess.call(["rm", name])
    
    if delete:
        subprocess.call(["rm", ana])
        subprocess.call(["rm", ana2])
        subprocess.call(["rm", "-rf", plotsDir])
    
    print "Finished processing run %s. Results at http://cmshcalweb01.cern.ch/hcalTB/Analysis/%s" % (runNum, plotsDir)


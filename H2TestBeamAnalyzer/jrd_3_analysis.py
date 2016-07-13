#!/usr/bin/env python
#run_analysis.py

import subprocess
import sys
import socket
import optparse
import os
import re

from runlists import getEmapFromRun 

#Options

parser = optparse.OptionParser("usage: %prog [options]")

parser.add_option ('-r','--r', type='string', dest="runs",
                   help="Pick a specific run number or range of numbers with Unix globbing - may need to use single quotes or -f before running. NOTE - MUST USE 6 DIGIT RUN NUMBER, add leading zeros as needed.")
parser.add_option ('-o','--dest', type='string', dest="outputLoc", help="Destination directory for run results/html. Remote locations ok")
parser.add_option ('--runDest', type='string', dest="runDest", help="Where the run files HTB*.root are to be stored during processing.")
parser.add_option ('-i', type='string',dest="inputLoc",help="Where to find the HTB*.root files.  This can be a network location.")
parser.add_option ('-v', dest="verbose", action="store_true",
                   default=False, help="Runs the analysis in verbose mode. Not recommended on large runs or batches of runs, as verbose output can be quite massive.")
parser.add_option ('--all', dest="all", action="store_true", default=False, help="Use --all to run on all files in spool")
parser.add_option ('-f','--clobber', dest="clobber", action="store_true", default=False, help="Ignore warnings about run(s) already being staged and proceed with processing run(s)")
parser.add_option ('-c', dest="cmsRun", action="store_true", default=False, help="Run only cmsRun h2testbeamanalyzer_cfg.py and other selected options, default is all on")
parser.add_option ('-a', dest="tb_ana", action="store_true", default=False, help="Run only tb_ana.py and other selected options, default is all on")
parser.add_option ('-p', dest="tb_plots", action="store_true", default=False, help="Run only tb_plots.py and other selected options, default is all on")
parser.add_option ('-m', dest="makeHtml", action="store_true", default=False, help="Run only makeHtml.py and other selected options, default is all on")
parser.add_option ('-s', dest="sync", action="store_true", default=False, help="Only sync and use and other selected options, default is all on")
parser.add_option ('-n', dest="nevents", default="-1", help="Number of events to process in cmsRun")
parser.add_option ('-e', dest="emap", default=None, help="Specific EMAP to be used for raw data processing")

#parser.add_option ('-d',
#                   dest="delete", action="store_true",
#                   default=False, help="Delete files after moving to destination")

#parser.add_option ('-q', dest="mute", action="store_true", default=False, help="Further decreases verbosity.")

#parser.add_option ('-u', dest="doUndone", action="store_true", default=False, help="Run analysis on all runs which have not been processed into the destination directory")


options, args = parser.parse_args()

all = options.all
#delete = options.delete
op_runDest = options.runDest
op_runs = options.runs
outputLoc = options.outputLoc
verbose = options.verbose
#mute = options.mute
#doUndone = options.doUndone
op_inputLoc = options.inputLoc
op_cmsRun = options.cmsRun
op_tb_ana = options.tb_ana
op_tb_plots = options.tb_plots
op_makeHtml = options.makeHtml
op_sync = options.sync
clobber = options.clobber
op_nevents = options.nevents
op_emapFile = options.emap

logLevel = 0

FATAL = 5
LEV4 = 4
INFO = 1
DIAG = 0

def writeout(severity, line):
    if (severity >= logLevel): print "[%i] %s" % (severity,line)

if len(sys.argv) == 1:
    parser.print_help()
    sys.exit(1)

# Determine the set of runs to be processed.
# The input argument is used with ls to determine matching run numbers
# If the input argument contains a hyphen, take it to be a range of runs.
# In this case, set the wildcard to '*' and set runList

runs = op_runs
if all:
    runs = '*'
if not runs:
    writeout(FATAL,"No runs specified. Use -r or --all")
    sys.exit(1)

runList = []
if '-' in runs:
    lo = runs.split('-')[0]
    hi = runs.split('-')[1]
    if lo.isdigit() and hi.isdigit():
        loRun = int(lo)
        hiRun = int(hi)
        runList = range(loRun,hiRun+1) 
        runs = '*'

writeout(DIAG,"runs = %s and runList = %s" % (runs,runList))

#envTest = subprocess.Popen(['$CMSSW_BASE'], stdout=subprocess.PIPE)
#envTest.wait()
#out = envTest.communicate()
#print out[0]
#if out[0] != "/":
#    print out[0]
#    print "CMS environment not set up."
#    sys.exit(1)

# Run all the steps unless one or more steps is specified

do_cmsRun = op_cmsRun
do_tb_ana = op_tb_ana
do_tb_plots = op_tb_plots
do_makeHtml = op_makeHtml
do_sync = op_sync

# If no specific run stage options are specified, the default is to run all stages
if (not op_cmsRun) and (not op_tb_ana) and (not op_tb_plots) and (not op_makeHtml) and (not op_sync):
    do_cmsRun = True
    do_tb_ana = True
    do_tb_plots = True
    do_makeHtml = True
    do_sync = True

# Determine the first and last steps to run
if do_sync: firstStep = 4
if do_makeHtml: firstStep = 3
if do_tb_plots: firstStep = 2
if do_tb_ana: firstStep = 1
if do_cmsRun: firstStep = 0

if do_cmsRun: lastStep = 0
if do_tb_ana: lastStep = 1
if do_tb_plots: lastStep = 2
if do_makeHtml: lastStep = 3
if do_sync: lastStep = 4

filePrefixList = ["HTB_","ana_h2_tb_run","ana_tb_out_run","tb_plots_run","tb_plots_run","tb_plots_run"]
fileSuffixList = [".root",".root",".root","","",""]

inputFileFormat = [filePrefixList[firstStep],fileSuffixList[firstStep]]
outputFileFormat = [filePrefixList[lastStep+1],fileSuffixList[lastStep+1]]

inputLoc = op_inputLoc
if not inputLoc:
    inputLoc = "/data/spool"
    if socket.gethostname() != "cmshcaltb05":
        inputLoc = "daq@cmshcaltb02.cern.ch:/data/spool"
        if firstStep > 0: inputLoc = "."
    writeout(DIAG,"setting inputLoc = %s" % inputLoc)
writeout(INFO,"Using input location: %s" % inputLoc)    

runDest = op_runDest
if not runDest:
    if socket.gethostname() != "cmshcaltb05": runDest = "."
    writeout(DIAG,"setting runDest = %s" % runDest)
writeout(INFO,"Using temporary run directory: %s" % runDest)    

        
# Based on the contents of the input and output directories and command line options, determine which files to process

inputLoginInfo = ""
inputIsRemote = False
if ':' in inputLoc:
    inputIsRemote = True     #if the input location has a colon, assume a network location
    inputLoginInfo = inputLoc.split(':')[0]
    inputLoc = inputLoc.split(':')[1]

run_glob = runs
if '*' not in runs: run_glob = '*' + runs  #'*' helps with leading zeroes
if firstStep == 1: run_glob = run_glob + '_*'
inputFileSpec = inputLoc.rstrip('/') + '/' + inputFileFormat[0] + run_glob + inputFileFormat[1]
unix_file_list = "ls "
if firstStep >= 3: unix_file_list = "ls -d " 
inputCommand = unix_file_list + inputFileSpec
writeout(DIAG,"inputCommand = %s" % inputCommand)

# At this point, the following are set: inputLoc, inputLoginInfo, inputIsRemote, inputLoginInfo, inputCommand

if inputIsRemote:
    ls1 = subprocess.Popen(['ssh', inputLoginInfo, inputCommand], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    ls1.wait()
    out, err =  ls1.communicate()
else:
    ls1 = subprocess.Popen(inputCommand, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)    
    ls1.wait()
    out, err =  ls1.communicate()

inputFileList = out.split()
inputFileList.sort(reverse=True)

if not inputFileList: 
    writeout(FATAL,"No input files matching %s to process." % inputFileSpec)
    sys.exit(1)

# If a runList is defined, select only those runs and reset the inputFileList

if len(runList) > 0:
    inputPattern = re.compile(inputFileFormat[0]+"(0*[0-9]+)")
    temp_input_file_list = []
    for oneInputFile in inputFileList:
        m = inputPattern.search(oneInputFile)
        if m:
            runnum = int(m.group(1).lstrip('0'))
            if runnum in runList: temp_input_file_list.append(oneInputFile)
    inputFileList = temp_input_file_list                        

    writeout(DIAG, "Attempting to find input files that match:" + inputLoginInfo + ':' + inputFileSpec)
    if len(inputFileList) == 0:
        writeout(DIAG, "No files found.")
    else:
        writeout(DIAG, "\n  ".join(inputFileList))

#Now check output location        
 
outputPattern = re.compile(outputFileFormat[0]+"(0*[0-9]+)_.*"+outputFileFormat[1])
        
if not outputLoc:
    outputLoc = "/hcalTB/Analysis"
    if socket.gethostname() != "cmshcaltb05": outputLoc = "daq@cmshcaltb05.cern.ch:/hcalTB/Analysis"

outputLoginInfo = ""
outputIsRemote = False
if ':' in outputLoc:
    outputIsRemote = True     #if the output location has a colon, assume a network location
    outputLoginInfo = outputLoc.split(':')[0]  #everything before the colon
    outputLoc = outputLoc.split(':')[1]  #everything after the colon

outputDirectory = outputLoc.rstrip('/')
outputCommand = 'ls ' + outputDirectory
    
                                                                                                                                                                                                                                                                  
if clobber:
    processFileList = inputFileList

else:

    if outputIsRemote:
        ls2 = subprocess.Popen(['ssh', outputLoginInfo, outputCommand], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        ls2.wait()
        out, err =  ls2.communicate()
    else:
        ls2 = subprocess.Popen(outputCommand, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)    
        ls2.wait()
        out, err =  ls2.communicate()

    outputFiles = out.split()

    runNumbersFoundList = []
    outputExistingFilesList = []                
    for oneFile in outputFiles:
        m = outputPattern.match(oneFile)
        if m: 
            runNumbersFoundList.append(m.group(1).lstrip('0'))
            outputExistingFilesList.append(oneFile)
    
    runNumbersFoundList.sort(reverse=True)
    outputExistingFilesList.sort(reverse=True)

    
    if len(outputExistingFilesList) > 0:
        writeout(DIAG,"Files that already appear in the output directory %s:" % outputDirectory)
        writeout(DIAG, "\n  ".join(outputExistingFilesList))

    processFileList = []
    for inputFileName in inputFileList:
        doProcess = True
        for runNumber in runNumbersFoundList:
            if runNumber in inputFileName:
                doProcess = False
                break
        if doProcess: processFileList.append(inputFileName)        
                                                                                                                                                                  
writeout(INFO,"Planning to process the following input data files:")
if len(processFileList) == 0:
    writeout(INFO, "None.")
else:
    writeout(INFO, "\n ".join(processFileList))
    
runNumberPattern = re.compile(inputFileFormat[0]+"(0*[0-9]+)")
emapPattern = re.compile(inputFileFormat[0]+"[0-9]+_(.*)\.")

# Here we really start the processing

for fileName in processFileList:
    
    runNum = -1
    m = runNumberPattern.search(fileName)
    if m: runNum = m.group(1)
    if runNum == -1: continue

    writeout(INFO, "Processing run number %s" % runNum)
                
    if op_emapFile:
        emapFile = op_emapFile
    else:
        m = emapPattern.search(fileName)
        if m:
            emapFile = m.group(1)
        else:
            emapFile = getEmapFromRun(int(runNum))

    writeout(INFO, "Using EMAP %s" % emapFile)
    emapFileShort = emapFile.rsplit('.',1)[0].rsplit('/')[-1]    
    
    # Generate the chanmap file
    subprocess.call(["./emap_to_tb_chanmap.py",emapFile])
    
    raw = fileName
    
    # If the input file is remote, we need to make a local copy
    if inputIsRemote:
        rsyncPath = inputLoginInfo + ':' + fileName
        writeout(INFO, "Transfering %s to %s" % (rsyncPath, runDest))
        subprocess.call(["rsync", "-av", rsyncPath, runDest])
        raw = fileName.rsplit('/',1)[-1]
            
    if do_cmsRun:
        writeout(LEV4,">> Stage 1: Executing cmsRun for Run %s" % runNum)
        command = ["./h2-tb-analyzer.py", "-e", emapFile, "-n", op_nevents, raw]
        writeout(LEV4,">> Executing \"%s\"" % " ".join(command))
        subprocess.call(command)
        
    ana = filePrefixList[1] + runNum + '_' + emapFileShort + fileSuffixList[1]
    ana2 = filePrefixList[2] + runNum + '_' + emapFileShort + fileSuffixList[2]
    plotsDir = filePrefixList[3] + runNum + '_' + emapFileShort + fileSuffixList[3]

    if do_tb_ana:
        writeout(LEV4,">> Stage 2: Running Analyzer (tb_ana) for Run %s" % runNum)
        command = ["./tb_ana.py", "--i", ana, "--o", ana2, "--r", str(int(runNum)),"-e",emapFileShort]
        writeout(LEV4,">> Executing \"%s\"" % " ".join(command))
        subprocess.call(command)
        #subprocess.call(["rm", "-rf", plotsDir])
    if do_tb_plots:
        writeout(LEV4,">> Stage 3: Generating Plots Directory %s" % plotsDir)
        command = ["./tb_plots.py", "--i", ana2, "--o", plotsDir, "--r", str(int(runNum)),"-e",emapFileShort]
        writeout(LEV4,">> Executing \"%s\"" % " ".join(command)) 
        subprocess.call(command)
    if do_makeHtml:
        writeout(LEV4,">> Stage 4: Generating HTML in Plots Directory %s" % plotsDir)
        command = ["./makeHtml.py", plotsDir]
        writeout(LEV4,">> Executing \"%s\"" % " ".join(command)) 
        subprocess.call(command)
        writeout(LEV4,">> Executing \"%s\"" % " ".join(command)) 
        command = ["./makeMenu.sh", plotsDir]
        subprocess.call(command)
    if do_sync:
        writeout(LEV4,">> Stage 5: Copying Plots Directory %s to %s" % (plotsDir, outputDirectory))
        if outputIsRemote:
            command = ["rsync", "-a", plotsDir, outputLoginInfo + ':' + outputDirectory]
            writeout(LEV4,">> Executing \"%s\"" % " ".join(command))            
            subprocess.call(command)
        else:
            command = ["cp -r", plotsDir, outputDirectory]
            writeout(LEV4,">> Executing \"%s\"" % " ".join(command))            
            subprocess.call(command)

# quit()


#fileList = processFileList
#
#for fileName in fileList:
#    name = fileName[12:]
#    runNum = fileName[16:-5]
#    if len(runNum) == 6:
#        print "Getting run number %s" % runNum
#        rsyncPath = "daq@cmshcaltb02:%s" % fileName
#        if verbose:
#            subprocess.call(["rsync", "-av", rsyncPath, runDest])
#        else:
#            subprocess.call(["rsync", "-aq", rsyncPath, runDest])
#        symLinkPath = runDest + '/' + name
#        if runDest != '.':
#            link = subprocess.Popen(["ln", "-s", symLinkPath, "."], stdout=open(os.devnull, 'wb'), stderr=subprocess.PIPE)
#            out, err = link.communicate()
#            if err[:2] == "ln":
#                if force:
#                    print "Warning, run %s has already been staged for processing. -f used, proceeding anyway..." % runNum
#                else:
#                    print "Warning, run %s has already been staged for processing, skipping." % runNum
#                    fileList.remove(fileName)
############################
##Run analysis
#for fileName in fileList:
#    name = fileName[12:]
#    runNum = fileName[16:-5]
#    #Check if file is an HTB*.root file
#    if len(name) == 15 and name[:3] == "HTB" and name[-5:] == ".root":
#        if do_cmsRun:
#            if verbose:
#                subprocess.call(["cmsRun", "h2testbeamanalyzer_cfg_verbose.py", runNum])
#            elif 1:
#                subprocess.call(["cmsRun", "h2testbeamanalyzer_cfg.py", runNum], stdout=open(os.devnull, 'wb'))
#            else:
#                subprocess.call(["cmsRun", "h2testbeamanalyzer_cfg.py", runNum])
#        ana = "ana_h2_tb_run%s.root" % runNum
#        ana2 = "ana_tb_out_run%s.root" % str(int(runNum))
#        plotsDir = "tb_plots_run%s" % str(int(runNum))
#        if mute:
#            if do_tb_ana:
#                subprocess.call(["./tb_ana.py", "--i", ana, "--o", ana2, "--r", str(int(runNum))], stdout=open(os.devnull, 'wb'))
#                subprocess.call(["rm", "-rf", plotsDir], stdout=open(os.devnull, 'wb'))
#            if do_tb_plots:
#                print "Generating plots for run " + runNum
#                subprocess.call(["./tb_plots.py", "--i", ana2, "--o", plotsDir, "--r", str(int(runNum))], stdout=open(os.devnull, 'wb'))
#            if do_makeHtml:
#                print "Generating html for run " + runNum
#                subprocess.call(["./makeHtml.py", plotsDir], st2dout=open(os.devnull, 'wb'))
#            #if do_sync:
#            #    print "Moving results of run " + runNum
#            #   subprocess.call(["rsync", "-aq", "--delete", plotsDir, outputLoc], stdout=open(os.devnull, 'wb'))
#        else:
#            if do_tb_ana:
#                subprocess.call(["./tb_ana.py", "--i", ana, "--o", ana2, "--r", str(int(runNum))])
#                subprocess.call(["rm", "-rf", plotsDir])
#            if do_tb_plots:
#                print "Generating plots for run " + runNum
#                subprocess.call(["./tb_plots.py", "--i", ana2, "--o", plotsDir, "--r", str(int(runNum))])
#            if do_makeHtml:
#                print "Generating html for run " + runNum
#                subprocess.call(["./makeHtml.py", plotsDir])
#                subprocess.call(["./makeMenu.sh", plotsDir])
#            #if do_sync:
#            #    print "Moving results of run " + runNum
#            #    subprocess.call(["rsync", "-av", "--delete", plotsDir, outputLoc])
#        subprocess.call(["rm", name])
#        if delete:
#            subprocess.call(["rm", ana])
#            subprocess.call(["rm", ana2])
#            subprocess.call(["rm", "-rf", plotsDir])
#        if do_sync:
#            print "Finished processing run %s. Results at http://cmshcalweb01.cern.ch/hcalTB/Analysis/%s" % (runNum, plotsDir)

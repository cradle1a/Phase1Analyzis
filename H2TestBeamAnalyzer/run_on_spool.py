#!/usr/bin/python
import subprocess

#login to cmshcaltb02 and get list of files
ls = subprocess.Popen(['ls'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
out, err =  ls.communicate()

fileList = out.split()
fileList.sort(reverse=True)  # so that we process the newest runs first

for file in fileList:
    #Check if file is an HTB*.root file
    if len(file) == 15 and file[:3] == "HTB" and file[-5:] == ".root":
	runNum = file[4:10]
	subprocess.call(["cmsRun", "h2testbeamanalyzer_cfg.py", runNum])
	ana = "ana_h2_tb_run%s.root" % runNum
	ana2 = "ana_tb_out_run%s.root" % str(int(runNum))
        plotsDir = "tb_plots_run%s" % str(int(runNum))
	subprocess.call(["./tb_ana.py", "--i", ana, "--o", ana2, "--r", str(int(runNum))])
        subprocess.call(["rm", "-rf", plotsDir])
        subprocess.call(["./tb_plots.py", "--i", ana2, "--o", plotsDir, "--r", str(int(runNum))])
        subprocess.call(["./makeHtml.py", plotsDir])
	subprocess.call(["rsync", "-av", "--delete", plotsDir, "cmshcal@hep06.baylor.edu:/www/html/cmshcal/HcalTestBeam/"])
	subprocess.call(["rm", file])
	#subprocess.call(["rm", ana])
	#subprocess.call(["rm", ana2])
	#subprocess.call(["rm", "-rf", plotsDir])

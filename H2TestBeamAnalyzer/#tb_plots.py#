#!/usr/bin/env python

print "Importing modules"
import sys
import optparse
import os
import ROOT
import array
import time
from math import exp, sqrt, log

#######################
# Get options
#######################

print "Getting options"

parser = optparse.OptionParser("usage: %prog [options] \n")

parser.add_option ('-o', '--o', dest='outdir', type='string',
                   default = None,
                   help="output directory")
parser.add_option ('-i', '--i', dest='infile', type='string',
                   default = None,
                   help="Input file")
parser.add_option ('-r', '--r', dest='runnum', type='int',
                   default = 1,
                   help="Run number")
parser.add_option ('--pe_only', action="store_true",
                   dest="doPE", default=False)
parser.add_option ('-e', dest='emap',
                   default=None,
                   help="EMAP filename in order to read specific tb_chanmap")

#parser.add_option ('--tout', action="store_true",
#                   dest="tout", default=False)

options, args = parser.parse_args()

doPE   = options.doPE
infile = options.infile
outdir = options.outdir
runnum = options.runnum
emapFile = options.emap

if infile is None:
    print "You did not provide an input file! Exiting"
    sys.exit(1)

# The following is needed to get the chanmap and associated
# variables from a tb_chanmap_* file, where the filename
# is known only at runtime.  This could be improved.

chanmapFile = "tb_chanmap"
if emapFile:
    emapFileShort = emapFile.rsplit('.',1)[0].rsplit('/')[-1]
    chanmapFile = "tb_chanmap_"+emapFileShort
chanmapModule = __import__(chanmapFile, globals(), locals(), [], -1)
chanmap = chanmapModule.chanmap
chanlist = chanmapModule.chanlist
from tb_utils import initialize_chanmap_vars
initialize_chanmap_vars(chanmap, chanlist)
from tb_utils import *




if outdir is not None:
    outdir += "/"
    if not os.path.isdir(outdir):
        os.system("mkdir -p "+outdir)


print "Setting ROOT options"

# turn off 'Info' messages from ROOT
ROOT.gErrorIgnoreLevel = ROOT.kWarning

ROOT.gROOT.SetBatch()
ROOT.gROOT.SetStyle("Plain")
#ROOT.gStyle.SetOptStat(111111111)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(1)
ROOT.gStyle.SetTitleX(0.50)
ROOT.gStyle.SetTitleY(0.97)
ROOT.gStyle.SetTitleAlign(23)
ROOT.gStyle.SetPalette(1)
ROOT.gStyle.SetNdivisions(405,"x");
#ROOT.gStyle.SetEndErrorSize(0.)
#ROOT.gStyle.SetErrorX(0.001)

NCont = 255
#stops = [ 0.00, 0.02, 0.34, 0.51, 0.64, 1.00 ]
#red   = [ 1.00, 0.00, 0.00, 0.87, 1.00, 0.51 ]
#green = [ 1.00, 0.00, 0.81, 1.00, 0.20, 0.00 ]
#blue  = [ 1.00, 0.51, 1.00, 0.12, 0.00, 0.00 ]

#stops = [ 0.00, 0.34, 0.61, 0.84, 0.92, 1.00]
#red   = [ 1.00, 0.00, 0.00, 0.87, 1.00, 0.51]
#green = [ 1.00, 0.00, 0.81, 1.00, 0.20, 0.00]
#blue  = [ 0.00, 0.51, 1.00, 0.12, 0.00, 0.00]

#stops  = [ 0.00, 0.34, 0.61, 0.84, 1.00 ]
#red    = [ 0.00, 0.00, 0.87, 1.00, 0.51 ]
stops  = [ 0.00, 0.08, 0.40, 0.84, 1.00 ]
red    = [ 0.00, 0.00, 0.77, 1.00, 1.00 ]
green  = [ 0.00, 0.81, 1.00, 0.20, 0.00 ]
blue   = [ 0.51, 1.00, 0.12, 0.00, 0.00 ]

NRGBs = len(stops)

stopsArray = array.array('d', stops)
redArray   = array.array('d', red)
greenArray = array.array('d', green)
blueArray  = array.array('d', blue)
ROOT.TColor.CreateGradientColorTable(NRGBs, stopsArray, redArray, greenArray, blueArray, NCont)
ROOT.gStyle.SetNumberContours(NCont)


print "Generating plots..."

# choose channel to plot along side all other channels in E_4TS  plots
refChan = 1

# edge threshold
edge_thold = 0.1

pstyle = {}
pstyle[22, "col"] = 1
pstyle[ 5, "col"] = 9
pstyle[23, "col"] = 419
pstyle[17, "col"] = 2
pstyle[18, "col"] = 4
pstyle[ 4, "col"] = 6
pstyle[ 6, "col"] = 46
pstyle[12, "col"] = 28
pstyle[24, "col"] = 3

depth_color_table = {}
depth_color_table[1] = ROOT.kBlack
depth_color_table[2] = ROOT.kRed
depth_color_table[3] = ROOT.kGreen+2
depth_color_table[4] = ROOT.kBlue+1
depth_color_table[5] = ROOT.kMagenta+1
depth_color_table[6] = ROOT.kCyan+1
depth_color_table[7] = ROOT.kOrange -3 
depth_color_table[8] = ROOT.kRed+2
depth_color_table[9] = ROOT.kRed-6
depth_color_table[10] = ROOT.kGreen-6
depth_color_table[11] = ROOT.kBlue-7
depth_color_table[12] = ROOT.kBlue-9
depth_color_table[13] = ROOT.kMagenta+3
depth_color_table[14] = ROOT.kMagenta-6
depth_color_table[15] = ROOT.kCyan+3
depth_color_table[16] = ROOT.kCyan+4
depth_color_table[17] = ROOT.kOrange 

tfile = ROOT.TFile(infile)

vname = {}
vname["hbhe"] = ["numChs", "numTS", "iphi", "ieta", "depth", "pulse"]
vname["hf"] = ["numChs", "numTS", "iphi", "ieta", "depth", "pulse"]
#vname["wc"] = ["xA", "yA", "xB", "yB", "xC", "yC", "xD", "yD"]
vname["wc"] = ["xA", "yA", "xB", "yB", "xC", "yC"]

hist = {}
# Define wire chamber histograms
for ip0 in ["A", "B", "C"]:
    hist["x"+ip0+"_v_y"+ip0]          = tfile.Get("h_x"+ip0+"_v_y"+ip0+"_clean")
    for ixy in ["x", "y"]:
        # 1D histos for x and y in all 4 chambers
        hist[ixy+ip0]          = tfile.Get("h_"+ixy+"_"+ip0+"_clean")
        # 2D histos for x and y correlations for all histo combinations
        
    for ip1 in ["A", "B", "C"]:
        if ((ip0 == "A" and ip1 == "B") or (ip0 == "A" and ip1 == "C") or (ip0 == "A" and ip1 == "D") or 
            (ip0 == "B" and ip1 == "C") or (ip0 == "B" and ip1 == "D") or (ip0 == "C" and ip1 == "D")):            
            for ixy in ["x", "y"]:
                hist[ixy+ip0+"_v_"+ixy+ip1] = tfile.Get("h_"+ixy+"_"+ip0+"v"+ip1+"_clean")


# Make energy plots
for ichan in chanlist:
    ieta = chanmap[ichan][0]
    iphi = chanmap[ichan][1]
    depth = chanmap[ichan][2]
    label = "ieta" + str(ieta) + "_iphi" + str(iphi) + "_depth" + str(depth)
    hist["avgpulse", ichan] = tfile.Get("AvgPulse_"+label)
    hlink = tfile.Get("Link_Error_"+label)
    if hlink:
        hist["linkerror", ichan] = hlink
    hist["e_4TS_noPS", ichan] = tfile.Get("Energy_noPS_"+label)	
    hist["e_4TS_PS", ichan] = tfile.Get("Energy_"+label) 
    hist["e_wcC"   , ichan] = tfile.Get("h_e_wcC_"+label)
    hist["e_wcC_x" , ichan] = tfile.Get("h_e_wcC_x_"+label)
    hist["e_wcC_y" , ichan] = tfile.Get("h_e_wcC_y_"+label)
    hist["e_wcC_noTScut"   , ichan] = tfile.Get("h_e_wcC_noTScut_"+label)
    hist["e_wcC_x_noTScut" , ichan] = tfile.Get("h_e_wcC_x_noTScut_"+label)
    hist["e_wcC_y_noTScut" , ichan] = tfile.Get("h_e_wcC_y_noTScut_"+label)
    hist["e_wcC_efficiency"   , ichan] = tfile.Get("h_e_wcC_efficiency_"+label)
    hist["e_wcC_x_efficiency" , ichan] = tfile.Get("h_e_wcC_x_efficiency_"+label)
    hist["e_wcC_y_efficiency" , ichan] = tfile.Get("h_e_wcC_y_efficiency_"+label)

#    hist["e_4TS"   , ichan] = tfile.Get("h_e_4TS_chan"   +str(ichan))

for depth in valid_depth:
    hist["e_4TS_etaphi",depth] = tfile.Get("Energy_Avg_depth"+str(depth))
    hist["occupancy_event_etaphi",depth] = tfile.Get("Occ_Event_depth_"+str(depth)) 

for iphi in valid_iphi:
    hist["e_4TS_etadepth",iphi] = tfile.Get("Energy_Avg_phi"+str(iphi))
    #hist["occupancy_event_etaphi",depth] = tfile.Get("Energy_Avg_phi"+str(iphi)) 

################################################################
# Get Npe for all samples from counting zeros and average
################################################################

#print "Chan :                    description :: Npe 0's : ped/tot :: Npe Mean : Npe Mean_2 : mean : chi2"
#print "Chan :                    description :: Npe zero-counting : Npe (mean-ped)/gain"
#print "================================================================================================="
#for ichan in chanlist:
#    # Counting zeros    
#    ped = hist["e_4TS", ichan].Integral(1,51)
#    tot = hist["e_4TS", ichan].Integral(1,5000)
#    pePerMip0 =  -1.*log(ped/tot)
#    # Mean
#    mean = hist["e_4TS", ichan].GetMean()
#    pePerMipMean = (mean-pecal[ichan][0])/(pecal[ichan][1]-pecal[ichan][0])
#    pePerMipMean2 = (mean-pecal[ichan][0])/(pecal[ichan][2]-pecal[ichan][1])
#
#    #    print "%4i : %30s ::   %5.2f :   %5.2f ::    %5.2f :      %5.2f :%5.0f :%5.0f" % (ichan, chanType[ichan, runnum], pePerMip0, ped/tot, pePerMipMean, pePerMipMean2, mean, pecal[ichan][3])
#    print "%4i : %30s :: %5.2f : %5.2f" % (ichan, chanType[ichan, runnum], pePerMip0, pePerMipMean)


################################################################
# Find edges of samples
################################################################
#find_edge = {}
#for ichan in chanlist:
#    for ixy in ["x", "y"]:
#        h1 = hist["e_wcC_"+ixy , ichan]
#        nbins = h1.GetNbinsX()
#        tot   = h1.Integral(1,nbins)
#        if tot == 0: 
#            find_edge[ichan, ixy, "L"] = -50.
#            find_edge[ichan, ixy, "H"] =  50.
#            continue
#        for ibin in range(1,nbins):
#            iint = h1.Integral(1,ibin)
#            if iint/tot > edge_thold:
#                
#                find_edge[ichan, ixy, "L"] = h1.GetBinCenter(ibin)
#                break
#        for ibin in range(1,nbins):
#            iint = h1.Integral(nbins-ibin,nbins)
#            if iint/tot > edge_thold:
#                find_edge[ichan, ixy, "H"] = h1.GetBinCenter(nbins-ibin)
#                break
#print " " 
#print "Computed edges for threshold of %2.0f percent" % (100.*edge_thold)
#print "================================================"
#for ichan in chanlist:
#    print "edges[%2i, %4i] = [%5.1f, %5.1f, %5.1f, %5.1f]" % (ichan, runnum, 
#                                                              find_edge[ichan, "x", "L"], 
#                                                              find_edge[ichan, "x", "H"],
#                                                              find_edge[ichan, "y", "L"],
#                                                              find_edge[ichan, "y", "H"])
#
#if doPE : sys.exit()    

##################################
# Plot Occupancy in eta-phi
##################################
cname = "a_occupancy_etaphi"
#ROOT.gStyle.SetOptTitle(1)
canv = ROOT.TCanvas(cname, cname, 800, 800)
pad = canv.GetPad(0)
setPadPasMargin(pad,0.13)
#pad.SetTopMargin(0.2)

for depth in valid_depth:
    setHist2D(hist["occupancy_event_etaphi",depth], "X", "Y", "", 0, 0, 0, 0.9, 0.999999999, 0.1)
    #hist["e_4TS_etaphi",depth].GetListOfFunctions().Add(palette,"br")
    hist["occupancy_event_etaphi",depth].GetXaxis().SetTitle("ieta")
    hist["occupancy_event_etaphi",depth].GetXaxis().CenterTitle(True)
    hist["occupancy_event_etaphi",depth].GetXaxis().SetNdivisions(16)
    hist["occupancy_event_etaphi",depth].GetXaxis().SetLabelSize(0.035)
    hist["occupancy_event_etaphi",depth].GetYaxis().SetNdivisions(16)
    hist["occupancy_event_etaphi",depth].GetYaxis().SetLabelSize(0.035)
    hist["occupancy_event_etaphi",depth].GetYaxis().SetTitle("iphi")
    hist["occupancy_event_etaphi",depth].GetYaxis().CenterTitle(True)
    hist["occupancy_event_etaphi",depth].SetTitle("Fraction of Events with a hit in each ieta,iphi for depth "+str(depth))
    hist["occupancy_event_etaphi",depth].Draw("COLZ ")

    for end in [".pdf", ".gif"]:
        canv.SaveAs(outdir+cname+"--depth"+str(depth).zfill(2)+end)

##################################
# Plot Average energy in eta-phi
##################################
cname = "b_energy_etaphi"
#ROOT.gStyle.SetOptTitle(1)
canv = ROOT.TCanvas(cname, cname, 900, 800)
pad = canv.GetPad(0)
setPadPasMargin(pad,0.18)
pad.SetLeftMargin(0.12)

maxz = 10.
for depth in valid_depth:
    maxz = max(maxz,hist["e_4TS_etaphi",depth].GetMaximum())

for depth in valid_depth:
    setHist2D(hist["e_4TS_etaphi",depth], "X", "Y", "", 0, 0, 0, 0.9, 0.9, 0.1)
    #hist["e_4TS_etaphi",depth].GetListOfFunctions().Add(palette,"br")
    hist["e_4TS_etaphi",depth].SetMaximum(maxz)
    hist["e_4TS_etaphi",depth].GetXaxis().SetTitle("ieta")
    hist["e_4TS_etaphi",depth].GetXaxis().CenterTitle(True)
    hist["e_4TS_etaphi",depth].GetXaxis().SetNdivisions(16)
    hist["e_4TS_etaphi",depth].GetXaxis().SetLabelSize(0.035)
    hist["e_4TS_etaphi",depth].GetYaxis().SetNdivisions(16)
    hist["e_4TS_etaphi",depth].GetYaxis().SetLabelSize(0.035)
    hist["e_4TS_etaphi",depth].GetYaxis().SetTitle("iphi")
    hist["e_4TS_etaphi",depth].GetYaxis().CenterTitle(True)
    hist["e_4TS_etaphi",depth].SetTitle("Average Energy per event in each ieta,iphi for depth "+str(depth))
    hist["e_4TS_etaphi",depth].Draw("COLZ ")

    for end in [".pdf", ".gif"]:
        canv.SaveAs(outdir+cname+"--depth"+str(depth).zfill(2)+end)

cname = "b_energy_etadepth"
#ROOT.gStyle.SetOptTitle(1)
canv = ROOT.TCanvas(cname, cname, 900, 800)
pad = canv.GetPad(0)
setPadPasMargin(pad,0.18)
pad.SetLeftMargin(0.12)

maxz = 10.
for iphi in valid_iphi:
    maxz = max(maxz,hist["e_4TS_etadepth",iphi].GetMaximum())

for iphi in valid_iphi:
    setHist2D(hist["e_4TS_etadepth",iphi], "X", "Y", "", 0, 0, 0, 0.9, 0.9, 0.1)
    #hist["e_4TS_etadepth",iphi].GetListOfFunctions().Add(palette,"br")
    hist["e_4TS_etadepth",iphi].SetMaximum(maxz)
    hist["e_4TS_etadepth",iphi].GetXaxis().SetTitle("ieta")
    hist["e_4TS_etadepth",iphi].GetXaxis().CenterTitle(True)
    hist["e_4TS_etadepth",iphi].GetXaxis().SetNdivisions(16)
    hist["e_4TS_etadepth",iphi].GetXaxis().SetLabelSize(0.035)
    hist["e_4TS_etadepth",iphi].GetYaxis().SetNdivisions(16)
    hist["e_4TS_etadepth",iphi].GetYaxis().SetLabelSize(0.035)
    hist["e_4TS_etadepth",iphi].GetYaxis().SetTitle("depth")
    hist["e_4TS_etadepth",iphi].GetYaxis().CenterTitle(True)
    hist["e_4TS_etadepth",iphi].SetTitle("Average Energy per event in each ieta,depth for iphi "+str(iphi))
    hist["e_4TS_etadepth",iphi].Draw("COLZ ")


    for end in [".pdf", ".gif"]:
        canv.SaveAs(outdir+cname+"--iphi"+str(iphi).zfill(2)+end)

ROOT.gStyle.SetOptTitle(0)

##################################
# Plot pulse shape comparison
##################################
cname = "c_avgPulseShape"
canv = ROOT.TCanvas(cname, cname, 400, 424)
pad = canv.GetPad(0)
setPadPasMargin(pad)

etaphipairs = {}
for ichan in chanlist:
    ieta = chanmap[ichan][0]
    iphi = chanmap[ichan][1]
    depth = chanmap[ichan][2]
    if not etaphipairs.has_key((ieta,iphi)): etaphipairs[ieta,iphi] = []
    etaphipairs[ieta,iphi].append(depth)
    
for etaphi in etaphipairs.keys():

    number_of_depths = len(etaphipairs[etaphi])
    textsize = 0.028 if number_of_depths < 8 else 0.025
    legx0 = 0.18
    legx1 = 0.58 if number_of_depths < 8 else 0.93
    legy0 = 0.895-0.015*number_of_depths if number_of_depths < 8 else 0.86-0.015*number_of_depths/2
    legy1 = 0.895+0.015*number_of_depths if number_of_depths < 8 else 0.86+0.015*number_of_depths/2
    leg = ROOT.TLegend(legx0, legy0, legx1, legy1)
    leg.SetFillColor(0)
    leg.SetTextSize(textsize)
    if number_of_depths >= 8:
        leg.SetNColumns(2)
    leg.SetColumnSeparation(0.03)
    leg.SetEntrySeparation(0.05)
    leg.SetMargin(0.2)
    
    first = True

    maxy = 100.
    for depth in sorted(etaphipairs[etaphi]):
        ieta = etaphi[0]
        iphi = etaphi[1]
        ichan = chanmap[(ieta,iphi,depth)]
        maxy = max(maxy,hist["avgpulse", ichan].GetMaximum())
  
    for depth in sorted(etaphipairs[etaphi]):
        ieta = etaphi[0]
        iphi = etaphi[1]
        ichan = chanmap[(ieta,iphi,depth)]
                
        setHist(hist["avgpulse", ichan], "Time sample", "Charge (fC)", 0, (0.,2400.), 1.3, depth_color_table[depth])
        buff = 1 if number_of_depths < 8 else 1.15
        hist["avgpulse", ichan].SetMaximum(1.20 * maxy * buff)
        hist["avgpulse", ichan].GetXaxis().SetNdivisions(10,1)
        hist["avgpulse", ichan].GetXaxis().SetLabelSize(0.035)
        hist["avgpulse", ichan].GetYaxis().SetLabelSize(0.035)
        hist["avgpulse", ichan].GetXaxis().SetTitleSize(0.045)
        hist["avgpulse", ichan].GetYaxis().SetTitleSize(0.045)
        hist["avgpulse", ichan].GetXaxis().SetTitleOffset(0.9)
        hist["avgpulse", ichan].GetYaxis().SetTitleOffset(1.6)
        if first:
            hist["avgpulse", ichan].Draw("hist")
            first = False
        else:
            hist["avgpulse", ichan].Draw("hist same")

        leg.AddEntry(hist["avgpulse", ichan], "iphi="+str(iphi)+"  "+"ieta="+str(ieta)+"  "+"depth="+str(depth))
        leg.Draw()
        
    for end in [".pdf", ".gif"]:
        canv.SaveAs(outdir+cname+"--iphi"+str(iphi).zfill(2)+"_ieta"+str(ieta).zfill(2)+end)

##################################
# Plot Energy Spectra with no Pedestal Subtraction
##################################
cname = "d_energy_noPS"
ROOT.gStyle.SetOptStat(1110)
canv = ROOT.TCanvas(cname, cname, 400, 424)
pad = canv.GetPad(0)
setPadPasMargin(pad)
pad.SetLogy()
pad.SetLogx(True)

for ichan in chanlist:
    ieta = chanmap[ichan][0]
    iphi = chanmap[ichan][1]
    depth = chanmap[ichan][2]

    setHist(hist["e_4TS_noPS", ichan], "Energy No Ped Subtract (uncalibrated) ", "# Events / fC", 0, 0, 1.3, pstyle[22, "col"])
    #hist["energy", ichan].GetXaxis().SetNdivisions(10,1)
    #max_x_bin = hist["e_4TS_noPS", ichan].FindLastBinAbove(0)
    #use_max = 1000
    #if max_x_bin < 500: use_max = 500
    #if max_x_bin < 100: use_max = 100
    #if use_max == 1000: hist["e_4TS_noPS", ichan].Rebin(10)
    #if use_max == 500: hist["e_4TS_noPS", ichan].Rebin(5)
    
    #hist["e_4TS_noPS", ichan].GetXaxis().SetRangeUser(1,use_max)
    hist["e_4TS_noPS", ichan].SetStats(True)
    hist["e_4TS_noPS", ichan].Scale(1,"width")
    hist["e_4TS_noPS", ichan].SetMaximum(10*hist["e_4TS_noPS", ichan].GetMaximum())
    hist["e_4TS_noPS", ichan].Draw()
    pad.Update()
    hist["e_4TS_noPS", ichan].FindObject("stats").SetX1NDC(0.7)
    hist["e_4TS_noPS", ichan].FindObject("stats").SetX2NDC(0.95)
    hist["e_4TS_noPS", ichan].FindObject("stats").SetY1NDC(0.8)
    hist["e_4TS_noPS", ichan].FindObject("stats").SetY2NDC(0.90)
    hist["e_4TS_noPS", ichan].Draw()
    

    textsize = 0.03; legx0 = 0.23; legx1 = 0.68; legy0 = 0.82; legy1 = 0.88
    leg = ROOT.TLegend(legx0, legy0, legx1, legy1)
    leg.SetFillColor(0)
    leg.SetTextSize(textsize)
    leg.SetColumnSeparation(0.0)
    leg.SetEntrySeparation(0.1)
    leg.SetMargin(0.2)

    leg.AddEntry(hist["e_4TS_noPS", ichan], "iphi="+str(iphi)+"  "+"ieta="+str(ieta)+"  "+"depth="+str(depth))
    leg.Draw()
        
    for end in [".pdf", ".gif"]:
        canv.SaveAs(outdir+cname+"--iphi"+str(iphi).zfill(2)+"_ieta"+str(ieta).zfill(2)+"_depth"+str(depth).zfill(2)+end)

ROOT.gStyle.SetOptStat(0)



##################################
# Plot Energy Spectra
##################################
cname = "d_energy"
ROOT.gStyle.SetOptStat(1110)
canv = ROOT.TCanvas(cname, cname, 400, 424)
pad = canv.GetPad(0)
setPadPasMargin(pad)
pad.SetLogy()
pad.SetLogx(True)

for ichan in chanlist:
    ieta = chanmap[ichan][0]
    iphi = chanmap[ichan][1]
    depth = chanmap[ichan][2]

    setHist(hist["e_4TS_PS", ichan], "Energy (uncalibrated)", "# Events / fC", 0, 0, 1.3, pstyle[22, "col"])
    #hist["energy", ichan].GetXaxis().SetNdivisions(10,1)
    #max_x_bin = hist["e_4TS_PS", ichan].FindLastBinAbove(0)
    #use_max = 1000
    #if max_x_bin < 500: use_max = 500
    #if max_x_bin < 100: use_max = 100
    #if use_max == 1000: hist["e_4TS_PS", ichan].Rebin(10)
    #if use_max == 500: hist["e_4TS_PS", ichan].Rebin(5)
    
    #hist["e_4TS_PS", ichan].GetXaxis().SetRangeUser(1,use_max)
    hist["e_4TS_PS", ichan].SetStats(True)
    hist["e_4TS_PS", ichan].Scale(1,"width")
    hist["e_4TS_PS", ichan].SetMaximum(10*hist["e_4TS_PS", ichan].GetMaximum())
    hist["e_4TS_PS", ichan].Draw()
    pad.Update()
    hist["e_4TS_PS", ichan].FindObject("stats").SetX1NDC(0.7)
    hist["e_4TS_PS", ichan].FindObject("stats").SetX2NDC(0.95)
    hist["e_4TS_PS", ichan].FindObject("stats").SetY1NDC(0.8)
    hist["e_4TS_PS", ichan].FindObject("stats").SetY2NDC(0.90)
    hist["e_4TS_PS", ichan].Draw()
    

    textsize = 0.03; legx0 = 0.23; legx1 = 0.68; legy0 = 0.82; legy1 = 0.88
    leg = ROOT.TLegend(legx0, legy0, legx1, legy1)
    leg.SetFillColor(0)
    leg.SetTextSize(textsize)
    leg.SetColumnSeparation(0.0)
    leg.SetEntrySeparation(0.1)
    leg.SetMargin(0.2)

    leg.AddEntry(hist["e_4TS_PS", ichan], "iphi="+str(iphi)+"  "+"ieta="+str(ieta)+"  "+"depth="+str(depth))
    leg.Draw()
        
    for end in [".pdf", ".gif"]:
        canv.SaveAs(outdir+cname+"--iphi"+str(iphi).zfill(2)+"_ieta"+str(ieta).zfill(2)+"_depth"+str(depth).zfill(2)+end)

ROOT.gStyle.SetOptStat(0)




########################################
## Plot energy in bins of WC C position
########################################
#
ledge = {}
elist = ["xL", "xH", "yL", "yH"]
for ichan in chanlist:
    ledge["xL", ichan] = ROOT.TLine(edges[ichan , runnum][0], -100., edges[ichan , runnum][0], 100.)
    ledge["xH", ichan] = ROOT.TLine(edges[ichan , runnum][1], -100., edges[ichan , runnum][1], 100.)
    ledge["yL", ichan] = ROOT.TLine(-100., edges[ichan , runnum][2], 100., edges[ichan , runnum][2])
    ledge["yH", ichan] = ROOT.TLine(-100., edges[ichan , runnum][3], 100., edges[ichan , runnum][3])
    for iedge in elist:
        ledge[iedge, ichan].SetLineStyle(2)

for ichan in chanlist:
    ieta = chanmap[ichan][0]
    iphi = chanmap[ichan][1]
    depth = chanmap[ichan][2]
    label = "iphi" + str(iphi).zfill(2) + "_ieta" + str(ieta).zfill(2) + "_depth" + str(depth).zfill(2)
    cname = "e_energy_wcC--"+label
    canv = ROOT.TCanvas(cname, cname, 400, 424)
    pad = canv.GetPad(0)
    #    pad.SetLogz()
    setPadPasMargin(pad, 0.25)

    setHist(hist["e_wcC", ichan], "Wire Chamber C x (mm)", "Wire Chamber C y (mm)", 0, 0, 1.3)
    hist["e_wcC"   , ichan].GetZaxis().SetTitle("Evts with E_{4TS} > 25 fC")
    hist["e_wcC"   , ichan].GetZaxis().SetLabelSize(0.03)    
    hist["e_wcC"   , ichan].GetZaxis().SetTitleOffset(1.3)
    hist["e_wcC"   , ichan].Draw("colz")
    
    for iedge in elist:
        ledge[iedge, ichan].Draw()
    textsize = 0.03; xstart = 0.2; ystart = 0.85
    latex = ROOT.TLatex(); latex.SetNDC(); latex.SetTextAlign(12); latex.SetTextSize(textsize)    
    latex.DrawLatex(xstart, ystart, "iphi="+str(iphi)+"  "+"ieta="+str(ieta)+"  "+"depth="+str(depth))
    for end in [".pdf", ".gif"]:
        canv.SaveAs(outdir+cname+end)




########################################
## Plot noTScut in bins of WC C position #added by Abdollah
########################################
#
ledge = {}
elist = ["xL", "xH", "yL", "yH"]
for ichan in chanlist:
    ledge["xL", ichan] = ROOT.TLine(edges[ichan , runnum][0], -100., edges[ichan , runnum][0], 100.)
    ledge["xH", ichan] = ROOT.TLine(edges[ichan , runnum][1], -100., edges[ichan , runnum][1], 100.)
    ledge["yL", ichan] = ROOT.TLine(-100., edges[ichan , runnum][2], 100., edges[ichan , runnum][2])
    ledge["yH", ichan] = ROOT.TLine(-100., edges[ichan , runnum][3], 100., edges[ichan , runnum][3])
    for iedge in elist:
        ledge[iedge, ichan].SetLineStyle(2)

for ichan in chanlist:
    ieta = chanmap[ichan][0]
    iphi = chanmap[ichan][1]
    depth = chanmap[ichan][2]
    label = "iphi" + str(iphi).zfill(2) + "_ieta" + str(ieta).zfill(2) + "_depth" + str(depth).zfill(2)
    cname = "e_energy_wcC_noTScut--"+label
    canv = ROOT.TCanvas(cname, cname, 400, 424)
    pad = canv.GetPad(0)
    #    pad.SetLogz()
    setPadPasMargin(pad, 0.25)
    
    setHist(hist["e_wcC_noTScut", ichan], "Wire Chamber C x (mm)", "Wire Chamber C y (mm)", 0, 0, 1.3)
    hist["e_wcC_noTScut"   , ichan].GetZaxis().SetTitle("All events")
    hist["e_wcC_noTScut"   , ichan].GetZaxis().SetLabelSize(0.03)
    hist["e_wcC_noTScut"   , ichan].GetZaxis().SetTitleOffset(1.3)
    hist["e_wcC_noTScut"   , ichan].Draw("colz")
    for iedge in elist:
        ledge[iedge, ichan].Draw()
    textsize = 0.03; xstart = 0.2; ystart = 0.85
    latex = ROOT.TLatex(); latex.SetNDC(); latex.SetTextAlign(12); latex.SetTextSize(textsize)
    latex.DrawLatex(xstart, ystart, "iphi="+str(iphi)+"  "+"ieta="+str(ieta)+"  "+"depth="+str(depth))
    for end in [".pdf", ".gif"]:
        canv.SaveAs(outdir+cname+end)



########################################
## Plot efficiency in bins of WC C position  #added by Abdollah
########################################
#
ledge = {}
elist = ["xL", "xH", "yL", "yH"]
for ichan in chanlist:
    ledge["xL", ichan] = ROOT.TLine(edges[ichan , runnum][0], -100., edges[ichan , runnum][0], 100.)
    ledge["xH", ichan] = ROOT.TLine(edges[ichan , runnum][1], -100., edges[ichan , runnum][1], 100.)
    ledge["yL", ichan] = ROOT.TLine(-100., edges[ichan , runnum][2], 100., edges[ichan , runnum][2])
    ledge["yH", ichan] = ROOT.TLine(-100., edges[ichan , runnum][3], 100., edges[ichan , runnum][3])
    for iedge in elist:
        ledge[iedge, ichan].SetLineStyle(2)

for ichan in chanlist:
    ieta = chanmap[ichan][0]
    iphi = chanmap[ichan][1]
    depth = chanmap[ichan][2]
    label = "iphi" + str(iphi).zfill(2) + "_ieta" + str(ieta).zfill(2) + "_depth" + str(depth).zfill(2)
    cname = "e_energy_wcC_efficiency--"+label
    canv = ROOT.TCanvas(cname, cname, 400, 424)
    pad = canv.GetPad(0)
    #    pad.SetLogz()
    setPadPasMargin(pad, 0.25)
    
    setHist(hist["e_wcC", ichan], "WC x (Efficiency)", "WC y (Efficiency)", 0, 0, 1.3)
    hist["e_wcC"   , ichan].GetZaxis().SetTitle("Efficiency of Wire Chamber")
    hist["e_wcC"   , ichan].Divide(hist["e_wcC_noTScut"   , ichan])
    hist["e_wcC"   , ichan].GetZaxis().SetLabelSize(0.03)
    hist["e_wcC"   , ichan].GetZaxis().SetTitleOffset(1.3)
    hist["e_wcC"   , ichan].Draw("colz")
    for iedge in elist:
        ledge[iedge, ichan].Draw()
    textsize = 0.03; xstart = 0.2; ystart = 0.85
    latex = ROOT.TLatex(); latex.SetNDC(); latex.SetTextAlign(12); latex.SetTextSize(textsize);latex.SetTextColor(2)
    latex.DrawLatex(xstart, ystart, "iphi="+str(iphi)+"  "+"ieta="+str(ieta)+"  "+"depth="+str(depth)+ "   'Efficiency'")
    for end in [".pdf", ".gif"]:
        canv.SaveAs(outdir+cname+end)

#######################################
# Plot 1D energy in bins of WC C position
#######################################
for ichan in chanlist:
    ieta = chanmap[ichan][0]
    iphi = chanmap[ichan][1]
    depth = chanmap[ichan][2]
    label = "iphi" + str(iphi).zfill(2) + "_ieta" + str(ieta).zfill(2) + "_depth" + str(depth)
    for ixy in ["x", "y"]:
        cname = "e_energy_wcC_"+ixy+"--"+label
        canv = ROOT.TCanvas(cname, cname, 400, 424)
        pad = canv.GetPad(0)
        setPadPasMargin(pad, 0.25)
        setHist(hist["e_wcC_"+ixy, ichan], "Wire Chamber C "+ixy+" (mm)", "Evts with E_{4TS} > 25 fC", 0, 0, 1.3)
        hist["e_wcC_"+ixy, ichan].Draw()
        #        for iedge in ["L","H"]:
        #    ledge[ixy+iedge, ichan].Draw()
        textsize = 0.03; xstart = 0.2; ystart = 0.85
        latex = ROOT.TLatex(); latex.SetNDC(); latex.SetTextAlign(12); latex.SetTextSize(textsize)    
        latex.DrawLatex(xstart, ystart, "iphi="+str(iphi)+"  "+"ieta="+str(ieta)+"  "+"depth="+str(depth))
        ###for end in [".pdf", ".gif"]:
        ###    canv.SaveAs(outdir+cname+end)

#######################################
# Plot 4TS energy
#######################################

for ichan in chanlist:
    ieta = chanmap[ichan][0]
    iphi = chanmap[ichan][1]
    depth = chanmap[ichan][2]
    label = "ieta" + str(ieta) + "_iphi" + str(iphi) + "_depth" + str(depth)
#    #    hist["e_4TS"   , ichan].Rebin()
#    #    hist["e_4TS"   , ichan].SetLineWidth(1)
    hist["e_4TS_PS"   , ichan].SetLineColor(pstyle[22, "col"])

setHist(hist["e_4TS_PS", refChan], "Energy in 4TS (LinADC)", "Events", (0.,300.), (0.5, 3.e4), 1.3, pstyle[22, "col"])
for ichan in chanlist:
    cname = "energy_4TS--chan"+str(ichan).zfill(3)
    canv = ROOT.TCanvas(cname, cname, 400, 424)
    pad = canv.GetPad(0)
    pad.SetLogy()
    setPadPasMargin(pad)
    setHist(hist["e_4TS_PS", ichan], "Energy in 4TS (LinADC)", "Events", (0.,300.), (0.5, 3.e4), 1.3, pstyle[22, "col"])
    hist["e_4TS_PS"   , refChan].Draw()
    hist["e_4TS_PS"   , ichan  ].Draw("same")
    textsize = 0.03; legx0 = 0.4; legx1 = 0.9; legy0 = 0.8; legy1 = 0.93
    leg = ROOT.TLegend(legx0, legy0, legx1, legy1)
    leg.SetFillColor(0)
    leg.SetTextSize(textsize)
    leg.SetColumnSeparation(0.0)
    leg.SetEntrySeparation(0.1)
    leg.SetMargin(0.2)
    leg.AddEntry(hist["e_4TS_PS", ichan], "ch"+str(ichan))
    leg.AddEntry(hist["e_4TS_PS", refChan], "ch"+str(refChan))
    leg.Draw()

    #for end in [".pdf", ".gif"]:
    #    canv.SaveAs(outdir+cname+end)



##################################
# Link Error
##################################
cname = "f_link_error"
canv = ROOT.TCanvas(cname, cname, 400, 424)
pad = canv.GetPad(0)
setPadPasMargin(pad)

for ichan in chanlist:
    ieta = chanmap[ichan][0]
    iphi = chanmap[ichan][1]
    depth = chanmap[ichan][2]

    if ("linkerror", ichan) in hist:
        setHist(hist["linkerror", ichan], "Link Error", "# Events", 0, 0, 1.3)
        hist["linkerror", ichan].SetMaximum(1.2*hist["linkerror", ichan].GetMaximum())
        hist["linkerror", ichan].Draw()
        pad.Update()

        textsize = 0.03; legx0 = 0.23; legx1 = 0.68; legy0 = 0.82; legy1 = 0.88
        leg = ROOT.TLegend(legx0, legy0, legx1, legy1)
        leg.SetFillColor(0)
        leg.SetTextSize(textsize)
        leg.SetColumnSeparation(0.0)
        leg.SetEntrySeparation(0.1)
        leg.SetMargin(0.2)

        leg.AddEntry(hist["linkerror", ichan], "iphi="+str(iphi)+"  "+"ieta="+str(ieta)+"  "+"depth="+str(depth))
        leg.Draw()
        
        for end in [".pdf", ".gif"]:
            canv.SaveAs(outdir+cname+"--iphi"+str(iphi).zfill(2)+"_ieta"+str(ieta).zfill(2)+"_depth"+str(depth).zfill(2)+end)


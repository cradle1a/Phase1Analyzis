#!/usr/bin/env python

print "Importing modules"
import sys
import optparse
import os
import ROOT
import array
import time
import numpy
from math import exp, sqrt

edges10_list = [1.58,   4.73,   7.88,   11.0,   14.2,   17.3,   20.5,   23.6,
  26.8,   29.9,   33.1,   36.2,   39.4,   42.5,   45.7,   48.8,
  53.6,   60.1,   66.6,   73.0,   79.5,   86.0,   92.5,   98.9,
  105,    112,    118,    125,    131,    138,    144,    151,
  157,    164,    170,    177,    186,    199,    212,    225,
  238,    251,    264,    277,    289,    302,    315,    328,
  341,    354,    367,    380,    393,    406,    418,    431,
  444,    464,    490,    516,    542,    568,    594,    620,
  645,    670,    695,    720,    745,
  771,    796,    821,    846,    871,    897,    922,    947,
  960,    1010,   1060,   1120,   1170,   1220,   1270,   1320,
  1370,   1430,   1480,   1530,   1580,   1630,   1690,   1740,
  1790,   1840,   1890,   1940,   2020,   2120,   2230,   2330,
  2430,   2540,   2640,   2740,   2850,   2950,   3050,   3150,
  3260,   3360,   3460,   3570,   3670,   3770,   3880,   3980,
  4080,   4240,   4450,   4650,   4860,   5070,   5280,   5490,
  5680,   5880,   6080,   6280,   6480,
  6680,   6890,   7090,   7290,   7490,   7690,   7890,   8090,
  8400,   8810,   9220,   9630,   10000,  10400,  10900,  11300,
  11700,  12100,  12500,  12900,  13300,  13700,  14100,  14500,
  15000,  15400,  15800,  16200,  16800,  17600,  18400,  19300,
  20100,  20900,  21700,  22500,  23400,  24200,  25000,  25800,
  26600,  27500,  28300,  29100,  29900,  30700,  31600,  32400,
  33200,  34400,  36100,  37700,  39400,  41000,  42700,  44300,
  45900,  47600,  49200,  50800,  52500,
  54100,  55700,  57400,  59000,  60600,  62200,  63900,  65500,
  68000,  71300,  74700,  78000,  81400,  84700,  88000,  91400,
  94700,  98100,  101000, 105000, 108000, 111000, 115000, 118000,
  121000, 125000, 128000, 131000, 137000, 145000, 152000, 160000,
  168000, 176000, 183000, 191000, 199000, 206000, 214000, 222000,
  230000, 237000, 245000, 253000, 261000, 268000, 276000, 284000,
  291000, 302000, 316000, 329000, 343000, 356000, 370000, 384000, 398000]

#######################
# Get options
#######################

print "Getting options"
parser = optparse.OptionParser("usage: %prog [options] \n")

parser.add_option ('-o', '--o', dest='outfile', type='string',
                   default = None,
                   help="output file")
parser.add_option ('-i', '--i', dest='infile', type='string',
                   default = None,
                   help="input file")
parser.add_option ('-r', '--r', dest='runnum', type='int',
                   default = -1,
                   help="Run number")
parser.add_option ('--adc', dest='adc',
                   action='store_true', default = False,
                   help="Turn off lineariziation of ADC counts")
parser.add_option ('--verbose', dest='verbose', 
                   action='store_true', default=False,
                   help="Turn on verbose mode")
parser.add_option ('-e', dest='emap',
                   default=None,
                   help="EMAP filename in order to read specific tb_chanmap")
parser.add_option ('--shunt', dest='shunt', type='float',
                   default=1.,
                   help="QIE shunt setting (default: %default)")
parser.add_option ('--start', dest='start', type='int',
                   default = 0,
                   help="Event number to start at (default: %default)")
parser.add_option ('--sigTS', dest='sigTS', type='int',
                   default = 4,
                   help="Number of time samples to use as signal (default: %default)")

options, args = parser.parse_args()
infile = options.infile
outfile = options.outfile
runnum = options.runnum
verbose = options.verbose
adc = options.adc
emapFile = options.emap
shunt = options.shunt
start = options.start
sigTS = options.sigTS

# Do some sanity checks
if infile is None: 
    print "You did not provide an input file! Exiting."
    sys.exit()
if outfile is None:
    print "You did not provide an output file! Exiting."
    sys.exit()
if runnum is None:
    print "You did not provide a run number! Exiting."
    sys.exit()

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

# Scale bin edges according to shunt value
edges10_np = numpy.array(edges10_list)
edges10_np = edges10_np/shunt

edges10 = array.array('d', edges10_np)

#######################
#  Set ROOT options  
#######################

print "Setting ROOT options"
ROOT.gROOT.SetBatch()
ROOT.gROOT.SetStyle("Plain")
#ROOT.gStyle.SetOptStat(111111111)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetPalette(1)
ROOT.gStyle.SetNdivisions(405,"x");
#ROOT.gStyle.SetEndErrorSize(0.)
#ROOT.gStyle.SetErrorX(0.001)

#######################
# Read input data
#######################

file = ROOT.TFile(infile)
#ntp = file.Get("HFData/Events;3")
ntp = {}
ntp["qie11"] = file.Get("QIE11Data/Events")
nevts    = ntp["qie11"].GetEntries()

############################
# Prepare for tree reading
############################           

vname = {}
vname["qie11"] = ["numChs", "numTS", "iphi", "ieta", "depth", "pulse", "ped", "pulse_adc", "ped_adc", "capid_error", "link_error", "soi","TSn","led_trigger","ped_trigger"]
MAXDIGIS = 300
MAXTS = 10

ROOT.gROOT.ProcessLine("struct qie11_struct {Int_t numChs; Int_t numTS; Int_t iphi[%(dg)d]; Int_t ieta[%(dg)d]; Int_t depth[%(dg)d]; Float_t pulse[%(dg)d * %(ts)d]; Float_t ped[%(dg)d]; UChar_t pulse_adc[%(dg)d * %(ts)d]; Float_t ped_adc[%(dg)d]; bool capid_error[%(dg)d]; bool link_error[%(dg)d]; bool soi[%(dg)d * %(ts)d]; Int_t TSn[%(dg)d * %(ts)d];Int_t led_trigger[%(dg)d];Int_t ped_trigger[%(dg)d];};" % {"dg": MAXDIGIS, "ts": MAXTS})
sqie11 = ROOT.qie11_struct()
for ivname in vname["qie11"]:
    ntp["qie11"].SetBranchAddress(ivname, ROOT.AddressOf(sqie11, ivname))

####################################################
# Define histograms
####################################################

outtfile = ROOT.TFile(outfile, "recreate")

hist = {}

for ichan in chanlist:
    ieta = chanmap[ichan][0]
    iphi = chanmap[ichan][1]
    depth = chanmap[ichan][2]
    label = "ieta" + str(ieta) + "_iphi" + str(iphi) + "_depth" + str(depth)
    hist["avgpulse", ichan] = ROOT.TProfile("AvgPulse_"+label, "AvgPulse_"+label, 10, -0.5, 9.5)
    hist["chargesum", ichan] = ROOT.TProfile("chargesum_"+label, "chargesum_"+label, 10, -0.5, 9.5)
    for its in range(10):
        hist["charge", ichan, its] = ROOT.TH1F("Charge_"+label+"_ts"+str(its),
                                               "Charge_"+label+"_ts"+str(its), 8000, 0., 8000.)

esum = {}
        
####################################################
# Event Loop
####################################################

fillEplots = True

print "Run %5i has %7i total events. " % (runnum, nevts)

# Run over all events starting from event 'start'
nevts_to_run = nevts - start

print "Processing ",nevts_to_run," events."    
for ievt in xrange(start, start + nevts_to_run):
    if (ievt+1) % 1000 == 0: print "Processing Run %5i Event %7i" % (runnum, (ievt+1))

    #######################
    # QIE Analysis
    #######################
   
    ntp["qie11"].GetEvent(ievt)

    chansToFind = []
    for ichan in chanlist: chansToFind.append(chanmap[ichan])

    if verbose: print "chansToFind:", chansToFind
    fchan = {}
    fread = {}
    for rchan in xrange(sqie11.numChs):
        test_chan = (sqie11.ieta[rchan], sqie11.iphi[rchan], sqie11.depth[rchan])
        if test_chan in chansToFind:
            chansToFind.remove(test_chan)
            fchan[chanmap[test_chan]] = rchan
	    #fread[rchan] = sqie11
            fread[test_chan] = sqie11
    if verbose:
        print "fchan:", fchan
    clean = True
    charge = {} 
    energy = {}   
    for ichan,rchan in fchan.iteritems():

        ieta, iphi, depth = chanmap[ichan]

        if verbose:
            print "processing ichan %s, rchan %s" % (ichan, rchan)
            print "corresponding to ieta %s, iphi %s, depth %s" % (ieta, iphi, depth)

        # Pull charges and energies for each time sample, convert to fC when appropriate
        nts = fread[(ieta,iphi,depth)].numTS
        for its in xrange(nts):
            if adc:
                charge[ichan,its] = fread[(ieta,iphi,depth)].pulse_adc[rchan*MAXTS+its]  #[row][col] -> [row*n_cols + col]
                energy[ichan,its] = charge[ichan,its]
            else:
                charge[ichan,its] = fread[(ieta,iphi,depth)].pulse[rchan*MAXTS+its]  #[row][col] -> [row*n_cols + col]
                energy[ichan,its] = charge[ichan,its]*calib[ichan]

        if verbose:
            print "charge: ", ",".join([str(charge[ichan,its]) for its in xrange(nts)])
        esum[ichan, "PED"] = fread[(ieta,iphi,depth)].ped[rchan]*calib[ichan]
        esum[ichan, "PED_ADC"] = fread[(ieta,iphi,depth)].ped_adc[rchan]

        if verbose:
            print "Pedestal (fC) = %s" % (fread[(ieta,iphi,depth)].ped[rchan])
            print "Pedestal (ADC counts) = %s" % (fread[(ieta,iphi,depth)].ped_adc[rchan])

        # Compute signal and pedestal-subtracted signal
        ts_list = xrange(1,1+sigTS) # [4,5,6,7]   #time samples in which to sum charge for signal (4-7 by default)
        sig_esum = 0.
        sig_esum_ps = 0.
        for its in ts_list:  
            if adc:
                sig_esum += charge_adc[ichan,its]
                sig_esum_ps += charge_adc[ichan,its] - esum[ichan, "PED_ADC"]  #pedestal-subtracted energy  
            else:
                sig_esum += energy[ichan,its]
                sig_esum_ps += energy[ichan,its] - esum[ichan, "PED"]  #pedestal-subtracted energy  
        esum[ichan, "4TS_noPS"] = sig_esum
        esum[ichan, "4TS_PS"] = sig_esum_ps    

        # Fill pulse shape plot
        if fillEplots: 
            for its in range(10):
                hist["avgpulse", ichan].Fill(its,energy[ichan,its])
                hist["chargesum", ichan].Fill(esum[ichan, "4TS_noPS"],1)

        if fillEplots: 
            for its in range(10):
                hist["charge", ichan, its].Fill(charge[ichan,its])

outtfile.Write()
outtfile.Close()

print "Finished Run %5i." % runnum

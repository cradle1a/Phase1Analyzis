// -*- C++ -*-
//
// Package:    UserCode/adcHists
// Class:      adcHists
// 
/**\class adcHists adcHists.cc UserCode/adcHists/plugins/adcHists.cc

   Description: [one line class summary]

   Implementation:
   [Notes on implementation]
*/
//
// Original Author:  
//         Created:  Fri, 14 Aug 2015 08:41:12 GMT
//
//


// system include files
#include <memory>

#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "CalibFormats/HcalObjects/interface/HcalCoder.h"

#include "UserCode/H2TestBeamAnalyzer/src/ADC_Conversion.h"
#include "UserCode/adcHists/include/TBCalib.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TBDataFormats/HcalTBObjects/interface/HcalTBTriggerData.h"
#include "TBDataFormats/HcalTBObjects/interface/HcalTBBeamCounters.h"
#include "TBDataFormats/HcalTBObjects/interface/HcalTBEventPosition.h"
#include "TBDataFormats/HcalTBObjects/interface/HcalTBParticleId.h"
#include "TBDataFormats/HcalTBObjects/interface/HcalTBTiming.h"

double edges10[257] = {
  1.58,   4.73,   7.88,   11.0,   14.2,   17.3,   20.5,   23.6,
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
  291000, 302000, 316000, 329000, 343000, 356000, 370000, 384000, 398000,
  410000, 430000, 450000, 470000, 520000, 550000, 580000, 640000, 680000
};

double qie8adc2fC[]={
    -0.5,0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5,9.5, 10.5,11.5,12.5,
    13.5,15.,17.,19.,21.,23.,25.,27.,29.5,32.5,35.5,38.5,42.,46.,50.,54.5,59.5,
    64.5,59.5,64.5,69.5,74.5,79.5,84.5,89.5,94.5,99.5,104.5,109.5,114.5,119.5,
    124.5,129.5,137.,147.,157.,167.,177.,187.,197.,209.5,224.5,239.5,254.5,272.,
    292.,312.,334.5,359.5,384.5,359.5,384.5,409.5,434.5,459.5,484.5,509.5,534.5,
    559.5,584.5,609.5,634.5,659.5,684.5,709.5,747.,797.,847.,897.,947.,997.,
    1047.,1109.5,1184.5,1259.5,1334.5,1422.,1522.,1622.,1734.5,1859.5,1984.5,
    1859.5,1984.5,2109.5,2234.5,2359.5,2484.5,2609.5,2734.5,2859.5,2984.5,
    3109.5,3234.5,3359.5,3484.5,3609.5,3797.,4047.,4297.,4547.,4797.,5047.,
    5297.,5609.5,5984.5,6359.5,6734.5,7172.,7672.,8172.,8734.5,9359.5,9984.5};

double edges8[]={
    -0.5,0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5,9.5, 10.5,11.5,12.5,
    13.5,15.,17.,19.,21.,23.,25.,27.,29.5,32.5,35.5,38.5,42.,46.,50.,54.5,59.5,
    64.5,69.5,74.5,79.5,84.5,89.5,94.5,99.5,104.5,109.5,114.5,119.5,
    124.5,129.5,137.,147.,157.,167.,177.,187.,197.,209.5,224.5,239.5,254.5,272.,
    292.,312.,334.5,359.5,384.5,409.5,434.5,459.5,484.5,509.5,534.5,
    559.5,584.5,609.5,634.5,659.5,684.5,709.5,747.,797.,847.,897.,947.,997.,
    1047.,1109.5,1184.5,1259.5,1334.5,1422.,1522.,1622.,1734.5,1859.5,1984.5,
    2109.5,2234.5,2359.5,2484.5,2609.5,2734.5,2859.5,2984.5,
    3109.5,3234.5,3359.5,3484.5,3609.5,3797.,4047.,4297.,4547.,4797.,5047.,
    5297.,5609.5,5984.5,6359.5,6734.5,7172.,7672.,8172.,8734.5,9359.5,9984.5};


class adcHists : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
public:
    explicit adcHists(const edm::ParameterSet&);
    ~adcHists();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


private:
    virtual void beginJob() override;
    virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
    virtual void endJob() override;

    edm::EDGetTokenT<HBHEDigiCollection> tok_HBHEDigiCollection_;
    edm::EDGetTokenT<QIE11DigiCollection> tok_QIE11DigiCollection_;
    edm::EDGetTokenT<HcalTBTriggerData> tok_HcalTBTriggerData_;
    edm::EDGetTokenT<HcalTBTiming> tok_HcalTBTiming_;

    double gain_;
    
    void fillHist(std::map<std::string, TH1*>& hists, std::string name, float value, int nbins, double bins[]);
    void fillHist(std::map<std::string, TH1*>& hists, std::string name, float value, int nbins, float ll, float ul);

    TH1* hq;

    std::map<std::string, TH1*> hists;
    
    edm::Service<TFileService> fs;

    std::vector<double> binsQIE8, binsQIE11, binsMIP, binsMIP8;

    TBCalibSource tbcs;
};

adcHists::adcHists(const edm::ParameterSet& iConfig)
{
    usesResource("TFileService");

    hq = fs->make<TH1D>("allMu", "allMu", 247, edges10);

    tok_HBHEDigiCollection_ = consumes<HBHEDigiCollection>(edm::InputTag("hcalDigis"));
    tok_QIE11DigiCollection_ = consumes<QIE11DigiCollection>(edm::InputTag("hcalDigis"));
    tok_HcalTBTriggerData_ = consumes<HcalTBTriggerData>(edm::InputTag("tbunpack"));
    tok_HcalTBTiming_ = consumes<HcalTBTiming>(edm::InputTag("tbunpack"));

    gain_ = iConfig.getUntrackedParameter<double>("gain");
}


adcHists::~adcHists()
{
    
}

void adcHists::fillHist(std::map<std::string, TH1*>& hists, std::string name, float value, int nbins, double bins[])
{
    auto hist = hists.find(name);
    if(hist == hists.end())
    {
	hists[name] = fs->make<TH1D>(name.c_str(), name.c_str(), nbins, bins);
	hists[name]->Sumw2();
	hists[name]->Fill(value);
    }
    else
    {
	hist->second->Fill(value);
    }
}

void adcHists::fillHist(std::map<std::string, TH1*>& hists, std::string name, float value, int nbins, float ll, float ul)
{
    auto hist = hists.find(name);
    if(hist == hists.end())
    {
	hists[name] = fs->make<TH1D>(name.c_str(), name.c_str(), nbins, ll, ul);
	hists[name]->Sumw2();
	hists[name]->Fill(value);
    }
    else
    {
	hist->second->Fill(value);
    }
}

void adcHists::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;

    edm::Handle<HBHEDigiCollection> hbheDigiCollection;
    iEvent.getByToken(tok_HBHEDigiCollection_,hbheDigiCollection);

    edm::Handle<QIE11DigiCollection> hqie11dc;
    iEvent.getByToken(tok_QIE11DigiCollection_, hqie11dc);

    edm::Handle<HcalTBTriggerData> trigData;
    iEvent.getByToken(tok_HcalTBTriggerData_, trigData);

    edm::Handle<HcalTBTiming> timing;
    iEvent.getByToken(tok_HcalTBTiming_,timing);

    //Reject any event which was not a beam trigger and only a beam trigger inside the spill window
    //if(trigData->wasInSpillPedestalTrigger() || trigData->wasOutSpillPedestalTrigger() || trigData->wasSpillIgnorantPedestalTrigger()) return;
    //if(trigData->wasLEDTrigger())   return;
    //if(trigData->wasLaserTrigger()) return;
    //if(trigData->wasFakeTrigger())  return;
    //if(!trigData->wasInSpill())     return;

    double s1Count = timing->S1Count();
    double s2Count = timing->S2Count();
    double s3Count = timing->S3Count();
    double s4Count = timing->S4Count();
    double triggerTime = timing->triggerTime();
    double ttcL1Atime = timing->ttcL1Atime();

    fillHist(hists, "s1Count", s1Count, 100, 0, 100);
    fillHist(hists, "s2Count", s2Count, 100, 0, 100);
    fillHist(hists, "s3Count", s3Count, 100, 0, 100);
    fillHist(hists, "s4Count", s4Count, 100, 0, 100);
    fillHist(hists, "triggerTime", triggerTime, 1000, 0, 20000);
    fillHist(hists, "ttcL1Atime", ttcL1Atime, 1000, 0, 20000);

    //ADC to nominal charge converter 
    Converter converter(gain_);

    std::map<std::string, float> towerSum, towerSumCal;

    for(auto& digi : *hbheDigiCollection)
    {
	int iphi = digi.id().iphi();
        int ieta = digi.id().ieta();
        int depth = digi.id().depth();
	
    	float ped = (qie8adc2fC[digi.sample(0).adc()&0xff] + qie8adc2fC[digi.sample(1).adc()&0xff] + qie8adc2fC[digi.sample(2).adc()&0xff])/3.0;
    	float q = qie8adc2fC[digi.sample(4).adc()&0xff] + qie8adc2fC[digi.sample(5).adc()&0xff] + qie8adc2fC[digi.sample(6).adc()&0xff] - 3*ped;

	//float q = digi.sample(5).adc()&0xff;

	std::stringstream hnum;
	hnum << ieta << "_" << iphi << "_" << depth;

	std::stringstream tnum;
	tnum << ieta << "_" << iphi;
	
	if(trigData->wasBeamTrigger())
	{    
	    fillHist(hists, "beam_adc_" + hnum.str(), q, binsQIE8.size() - 1, binsQIE8.data());

	    towerSum[tnum.str()] += q;
	    towerSumCal[tnum.str()] += q * tbcs.getQIE8Corr(ieta, iphi);
	    fillHist(hists, "tower_adc_" + tnum.str(), q, binsQIE8.size() - 1, binsQIE8.data());
	}
	else
	{
	    fillHist(hists, "ped_adc_" + hnum.str(), q, binsQIE8.size() - 1, binsQIE8.data());
	}
    }

    std::map<std::string, float> times;

    const QIE11DigiCollection& qie11dc = *hqie11dc;
    for (int j=0; j < qie11dc.size(); ++j)
    {
        // Extract info on detector location
        DetId detid = qie11dc[j].detid();
        HcalDetId hcaldetid = HcalDetId(detid);
        int ieta = hcaldetid.ieta();
        int iphi = hcaldetid.iphi();
        int depth = hcaldetid.depth();

        float adc[10];//, tdc[10];
        for(int i = 0; i < 10; ++i)
        {
            adc[i] = converter.linearize(qie11dc[j][i].adc());
	    //tdc[i] = float(qie11dc[j][i].tdc())/2.0;	
        }

	float tdc = -999.0;

	for(int i = 4; i <= 8; ++i)
	{
	    if(qie11dc[j][i].tdc() != 62 && qie11dc[j][i].tdc() != 63)
	    {
		tdc = (i - 4)*25.0 + float(qie11dc[j][i].tdc())/2.0;
		break;
	    }
	}

	float ped = (adc[0] + adc[1] + adc[2])/3.0;
	float q = adc[4] + adc[5] + adc[6] - 3*ped;
	float qNosub = adc[4] + adc[5] + adc[6];

	std::stringstream hnum;
	hnum << ieta << "_" << iphi << "_" << depth;
	
	std::stringstream tnum;
	tnum << ieta << "_" << iphi;

	times[hnum.str()] = tdc;

	if(trigData->wasBeamTrigger())
	{    
	    fillHist(hists, "beam_adc_" + hnum.str(), q, 247, binsQIE11.data());

	    towerSum[tnum.str()] += q;
	    towerSumCal[tnum.str()] += q * tbcs.getQIE11Corr(ieta, iphi, depth);
	    fillHist(hists, "tower_adc_" + tnum.str(), q, 247, binsQIE11.data());
	    
	    fillHist(hists, "beam_tdc_" + hnum.str(), tdc, 250, 0, 125);
	    
	    hq->Fill(q);
	}
	else
	{
	    fillHist(hists, "ped_adc_" + hnum.str(), q, 247, binsQIE11.data());
	    
	    fillHist(hists, "ped_tdc_" + hnum.str(), tdc, 250, 0, 125);
	}

	fillHist(hists, "adc_nosub_" + hnum.str(), qNosub, 247, binsQIE11.data());
    }

    for(int ieta = 18; ieta < 21; ++ieta)
    {
	std::stringstream hnumRef;
	hnumRef << ieta << "_" << 5 << "_" << 2;
	float& tref = times[hnumRef.str()];
	
	for(int depth = 1; depth <= 6; ++depth)
	{
	    std::stringstream hnum;
	    hnum << ieta << "_" << 5 << "_" << depth;

	    float& t    = times[hnum.str()];

	    float dt = t - tref;
	    if(t < 0 || tref < 0) dt = -999.9;

	    fillHist(hists, "timeDiff_" + hnum.str(), dt, 100, -25, 25);
	}
    }

    double qCluster_all = 0.0;
    double qClusterCal_all = 0.0;

    for(auto& tower : towerSum)
    {
	fillHist(hists, "towerSum_adc_" + tower.first, tower.second, 247, binsQIE11.data());
	qCluster_all += tower.second;
    }

    for(auto& tower : towerSumCal)
    {
	fillHist(hists, "towerSumCal_adc_" + tower.first, tower.second, 247, binsQIE11.data());
	qClusterCal_all += tower.second;
    }

    if(towerSum.size())
    {
	//Uncalibrated SiPM-only clusters
	double qCluster_17_5 = towerSum["16_5"] + towerSum["17_5"] + towerSum["18_5"] + towerSum["16_6"] + towerSum["17_6"] + towerSum["18_6"];
	double qCluster_18_5 = towerSum["17_5"] + towerSum["18_5"] + towerSum["19_5"] + towerSum["17_6"] + towerSum["18_6"] + towerSum["19_6"];
	double qCluster_19_5 = towerSum["18_5"] + towerSum["19_5"] + towerSum["20_5"] + towerSum["18_6"] + towerSum["19_6"] + towerSum["20_6"];
	double qCluster_20_5 = towerSum["19_5"] + towerSum["20_5"] + towerSum["21_5"] + towerSum["19_6"] + towerSum["20_6"] + towerSum["21_6"];
	fillHist(hists, "cluster_adc_17_5", qCluster_17_5, binsQIE11.size() - 1, binsQIE11.data());
	fillHist(hists, "cluster_adc_18_5", qCluster_18_5, binsQIE11.size() - 1, binsQIE11.data());
	fillHist(hists, "cluster_adc_19_5", qCluster_19_5, binsQIE11.size() - 1, binsQIE11.data());
	fillHist(hists, "cluster_adc_20_5", qCluster_20_5, binsQIE11.size() - 1, binsQIE11.data());
	fillHist(hists, "cluster_adc_all",  qCluster_all,  binsQIE11.size() - 1, binsQIE11.data());

	//Calibrated SiPM-only clusters
	double qClusterCal_17_5 = towerSumCal["16_5"] + towerSumCal["17_5"] + towerSumCal["18_5"] + towerSumCal["16_6"] + towerSumCal["17_6"] + towerSumCal["18_6"];
	double qClusterCal_18_5 = towerSumCal["17_5"] + towerSumCal["18_5"] + towerSumCal["19_5"] + towerSumCal["17_6"] + towerSumCal["18_6"] + towerSumCal["19_6"];
	double qClusterCal_19_5 = towerSumCal["18_5"] + towerSumCal["19_5"] + towerSumCal["20_5"] + towerSumCal["18_6"] + towerSumCal["19_6"] + towerSumCal["20_6"];
	double qClusterCal_20_5 = towerSumCal["19_5"] + towerSumCal["20_5"] + towerSumCal["21_5"] + towerSumCal["19_6"] + towerSumCal["20_6"] + towerSumCal["21_6"];
	fillHist(hists, "clusterCal_adc_17_5", qClusterCal_17_5, binsQIE11.size() - 1, binsQIE11.data());
	fillHist(hists, "clusterCal_adc_18_5", qClusterCal_18_5, binsQIE11.size() - 1, binsQIE11.data());
	fillHist(hists, "clusterCal_adc_19_5", qClusterCal_19_5, binsQIE11.size() - 1, binsQIE11.data());
	fillHist(hists, "clusterCal_adc_20_5", qClusterCal_20_5, binsQIE11.size() - 1, binsQIE11.data());
	fillHist(hists, "clusterCal_adc_all",  qClusterCal_all,  binsQIE11.size() - 1, binsQIE11.data());

	//Uncalibrated HPD-only clusters - only does the right thing for old runs pre-SiPM installation (use VME readout to be 'safe')
	double qClusterHPD_17_15 = towerSum["16_15"] + towerSum["17_15"] + towerSum["18_15"] + towerSum["16_16"] + towerSum["17_16"] + towerSum["18_16"] + towerSum["16_14"] + towerSum["17_14"] + towerSum["18_14"];
	double qClusterHPD_18_15 = towerSum["17_15"] + towerSum["18_15"] + towerSum["19_15"] + towerSum["17_16"] + towerSum["18_16"] + towerSum["19_16"] + towerSum["17_14"] + towerSum["18_14"] + towerSum["19_14"];
	double qClusterHPD_19_15 = towerSum["18_15"] + towerSum["19_15"] + towerSum["20_15"] + towerSum["18_16"] + towerSum["19_16"] + towerSum["20_16"] + towerSum["18_14"] + towerSum["19_14"] + towerSum["20_14"];
	double qClusterHPD_20_15 = towerSum["19_15"] + towerSum["20_15"] + towerSum["21_15"] + towerSum["19_16"] + towerSum["20_16"] + towerSum["21_16"] + towerSum["19_14"] + towerSum["20_14"] + towerSum["21_14"];
	fillHist(hists, "clusterHPD_adc_17_15", qClusterHPD_17_15, binsQIE8.size() - 1, binsQIE8.data());
	fillHist(hists, "clusterHPD_adc_18_15", qClusterHPD_18_15, binsQIE8.size() - 1, binsQIE8.data());
	fillHist(hists, "clusterHPD_adc_19_15", qClusterHPD_19_15, binsQIE8.size() - 1, binsQIE8.data());
	fillHist(hists, "clusterHPD_adc_20_15", qClusterHPD_20_15, binsQIE8.size() - 1, binsQIE8.data());

	//Calibrated HPD-only clusters - only does the right thing for old runs pre-SiPM installation (use VME readout to be 'safe')
	double qClusterCalHPD_17_15 = towerSumCal["16_15"] + towerSumCal["17_15"] + towerSumCal["18_15"] + towerSumCal["16_16"] + towerSumCal["17_16"] + towerSumCal["18_16"] + towerSumCal["16_14"] + towerSumCal["17_14"] + towerSumCal["18_14"];
	double qClusterCalHPD_18_15 = towerSumCal["17_15"] + towerSumCal["18_15"] + towerSumCal["19_15"] + towerSumCal["17_16"] + towerSumCal["18_16"] + towerSumCal["19_16"] + towerSumCal["17_14"] + towerSumCal["18_14"] + towerSumCal["19_14"];
	double qClusterCalHPD_19_15 = towerSumCal["18_15"] + towerSumCal["19_15"] + towerSumCal["20_15"] + towerSumCal["18_16"] + towerSumCal["19_16"] + towerSumCal["20_16"] + towerSumCal["18_14"] + towerSumCal["19_14"] + towerSumCal["20_14"];
	double qClusterCalHPD_20_15 = towerSumCal["19_15"] + towerSumCal["20_15"] + towerSumCal["21_15"] + towerSumCal["19_16"] + towerSumCal["20_16"] + towerSumCal["21_16"] + towerSumCal["19_14"] + towerSumCal["20_14"] + towerSumCal["21_14"];
	fillHist(hists, "clusterCalHPD_adc_17_15", qClusterCalHPD_17_15, binsQIE8.size() - 1, binsQIE8.data());
	fillHist(hists, "clusterCalHPD_adc_18_15", qClusterCalHPD_18_15, binsQIE8.size() - 1, binsQIE8.data());
	fillHist(hists, "clusterCalHPD_adc_19_15", qClusterCalHPD_19_15, binsQIE8.size() - 1, binsQIE8.data());
	fillHist(hists, "clusterCalHPD_adc_20_15", qClusterCalHPD_20_15, binsQIE8.size() - 1, binsQIE8.data());

	//Uncalibrated SiPM+HPD clusters
	double qClusterUncalMIP_17_5 = qCluster_17_5 / TBCalibSource::QIE11MIP + (towerSum["16_4"] + towerSum["17_4"] + towerSum["18_4"]) / TBCalibSource::QIE8MIP;
	double qClusterUncalMIP_18_5 = qCluster_18_5 / TBCalibSource::QIE11MIP + (towerSum["17_4"] + towerSum["18_4"] + towerSum["19_4"]) / TBCalibSource::QIE8MIP;
	double qClusterUncalMIP_19_5 = qCluster_19_5 / TBCalibSource::QIE11MIP + (towerSum["18_4"] + towerSum["19_4"] + towerSum["20_4"]) / TBCalibSource::QIE8MIP;
	double qClusterUncalMIP_20_5 = qCluster_20_5 / TBCalibSource::QIE11MIP + (towerSum["19_4"] + towerSum["20_4"] + towerSum["21_4"]) / TBCalibSource::QIE8MIP;
	fillHist(hists, "cluster_MIP_17_5", qClusterUncalMIP_17_5, binsMIP.size() - 1, binsMIP.data());
	fillHist(hists, "cluster_MIP_18_5", qClusterUncalMIP_18_5, binsMIP.size() - 1, binsMIP.data());
	fillHist(hists, "cluster_MIP_19_5", qClusterUncalMIP_19_5, binsMIP.size() - 1, binsMIP.data());
	fillHist(hists, "cluster_MIP_20_5", qClusterUncalMIP_20_5, binsMIP.size() - 1, binsMIP.data());

	//Calibrated SiPM+HPD clusters
	double qClusterMIP_17_5 = qClusterCal_17_5 / TBCalibSource::QIE11MIP + (towerSumCal["16_4"] + towerSumCal["17_4"] + towerSumCal["18_4"]) / TBCalibSource::QIE8MIP;
	double qClusterMIP_18_5 = qClusterCal_18_5 / TBCalibSource::QIE11MIP + (towerSumCal["17_4"] + towerSumCal["18_4"] + towerSumCal["19_4"]) / TBCalibSource::QIE8MIP;
	double qClusterMIP_19_5 = qClusterCal_19_5 / TBCalibSource::QIE11MIP + (towerSumCal["18_4"] + towerSumCal["19_4"] + towerSumCal["20_4"]) / TBCalibSource::QIE8MIP;
	double qClusterMIP_20_5 = qClusterCal_20_5 / TBCalibSource::QIE11MIP + (towerSumCal["19_4"] + towerSumCal["20_4"] + towerSumCal["21_4"]) / TBCalibSource::QIE8MIP;
	fillHist(hists, "clusterCal_MIP_17_5", qClusterMIP_17_5, binsMIP.size() - 1, binsMIP.data());
	fillHist(hists, "clusterCal_MIP_18_5", qClusterMIP_18_5, binsMIP.size() - 1, binsMIP.data());
	fillHist(hists, "clusterCal_MIP_19_5", qClusterMIP_19_5, binsMIP.size() - 1, binsMIP.data());
	fillHist(hists, "clusterCal_MIP_20_5", qClusterMIP_20_5, binsMIP.size() - 1, binsMIP.data());

	//MIP HPD only plots
	fillHist(hists, "clusterCalHPD_MIP_17_15", qClusterCalHPD_17_15 / TBCalibSource::QIE8MIP, binsQIE8.size() - 1, binsMIP8.data());
	fillHist(hists, "clusterCalHPD_MIP_18_15", qClusterCalHPD_18_15 / TBCalibSource::QIE8MIP, binsQIE8.size() - 1, binsMIP8.data());
	fillHist(hists, "clusterCalHPD_MIP_19_15", qClusterCalHPD_19_15 / TBCalibSource::QIE8MIP, binsQIE8.size() - 1, binsMIP8.data());
	fillHist(hists, "clusterCalHPD_MIP_20_15", qClusterCalHPD_20_15 / TBCalibSource::QIE8MIP, binsQIE8.size() - 1, binsMIP8.data());

	//MIP SiPM only plots
	fillHist(hists, "clusterCalSiPM_MIP_17_5", qClusterCal_17_5 / TBCalibSource::QIE11MIP, binsMIP.size() - 1, binsMIP.data());
	fillHist(hists, "clusterCalSiPM_MIP_18_5", qClusterCal_18_5 / TBCalibSource::QIE11MIP, binsMIP.size() - 1, binsMIP.data());
	fillHist(hists, "clusterCalSiPM_MIP_19_5", qClusterCal_19_5 / TBCalibSource::QIE11MIP, binsMIP.size() - 1, binsMIP.data());
	fillHist(hists, "clusterCalSiPM_MIP_20_5", qClusterCal_20_5 / TBCalibSource::QIE11MIP, binsMIP.size() - 1, binsMIP.data());
    }
}


// ------------ method called once each job just before starting event loop  ------------
void adcHists::beginJob()
{
    for(double& binEdge : edges10)
    {
	binsQIE11.push_back(binEdge/gain_);
	binsMIP.push_back(binEdge/(gain_*TBCalibSource::QIE11MIP));
    }
    for(double& binEdge : edges8)  
    {
	binsQIE8.push_back(binEdge);
	binsMIP8.push_back(binEdge/(gain_*TBCalibSource::QIE8MIP));
    }
}

// ------------ method called once each job just after ending the event loop  ------------
void adcHists::endJob()
{
    for(int i = 1; i <= hq->GetNbinsX(); i++)
    {
	hq->SetBinContent(i, hq->GetBinContent(i)/hq->GetBinWidth(i));
    }

    for(auto& hist : hists)
    {
	hist.second->Scale(1.0, "width");
    }
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void adcHists::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(adcHists);

// root script to process data from H2TestBeamAnalyzer
// usage example:
//   $ cmsRun h2testbeamanalyzer_cfg.py 009017
//   $ root
//   [] .x plots.cxx("ana_h2_tb_run009017.root")

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TStyle.h"
#include "TCanvas.h"

#include <iostream>
#include <vector>
using namespace std;

#include "src/format.h"

void plots(const char* inFileName)
{
  // open input file
  cout << "Input file: " << inFileName << endl;
  TFile* in = new TFile(inFileName);
  
  // book trees
  TQIE8Info hbhe;
  TQIE11Info qie11;
  H2Triggers triggers;
  #define NUMWC 5
  vector<double>* wcX[NUMWC];
  vector<double>* wcY[NUMWC];
  
  TTree* treeHBHE = (TTree*) in->Get("HBHEData/Events");
  treeHBHE->SetBranchAddress("numChs", &hbhe.numChs);
  treeHBHE->SetBranchAddress("numTS", &hbhe.numTS);
  treeHBHE->SetBranchAddress("iphi", &hbhe.iphi);
  treeHBHE->SetBranchAddress("ieta", &hbhe.ieta);
  treeHBHE->SetBranchAddress("depth", &hbhe.depth);
  treeHBHE->SetBranchAddress("pulse", hbhe.pulse);
  treeHBHE->SetBranchAddress("pulse_adc", hbhe.pulse_adc);
  treeHBHE->SetBranchAddress("valid", hbhe.valid);
  
  TTree* treeQIE11 = (TTree*) in->Get("QIE11Data/Events");
  treeQIE11->SetBranchAddress("numChs", &qie11.numChs);
  treeQIE11->SetBranchAddress("numTS", &qie11.numTS);
  treeQIE11->SetBranchAddress("iphi", &qie11.iphi);
  treeQIE11->SetBranchAddress("ieta", &qie11.ieta);
  treeQIE11->SetBranchAddress("depth", &qie11.depth);
  treeQIE11->SetBranchAddress("pulse", qie11.pulse);
  treeQIE11->SetBranchAddress("pulse_adc", qie11.pulse_adc);
  treeQIE11->SetBranchAddress("link_error", qie11.link_error);
  
  TTree* treeTriggers = (TTree*) in->Get("Triggers/Events");
  treeTriggers->SetBranchAddress("ped", &triggers.ped);
  treeTriggers->SetBranchAddress("beam", &triggers.beam);
  
  TTree* treeWC = (TTree*) in->Get("WCData/Events");
  treeWC->SetBranchAddress("xA", &(wcX[0]));
  treeWC->SetBranchAddress("yA", &(wcY[0]));
  treeWC->SetBranchAddress("xB", &(wcX[1]));
  treeWC->SetBranchAddress("yB", &(wcY[1]));
  treeWC->SetBranchAddress("xC", &(wcX[2]));
  treeWC->SetBranchAddress("yC", &(wcY[2]));
  treeWC->SetBranchAddress("xD", &(wcX[3]));
  treeWC->SetBranchAddress("yD", &(wcY[3]));
  treeWC->SetBranchAddress("xE", &(wcX[4]));
  treeWC->SetBranchAddress("yE", &(wcY[4]));
  
  // style
  gStyle->SetOptStat("emruo");
  
  // book histograms
  #define NUMPHIS 20
  #define NUMETAS 20
  #define NUMDEPTHS 10
  const char* roNames[2] = {"QIE8", "QIE11"};
  TH1I* hFull[2][NUMPHIS][NUMETAS][NUMDEPTHS];
  TH1I* hPed[2][NUMPHIS][NUMETAS][NUMDEPTHS];
  TH1I* hSignal[2][NUMPHIS][NUMETAS][NUMDEPTHS];
  TProfile* pSignalPulses[2][NUMPHIS][NUMETAS][NUMDEPTHS];
  const char wcNames[NUMWC] = {'A', 'B', 'C', 'D', 'E'};
  TH2I* hWC[NUMWC];
  
  TString name;
  for (int i = 0; i < NUMWC; i++) {
    name.Form("WC_%c XY", wcNames[i]);
    hWC[i] = new TH2I(name, name + ";X;Y", 1000, -100, 100, 1000, -100, 100);
  }
  
  for (int n = 0; n < 2; n++) {
    for (int iphi = 0; iphi < NUMPHIS; iphi++) {
      for (int ieta = 0; ieta < NUMETAS; ieta++) {
        for (int idepth = 0; idepth < NUMDEPTHS; idepth++) {
          name.Form("%s Full_iphi%d_ieta%d_d%d", roNames[n], iphi, ieta, idepth);
          hFull[n][iphi][ieta][idepth] = new TH1I(name, name + ";Charge, fC", 200, -100, 1000);
          
          name.Form("%s Ped_iphi%d_ieta%d_d%d", roNames[n], iphi, ieta, idepth);
          hPed[n][iphi][ieta][idepth] = new TH1I(name, name + ";Charge, fC", 200, -100, 1000);
          
          name.Form("%s Signal_iphi%d_ieta%d_d%d", roNames[n], iphi, ieta, idepth);
          hSignal[n][iphi][ieta][idepth] = new TH1I(name, name + ";Charge, fC", 200, -100, 1000);
          
          name.Form("%s SignalAvgPulse_iphi%d_ieta%d_d%d", roNames[n], iphi, ieta, idepth);
          pSignalPulses[n][iphi][ieta][idepth] = new TProfile(name, name + ";TS;<Charge>, fC", 10, 0, 10);
        }
      }
    }
  }
  
  // read events
  const int numEvents = treeHBHE->GetEntries();
  cout << "Number of events: " << numEvents << endl;
  
  for (int i = 0; i < numEvents; i++) {
    if (i % 1000 == 0)
      cout << "Event: " << i << endl;
    
    treeHBHE->GetEntry(i);
    treeQIE11->GetEntry(i);
    treeTriggers->GetEntry(i);
    treeWC->GetEntry(i);
    
    // skip pedestal events
    if (triggers.ped == 1) {
      continue;
    }
    
    // Wire Chambers data
    for (int iWC = 0; iWC < NUMWC; iWC++) {
      //cout << "WC" << iWC << " #x = " << wcX[iWC]->size() << "  #y = "<< wcY[iWC]->size() << endl;
      
      // plot data only if there is a single hit in X and Y planes
      if ((wcX[iWC]->size() == 1) && (wcY[iWC]->size() == 1)) {
        const double x = wcX[iWC]->at(0);
        const double y = wcY[iWC]->at(0);
        hWC[iWC]->Fill(x, y);
      }
    }
    
    // QIE8 (HBHE) data
    for (int iCh = 0; iCh < hbhe.numChs; iCh++) {
      int iphi = hbhe.iphi[iCh];
      int ieta = hbhe.ieta[iCh];
      int depth = hbhe.depth[iCh];
      
      // debug print
      if (false && (iphi == 14) && (ieta == 17)) {
        cout << "Event: " << i
             << " iphi/ieta/depth = " << iphi << " " << ieta << " " << depth
             << " valid = " << hbhe.valid[iCh]
             << " pulse = ";
        for (int iTS = 0; iTS < hbhe.numTS; iTS++) cout << hbhe.pulse[iCh][iTS] << " ";
        cout << endl;
        cout << "                                                       ";
        for (int iTS = 0; iTS < hbhe.numTS; iTS++) cout << hbhe.pulse_adc[iCh][iTS] << " ";
        cout << endl;
      }
      
      // skip digis with invalid capid flag
      if (! hbhe.valid[iCh]) continue;
      
      double full = 0;
      double ped = 0.;
      double signal = 0;
      
      for (int iTS = 0; iTS < hbhe.numTS; iTS++) {
        const double pulse = hbhe.pulse[iCh][iTS];
        full += pulse;
        if (iTS < 4) ped += pulse;
        if (4 <= iTS && iTS < 8) signal += pulse;
        pSignalPulses[0][iphi][ieta][depth]->Fill(iTS, pulse);
      }
      
      hPed[0][iphi][ieta][depth]->Fill(ped);
      hSignal[0][iphi][ieta][depth]->Fill(signal);
      hFull[0][iphi][ieta][depth]->Fill(full);
    }
    
    // QIE11 data
    for (int iCh = 0; iCh < qie11.numChs; iCh++) {
      int iphi = qie11.iphi[iCh];
      int ieta = qie11.ieta[iCh];
      int depth = qie11.depth[iCh];
      
      // debug print
      if (false && (iphi == 6) && (ieta == 17)) {
        cout << "Event: " << i
             << " iphi/ieta/depth = " << iphi << " " << ieta << " " << depth
             << " link_error = " << qie11.link_error[iCh]
             << " pulse = ";
        for (int iTS = 0; iTS < qie11.numTS; iTS++) cout << qie11.pulse[iCh][iTS] << " ";
        cout << endl;
        cout << "                                                       ";
        for (int iTS = 0; iTS < qie11.numTS; iTS++) cout << qie11.pulse_adc[iCh][iTS] << " ";
        cout << endl;
      }
      
      double full = 0;
      double ped = 0.;
      double signal = 0;
      
      for (int iTS = 0; iTS < qie11.numTS; iTS++) {
        const double pulse = qie11.pulse[iCh][iTS];
        full += pulse;
        if (iTS < 4) ped += pulse;
        if (4 <= iTS && iTS < 8) signal += pulse;
        pSignalPulses[1][iphi][ieta][depth]->Fill(iTS, pulse);
      }
      
      hPed[1][iphi][ieta][depth]->Fill(ped);
      hSignal[1][iphi][ieta][depth]->Fill(signal);
      hFull[1][iphi][ieta][depth]->Fill(full);
    }
  }
  
  // show several plots
  // QIE11 iphi=6,ieta=17,depth=2 and 3
  TCanvas* c = new TCanvas;
  c->Divide(4, 2);
  c->cd(1); pSignalPulses[1][6][17][2]->Draw();
  c->cd(2); hFull[1][6][17][2]->Draw();
  c->cd(3); hPed[1][6][17][2]->Draw();
  c->cd(4); hSignal[1][6][17][2]->Draw();
  
  c->cd(5); pSignalPulses[1][6][17][3]->Draw();
  c->cd(6); hFull[1][6][17][3]->Draw();
  c->cd(7); hPed[1][6][17][3]->Draw();
  c->cd(8); hSignal[1][6][17][3]->Draw();
  
  // wire chambers
  TCanvas* c2 = new TCanvas;
  c2->Divide(3, 2);
  c2->cd(1); hWC[0]->Draw();
  c2->cd(2); hWC[1]->Draw();
  c2->cd(3); hWC[2]->Draw();
  c2->cd(4); hWC[3]->Draw();
  c2->cd(5); hWC[4]->Draw();
}

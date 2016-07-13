#include "Riostream.h"
#include <string>
#include <stdlib.h>
#include <iostream> 
#include <cstdio>
#include <cstdlib>
const int Numberofchannels=48;
TCanvas *c[102];
TCanvas *cFR[102];
int fiber,fiberch;
char label[50],name[70];
TFile *fileinput;
TH1F *hsinglePS[10][100][10][2];
TH1F *hpulse[10][100][2];
float pulse[48][10];
int TSn,fibermin=100,fibermax=0;
//int ieta;
//int depth;
//int channel[10][6];
TH1F *avgPS[10][100][2];
TH1F *maxPS[10][100][2];
TH1F *maxPSTS[10][100][2];
TH1F *weightTS[10][100][2];
TH1F *integQ[10][100][2];

//string listn ;
int ch2eta[Numberofchannels];
int ch2depth[Numberofchannels];
TH2F *spectr[2],*meansignalmap[2],*rmssignalmap[2];

void Naz_averagePS(int runnumber,int uhtr){
  // getting map from file
 ///
 ///
 FILE * in;
 char label1[50];
 sprintf(name,"tb_chanmap.py");
 in = fopen (name, "rt");
 rewind(in);
 fgets(label1,50,in);
 fgets(label1,50,in);
 // printf("%s\n",label1);
 int etaN,depthN,channelN,led_trigger;
 for(int channel=0;channel<Numberofchannels;channel++){
   
     fgets(label1,50,in);
     //printf("%s\n",label1);
     //  printf("%s\n",listn);
     sscanf(label1,"chanmap[%d,8,%d] = %d",&etaN,&depthN,&channelN);
     // channel[etaN][depthN]=channelN;
     //  printf("channel[%d][%d]=%d\n",etaN,depthN,channelN);
     ch2eta[channelN-1]=etaN;
     ch2depth[channelN-1]=depthN;
     
   
 }
 fclose(in);
 //getting fibermin fibermax
for(int channel=0;channel<Numberofchannels;channel++){
  if (ch2depth[channel]>fibermax) fibermax = ch2depth[channel];
  if (ch2depth[channel]<fibermin) fibermin = ch2depth[channel];
 }
 printf("fiber %d - %d\n",fibermin,fibermax);
 ///create histos
 for(int ietan=0;ietan<10;ietan++){
   for(int depthn=fibermin;depthn<fibermax+1;depthn++){
     for(int led=0;led<2;led++){
       for(int TSnn=0;TSnn<10;TSnn++){
	 
	 sprintf(label,"charge eta%d depth%d TS%d led=%d",ietan,depthn,TSnn,led);
	 if(gROOT->FindObject(label))  hsinglePS[ietan][depthn][TSnn][led]=(TH1F*)gROOT->FindObject(label);
	 else	 hsinglePS[ietan][depthn][TSnn][led]= new TH1F(label,label,10000,0,10000);
	 hsinglePS[ietan][depthn][TSnn][led]->Reset();
	
       }
      

       sprintf(label,"avgPS eta%d depth%d led=%d",ietan,depthn,led);
     if(gROOT->FindObject(label))  avgPS[ietan][depthn][led] = (TH1F*)gROOT->FindObject(label);
     else   avgPS[ietan][depthn][led]= new TH1F(label,label,10,0,10);
     avgPS[ietan][depthn][led]->Reset();

     sprintf(label,"pulse eta%d depth%d led=%d",ietan,depthn,led);
     if(gROOT->FindObject(label)) hpulse[ietan][depthn][led] = (TH1F*)gROOT->FindObject(label);
     else   hpulse[ietan][depthn][led]=new TH1F(label,label,10000,0,10000);
     hpulse[ietan][depthn][led]->Reset();

       sprintf(label,"maxPS eta%d depth%d led=%d",ietan,depthn,led);
     if(gROOT->FindObject(label))  maxPS[ietan][depthn][led] = (TH1F*)gROOT->FindObject(label);
     else   maxPS[ietan][depthn][led]= new TH1F(label,label,10000,0,10000);
     maxPS[ietan][depthn][led]->Reset();

       sprintf(label,"TS of maxPS eta%d depth%d led=%d",ietan,depthn,led);
     if(gROOT->FindObject(label))  maxPSTS[ietan][depthn][led] = (TH1F*)gROOT->FindObject(label);
     else   maxPSTS[ietan][depthn][led]= new TH1F(label,label,10,0,10);
     maxPSTS[ietan][depthn][led]->Reset();

       sprintf(label,"weighted TS where max occurs eta%d depth%d led=%d",ietan,depthn,led);
     if(gROOT->FindObject(label))  weightTS[ietan][depthn][led] = (TH1F*)gROOT->FindObject(label);
     else   weightTS[ietan][depthn][led]= new TH1F(label,label,10,0,10);
     weightTS[ietan][depthn][led]->Reset();

       sprintf(label,"integrated pulse (TSmax-1,TSmax+2) eta%d depth%d led=%d",ietan,depthn,led);
     if(gROOT->FindObject(label))  integQ[ietan][depthn][led] = (TH1F*)gROOT->FindObject(label);
     else   integQ[ietan][depthn][led]= new TH1F(label,label,10,0,0);
     integQ[ietan][depthn][led]->Reset();

     }
   }
 }
 for (int led=0;led<2;led++){
   sprintf(label,"spectraVS fiber led=%d",led);
   if(gROOT->FindObject(label))  spectr[led] =  (TH2F*)gROOT->FindObject(label);
   else  spectr[led] = new TH2F(label,label,10*(fibermax-fibermin+1),fibermin*10,(fibermax+1)*10,1000,0,10000);
   spectr[led]->Reset();

   sprintf(label,"mean led=%d",led);
   if(gROOT->FindObject(label)) meansignalmap[led] =  (TH2F*)gROOT->FindObject(label);
   else   meansignalmap[led] = new TH2F(label,label,fibermax-fibermin+1,fibermin,fibermax+1,6,0,6);
   meansignalmap[led]->Reset();

   sprintf(label,"rms led=%d",led);
   if(gROOT->FindObject(label)) rmssignalmap[led] = (TH2F*)gROOT->FindObject(label);
   else  rmssignalmap[led] = new TH2F(label,label,fibermax-fibermin+1,fibermin,fibermax+1,6,0,6);
   rmssignalmap[led]->Reset();

 }
 sprintf(name,"ana_h2_tb_run%.6d_EMAP-kalinin_HTR%d.root",runnumber,uhtr);
 fileinput = TFile::Open(name);
 
 

 //read TTree
 
 TTree *Events = (TTree*)fileinput->Get("QIE11Data/Events");
 Events->SetBranchAddress("pulse",&pulse);
 // Events->SetBranchAddress("TSn",&TSn);
 // Events->SetBranchAddress("ieta",&ieta);
 //Events->SetBranchAddress("depth",&depth);
 Events->SetBranchAddress("led_trigger",&led_trigger);
 int sumTS,sumall;
 float maxOfPulse;
 int maxTS;
 float weightTSnum, weightTSdenom, sumQ;
 //sprintf(cut_criteria,"ieta==%d && depth==%d",2,0);
 Int_t nentries = (Int_t)Events->GetEntries();
 printf("total number of events in tree = %d\n",nentries);
 for (Int_t i=0; i<nentries; i++) {
   if(i%(100+(900*(i>999)))==0)  printf("processing %d event\n",i);
   Events->GetEntry(i);
   for (channelN=0;channelN<Numberofchannels;channelN++){
     sumTS=0;
     sumall=0;
     maxTS=0;
     maxOfPulse=0.;
     weightTSnum=0.;
     weightTSdenom=0.;
     sumQ = 0.;
     for(int TS=0;TS<10;TS++){ // loop over time slices
       
       // if (led_trigger==1){
	 hsinglePS[ch2eta[channelN]][ch2depth[channelN]][TS][led_trigger]->Fill(pulse[channelN][TS]);
	 if((TS>0)&&(TS<5)) sumTS+=pulse[channelN][TS];
	 sumall+=pulse[channelN][TS];
	 if(pulse[channelN][TS] > maxOfPulse)
	   {
	     maxOfPulse = pulse[channelN][TS];
	     maxTS = TS;
	   }
	 // }
     } // loop over time slices

     for(int TS=0;TS<10;TS++){ // loop again over time slices  
	 if (TS >= (maxTS - 1) && (TS <= (maxTS+2)))
	   {
	     weightTSnum+=(TS*pulse[channelN][TS]);
	     weightTSdenom+=pulse[channelN][TS];
	     sumQ+=pulse[channelN][TS];
	   }
     } // loop again over time slices


     // if (led_trigger==1) {
       spectr[led_trigger]->Fill(ch2depth[channelN]*10+ch2eta[channelN],sumTS);
       hpulse[ch2eta[channelN]][ch2depth[channelN]][led_trigger]->Fill(sumall);
       // }
       //std::cout << "Pulse maximum for channel " << channelN << " was " << maxOfPulse << std::endl;
       //std::cout << "  TS at which this max occurred: " << maxTS << std::endl;
       maxPS[ch2eta[channelN]][ch2depth[channelN]][led_trigger]->Fill(maxOfPulse);
       maxPSTS[ch2eta[channelN]][ch2depth[channelN]][led_trigger]->Fill(maxTS);
       weightTS[ch2eta[channelN]][ch2depth[channelN]][led_trigger]->Fill(weightTSnum/weightTSdenom);
       integQ[ch2eta[channelN]][ch2depth[channelN]][led_trigger]->Fill(sumQ);
   }
   }
 
 //Filling Pulse shape histo
  for(int ietan=0;ietan<6;ietan++){
    for(int depthn=fibermin;depthn<fibermax+1;depthn++){
      for (int led=0;led<2;led++){
	for(int TSnn=0;TSnn<10;TSnn++){
	  avgPS[ietan][depthn][led]->Fill(TSnn,hsinglePS[ietan][depthn][TSnn][led]->GetMean());
	}
	meansignalmap[led]->SetBinContent(depthn-fibermin+1,ietan+1,hpulse[ietan][depthn][led]->GetMean());
	rmssignalmap[led]->SetBinContent(depthn-fibermin+1,ietan+1,hpulse[ietan][depthn][led]->GetRMS());
      }
    }
  }
  //Drawing PS histos
  for (fiber=fibermin;fiber<fibermax+1+2;fiber++){
    sprintf(label,"fiber %d",fiber);
    if (fiber==fibermax+1) sprintf(label,"spectrVSchannel");
    if (fiber==fibermax+2) sprintf(label,"mean rms map");
    if (gROOT->FindObject(label)) c[fiber-fibermin] = (TCanvas*)gROOT->FindObject(label);
    else   
      {c[fiber-fibermin]=new TCanvas (label,label,0,0,400,400);
	if (fiber<fibermax+1) c[fiber-fibermin]->Divide(3,2);
	if (fiber==fibermax+1) c[fiber-fibermin]->Divide(1,2);
	if (fiber==fibermax+2)c[fiber-fibermin]->Divide(2,2);
      }
    if (fiber<fibermax+1)
      {
	sprintf(label,"fiber %d - max PS/TS analysis",fiber);
	if (gROOT->FindObject(label)) cFR[fiber-fibermin] = (TCanvas*)gROOT->FindObject(label);
	else{
	  cFR[fiber-fibermin]=new TCanvas (label,label,0,0,400,400);
	  cFR[fiber-fibermin]->Divide(3,2);
	}
      } 
  }

  for(fiber=fibermin;fiber<fibermax+1;fiber++){
    for(fiberch=0;fiberch<6;fiberch++){
      c[fiber-fibermin]->cd(fiberch+1);
      weightTS[fiberch][fiber][1]->SetLineColor(kRed);
      weightTS[fiberch][fiber][1]->Draw();
     // avgPS[fiberch][fiber][0]->Draw("same");

      //to plot maxPS or maxPSTS, just change the name of the histogram being drawn
      cFR[fiber-fibermin]->cd(fiberch+1);
      integQ[fiberch][fiber][1]->SetLineColor(kRed);
      // maxPSTS[fiberch][fiber][0]->Scale(1./maxPSTS[fiberch][fiber][0]->Integral());
      //maxPSTS[fiberch][fiber][1]->Scale(1./maxPSTS[fiberch][fiber][1]->Integral());
      integQ[fiberch][fiber][1]->Draw();
      // maxPSTS[fiberch][fiber][1]->Draw("same");
      if ( integQ[fiberch][fiber][1]->GetMaximum() >  integQ[fiberch][fiber][0]->GetMaximum())
	 integQ[fiberch][fiber][0]->SetMaximum( integQ[fiberch][fiber][1]->GetMaximum()*1.1);
    }
  }
  //c[8]->SetLogz();
  c[fibermax-fibermin+1]->cd(1);
  c[fibermax-fibermin+1]->GetPad(1)->SetLogz();
  spectr[1]->Draw("colz");
  c[fibermax-fibermin+1]->cd(2);
  c[fibermax-fibermin+1]->GetPad(2)->SetLogz();
  spectr[0]->Draw("colz");
  
  c[fibermax-fibermin+2]->cd(1);
  meansignalmap[1]->GetXaxis()->CenterLabels();
  meansignalmap[1]->GetYaxis()->CenterLabels();
  meansignalmap[1]->SetStats(0);
  meansignalmap[1]->Draw("colz text");
  c[fibermax-fibermin+2]->cd(2);
  meansignalmap[0]->GetXaxis()->CenterLabels();
  meansignalmap[0]->GetYaxis()->CenterLabels();
  meansignalmap[0]->SetStats(0);
  meansignalmap[0]->Draw("colz text");
  c[fibermax-fibermin+2]->cd(3);
  rmssignalmap[1]->GetXaxis()->CenterLabels();
  rmssignalmap[1]->GetYaxis()->CenterLabels();
  rmssignalmap[1]->SetStats(0);
  rmssignalmap[1]->Draw("colz text");
  c[fibermax-fibermin+2]->cd(4);
  rmssignalmap[0]->GetXaxis()->CenterLabels();
  rmssignalmap[0]->GetYaxis()->CenterLabels();
  rmssignalmap[0]->SetStats(0);
  rmssignalmap[0]->Draw("colz text");
  fileinput->Close();
}
 

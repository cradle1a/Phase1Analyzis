#include "Riostream.h"
#include <string>
#include <stdlib.h>
#include <iostream> 
#include <cstdio>
#include <cstdlib>
const int Numberofchannels=1000;
int Numberofchannels_emap;

	double const adc2fC[256] = {
			1.58, 4.73, 7.88, 11.0, 14.2, 17.3, 20.5, 23.6, 
			26.8, 29.9, 33.1, 36.2, 39.4, 42.5, 45.7, 48.8,
	  		53.6, 60.1, 66.6, 73.0, 79.5, 86.0, 92.5, 98.9,
	    		105, 112, 118, 125, 131, 138, 144, 151,
		 	157, 164, 170, 177, 186, 199, 212, 225,
		   	238, 251, 264, 277, 289, 302, 315, 328,
		     	341, 354, 367, 380, 393, 406, 418, 431,
			444, 464, 490, 516, 542, 568, 594, 620,
	  		569, 594, 619, 645, 670, 695, 720, 745,
	    		771, 796, 821, 846, 871, 897, 922, 947,
		  	960, 1010, 1060, 1120, 1170, 1220, 1270, 1320,
		    	1370, 1430, 1480, 1530, 1580, 1630, 1690, 1740,
	  		1790, 1840, 1890, 1940,  2020, 2120, 2230, 2330,
	    		2430, 2540, 2640, 2740, 2850, 2950, 3050, 3150,
		  	3260, 3360, 3460, 3570, 3670, 3770, 3880, 3980,
		    	4080, 4240, 4450, 4650, 4860, 5070, 5280, 5490,

	  		5080, 5280, 5480, 5680, 5880, 6080, 6280, 6480,
	    		6680, 6890, 7090, 7290, 7490, 7690, 7890, 8090,
		  	8400, 8810, 9220, 9630, 10000, 10400, 10900, 11300,
		    	11700, 12100, 12500, 12900, 13300, 13700, 14100, 14500,
	  		15000, 15400, 15800, 16200, 16800, 17600, 18400, 19300,
	    		20100, 20900, 21700, 22500, 23400, 24200, 25000, 25800,
		  	26600, 27500, 28300, 29100, 29900, 30700, 31600, 32400,
		    	33200, 34400, 36100, 37700, 39400, 41000, 42700, 44300,
	  		41100, 42700, 44300, 45900, 47600, 49200, 50800, 52500,
	    		54100, 55700, 57400, 59000, 60600, 62200, 63900, 65500,
	  		68000, 71300, 74700, 78000, 81400, 84700, 88000, 91400,
	    		94700, 98100, 101000, 105000, 108000, 111000, 115000, 118000,
	  		121000, 125000, 128000, 131000, 137000, 145000, 152000, 160000,
	    		168000, 176000, 183000, 191000, 199000, 206000, 214000, 222000,
	  		230000, 237000, 245000, 253000, 261000, 268000, 276000, 284000,
	    		291000, 302000, 316000, 329000, 343000, 356000, 370000, 384000
		};


TCanvas *c[102];
int fiber,fiberch;
char label[50],name[70];
TFile *fileinput;
TH1F *hsinglePS[10][100][10][2];
TH1F *TSmax[10][100][2];
TH1F *hpulse[10][100][2];
TH1F *hpulseref[10][100];
float pulse[Numberofchannels][10];
int TSn,fibermin=100,fibermax=0;
//int ieta;
//int depth;
//int channel[10][6];
TH1F *avgPS[10][100][2];
//string listn ;
int ch2eta[Numberofchannels];
int ch2depth[Numberofchannels];
TH2F *spectr[2],*meansignalmap[2],*rmssignalmap[2];
TH2F *reflectionmap,*reflectionentriesmap;
TH1F *gain1d;//= new TH1F("gain1d","gain1d",100,0,100);
TH2F *gain2d;//
int number_TS_to_process=2;
int maxts(float TSvalue[10],int number_TS_to_process);
void averagePSref(int runnumber,int uhtr,int refcheta=2,int refchdepth=5,int whattodraw=0){
  // getting map from file
 ///
 ///
double  low_edge[256];
low_edge[0]=0;
 for(int i=1;i<256;i++){
   low_edge[i]=(adc2fC[i]-adc2fC[i-1])/2+adc2fC[i-1];printf("%d %2f\t %2f\n",i,low_edge[i],adc2fC[i]);
   if (low_edge[i]<low_edge[i-1]){low_edge[i]=low_edge[i-1];printf("!!!%2f\n",low_edge[i]);}
 }//printf("\n");return;
 FILE * in;
 char label1[50];
 sprintf(name,"tb_chanmap_EMAP-kalinin_HTR%d.py",uhtr);
 in = fopen (name, "rt");
 rewind(in);
 fgets(label1,50,in);
 fgets(label1,50,in);
 // printf("%s\n",label1);
 int etaN,depthN,channelN,led_trigger;
 for(int channel=0;channel<Numberofchannels;channel++){
   
   fgets(label1,50,in);
   if (label1[0]=='\n') {printf("channels %d\n",channel);Numberofchannels_emap=channel;break;}// else {printf("exist%d\n",channel);}
   //  printf("%s\n",label1);
     //  printf("%s\n",listn);
     sscanf(label1,"chanmap[%d,8,%d] = %d",&etaN,&depthN,&channelN);
     // channel[etaN][depthN]=channelN;
     //  printf("channel[%d][%d]=%d\n",etaN,depthN,channelN);
     ch2eta[channelN-1]=etaN;
     ch2depth[channelN-1]=depthN;
     
   
 }
 fclose(in);
 //getting fibermin fibermax
for(int channel=0;channel<Numberofchannels_emap;channel++){
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
	 //	 else	 hsinglePS[ietan][depthn][TSnn][led]= new TH1F(label,label,1000000,0,1000000);
	 else	 hsinglePS[ietan][depthn][TSnn][led]= new TH1F(label,label,255,low_edge);
	 hsinglePS[ietan][depthn][TSnn][led]->Reset();
	
       }
      

       sprintf(label,"avgPS eta%d depth%d led=%d",ietan,depthn,led);
     if(gROOT->FindObject(label))  avgPS[ietan][depthn][led] = (TH1F*)gROOT->FindObject(label);
     else   avgPS[ietan][depthn][led]= new TH1F(label,label,10,0,10);
     avgPS[ietan][depthn][led]->Reset();

     sprintf(label,"TSmax eta%d depth%d led=%d",ietan,depthn,led);
     if(gROOT->FindObject(label))  TSmax[ietan][depthn][led] = (TH1F*)gROOT->FindObject(label);
     else   TSmax[ietan][depthn][led]= new TH1F(label,label,10,0,10);
     TSmax[ietan][depthn][led]->Reset();

     sprintf(label,"pulse eta%d depth%d led=%d",ietan,depthn,led);
     if(gROOT->FindObject(label)) hpulse[ietan][depthn][led] = (TH1F*)gROOT->FindObject(label);
     else   hpulse[ietan][depthn][led]=new TH1F(label,label,1500000,0,1500000);
     hpulse[ietan][depthn][led]->Reset();

     }
   
     sprintf(label,"refpulse eta%d depth%d(rech%d %d)",ietan,depthn,refcheta,refchdepth);
     if(gROOT->FindObject(label)) hpulseref[ietan][depthn] = (TH1F*)gROOT->FindObject(label);
     else   hpulseref[ietan][depthn]=new TH1F(label,label,1500000,0,1500000);
     hpulseref[ietan][depthn]->Reset();
   }
 }
 for (int led=0;led<2;led++){
   sprintf(label,"spectraVS fiber led=%d",led);
   if(gROOT->FindObject(label))  spectr[led] =  (TH2F*)gROOT->FindObject(label);
   else  spectr[led] = new TH2F(label,label,10*(fibermax-fibermin+1),fibermin*10,(fibermax+1)*10,1500000,0,1500000);
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
   sprintf(label,"reflection");
   if(gROOT->FindObject(label)) reflectionmap = (TH2F*)gROOT->FindObject(label);
   else  reflectionmap = new TH2F(label,label,fibermax-fibermin+1,fibermin,fibermax+1,6,0,6);
   reflectionmap->Reset();

sprintf(label,"reflectionentriesmap");
   if(gROOT->FindObject(label)) reflectionentriesmap = (TH2F*)gROOT->FindObject(label);
   else  reflectionentriesmap = new TH2F(label,label,fibermax-fibermin+1,fibermin,fibermax+1,6,0,6);
   reflectionentriesmap->Reset();

sprintf(label,"gain1d");
   if(gROOT->FindObject(label)) gain1d = (TH1F*)gROOT->FindObject(label);
   else  gain1d = new TH1F(label,label,100,0,100);
   gain1d->Reset();

sprintf(label,"gain2d");
   if(gROOT->FindObject(label)) gain2d = (TH2F*)gROOT->FindObject(label);
   else  gain2d = new TH2F(label,label,fibermax-fibermin+1,fibermin,fibermax+1,6,0,6);
   gain2d->Reset();

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
 
 //sprintf(cut_criteria,"ieta==%d && depth==%d",2,0);
 Int_t nentries = (Int_t)Events->GetEntries();
 printf("total number of events in tree = %d\n",nentries);



 for (Int_t i=0; i<nentries; i++) {
   if(i%(100+(900*(i>999)))==0)  printf("processing %d event\n",i);
   Events->GetEntry(i);
 
    
   for (channelN=0;channelN<Numberofchannels_emap;channelN++){
     sumTS=0;
     sumall=0;

     int maxtsn=maxts(pulse[channelN],number_TS_to_process);
     TSmax[ch2eta[channelN]][ch2depth[channelN]][led_trigger]->Fill(maxtsn);

     for(int TS=0;TS<10;TS++){
       
       // if (led_trigger==1){
       // if (led_trigger==1 ) hsinglePS[ch2eta[channelN]][ch2depth[channelN]][TS][led_trigger]->Fill(pulse[channelN][TS]);
       //       if ((led_trigger==0)&&(pulse[channelN][TS]<50)) 
       hsinglePS[ch2eta[channelN]][ch2depth[channelN]][TS][led_trigger]->Fill(pulse[channelN][TS]);
       if((TS>=maxtsn)&&(TS<maxtsn+number_TS_to_process)) sumTS+=pulse[channelN][TS];
	 sumall+=pulse[channelN][TS];
	 // }
     }
      if (led_trigger==0) {
       spectr[led_trigger]->Fill(ch2depth[channelN]*10+ch2eta[channelN],sumTS);
       hpulse[ch2eta[channelN]][ch2depth[channelN]][led_trigger]->Fill(sumTS);
       }
      if (led_trigger==1) {
	spectr[led_trigger]->Fill(ch2depth[channelN]*10+ch2eta[channelN],sumTS);
       hpulse[ch2eta[channelN]][ch2depth[channelN]][led_trigger]->Fill(sumTS);
       }

   }
   }

 
 
 //Filling Pulse shape histo
  for(int ietan=0;ietan<6;ietan++){
    for(int depthn=fibermin;depthn<fibermax+1;depthn++){
      for (int led=0;led<2;led++){
	for(int TSnn=0;TSnn<10;TSnn++){
	  avgPS[ietan][depthn][led]->Fill(TSnn,hsinglePS[ietan][depthn][TSnn][led]->GetMean());
	  avgPS[ietan][depthn][led]->SetBinError(TSnn+1,hsinglePS[ietan][depthn][TSnn][led]->GetRMS());
	}
	meansignalmap[led]->SetBinContent(depthn-fibermin+1,ietan+1,hpulse[ietan][depthn][led]->GetMean());
	rmssignalmap[led]->SetBinContent(depthn-fibermin+1,ietan+1,hpulse[ietan][depthn][led]->GetRMS());
      }
    }
  }

//second scan reference
  if (refchdepth+refcheta!=0){
   for (Int_t i=0; i<nentries; i++) {
     if(i%(100+(900*(i>999)))==0)  printf("processing %d event\n",i);
     Events->GetEntry(i);if (!led_trigger) continue;
     //getting refchvalues
     float refchpulse=0;
     for (channelN=0;channelN<Numberofchannels_emap;channelN++){
       //  printf("channelN %d ch2depth %d ch2eta %d\n",channelN,ch2depth[channelN],ch2eta[channelN]);
       if ((ch2depth[channelN]==refchdepth)&&(ch2eta[channelN]==refcheta)){
	 // printf("channelref %d\n",channelN);
	 int maxtsn=maxts(pulse[channelN],number_TS_to_process);
	 for(int TS=0;TS<10;TS++){
	 if((TS>=maxtsn)&&(TS<maxtsn+number_TS_to_process)) refchpulse+=pulse[channelN][TS];  
	 }
       }
     }
       printf("refchpulse %f\n",refchpulse);
       refchpulse-=meansignalmap[0]->GetBinContent(refchdepth-fibermin+1,refcheta+1);
     //end getting refchvalues
     for (channelN=0;channelN<Numberofchannels_emap;channelN++){
       sumTS=0;
       sumall=0;
       
       int maxtsn=maxts(pulse[channelN],number_TS_to_process);
       //  TSmax[ch2eta[channelN]][ch2depth[channelN]][led_trigger]->Fill(maxtsn);
      
       //printf("refchpulse %f\n",refchpulse);
       float normfactor=meansignalmap[1]->GetBinContent(refchdepth-fibermin+1,refcheta+1);
       normfactor-= meansignalmap[0]->GetBinContent(refchdepth-fibermin+1,refcheta+1);
       normfactor=1/refchpulse; //normfactor/refchpulse;
       //  printf("normfactor %f refpulse %f pedestal %f\n ",normfactor,refchpulse,meansignalmap[0]->GetBinContent(refchdepth-fibermin+1,refcheta+1));
       maxtsn=maxts(pulse[channelN],number_TS_to_process);
       for(int TS=0;TS<10;TS++){
	 if((TS>=maxtsn)&&(TS<maxtsn+number_TS_to_process)) sumTS+=pulse[channelN][TS];
	 sumall+=pulse[channelN][TS];
       }
       // hpulseref[ch2eta[channelN]][ch2depth[channelN]]->Fill(normfactor*(sumTS- meansignalmap[0]->GetBinContent(ch2depth[channelN]-fibermin+1,ch2eta[channelN]+1)));
       //  if ((sumTS> (meansignalmap[0]->GetBinContent(ch2depth[channelN]-fibermin+1,ch2eta[channelN]+1)))){
	 hpulseref[ch2eta[channelN]][ch2depth[channelN]]->Fill((sumTS- meansignalmap[0]->GetBinContent(ch2depth[channelN]-fibermin+1,ch2eta[channelN]+1))/refchpulse);
	 printf(" eta %d depth %d refchpulse %f  sumTS %d pedestal %f subtract %f pedref %f\n",ch2eta[channelN],ch2depth[channelN],refchpulse,sumTS,meansignalmap[0]->GetBinContent(ch2depth[channelN]-fibermin+1,ch2eta[channelN]+1),sumTS- meansignalmap[0]->GetBinContent(ch2depth[channelN]-fibermin+1,ch2eta[channelN]+1),meansignalmap[0]->GetBinContent(refchdepth-fibermin+1,refcheta+1));
	 //	 }
     }
   }

   //reflectionmap fill
   for(int ietan=0;ietan<6;ietan++){
     for(int depthn=fibermin;depthn<fibermax+1;depthn++){
       reflectionentriesmap->SetBinContent(depthn-fibermin+1,ietan+1,(hpulseref[ietan][depthn]->GetEntries()-hpulseref[ietan][depthn]->GetBinContent(0))/hpulse[ietan][depthn][1]->GetEntries());
       reflectionmap->SetBinContent(depthn-fibermin+1,ietan+1,hpulseref[ietan][depthn]->GetMean());
				    }
     }
   }
 //end second scan reference

  //Drawing PS histos
  for (fiber=fibermin;fiber<fibermax+1+2+1;fiber++){
    sprintf(label,"fiber %d",fiber);
    if (fiber==fibermax+1) sprintf(label,"spectrVSchannel");
    if (fiber==fibermax+2) sprintf(label,"mean rms map");
    if (fiber==fibermax+3) sprintf(label,"reflectionmap");
    if (gROOT->FindObject(label)) c[fiber-fibermin] = (TCanvas*)gROOT->FindObject(label);
    else   
      {c[fiber-fibermin]=new TCanvas (label,label,0,0,400,400);
	if (fiber<fibermax+1) c[fiber-fibermin]->Divide(3,2);
	if (fiber==fibermax+1) c[fiber-fibermin]->Divide(1,2);
	if (fiber==fibermax+2)c[fiber-fibermin]->Divide(2,2);
      }
  }
  for(fiber=fibermin;fiber<fibermax+1;fiber++){
    for(fiberch=0;fiberch<6;fiberch++){
      c[fiber-fibermin]->cd(fiberch+1);
      //   c[fiber-fibermin]->GetPad(fiberch+1)->SetLogy();
      if (whattodraw==0)
	{
	  int whattodrawfirst,TSmaxvalue=0,TSmax=0;
	  for(int TS=0;TS<20;TS++)
	    {
	      if  (avgPS[fiberch][fiber][int(TS/10)]->GetBinContent(TS%10)>TSmax) {TSmax=TS;TSmaxvalue=avgPS[fiberch][fiber][int(TS/10)]->GetBinContent(TS%10)>TSmax;}
	    }
	  avgPS[fiberch][fiber][1]->SetLineColor(kRed);
	  avgPS[fiberch][fiber][int(TSmax/10)]->Draw();
	  avgPS[fiberch][fiber][1-int(TSmax/10)]->Draw("same");
	}
      if ((whattodraw==1)||(whattodraw==2))
	{  
	  c[fiber-fibermin]->GetPad(fiberch+1)->SetLogy();
	  Double_t par[9];
	  hpulse[fiberch][fiber][whattodraw-1]->Rebin(4);
	  int binmax=   hpulse[fiberch][fiber][whattodraw-1]->GetMaximumBin();
	  int maximumvalue=  hpulse[fiberch][fiber][whattodraw-1]->GetBinCenter(binmax);
	  TF1 *g1    = new TF1("g1","gaus",maximumvalue-20,maximumvalue+20);
	  hpulse[fiberch][fiber][whattodraw-1]->Fit(g1,"R");
	  g1->GetParameters(&par[0]);
	  hpulse[fiberch][fiber][whattodraw-1]->GetXaxis()->SetRangeUser(maximumvalue+20,maximumvalue+100);
	  binmax=   hpulse[fiberch][fiber][whattodraw-1]->GetMaximumBin();
	  maximumvalue=  hpulse[fiberch][fiber][whattodraw-1]->GetBinCenter(binmax);
	  TF1  *g2    = new TF1("g2","gaus",maximumvalue-15,maximumvalue+30);
	  hpulse[fiberch][fiber][whattodraw-1]->Fit(g2,"R+");
	  g2->GetParameters(&par[3]);
	  hpulse[fiberch][fiber][whattodraw-1]->GetXaxis()->SetRangeUser(maximumvalue+20,maximumvalue+100);
	  binmax=   hpulse[fiberch][fiber][whattodraw-1]->GetMaximumBin();
	  maximumvalue=  hpulse[fiberch][fiber][whattodraw-1]->GetBinCenter(binmax);
	  TF1  *g3    = new TF1("g3","gaus",maximumvalue-15,maximumvalue+30);
	  hpulse[fiberch][fiber][whattodraw-1]->Fit(g3,"R+");
	  g3->GetParameters(&par[6]);
	  // TF1 *total = new TF1("total","g1(0)+g2(3)",0,maximumvalue+20);
	  // total->SetParameters(par);
	  //hpulse[fiberch][fiber][whattodraw-1]->Fit(g1,"R");
	  // hpulse[fiberch][fiber][whattodraw-1]->Fit(g2,"R+");
	  // hpulse[fiberch][fiber][whattodraw-1]->Fit(total,"R+");
	  hpulse[fiberch][fiber][whattodraw-1]->GetXaxis()->SetRangeUser(0,hpulse[fiberch][fiber][whattodraw-1]->GetMean()*3);
	  hpulse[fiberch][fiber][whattodraw-1]->Draw();

	  //begin gain calculation
	 
	  gain1d->Fill(par[4]-par[1]);

	  gain2d->SetBinContent(fiber-fibermin+1,fiberch+1,(par[4]-par[1])/(par[7]-par[4]));
	  gain2d->GetZaxis()->SetRangeUser(0.9,1.1);
	  // end gain calculation

	}
    }
  }
  //c[8]->SetLogz();
 
  int maxrange[2]={0,0};
  for (int led=0;led<2;led++){
    for (channelN=0;channelN<Numberofchannels_emap;channelN++){
      if (maxrange[led]<hpulse[ch2eta[channelN]][ch2depth[channelN]][led]->GetMean()) 
	maxrange[led]=hpulse[ch2eta[channelN]][ch2depth[channelN]][led]->GetMean();
    }
  }
  spectr[0]->GetYaxis()->SetRangeUser(0,maxrange[0]*4);
  spectr[1]->GetYaxis()->SetRangeUser(0,maxrange[1]*4);
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
  c[fibermax-fibermin+3]->Divide(2,1);
  c[fibermax-fibermin+3]->cd(1);
  reflectionmap->GetZaxis()->SetRangeUser(0,0.005);
  reflectionmap->SetStats(0);
  reflectionmap->Draw("colz text");
  c[fibermax-fibermin+3]->cd(2);
  reflectionentriesmap->GetZaxis()->SetRangeUser(0,1);
  reflectionentriesmap->SetStats(0);
  reflectionentriesmap->Draw("colz text");
  fileinput->Close();
  //outfile
  /*FILE * out;
 //char label1[50];
 sprintf(name,"reflections.txt");
 out = fopen (name, "a");
   for(int ietan=0;ietan<6;ietan++){
     for(int depthn=fibermin;depthn<fibermax+1;depthn++){
       fprintf(out,"%d\t%d\t%d\t%d\t%f\t%f\n",refchdepth,refcheta,depthn,ietan,reflectionentriesmap->GetBinContent(depthn-fibermin+1,ietan+1),reflectionmap->GetBinContent(depthn-fibermin+1,ietan+1));
	 }
     }
     fclose(out);*/
   //end outfile
}
 
int maxts(float TSvalue[10],int number_TS_to_process){
  // return 2;
  int max[10]={0,0,0,0,0,0,0,0,0,0};
  int currentmax=0,currentmin=0,currentmaxcounter=0;
  for(TSn=0;TSn<10;TSn++){
    if (max[currentmax]<TSvalue[TSn]){currentmax=TSn;currentmin=TSn;max[currentmax]=TSvalue[currentmax];}
  }
  currentmaxcounter++;
  while (((currentmaxcounter<=number_TS_to_process)&&(currentmin>=0)&&(currentmax<=9))){
  
  if ((currentmaxcounter==number_TS_to_process)||(currentmin==0)) return currentmin;
  if (currentmax==9) return currentmax-number_TS_to_process;

  if (TSvalue[currentmin-1]>TSvalue[currentmax+1]){currentmin--;} else {currentmax++;}
  currentmaxcounter++;
 
  }

  return 0;
}

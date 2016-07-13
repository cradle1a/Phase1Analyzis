#include "Riostream.h"
#include <string>
#include <stdlib.h>
#include <iostream> 
#include <cstdio>
#include <cstdlib>
int deptheta2BV(int depth,int eta,int RM);
void plotreflection(){
 FILE *in;
 TH2F *reflectionentriesmap,*reflectionmap,*onerun,*onerunBV;
 TH1F *reflectionentriesmap1d,*reflectionmap1d;
 reflectionentriesmap= new TH2F("reflectionentriesmap","reflectionentriesmap",48,0,48,48,0,48);
 reflectionmap= new TH2F("reflectionmap","reflectionmap",48,0,48,48,0,48);
 onerun= new TH2F("onerun","onerun",8,0,8,6,0,6);
 onerunBV= new TH2F("onerunBV","onerunBV",8,0,8,6,0,6);
 reflectionentriesmap1d= new TH1F("reflectionentriesmap1d","reflectionentriesmap1d",50,0,2);
 reflectionmap1d= new TH1F("","RM CrossTalk",10000,0,1.1);
int refchdepth[48*48],refcheta[48*48],depth[48*48],eta[48*48];
float probability[48*48],ratio[48*48];
char name[50],label1[50];
 sprintf(name,"reflections.txt");
 in = fopen (name, "rt");
 rewind(in);
// fgets(label1,50,in);
//fgets(label1,50,in);
 // printf("%s\n",label1);
 int etaN,depthN,channelN,led_trigger;
 for(int i=0;i<48*48;i++){
   
     fgets(label1,50,in);    
     sscanf(label1,"%d\t%d\t%d\t%d\t%f\t%f\n",&refchdepth[i],&refcheta[i],&depth[i],&eta[i],&probability[i],&ratio[i]);
     printf("%d %d\n",refchdepth[i],refcheta[i]);
     reflectionentriesmap->SetBinContent((refchdepth[i]-14)*6+refcheta[i]+1,(depth[i]-14)*6+eta[i]+1,probability[i]);
     if (probability[i]>0.7) reflectionmap->SetBinContent((refchdepth[i]-14)*6+refcheta[i]+1,(depth[i]-14)*6+eta[i]+1,ratio[i]);
     reflectionentriesmap1d->Fill(probability[i]);
     if (probability[i]>0.99) reflectionmap1d->Fill(ratio[i]);
     if ((probability[i]>0.99)&&(refchdepth[i]==19)&&(refcheta[i]==4)) onerun->SetBinContent((depth[i]-14)+1,eta[i]+1,ratio[i]);
     if ((probability[i]>0.99)&&(refchdepth[i]==19)&&(refcheta[i]==4)) onerunBV->SetBinContent((deptheta2BV(depth[i],eta[i],4)-1)%8+1,int((deptheta2BV(depth[i],eta[i],4)-1)/8)+1,ratio[i]);

 }
 reflectionentriesmap->SetStats(0);
 //Int_t colors[] = {8, 1, 1, 1, 1, 1, 1,9,9,4,4,4,5}; // #colors >= #levels - 1
 //gStyle->SetPalette((sizeof(colors)/sizeof(Int_t)), colors);
 reflectionentriesmap->GetZaxis()->SetRangeUser(0,1.11);
 reflectionmap->GetZaxis()->SetRangeUser(0,0.02);
 reflectionmap->SetStats(0);
 TCanvas *tt1= new TCanvas("fraction of events more then Mean of pedestal","fraction of events more then Mean of pedestal",0,0,400,400);
 tt1->cd();
 reflectionentriesmap->Draw("colz");
 TCanvas *tt2= new TCanvas("crosstalk value","crosstalk value",0,0,400,400);
 tt2->cd();
 reflectionmap->Draw("colz");
 TCanvas *tp1= new TCanvas("fraction of events more then Mean of pedestal1d","fraction of events more then Mean of pedestal1d",0,0,400,400);
 tp1->cd();
 reflectionentriesmap1d->Draw("");
 TCanvas *tp2= new TCanvas("crosstalk value1d","crosstalk value1d",0,0,800,800);
 tp2->cd();
 reflectionmap1d->Draw("");
 TCanvas *tz1= new TCanvas("onerun","onerun",0,0,800,800);
 tz1->cd();
 onerun->SetStats(0);
 onerun->GetZaxis()->SetRangeUser(0.0001,1);
 tz1->SetLogz();
 onerun->Draw("colz text");

 TCanvas *tz2= new TCanvas("onerunBV","onerunBV",0,0,800,800);
 tz2->cd();
 onerunBV->SetStats(0);
 onerunBV->GetZaxis()->SetRangeUser(0.0001,1);
 tz2->SetLogz();
 onerunBV->Draw("colz text");
 fclose(in);
}

int deptheta2BV(int depth,int eta,int RM){
  int BV;
  if ((depth==14) && (eta==0)) BV=40;
  if ((depth==14) && (eta==1)) BV=48;
  if ((depth==14) && (eta==2)) BV=21;
  if ((depth==14) && (eta==3)) BV=22;
  if ((depth==14) && (eta==4)) BV=23;
  if ((depth==14) && (eta==5)) BV=24;

  if ((depth==15) && (eta==0)) BV=7;
  if ((depth==15) && (eta==1)) BV=6;
  if ((depth==15) && (eta==2)) BV=5;
  if ((depth==15) && (eta==3)) BV=4;
  if ((depth==15) && (eta==4)) BV=15;
  if ((depth==15) && (eta==5)) BV=3;

  if ((depth==16) && (eta==0)) BV=45;
  if ((depth==16) && (eta==1)) BV=31;
  if ((depth==16) && (eta==2)) BV=39;
  if ((depth==16) && (eta==3)) BV=46;
  if ((depth==16) && (eta==4)) BV=32;
  if ((depth==16) && (eta==5)) BV=47;

  if ((depth==17) && (eta==0)) BV=8;
  if ((depth==17) && (eta==1)) BV=16;
  if ((depth==17) && (eta==2)) BV=14;
  if ((depth==17) && (eta==3)) BV=13;
  if ((depth==17) && (eta==4)) BV=30;
  if ((depth==17) && (eta==5)) BV=29;

  if ((depth==18) && (eta==0)) BV=41;
  if ((depth==18) && (eta==1)) BV=33;
  if ((depth==18) && (eta==2)) BV=35;
  if ((depth==18) && (eta==3)) BV=43;
  if ((depth==18) && (eta==4)) BV=42;
  if ((depth==18) && (eta==5)) BV=18;

  if ((depth==19) && (eta==0)) BV=38;
  if ((depth==19) && (eta==1)) BV=37;
  if ((depth==19) && (eta==2)) BV=44;
  if ((depth==19) && (eta==3)) BV=36;
  if ((depth==19) && (eta==4)) BV=28;
  if ((depth==19) && (eta==5)) BV=34;

  if ((depth==20) && (eta==0)) BV=1;
  if ((depth==20) && (eta==1)) BV=2;
  if ((depth==20) && (eta==2)) BV=10;
  if ((depth==20) && (eta==3)) BV=11;
  if ((depth==20) && (eta==4)) BV=12;
  if ((depth==20) && (eta==5)) BV=17;

  if ((depth==21) && (eta==0)) BV=9;
  if ((depth==21) && (eta==1)) BV=27;
  if ((depth==21) && (eta==2)) BV=26;
  if ((depth==21) && (eta==3)) BV=25;
  if ((depth==21) && (eta==4)) BV=20;
  if ((depth==21) && (eta==5)) BV=19;

  return BV;


}


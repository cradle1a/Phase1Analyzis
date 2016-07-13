#include "Riostream.h"
#include <string>
#include <stdlib.h>
#include <iostream> 
#include <cstdio>
#include <cstdlib>
TCanvas *c[8];
int fiber,fiberch;
char label[50],name[70],cut_criteria[80];
TFile *fileinput;
TH1F *htmp;
float pulse[48][10];
int TSn;
int ieta;
int depth;
FILE *maptxt;
int channel[10][10];
TH1F *avgPS[10][10];
TProfile *avgPS2[10][10];
string listn ;
int ch2eta[50];
int ch2depth[50];
TH2F *htmp2[10][10];

void averagePS2(int runnumber){

 for(int ietan=0;ietan<10;ietan++){
   for(int depthn=0;depthn<10;depthn++){
     /*   for(int TSnn=0;TSnn<10;TSnn++){
       sprintf(label,"charge eta%d depth%d TS%d",ietan,depthn,TSnn);
       //      htmp[ietan][depthn][TSnn]= new TH1F(label,label,10000,0,10000);
       }*/
     sprintf(label,"avgPS eta%d depth%d",ietan,depthn);
       avgPS[ietan][depthn]= new TH1F(label,label,10,0,10);
   }
 }
 sprintf(name,"ana_h2_tb_run%.6d_EMAP-kalinin.root",runnumber);
 fileinput = TFile::Open(name);
 FILE * in;


 char label1[50];
 sprintf(name,"tb_chanmap.py");
 in = fopen (name, "rt");
 rewind(in);
 fgets(label1,50,in);
 fgets(label1,50,in);
 printf("%s\n",label1);
 int etaN,depthN,channelN,led_trigger;
 for(int ietan=0;ietan<6;ietan++){
   for(int depthn=2;depthn<10;depthn++){
     fgets(label1,50,in);
     printf("%s\n",label1);
     //  printf("%s\n",listn);
     sscanf(label1,"chanmap[%d,8,%d] = %d",&etaN,&depthN,&channelN);
     // channel[etaN][depthN]=channelN;
     //  printf("channel[%d][%d]=%d\n",etaN,depthN,channelN);
     ch2eta[channelN-1]=etaN;
     ch2depth[channelN-1]=depthN;
     
   }
 }


 fclose(in);
 TTree *Events = (TTree*)fileinput->Get("QIE11Data/Events");
 /*Events->SetBranchAddress("pulse",&pulse);
 // Events->SetBranchAddress("TSn",&TSn);
 Events->SetBranchAddress("ieta",&ieta);
 Events->SetBranchAddress("depth",&depth);
 Events->SetBranchAddress("led_trigger",&led_trigger);
 */
 fileinput->cd("QIE11Data");
 for(int ietan=0;ietan<6;ietan++){
   printf("processing ieta=%d\n",ietan);
   for (int depthn=2;depthn<3;depthn++){
     printf("processing depth=%d\n",depthn);
     /* for (int TSn=0;TSn<10;TSn++){
       printf("processing TS=%d\n",TSn);
       sprintf(cut_criteria,"ieta==%d && depth==%d",ietan,depthn);
       sprintf(label,"pulse[][%d]>>htmp(1000,0,10000)",TSn);
       Events->Draw(label,cut_criteria,"");
       htmp=(TH1F*)gDirectory->Get("htmp");
       avgPS[ietan][depthn]->Fill(TSn,htmp->GetMean());
       htmp->Clear();
       }*/
     sprintf(cut_criteria,"ieta==%d && depth==%d",ietan,depthn);
     sprintf(label,"pulse:TSn>>htmp2%d-%d",ieta,depth);
     Events->Draw(label,cut_criteria,"");
     sprintf(label,"htmp2%d-%d",ieta,depth);
     htmp2[ieta][depth]=(TH2F*)gDirectory->Get(label);
     avgPS2[ietan][depthn]=htmp2[ieta][depth]->ProfileX();
     // htmp2->Clear(); 

   }
  
 }
 // printf("%f\n",htmp->GetMean());
 /*Int_t nentries = (Int_t)Events->GetEntries();
 printf("total number of events in tree = %d\n",nentries);
  for (Int_t i=0; i<nentries; i++) {
    Events->GetEntry(i);
    printf("entry %d eta%d depth%d\n",i,ieta,depth);
    //    printf("%f\n",pulse[18][1]);
    for(int TS=0;TS<10;TS++){
      for (channelN=0;channelN<48;channelN++){
	if (led_trigger==1)
    htmp[ch2eta[channelN]][ch2depth[channelN]][TS]->Fill(pulse[channelN][TS]);
    //hpxpy->Fill(px,py);
			  }
	 }
      }
  // c[0]->cd();
  //  htmp[2][0][0]->Draw();
 // Events->Draw("pulse[][0]>>htmp(10000,0,10000)",cut_criteria,"");
  //  printf("%f %d\n",htmp[2][0][0]->GetMean(),channel[ieta][depth]);
 
  for(int ietan=0;ietan<10;ietan++){
    for(int depthn=0;depthn<6;depthn++){
      for(int TSnn=0;TSnn<10;TSnn++){
	avgPS[ietan][depthn]->Fill(TSnn,htmp[ietan][depthn][TSnn]->GetMean());
      }
    }
    }*/

for (fiber=2;fiber<10;fiber++){
  sprintf(label,"fiber %d",fiber);
  c[fiber-2]=new TCanvas (label,label,0,0,400,400);
  c[fiber-2]->Divide(3,2);
 }
 for(int depthn=2;depthn<3;depthn++){
   for(int ietan=0;ietan<6;ietan++){
     c[depthn-2]->cd(ietan+1);
     avgPS2[ietan][depthn]->Draw();
   }
 }
 // fileinput->Close();
}
 

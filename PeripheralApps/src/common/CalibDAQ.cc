//-----------------------------------------------------------------------
// $Id: CalibDAQ.cc,v 1.4 2009/08/10 15:03:15 liu Exp $
// $Log: CalibDAQ.cc,v $
// Revision 1.4  2009/08/10 15:03:15  liu
// expand configure() to include a second argument: configID
//
// Revision 1.3  2009/03/25 11:37:22  liu
// move header files to include/emu/pc
//
// Revision 1.2  2008/08/13 11:30:52  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 1.1  2007/12/26 11:23:47  liu
// new parser and applications
//
// Revision 3.12  2007/08/06 14:18:23  rakness
// comment out many unused variables
//
// Revision 3.11  2007/07/17 16:28:40  liu
// replace CCBStartTrigger() with CCB->startTrigger() & CCB->bc0()
//
// Revision 3.10  2006/11/10 16:51:44  mey
// Update
//
// Revision 3.9  2006/11/07 23:21:02  mey
// Update
//
// Revision 3.8  2006/11/07 17:12:36  mey
// UPdate
//
// Revision 3.7  2006/10/19 09:42:03  rakness
// remove old ALCTController
//
// Revision 3.6  2006/10/18 15:50:56  mey
// UPdate
//
// Revision 3.5  2006/10/12 17:52:12  mey
// Update
//
// Revision 3.4  2006/10/04 14:23:56  mey
// UPdate
//
// Revision 3.3  2006/09/14 09:11:34  mey
// Update
//
// Revision 3.2  2006/09/13 14:13:32  mey
// Update
//
// Revision 3.1  2006/09/06 12:38:18  mey
// Update
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.48  2006/07/19 09:09:57  mey
// Update
//
// Revision 2.47  2006/07/18 15:23:31  mey
// UPdate
//
// Revision 2.46  2006/07/18 12:21:55  rakness
// ALCT threshold scan with ALCTNEW
//
// Revision 2.45  2006/07/17 11:56:19  mey
// Included CFEB coonectivity calibration
//
// Revision 2.44  2006/07/14 12:33:26  mey
// New XML structure
//
// Revision 2.43  2006/07/14 11:46:31  rakness
// compiler switch possible for ALCTNEW
//
// Revision 2.42  2006/07/12 12:28:02  mey
// Update
//
// Revision 2.41  2006/07/12 12:07:11  mey
// ALCT connectivity
//
// Revision 2.40  2006/06/03 19:31:04  mey
// UPdate
//
// Revision 2.39  2006/05/30 13:13:01  mey
// UPdate
//
// Revision 2.38  2006/05/30 09:16:38  mey
// Changed Threshold routine
//
// Revision 2.37  2006/05/30 07:22:20  mey
// UPdate
//
// Revision 2.36  2006/05/24 11:10:00  mey
// Update
//
// Revision 2.35  2006/05/23 14:52:38  mey
// Update
//
// Revision 2.34  2006/05/23 14:11:11  mey
// Update
//
// Revision 2.33  2006/05/23 13:17:49  mey
// Update
//
// Revision 2.32  2006/05/23 13:07:33  mey
// Update
//
// Revision 2.31  2006/05/23 09:01:21  rakness
// Update
//
// Revision 2.30  2006/05/22 19:38:16  mey
// UPdate
//
// Revision 2.29  2006/05/22 17:45:19  mey
// Update
//
// Revision 2.28  2006/05/22 17:16:41  mey
// Update
//
// Revision 2.27  2006/05/22 17:14:23  mey
// Update
//
// Revision 2.26  2006/05/22 17:11:58  mey
// Update
//
// Revision 2.25  2006/05/22 16:20:39  mey
// UPdate
//
// Revision 2.24  2006/05/22 14:07:39  rakness
// Update
//
// Revision 2.23  2006/05/19 15:13:32  mey
// UPDate
//
// Revision 2.22  2006/05/19 12:46:48  mey
// Update
//
// Revision 2.21  2006/05/18 08:35:44  mey
// Update
//
// Revision 2.20  2006/05/17 14:16:44  mey
// Update
//
// Revision 2.19  2006/04/28 13:41:17  mey
// Update
//
// Revision 2.18  2006/04/27 18:46:04  mey
// UPdate
//
// Revision 2.17  2006/04/25 14:50:45  mey
// Update
//
// Revision 2.16  2006/04/25 13:25:19  mey
// Update
//
// Revision 2.15  2006/04/24 14:57:21  mey
// Update
//
// Revision 2.14  2006/04/20 15:33:54  mey
// Update
//
// Revision 2.13  2006/04/19 15:19:06  mey
// Update
//
// Revision 2.12  2006/04/18 08:17:29  mey
// UPdate
//
// Revision 2.11  2006/04/11 15:27:42  mey
// Update
//
// Revision 2.10  2006/03/30 13:55:38  mey
// Update
//
// Revision 2.9  2006/03/27 09:51:42  mey
// UPdate
//
// Revision 2.8  2006/03/27 09:22:36  mey
// Update
//
// Revision 2.7  2006/03/24 16:40:36  mey
// Update
//
// Revision 2.6  2006/03/24 14:35:04  mey
// Update
//
// Revision 2.5  2006/03/07 09:22:58  mey
// Update
//
// Revision 2.4  2006/02/20 13:31:14  mey
// Update
//
// Revision 2.3  2006/02/15 22:39:57  mey
// UPdate
//
// Revision 2.2  2006/01/18 19:38:16  mey
// Fixed bugs
//
// Revision 2.1  2006/01/11 17:00:01  mey
// Update
//
// Revision 2.0  2005/06/06 10:01:03  geurts
// calibration routines by Alex Tumanov and Jason Gilmore
//
//
//-----------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <unistd.h> // for sleep
#include <string>
#include "emu/pc/CalibDAQ.h"
#include "emu/pc/Crate.h"
#include "emu/pc/DAQMB.h"
#include "emu/pc/CCB.h"
#include "emu/pc/JTAG_constants.h"
#include "emu/pc/ChamberUtilities.h"
#include "emu/pc/EmuEndcap.h"

namespace emu {
  namespace pc {

CalibDAQ::CalibDAQ(EmuEndcap *endcap){
  myEndcap = endcap;
}

void CalibDAQ::rateTest() {
  //
  //  int chip,ch,brd, nstrip;
  int nstrip;
  //  int counter = 0;
  float dac;
  //
  pulseAllDMBsInit();
  //
  std::vector<Crate*> myCrates = myEndcap->crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    //    CCB * ccb = myCrates[j]->ccb();
    std::vector<DAQMB*> myDmbs = myEndcap->daqmbs(myCrates[j]);
    std::vector<TMB*> myTmbs   = myEndcap->tmbs(myCrates[j]);
    //
    for (nstrip=0;nstrip<16;nstrip++) {  
      for (int j=0; j<10; j++) {
	dac=0.2+0.2*j;
	//
	for (int tries=0;tries<20; tries++){
	  pulseAllDMBs(-1,nstrip,dac,0);
	}
	//
      } //end of loop by strips
    } //endof loop by amplitudes
  }//end of loop by crates
}
//
void CalibDAQ::pulseComparatorPulse(){
  //
  for (int thresh=0; thresh<35; thresh++) {
    for (int i=0;i<16;i++) {  
      injectComparator(25, i, 0.15, 1000,thresh*0.003+0.013);
    }
  }
  //
  for (int thresh=0; thresh<35; thresh++) {
    for (int i=0;i<16;i++) {  
      injectComparator(25, i, 0.35, 1000,thresh*0.003+0.049);
    }
  }
  //
}
//
void CalibDAQ::pulseAllWires(){
  //
  std::vector<Crate*> myCrates = myEndcap->crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    usleep(100);
    //
    std::vector<TMB*>   myTmbs   = myEndcap->tmbs(myCrates[j]);
    //
    for (unsigned i=0; i<myTmbs.size(); i++) {
      //myTmbs[i]->DisableCLCTInputs();
      myTmbs[i]->ResetALCTRAMAddress();
      //myTmbs[i]->scope(1,0,0);
      //
    }
    //
    CCB * ccb = myCrates[j]->ccb();
    //
    //
    std::cout << "0x28= " << std::hex << ccb->ReadRegister(0x28) << std::endl;
    std::cout << "0x20= " << std::hex << ccb->ReadRegister(0x20) << std::endl;
    //
    ::usleep(500);
    ccb->GenerateAlctAdbSync();	 
    ::usleep(500);
    //
  }
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    std::vector<TMB*>   myTmbs   = myEndcap->tmbs(myCrates[j]);
    std::vector<DAQMB*> myDmbs = myEndcap->daqmbs(myCrates[j]);
    for (unsigned i=0; i<myTmbs.size(); i++) {
      myTmbs[i]->DecodeALCT();
      myTmbs[i]->GetCounters();
      myTmbs[i]->PrintCounters();
      int WordCount = myTmbs[i]->GetALCTWordCount();
      std::cout << "WordCount = " << WordCount <<std::endl ;
      //
    }
    for (unsigned i=0; i<myDmbs.size(); i++) {
      myDmbs[i]->PrintCounters();
    }
  }
  //
}
//
/*
void CalibDAQ::pulseAllWires(){
  //
  std::vector<Crate*> myCrates = myEndcap->crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    usleep(100);
    //
    std::vector<TMB*>   myTmbs   = myEndcap->tmbs(myCrates[j]);
    //
    for (unsigned i=0; i<myTmbs.size(); i++) {
      myTmbs[i]->DisableCLCTInputs();
      myTmbs[i]->ResetALCTRAMAddress();
      //myTmbs[i]->scope(1,0,0);
      //
    }
    //
    CCB * ccb = myCrates[j]->ccb();
    //
    ccb->setCCBMode(CCB::VMEFPGA);
    ccb->WriteRegister(0x20,0xc7f9);  //Only enable adb_async as l1a source
    ccb->WriteRegister(0x04,0x0001);  //Softreset
    ccb->WriteRegister(0x28,0x7878);  //4Aug05 DM changed 0x789b to 0x7862
    //
    std::cout << "0x28= " << std::hex << ccb->ReadRegister(0x28) << std::endl;
    std::cout << "0x20= " << std::hex << ccb->ReadRegister(0x20) << std::endl;
    //
    //::usleep(500000);
    ::usleep(2000);
    ccb->GenerateAlctAdbSync();	 
    ::usleep(600000);
    //
  }
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    std::vector<TMB*>   myTmbs   = myEndcap->tmbs(myCrates[j]);
    for (unsigned i=0; i<myTmbs.size(); i++) {
      myTmbs[i]->DecodeALCT();
      //myTmbs[i]->ForceScopeTrigger();
      int WordCount = myTmbs[i]->GetALCTWordCount();
      std::cout << "WordCount = " << WordCount <<std::endl ;
      //myTmbs[i]->ALCTRawhits();
      //
    }
  }
  //
}
*/
//
void CalibDAQ::timeCFEB() { 
  //
  float dac;
  //  int counter=0;
  //  int nsleep = 100;  
  dac = 1.0;
  //
  std::cout << "timeCFEBtest" << std::endl;
  //
  // setup the TMB and CCB
  std::vector<Crate*> myCrates = myEndcap->crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    std::vector<DAQMB*> myDmbs   = myEndcap->daqmbs(myCrates[j]);
    std::vector<TMB*>   myTmbs   = myEndcap->tmbs(myCrates[j]);
    //
    for (unsigned i=0; i<myTmbs.size(); i++) {
      myTmbs[i]->DisableCLCTInputs();
      std::cout << "Disabling inputs for slot " << myTmbs[i]->slot() << std::endl;
      myTmbs[i]->DisableALCTInputs();
    }
  }
  //
  //
  for (int nstrip=0;nstrip<16;nstrip++) {  
    //
    for(unsigned j = 0; j < myCrates.size(); ++j) {
      //
      std::vector<DAQMB*> myDmbs   = myEndcap->daqmbs(myCrates[j]);
      //
      for(unsigned i =0; i < myDmbs.size(); ++i) {
	//
	// set amplitude
	//
	myDmbs[i]->set_cal_dac(dac,dac);
	myDmbs[i]->fxpreblkend(6); // Set pre block end to 6
	//myDmbs[i]->set_comp_thresh(thresh);
	//
	// set external pulser for strip # nstrip on all 6 chips
	for(int brd=0;brd<5;brd++){
	  for(int chip=0;chip<6;chip++){
	    for(int ch=0;ch<16;ch++){
	      myDmbs[i]->shift_array[brd][chip][ch]=NORM_RUN;
	    }
	    if ( nstrip != -1 ) myDmbs[i]->shift_array[brd][chip][nstrip]=EXT_CAP;
	  }
	}
	//
	myDmbs[i]->buck_shift();
	//::usleep(1000);
      }
    }
    //
    //
    for (int ntim=0;ntim<20;ntim++) {
      //
      for(unsigned j = 0; j < myCrates.size(); ++j) {
	//
	std::vector<DAQMB*> myDmbs   = myEndcap->daqmbs(myCrates[j]);
	//
	for(unsigned i=0; i < myDmbs.size(); ++i) {
	  std::cout << "Dmbs i="<< i << std::endl;
	  std::cout << "Dmbs slot="<< myDmbs[i]->slot() << std::endl;
	  myDmbs[i]->set_cal_tim_pulse(ntim);
	}
	//::usleep(2000);
	//
	CCB * ccb = myCrates[j]->ccb();
	// (myCrates[j]->chamberUtilsMatch())[0].CCBStartTrigger();
        ccb->startTrigger();
        ccb->bc0();
	//
	//::usleep(2000);
	//
	std::cout << "Sending pulse" <<std::endl;
	//
	ccb->pulse(1, 0xff);//pulse all dmbs in this crate
	//
	//::usleep(2000);
	//
      }
      //
    }
  }
  //
}
//
void CalibDAQ::gainCFEB() { 
  //
  float dac;
  //  int counter=0;
  //  int nsleep = 100;  
  dac = 1.0;
  //
  std::cout << "CFEB Gain test" << std::endl;
  //
  // setup the TMB and CCB
  std::vector<Crate*> myCrates = myEndcap->crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    std::vector<DAQMB*> myDmbs   = myEndcap->daqmbs(myCrates[j]);
    std::vector<TMB*>   myTmbs   = myEndcap->tmbs(myCrates[j]);
    //
    for (unsigned i=0; i<myTmbs.size(); i++) {
      myTmbs[i]->DisableCLCTInputs();
      std::cout << "Disabling inputs for slot " << myTmbs[i]->slot() << std::endl;
      myTmbs[i]->DisableALCTInputs();
    }
  }
  //
  //
  for (int nstrip=0;nstrip<16;nstrip++) {  
    //
    for(unsigned j = 0; j < myCrates.size(); ++j) {
      //
      std::vector<DAQMB*> myDmbs   = myEndcap->daqmbs(myCrates[j]);
      //
      for(unsigned i =0; i < myDmbs.size(); ++i) {
	//
	// set amplitude
	//
	//	myDmbs[i]->set_cal_time_pulse(pulse_delay_);
	myDmbs[i]->fxpreblkend(6); // Set pre block end to 6
	//
	for(int brd=0;brd<5;brd++){
	  for(int chip=0;chip<6;chip++){
	    for(int ch=0;ch<16;ch++){
	      myDmbs[i]->shift_array[brd][chip][ch]=NORM_RUN;
	    }
	    if ( nstrip != -1 ) myDmbs[i]->shift_array[brd][chip][nstrip]=EXT_CAP;
	  }
	}
	//
	myDmbs[i]->buck_shift();
      }
    }
    //
    //
    for (int ngain=0;ngain<10;ngain++) {
      dac= 0.2 + 0.2*ngain;
      //
      for(unsigned j = 0; j < myCrates.size(); ++j) {
	//
	std::vector<DAQMB*> myDmbs   = myEndcap->daqmbs(myCrates[j]);
	//
	for(unsigned i=0; i < myDmbs.size(); ++i) {
 	  myDmbs[i]->set_cal_dac(dac,dac);
	}
	::usleep(1000);
	//
	CCB * ccb = myCrates[j]->ccb();
	//(myCrates[j]->chamberUtilsMatch())[0].CCBStartTrigger();
        ccb->startTrigger();
        ccb->bc0();
	//
	::usleep(1000);
	//
	for (int jpulse=0;jpulse<20;jpulse++)
	{ std::cout << "Sending pulse" <<std::endl;
	//
	  ccb->pulse(1, 0xff);//pulse all dmbs in this crate
	//
	  ::usleep(10000);
	}
	//
      }
      //
    }
  }
  //
}
//
void CalibDAQ::pedestalCFEB() { 
  //
  //  int counter=0;
  //  int nsleep = 100;  
  //
  std::cout << "CFEB Pedestal" << std::endl;
  //
  // setup the TMB and CCB
  std::vector<Crate*> myCrates = myEndcap->crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    std::vector<DAQMB*> myDmbs   = myEndcap->daqmbs(myCrates[j]);
    std::vector<TMB*>   myTmbs   = myEndcap->tmbs(myCrates[j]);
    //
    for (unsigned i=0; i<myTmbs.size(); i++) {
      myTmbs[i]->DisableCLCTInputs();
      std::cout << "Disabling inputs for slot " << myTmbs[i]->slot() << std::endl;
      myTmbs[i]->DisableALCTInputs();
    }
  }
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    std::vector<DAQMB*> myDmbs   = myEndcap->daqmbs(myCrates[j]);
    //
    for(unsigned i =0; i < myDmbs.size(); ++i) {
      for(int brd=0;brd<5;brd++){
	for(int chip=0;chip<6;chip++){
	  for(int ch=0;ch<16;ch++){
	    myDmbs[i]->shift_array[brd][chip][ch]=NORM_RUN;
	  }
	}
      }
      //
      myDmbs[i]->fxpreblkend(6); // Set pre block end to 6
      myDmbs[i]->buck_shift();
      myDmbs[i]->set_cal_dac(0.01,0.01);
    }
    //
  ::usleep(1000);
    //
    CCB * ccb = myCrates[j]->ccb();
    //(myCrates[j]->chamberUtilsMatch())[0].CCBStartTrigger();
        ccb->startTrigger();
        ccb->bc0();
    //
    ::usleep(1000);
    //
  }
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    CCB * ccb = myCrates[j]->ccb();
    for (int jpulse=0;jpulse<1000;jpulse++)
      { std::cout << "Sending pulse" <<std::endl;
      //
      ccb->pulse(1, 0xff);//pulse all dmbs in this crate
      //
      ::usleep(10000);
      }
  }
}
//
void CalibDAQ::CFEBSaturationTest() { 
  //
  float dac;
  //  int counter=0;
  //  int nsleep = 100;  
  //
  std::cout << "CFEB Gain test" << std::endl;
  //
  // setup the TMB and CCB
  std::vector<Crate*> myCrates = myEndcap->crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    std::vector<DAQMB*> myDmbs   = myEndcap->daqmbs(myCrates[j]);
    std::vector<TMB*>   myTmbs   = myEndcap->tmbs(myCrates[j]);
    //
    for (unsigned i=0; i<myTmbs.size(); i++) {
      myTmbs[i]->DisableCLCTInputs();
      std::cout << "Disabling inputs for slot " << myTmbs[i]->slot() << std::endl;
      myTmbs[i]->DisableALCTInputs();
    }
  }
  //
  //
  for (int nstrip=0;nstrip<16;nstrip++) {  
    //
    for(unsigned j = 0; j < myCrates.size(); ++j) {
      //
      std::vector<DAQMB*> myDmbs   = myEndcap->daqmbs(myCrates[j]);
      //
      for(unsigned i =0; i < myDmbs.size(); ++i) {
	//
	for(int brd=0;brd<5;brd++){
	  for(int chip=0;chip<6;chip++){
	    for(int ch=0;ch<16;ch++){
	      myDmbs[i]->shift_array[brd][chip][ch]=NORM_RUN;
	    }
	    if ( nstrip != -1 ) myDmbs[i]->shift_array[brd][chip][nstrip]=EXT_CAP;
	  }
	}
	//
	myDmbs[i]->buck_shift();
      }
    }
    //
    //
    for (int ngain=0;ngain<10;ngain++) {
      dac= 3.0 + 0.2*ngain;
      //
      for(unsigned j = 0; j < myCrates.size(); ++j) {
	//
	std::vector<DAQMB*> myDmbs   = myEndcap->daqmbs(myCrates[j]);
	//
	for(unsigned i=0; i < myDmbs.size(); ++i) {
 	  myDmbs[i]->set_cal_dac(dac,dac);
	}
	::usleep(1000);
	//
	CCB * ccb = myCrates[j]->ccb();
	//(myCrates[j]->chamberUtilsMatch())[0].CCBStartTrigger();
        ccb->startTrigger();
        ccb->bc0();
	//
	::usleep(1000);
	//
	for (int jpulse=0;jpulse<20;jpulse++)
	{ std::cout << "Sending pulse" <<std::endl;
	//
	  ccb->pulse(1, 0xff);//pulse all dmbs in this crate
	//
	  ::usleep(10000);
	}
	//
      }
      //
    }
  }
}
//
void CalibDAQ::CFEBSaturation() { 
  //
  float dac;
  int counter=0;
  int nsleep = 100;  
  //
  pulseAllDMBsInit();
  //
  for (int nstrip=0;nstrip<16;nstrip++) {  
    for (int j=0; j<24; j++) {
      dac=3.0+0.2*j;
      for (int ntim=0;ntim<20;ntim++) {	  
	pulseAllDMBs(1, nstrip, dac, nsleep);  
	counter++;
	std::cout << "dac = " << dac <<
	  "  strip = " << nstrip <<
	  "  ntim = " << ntim <<
	  "  event  = " << counter << std::endl;
      }
    }
  }
  //
}
//
void CalibDAQ::pulseAllDMBsPre(int ntim, int nstrip, float dac, int nsleep,int calType) { 
  //
  int chip,ch,brd;
  std::vector<Crate*> myCrates = myEndcap->crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    std::vector<DAQMB*> myDmbs   = myEndcap->daqmbs(myCrates[j]);
    std::vector<TMB*>   myTmbs   = myEndcap->tmbs(myCrates[j]);
    //
    for(unsigned i =0; i < myDmbs.size(); ++i) {
      //
      // set amplitude
      //
      myDmbs[i]->set_cal_dac(dac,dac);
      //myDmbs[i]->fxpreblkend(6); // Set pre block end to 6
      //myDmbs[i]->set_comp_thresh(thresh);
      //
      // set external pulser for strip # nstrip on all 6 chips
      for(brd=0;brd<5;brd++){
	for(chip=0;chip<6;chip++){
	  for(ch=0;ch<16;ch++){
	    myDmbs[i]->shift_array[brd][chip][ch]=NORM_RUN;
	  }
	  if ( nstrip != -1 ) myDmbs[i]->shift_array[brd][chip][nstrip]=EXT_CAP;
	}
      }
      //
      myDmbs[i]->buck_shift();
    }
  }
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    std::vector<DAQMB*> myDmbs   = myEndcap->daqmbs(myCrates[j]);
    std::vector<TMB*>   myTmbs   = myEndcap->tmbs(myCrates[j]);
    //
    for(unsigned i =0; i < myDmbs.size(); ++i) {
      //
      if (ntim!=-1) myDmbs[i]->set_cal_tim_pulse(ntim);   
      //
    }
  }
}
//
void CalibDAQ::pulseAllDMBsInit() { 
  //
  //  int chip,ch,brd;
  std::vector<Crate*> myCrates = myEndcap->crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    std::vector<DAQMB*> myDmbs   = myEndcap->daqmbs(myCrates[j]);
    std::vector<TMB*>   myTmbs   = myEndcap->tmbs(myCrates[j]);
    //
    for (unsigned i=0; i<myTmbs.size(); i++) {
      myTmbs[i]->DisableCLCTInputs();
      std::cout << "Disabling inputs for slot " << myTmbs[i]->slot() << std::endl;
      myTmbs[i]->DisableALCTInputs();
    }
    //
  }
}
//
void CalibDAQ::pulseAllDMBsPost(int ntim, int nstrip, float dac, int nsleep,int calType) { 
  //
  std::vector<Crate*> myCrates = myEndcap->crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    std::vector<DAQMB*> myDmbs   = myEndcap->daqmbs(myCrates[j]);
    std::vector<TMB*>   myTmbs   = myEndcap->tmbs(myCrates[j]);
    //
    for(unsigned i =0; i < myDmbs.size(); ++i) {
      std::cout << "Slot " << myDmbs[i]->slot();
      myDmbs[i]->PrintCounters();
    }
    //
    std::cout <<"After"<<std::endl;
    for(unsigned i =0; i < myTmbs.size(); ++i) {
      myTmbs[i]->GetCounters();
      myTmbs[i]->PrintCounters();
      myTmbs[i]->ResetCounters();
      myTmbs[i]->DisableCLCTInputs();
    }
    //
  }
}
//
void CalibDAQ::pulseAllDMBs(int ntim, int nstrip, float dac, int nsleep,int calType) { 
  //
  //injects identical pulse to all dmbs
  //in all crates one crate at a time          
  //disable TMBs and ALCts
  //
  pulseAllDMBsPre(ntim, nstrip, dac, nsleep,calType);
  //
  std::vector<Crate*> myCrates = myEndcap->crates();
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    CCB * ccb = myCrates[j]->ccb();
    //(myCrates[j]->chamberUtilsMatch())[0].CCBStartTrigger();
        ccb->startTrigger();
        ccb->bc0();
    //
    ::usleep(1000);
    //
    std::cout << "Sending pulse" <<std::endl;
    //
    if(calType==2){
      ccb->pedestal(1,0xff);
    } else {
      ccb->pulse(1, 0xff);//pulse all dmbs in this crate
    }
    //
    ::usleep(1000);
    //
  }
  //
  //pulseAllDMBsPost(ntim, nstrip, dac, nsleep,calType);
  //
}
//
void CalibDAQ::injectComparator(int ntim, int nstrip, float dac, int nsleep, float thresh) { 
  //
  //injects identical pulse to all dmbs
  //in all crates one crate at a time          
  //
  //  int chip,ch,brd;
  std::vector<Crate*> myCrates = myEndcap->crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    CCB * ccb = myCrates[j]->ccb();
    //(myCrates[j]->chamberUtilsMatch())[0].CCBStartTrigger();
        ccb->startTrigger();
        ccb->bc0();
    usleep(100);
    //
    std::vector<DAQMB*> myDmbs = myEndcap->daqmbs(myCrates[j]);
    std::vector<TMB*> myTmbs   = myEndcap->tmbs(myCrates[j]);
    //
    for (unsigned i=0; i<myTmbs.size(); i++) {
      myTmbs[i]->DisableCLCTInputs();
      //myTmbs[i]->EnableInternalL1aSequencer();
      std::cout << "Disabling inputs for slot " << myTmbs[i]->slot() << std::endl;
      myTmbs[i]->DisableALCTInputs();
    }
    //
    for(unsigned i =0; i < myDmbs.size(); ++i) {
      //
      // set amplitude
      //
      myDmbs[i]->settrgsrc(0); //Disable out L1a generation
      myDmbs[i]->set_cal_dac(dac,dac);
      myDmbs[i]->set_comp_thresh(thresh);
      //
      // set external pulser for strip # nstrip on all 6 chips
      //for(brd=0;brd<5;brd++){
      //for(chip=0;chip<6;chip++){
      //  for(ch=0;ch<16;ch++){
      //    myDmbs[i]->shift_array[brd][chip][ch]=NORM_RUN;
      //  }
      //  if ( nstrip != -1 ) myDmbs[i]->shift_array[brd][chip][nstrip]=SMALL_CAP;
      //}
      //}
      //
      //myDmbs[i]->buck_shift();
      //
      int HalfStrip = nstrip ;
      //
      int hp[6] = {HalfStrip, HalfStrip, HalfStrip, HalfStrip, HalfStrip, HalfStrip};       
      //
      // Set the pattern
      //
      myDmbs[i]->trigsetx(hp);
      //
      //set timing
      //ntim is the same as pulse_delay initially set in xml configuration file
      //
      myDmbs[i]->set_cal_tim_pulse(ntim);   
      //
    }
    //
    ::usleep(1000);
    //
    for (unsigned i=0; i<myTmbs.size(); i++) {
      myTmbs[i]->EnableCLCTInputs(0x1f);
    }
    //
    usleep(1000);
    //
    std::cout << "Sending inject" <<std::endl;
    ccb->inject(1, 0xff);//pulse all dmbs in this crate
    //
    usleep(1000);
    //
    for(unsigned i =0; i < myDmbs.size(); ++i) {
      std::cout << "Slot " << myDmbs[i]->slot();
      myDmbs[i]->PrintCounters();
    }
    //
    std::cout <<"After"<<std::endl;
    for(unsigned i =0; i < myTmbs.size(); ++i) {
      myTmbs[i]->GetCounters();
      myTmbs[i]->PrintCounters();
      myTmbs[i]->ResetCounters();
      myTmbs[i]->DisableCLCTInputs();
    }
    //
  }
}
//
void CalibDAQ::FindL1aDelayALCT() { 
  //
  int counter    [300][9];
  int counterC   [300][9];
  int DMBCounter [300][9];
  int DMBCounterALCT[300][9];
  int DMBCounterTMB[300][9];
  int DMBCounterCFEB[300][9];
  //
  int nmin=75;
  int nmax=90;
  //
  for(int i=0; i<300;i++) for(int j=0;j<9;j++) {
    counter[i][j]     = 0;
    counterC[i][j]     = 0;
    DMBCounter[i][j]  = 0;
    DMBCounterALCT[i][j] = 0;
    DMBCounterTMB[i][j] = 0;
    DMBCounterCFEB[i][j] = 0;
  }
  //
  std::vector<Crate*> myCrates = myEndcap->crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    CCB * ccb = myCrates[j]->ccb();
    //(myCrates[j]->chamberUtilsMatch())[0].CCBStartTrigger();
        ccb->startTrigger();
        ccb->bc0();
    usleep(100);
  }
  //
  for(int delay=nmin;delay<nmax;delay++){
    //
    for (int npulses=0; npulses<1; npulses++) {
      for(unsigned j = 0; j < myCrates.size(); j++) {
	CCB * ccb = myCrates[j]->ccb();
	ccb->ResetL1aCounter();
	ccb->EnableL1aCounter();
	std::vector<TMB*> myTmbs = myEndcap->tmbs(myCrates[j]);
	for (unsigned i=0; i<myTmbs.size(); i++) {
	  myTmbs[i]->alctController()->ReadConfigurationReg();	  
	  myTmbs[i]->alctController()->SetL1aDelay(delay);
	  myTmbs[i]->lvl1_delay(delay);
	  myTmbs[i]->ResetCounters();
	  myTmbs[i]->alctController()->SetSendEmpty(0);
	  myTmbs[i]->alctController()->SetL1aInternal(0);
	  myTmbs[i]->alctController()->WriteConfigurationReg();
	  //	  
	  myTmbs[i]->alctController()->SetUpPulsing();
	  myTmbs[i]->SetALCTPatternTrigger();
	}
      }
      //
      pulseAllWires();
      //
      for(unsigned j = 0; j < myCrates.size(); j++) {
	CCB * ccb = myCrates[j]->ccb();
	//
	std::cout << "CCB " << std::hex << ccb << std::endl;
	//
	std::cout << "CCB L1a counter = " << ccb->ReadL1aCounter() << std::endl ;
	std::vector<TMB*> myTmbs = myEndcap->tmbs(myCrates[j]);
	std::vector<DAQMB*> myDmbs = myEndcap->daqmbs(myCrates[j]);
	for (unsigned i=0; i<myTmbs.size(); i++) {
	  counter[delay][i] += myTmbs[i]->GetALCTWordCount();
	  myTmbs[i]->GetCounters();
	  //
	  myTmbs[i]->PrintCounters(8);  // display them to screen
	  myTmbs[i]->PrintCounters(19);
	  myTmbs[i]->PrintCounters(20);
	  //
	  counterC[delay][i] += myTmbs[i]->GetCounter(19);
	  //
	}
	for (unsigned i=0; i<myDmbs.size(); i++) {
	  myDmbs[i]->PrintCounters();
	  DMBCounterALCT[delay][i] = myDmbs[i]->GetAlctDavCounter();
	  DMBCounterTMB[delay][i]  = myDmbs[i]->GetTmbDavCounter();
	  DMBCounterCFEB[delay][i] = myDmbs[i]->GetCfebDavCounter();
	  if ( myDmbs[i]->GetAlctDavCounter() > 0 )  DMBCounter[delay][i] += myDmbs[i]->GetAlctDavCounter();
	}
      }
      //
    }
  }
  //
  for(unsigned j = 0; j < myCrates.size(); j++) {
    std::vector<TMB*> myTmbs = myEndcap->tmbs(myCrates[j]);
    std::vector<DAQMB*> myDmbs = myEndcap->daqmbs(myCrates[j]);
    std::cout << std::endl;
    for(int delay=nmin; delay<nmax;delay++) {
      std::cout << std::dec << std::setw(3) << delay << " " ;
      for(unsigned i =0; i < myTmbs.size(); ++i) std::cout << std::dec << std::setw(3) << counter[delay][i] << " ";
      std::cout << " | " ;
      for(unsigned i =0; i < myTmbs.size(); ++i) std::cout << std::dec << std::setw(3) << counterC[delay][i] << " ";
      std::cout << " | " ;
      for(unsigned i =0; i < myDmbs.size(); ++i) std::cout << std::dec << std::setw(3) << DMBCounter[delay][i] << " ";
      std::cout << " | " ;
      for(unsigned i =0; i < myTmbs.size(); ++i) std::cout << std::dec << std::setw(3) << DMBCounterALCT[delay][i] << " ";
      std::cout << " | " ;
      for(unsigned i =0; i < myTmbs.size(); ++i) std::cout << std::dec << std::setw(3) << DMBCounterTMB[delay][i] << " ";
      std::cout << " | " ;
      for(unsigned i =0; i < myDmbs.size(); ++i) std::cout << std::dec << std::setw(3) << DMBCounterCFEB[delay][i] << " ";
      std::cout << std::endl;
    }
  }
  //
}
//
void CalibDAQ::ALCTThresholdScan() { 
  //
  int counter    [300][9];
  int counterC   [300][9];
  int DMBCounter [300][9];
  int DMBCounter0[300][9];
  //
  //  int nmin=0;
  //  int nmax=200;
  int Counter=0;
  //
  int Npulses = 100;
  //
  for(int i=0; i<300;i++) for(int j=0;j<9;j++) {
    counter[i][j]     = 0;
    counterC[i][j]     = 0;
    DMBCounter[i][j]  = 0;
    DMBCounter0[i][j] = 0;
  }
  //
  std::vector<Crate*> myCrates = myEndcap->crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    CCB * ccb = myCrates[j]->ccb();
    //(myCrates[j]->chamberUtilsMatch())[0].CCBStartTrigger();
        ccb->startTrigger();
        ccb->bc0();
    usleep(100);
  }
  //
    //
    for(unsigned j = 0; j < myCrates.size(); j++) {
      CCB * ccb = myCrates[j]->ccb();
      ccb->ResetL1aCounter();
      ccb->EnableL1aCounter();
      //
      ccb->setCCBMode(CCB::VMEFPGA);
      ccb->WriteRegister(0x20,0xc7f9);  //Only enable adb_sync as l1a source
      ccb->WriteRegister(0x04,0x0001);  //Softreset
      ccb->WriteRegister(0x28,0x7878);  //4Aug05 DM changed 0x789b to 0x7862
      //
      std::vector<TMB*> myTmbs = myEndcap->tmbs(myCrates[j]);
      for (unsigned i=0; i<myTmbs.size(); i++) {
	myTmbs[i]->lvl1_delay(70);
	ALCTController * alct = myTmbs[i]->alctController() ;
	alct->SetL1aDelay(82);
	alct->SetSendEmpty(0);
	alct->SetL1aInternal(0);
	alct->SetPretrigNumberOfLayers(1);
	alct->SetPretrigNumberOfPattern(1);
	alct->WriteConfigurationReg();
	//alct->ReadConfigurationReg();
	//alct->PrintConfigurationReg();
	//
	//
	myTmbs[i]->SetCLCTPatternTrigger();
	myTmbs[i]->DisableCLCTInputs();
	myTmbs[i]->ResetCounters();
      }	
    }
    //
    for(int thres=0; thres<40; thres++) {      
      for (int iter=0; iter<2;iter++){
	//
	for(unsigned j = 0; j < myCrates.size(); j++) {
	  std::vector<TMB*> myTmbs = myEndcap->tmbs(myCrates[j]);
	  for (unsigned i=0; i<myTmbs.size(); i++) {
	    ALCTController * alct = myTmbs[i]->alctController() ;
	    //alct->SetUpPulsing();
	    if(iter==0) {
	      alct->SetUpPulsing(32);
	    }
	    else {alct->SetUpPulsing(48);}
	    //
	    int nAFEBS = alct->GetNumberOfAfebs();
	    std::cout << "nAFEBS = " << nAFEBS << std::endl;
	    for(int afebs=0; afebs<nAFEBS; afebs++) {
	      alct->SetAfebThreshold(afebs,thres);	    
	    }
	    alct->WriteAfebThresholds();
	    ::usleep(200);
	    alct->ReadAfebThresholds();
	    alct->PrintAfebThresholds();
	    //
	  }
	}
	//
	for (int npulses=0; npulses<Npulses; npulses++) {
	  //
	  pulseAllWires();
	  Counter++;
	}
      }
    }
    //
}
//
void CalibDAQ::ALCTConnectivity() { 
  //
  int counter    [300][9];
  int counterC   [300][9];
  int DMBCounter [300][9];
  int DMBCounter0[300][9];
  //
  //  int nmin=0;
  //  int nmax=200;
  int Counter=0;
  //
  int Npulses = 200;
  //
  for(int i=0; i<300;i++) for(int j=0;j<9;j++) {
    counter[i][j]     = 0;
    counterC[i][j]     = 0;
    DMBCounter[i][j]  = 0;
    DMBCounter0[i][j] = 0;
  }
  //
  std::vector<Crate*> myCrates = myEndcap->crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    CCB * ccb = myCrates[j]->ccb();
    //(myCrates[j]->chamberUtilsMatch())[0].CCBStartTrigger();
        ccb->startTrigger();
        ccb->bc0();
    usleep(100);
  }
  //
    for(unsigned j = 0; j < myCrates.size(); j++) {
      CCB * ccb = myCrates[j]->ccb();
      ccb->ResetL1aCounter();
      ccb->EnableL1aCounter();
      //
      ccb->setCCBMode(CCB::VMEFPGA);
      ccb->WriteRegister(0x20,0xc7f9);  //Only enable adb_sync as l1a source
      ccb->WriteRegister(0x04,0x0001);  //Softreset
      ccb->WriteRegister(0x28,0x7878);  //4Aug05 DM changed 0x789b to 0x7862
      //
      std::vector<TMB*> myTmbs = myEndcap->tmbs(myCrates[j]);
      for (unsigned i=0; i<myTmbs.size(); i++) {
	myTmbs[i]->alctController()->SetL1aDelay(82);
	myTmbs[i]->lvl1_delay(70);
	myTmbs[i]->alctController()->SetSendEmpty(0);
	myTmbs[i]->alctController()->SetL1aInternal(0);
	myTmbs[i]->alctController()->SetPretrigNumberOfLayers(1);
	myTmbs[i]->alctController()->SetPretrigNumberOfPattern(1);
	myTmbs[i]->alctController()->WriteConfigurationReg();
	//
	myTmbs[i]->SetCLCTPatternTrigger();
	myTmbs[i]->DisableCLCTInputs();
	myTmbs[i]->ResetCounters();
	//
	ALCTController * alct = myTmbs[i]->alctController() ;
	int nAFEBS = alct->GetNumberOfAfebs();
	for(int afebs=0; afebs<nAFEBS; afebs++) {
	  alct->SetAfebThreshold(afebs,10);	    
	}
	alct->WriteAfebThresholds();
	::usleep(200);
	//
      }
    }
    //
    for(int layer=0; layer<6; layer++){
      //
      for(unsigned j = 0; j < myCrates.size(); j++) {
	std::vector<TMB*> myTmbs = myEndcap->tmbs(myCrates[j]);
	for (unsigned i=0; i<myTmbs.size(); i++) {
	  ALCTController * alct = myTmbs[i]->alctController() ;
	  alct->SetUpPulsing(0x55,PULSE_LAYERS,(1<<layer),ADB_SYNC);
	  ::usleep(200);
	}
      }
      for (int npulses=0; npulses<Npulses; npulses++) {
	pulseAllWires();
	Counter++;
      }
    }
    //
}
//
void CalibDAQ::CFEBConnectivity() { 
  //
  int counter    [300][9];
  int counterC   [300][9];
  int DMBCounter [300][9];
  int DMBCounter0[300][9];
  //
  //  int nmin=0;
  //  int nmax=200;
  int Counter=0;
  //
  int Npulses = 10;
  //
  for(int i=0; i<300;i++) for(int j=0;j<9;j++) {
    counter[i][j]     = 0;
    counterC[i][j]     = 0;
    DMBCounter[i][j]  = 0;
    DMBCounter0[i][j] = 0;
  }
  //
  std::vector<Crate*> myCrates = myEndcap->crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    CCB * ccb = myCrates[j]->ccb();
    //(myCrates[j]->chamberUtilsMatch())[0].CCBStartTrigger();
        ccb->startTrigger();
        ccb->bc0();
    usleep(100);
  }
  //
  for (int npulses=0; npulses<Npulses; npulses++) {
    for(int layer=0; layer<6; layer++){
      for(unsigned j = 0; j < myCrates.size(); j++) {
	CCB * ccb = myCrates[j]->ccb();
	ccb->ResetL1aCounter();
	ccb->EnableL1aCounter();
	std::vector<TMB*> myTmbs = myEndcap->tmbs(myCrates[j]);
	for (unsigned i=0; i<myTmbs.size(); i++) {
	  myTmbs[i]->alctController()->SetL1aDelay(83); // Set out of time
	  myTmbs[i]->lvl1_delay(70);
	  //myTmbs[i]->ResetCounters();
	  myTmbs[i]->alctController()->SetSendEmpty(0);
	  myTmbs[i]->alctController()->SetL1aInternal(0);
	  myTmbs[i]->alctController()->SetPretrigNumberOfLayers(1);
	  myTmbs[i]->alctController()->SetPretrigNumberOfPattern(1);
	  myTmbs[i]->alctController()->WriteConfigurationReg();
	  //
	  //	  ALCTController * alct = myTmbs[i]->alctController() ;
	  //
	  myTmbs[i]->alctController()->SetUpPulsing(0xff,PULSE_LAYERS,0xff,ADB_SYNC);
	  myTmbs[i]->SetALCTPatternTrigger();
	}
      }
      //
      pulseAllWires();
      Counter++;
      //
      std::cout << "Event = " << Counter <<std::endl;
      //
      for(unsigned j = 0; j < myCrates.size(); j++) {
	//	CCB * ccb = myCrates[j]->ccb();
	//
	std::vector<TMB*> myTmbs = myEndcap->tmbs(myCrates[j]);
	std::vector<DAQMB*> myDmbs = myEndcap->daqmbs(myCrates[j]);
	for (unsigned i=0; i<myTmbs.size(); i++) {
	  myTmbs[i]->GetCounters();
	  //
	  myTmbs[i]->PrintCounters();
	  //
	}
	for (unsigned i=0; i<myDmbs.size(); i++) {
	  myDmbs[i]->PrintCounters();
	}
      }
    }
    //
  }
  //
}
//

void CalibDAQ::FindL1aDelayComparator() { 
//injects identical pulse to all dmbs (EXT capacitors)
//in all crates one crate at a time          
  //
  //  int chip,ch,brd;
  int counter[200][9];
  //
  int ntim=25;
  float thresh = 0.1;
  float dac = 0.4;
  //
  for(int i=0; i<200;i++) for(int j=0;j<9;j++) counter[i][j] = 0;
  //
  std::vector<Crate*> myCrates = myEndcap->crates();
  //
  for(int delay=0;delay<200;delay++){
    for(unsigned j = 0; j < myCrates.size(); ++j) {
      //
      CCB * ccb = myCrates[j]->ccb();
      //(myCrates[j]->chamberUtilsMatch())[0].CCBStartTrigger();
        ccb->startTrigger();
        ccb->bc0();
      usleep(100);
      //
      std::vector<DAQMB*> myDmbs = myEndcap->daqmbs(myCrates[j]);
      std::vector<TMB*> myTmbs   = myEndcap->tmbs(myCrates[j]);
      //
      for (unsigned i=0; i<myTmbs.size(); i++) {
	myTmbs[i]->DisableCLCTInputs();
	myTmbs[i]->lvl1_delay(delay);
	myTmbs[i]->EnableInternalL1aSequencer();
	std::cout << "Disabling inputs for slot " << myTmbs[i]->slot() << std::endl;
	myTmbs[i]->DisableALCTInputs();
      }
      //
      for(unsigned i =0; i < myDmbs.size(); ++i) {
	//
	// set amplitude
	//
	myDmbs[i]->set_cal_dac(dac,dac);
	myDmbs[i]->set_comp_thresh(thresh);
	//
	//myDmbs[i]->set_comp_thresh(0.4);
	//
	// set external pulser for strip # nstrip on all 6 chips
	//for(brd=0;brd<5;brd++){
	//for(chip=0;chip<6;chip++){
	//  for(ch=0;ch<16;ch++){
	//    myDmbs[i]->shift_array[brd][chip][ch]=NORM_RUN;
	//  }
	//  if ( nstrip != -1 ) myDmbs[i]->shift_array[brd][chip][nstrip]=SMALL_CAP;
	//}
	//}
	//
	//myDmbs[i]->buck_shift();
	//
	int HalfStrip = 16;
	//
	int hp[6] = {HalfStrip, HalfStrip, HalfStrip, HalfStrip, HalfStrip, HalfStrip};       
	//
	// Set the pattern
	//
	myDmbs[i]->trigsetx(hp);
	//
	//set timing
	//ntim is the same as pulse_delay initially set in xml configuration file
	//
	myDmbs[i]->set_cal_tim_pulse(ntim);   
	//
      }
      //
      ::usleep(1000);
      //
      for (unsigned i=0; i<myTmbs.size(); i++) {
	myTmbs[i]->EnableCLCTInputs(0x1f);
      }
      //
      usleep(1000);
      //
      //for(unsigned i =0; i<myDmbs.size(); ++i) {
      //myDmbs[i]->inject(1,0x4f);
      //}
      //
      std::cout << "Sending inject" <<std::endl;
      ccb->inject(1, 0xff);//pulse all dmbs in this crate
      //
      usleep(1000);
      //
      for(unsigned i =0; i < myDmbs.size(); ++i) {
	std::cout << "Slot " << myDmbs[i]->slot();
	myDmbs[i]->PrintCounters();
	int mycounter = myDmbs[i]->GetTmbDavScope();
	if ( mycounter>0 ) counter[delay][i] = mycounter;
      }
      //
      std::cout <<"After"<<std::endl;
      for(unsigned i =0; i < myTmbs.size(); ++i) {
	myTmbs[i]->GetCounters();
	myTmbs[i]->PrintCounters();
	myTmbs[i]->ResetCounters();
	myTmbs[i]->DisableCLCTInputs();
      }
      //
    }
  }
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    std::vector<DAQMB*> myDmbs = myEndcap->daqmbs(myCrates[j]);
    for(int delay=0; delay<200;delay++) {
      std::cout << delay << " " ;
      for(unsigned i =0; i < myDmbs.size(); ++i) std::cout << counter[delay][i] << " ";
      std::cout << std::endl;
    }
  }
  //
}
  } // namespace emu::pc
 } // namespace emu

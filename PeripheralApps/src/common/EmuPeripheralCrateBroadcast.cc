// $Id: EmuPeripheralCrateBroadcast.cc,v 1.56 2012/07/10 15:29:26 liu Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "emu/pc/EmuPeripheralCrateBroadcast.h"

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
#include <unistd.h> // for sleep()
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>
#include "emu/pc/EmuEndcap.h"

namespace emu {
  namespace pc {

const std::string       VMECC_FIRMWARE_DIR = "vcc"; 

EmuPeripheralCrateBroadcast::EmuPeripheralCrateBroadcast(xdaq::ApplicationStub * s): EmuPeripheralCrateBase(s)
{	
  broadcastCrate = 0;
  HomeDir_     = getenv("HOME");
  FirmwareDir_ = HomeDir_+"/firmware/";
  ConfigDir_   = HomeDir_+"/config/";
  //
  PeripheralCrateBroadcastXmlFile_  = ConfigDir_+"pc/broadcast.xml";
  DmbControlFPGAFirmwareFile_       = FirmwareDir_+"dmb/dmb6cntl_pro.svf";
  DmbVmeFPGAFirmwareFile_           = FirmwareDir_+"dmb/dmb6vme_pro.svf";
  CfebFPGAFirmwareFile_             = FirmwareDir_+"cfeb/cfeb_pro.svf";
//  CCBFirmwareFile_                  = FirmwareDir_+"ccb/ccb2004p_030507.svf";
//  MPCFirmwareFile_                  = FirmwareDir_+"mpc/mpc2004_100808.svf";
  //
  number_of_layers_pretrig_ = 2;
  number_of_layers_pattern_ = 4;
  //
  //    std::cout << "PeripheralCrateBroadcastXmlFile_ = " << PeripheralCrateBroadcastXmlFile_ << std::endl;
  //    std::cout << "DmbControlFPGAFirmwareFile_      = " << DmbControlFPGAFirmwareFile_      << std::endl;
  //    std::cout << "DmbVmeFPGAFirmwareFile_          = " << DmbVmeFPGAFirmwareFile_          << std::endl;
  //    std::cout << "CfebFPGAFirmwareFile_            = " << CfebFPGAFirmwareFile_            << std::endl;
  //
  // Bind SOAP callback
  //
  xoap::bind(this, &EmuPeripheralCrateBroadcast::onConfigure, "Configure", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateBroadcast::onEnable,    "Enable",    XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateBroadcast::onDisable,   "Disable",   XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateBroadcast::onHalt,      "Halt",      XDAQ_NS_URI);

  xgi::bind(this,&EmuPeripheralCrateBroadcast::Default, "Default");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::MainPage, "MainPage");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadCFEBcalchannel, "LoadCFEBcalchannel");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadDACandTrigger, "LoadDACandTrigger");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadCFEBinternal, "LoadCFEBinternal");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadCFEBexternal, "LoadCFEBexternal");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::DmbTurnOnPower, "DmbTurnOnPower");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::DmbTurnOffPower, "DmbTurnOffPower");
  //
  xgi::bind(this,&EmuPeripheralCrateBroadcast::SetNumberOfLayersInTrigger, "SetNumberOfLayersInTrigger");
  //
  // Bind firmware loading
  //
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadDMBCFEBFPGAFirmware, "LoadDMBCFEBFPGAFirmware");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadDMBControlFPGAFirmware, "LoadDMBControlFPGAFirmware");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadDMBvmeFPGAFirmware, "LoadDMBvmeFPGAFirmware");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadCFEBFPGAFirmware, "LoadCFEBFPGAFirmware");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadRATFirmware , "LoadRATFirmware" );
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadCCBFirmware , "LoadCCBFirmware" );
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadMPCFirmware , "LoadMPCFirmware" );
  xgi::bind(this,&EmuPeripheralCrateBroadcast::VMECCLoadFirmwareBcast,  "VMECCLoadFirmwareBcast"); 
  xgi::bind(this,&EmuPeripheralCrateBroadcast::VMECCTestBcast,  "VMECCTestBcast"); 
  xgi::bind(this,&EmuPeripheralCrateBroadcast::VMECCTestSkewClear,  "VMECCTestSkewClear");
  xoap::bind(this, &EmuPeripheralCrateBroadcast::onConfigCalCFEB, "ConfigCalCFEB", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateBroadcast::onConfigCalALCT, "ConfigCalALCT", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateBroadcast::onEnableCalCFEBGains, "EnableCalCFEBGains", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateBroadcast::onEnableCalCFEBCrossTalk, "EnableCalCFEBCrossTalk", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateBroadcast::onEnableCalCFEBSCAPed, "EnableCalCFEBSCAPed", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateBroadcast::onEnableCalCFEBComparator, "EnableCalCFEBComparator", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateBroadcast::onEnableCalALCTConnectivity, "EnableCalALCTConnectivity", XDAQ_NS_URI);
  //
  fsm_.addState('H', "Halted", this, &EmuPeripheralCrateBroadcast::stateChanged);
  fsm_.addState('C', "Configured", this, &EmuPeripheralCrateBroadcast::stateChanged);
  fsm_.addState('E', "Enabled",    this, &EmuPeripheralCrateBroadcast::stateChanged);
  //
  fsm_.addStateTransition('H', 'C', "Configure", this, &EmuPeripheralCrateBroadcast::dummyAction);
  fsm_.addStateTransition('C', 'C', "Configure", this, &EmuPeripheralCrateBroadcast::dummyAction);
  fsm_.addStateTransition('E', 'E', "Configure", this, &EmuPeripheralCrateBroadcast::dummyAction); // invalid, do nothing

  fsm_.addStateTransition('H', 'H', "Enable",    this, &EmuPeripheralCrateBroadcast::dummyAction); // invalid, do nothing
  fsm_.addStateTransition('C', 'E', "Enable",    this, &EmuPeripheralCrateBroadcast::dummyAction);
  fsm_.addStateTransition('E', 'E', "Enable",    this, &EmuPeripheralCrateBroadcast::dummyAction);

  fsm_.addStateTransition('H', 'H', "Disable",   this, &EmuPeripheralCrateBroadcast::dummyAction); // invalid, do nothing
  fsm_.addStateTransition('C', 'C', "Disable",   this, &EmuPeripheralCrateBroadcast::dummyAction);
  fsm_.addStateTransition('E', 'C', "Disable",   this, &EmuPeripheralCrateBroadcast::dummyAction);

  fsm_.addStateTransition('C', 'H', "Halt",      this, &EmuPeripheralCrateBroadcast::dummyAction);
  fsm_.addStateTransition('E', 'H', "Halt",      this, &EmuPeripheralCrateBroadcast::dummyAction);
  fsm_.addStateTransition('H', 'H', "Halt",      this, &EmuPeripheralCrateBroadcast::dummyAction);
  //
  fsm_.setInitialState('H');
  fsm_.reset();

  state_ = fsm_.getStateName(fsm_.getCurrentState());

  current_state_ = 0;
  getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &PeripheralCrateBroadcastXmlFile_);

}

//
void EmuPeripheralCrateBroadcast::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/"
       <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<std::endl;
}
//
/////////////////////////////////////////////////////////////////////////////////
// Main page description
/////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateBroadcast::MainPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
  //
  MyHeader(in,out,"EmuPeripheralCrateBroadcast");
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "Main Page");
  //
  if(current_state_==2) return;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  //
  *out << cgicc::legend("Broadcast to All Crates").set("style","color:blue") << cgicc::p() << std::endl ;
  //
  std::string LoadDMBCFEBFPGA = toolbox::toString("/%s/LoadDMBCFEBFPGAFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadDMBCFEBFPGA) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","!!!      Broadcast Download Firmware      !!!") << std::endl ;
  *out << cgicc::form();
  //
   std::string VMECCTestBcast =
     toolbox::toString("/%s/VMECCTestBcast",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",VMECCTestBcast) << std::endl ;
   *out << cgicc::input().set("type","submit")
     .set("value"," Probe for VMECC/DMB boards") << std::endl ;  
   *out << cgicc::form();
    //
   std::string VMECCTestSkewClear =
     toolbox::toString("/%s/VMECCTestSkewClear",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",VMECCTestSkewClear) << std::endl ;
   *out << cgicc::input().set("type","submit")
     .set("value"," Test DMB->CFEB SkewClear Connections") << std::endl ;  
   *out << cgicc::form();

   //


  std::string LoadCFEBchannel = toolbox::toString("/%s/LoadCFEBcalchannel",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadCFEBchannel) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","-----   Load CFEB Buckeye Patterns for Calibration    -----") << std::endl ;
  *out << cgicc::form();
  //
  std::string DmbTurnOnPower = toolbox::toString("/%s/DmbTurnOnPower",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DmbTurnOnPower) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","-----  Broadcast Turn ON On-chamber electronics power -----") << std::endl ;
  *out << cgicc::form();
  //
  std::string DmbTurnOffPower = toolbox::toString("/%s/DmbTurnOffPower",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DmbTurnOffPower) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","-----  Broadcast Turn OFF On-chamber electronics power ----") << std::endl ;
  *out << cgicc::form();
  //
  *out << cgicc::br();
  //
  char buf[20];
  //
  std::string SetNumberOfLayersInTrigger = toolbox::toString("/%s/SetNumberOfLayersInTrigger",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",SetNumberOfLayersInTrigger) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Set thresholds for ALCT*CLCT trigger") << std::endl ;
  *out << "N_layers_pretrig" << std::endl;
  sprintf(buf,"%d",number_of_layers_pretrig_);
  *out << cgicc::input().set("type","text").set("value",buf).set("name","number_of_layers_pretrig") << std::endl ;
  *out << "N_layers_pattern" << std::endl;
  sprintf(buf,"%d",number_of_layers_pattern_);
  *out << cgicc::input().set("type","text").set("value",buf).set("name","number_of_layers_pattern") << std::endl ;
  *out << cgicc::form();
  //
  //  std::string SetOutputToMPCDisable = toolbox::toString("/%s/SetOutputToMPCDisable",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",SetOutputToMPCDisable) << std::endl ;
  //  *out << cgicc::input().set("type","submit").set("value","Disable Output to MPC on all crates") << std::endl ;
  //  *out << cgicc::form();
  //  
  //
  *out << cgicc::fieldset() ;
  //
}
//
void EmuPeripheralCrateBroadcast::LoadCFEBcalchannel(xgi::Input * in, xgi::Output * out ) {
  //
  MyHeader(in,out,"Load CFEB buckeye channels, broadcast !!!");
  //
  DefineBroadcastCrate();
  //
  std::cout <<" Buckeye shift channels......"<<std::endl;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  //
  std::string LoadDACandTrigger = toolbox::toString("/%s/LoadDACandTrigger",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadDACandTrigger) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load the DAC to 1V, and trigger to DMB-->LCT, LTC-->L1A") << std::endl ;
  *out << cgicc::form()<<std::endl;
  //
  std::string LoadCFEBinternal = toolbox::toString("/%s/LoadCFEBinternal",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadCFEBinternal) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load Buckeye half-strip pattern 8") << std::endl ;
  *out << cgicc::form()<<std::endl;
  //
  std::string LoadCFEBexternal = toolbox::toString("/%s/LoadCFEBexternal",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadCFEBexternal) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load Buckeye external channel 8") << std::endl ;
  *out << cgicc::form()<<std::endl;
  //
  *out << cgicc::fieldset()<<std::endl;
  //
}
//
void EmuPeripheralCrateBroadcast::LoadCFEBinternal(xgi::Input * in, xgi::Output * out )  {
  //
  unsigned short int dword[2];
  dword[0]=0;
  std::cout << " Loading all Buckeye half-strip pattern 8 ..."<<std::endl;
  broadcastDMB->buck_shift_comp_bc(8);
  in=NULL;
  this->LoadCFEBcalchannel(in, out);
}
//
void EmuPeripheralCrateBroadcast::LoadCFEBexternal(xgi::Input * in, xgi::Output * out )  {
  //
  unsigned short int dword[2];
  dword[0]=0;
  std::cout <<" Loading all Buckeye external pattern 8 ..."<<std::endl;
  broadcastDMB->buck_shift_ext_bc(8);
  in=NULL;
  this->LoadCFEBcalchannel(in, out);
}
//
void EmuPeripheralCrateBroadcast::LoadDACandTrigger(xgi::Input * in, xgi::Output * out )  {
  //
  unsigned short int dword[2];
  dword[0]=0;
  std::cout <<" Loading all Buckeye external pattern 8 ..."<<std::endl;
  broadcastDMB->set_cal_dac(1.0,1.0);
  broadcastDMB->settrgsrc(1);
  in=NULL;
  this->LoadCFEBcalchannel(in, out);
}
//
/////////////////////////////////////////////////////////////////////////////////
// Setup broadcast classes
/////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateBroadcast::DefineBroadcastCrate() {
  //
  //define broadcast crate and board, if not defined before
  //
  if (!broadcastCrate) {
    std::cout <<" Broadcast crate has not been defined yet"<<std::endl;
    std::cout <<" Defining Broadcast crate from " << PeripheralCrateBroadcastXmlFile_.toString() << std::endl;
    //
    CommonParser("xml", PeripheralCrateBroadcastXmlFile_.toString());
    EmuEndcap *myEndcap = GetEmuEndcap();
    std::vector<Crate *> tmpcrate = myEndcap->broadcast_crate();
    broadcastCrate = tmpcrate[0];
    unsigned int ib=(broadcastCrate->daqmbs()).size()-1;
    broadcastDMB = (broadcastCrate->daqmbs())[ib];
    broadcastTMB = (broadcastCrate->tmbs())[0];
    broadcastALCT = broadcastTMB->alctController();
    broadcastRAT  = broadcastTMB->getRAT();
    broadcastMPC  = broadcastCrate->mpc();
    broadcastCCB  = broadcastCrate->ccb();
  }
  //
  std::cout <<" Broadcast components are defined "<<std::endl;
  //
  return;
}
//
/////////////////////////////////////////////////////////////////////////////////
// Firmware page and methods
/////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateBroadcast::LoadDMBCFEBFPGAFirmware(xgi::Input * in, xgi::Output * out ){
  //
  MyHeader(in,out,"Broadcast Download Firmware,  Be extra careful    !!!");
  //
  DefineBroadcastCrate();
  //
  // load the VMECC controller firmware
  //
     std::string VMECCLoadFirmwareBcast =
       toolbox::toString("/%s/VMECCLoadFirmwareBcast",getApplicationDescriptor()->getURN().c_str());
  //
     *out << cgicc::form().set("method","GET").set("action",VMECCLoadFirmwareBcast)
   << std::endl ;
     *out << cgicc::input().set("type","submit")
       .set("value","Load VMECC Firmware(broadcast)") 
   << std::endl ;
     *out << cgicc::form() << std::endl ;
    //
  // load the DAQMB Controller FPGA firmware
  //
     std::cout <<"Ready to load firmware for all components ..."<<std::endl;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  //
  std::string LoadDMBvmeFPGA = toolbox::toString("/%s/LoadDMBvmeFPGAFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadDMBvmeFPGA) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load DMBs VME FPGA Firmware") << std::endl ;
  *out << DmbVmeFPGAFirmwareFile_;
  *out << cgicc::form()<<std::endl;
  //
  std::string LoadDMBControlFPGA = toolbox::toString("/%s/LoadDMBControlFPGAFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadDMBControlFPGA) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load DMBs Control FPGA Firmware") << std::endl ;
  *out << DmbControlFPGAFirmwareFile_;
  *out << cgicc::form()<<std::endl;
  //
  std::string LoadCFEBFPGA = toolbox::toString("/%s/LoadCFEBFPGAFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadCFEBFPGA) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load CFEBs FPGA Firmware") << std::endl ;
  *out << CfebFPGAFirmwareFile_;
  *out << cgicc::form()<<std::endl;
  //
  //create RAT filename for firmware based on expected dates...
  char ratdate[8];
  sprintf(ratdate,"%4x%1x%1x%1x%1x",
	  broadcastRAT->GetExpectedRatFirmwareYear()&0xffff,
	  (broadcastRAT->GetExpectedRatFirmwareMonth()>>4)&0xf,
	  (broadcastRAT->GetExpectedRatFirmwareMonth()   )&0xf,
	  (broadcastRAT->GetExpectedRatFirmwareDay()  >>4)&0xf,
	  (broadcastRAT->GetExpectedRatFirmwareDay()     )&0xf);
  RATFirmwareFile_ = FirmwareDir_+"rat/"+ratdate+"/rat.svf";
  //
  std::string LoadRATFirmware = toolbox::toString("/%s/LoadRATFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadRATFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load RAT Firmware") << std::endl ;
  *out << RATFirmwareFile_ << std::endl;
  *out << cgicc::form()<<std::endl;
  //
  //  create filename for CCB & MPC
    int year  = (broadcastCCB->GetExpectedFirmwareYear())%100;
    int month = broadcastCCB->GetExpectedFirmwareMonth();
    int day   = broadcastCCB->GetExpectedFirmwareDay();
    char ccbdate[7];
    sprintf(ccbdate,"%02u%02u%02u",month,day,year);
    std::ostringstream CCBFirmware;
    CCBFirmware << FirmwareDir_ << "ccb/ccb2004p_" << ccbdate << ".svf";
    CCBFirmwareFile_ = CCBFirmware.str();

    year  = (broadcastMPC->GetExpectedFirmwareYear())%100;
    month = broadcastMPC->GetExpectedFirmwareMonth();
    day   = broadcastMPC->GetExpectedFirmwareDay();
    char mpcdate[7];
    sprintf(mpcdate,"%02u%02u%02u",month,day,year);
    std::ostringstream MPCFirmware;
    MPCFirmware << FirmwareDir_ << "mpc/mpc2004_" << mpcdate << ".svf";
    MPCFirmwareFile_ = MPCFirmware.str();
 
  std::string LoadMPCFirmware = toolbox::toString("/%s/LoadMPCFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadMPCFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load MPC Firmware") << std::endl ;
  *out << MPCFirmwareFile_;
  *out << cgicc::form() << std::endl;
  //
  std::string LoadCCBFirmware = toolbox::toString("/%s/LoadCCBFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadCCBFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load CCB Firmware") << std::endl ;
  *out << CCBFirmwareFile_;
  *out << cgicc::form() << std::endl;
  //
  *out << cgicc::fieldset() << std::endl;
  //
}
//
//
void EmuPeripheralCrateBroadcast::LoadDMBControlFPGAFirmware(xgi::Input * in, xgi::Output * out )  {
  //
  // load the DAQMB Controller FPGA firmware
  //
  unsigned short int dword[2];
  dword[0]=0;
  char *outp=(char *)dword;
  std::cout <<" Loading all the DMB's Controller FPGAs firmware from " << DmbControlFPGAFirmwareFile_ <<std::endl;
  broadcastDMB->epromload(MPROM,DmbControlFPGAFirmwareFile_.c_str(),1,outp);
  in=NULL;
  this->LoadDMBCFEBFPGAFirmware(in, out);
}
//
void EmuPeripheralCrateBroadcast::LoadDMBvmeFPGAFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
  //
  // load the DAQMB VME FPGA firmware
  //
  char *outp="0";
  std::cout <<" Loading all the DMB's VME FPGAs firmware from " << DmbVmeFPGAFirmwareFile_ <<std::endl;
  std::cout <<" Step 1: Sending SOAP message to all the crates to readback the VME_PROM_ID"<<std::endl;
  std::cout <<"         This is the DMB board number"<<std::endl;
  //
  //SOAP message to read back the DMB board ID:
  PCsendCommand("ReadVmePromUserid","emu::pc::EmuPeripheralCrateConfig");
  //
  std::cout <<" Step 2: Broadcast programming the VME until the 'loading USERCODE' point"<<std::endl;
  broadcastDMB->epromload_broadcast(VPROM,DmbVmeFPGAFirmwareFile_.c_str(),1,outp,1);
  //
  std::cout <<" Step 3: Sending SOAP message to program PROM_USERCODE"<<std::endl;
  //SOAP message to individual crates to program the PROM_USERCODE
  PCsendCommand("LoadVmePromUserid","emu::pc::EmuPeripheralCrateConfig");
  //
  std::cout <<" Step 4: Broadcast the remaining part of the PROM/SVF"<<std::endl;
  broadcastDMB->epromload_broadcast(VPROM,DmbVmeFPGAFirmwareFile_.c_str(),1,outp,3);
  //
  this->LoadDMBCFEBFPGAFirmware(in, out);
}
//
  void EmuPeripheralCrateBroadcast::VMECCLoadFirmwareBcast(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    DefineBroadcastCrate();
    vmecc=broadcastCrate->vmecc();
    broadcastCrate->vmeController()->init();

    std::cout<< " Entered VMECCLoadFirmwareBcast \n" <<std::endl;
    std::string PROM_Path = FirmwareDir_+VMECC_FIRMWARE_DIR;
    std::cout << "Path = " << PROM_Path << "\nVer = " << vmecc->VCC_frmw_ver << std::endl;
    vmecc->prg_vcc_prom_bcast(PROM_Path.c_str(),vmecc->VCC_frmw_ver.c_str());
    this->LoadDMBCFEBFPGAFirmware(in,out);
    //
  }

void EmuPeripheralCrateBroadcast::VMECCTestBcast(xgi::Input * in, xgi::Output * out )throw (xgi::exception::Exception)
{
  DefineBroadcastCrate();
  vmecc=broadcastCrate->vmecc();
  broadcastCrate->vmeController()->init();
  broadcastCrate->vmeController()->write_ResetMisc_CR(0x001B);

  //
  MyHeader(in,out,"EmuPeripheralCrate Broadcast Probe");
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  //
  *out << cgicc::legend("Probe for Crate Controllers").set("style","color:blue") << cgicc::p() << std::endl ;
  //
  *out << cgicc::pre();
  char buf[1000], sbuf[100];
  int nc=vmecc->read_dev_id_broadcast(buf);
  *out << nc << " crate controller(s) responded: " << std::endl;
  for(int i=0; i<nc; i++) {
     int *device_id=(int *)(buf+i*10+6);
     sprintf(sbuf, "    Controller MAC address %02X:%02X:%02X:%02X:%02X:%02X, Device ID: %08X\n",
         buf[i*10]&0xff, buf[i*10+1]&0xff, buf[i*10+2]&0xff,
         buf[i*10+3]&0xff, buf[i*10+4]&0xff, buf[i*10+5]&0xff, 
         *device_id);
     *out << sbuf;
     }

  *out << cgicc::pre();
  *out << cgicc::fieldset()<<std::endl;

  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << cgicc::legend("Probe for DMBs").set("style","color:blue") << cgicc::p() << std::endl ;
  *out << cgicc::pre();
  // Probe for DMBs
  unsigned int limit=(broadcastCrate->daqmbs()).size()-1;
  printf(" limit %d \n",limit);
  for(unsigned int j=0;j<limit;j++){
    DAQMB *broadcastDMB0 = (broadcastCrate->daqmbs())[j];
    int slott=broadcastDMB0->slot();
    printf(" %d slott %d \n",j,slott);
    // now globally pole the DMB in slot 1
    broadcastDMB0->mbpromuser(0);
    nc=vmecc->vme_read_broadcast(buf);
    *out << nc << " DMB's in slot " << slott << " responded: " << std::endl;
    for(int i=0; i<nc; i++) {
        int *device_id=(int *)(buf+i*10+6);
        sprintf(sbuf, "    from MAC address %02X:%02X:%02X:%02X:%02X:%02X, userid: %08X\n",
            buf[i*10]&0xff, buf[i*10+1]&0xff, buf[i*10+2]&0xff,
            buf[i*10+3]&0xff, buf[i*10+4]&0xff, buf[i*10+5]&0xff, 
            *device_id);
        *out << sbuf;
    }
  }
  *out << cgicc::pre();
  *out << cgicc::fieldset()<<std::endl;
   
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << cgicc::legend("Probe for DMB mbpromuser(1)s").set("style","color:blue") << cgicc::p() << std::endl ;
  *out << cgicc::pre();
  // Probe for DMBs
  unsigned int limit1=(broadcastCrate->daqmbs()).size()-1;
  printf(" limit %d \n",limit1);
  for(unsigned int j=0;j<limit1;j++){
    DAQMB *broadcastDMB0 = (broadcastCrate->daqmbs())[j];
    int slott=broadcastDMB0->slot();
    printf(" %d slott %d \n",j,slott);
    // now globally pole the DMB in slot 1
    broadcastDMB0->mbpromuser(1);
    nc=vmecc->vme_read_broadcast(buf);
    *out << nc << " DMB's in slot " << slott << " responded: " << std::endl;
    for(int i=0; i<nc; i++) {
        int *device_id=(int *)(buf+i*10+6);
        sprintf(sbuf, "    from MAC address %02X:%02X:%02X:%02X:%02X:%02X, userid: %08X\n",
            buf[i*10]&0xff, buf[i*10+1]&0xff, buf[i*10+2]&0xff,
            buf[i*10+3]&0xff, buf[i*10+4]&0xff, buf[i*10+5]&0xff, 
            *device_id);
        *out << sbuf;
    }
  }
  *out << cgicc::pre();
  *out << cgicc::fieldset()<<std::endl;



  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << cgicc::legend("Probe for DMB mbfpgauser s").set("style","color:blue") << cgicc::p() << std::endl ;
  *out << cgicc::pre();
  // Probe for DMBs
  unsigned int limit2=(broadcastCrate->daqmbs()).size()-1;
  printf(" limit %d \n",limit2);
  for(unsigned int j=0;j<limit2;j++){
    DAQMB *broadcastDMB0 = (broadcastCrate->daqmbs())[j];
    int slott=broadcastDMB0->slot();
    printf(" %d slott %d \n",j,slott);
    // now globally pole the DMB in slot 1
    broadcastDMB0->mbfpgauser();
    nc=vmecc->vme_read_broadcast(buf);
    *out << nc << " DMB's in slot " << slott << " responded: " << std::endl;
    for(int i=0; i<nc; i++) {
        int *device_id=(int *)(buf+i*10+6);
        sprintf(sbuf, "    from MAC address %02X:%02X:%02X:%02X:%02X:%02X, userid: %08X\n",
            buf[i*10]&0xff, buf[i*10+1]&0xff, buf[i*10+2]&0xff,
            buf[i*10+3]&0xff, buf[i*10+4]&0xff, buf[i*10+5]&0xff, 
            *device_id);
        *out << sbuf;
    }
  }
  *out << cgicc::pre();
  *out << cgicc::fieldset()<<std::endl;


  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << cgicc::legend("Probe for DMB CFEBs fpgauserid").set("style","color:blue") << cgicc::p() << std::endl ;
  *out << cgicc::pre();
  // Probe for DMBs
  unsigned int limit3=(broadcastCrate->daqmbs()).size()-1;
  printf(" limit %d \n",limit3);
  for(unsigned int j=0;j<limit3;j++){
    DAQMB *broadcastDMB0 = (broadcastCrate->daqmbs())[j];
    std::vector<CFEB> cfebs = broadcastDMB0->cfebs() ;
    typedef std::vector<CFEB>::iterator CFEBItr;
    for(CFEBItr cfebItr = cfebs.begin(); cfebItr != cfebs.end(); ++cfebItr) {
        int slott=broadcastDMB0->slot();
        broadcastDMB0->febfpgauser(*cfebItr);
        nc=vmecc->vme_read_broadcast(buf);
        sprintf(sbuf,"CFEB%d  ",(*cfebItr).number());
        *out << nc << sbuf <<" in slot " << slott << " responded: " << std::endl;
    for(int i=0; i<nc; i++) {
        int *device_id=(int *)(buf+i*10+6);
        sprintf(sbuf, "    from MAC address %02X:%02X:%02X:%02X:%02X:%02X, userid: %08X\n",
            buf[i*10]&0xff, buf[i*10+1]&0xff, buf[i*10+2]&0xff,
            buf[i*10+3]&0xff, buf[i*10+4]&0xff, buf[i*10+5]&0xff, 
            *device_id);
        *out << sbuf;
    }
  }
  }
  *out << cgicc::pre();
  *out << cgicc::fieldset()<<std::endl;



  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << cgicc::legend("Probe for DMB CFEBs promuserid").set("style","color:blue") << cgicc::p() << std::endl ;
  *out << cgicc::pre();
  // Probe for DMBs
  unsigned int limit4=(broadcastCrate->daqmbs()).size()-1;
  printf(" limit %d \n",limit4);
  for(unsigned int j=0;j<limit4;j++){
    DAQMB *broadcastDMB0 = (broadcastCrate->daqmbs())[j];
    std::vector<CFEB> cfebs = broadcastDMB0->cfebs() ;
    typedef std::vector<CFEB>::iterator CFEBItr;
    for(CFEBItr cfebItr = cfebs.begin(); cfebItr != cfebs.end(); ++cfebItr) {
        int slott=broadcastDMB0->slot();
        broadcastDMB0->febpromuser(*cfebItr);
        nc=vmecc->vme_read_broadcast(buf);
        sprintf(sbuf," CFEB%d  ",(*cfebItr).number());
        *out << nc << sbuf <<" in slot " << slott << " responded: " << std::endl;
	char rcvbuf[5];
    for(int i=0; i<nc; i++) {
        rcvbuf[0]=buf[i*10+6];
        rcvbuf[1]=buf[i*10+7];
        rcvbuf[2]=buf[i*10+8];
        rcvbuf[3]=buf[i*10+9];
        rcvbuf[4]=buf[i*10+10];
       	rcvbuf[0]=((rcvbuf[0]>>1)&0x7f)+((rcvbuf[1]<<7)&0x80);
	rcvbuf[1]=((rcvbuf[1]>>1)&0x7f)+((rcvbuf[2]<<7)&0x80);
	rcvbuf[2]=((rcvbuf[2]>>1)&0x7f)+((rcvbuf[3]<<7)&0x80);
	rcvbuf[3]=((rcvbuf[3]>>1)&0x7f)+((rcvbuf[4]<<7)&0x80);
        rcvbuf[3]=rcvbuf[3]|0x80;
        unsigned int device=0x00000000;
        device=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)
        |((rcvbuf[3]&0xff)<<24)|device;
        sprintf(sbuf, "    from MAC address %02X:%02X:%02X:%02X:%02X:%02X, userid: %08X\n",
            buf[i*10]&0xff, buf[i*10+1]&0xff, buf[i*10+2]&0xff,
            buf[i*10+3]&0xff, buf[i*10+4]&0xff, buf[i*10+5]&0xff, 
            device);
        *out << sbuf;
    }
  }
  }
  broadcastCrate->vmeController()->init();

  *out << cgicc::pre();
  *out << cgicc::fieldset()<<std::endl;

  //
 }

void EmuPeripheralCrateBroadcast::VMECCTestSkewClear(xgi::Input * in,xgi::Output *out) throw(xgi::exception::Exception)
{
  DefineBroadcastCrate();
  broadcastCrate->vmeController()->init();
  broadcastCrate->vmeController()->write_ResetMisc_CR(0x001B);

  //
  MyHeader(in,out,"EmuPeripheralCrate Broadcast Probe");
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  //
  *out << cgicc::legend(" Test Skew Clear Connections").set("style","color:blue") << cgicc::p() << std::endl ;

  *out << cgicc::pre();
  char buf[1000],sbuf[100];
  int nc;
  unsigned int onoff[6]={0x21,0x22,0x24,0x28,0x30,0x3F};
  int ionoff[6]={0,1,2,3,4,5};
  for(unsigned int iof=0;iof<6;iof++){
  broadcastDMB->lowv_onoff(onoff[iof]);
  ::sleep(1);
  if(iof==5)break;
  // Probe for DMBs
  unsigned int limit=(broadcastCrate->daqmbs()).size()-1;
  printf(" limit %d \n",limit);
  *out << " ****CFEB ON-OFF REGISTER SET " << std::hex << onoff[iof] << std::dec << " Only CFEB" << ionoff[iof] << " will return usercode "<< std::endl;
  for(unsigned int j=0;j<limit;j++){
    DAQMB *broadcastDMB0 = (broadcastCrate->daqmbs())[j];
    std::vector<CFEB> cfebs = broadcastDMB0->cfebs() ;
    typedef std::vector<CFEB>::iterator CFEBItr;
    for(CFEBItr cfebItr = cfebs.begin(); cfebItr != cfebs.end(); ++cfebItr) {
        int slott=broadcastDMB0->slot();
        broadcastDMB0->febfpgauser(*cfebItr);
        nc=vmecc->vme_read_broadcast(buf);
        sprintf(sbuf,"CFEB%d  ",(*cfebItr).number());
        *out << nc << sbuf <<" in slot " << slott << " responded: " << std::endl;
    for(int i=0; i<nc; i++) {
        int *device_id=(int *)(buf+i*10+6);
        unsigned int dev_id=*device_id;

        sprintf(sbuf, "    from MAC address %02X:%02X:%02X:%02X:%02X:%02X, userid: %08X",
            buf[i*10]&0xff, buf[i*10+1]&0xff, buf[i*10+2]&0xff,
            buf[i*10+3]&0xff, buf[i*10+4]&0xff, buf[i*10+5]&0xff, 
            *device_id);
        if((int)(*cfebItr).number()==ionoff[iof]&&dev_id!=0xffffffff)*out << sbuf << " Good " << std::endl;
        if((int)(*cfebItr).number()==ionoff[iof]&&dev_id==0xffffffff)*out << sbuf << " Bad " << std::endl;
        if((int)((*cfebItr).number()!=ionoff[iof]&&dev_id!=0xffffffff))*out << sbuf << " Bad " << std::endl;
        if((int)((*cfebItr).number()!=ionoff[iof]&&dev_id==0xffffffff))*out << sbuf << " Good " << std::endl;
    }
  }
  }
  }
  *out << cgicc::pre();
  *out << cgicc::fieldset()<<std::endl;
  broadcastCrate->vmeController()->init();
}


void EmuPeripheralCrateBroadcast::LoadCFEBFPGAFirmware(xgi::Input * in, xgi::Output * out ) {
  //
  // load the CFEB FPGA firmware
  //
  char *outp="0";
  //
  std::cout <<" Loading all the CFEBs FPGAs firmware from " << CfebFPGAFirmwareFile_ <<std::endl;
  //
  std::cout <<" Step 1: Sending SOAP message to all the crates to readback the CFEB_PROM_ID"<<std::endl;
  std::cout <<"         This is the CFEB board number"<<std::endl;
  //SOAP message to read back the CFEB board ID:
  PCsendCommand("ReadCfebPromUserid","emu::pc::EmuPeripheralCrateConfig");
  //
  std::cout <<" Step 2: Broadcast programming the CFEB until the 'loading USERCODE' point"<<std::endl;
  broadcastDMB->epromload_broadcast(FAPROM,CfebFPGAFirmwareFile_.c_str(),1,outp,1);
  //
  std::cout <<" Step 3: Sending SOAP message to program CFEB PROM_USERCODE"<<std::endl;
  //SOAP message to individual crates to program the CFEB PROM_USERCODE
  PCsendCommand("LoadCfebPromUserid","emu::pc::EmuPeripheralCrateConfig");
  //
  std::cout <<" Step 4: Broadcast the remaining part of the PROM/SVF"<<std::endl;
  broadcastDMB->epromload_broadcast(FAPROM,CfebFPGAFirmwareFile_.c_str(),1,outp,3);
  //
  this->LoadDMBCFEBFPGAFirmware(in, out);
  //
}
//
void EmuPeripheralCrateBroadcast::LoadRATFirmware(xgi::Input * in, xgi::Output * out )  {
  //
  // load the RAT firmware
  //
  std::cout <<" Loading all RATs with firmware from " << RATFirmwareFile_ << std::endl;
  //
  int debugMode(0);
  int jch(7);
  //
  broadcastTMB->disableAllClocks();
  broadcastRAT->SVFLoad(&jch,RATFirmwareFile_.c_str(),debugMode);
  broadcastTMB->enableAllClocks();
  //
  in=NULL;
  this->LoadDMBCFEBFPGAFirmware(in, out);
}
//
void EmuPeripheralCrateBroadcast::LoadMPCFirmware(xgi::Input * in, xgi::Output * out )  
{
  //
  // load the MPC firmware
  //
  if (broadcastMPC) {
    std::cout <<" Loading all MPCs with firmware from " << MPCFirmwareFile_ << std::endl;

    int debugMode(0);
    int jch(0);
    int verify(0);
    //
    broadcastMPC->svfLoad(jch,MPCFirmwareFile_.c_str(),debugMode,verify);
  //
  } else 
  {
    std::cout <<" No broadcast MPC exists" << std::endl;
  }
  in=NULL;
  this->LoadDMBCFEBFPGAFirmware(in, out);
}
//
void EmuPeripheralCrateBroadcast::LoadCCBFirmware(xgi::Input * in, xgi::Output * out )  
{
  //
  // load the CCB firmware
  //
  if (broadcastCCB) {
    std::cout <<" Loading all CCBs with firmware from " << CCBFirmwareFile_ << std::endl;
    //
    int debugMode(0);
    int jch(0);
    int verify(0);
    //
    broadcastCCB->svfLoad(jch,CCBFirmwareFile_.c_str(),debugMode,verify);
  } else 
  {
    std::cout <<" No broadcast CCB exists" << std::endl;
  }
  in=NULL;
  this->LoadDMBCFEBFPGAFirmware(in, out);
}
//
/////////////////////////////////////////////////////////////////////////////
// Other (nice) broadcast commands
/////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateBroadcast::DmbTurnOnPower(xgi::Input * in, xgi::Output * out )  {
  //
  DefineBroadcastCrate();
  //
  std::cout <<" Broadcast to turn ON the On-chamber electronics power ..."<<std::endl;
  broadcastDMB->lowv_onoff(0x3f);
  in=NULL;
  this->Default(in, out);
  //
}
//
void EmuPeripheralCrateBroadcast::DmbTurnOffPower(xgi::Input * in, xgi::Output * out )  {
  //
  DefineBroadcastCrate();
  //
  std::cout <<" Broadcast to turn ON the On-chamber electronics power ..."<<std::endl;
  broadcastDMB->lowv_onoff(0x00);
  in=NULL;
  this->Default(in, out);
  //
}
//
void EmuPeripheralCrateBroadcast::MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
  throw (xgi::exception::Exception) {
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  //*out << cgicc::title(title) << std::endl;
  //*out << "<a href=\"/\"><img border=\"0\" src=\"/daq/xgi/images/XDAQLogo.gif\" title=\"XDAQ\" alt=\"\" style=\"width: 145px; height: 89px;\"></a>" << h2(title) << std::endl;
  //
  cgicc::Cgicc cgi(in);
  //
  //const CgiEnvironment& env = cgi.getEnvironment();
  //
  std::string myUrl = getApplicationDescriptor()->getContextDescriptor()->getURL();
  std::string myUrn = getApplicationDescriptor()->getURN();
  xgi::Utils::getPageHeader(out,title,myUrl,myUrn,"");
  //
}
//
xoap::MessageReference EmuPeripheralCrateBroadcast::onConfigCalCFEB (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  In_Broadcast_ = true;
  //
  //implement the cal0 configure process:
  float dac;
//  int nsleep = 100;  
  std::cout << "DMB setup for calibration " << std::endl;
  calsetup=0;
  dac=1.00;
  //
  DefineBroadcastCrate();
  //
#if 0
  // broadcastCrate->vmeController()->Debug(10);
  //  
  broadcastTMB->DisableCLCTInputs();
  std::cout << "Disabling inputs for TMB slot  " << broadcastTMB->slot() << std::endl;
  broadcastTMB->DisableALCTInputs();
  //
//move to Command
  // DMB fifo Master_Reset
  broadcastDMB->calctrl_fifomrst();
  std::cout << "DMB Fifo reset and initialization "<<std::endl;
  usleep(5000);
  broadcastDMB->restoreMotherboardIdle();
  //
  std::cout << "Set DAC for DMB slot  " << broadcastDMB->slot() << std::endl;
  broadcastDMB->set_cal_dac(dac,dac);
  std::cout <<" DAC is set to: "<<dac<<std::endl;
  //Enable CLCT (bit0=1), disable L1A (bit1=0) on DMB calibration
//  broadcastDMB->settrgsrc(1);
  //
  //set the default DMB Calibration timing:
  /* // change the itim to adjust the pulse position
     int cal_delay_bits = (calibration_LCT_delay_ & 0xF)
     | (calibration_l1acc_delay_ & 0x1F) << 4
     | (itim & 0x1F) << 9
     | (inject_delay_ & 0x1F) << 14;
  */
  int dword= (6 | (20<<4) | (10<<9) | (15<<14) ) &0xfffff;
  broadcastDMB->setcaldelay(dword);

  std::cout << " The Peripheral Crate configure finished "<<std::endl;
  ::usleep(nsleep);
#endif

  In_Broadcast_ = false;
  return createReply(message);
  //
}

xoap::MessageReference EmuPeripheralCrateBroadcast::onConfigCalALCT (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  In_Broadcast_ = true;

//  std::cout<< "Inside onConfigCalALCT-broadcast"<<std::endl;
//  std::ostringstream test;
//  message->writeTo(test);
//  std::cout << test.str() << std::endl;
//  std::cout << "Inside onConfigCalALCT-broadcast, dealt with message"<< std::endl;
  //
  //
//  DefineBroadcastCrate(); // this pulls out all broadcast objects, inlcuding alct
  //
  // do setup here

  In_Broadcast_ = false;
  return createReply(message);
  //
}

xoap::MessageReference EmuPeripheralCrateBroadcast::onEnableCalALCTConnectivity (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{

	In_Broadcast_ = true;
	std::ostringstream test;
	message->writeTo(test);
	std::cout << test.str() <<std::endl;

	calsetup++; // step counter

	std::cout << "Setting up for ALCT connectivity test, calsetup= " <<calsetup<< std::endl;

	// Initial setup, first time only
	if (calsetup==1) 
	{
		// more configuration here
		std::cout << "Starting STEP test: 12"  << std::endl;
	}
/*
	broadcastALCT->SetUpPulsing
	(
		100, // pulse amplitude, later will have to make individual for each chamber type
		PULSE_LAYERS, 
		(1 << (calsetup-1)), // strip mask
		ADB_SYNC
	);
*/
	In_Broadcast_ = false;
	return createReply(message);
}


//
xoap::MessageReference EmuPeripheralCrateBroadcast::onEnableCalCFEBComparator (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  float dac, threshold;
  int nsleep = 100, highthreshold;  
  In_Broadcast_ = true;
  std::ostringstream test;
  message->writeTo(test);
  std::cout << test.str() <<std::endl;
  //
  calsetup++;
  //
  //implement the comparator setup process:
  std::cout << "DMB setup for CFEB Comparator, calsetup= " <<calsetup<< std::endl;
  //
  //Start the setup process:
  //
  if (calsetup==1) {
    broadcastTMB->SetTmbAllowClct(1);
    broadcastTMB->SetTmbAllowMatch(0);
    broadcastTMB->WriteRegister(0x86,broadcastTMB->FillTMBRegister(0x86));
    broadcastTMB->SetAlctMatchWindowSize(7);
    broadcastTMB->WriteRegister(0xb2,broadcastTMB->FillTMBRegister(0xb2));
    broadcastTMB->SetL1aDelay(154);
    broadcastTMB->SetL1aWindowSize(7);
    broadcastTMB->WriteRegister(0x74,broadcastTMB->FillTMBRegister(0x74));
    //
    broadcastTMB->EnableCLCTInputs(0x1f); //enable TMB's CLCT inputs
    broadcastDMB->settrgsrc(0); //disable the DMB internal LCT & L1A
  }
  int thresholdsetting =((calsetup-1)%20);   //35 Comparator threshold setting for each channel
  int nstrip=(calsetup-1)/20;           //16 channels, total loop: 32*35=1120
  highthreshold=nstrip/16;
  dac=0.02+0.18*highthreshold;
  nstrip=nstrip%16;
  if (!thresholdsetting) {
    broadcastDMB->buck_shift_comp_bc(nstrip);
    if (!nstrip) broadcastDMB->set_cal_dac(dac,dac);
  }
  threshold=0.003*thresholdsetting+0.01+ (0.19+0.007*thresholdsetting)*highthreshold;
  broadcastDMB->set_comp_thresh_bc(threshold);
  std::cout <<" The strip was set to: "<<nstrip<<" DAC was set to: "<<dac <<std::endl;
  ::usleep(nsleep);
  //    fireEvent("Enable");
  //
  In_Broadcast_ = false;
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateBroadcast::onEnableCalCFEBGains (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  float dac;
  int nsleep = 100;  
  In_Broadcast_ = true;
  std::ostringstream test;
  message->writeTo(test);
  std::cout << test.str() <<std::endl;
  //
  calsetup++;
  //
  //implement the cal0 setup process:
  std::cout << "DMB setup for CFEB Gain, calsetup= " <<calsetup<< std::endl;
  //
  //Start the setup process:
  int gainsetting =((calsetup-1)%20);
  int nstrip=(calsetup-1)/20;
  if (!gainsetting) broadcastDMB->buck_shift_ext_bc(nstrip);
  dac=0.1+0.25*gainsetting;
  broadcastDMB->set_cal_dac(dac,dac);
  std::cout <<" The strip was set to: "<<nstrip<<" DAC was set to: "<<dac <<std::endl;
  ::usleep(nsleep);
  //    fireEvent("Enable");
  //
  In_Broadcast_ = false;
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateBroadcast::onEnableCalCFEBCrossTalk (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  int nsleep = 100;  
  //
  std::cout<< "This is a checking printing for OnEnableCalCFEBCrossTalk"<<std::endl;
  In_Broadcast_ = true;
  std::ostringstream test;
  message->writeTo(test);
  std::cout << test.str() <<std::endl;
  //
  calsetup++;
  //
  //implement the cal0 setup process:
  std::cout << "DMB setup for CFEB Time, calsetup= " <<calsetup<< std::endl;
  //
  //Start the setup process:
  int timesetting =((calsetup-1)%10);
  int nstrip=(calsetup-1)/10;
  if (!timesetting) broadcastDMB->buck_shift_ext_bc(nstrip);
  broadcastDMB->set_cal_tim_pulse(timesetting+5);
  std::cout <<" The strip was set to: "<<nstrip<<" Time was set to: "<<timesetting <<std::endl;
  ::usleep(nsleep);
  //    fireEvent("Enable");
  //
  In_Broadcast_ = false;
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateBroadcast::onEnableCalCFEBSCAPed (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  float dac;
  int nsleep = 100;  
  //
  In_Broadcast_ = true;
  std::ostringstream test;
  message->writeTo(test);
  std::cout << test.str() <<std::endl;
  //
  calsetup++;
  //
  //implement the CFEB_Pedestal setup process:
  std::cout << "DMB setup for CFEB Pedestal, calsetup= " <<calsetup<< std::endl;
  //
  // Start the setup process: Set all channel to normal, DAC to 0, No_pulse:
  broadcastDMB->buck_shift_ext_bc(-1);
  dac=0.0;
  broadcastDMB->set_cal_dac(dac,dac);
  std::cout <<" The strip was set to: -1, " <<" DAC was set to: "<<dac <<std::endl;
  ::usleep(nsleep);
  broadcastDMB->toggle_pedestal();
  std::cout<<" Toggle DMB Pedestal switch, to disable the pulsing."<<std::endl;
  ::usleep(nsleep);

  //    fireEvent("Enable");
  //
  In_Broadcast_ = false;
  return createReply(message);
}
//
void EmuPeripheralCrateBroadcast::SetNumberOfLayersInTrigger(xgi::Input * in, xgi::Output * out )  {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("number_of_layers_pretrig");
  cgicc::form_iterator name2 = cgi.getElement("number_of_layers_pattern");
  //
  if(name != cgi.getElements().end()) 
    number_of_layers_pretrig_ = strtol(cgi["number_of_layers_pretrig"]->getValue().c_str(),NULL,10);
  //
  if(name2 != cgi.getElements().end()) 
    number_of_layers_pattern_ = strtol(cgi["number_of_layers_pattern"]->getValue().c_str(),NULL,10);
  //
  DefineBroadcastCrate();
  //
  std::cout << " Broadcast " << number_of_layers_pretrig_ << "/" << number_of_layers_pattern_ 
	    << " layers in pretrig/pattern trigger..." << std::endl;
  //
  broadcastTMB->SetHsPretrigThresh(number_of_layers_pretrig_);
  broadcastTMB->SetMinHitsPattern(number_of_layers_pattern_);
  broadcastTMB->WriteRegister(0x70);
  //
  broadcastALCT->SetFifoPretrig(10);
  broadcastALCT->SetDriftDelay(2);
  broadcastALCT->SetPretrigNumberOfLayers(number_of_layers_pretrig_);
  broadcastALCT->SetPretrigNumberOfPattern(number_of_layers_pattern_);
  broadcastALCT->WriteConfigurationReg();
  //
  in=NULL;
  this->Default(in, out);
  //
}
////
//void EmuPeripheralCrateBroadcast::SetOutputToMPCDisable(xgi::Input * in, xgi::Output * out )  {
//  //
//  DefineBroadcastCrate();
//  //
//  std::cout <<" Broadcast Disable TMB output to MPC..."<<std::endl;
//  //
//  broadcastTMB->SetMpcOutputEnable(0);
//  broadcastTMB->SetTmbAllowClct(0);
//  broadcastTMB->SetTmbAllowMatch(1);
//  broadcastTMB->WriteRegister(0x86);
//  //
//  in=NULL;
//  this->Default(in, out);
//  //
//}
//

void EmuPeripheralCrateBroadcast::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
    throw (toolbox::fsm::exception::Exception) {
  changeState(fsm);
}

void EmuPeripheralCrateBroadcast::dummyAction(toolbox::Event::Reference e)
    throw (toolbox::fsm::exception::Exception) {
  // currently do nothing
}

//
xoap::MessageReference EmuPeripheralCrateBroadcast::onConfigure (xoap::MessageReference message)
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Configure" << std::endl;
  //
  current_state_ = 1;
  fireEvent("Configure");
  //
  return createReply(message);
}

//
xoap::MessageReference EmuPeripheralCrateBroadcast::onEnable (xoap::MessageReference message)
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Enable" << std::endl;
  //
  current_state_ = 2;
  fireEvent("Enable");
  //
  return createReply(message);
}

//
xoap::MessageReference EmuPeripheralCrateBroadcast::onDisable (xoap::MessageReference message)
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Disable" << std::endl;
  //
  current_state_ = 1;
  fireEvent("Disable");
  //
  return createReply(message);
}

//
xoap::MessageReference EmuPeripheralCrateBroadcast::onHalt (xoap::MessageReference message)
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Halt" << std::endl;
  //
  current_state_ = 0;
  fireEvent("Halt");
  //
  return createReply(message);
}

 }  // namespace emu::pc
}  // namespace emu

//
// provides factory method for instantion of SimpleSOAPSender application
//
XDAQ_INSTANTIATOR_IMPL(emu::pc::EmuPeripheralCrateBroadcast)
//

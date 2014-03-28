// $Id: EmuPeripheralCrateManager.cc,v 1.29 2012/10/07 13:32:15 liu Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "emu/pc/EmuPeripheralCrateManager.h"

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
#include <unistd.h> // for sleep()
#include <sstream>
#include <cstdlib>

//using namespace cgicc;
//using namespace std;

namespace emu {
  namespace pc {

EmuPeripheralCrateManager::EmuPeripheralCrateManager(xdaq::ApplicationStub * s): EmuPeripheralCrateBase(s)
{	
  xgi::bind(this,&EmuPeripheralCrateManager::Default, "Default");
  xgi::bind(this,&EmuPeripheralCrateManager::MainPage, "MainPage");
  xgi::bind(this,&EmuPeripheralCrateManager::ForEmuPage1, "ForEmuPage1");
  //
  // Normal SOAP call-back function
  // SOAP call-back functions, which relays to *Action method.
  xoap::bind(this, &EmuPeripheralCrateManager::onConfigure, "Configure", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnable,    "Enable",    XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onDisable,   "Disable",   XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onHalt,      "Halt",      XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onConfigCalCFEB,"ConfigCalCFEB", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalCFEBGains,"EnableCalCFEBGains", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalCFEBCrossTalk,"EnableCalCFEBCrossTalk", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalCFEBSCAPed,"EnableCalCFEBSCAPed", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalCFEBComparator,"EnableCalCFEBComparator", XDAQ_NS_URI);

// alct calibrations (Madorsky)
  xoap::bind(this, &EmuPeripheralCrateManager::onConfigCalALCT,"ConfigCalALCT", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalALCTConnectivity,"EnableCalALCTConnectivity", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalALCTThresholds,"EnableCalALCTThresholds", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalALCTDelays,"EnableCalALCTDelays", XDAQ_NS_URI);
// ---
  //
  // fsm_ is defined in EmuApplication
  fsm_.addState('H', "Halted",     this, &EmuPeripheralCrateManager::stateChanged);
  fsm_.addState('C', "Configured", this, &EmuPeripheralCrateManager::stateChanged);
  fsm_.addState('E', "Enabled",    this, &EmuPeripheralCrateManager::stateChanged);
  //
  fsm_.addStateTransition('H', 'C', "Configure", this, &EmuPeripheralCrateManager::configureAction);
  fsm_.addStateTransition('C', 'C', "Configure", this, &EmuPeripheralCrateManager::configureAction);
  fsm_.addStateTransition('E', 'E', "Configure", this, &EmuPeripheralCrateManager::configureAction); // invalid, do nothing

  fsm_.addStateTransition('H', 'H', "Enable",    this, &EmuPeripheralCrateManager::enableAction); // invalid, do nothing
  fsm_.addStateTransition('C', 'E', "Enable",    this, &EmuPeripheralCrateManager::enableAction);
  fsm_.addStateTransition('E', 'E', "Enable",    this, &EmuPeripheralCrateManager::enableAction);

  fsm_.addStateTransition('H', 'H', "Disable",   this, &EmuPeripheralCrateManager::disableAction); // invalid, do nothing
  fsm_.addStateTransition('C', 'C', "Disable",   this, &EmuPeripheralCrateManager::disableAction);
  fsm_.addStateTransition('E', 'C', "Disable",   this, &EmuPeripheralCrateManager::disableAction);

  fsm_.addStateTransition('H', 'H', "Halt",      this, &EmuPeripheralCrateManager::haltAction);
  fsm_.addStateTransition('C', 'H', "Halt",      this, &EmuPeripheralCrateManager::haltAction);
  fsm_.addStateTransition('E', 'H', "Halt",      this, &EmuPeripheralCrateManager::haltAction);
  //
  fsm_.setInitialState('H');
  fsm_.reset();
  //
  // state_ is defined in EmuApplication
  state_ = fsm_.getStateName(fsm_.getCurrentState());
  //
  need_init = true;
  InFlash_plus = "-1";
  InFlash_minus = "-2";

  page1_state_ = 0;
  Page1States.push_back( "Halted" );      // 0
  Page1States.push_back( "Configured" );  // 1
  Page1States.push_back( "Enabled" );     // 2
  Page1States.push_back( "Calibration" ); // 3
  Page1States.push_back( "Local Run" );   // 4
  Page1States.push_back( "Global Run" );  // 5
}  
//
void EmuPeripheralCrateManager::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/"
       <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<std::endl;
}
//
/////////////////////////////////////////////////////////////////////////////////
// Main page description
/////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateManager::MainPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
  //
  MyHeader(in,out,"EmuPeripheralCrateManager");
}
//
//////////////////////////////////////////////////////////////////////////////
// Responses to soap messages
//////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateManager::configureAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
}
//
void EmuPeripheralCrateManager::enableAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
}  
//
void EmuPeripheralCrateManager::disableAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
}
//
void EmuPeripheralCrateManager::haltAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
}
//
void EmuPeripheralCrateManager::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
  throw (toolbox::fsm::exception::Exception) {
  changeState(fsm);
}
//
void EmuPeripheralCrateManager::MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
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


xoap::MessageReference EmuPeripheralCrateManager::onConfigCalCFEB (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {

  msgHandler("Get SOAP message ConfigCalCFEB");
  page1_state_ = 1;

  PCsendCommand("MonitorStop","emu::pc::EmuPeripheralCrateMonitor");
  PCsendCommand("ConfigCalCFEB","emu::pc::EmuPeripheralCrateBroadcast");
  PCsendCommand("ConfigCalCFEB","emu::pc::EmuPeripheralCrateCommand");
   //
  fireEvent("Configure");
  return createReply(message);
}
// ALCT calibrations (Madorsky)
xoap::MessageReference EmuPeripheralCrateManager::onConfigCalALCT (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {

  msgHandler("Get SOAP message ConfigCalALCT");
  page1_state_ = 1;

   PCsendCommand("ConfigCalALCT","emu::pc::EmuPeripheralCrateCommand");
   //

   fireEvent("Configure");
  return createReply(message);
}
// ---
xoap::MessageReference EmuPeripheralCrateManager::onEnableCalCFEBCrossTalk (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {

  msgHandler("Get SOAP message EnableCalCFEBCrossTalk");
  page1_state_ = 3;

  PCsendCommand("EnableCalCFEBCrossTalk","emu::pc::EmuPeripheralCrateBroadcast");
  //
  fireEvent("Enable");
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateManager::onEnableCalCFEBSCAPed (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {

  msgHandler("Get SOAP message EnableCalCFEBSCAPed");
  page1_state_ = 3;

  PCsendCommand("EnableCalCFEBSCAPed","emu::pc::EmuPeripheralCrateBroadcast");
  //
  fireEvent("Enable");
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateManager::onEnableCalCFEBGains (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {

  msgHandler("Get SOAP message EnableCalCFEBGains");
  page1_state_ = 3;

  PCsendCommand("EnableCalCFEBGains","emu::pc::EmuPeripheralCrateBroadcast");
  //
  fireEvent("Enable");
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateManager::onEnableCalCFEBComparator (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {

  msgHandler("Get SOAP message EnableCalCFEBComparator");
  page1_state_ = 3;

  PCsendCommand("EnableCalCFEBComparator","emu::pc::EmuPeripheralCrateBroadcast");
  //
  fireEvent("Enable");
  return createReply(message);
}

// alct calibrations (Madorsky)
xoap::MessageReference EmuPeripheralCrateManager::onEnableCalALCTConnectivity (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {

  msgHandler("Get SOAP message EnableCalALCTConnectivity");
  page1_state_ = 3;

  PCsendCommand("EnableCalALCTConnectivity","emu::pc::EmuPeripheralCrateCommand");
  //
  fireEvent("Enable");
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateManager::onEnableCalALCTThresholds (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {

  msgHandler("Get SOAP message EnableCalALCTThresholds");
  page1_state_ = 3;

  PCsendCommand("EnableCalALCTThresholds","emu::pc::EmuPeripheralCrateCommand");
  //
  fireEvent("Enable");
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateManager::onEnableCalALCTDelays (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {

  msgHandler("Get SOAP message EnableCalALCTDelays");
  page1_state_ = 3;

  PCsendCommand("EnableCalALCTDelays","emu::pc::EmuPeripheralCrateCommand");
  //
  fireEvent("Enable");
  return createReply(message);
}
// ---
xoap::MessageReference EmuPeripheralCrateManager::onConfigure (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {

  msgHandler("Get SOAP message Configure");
  page1_state_ = 1;

//  PCsendCommand("MonitorStop","emu::pc::EmuPeripheralCrateMonitor");
  PCsendCommand("Configure","emu::pc::EmuPeripheralCrateCommand");
  PCsendCommand("Configure","emu::pc::EmuPeripheralCrateBroadcast");
  //
  fireEvent("Configure");
  //
  return createReply(message);
  //
}

xoap::MessageReference EmuPeripheralCrateManager::onEnable (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {

  msgHandler("Get SOAP message Enable");
  page1_state_ = 2;

  PCsendCommand("Enable","emu::pc::EmuPeripheralCrateCommand");
  PCsendCommand("Enable","emu::pc::EmuPeripheralCrateBroadcast");
//  PCsendCommand("MonitorStart","emu::pc::EmuPeripheralCrateMonitor");
  //
  fireEvent("Enable");
  //
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateManager::onDisable (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {

  msgHandler("Get SOAP message Disable");
  page1_state_ = 1;

  PCsendCommand("Disable","emu::pc::EmuPeripheralCrateCommand");
  PCsendCommand("Disable","emu::pc::EmuPeripheralCrateBroadcast");
//  PCsendCommand("MonitorStop","emu::pc::EmuPeripheralCrateMonitor");
  //
  fireEvent("Disable");
  //
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateManager::onHalt (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {

  msgHandler("Get SOAP message Halt");
  page1_state_ = 0;

  PCsendCommand("Halt","emu::pc::EmuPeripheralCrateCommand");
  PCsendCommand("Halt","emu::pc::EmuPeripheralCrateBroadcast");
//  PCsendCommand("MonitorStop","emu::pc::EmuPeripheralCrateMonitor");
  //
  fireEvent("Halt");
  //
  return createReply(message);
}

void EmuPeripheralCrateManager::msgHandler(std::string msg)
{
     std::string logmsg = getLocalDateTime() + " " + msg;
     LOG4CPLUS_INFO(getApplicationLogger(), logmsg);
     std::cout << "        " << logmsg << std::endl;
}

void EmuPeripheralCrateManager::ForEmuPage1(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  if(need_init)
  {
     emu::db::TStoreReadWriter *myTStore = GetEmuTStore();
     if(myTStore)
     {    
       try 
       {
          xdata::UnsignedInteger64 flashid64 = myTStore->readLastConfigIdFlashed("plus");
          InFlash_plus = flashid64.toString();
          flashid64 = myTStore->readLastConfigIdFlashed("minus");
          InFlash_minus = flashid64.toString();
       }
       catch (emu::exception::ConfigurationException &e)
       {
         msgHandler("Could not read last flashed configIDs: " + std::string(e.what()));
       }
       catch (...)
       {
         msgHandler("Could not read last flashed configIDs (unknown exception)");
       }
     }
     else
     {  std::cout << "Can't create object TStoreReadWriter" << std::endl;
     }
     need_init = false;
     delete myTStore;
  }
  time_t t;
  time(&t);
  *out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl
       << "<?xml-stylesheet type=\"text/xml\" href=\"/emu/base/html/EmuPage1_XSL.xml\"?>" << std::endl
       << "<ForEmuPage1 application=\"" << getApplicationDescriptor()->getClassName()
       <<                   "\" url=\"" << getApplicationDescriptor()->getContextDescriptor()->getURL()
       <<         "\" localUnixTime=\"" << t
       <<         "\" localDateTime=\"" << getLocalDateTime() << "\">" << std::endl;

    *out << "  <monitorable name=\"" << "title"
         <<            "\" value=\"" << "PCrate Manager"
         <<  "\" nameDescription=\"" << " "
         << "\" valueDescription=\"" << " "
         <<          "\" nameURL=\"" << " "
         <<         "\" valueURL=\"" << " "
         << "\"/>" << std::endl;

    *out << "  <monitorable name=\"" << "State"
         <<            "\" value=\"" << Page1States[page1_state_]
         <<  "\" nameDescription=\"" << " "
         << "\" valueDescription=\"" << " "
         <<          "\" nameURL=\"" << " "
         <<         "\" valueURL=\"" << " "
         << "\"/>" << std::endl;

    *out << "  <monitorable name=\"" << "Key+"
         <<            "\" value=\"" << InFlash_plus
         <<  "\" nameDescription=\"" << "Plus Endcap"
         << "\" valueDescription=\"" << "Configuration Key currently in the FLASH"
         <<          "\" nameURL=\"" << " "
         <<         "\" valueURL=\"" << " "
         << "\"/>" << std::endl;

    *out << "  <monitorable name=\"" << "Key-"
         <<            "\" value=\"" << InFlash_minus
         <<  "\" nameDescription=\"" << "Minus Endcap"
         << "\" valueDescription=\"" << "Configuration Key currently in the FLASH"
         <<          "\" nameURL=\"" << " "
         <<         "\" valueURL=\"" << " "
         << "\"/>" << std::endl;

  *out << "</ForEmuPage1>" << std::endl;
}

  } // namespace emu::pc
} // namespace emu
//
//
XDAQ_INSTANTIATOR_IMPL(emu::pc::EmuPeripheralCrateManager)

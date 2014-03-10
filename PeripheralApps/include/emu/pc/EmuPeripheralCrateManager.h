// $Id: EmuPeripheralCrateManager.h,v 1.8 2010/01/29 14:44:08 liu Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _EmuPeripheralCrateManager_h_
#define _EmuPeripheralCrateManager_h_

#include "xgi/Utils.h"
#include "xgi/Method.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"

#include "xdaq/NamespaceURI.h"

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "emu/pc/EmuPeripheralCrateBase.h"

namespace emu {
  namespace pc {
  
class EmuPeripheralCrateManager: public EmuPeripheralCrateBase
{
protected:
  int page1_state_;
  std::vector< std::string> Page1States;
  bool need_init;
  std::string InFlash_plus, InFlash_minus;
  
public:
  
  XDAQ_INSTANTIATOR();
  
  //
  EmuPeripheralCrateManager(xdaq::ApplicationStub * s);
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MainPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void ForEmuPage1(xgi::Input *in, xgi::Output *out) throw (xgi::exception::Exception);
  void msgHandler(std::string msg);
  //
  void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
  void enableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
  void disableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
  void haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
  void stateChanged(toolbox::fsm::FiniteStateMachine &fsm) throw (toolbox::fsm::exception::Exception);
  void MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) throw (xgi::exception::Exception);
  //
  // What to do when we receive soap messages
  //
  xoap::MessageReference onConfigure (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnable (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onDisable (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onHalt (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onConfigCalCFEB (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalCFEBCrossTalk (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalCFEBSCAPed (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalCFEBGains (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalCFEBComparator (xoap::MessageReference message) throw (xoap::exception::Exception);
// ALCT calibrations (Madorsky)
  xoap::MessageReference onConfigCalALCT (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalALCTConnectivity (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalALCTThresholds (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalALCTDelays (xoap::MessageReference message) throw (xoap::exception::Exception);
// ---
//
};

  } // namespace emu::pc
} // namespace emu
  
#endif

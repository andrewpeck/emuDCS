//-----------------------------------------------------------------------
// $Id: Crate.h,v 1.12 2012/09/05 21:39:11 liu Exp $
// $Log: Crate.h,v $
// Revision 1.12  2012/09/05 21:39:11  liu
// remove ODMB class
//
// Revision 1.10  2012/04/24 14:42:16  liu
// add new functions to delete VMEModule and Chamber from Crate
//
// Revision 1.9  2012/02/22 17:53:23  liu
// new function to return Chamber name
//
// Revision 1.8  2011/10/28 18:13:37  liu
// add DDU class
//
// Revision 1.7  2010/08/25 19:45:41  liu
// read TMB voltages in VME jumbo packet
//
// Revision 1.6  2009/12/08 11:41:39  liu
// sort chambers within a crate if they are in random order
//
// Revision 1.5  2009/10/25 09:54:45  liu
// add a new parameter power_mask for DMB, more counters for CCB
//
// Revision 1.4  2009/08/10 15:03:04  liu
// expand configure() to include a second argument: configID
//
// Revision 1.3  2009/04/21 13:23:49  liu
// introduce dead channel masks in monitoring
//
// Revision 1.2  2009/03/31 16:24:20  liu
// move check controller to Crate class
//
// Revision 1.1  2009/03/25 10:07:42  liu
// move header files to include/emu/pc
//
// Revision 3.16  2008/08/22 13:05:36  liu
// monitoring update
//
// Revision 3.15  2008/08/13 11:30:53  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.14  2008/06/22 14:57:52  liu
// new functions for monitoring
//
// Revision 3.13  2008/05/20 10:46:01  liu
// error handling update
//
// Revision 3.12  2008/04/25 15:35:31  liu
// updates on VMECC
//
// Revision 3.11  2008/02/21 09:37:24  liu
// fast config option
//
// Revision 3.10  2008/02/18 12:08:48  liu
// new functions for monitoring
//
// Revision 3.9  2007/12/27 00:34:25  liu
// update
//
// Revision 3.8  2007/12/25 13:55:43  liu
// update
//
// Revision 3.7  2007/12/17 15:02:38  liu
// update
//
// Revision 3.6  2007/07/17 16:16:09  liu
// remove ChamberUtilities dependence
//
// Revision 3.5  2006/11/28 14:17:15  mey
// UPdate
//
// Revision 3.4  2006/11/15 16:01:36  mey
// Cleaning up code
//
// Revision 3.3  2006/11/13 16:25:31  mey
// Update
//
// Revision 3.2  2006/10/30 15:53:39  mey
// Update
//
// Revision 3.1  2006/10/03 07:36:01  mey
// UPdate
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.6  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.5  2006/03/24 14:35:03  mey
// Update
//
// Revision 2.4  2006/01/18 19:38:16  mey
// Fixed bugs
//
// Revision 2.3  2006/01/18 12:46:18  mey
// Update
//
// Revision 2.2  2006/01/10 23:31:58  mey
// Update
//
// Revision 2.1  2005/12/20 23:39:13  mey
// UPdate
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef CRATE_h
#define CRATE_h
//
#include <vector>
#include <iostream>
//

namespace emu {
  namespace pc {

class VMEModule;
class VMEController;
class ALCTController;
class TMB;
class DAQMB;
class CCB;
class MPC;
class Chamber;
class VMECC;
class DDU;

class Crate {
public:
  Crate(int, VMEController *);
  ~Crate();
  
  int CrateID() const {return theCrateID;}
  
  void enable();
  void disable();
  int configure(int, int ID=0);
  void init();
  void addModule(VMEModule * module);
  void deleteModule(VMEModule * module);
  void AddChamber(Chamber * chamber);
  void deleteChamber();
  void DumpConfiguration();
  int CheckController();
  //
  std::string GetLabel() {return label_;}
  inline void SetLabel(std::string label ) {label_ = label;}
  inline void SetLife(bool life) { alive_=life; }
  bool IsAlive();
  //
  VMEController * vmeController() const {return theController;}

  /// uses RTTI to find types
  std::vector<DAQMB *> daqmbs() const;
  std::vector<TMB *> tmbs() const;
  std::vector<ALCTController *> alcts() const;
  std::vector<Chamber *> chambers();
  std::vector<DDU *> ddus() const;
  //
  VMECC * vmecc() const;
  CCB * ccb() const;
  MPC * mpc() const;
  Chamber * GetChamber(int slot);
  Chamber * GetChamber(TMB *tmb);
  Chamber * GetChamber(DAQMB *dmb);
  TMB * GetTMB(unsigned int slot);
  DAQMB * GetDAQMB(unsigned int slot);
  std::string GetChamberName(int n);
  void PowerOff();
  void MonitorCCB(int cycle, char * buf);
  void MonitorTMB(int cycle, char * buf, unsigned mask=0);
  void MonitorDMB(int cycle, char * buf, unsigned mask=0);
  void MonitorDCS(int cycle, char * buf, unsigned mask=0);
  void MonitorDCS2(int cycle, char * buf, unsigned mask=0);
  void MonitorTCS(int cycle, char * buf, unsigned mask=0);

private:

  template<class T> T * findBoard() const
  {
    for(unsigned i = 0; i < theModules.size(); ++i) {
      T * result = dynamic_cast<T *>(theModules[i]);
      if(result != 0) return result;
    }
    return 0;
  }
  
  int theCrateID;
  std::string label_;
  bool alive_;
  bool sorted_;
  /// indexed by slot 
  std::vector<VMEModule *> theModules;
  std::vector<Chamber *> theChambers;
  std::vector<Chamber *> sortedChambers;
  VMEController * theController;
};


  } // namespace emu::pc
  } // namespace emu
#endif


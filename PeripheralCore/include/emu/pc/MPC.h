//-----------------------------------------------------------------------
// $Id: MPC.h,v 1.6 2012/09/05 22:34:46 liu Exp $
// $Log: MPC.h,v $
// Revision 1.6  2012/09/05 22:34:46  liu
// introduce HardwareVersion attribute
//
// Revision 1.5  2011/08/25 21:08:57  liu
// add functions for new (Virtex 5) MPC
//
// Revision 1.4  2011/06/30 21:26:36  liu
// add setDelay function
//
// Revision 1.3  2010/05/21 12:15:20  liu
// add MPC mask
//
// Revision 1.2  2009/03/25 10:19:41  liu
// move header files to include/emu/pc
//
// Revision 1.1  2009/03/25 10:07:42  liu
// move header files to include/emu/pc
//
// Revision 3.11  2008/08/13 11:30:53  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.10  2008/08/08 11:01:23  rakness
// centralize logging
//
// Revision 3.9  2008/06/12 21:08:54  rakness
// add firmware tags for DMB, CFEB, MPC, CCB into xml file; add check firmware button
//
// Revision 3.8  2008/04/11 14:48:47  liu
// add CheckConfig() function
//
// Revision 3.7  2008/01/08 10:58:56  liu
// remove exit() in functions
//
// Revision 3.6  2007/08/28 18:05:37  liu
// remove unused & outdated functions
//
// Revision 3.5  2007/08/27 22:50:39  liu
// update
//
// Revision 3.4  2006/10/30 15:55:01  mey
// Update
//
// Revision 3.3  2006/09/27 16:50:50  mey
// Update
//
// Revision 3.2  2006/08/09 11:56:23  mey
// Got rid of friend classes in MPCParser
//
// Revision 3.1  2006/08/07 14:14:11  mey
// Added BoardId
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.18  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.17  2006/07/12 07:58:18  mey
// Update
//
// Revision 2.16  2006/07/05 09:29:18  mey
// Update
//
// Revision 2.15  2006/06/16 13:05:24  mey
// Got rid of Compiler switches
//
// Revision 2.14  2006/01/13 10:15:26  mey
// Update
//
// Revision 2.13  2006/01/13 10:07:00  mey
// Got rid of virtuals
//
// Revision 2.12  2006/01/12 23:43:53  mey
// Update
//
// Revision 2.11  2006/01/12 22:35:50  mey
// UPdate
//
// Revision 2.10  2006/01/12 12:27:56  mey
// UPdate
//
// Revision 2.9  2006/01/11 13:26:37  mey
// Update
//
// Revision 2.8  2005/12/10 04:47:21  mey
// Fix bug
//
// Revision 2.7  2005/12/08 12:00:16  mey
// Update
//
// Revision 2.6  2005/11/25 23:42:02  mey
// Update
//
// Revision 2.5  2005/11/22 15:14:39  mey
// Update
//
// Revision 2.4  2005/11/21 17:38:45  mey
// Update
//
// Revision 2.3  2005/08/22 07:55:44  mey
// New TMB MPC injector routines and improved ALCTTiming
//
// Revision 2.2  2005/08/17 12:27:22  mey
// Updated FindWinner routine. Using FIFOs now
//
// Revision 2.1  2005/06/06 19:22:35  geurts
// additions for MPC/SP connectivity tests (Dan Holmes)
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef MPC_h
#define MPC_h
#include "emu/pc/VMEModule.h"
#include "emu/pc/EmuLogger.h"
#include <string>

namespace emu {
  namespace pc {




class Crate;

class MPC : public VMEModule, public EmuLogger {
 public:
  //friend class MPCParser;


  MPC(Crate * , int slot);
  ~MPC();
  void firmwareVersion();
  void printFirmwareVersion();

  /// from the BOARDTYPE enum
  inline std::vector<unsigned long int> GetFIFOBLct0() { return FIFOBLct0 ; }
  inline std::vector<unsigned long int> GetFIFOBLct1() { return FIFOBLct1 ; }
  inline std::vector<unsigned long int> GetFIFOBLct2() { return FIFOBLct2 ; }
  //
  inline void ResetFIFOBLct() {
    FIFOBLct0.clear();
    FIFOBLct1.clear();
    FIFOBLct2.clear();
  }
  //
  unsigned int boardType() const {return MPC_ENUM;} 
  //
  void start();
  //
  friend std::ostream & operator<<(std::ostream & os, MPC & ccb);
  //
  bool SelfTest();
  void configure();  
  void init();
  int CheckConfig();
  //
  bool CheckFirmwareDate();
  //
  /// address is usually one of the above enums.  theBaseAddress
  /// defined in the constructor automatically added
  void read_fifo(unsigned address, char * data);

  void read_fifos();
  void read_fifosA();

  // dump_fifos just writes read_fifos output to file

  /// control status register
  void read_csr0();
 
  //fg void read_date();

  void read_status();

  void setTLK2501TxMode(int);

  void setDelay(int delay);

  void SoftReset() ;
  //
  inline void SetBoardID(int value){BoardId_ = value;}
  inline int  GetBoardID(){return BoardId_;}
  //
  inline void SetTMBDelays(int value){TMBDelayPattern_ = value;}
  //
  inline void SetTransparentMode(int value){TransparentModeSources_ = value;}
  inline int  GetTransparentMode(){return TransparentModeSources_;}
  //
  inline void SetTLK2501TxMode(int value){TLK2501TxMode_ = value;}
  inline int  GetSerializerMode(){return TLK2501TxMode_;}
  //
  inline void SetExpectedFirmwareDay(int day) {expected_firmware_day_ = day;}
  inline int  GetExpectedFirmwareDay() { return expected_firmware_day_;}
  //
  inline void SetExpectedFirmwareMonth(int month) {expected_firmware_month_ = month;}
  inline int  GetExpectedFirmwareMonth() { return expected_firmware_month_;}
  //
  inline void SetExpectedFirmwareYear(int year) {expected_firmware_year_ = year;}
  inline int  GetExpectedFirmwareYear() { return expected_firmware_year_;}
  //
  inline void SetHardwareVersion(int version) {hardware_version_ = version;}
  inline int GetHardwareVersion() {return hardware_version_;}
  //
  /// used by GUIs
  void executeCommand(std::string command);

  void WriteRegister(int,int);
  int  ReadRegister(int);

  void enablePRBS();
  void disablePRBS();

  int ReadMask();
  void WriteMask(int mask);

  void injectSP();      // injects some test patterns into fifos :dan Holmes
  int injectSP(char*); // as above you give me file with data in.:dan Holmes

  void setTransparentMode();
  void setTransparentMode(unsigned int);
  void setSorterMode();
  void check_generation();
  int readDSN(void *data);

 protected:
  /// MPC base address should always correspond to VME Slot 12 (=0x600000)
//  static const unsigned long int theBaseAddress=0x00600000;
  enum VMEAddresses {
    /// FIFOs Ax[a,b] correspond to TMB x, a=[15-0] and b=[31-16]
    FIFO_A1a = 0x80, FIFO_A1b = 0x82, FIFO_A2a = 0x84, FIFO_A2b = 0x86, 
    FIFO_A3a = 0x88, FIFO_A3b = 0x8A, FIFO_A4a = 0x8C, FIFO_A4b = 0x8E, 
    FIFO_A5a = 0x90, FIFO_A5b = 0x92, FIFO_A6a = 0x94, FIFO_A6b = 0x96, 
    FIFO_A7a = 0x98, FIFO_A7b = 0x9A, FIFO_A8a = 0x9C, FIFO_A8b = 0x9E, 
    FIFO_A9a = 0xA0, FIFO_A9b = 0xA2, 
    /// FIFOs Bx correspond to the x-th best selected LCT
    FIFO_B1  = 0xA4, FIFO_B2  = 0xA6, FIFO_B3  = 0xA8,
    /// Various Control & Status registers (CSR3 read only)
    CSR0 = 0x00, CSR1 = 0xAA, CSR2 = 0xAC,
    CSR3 = 0xAE, CSR4 = 0xB8, CSR5 = 0xBA,
    CSR6 = 0xBC, CSR7 = 0xCA, CSR8 = 0xCC,
    // permanent TMB mask register in new MPC
    CSRM = 0x06,
    /// L1-accept counter (read only)
    L1ACC = 0xB0,
    /// Transmit 511 words from all FIFO_A in test-mode (write only)
    TXMIT511 = 0xB2,
    /// Send TxEn "0" pulse to all three TLK2501 transmitters (write only)
    TXENALL = 0xB6,
    // DATE     = 0xAA, STATUS   = 0xAE
    DSNreset   = 0xc0,
    DSNread    = 0xc2,
    DSNclear   = 0xc4,
    DSNwrite0  = 0xc6,
    DSNwrite1  = 0xc8
  };
  enum CSR0options {
    CSR0_FPGATestMode = 0x01, CSR0_FPGAResetLogic = 0x02,
    CSR0_FPGAHardReset = 0x04,
    CSR0_TDI = 0x08, CSR0_TMS = 0x10, CSR0_TCK = 0x20, CSR0_TDO = 0x40,
    CSR0_TXEN = 0x80, CSR0_MASKCOMP = 0x100
  };


 private:
  //
  std::vector<unsigned long int> FIFOBLct0;
  std::vector<unsigned long int> FIFOBLct1;
  std::vector<unsigned long int> FIFOBLct2;
  //
  int BoardId_;
  int TLK2501TxMode_;
  int TransparentModeSources_;
  int TMBDelayPattern_;
  //
  int read_firmware_day_;
  int read_firmware_month_; 
  int read_firmware_year_;
  //
  int expected_firmware_day_;
  int expected_firmware_month_; 
  int expected_firmware_year_;
  int mpc_generation;
  int hardware_version_;
  //
};

  } // namespace emu::pc
  } // namespace emu
#endif

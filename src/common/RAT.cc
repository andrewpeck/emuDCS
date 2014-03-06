#include <iostream>
#include <iomanip>
#include <unistd.h> // for sleep
#include <vector>
#include <string>
//
#include "emu/pc/RAT.h"
#include "emu/pc/TMB.h"
#include "emu/pc/TMB_constants.h"
//
//

namespace emu {
  namespace pc {



RAT::RAT(TMB * tmb) : EmuLogger()
{
  //
#ifdef debugV
  (*MyOutput_) << "RAT board" << std::endl;
#endif
  //
  tmb_ = tmb;
  //
  SetPowerUpUser2Register_();
  SetRatTmbDelay(9);
  //
  tmb_->SetCheckJtagWrite(false);
  //
  rat_configuration_status_ = -1;
  //
};
//
RAT::~RAT() {
  //
  //
}
//
void RAT::configure() {
  //
  // the configuration for the RAT has been moved into TMB
  //
  //  WriteRpcRatDelay();
  //  if (tmb_->GetCheckJtagWrite())
  //    PrintRatUser1();
  //
  // The following should probably go in TMB configure, since it is actually a delay on the TMB:
  //  WriteRatTmbDelay();
  //
  return;
}
//
void RAT::CheckRATConfiguration() {
  //
  bool config_ok = true;
  //
  ReadRatUserCode(); // fill the read values in the software
  //
  config_ok &= compareValues("RAT Firmware Day"  ,GetReadRatFirmwareDay()  ,tmb_->GetExpectedRatFirmwareDay()  );
  config_ok &= compareValues("RAT Firmware Month",GetReadRatFirmwareMonth(),tmb_->GetExpectedRatFirmwareMonth());
  config_ok &= compareValues("RAT Firmware Year" ,GetReadRatFirmwareYear() ,tmb_->GetExpectedRatFirmwareYear() );
  //
  //  ReportCheck("RAT configuration check",config_ok);
  //
  rat_configuration_status_ = (int) config_ok;
  //
  return;
}
//
///////////////////////////////////
// user and ID codes...
///////////////////////////////////
void RAT::ReadRatIdCode(){
  //
  //  (*MyOutput_) << "RAT: Read RAT ID Codes" << std::endl;
  //
  tmb_->setup_jtag(ChainRat);
  //
  tmb_->ShfIR_ShfDR(ChipLocationRatFpga,
		    FPGAidCode,
		    RegSizeRatFpga_FPGAidCode);
  rat_idcode_[0] = tmb_->bits_to_int(tmb_->GetDRtdo(),
				     tmb_->GetRegLength(),
				     LSBfirst);
  //
  //
  tmb_->ShfIR_ShfDR(ChipLocationRatProm,
		    PROMidCode,
		    RegSizeRatProm_PROMidCode);
  //
  rat_idcode_[1] = tmb_->bits_to_int(tmb_->GetDRtdo(),
				     tmb_->GetRegLength(),
				     LSBfirst);
  //
  //  (*MyOutput_) << "RAT FPGA ID code = " << std::hex << rat_idcode_[ChipLocationRatFpga] << std::endl;
  //  (*MyOutput_) << "RAT PROM ID code = " << std::hex << rat_idcode_[ChipLocationRatProm] << std::endl;
  //
  return;
}
//
int RAT::GetRatIdCode(int device) {
  // 
  return rat_idcode_[device] ; 
}
//
//
void RAT::ReadRatUserCode(){
  //
  //  (*MyOutput_) << "RAT: Read RAT User Codes" << std::endl;
  //
  tmb_->setup_jtag(ChainRat);
  //
  tmb_->ShfIR_ShfDR(ChipLocationRatFpga,
		    FPGAuserCode,
		    RegSizeRatFpga_FPGAuserCode);
  //
  rat_usercode_[ChipLocationRatFpga] = tmb_->bits_to_int(tmb_->GetDRtdo(),
							 tmb_->GetRegLength(),
							 LSBfirst);
  //
  //
  tmb_->ShfIR_ShfDR(ChipLocationRatProm,
		    PROMuserCode,
		    RegSizeRatProm_PROMuserCode);
  //
  rat_usercode_[ChipLocationRatProm] = tmb_->bits_to_int(tmb_->GetDRtdo(),
							 tmb_->GetRegLength(),
							 LSBfirst);
  //
  //  (*MyOutput_) << "RAT FPGA User code = " << std::hex << rat_usercode_[ChipLocationRatFpga] << std::endl;
  //  (*MyOutput_) << "RAT PROM User code = " << std::hex << rat_usercode_[ChipLocationRatProm] << std::endl;
  //
  read_rat_firmware_month_ =  (rat_usercode_[ChipLocationRatFpga] >> 24) & 0xff;
  read_rat_firmware_day_   =  (rat_usercode_[ChipLocationRatFpga] >> 16) & 0xff;
  read_rat_firmware_year_  =  (rat_usercode_[ChipLocationRatFpga] >>  0) & 0xffff;
  // convert to real decimal
  read_rat_firmware_day_ = (read_rat_firmware_day_ >> 4)*10 + (read_rat_firmware_day_ & 0xF);
  read_rat_firmware_month_ = (read_rat_firmware_month_ >> 4)*10 + (read_rat_firmware_month_ & 0xF);
  read_rat_firmware_year_ = ((read_rat_firmware_year_ >> 12)&0xF)*1000 + ((read_rat_firmware_year_ >> 8)&0xF)*1000+ ((read_rat_firmware_year_ >> 4)&0xF)*10 +(read_rat_firmware_year_&0xF);
  //
  return;
}
//
int RAT::GetRatUserCode(int device) { 
  //
  return rat_usercode_[device] ; 
}
//
//
/////////////////////////////////////
// User1 Register (read register):
/////////////////////////////////////
void RAT::ReadRatUser1(){
  //
  (*MyOutput_) << "RAT: Read RAT USER1 JTAG (status register)" << std::endl;
  //
  //clear user1_value_
  for (int i=0; i<MAX_NUM_FRAMES; i++) 
    user1_value_[i] = 0;
  //
  tmb_->setup_jtag(ChainRat);
  //	       
  tmb_->ShfIR_ShfDR(ChipLocationRatFpga,
		    FPGAuser1,
		    RegSizeRatFpga_FPGAuser1);
  //
  //Fill user1_value_ with JTAG data...
  rat_user1_length_ = tmb_->GetRegLength();
  int * user1_pointer = tmb_->GetDRtdo();
  //
  for (int i=0; i<rat_user1_length_; i++) 
    user1_value_[i] = *(user1_pointer+i);
  //  
  //  
  //Print out the USER1 value from right (first bit out) to left (last bit out):
  char rat_user1[rat_user1_length_/8];
  tmb_->packCharBuffer(user1_value_,rat_user1_length_,rat_user1);
  //
  (*MyOutput_) << "RAT USER1 = " << std::hex;
  for (int counter=(rat_user1_length_/8)-1; counter>=0; counter--) 
    (*MyOutput_) << ((rat_user1[counter] >> 4) & 0xf) << (rat_user1[counter] & 0xf);
  (*MyOutput_) << std::endl;
  //
  decodeRATUser1_();
  //
  return;
}
//
void RAT::decodeRATUser1_() {
  // ** parse the bit array from the USER1 data register
  //  
  read_rs_begin_ = tmb_->bits_to_int(user1_value_ + read_rs_begin_bitlo,
				     read_rs_begin_bithi - read_rs_begin_bitlo + 1,
				     LSBfirst);
  //
  read_rs_version_ = tmb_->bits_to_int(user1_value_ + read_rs_version_bitlo,
				       read_rs_version_bithi - read_rs_version_bitlo + 1,
				       LSBfirst);
  //
  read_rs_monthday_ = tmb_->bits_to_int(user1_value_ + read_rs_monthday_bitlo,
					read_rs_monthday_bithi - read_rs_monthday_bitlo + 1,
					LSBfirst);
  //
  read_rs_year_ = tmb_->bits_to_int(user1_value_ + read_rs_year_bitlo,
				    read_rs_year_bithi - read_rs_year_bitlo + 1,			 
				    LSBfirst);
  //
  read_rs_syncmode_ = tmb_->bits_to_int(user1_value_ + read_rs_syncmode_bitlo,
					read_rs_syncmode_bithi - read_rs_syncmode_bitlo + 1,			 
					LSBfirst);
  //
  read_rs_posneg_ = tmb_->bits_to_int(user1_value_ + read_rs_posneg_bitlo,
				      read_rs_posneg_bithi - read_rs_posneg_bitlo + 1,			 
				      LSBfirst);
  //
  read_rs_loop_ = tmb_->bits_to_int(user1_value_ + read_rs_loop_bitlo,
				    read_rs_loop_bithi - read_rs_loop_bitlo + 1,			 
				    LSBfirst);
  //
  read_rs_rpc_en_ = tmb_->bits_to_int(user1_value_ + read_rs_rpc_en_bitlo,
				      read_rs_rpc_en_bithi - read_rs_rpc_en_bitlo + 1,			 
				      LSBfirst);
  //
  read_rs_clk_active_ = tmb_->bits_to_int(user1_value_ + read_rs_clk_active_bitlo,
					  read_rs_clk_active_bithi - read_rs_clk_active_bitlo + 1,			 
					  LSBfirst);
  //
  read_rs_locked_tmb_ = tmb_->bits_to_int(user1_value_ + read_rs_locked_tmb_bitlo,
					  read_rs_locked_tmb_bithi - read_rs_locked_tmb_bitlo + 1,			 
					  LSBfirst);
  //
  read_rs_locked_rpc0_ = tmb_->bits_to_int(user1_value_ + read_rs_locked_rpc0_bitlo,
					   read_rs_locked_rpc0_bithi - read_rs_locked_rpc0_bitlo + 1,			 
					   LSBfirst);
  //
  read_rs_locked_rpc1_ = tmb_->bits_to_int(user1_value_ + read_rs_locked_rpc1_bitlo,
					   read_rs_locked_rpc1_bithi - read_rs_locked_rpc1_bitlo + 1,			 
					   LSBfirst);
  //
  read_rs_locklost_tmb_ = tmb_->bits_to_int(user1_value_ + read_rs_locklost_tmb_bitlo, 
					    read_rs_locklost_tmb_bithi - read_rs_locklost_tmb_bitlo + 1, 
					    LSBfirst);
  //
  read_rs_locklost_rpc0_ = tmb_->bits_to_int(user1_value_ + read_rs_locklost_rpc0_bitlo, 
					     read_rs_locklost_rpc0_bithi - read_rs_locklost_rpc0_bitlo + 1,
					     LSBfirst);
  //
  read_rs_locklost_rpc1_ = tmb_->bits_to_int(user1_value_ + read_rs_locklost_rpc1_bitlo, 
					     read_rs_locklost_rpc1_bithi - read_rs_locklost_rpc1_bitlo + 1,
					     LSBfirst);
  //
  read_rs_txok_ = tmb_->bits_to_int(user1_value_ + read_rs_txok_bitlo, 
				    read_rs_txok_bithi - read_rs_txok_bitlo + 1,			 
				    LSBfirst);
  //
  read_rs_rxok_ = tmb_->bits_to_int(user1_value_ + read_rs_rxok_bitlo, 
				    read_rs_rxok_bithi - read_rs_rxok_bitlo + 1,			 
				    LSBfirst);
  //
  read_rs_ntcrit_ = tmb_->bits_to_int(user1_value_ + read_rs_ntcrit_bitlo, 
				      read_rs_ntcrit_bithi - read_rs_ntcrit_bitlo + 1,			 
				      LSBfirst);
  //
  read_rs_rpc_free_ = tmb_->bits_to_int(user1_value_ + read_rs_rpc_free_bitlo, 
					read_rs_rpc_free_bithi - read_rs_rpc_free_bitlo + 1,			 
					LSBfirst);
  //
  read_rs_dsn_ = tmb_->bits_to_int(user1_value_ + read_rs_dsn_bitlo, 
				   read_rs_dsn_bithi - read_rs_dsn_bitlo + 1,			 
				   LSBfirst);
  //
  read_rs_dddoe_wr_ = tmb_->bits_to_int(user1_value_ + read_rs_dddoe_wr_bitlo,
					read_rs_dddoe_wr_bithi - read_rs_dddoe_wr_bitlo + 1,			 
					LSBfirst);
  //
  read_rs_ddd_wr_ = tmb_->bits_to_int(user1_value_ + read_rs_ddd_wr_bitlo,
				      read_rs_ddd_wr_bithi - read_rs_ddd_wr_bitlo + 1,			 
				      LSBfirst);
  for (int rpc=0; rpc<4; rpc++)
    read_rpc_rat_delay_[rpc] = (read_rs_ddd_wr_ >> rpc*4) & 0xf;
  //
  read_rs_ddd_auto_ = tmb_->bits_to_int(user1_value_ + read_rs_ddd_auto_bitlo, 
					read_rs_ddd_auto_bithi - read_rs_ddd_auto_bitlo + 1,			 
					LSBfirst);
  //
  read_rs_ddd_start_ = tmb_->bits_to_int(user1_value_ + read_rs_ddd_start_bitlo, 
					 read_rs_ddd_start_bithi - read_rs_ddd_start_bitlo + 1,			 
					 LSBfirst);
  //
  read_rs_ddd_busy_ = tmb_->bits_to_int(user1_value_ + read_rs_ddd_busy_bitlo, 
					read_rs_ddd_busy_bithi - read_rs_ddd_busy_bitlo + 1,			 
					LSBfirst);
  //
  read_rs_ddd_verify_ok_ = tmb_->bits_to_int(user1_value_ + read_rs_ddd_verify_ok_bitlo, 
					     read_rs_ddd_verify_ok_bithi - read_rs_ddd_verify_ok_bitlo + 1,
					     LSBfirst);
  //
  read_rs_rpc0_parity_ok_ = tmb_->bits_to_int(user1_value_ + read_rs_rpc0_parity_ok_bitlo, 
					      read_rs_rpc0_parity_ok_bithi - read_rs_rpc0_parity_ok_bitlo + 1,
					      LSBfirst);
  //
  read_rs_rpc1_parity_ok_ = tmb_->bits_to_int(user1_value_ + read_rs_rpc1_parity_ok_bitlo, 
					      read_rs_rpc1_parity_ok_bithi - read_rs_rpc1_parity_ok_bitlo + 1,
					      LSBfirst);
  //
  read_rs_rpc0_cnt_perr_ = tmb_->bits_to_int(user1_value_ + read_rs_rpc0_cnt_perr_bitlo,
					     read_rs_rpc0_cnt_perr_bithi - read_rs_rpc0_cnt_perr_bitlo + 1,
					     LSBfirst);
  //
  read_rs_rpc1_cnt_perr_ = tmb_->bits_to_int(user1_value_ + read_rs_rpc1_cnt_perr_bitlo,
					     read_rs_rpc1_cnt_perr_bithi - read_rs_rpc1_cnt_perr_bitlo + 1,
					     LSBfirst);
  //
  read_rs_last_opcode_ = tmb_->bits_to_int(user1_value_ + read_rs_last_opcode_bitlo,
					   read_rs_last_opcode_bithi - read_rs_last_opcode_bitlo + 1,
					   LSBfirst);
  //
  read_rw_rpc_en_ = tmb_->bits_to_int(user1_value_ + read_rw_rpc_en_bitlo,
				      read_rw_rpc_en_bithi - read_rw_rpc_en_bitlo + 1,			 
				      LSBfirst);
  //
  read_rw_ddd_start_ = tmb_->bits_to_int(user1_value_ + read_rw_ddd_start_bitlo, 
					 read_rw_ddd_start_bithi - read_rw_ddd_start_bitlo + 1,			 
					 LSBfirst);
  //
  read_rw_ddd_wr_ = tmb_->bits_to_int(user1_value_ + read_rw_ddd_wr_bitlo,
				      read_rw_ddd_wr_bithi - read_rw_ddd_wr_bitlo + 1,			 
				      LSBfirst);
  //
  read_rw_dddoe_wr_ = tmb_->bits_to_int(user1_value_ + read_rw_dddoe_wr_bitlo,
					read_rw_dddoe_wr_bithi - read_rw_dddoe_wr_bitlo + 1,			 
					LSBfirst);
  //
  read_rw_perr_reset_ = tmb_->bits_to_int(user1_value_ + read_rw_perr_reset_bitlo, 
					  read_rw_perr_reset_bithi - read_rw_perr_reset_bitlo + 1,			 
					  LSBfirst);
  //
  read_rw_parity_odd_ = tmb_->bits_to_int(user1_value_ + read_rw_parity_odd_bitlo, 
					  read_rw_parity_odd_bithi - read_rw_parity_odd_bitlo + 1,			 
					  LSBfirst);
  //
  read_rw_perr_ignore_ = tmb_->bits_to_int(user1_value_ + read_rw_perr_ignore_bitlo, 
					   read_rw_perr_ignore_bithi - read_rw_perr_ignore_bitlo + 1,			 
					   LSBfirst);
  //
  read_rw_rpc_future_ = tmb_->bits_to_int(user1_value_ + read_rw_rpc_future_bitlo,
					  read_rw_rpc_future_bithi - read_rw_rpc_future_bitlo + 1,			 
					  LSBfirst);
  //
  read_rs_rpc0_pdata_ = tmb_->bits_to_int(user1_value_ + read_rs_rpc0_pdata_bitlo,
					  read_rs_rpc0_pdata_bithi - read_rs_rpc0_pdata_bitlo + 1,			 
					  LSBfirst);
  //
  read_rs_rpc1_pdata_ = tmb_->bits_to_int(user1_value_ + read_rs_rpc1_pdata_bitlo,
					  read_rs_rpc1_pdata_bithi - read_rs_rpc1_pdata_bitlo + 1,			 
					  LSBfirst);
  //
  read_rs_unused_ = tmb_->bits_to_int(user1_value_ + read_rs_unused_bitlo,
				      read_rs_unused_bithi - read_rs_unused_bitlo + 1,			 
				      LSBfirst);
  //
  read_rs_end_ = tmb_->bits_to_int(user1_value_ + read_rs_end_bitlo,
				   read_rs_end_bithi - read_rs_end_bitlo + 1,			 
				   LSBfirst);
  return;
}
//
void RAT::PrintRatUser1() {
  //
  (*MyOutput_) << std::hex;
  (*MyOutput_) << "Begin marker = " << read_rs_begin_ << std::endl;
  (*MyOutput_) << "Version ID = " << read_rs_version_ << std::endl;
  (*MyOutput_) << "Version Month/Day = " << read_rs_monthday_ << std::endl;
  (*MyOutput_) << "Version Year = " << read_rs_year_ << std::endl;
  (*MyOutput_) << "1-> 80MHz sync mode = " << read_rs_syncmode_ << std::endl;
  (*MyOutput_) << "1-> Latch 40MHz RPC data on posedge = " << read_rs_posneg_ << std::endl;
  (*MyOutput_) << "1-> Loopback mode = " << read_rs_loop_ << std::endl;
  (*MyOutput_) << "RPC driver enabled = " << read_rs_rpc_en_ << std::endl;
  (*MyOutput_) << "RPC direct clock status = " << read_rs_clk_active_ << std::endl;
  (*MyOutput_) << "TMB DLL locked = " << read_rs_locked_tmb_ << std::endl;
  (*MyOutput_) << "RPC0 DLL locked = " << read_rs_locked_rpc0_ << std::endl;
  (*MyOutput_) << "RPC1 DLL locked = " << read_rs_locked_rpc1_ << std::endl;
  (*MyOutput_) << "TMB DLL lost lock = " << read_rs_locklost_tmb_ << std::endl;
  (*MyOutput_) << "RPC0 DLL lost lock = " << read_rs_locklost_rpc0_ << std::endl;
  (*MyOutput_) << "RPC1 DLL lost lock = " << read_rs_locklost_rpc1_ << std::endl;
  (*MyOutput_) << "ALCT TX OK = " << read_rs_txok_ << std::endl;
  (*MyOutput_) << "ALCT RX OK = " << read_rs_rxok_ << std::endl;
  (*MyOutput_) << "Over Temperature Threshold = " << read_rs_ntcrit_ << std::endl;
  (*MyOutput_) << "rpc_free0 from TMB = " << read_rs_rpc_free_ << std::endl;
  (*MyOutput_) << "rpc_dsn to TMB = " << read_rs_dsn_ << std::endl;
  (*MyOutput_) << "DDD status:  output enables = " << read_rs_dddoe_wr_ << std::endl;
  (*MyOutput_) << "DDD status:  delay values = " << read_rs_ddd_wr_ << std::endl;
  (*MyOutput_) << "1-> start DDD on power-up = " << read_rs_ddd_auto_ << std::endl;
  (*MyOutput_) << "DDD status: start ddd machine  = " << read_rs_ddd_start_ << std::endl;
  (*MyOutput_) << "DDD status: state machine busy = " << read_rs_ddd_busy_ << std::endl;
  (*MyOutput_) << "DDD status: data readback OK = " << read_rs_ddd_verify_ok_ << std::endl;
  (*MyOutput_) << "RPC0 parity OK currently = " << read_rs_rpc0_parity_ok_ << std::endl;
  (*MyOutput_) << "RPC1 parity OK currently = " << read_rs_rpc1_parity_ok_ << std::endl;
  (*MyOutput_) << "RPC0 parity error counter = " << read_rs_rpc0_cnt_perr_ << std::endl;
  (*MyOutput_) << "RPC1 parity error counter = " << read_rs_rpc1_cnt_perr_ << std::endl;
  (*MyOutput_) << "Last firmware TAP cmd opcode = " << read_rs_last_opcode_ << std::endl;
  (*MyOutput_) << "read_rw_rpc_en = " << read_rw_rpc_en_ << std::endl;
  (*MyOutput_) << "read_rw_ddd_start  = " << read_rw_ddd_start_ << std::endl;
  (*MyOutput_) << "read_rw_ddd_wr = " << read_rw_ddd_wr_ << std::endl;
  (*MyOutput_) << "read_rw_dddoe_wr = " << read_rw_dddoe_wr_ << std::endl;
  (*MyOutput_) << "read_rw_perr_reset = " << read_rw_perr_reset_ << std::endl;
  (*MyOutput_) << "read_rw_parity_odd = " << read_rw_parity_odd_ << std::endl;
  (*MyOutput_) << "read_rw_perr_ignore = " << read_rw_perr_ignore_ << std::endl;
  (*MyOutput_) << "read_rw_rpc_future = " << read_rw_rpc_future_ << std::endl;
  (*MyOutput_) << "RPC0 data (includes 16 pad bits + 3bxn) = " << read_rs_rpc0_pdata_ << std::endl;
  (*MyOutput_) << "RPC1 data (includes 16 pad bits + 3bxn) = " << read_rs_rpc1_pdata_ << std::endl;
  (*MyOutput_) << "Unused bits = " << read_rs_unused_ << std::endl;
  (*MyOutput_) << "End marker = " << read_rs_end_ << std::endl;
  //
  return;
}
//
//
///////////////////////////////////////////////////////////////////////////////////
//  here is a selection of useful values to be gotten from the User1 register...
///////////////////////////////////////////////////////////////////////////////////
int RAT::GetRat3dBusy_() {
  //
  return read_rs_ddd_busy_;
}
//
int RAT::GetRat3dVerifyOk_() {
  //
  return read_rs_ddd_verify_ok_;
}
//
int RAT::GetRatRpcParityErrorCounter(int rpc) { 
  //
  if (rpc==0) {
    return read_rs_rpc0_cnt_perr_; 
  } else if (rpc==1) {
    return read_rs_rpc1_cnt_perr_; 
  } else {
    (*MyOutput_) << "GetRatRpcParityErrorCounter ERROR: RPC " << rpc << " does not exist" << std::endl;
    return -1;
  }
}
//
int RAT::GetRatRpcParityOk(int rpc) {
  //
  if (rpc==0) {
    return read_rs_rpc0_parity_ok_; 
  } else if (rpc==1) {
    return read_rs_rpc1_parity_ok_; 
  } else {
    (*MyOutput_) << "GetRatRpcParityOk ERROR: RPC " << rpc << " does not exist" << std::endl;
    return -1;
  }
}
//
int RAT::GetRatRpcPdata(int rpc) {
  //
  if (rpc==0) {
    return read_rs_rpc0_pdata_; 
  } else if (rpc==1) {
    return read_rs_rpc1_pdata_; 
  } else {
    (*MyOutput_) << "GetRatRpcPdata ERROR: RPC " << rpc << " does not exist" << std::endl;
    return -1;
  }
}
//
int RAT::GetRatTxOk() {
  //
  return read_rs_txok_;
}
//
int RAT::GetRatRxOk() {
  //
  return read_rs_rxok_;
}
//
int RAT::GetRatCriticalTempBit() {
  //
  return read_rs_ntcrit_;
}  
//
int RAT::GetRatParityOdd() {
  //
  return read_rw_parity_odd_;
}
//
//
////////////////////////////////////////
// User2 Register (control register):
////////////////////////////////////////
void RAT::WriteRatUser2_(){
  //
  (*MyOutput_) << "RAT: Write RAT USER2 JTAG (control register)" << std::endl;
  //
  FillRatUser2_();
  //
  tmb_->setup_jtag(ChainRat);
  //	       
  tmb_->ShfIR_ShfDR(ChipLocationRatFpga,
		    FPGAuser2,
		    RegSizeRatFpga_FPGAuser2,
		    user2_value_);
  //
  if (tmb_->GetCheckJtagWrite()) {
    // Compare the write values into the User2 register with a copy of the 
    // values which are embedded into the read-only User1 register
    ReadRatUser1();
    tmb_->CompareBitByBit(user2_value_,
			  user1_value_ + read_rw_rpc_en_bitlo,
			  RegSizeRatFpga_FPGAuser2);
  }
  //
  return;
}
//
//
void RAT::FillRatUser2_() {
  // ** fill the bit array for the USER2 data register
  //
  tmb_->int_to_bits(write_rw_rpc_en_,
		    write_rw_rpc_en_bithi - write_rw_rpc_en_bitlo + 1,
		    user2_value_ + write_rw_rpc_en_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_rw_ddd_start_,
		    write_rw_ddd_start_bithi - write_rw_ddd_start_bitlo + 1,
		    user2_value_ + write_rw_ddd_start_bitlo,
		    LSBfirst);
  //
  for (int rpc=0; rpc<4; rpc++)
    tmb_->int_to_bits(write_rpc_rat_delay_[rpc],
		      4,
		      user2_value_ + write_rw_ddd_wr_bitlo + 4*rpc,		
		      LSBfirst);
  //
  tmb_->int_to_bits(write_rw_dddoe_wr_,
		    write_rw_dddoe_wr_bithi - write_rw_dddoe_wr_bitlo + 1,
		    user2_value_ + write_rw_dddoe_wr_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_rw_perr_reset_,
		    write_rw_perr_reset_bithi - write_rw_perr_reset_bitlo + 1,
		    user2_value_ + write_rw_perr_reset_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_rw_parity_odd_,
		    write_rw_parity_odd_bithi - write_rw_parity_odd_bitlo + 1,
		    user2_value_ + write_rw_parity_odd_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_rw_perr_ignore_,
		    write_rw_perr_ignore_bithi - write_rw_perr_ignore_bitlo + 1,
		    user2_value_ + write_rw_perr_ignore_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_rw_rpc_future_,
		    write_rw_rpc_future_bithi - write_rw_rpc_future_bitlo + 1,
		    user2_value_ + write_rw_rpc_future_bitlo,
		    LSBfirst);
  //
  return;
}
//
void RAT::SetRatRpcEnable_(int enable) { 
  //
  write_rw_rpc_en_ = enable & 0x3;
  return; 
} 
//              
void RAT::SetRat3dStart_(int on_or_off) { 
  //
  write_rw_ddd_start_ = on_or_off & 0x1;
  return; 
}
//
void RAT::SetRat3dOutputEnable_(int enable) { 
  //
  write_rw_dddoe_wr_ = enable & 0xf; 
  return;
} 
//            
void RAT::SetRatParityErrorReset_(int on_or_off) { 
  //
  write_rw_perr_reset_ = on_or_off & 0x1;
  return; 
}    
//
void RAT::SetRatParityOdd_(int on_or_off) { 
  //
  write_rw_parity_odd_ = on_or_off & 0x1;
  return;
} 
//          
void RAT::SetRatParityErrorIgnore_(int on_or_off) { 
  //
  write_rw_perr_ignore_ = on_or_off & 0x1; 
  return;
}  
//
void RAT::SetRatRpcFuture_(int dummy) { 
  //
  write_rw_rpc_future_ = dummy & 0x3f; 
  return; 
}               
//
void RAT::SetPowerUpUser2Register_() {
  //
  SetRatRpcEnable_(3);
  SetRat3dStart_(OFF);
  //
  SetRpcRatDelay(0,3);
  SetRpcRatDelay(1,3);
  write_rpc_rat_delay_[2] = 0;    //since the set function has a requirement that the RPC exists,
  write_rpc_rat_delay_[3] = 0;    //set these last two values by hand...
  //
  SetRat3dOutputEnable_(3);             
  SetRatParityErrorReset_(OFF);
  SetRatParityOdd_(ON);
  SetRatParityErrorIgnore_(OFF);
  SetRatRpcFuture_(0);
  //
  return;
}
//
//
//---------------------------------
// RPC-RAT phase:
//---------------------------------
void RAT::WriteRpcRatDelay() {
  //
  // Unstart state machine at the same time as delay values are set:
  SetRat3dStart_(OFF);
  WriteRatUser2_();
  //
  // Check that the DDD state machine went idle:
  if (tmb_->GetCheckJtagWrite()) {
    while (GetRat3dBusy_() != 0) {
      ReadRatUser1();
      //
      if (GetRat3dBusy_() != 0) {
	(*MyOutput_) << "ERROR: State machine stuck busy" << std::endl;
	::sleep(1);
      }
    }
  }
  //
  // Start DDD state machine to push the delay values in:
  SetRat3dStart_(ON);
  WriteRatUser2_();
  //
  // Check that the DDD state machine went busy:
  if (tmb_->GetCheckJtagWrite()) {
    while (GetRat3dBusy_() != 1) {
      ReadRatUser1();
      //
      if (GetRat3dBusy_() != 1) {
	(*MyOutput_) << "ERROR: State machine did not go busy" << std::endl;
	::sleep(1);
      }
    }
  }
  //
  // Unstart DDD state machine:
  SetRat3dStart_(OFF);
  WriteRatUser2_();
  //
  // Check that the DDD state machine is not busy:
  if (tmb_->GetCheckJtagWrite()) {
    while (GetRat3dBusy_() != 0) {
      ReadRatUser1();
      //
      if (GetRat3dBusy_() != 0) {
	(*MyOutput_) << "ERROR: State machine is busy" << std::endl;
	::sleep(1);
      }
    }
    //
    // Check that the data made it OK:
    if (GetRat3dVerifyOk_() != 1) {
      (*MyOutput_) << "ERROR: DDD data not verified" << std::endl;
      ::sleep(1);
    }
  }
  //
  //  PrintRpcRatDelay();
  //
  return;
}
//
void RAT::PrintRpcRatDelay(){
  (*MyOutput_) << "RAT: Read RPC-RAT delays:" << std::endl;
  (*MyOutput_) << "RPC0 = " << std::hex << GetRpcRatDelay(0) << std::endl;
  (*MyOutput_) << "RPC1 = " << std::hex << GetRpcRatDelay(1) << std::endl;
  //
  return;
}
//
void RAT::SetRpcRatDelay(int rpc, int delay) { 
  //
  if ( rpc<0 || rpc>1 ) {
    (*MyOutput_) << "SetRpcRatDelay ERROR: RPC " << rpc << " does not exist" << std::endl;
    return;
  }
  //
  if (delay<0 || delay>12 ) {
    (*MyOutput_) << "SetRpcRatDelay ERROR:  only delay values 0-12 allowed" << std::endl;      
    return;
  }
  //
  write_rpc_rat_delay_[rpc] = delay & 0xf;
  return;
} 
//
int RAT::GetRpcRatDelay(int rpc) { 
  //
  if ( rpc<0 || rpc>1 ) {
    (*MyOutput_) << "SetRpcRatDelay ERROR: RPC " << rpc << " does not exist" << std::endl;
    return 999;
  }
  //
  return read_rpc_rat_delay_[rpc];
} 
//
//
//---------------------------------
// RAT-TMB phase:
//---------------------------------
void RAT::WriteRatTmbDelay() {
  //
  tmb_->tmb_clk_delays(write_rat_tmb_delay_,8);
  //
  return;
}
//
void RAT::ReadRatTmbDelay() {
  //
  read_rat_tmb_delay_ = tmb_->tmb_read_delays(8);
  //
  // PrintRatTmbDelay();
  //
  return;
}
//
void RAT::SetRatTmbDelay(int delay) {
  //
  if (delay < 0 || delay >15 ) {
    (*MyOutput_) << "only delay values 0-15 allowed" << std::endl;      
    return;
  }
  //
  write_rat_tmb_delay_ = delay;
  //
  return;
}
//
int RAT::GetRatTmbDelay() { 
  //
  return read_rat_tmb_delay_ ; 
}
//
void RAT::PrintRatTmbDelay() {
  //
    (*MyOutput_) << "RAT-TMB delay = " << std::hex 
		 << GetRatTmbDelay() << std::endl;      
    return;
}
//
//
////////////////////////////////////////////
// Other useful methods:
////////////////////////////////////////////
void RAT::reset_parity_error_counter() {
  (*MyOutput_) << "RAT: Reset RPC parity error counter" << std::endl;
  //
  SetRatParityErrorReset_(ON);              // assert parity reset
  WriteRatUser2_();
  //
  SetRatParityErrorReset_(OFF);             // de-assert parity reset
  WriteRatUser2_();
  //
  return;
}
//
int RAT::ReadRATtempPCB() {
  //
  int smb_adr = 0x18;   // gnd, gnd state RAT LM84 chip address
  int command = 0x00;   // "local" temperature read
  int temperature = tmb_->smb_io(smb_adr,command,2);
  //
  (*MyOutput_) << "RAT temperature (PCB)                 = " << std::dec << temperature
	       << " deg C " << std::endl;
  //
  return temperature;
}
//
int RAT::ReadRATtempHSink() {
  //
  int smb_adr = 0x18;   // gnd, gnd state RAT LM84 chip address
  int command = 0x01;   // "remote" temperature read
  int temperature = tmb_->smb_io(smb_adr,command,2);
  //
  (*MyOutput_) << "RAT temperature (Heat Sink)           = " << std::dec << temperature
	       << " deg C " << std::endl;
  //
  return temperature;
}
//
int RAT::ReadRATtCritPCB() {
  //
  int smb_adr = 0x18;   // gnd, gnd state RAT LM84 chip address  
  int command = 0x05;   // "local" temperature critical read
  int temperature = tmb_->smb_io(smb_adr,command,2);
  //
  (*MyOutput_) << "RAT Critical Temperature (PCB)        = " << std::dec << temperature
	       << " deg C " << std::endl;
  //
  return temperature;
}
//
int RAT::ReadRATtCritHSink() {
  //
  int smb_adr = 0x18;   // gnd, gnd state RAT LM84 chip address
  int command = 0x07;   // "remote" temperature critical read
  int temperature = tmb_->smb_io(smb_adr,command,2);
  //
  (*MyOutput_) << "RAT Critical Temperature (Heat Sink)  = " << std::dec << temperature
	       << " deg C " << std::endl;
  //
  return temperature;
}
//
// Methods used to program RAT prom: 
//
int RAT::SVFLoad(int * arg1, const char * arg2, int arg3) { 
  //
  return tmb_->SVFLoad(arg1,arg2,arg3); 
}
//
void RAT::SetExpectedRatFirmwareDay(int day) { 
  //
  tmb_->SetExpectedRatFirmwareDay(day); 
  return;
}
//
int RAT::GetExpectedRatFirmwareDay() { 
  //
  return tmb_->GetExpectedRatFirmwareDay(); 
  //
}
//
void RAT::SetExpectedRatFirmwareMonth(int month) { 
  //
  tmb_->SetExpectedRatFirmwareMonth(month); 
  return;
}
int RAT::GetExpectedRatFirmwareMonth() { 
  //
  return tmb_->GetExpectedRatFirmwareMonth(); 
  //
}
//
void RAT::SetExpectedRatFirmwareYear(int year) { 
  //
  tmb_->SetExpectedRatFirmwareYear(year); 
  return;
}
//
int RAT::GetExpectedRatFirmwareYear() { 
  //
  return tmb_->GetExpectedRatFirmwareYear(); 
  //
}

} // namespace emu::pc  
} // namespace emu  


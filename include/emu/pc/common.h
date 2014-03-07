#ifndef COMMON_H
#define COMMON_H
#include <cmath>
#include <cstdlib>
#include <cstring> 
#include <iostream>
#include <stdio.h>
using namespace std; 

//------------------------------------------------------------------------------
//  Common
//------------------------------------------------------------------------------

const int   mxframe =   8192;   // Max raw hits frame number, scope adds 512*160/16=5120 frames
const int   mxtbins =   32;     // Highest time bin, 0 implies 32
const int   mxly    =   6;      // # CSC Layers
const int   mxds    =   8;      // # DiStrips per CFEB
const int   mxdsabs =   40;     // # DiStrips per CSC
const int   mxcfeb  =   5;      // # CFEBs
const int   mxbitstream=200;    // Max # bits in a jtag cycle

// Common/decode_readout_common/
extern int             scp_tbins;
extern int             scp_playback;
extern int             fifo_tbins_mini;
extern int             first_event;
extern int             itriad[mxtbins][mxdsabs][mxly];
extern int             clct0_vme;
extern int             clct1_vme;
extern int             clctc_vme;
extern int             mpc0_frame0_vme;
extern int             mpc0_frame1_vme;
extern int             mpc1_frame0_vme;
extern int             mpc1_frame1_vme;
extern int             nonzero_triads;
extern int             adjcfeb_dist;
extern int             mpc_me1a_block;

extern int             expect_zero_alct;
extern int             expect_zero_clct;
extern int             expect_one_alct;
extern int             expect_one_clct;
extern int             expect_two_alct;
extern int             expect_two_clct;
extern int             expect_dupe_alct;
extern int             expect_dupe_clct;
extern int             vme_bx0_emu_en;
extern bool            first_scn;

//  Common/TMB_VME_addresses
const unsigned long tmb_global_slot         = 26;
const unsigned long tmb_brcst_slot          = 27;
const unsigned long tmb_boot_adr            = 0x070000;
//------------------------------------------------------------------------------
// For tmb2005 and bdtest_v5
const unsigned long vme_idreg_adr           = 0x000000; 
const unsigned long vme_status_adr          = 0x000008;
const unsigned long vme_adr0_adr            = 0x00000A;
const unsigned long vme_adr1_adr            = 0x00000C;
const unsigned long vme_loopbk_adr          = 0x00000E;

const unsigned long vme_usr_jtag_adr        = 0x000010;
const unsigned long vme_prom_adr            = 0x000012;

const unsigned long vme_dddsm_adr           = 0x000014;
const unsigned long vme_ddd0_adr            = 0x000016;
const unsigned long vme_ddd1_adr            = 0x000018;
const unsigned long vme_ddd2_adr            = 0x00001A;
const unsigned long vme_dddoe_adr           = 0x00001C;
const unsigned long vme_ratctrl_adr         = 0x00001E;

const unsigned long vme_step_adr            = 0x000020;
const unsigned long vme_led_adr             = 0x000022;
const unsigned long vme_adc_adr             = 0x000024;
const unsigned long vme_dsn_adr             = 0x000026;
//------------------------------------------------------------------------------
// For tmb2005 normal firmware
const unsigned long mod_cfg_adr             = 0x000028; 
const unsigned long ccb_cfg_adr             = 0x00002A;
const unsigned long ccb_trig_adr            = 0x00002C;
const unsigned long ccb_stat0_adr           = 0x00002E;
const unsigned long alct_cfg_adr            = 0x000030;
const unsigned long alct_inj_adr            = 0x000032;
const unsigned long alct0_inj_adr           = 0x000034;
const unsigned long alct1_inj_adr           = 0x000036;
const unsigned long alct_stat_adr           = 0x000038;
const unsigned long alct_alct0_adr          = 0x00003A;
const unsigned long alct_alct1_adr          = 0x00003C;
const unsigned long alct_fifo_adr           = 0x00003E;
const unsigned long dmb_mon_adr             = 0x000040;
const unsigned long cfeb_inj_adr            = 0x000042;
const unsigned long cfeb_inj_adr_adr        = 0x000044;
const unsigned long cfeb_inj_wdata_adr      = 0x000046;
const unsigned long cfeb_inj_rdata_adr      = 0x000048;
const unsigned long hcm001_adr              = 0x00004A;
const unsigned long hcm023_adr              = 0x00004C;
const unsigned long hcm045_adr              = 0x00004E;
const unsigned long hcm101_adr              = 0x000050;
const unsigned long hcm123_adr              = 0x000052;
const unsigned long hcm145_adr              = 0x000054;
const unsigned long hcm201_adr              = 0x000056;
const unsigned long hcm223_adr              = 0x000058;
const unsigned long hcm245_adr              = 0x00005A;
const unsigned long hcm301_adr              = 0x00005C;
const unsigned long hcm323_adr              = 0x00005E;
const unsigned long hcm345_adr              = 0x000060;
const unsigned long hcm401_adr              = 0x000062;
const unsigned long hcm423_adr              = 0x000064;
const unsigned long hcm445_adr              = 0x000066;
const unsigned long seq_trig_en_adr         = 0x000068;
const unsigned long seq_trig_dly0_adr       = 0x00006A;
const unsigned long seq_trig_dly1_adr       = 0x00006C;
const unsigned long seq_id_adr              = 0x00006E;
const unsigned long seq_clct_adr            = 0x000070;
const unsigned long seq_fifo_adr            = 0x000072;
const unsigned long seq_l1a_adr             = 0x000074;
const unsigned long seq_offset0_adr         = 0x000076;
const unsigned long seq_clct0_adr           = 0x000078;
const unsigned long seq_clct1_adr           = 0x00007A;
const unsigned long seq_trig_src_adr        = 0x00007C;
const unsigned long dmb_ram_adr             = 0x00007E;
const unsigned long dmb_wdata_adr           = 0x000080;
const unsigned long dmb_wdcnt_adr           = 0x000082;
const unsigned long dmb_rdata_adr           = 0x000084;
const unsigned long tmb_trig_adr            = 0x000086;
const unsigned long mpc0_frame0_adr         = 0x000088;
const unsigned long mpc0_frame1_adr         = 0x00008A;
const unsigned long mpc1_frame0_adr         = 0x00008C;
const unsigned long mpc1_frame1_adr         = 0x00008E;
const unsigned long mpc_inj_adr             = 0x000090;
const unsigned long mpc_ram_adr             = 0x000092;
const unsigned long mpc_ram_wdata_adr       = 0x000094;
const unsigned long mpc_ram_rdata_adr       = 0x000096;
const unsigned long scp_ctrl_adr            = 0x000098;
const unsigned long scp_rdata_adr           = 0x00009A;
const unsigned long ccb_cmd_adr             = 0x00009C;

const unsigned long buf_stat0_adr           = 0x00009E;
const unsigned long buf_stat1_adr           = 0x0000A0;
const unsigned long buf_stat2_adr           = 0x0000A2;
const unsigned long buf_stat3_adr           = 0x0000A4;
const unsigned long buf_stat4_adr           = 0x0000A6;
const unsigned long alctfifo1_adr           = 0x0000A8;
const unsigned long alctfifo2_adr           = 0x0000AA;
const unsigned long seqmod_adr              = 0x0000AC;
const unsigned long seqsm_adr               = 0x0000AE;
const unsigned long seq_clctm_adr           = 0x0000B0;
const unsigned long tmbtim_adr              = 0x0000B2;
const unsigned long lhc_cycle_adr           = 0x0000B4;
const unsigned long rpc_cfg_adr             = 0x0000B6;
const unsigned long rpc_rdata_adr           = 0x0000B8;
const unsigned long rpc_raw_delay_adr       = 0x0000BA;
const unsigned long rpc_inj_adr             = 0x0000BC;
const unsigned long rpc_inj_adr_adr         = 0x0000BE;
const unsigned long rpc_inj_wdata_adr       = 0x0000C0;
const unsigned long rpc_inj_rdata_adr       = 0x0000C2;
const unsigned long rpc_tbins_adr           = 0x0000C4;
const unsigned long rpc_rpc0_hcm_adr        = 0x0000C6;
const unsigned long rpc_rpc1_hcm_adr        = 0x0000C8;
const unsigned long bx0_delay_adr           = 0x0000CA;
const unsigned long non_trig_adr            = 0x0000CC;
const unsigned long scp_trig_adr            = 0x0000CE;
const unsigned long cnt_ctrl_adr            = 0x0000D0;
const unsigned long cnt_rdata_adr           = 0x0000D2;

const unsigned long jtagsm0_adr             = 0x0000D4;
const unsigned long jtagsm1_adr             = 0x0000D6;
const unsigned long jtagsm2_adr             = 0x0000D8;

const unsigned long vmesm0_adr              = 0x0000DA;
const unsigned long vmesm1_adr              = 0x0000DC;
const unsigned long vmesm2_adr              = 0x0000DE;
const unsigned long vmesm3_adr              = 0x0000E0;
const unsigned long vmesm4_adr              = 0x0000E2;

const unsigned long dddrsm_adr              = 0x0000E4;
const unsigned long dddr0_adr               = 0x0000E6;

const unsigned long uptimer_adr             = 0x0000E8;
const unsigned long bdstatus_adr            = 0x0000EA;

const unsigned long bxn_clct_adr            = 0x0000EC;
const unsigned long bxn_alct_adr            = 0x0000EE;

const unsigned long layer_trig_adr          = 0x0000F0;
const unsigned long ise_version_adr         = 0x0000F2;

const unsigned long temp0_adr               = 0x0000F4;
const unsigned long temp1_adr               = 0x0000F6;
const unsigned long temp2_adr               = 0x0000F8;

const unsigned long parity_adr              = 0x0000FA;
const unsigned long ccb_stat1_adr           = 0x0000FC;
const unsigned long bxn_l1a_adr             = 0x0000FE;
const unsigned long l1a_lookback_adr        = 0x000100;
const unsigned long seq_debug_adr           = 0x000102;

const unsigned long alct_sync_ctrl_adr      = 0x000104; // ALCT sync mode control
const unsigned long alct_sync_txdata_1st    = 0x000106; // ALCT sync mode transmit data, 1st in time
const unsigned long alct_sync_txdata_2nd    = 0x000108; // ALCT sync mode transmit data, 2nd in time

const unsigned long seq_offset1_adr         = 0x00010A;
const unsigned long miniscope_adr           = 0x00010C;

const unsigned long phaser0_adr             = 0x00010E;
const unsigned long phaser1_adr             = 0x000110;
const unsigned long phaser2_adr             = 0x000112;
const unsigned long phaser3_adr             = 0x000114;
const unsigned long phaser4_adr             = 0x000116;
const unsigned long phaser5_adr             = 0x000118;
const unsigned long phaser6_adr             = 0x00011A;

const unsigned long delay0_int_adr          = 0x00011C;
const unsigned long delay1_int_adr          = 0x00011E;

const unsigned long sync_err_ctrl_adr       = 0x000120; // Synchronization Error Control

const unsigned long cfeb_badbits_ctrl_adr   = 0x000122; // CFEB  Bad Bit Control/Status
const unsigned long cfeb_badbits_timer_adr  = 0x000124; // CFEB  Bad Bit Check Interval

const unsigned long cfeb0_badbits_ly01_adr  = 0x000126; // CFEB0 Bad Bit Array
const unsigned long cfeb0_badbits_ly23_adr  = 0x000128; // CFEB0 Bad Bit Array
const unsigned long cfeb0_badbits_ly45_adr  = 0x00012A; // CFEB0 Bad Bit Array

const unsigned long cfeb1_badbits_ly01_adr  = 0x00012C; // CFEB1 Bad Bit Array
const unsigned long cfeb1_badbits_ly23_adr  = 0x00012E; // CFEB1 Bad Bit Array
const unsigned long cfeb1_badbits_ly45_adr  = 0x000130; // CFEB1 Bad Bit Array

const unsigned long cfeb2_badbits_ly01_adr  = 0x000132; // CFEB2 Bad Bit Array
const unsigned long cfeb2_badbits_ly23_adr  = 0x000134; // CFEB2 Bad Bit Array
const unsigned long cfeb2_badbits_ly45_adr  = 0x000136; // CFEB2 Bad Bit Array

const unsigned long cfeb3_badbits_ly01_adr  = 0x000138; // CFEB3 Bad Bit Array
const unsigned long cfeb3_badbits_ly23_adr  = 0x00013A; // CFEB3 Bad Bit Array
const unsigned long cfeb3_badbits_ly45_adr  = 0x00013C; // CFEB3 Bad Bit Array

const unsigned long cfeb4_badbits_ly01_adr  = 0x00013E; // CFEB4 Bad Bit Array
const unsigned long cfeb4_badbits_ly23_adr  = 0x000140; // CFEB4 Bad Bit Array
const unsigned long cfeb4_badbits_ly45_adr  = 0x000142; // CFEB4 Bad Bit Array


const unsigned long adr_alct_startup_delay  = 0x000144; // ALCT startup delay milliseconds for Spartan-6
const unsigned long adr_alct_startup_status = 0x000146; // ALCT startup delay machine status

const unsigned long adr_virtex6_snap12_qpll = 0x000148; // Virtex-6 SNAP12 Serial interface + QPLL
const unsigned long adr_virtex6_gtx_rx_all  = 0x00014A; // Virtex-6 GTX  common control
const unsigned long adr_virtex6_gtx_rx0     = 0x00014C; // Virtex-6 GTX0 control and status
const unsigned long adr_virtex6_gtx_rx1     = 0x00014E; // Virtex-6 GTX1 control and status
const unsigned long adr_virtex6_gtx_rx2     = 0x000150; // Virtex-6 GTX2 control and status
const unsigned long adr_virtex6_gtx_rx3     = 0x000152; // Virtex-6 GTX3 control and status
const unsigned long adr_virtex6_gtx_rx4     = 0x000154; // Virtex-6 GTX4 control and status
const unsigned long adr_virtex6_gtx_rx5     = 0x000156; // Virtex-6 GTX5 control and status
const unsigned long adr_virtex6_gtx_rx6     = 0x000158; // Virtex-6 GTX6 control and status
const unsigned long adr_virtex6_sysmon      = 0x00015A; // Virtex-6 Sysmon ADC

const unsigned long last_vme_adr            = 0x00015C; // Last valid address instantiated
//------------------------------------------------------------------------------
// For bdtestv3
const unsigned long vme_gpio_adr            = 0x000028; 
const unsigned long vme_cfg_adr             = 0x00002A;

const unsigned long cfeb0a_adr              = 0x00002C; // Repeats 2C-48 for CFEBs1-4
const unsigned long cfeb0b_adr              = 0x00002E;
const unsigned long cfeb0c_adr              = 0x000030;
const unsigned long cfeb_offset_adr         = 0x000006;

const unsigned long alct_rxa_adr            = 0x00004A;
const unsigned long alct_rxb_adr            = 0x00004C;
const unsigned long alct_rxc_adr            = 0x00004E;
const unsigned long alct_rxd_adr            = 0x000050;

const unsigned long dmb_rxa_adr             = 0x000052;
const unsigned long dmb_rxb_adr             = 0x000054;
const unsigned long dmb_rxc_adr             = 0x000056;
const unsigned long dmb_rxd_adr             = 0x000058;

const unsigned long mpc_rxa_adr             = 0x00005A;
const unsigned long mpc_rxb_adr             = 0x00005C;

const unsigned long rpc_rxa_adr             = 0x00005E;
const unsigned long rpc_rxb_adr             = 0x000060;
const unsigned long rpc_rxc_adr             = 0x000062;
const unsigned long rpc_rxd_adr             = 0x000064;
const unsigned long rpc_rxe_adr             = 0x000066;
const unsigned long rpc_rxf_adr             = 0x000068;

const unsigned long ccb_rxa_adr             = 0x00006A;
const unsigned long ccb_rxb_adr             = 0x00006C;
const unsigned long ccb_rxc_adr             = 0x00006E;
const unsigned long ccb_rxd_adr             = 0x000070;

const unsigned long alct_txa_adr            = 0x000072;
const unsigned long alct_txb_adr            = 0x000074;

const unsigned long rpc_txa_adr             = 0x000076;
const unsigned long rpc_txb_adr             = 0x000078;

const unsigned long dmb_txa_adr             = 0x00007A;
const unsigned long dmb_txb_adr             = 0x00007C;
const unsigned long dmb_txc_adr             = 0x00007E;
const unsigned long dmb_txd_adr             = 0x000080;

const unsigned long mpc_txa_adr             = 0x000082;
const unsigned long mpc_txb_adr             = 0x000084;

const unsigned long ccb_txa_adr             = 0x000086;
const unsigned long ccb_txb_adr             = 0x000088;

const unsigned long heater_adr              = 0x00008A; // Last bdtestv3 address instantiated

//------------------------------------------------------------------------------
//  Common
//------------------------------------------------------------------------------

#define     logical(L)      ((L)?'T':'F')
#define       yesno(L)        ((L)?'y':'n')

extern string msg_string; 
extern int data_read; 
extern int data_expect; 

//------------------------------------------------------------------------------
// File scope declarations
//------------------------------------------------------------------------------

// VME calls
extern long            status;
extern unsigned long   boot_adr;
extern unsigned long   adr;
extern unsigned short  rd_data;
extern unsigned short  wr_data;

//------------------------------------------------------------------------------
// Event counters
//------------------------------------------------------------------------------
const int      mxcounter = 86;
extern int     cnt_lsb;
extern int     cnt_msb;
extern int     cnt_full;
extern int     cnt[mxcounter];
extern string  scnt[mxcounter];

//------------------------------------------------------------------------------
// The bitter end.. 
//------------------------------------------------------------------------------

#endif //COMMON_H

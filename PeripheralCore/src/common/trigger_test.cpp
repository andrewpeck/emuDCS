#ifndef TRIGGER_TEST_H
#define TRIGGER_TEST_H
#include "emu/pc/trigger_test.h"

#include <cmath>
#include <cstdlib>
#include <cstring> 
#include <iostream>
#include <stdio.h>

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
int             scp_tbins;
int             scp_playback;
int             fifo_tbins_mini;
int             first_event;
int             itriad[mxtbins][mxdsabs][mxly];
int             clct0_vme;
int             clct1_vme;
int             clctc_vme;
int             mpc0_frame0_vme;
int             mpc0_frame1_vme;
int             mpc1_frame0_vme;
int             mpc1_frame1_vme;
int             nonzero_triads;
int             adjcfeb_dist;
int             mpc_me1a_block;

int             expect_zero_alct;
int             expect_zero_clct;
int             expect_one_alct;
int             expect_one_clct;
int             expect_two_alct;
int             expect_two_clct;
int             expect_dupe_alct;
int             expect_dupe_clct;
int             vme_bx0_emu_en;
bool            first_scn;

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

std::string msg_string; 
int data_read; 
int data_expect; 


//------------------------------------------------------------------------------
// File scope declarations
//------------------------------------------------------------------------------

// VME calls
long            status;
unsigned long   boot_adr;
unsigned long   adr;
unsigned short  rd_data;
unsigned short  wr_data;

//------------------------------------------------------------------------------
// Event counters
//------------------------------------------------------------------------------
const int      mxcounter = 86;
int     cnt_lsb;
int     cnt_msb;
int     cnt_full;
int     cnt[mxcounter];
std::string  scnt[mxcounter];

// Includes from trigger test
//------------------------------------------------------------------------------
// Local
//------------------------------------------------------------------------------
int                	id_slot;

// CSC type
int                	csc_me1ab;
int                	stagger_hs_csc;
int                	reverse_hs_csc;
int                	reverse_hs_me1a;
int                	reverse_hs_me1b;
int                	csc_type;

//string           flags_ok;

// Menu
char           		line[80];
int                	ifunc;
int                	i,j,k,n;

// CCB
int                	ttc_cmd;
int                	ccb_cmd;

// Trigger tests
int                	dmb_thresh_pretrig;
int                	hit_thresh_pretrig;
int                	hit_thresh_postdrift;
int                	pid_thresh_pretrig;
int                	pid_thresh_postdrift;
int                	lyr_thresh_pretrig;

int                	triad_persist;
int                	drift_delay;
int                	clct_sep;
int                	active_feb_src;
int                	active_feb_list;

bool           layer_mode;
int                layer_trig_en;

int                fifo_mode;
int                fifo_tbins;
int                fifo_pretrig;

int                fifo_tbins_rpc;
int                fifo_pretrig_rpc;
int                rpc_decouple;

int                alct_bx0_en;
int                alct_delay;
int                clct_width;

int                tmb_allow_clct;
int                tmb_allow_alct;
int                tmb_allow_match;

int                tmb_allow_alct_ro;
int                tmb_allow_clct_ro;
int                tmb_allow_match_ro;

int                inj_delay_rat;
int                rpc_tbins_test;
int                rpc_exists;

bool           rrhd;
bool           cprr;
bool           cprr_ignore;
bool           rat_injector_sync;
bool           rat_injector_enable;
bool           rpcs_in_rdout;
bool           pause_on_fail;
int                vme_bx0_emu_en_default;

int                nclcts_inject;
int                nalcts_inject;
int                mcl;

const int      mxclcts=8;
int                clct_pat_inject[mxclcts];
int                clct_pid_inject[mxclcts];
int                clct_key_inject[mxclcts];
int                clct_hit_inject[mxclcts];
bool           clct_blanked[mxclcts];
int                clct_hit_inject_clip[mxclcts];
bool           loop_keys[mxclcts];
bool           loop_pids[mxclcts];
int                iclct;

int             triad_1st_tbin[6];
int             l1a_delay; // hits 0th l1a window bx
int             rat_sync_mode;
int             rat_injector_delay;

int             irpc;
int             ibxn;
int             rpc_pad;
int             rpc_inj_bxn;
int             rpc_inj_image[256][2];
int             rpc_inj_wen;
int             rpc_inj_ren;
int             rpc_inj_rwadr;
int             rpc_inj_data;

// Scope
bool                rdscope;
int                 scp_arm;
int                 scp_readout;
int                 scp_raw_decode;
int                 scp_silent;
int                 scp_raw_data[512*160/16];
int                 scp_auto;
int                 scp_nowrite;

// Misc
int             ipass;

// Pattern cells
int             icell;
int             ihit;
int             ihitp;

int             nhits;
int             layer;

int             ikey;
int             ikeylp;
int             ikey_min;
int             ikey_max;
int             ikey_sep;

int             ipid;
int             ipidlp;
int             ipid_min;
int             ipid_max;

int             icfeb;
int             icfebg;
int             key;
int             ihs[6][160];
int             nstag;

int             idistrip;
int             idslocal;
int             itbin;
int             itbin0;
int             ihstrip;
int             iram;
//int                    pat_ram[32][3][5]={0};
int             pat_ram[32][3][5];
int             wen;
int             ren;
int             wadr;
int             febsel;
int             icfeblp;
int             ibit;
int             wr_data_mem;
int             rd_data_mem;

std::string          marker;

//------------------------------------------------------------------------------
//  CLCT Bend Pattern Images, key layer 2
//------------------------------------------------------------------------------

const int pattern_image[11][6][11]={  // pid,layer,cell
    //  0 1 2 3 4 5 6 7 8 9 A
   {{0,0,0,0,0,0,0,0,0,0,0},      // ly0 Pattern 0, empty
    {0,0,0,0,0,0,0,0,0,0,0},      // ly1
    {0,0,0,0,0,0,0,0,0,0,0},      // ly2
    {0,0,0,0,0,0,0,0,0,0,0},      // ly3
    {0,0,0,0,0,0,0,0,0,0,0},      // ly4
    {0,0,0,0,0,0,0,0,0,0,0}},      // ly5

   {{0,0,0,0,0,0,0,0,0,0,0},      // ly0 Pattern 1, empty
    {0,0,0,0,0,0,0,0,0,0,0},      // ly1
    {0,0,0,0,0,0,0,0,0,0,0},      // ly2
    {0,0,0,0,0,0,0,0,0,0,0},      // ly3
    {0,0,0,0,0,0,0,0,0,0,0},      // ly4
    {0,0,0,0,0,0,0,0,0,0,0}},      // ly5

   {{0,0,0,0,0,0,0,0,2,2,1},      // ly0 Pattern 2, 0=no hit, 1=primary hit, 2=secondary hit
    {0,0,0,0,0,0,2,1,0,0,0},      // ly1
    {0,0,0,0,0,1,0,0,0,0,0},      // ly2
    {0,0,0,1,2,2,0,0,0,0,0},      // ly3
    {0,1,2,2,0,0,0,0,0,0,0},      // ly4
    {1,2,2,0,0,0,0,0,0,0,0}},      // ly5

   {{1,2,2,0,0,0,0,0,0,0,0},      // ly0 Pattern 3
    {0,0,0,1,2,0,0,0,0,0,0},      // ly1
    {0,0,0,0,0,1,0,0,0,0,0},      // ly2
    {0,0,0,0,0,2,2,1,0,0,0},      // ly3
    {0,0,0,0,0,0,0,2,2,1,0},      // ly4
    {0,0,0,0,0,0,0,0,2,2,1}},      // ly5

   {{0,0,0,0,0,0,0,2,2,1,0},      // ly0 Pattern 4
    {0,0,0,0,0,0,2,1,0,0,0},      // ly1
    {0,0,0,0,0,1,0,0,0,0,0},      // ly2
    {0,0,0,1,2,0,0,0,0,0,0},      // ly3
    {0,1,2,2,0,0,0,0,0,0,0},      // ly4
    {0,1,2,2,0,0,0,0,0,0,0}},      // ly5

   {{0,1,2,2,0,0,0,0,0,0,0},      // ly0 Pattern 5
    {0,0,0,1,2,0,0,0,0,0,0},      // ly1
    {0,0,0,0,0,1,0,0,0,0,0},      // ly2
    {0,0,0,0,0,0,2,1,0,0,0},      // ly3
    {0,0,0,0,0,0,0,2,2,1,0},      // ly4
    {0,0,0,0,0,0,0,2,2,1,0}},      // ly5

   {{0,0,0,0,0,0,2,2,1,0,0},      // ly0 Pattern 6
    {0,0,0,0,0,2,1,0,0,0,0},      // ly1
    {0,0,0,0,0,1,0,0,0,0,0},      // ly2
    {0,0,0,0,1,2,0,0,0,0,0},      // ly3
    {0,0,0,1,2,0,0,0,0,0,0},      // ly4
    {0,0,1,2,2,0,0,0,0,0,0}},      // ly5

   {{0,0,1,2,2,0,0,0,0,0,0},      // ly0 Pattern 7
    {0,0,0,0,1,2,0,0,0,0,0},      // ly1
    {0,0,0,0,0,1,0,0,0,0,0},      // ly2
    {0,0,0,0,0,2,1,0,0,0,0},      // ly3
    {0,0,0,0,0,0,2,1,0,0,0},      // ly4
    {0,0,0,0,0,0,2,2,1,0,0}},      // ly5

   {{0,0,0,0,0,2,2,1,0,0,0},      // ly0 Pattern 8
    {0,0,0,0,0,2,1,0,0,0,0},      // ly1
    {0,0,0,0,0,1,0,0,0,0,0},      // ly2
    {0,0,0,0,1,2,0,0,0,0,0},      // ly3
    {0,0,0,1,2,2,0,0,0,0,0},      // ly4
    {0,0,0,1,2,2,0,0,0,0,0}},      // ly5

   {{0,0,0,1,2,2,0,0,0,0,0},      // ly0 Pattern 9
    {0,0,0,0,1,2,0,0,0,0,0},      // ly1
    {0,0,0,0,0,1,0,0,0,0,0},      // ly2
    {0,0,0,0,0,2,1,0,0,0,0},      // ly3
    {0,0,0,0,0,2,2,1,0,0,0},      // ly4
    {0,0,0,0,0,2,2,1,0,0,0}},      // ly5

   {{0,0,0,0,2,1,2,0,0,0,0},      // ly0 Pattern A
    {0,0,0,0,0,1,0,0,0,0,0},      // ly1
    {0,0,0,0,0,1,0,0,0,0,0},      // ly2
    {0,0,0,0,0,1,0,0,0,0,0},      // ly3
    {0,0,0,0,2,1,2,0,0,0,0},      // ly4
    {0,0,0,0,2,1,2,0,0,0,0}}       // ly5
};

//------------------------------------------------------------------------------
int             	alct_injector_delay; // experimental

int                 alct0_rd;
int                 alct1_rd;
int                 alct0_prev;
int                 alct1_prev;

// crc checks
long int            din;
long int            crc;
long int            tmb_crc_lsb;
long int            tmb_crc_msb;
long int            tmb_crc;
bool                crc_match;

int                 dmb_wdcnt;
int                 dmb_busy;
int                 dmb_rdata;
int                 dmb_rdata_lsb;
int                 dmb_rdata_msb;

int                 nlayers_hit;
int                 scint_veto;

int                 alct0_inj_rd;
int                 alct1_inj_rd;
int                 alct0_inj_wr;
int                 alct1_inj_wr;

int                 alct0_vpf_inj;
int                 alct0_qual_inj;
int                 alct0_amu_inj;
int                 alct0_key_inj;
int                 alct0_bxn_inj;

int                 alct1_vpf_inj;
int                 alct1_qual_inj;
int                 alct1_amu_inj;
int                 alct1_key_inj;
int                 alct1_bxn_inj;

int                 alct0_vpf;
int                 alct0_qual;
int                 alct0_amu;
int                 alct0_key;
int                 alct0_bxn;

int                 alct1_vpf;
int                 alct1_qual;
int                 alct1_amu;
int                 alct1_key;
int                 alct1_bxn;

int                 vf_data[mxframe];
int                 iframe;
int                 ilayer;

int                 adr_e0b;
int                 r_nheaders;
int                 r_ncfebs;
int                 r_fifo_tbins;

int                 read_pat[mxtbins][mxly][mxdsabs];
int                 rdcid;
int                 rdtbin;
int                 hits8;
int                 hits1;
int                 ids;
int                 ids_abs;
int                 jcfeb;
const char                x[]="          ";
bool                display_cfeb;
int                 clct_bxn_expect;

int                 clct_key_expect[mxclcts];
int                 clct_pid_expect[mxclcts];
int                 clct_hit_expect[mxclcts];
int                 hit_thresh_pretrig_temp;
int                 hit_thresh_postdrift_temp;

int             fmm_state;
std::string          sfmm_state[5];

bool            rdraw;
int             nbxn0;
int             ntrig;

int             alct_raw_busy;
int             alct_raw_done;
int             alct_raw_nwords;
int             alct_raw_data;

int             alct0_raw_lsb;
int             alct0_raw_msb;
int             alct0_raw;

int             alct1_raw_lsb;
int             alct1_raw_msb;
int             alct1_raw;

int             seqdeb_adr;
int             seqdeb_rd_mux;
unsigned long   deb_adr_diff;
unsigned long   deb_wr_buf_adr;
unsigned long   deb_buf_push_adr;
unsigned long   deb_buf_pop_adr;
unsigned long   deb_buf_push_data;
unsigned long   deb_buf_pop_data;

int             radix;
FILE*               ram_file;
std::string              ram_file_name;
int             iadr;
int             blue_flash;
bool            fire_injector;
bool            err_check;

unsigned short  wr_fire_l1a;

int                 l1a_lookback;
int             hdr_wr_continuous;
int             lookback_triad_hits[2048];

int             first_nonzero_bx;
int             last_nonzero_bx;
int             first_bx;
int             last_bx;
int             max_triads;
double          scale;
int             tck;

// Miniscope
int             	mini_read_enable;
int             	mini_test;
int             	mini_tbins_word;
int             	fifo_pretrig_mini;
int             	csc_id;
int                 maxv;
int                 minv;
char                symbol;
int           		bcb_read_enable;

// Pattern_finder
int                 cfeb_en[5];
int                 hs_key_1st_emu;
int                 hs_pid_1st_emu;
int                 hs_hit_1st_emu;

int                 hs_key_2nd_emu;
int                 hs_pid_2nd_emu;
int                 hs_hit_2nd_emu;

int                 nlayers_hit_emu;
int                 layer_trig_emu;
int                 cfeb_active_emu[5];

bool                emulator_pretrig;
bool                emulator_latch_clct0;
bool                emulator_latch_clct1;
bool                send_emulator_to_tmb;

int                 clct0_vpf_emu;
int                 clct0_hit_emu;
int                 clct0_pid_emu;
int                 clct0_key_emu;
int                 clct0_cfeb_emu;
int                 clctc_sync_emu;

int                 clct1_vpf_emu;
int                 clct1_hit_emu;
int                 clct1_pid_emu;
int                 clct1_key_emu;
int                 clct1_cfeb_emu;

int                 clct0_vpf_vme;
int                 clct0_hit_vme;
int                 clct0_pid_vme;
int                 clct0_key_vme;
int                 clct0_cfeb_vme;

int                 clct1_vpf_vme;
int                 clct1_hit_vme;
int                 clct1_pid_vme;
int                 clct1_key_vme;
int                 clct1_cfeb_vme;

int                 clctc_bxn_vme;
int                 clctc_sync_vme;

int                 nspan;
int                 pspan;
int                 busy_min;
int                 busy_max;
bool                clct0_is_on_me1a;
int                 key_inj;
int                 busy_key_inj[160];

int                 clct_hit_inj_expect[2];
int                 clct_key_inj_expect[2];
int                 clct_pid_inj_expect[2];
int                 clct_pat_inj_expect[2];

bool                injector_clct0_over;
bool                injector_clct1_over;
bool                injector_pretrig;
bool                injector_latch_clct0;
bool                injector_latch_clct1;

int                 clct0_vpf_vme_expect;
int                 clct0_hit_vme_expect;
int                 clct0_pid_vme_expect;
int                 clct0_key_vme_expect;
int                 clct0_cfeb_vme_expect;
int                 clctc_sync_vme_expect;

int                 clct1_vpf_vme_expect;
int                 clct1_hit_vme_expect;
int                 clct1_pid_vme_expect;
int                 clct1_key_vme_expect;
int                 clct1_cfeb_vme_expect;

int                 clct0_vpf_tmb;
int                 clct0_hit_tmb;
int                 clct0_pid_tmb;
int                 clct0_key_tmb;
int                 clct0_cfeb_tmb;

int                 clct1_vpf_tmb;
int                 clct1_hit_tmb;
int                 clct1_pid_tmb;
int                 clct1_key_tmb;
int                 clct1_cfeb_tmb;

int                 clctc_bxn_tmb;
int                 clctc_sync_err_tmb;

int                 key0;
int                 key1;

bool            kill_me1a_clcts;
bool            clct0_exists;
bool            clct1_exists;
bool            clct0_cfeb4;
bool            clct1_cfeb4;
bool            kill_clct0;
bool            kill_clct1;
bool            kill_trig;
bool            bans;

bool            clct_noalct_lost;
bool            alct_only_trig;

bool            clct_keep;
bool            alct_keep;

bool            clct_keep_ro;
bool            alct_keep_ro;

bool            clct_discard;
bool            alct_discard;

bool            clct_match;
bool            clct_noalct;
bool            alct_noclct;
bool            alct_pulse;

bool            trig_pulse;
bool            trig_keep;
bool            non_trig_keep;
bool            alct_only;

bool            clct_match_tr;
bool            alct_noclct_tr;
bool            clct_noalct_tr;
bool            clct_match_ro;
bool            alct_noclct_ro;
bool            clct_noalct_ro;

bool            tmb_trig_pulse;
bool            tmb_trig_keep_ff;
bool            tmb_non_trig_keep_ff;

bool            tmb_match;
bool            tmb_alct_only;
bool            tmb_clct_only;

bool            tmb_match_ro_ff;
bool            tmb_alct_only_ro_ff;
bool            tmb_clct_only_ro_ff;

bool            tmb_alct_discard;
bool            tmb_clct_discard;

int                 tmb_alct0;
int                 tmb_alct1;
int                 alct0_pipe;
int                 alct1_pipe;
int                 alct0_real;
int                 alct1_real;

int                 keep_clct;
int                 clct0_real;
int                 clct0_pipe;
int                 clct1_real;
int                 clct1_pipe;
int                 clctc_real;
int                 clctc_pipe;

bool            tmb_match_ro;
bool            tmb_alct_only_ro;
bool            tmb_clct_only_ro;

bool            tmb_trig_keep;
bool            tmb_non_trig_keep;

int             tmb_no_alct_emu;
int             tmb_no_clct_emu;
int             tmb_one_alct_emu;
int             tmb_one_clct_emu;
int             tmb_two_alct_emu;
int             tmb_two_clct_emu;
int             tmb_dupe_alct_emu;
int             tmb_dupe_clct_emu;

int             alct_dummy;
int             clct_dummy;
int             clctc_dummy;
int             clct_bxn_insert_emu;

int             clct0_emu;
int             clct1_emu;
int             clctc_emu;

int             alct0_emu;
int             alct1_emu;

int             lct0_vpf_emu;
int             lct1_vpf_emu;

int             alct0_vpf_emu;
int             alct0_valid_emu;
int             alct0_quality_emu;
int             alct0_amu_emu;
int             alct0_key_emu;
int             alct0_bxn_emu;

int             alct1_vpf_emu;
int             alct1_valid_emu;
int             alct1_quality_emu;
int             alct1_amu_emu;
int             alct1_key_emu;
int             alct1_bxn_emu;

int             clct0_valid_emu;
int             clct0_bend_emu;

int             clct1_valid_emu;
int             clct1_bend_emu;

int             clct_bxn_emu;
int             clct_sync_err_emu;

int             alct0_hit_emu;
int             alct1_hit_emu;

int             clct0_cpat_emu;
int             clct1_cpat_emu;

int             ACC;
int             A;
int             C;
int             A4;
int             C4;
int             P;
int             CPAT;
int             Q;

int             lct0_quality_emu;
int             lct1_quality_emu;
int             tmb_rank_err_emu;

int             alct_bx0_emu;
int             clct_bx0_emu;

int             csc_id_emu;

int             trig_mpc_emu;
int             trig_mpc0_emu;
int             trig_mpc1_emu;

int             mpc0_frame0_emu;
int             mpc0_frame1_emu;
int             mpc1_frame0_emu;
int             mpc1_frame1_emu;

int             mpc0_frame0_pulse;
int             mpc0_frame1_pulse;
int             mpc1_frame0_pulse;
int             mpc1_frame1_pulse;

int             mpc_alct0_key_expect;
int             mpc_clct0_pid_expect;
int             mpc_lct0_qual_expect;
int             mpc_lct0_vpf_expect;

int             mpc_clct0_key_expect;
int             mpc_clct0_bend_expect;
int             mpc_sync_err0_expect;
int             mpc_alct0_bxn_expect;
int             mpc_bx0_clct_expect;
int             mpc_csc_id0_expect;

int             mpc_alct1_key_expect;
int             mpc_clct1_pid_expect;
int             mpc_lct1_qual_expect;
int             mpc_lct1_vpf_expect;

int             mpc_clct1_key_expect;
int             mpc_clct1_bend_expect;
int             mpc_sync_err1_expect;
int             mpc_alct1_bxn_expect;
int             mpc_bx0_alct_expect;
int             mpc_csc_id1_expect;

int             mpc_alct0_key_vme;
int             mpc_clct0_pid_vme;
int             mpc_lct0_qual_vme;
int             mpc_lct0_vpf_vme;

int             mpc_bx0_clct_vme;
int             mpc_clct0_key_vme;
int             mpc_sync_err0_vme;
int             mpc_csc_id0_vme;
int             mpc_clct0_bend_vme;
int             mpc_alct0_bxn_vme;

int             mpc_alct1_key_vme;
int             mpc_clct1_pid_vme;
int             mpc_lct1_qual_vme;
int             mpc_lct1_vpf_vme;

int                 mpc_clct1_key_vme;
int                 mpc_clct1_bend_vme;
int                 mpc_sync_err1_vme;
int                 mpc_alct1_bxn_vme;
int                 mpc_bx0_alct_vme;
int                 mpc_csc_id1_vme;

int             lct0_vme;
int             lct1_vme;

int                 ihs_min;
int                 ihs_max;
int                 ihs_hit;
int                 ihs_ds;

int                 sync_err; 
int                 clct_bx0_sync_err; 
int                 alct_ecc_rx_err_ff; 
int                 alct_ecc_tx_err_ff; 
int                 bx0_match_err_ff; 
int                 sync_err_force; 

bool                load_clct_injector_image;
int                 dscfeb[mxcfeb];

int                 mask_all;
int                 mask_cfeb;
int                 inj_trig;
int                 inj_busy;
bool                engage_pattern_finder;

int                 wr_data_lsb;
int                 wr_data_msb;
int                 rd_data_lsb;
int                 rd_data_msb;
int                 l1a_lookback_data;

//------------------------------------------------------------------------------
//  Prototypes
//------------------------------------------------------------------------------

void TriggerTestInjectALCT();
void TriggerTestInjectCLCT();
void TriggerTestInjectALCTCLCT();
int  TriggerTestInjectALCTCLCT_Readout();
int  TriggerTestFire_L1A_to_ALCT();
void TriggerTestFire_CLCT_ext_trig_with_ALCT ();
void TriggerTestInject_walking_CLCT();
void TriggerTestExternalTriggerALCTCLCT();
void TriggerTestExternalTriggerALCT();
void TriggerTestExternalTriggerCLCT();
void TriggerTestExternalTriggerALCTCLCTwithGTLpulser();
void TriggerTestExternalALCTCLCTwithGTLpulserCheckCRC();
void TriggerTestTestBXNCounter();
int  TriggerTestFireL1A();
int  TriggerTestForceCLCTtriggerandReadout();

#endif

#ifndef TRIGGER_TEST_H
#define TRIGGER_TEST_H

#include <cmath>
#include <cstdlib>
#include <cstring> 
#include <iostream>
#include <stdio.h>
using namespace std; 

//------------------------------------------------------------------------------
//  Common
//------------------------------------------------------------------------------

const int   mxframe     =   8192;   // Max raw hits frame number, scope adds 512*160/16=5120 frames
const int   mxtbins     =   32;     // Highest time bin, 0 implies 32
const int   mxly        =   6;      // # CSC Layers
const int   mxds        =   8;      // # DiStrips per CFEB
const int   mxdsabs     =   40;     // # DiStrips per CSC
const int   mxcfeb      =   5;      // # CFEBs
const int   mxbitstream =   200;    // Max # bits in a jtag cycle

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
// Local
//------------------------------------------------------------------------------
extern int                	id_slot;

// CSC type
extern int                	csc_me1ab;
extern int                	stagger_hs_csc;
extern int                	reverse_hs_csc;
extern int                	reverse_hs_me1a;
extern int                	reverse_hs_me1b;
extern int                	csc_type;

//string           flags_ok;

// Menu
extern char           		line[80];
extern int                	ifunc;
extern int                	i,j,k,n;

// CCB
extern int                	ttc_cmd;
extern int                	ccb_cmd;

// Trigger tests
extern int                	dmb_thresh_pretrig;
extern int                	hit_thresh_pretrig;
extern int                	hit_thresh_postdrift;
extern int                	pid_thresh_pretrig;
extern int                	pid_thresh_postdrift;
extern int                	lyr_thresh_pretrig;

extern int                	triad_persist;
extern int                	drift_delay;
extern int                	clct_sep;
extern int                	active_feb_src;
extern int                	active_feb_list;

extern bool           layer_mode;
extern int                layer_trig_en;

extern int                fifo_mode;
extern int                fifo_tbins;
extern int                fifo_pretrig;

extern int                fifo_tbins_rpc;
extern int                fifo_pretrig_rpc;
extern int                rpc_decouple;

extern int                alct_bx0_en;
extern int                alct_delay;
extern int                clct_width;

extern int                tmb_allow_clct;
extern int                tmb_allow_alct;
extern int                tmb_allow_match;

extern int                tmb_allow_alct_ro;
extern int                tmb_allow_clct_ro;
extern int                tmb_allow_match_ro;

extern int                inj_delay_rat;
extern int                rpc_tbins_test;
extern int                rpc_exists;

extern bool           rrhd;
extern bool           cprr;
extern bool           cprr_ignore;
extern bool           rat_injector_sync;
extern bool           rat_injector_enable;
extern bool           rpcs_in_rdout;
extern bool           pause_on_fail;
extern int                vme_bx0_emu_en_default;

extern int                nclcts_inject;
extern int                nalcts_inject;
extern int                mcl;

const int      mxclcts=8;
extern int                clct_pat_inject[mxclcts];
extern int                clct_pid_inject[mxclcts];
extern int                clct_key_inject[mxclcts];
extern int                clct_hit_inject[mxclcts];
extern bool           clct_blanked[mxclcts];
extern int                clct_hit_inject_clip[mxclcts];
extern bool           loop_keys[mxclcts];
extern bool           loop_pids[mxclcts];
extern int                iclct;

extern int             triad_1st_tbin[6];
extern int             l1a_delay; // hits 0th l1a window bx
extern int             rat_sync_mode;
extern int             rat_injector_delay;

extern int             irpc;
extern int             ibxn;
extern int             rpc_pad;
extern int             rpc_inj_bxn;
extern int             rpc_inj_image[256][2];
extern int             rpc_inj_wen;
extern int             rpc_inj_ren;
extern int             rpc_inj_rwadr;
extern int             rpc_inj_data;

// Scope
extern bool                rdscope;
extern int                 scp_arm;
extern int                 scp_readout;
extern int                 scp_raw_decode;
extern int                 scp_silent;
extern int                 scp_raw_data[512*160/16];
extern int                 scp_auto;
extern int                 scp_nowrite;

// Misc
extern int             ipass;

// Pattern cells
extern int             icell;
extern int             ihit;
extern int             ihitp;

extern int             nhits;
extern int             layer;

extern int             ikey;
extern int             ikeylp;
extern int             ikey_min;
extern int             ikey_max;
extern int             ikey_sep;

extern int             ipid;
extern int             ipidlp;
extern int             ipid_min;
extern int             ipid_max;

extern int             icfeb;
extern int             icfebg;
extern int             key;
extern int             ihs[6][160];
extern int             nstag;

extern int             idistrip;
extern int             idslocal;
extern int             itbin;
extern int             itbin0;
extern int             ihstrip;
extern int             iram;
//int                    pat_ram[32][3][5]={0};
extern int             pat_ram[32][3][5];
extern int             wen;
extern int             ren;
extern int             wadr;
extern int             febsel;
extern int             icfeblp;
extern int             ibit;
extern int             wr_data_mem;
extern int             rd_data_mem;

extern string          marker;

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
extern int             	alct_injector_delay; // experimental

extern int                 alct0_rd;
extern int                 alct1_rd;
extern int                 alct0_prev;
extern int                 alct1_prev;

// crc checks
extern long int            din;
extern long int            crc;
extern long int            tmb_crc_lsb;
extern long int            tmb_crc_msb;
extern long int            tmb_crc;
extern bool                crc_match;

extern int                 dmb_wdcnt;
extern int                 dmb_busy;
extern int                 dmb_rdata;
extern int                 dmb_rdata_lsb;
extern int                 dmb_rdata_msb;

extern int                 nlayers_hit;
extern int                 scint_veto;

extern int                 alct0_inj_rd;
extern int                 alct1_inj_rd;
extern int                 alct0_inj_wr;
extern int                 alct1_inj_wr;

extern int                 alct0_vpf_inj;
extern int                 alct0_qual_inj;
extern int                 alct0_amu_inj;
extern int                 alct0_key_inj;
extern int                 alct0_bxn_inj;

extern int                 alct1_vpf_inj;
extern int                 alct1_qual_inj;
extern int                 alct1_amu_inj;
extern int                 alct1_key_inj;
extern int                 alct1_bxn_inj;

extern int                 alct0_vpf;
extern int                 alct0_qual;
extern int                 alct0_amu;
extern int                 alct0_key;
extern int                 alct0_bxn;

extern int                 alct1_vpf;
extern int                 alct1_qual;
extern int                 alct1_amu;
extern int                 alct1_key;
extern int                 alct1_bxn;

extern int                 vf_data[mxframe];
extern int                 iframe;
extern int                 ilayer;

extern int                 adr_e0b;
extern int                 r_nheaders;
extern int                 r_ncfebs;
extern int                 r_fifo_tbins;

extern int                 read_pat[mxtbins][mxly][mxdsabs];
extern int                 rdcid;
extern int                 rdtbin;
extern int                 hits8;
extern int                 hits1;
extern int                 ids;
extern int                 ids_abs;
extern int                 jcfeb;
const char                x[]="          ";
extern bool                display_cfeb;
extern int                 clct_bxn_expect;

extern int                 clct_key_expect[mxclcts];
extern int                 clct_pid_expect[mxclcts];
extern int                 clct_hit_expect[mxclcts];
extern int                 hit_thresh_pretrig_temp;
extern int                 hit_thresh_postdrift_temp;

extern int             fmm_state;
extern string          sfmm_state[5];

extern bool            rdraw;
extern int             nbxn0;
extern int             ntrig;

extern int             alct_raw_busy;
extern int             alct_raw_done;
extern int             alct_raw_nwords;
extern int             alct_raw_data;

extern int             alct0_raw_lsb;
extern int             alct0_raw_msb;
extern int             alct0_raw;

extern int             alct1_raw_lsb;
extern int             alct1_raw_msb;
extern int             alct1_raw;

extern int             seqdeb_adr;
extern int             seqdeb_rd_mux;
extern unsigned long   deb_adr_diff;
extern unsigned long   deb_wr_buf_adr;
extern unsigned long   deb_buf_push_adr;
extern unsigned long   deb_buf_pop_adr;
extern unsigned long   deb_buf_push_data;
extern unsigned long   deb_buf_pop_data;

extern int             radix;
extern FILE*               ram_file;
extern string              ram_file_name;
extern int             iadr;
extern int             blue_flash;
extern bool            fire_injector;
extern bool            err_check;

extern unsigned short  wr_fire_l1a;

extern int                 l1a_lookback;
extern int             hdr_wr_continuous;
extern int             lookback_triad_hits[2048];

extern int             first_nonzero_bx;
extern int             last_nonzero_bx;
extern int             first_bx;
extern int             last_bx;
extern int             max_triads;
extern double          scale;
extern int             tck;

// Miniscope
extern int             	mini_read_enable;
extern int             	mini_test;
extern int             	mini_tbins_word;
extern int             	fifo_pretrig_mini;
extern int             	csc_id;
extern int                 maxv;
extern int                 minv;
extern char                symbol;
extern int           		bcb_read_enable;

// Pattern_finder
extern int                 cfeb_en[5];
extern int                 hs_key_1st_emu;
extern int                 hs_pid_1st_emu;
extern int                 hs_hit_1st_emu;

extern int                 hs_key_2nd_emu;
extern int                 hs_pid_2nd_emu;
extern int                 hs_hit_2nd_emu;

extern int                 nlayers_hit_emu;
extern int                 layer_trig_emu;
extern int                 cfeb_active_emu[5];

extern bool                emulator_pretrig;
extern bool                emulator_latch_clct0;
extern bool                emulator_latch_clct1;
extern bool                send_emulator_to_tmb;

extern int                 clct0_vpf_emu;
extern int                 clct0_hit_emu;
extern int                 clct0_pid_emu;
extern int                 clct0_key_emu;
extern int                 clct0_cfeb_emu;
extern int                 clctc_sync_emu;

extern int                 clct1_vpf_emu;
extern int                 clct1_hit_emu;
extern int                 clct1_pid_emu;
extern int                 clct1_key_emu;
extern int                 clct1_cfeb_emu;

extern int                 clct0_vpf_vme;
extern int                 clct0_hit_vme;
extern int                 clct0_pid_vme;
extern int                 clct0_key_vme;
extern int                 clct0_cfeb_vme;

extern int                 clct1_vpf_vme;
extern int                 clct1_hit_vme;
extern int                 clct1_pid_vme;
extern int                 clct1_key_vme;
extern int                 clct1_cfeb_vme;

extern int                 clctc_bxn_vme;
extern int                 clctc_sync_vme;

extern int                 nspan;
extern int                 pspan;
extern int                 busy_min;
extern int                 busy_max;
extern bool                clct0_is_on_me1a;
extern int                 key_inj;
extern int                 busy_key_inj[160];

extern int                 clct_hit_inj_expect[2];
extern int                 clct_key_inj_expect[2];
extern int                 clct_pid_inj_expect[2];
extern int                 clct_pat_inj_expect[2];

extern bool                injector_clct0_over;
extern bool                injector_clct1_over;
extern bool                injector_pretrig;
extern bool                injector_latch_clct0;
extern bool                injector_latch_clct1;

extern int                 clct0_vpf_vme_expect;
extern int                 clct0_hit_vme_expect;
extern int                 clct0_pid_vme_expect;
extern int                 clct0_key_vme_expect;
extern int                 clct0_cfeb_vme_expect;
extern int                 clctc_sync_vme_expect;

extern int                 clct1_vpf_vme_expect;
extern int                 clct1_hit_vme_expect;
extern int                 clct1_pid_vme_expect;
extern int                 clct1_key_vme_expect;
extern int                 clct1_cfeb_vme_expect;

extern int                 clct0_vpf_tmb;
extern int                 clct0_hit_tmb;
extern int                 clct0_pid_tmb;
extern int                 clct0_key_tmb;
extern int                 clct0_cfeb_tmb;

extern int                 clct1_vpf_tmb;
extern int                 clct1_hit_tmb;
extern int                 clct1_pid_tmb;
extern int                 clct1_key_tmb;
extern int                 clct1_cfeb_tmb;

extern int                 clctc_bxn_tmb;
extern int                 clctc_sync_err_tmb;

extern int                 key0;
extern int                 key1;

extern bool            kill_me1a_clcts;
extern bool            clct0_exists;
extern bool            clct1_exists;
extern bool            clct0_cfeb4;
extern bool            clct1_cfeb4;
extern bool            kill_clct0;
extern bool            kill_clct1;
extern bool            kill_trig;
extern bool            bans;

extern bool            clct_noalct_lost;
extern bool            alct_only_trig;

extern bool            clct_keep;
extern bool            alct_keep;

extern bool            clct_keep_ro;
extern bool            alct_keep_ro;

extern bool            clct_discard;
extern bool            alct_discard;

extern bool            clct_match;
extern bool            clct_noalct;
extern bool            alct_noclct;
extern bool            alct_pulse;

extern bool            trig_pulse;
extern bool            trig_keep;
extern bool            non_trig_keep;
extern bool            alct_only;

extern bool            clct_match_tr;
extern bool            alct_noclct_tr;
extern bool            clct_noalct_tr;
extern bool            clct_match_ro;
extern bool            alct_noclct_ro;
extern bool            clct_noalct_ro;

extern bool            tmb_trig_pulse;
extern bool            tmb_trig_keep_ff;
extern bool            tmb_non_trig_keep_ff;

extern bool            tmb_match;
extern bool            tmb_alct_only;
extern bool            tmb_clct_only;

extern bool            tmb_match_ro_ff;
extern bool            tmb_alct_only_ro_ff;
extern bool            tmb_clct_only_ro_ff;

extern bool            tmb_alct_discard;
extern bool            tmb_clct_discard;

extern int                 tmb_alct0;
extern int                 tmb_alct1;
extern int                 alct0_pipe;
extern int                 alct1_pipe;
extern int                 alct0_real;
extern int                 alct1_real;

extern int                 keep_clct;
extern int                 clct0_real;
extern int                 clct0_pipe;
extern int                 clct1_real;
extern int                 clct1_pipe;
extern int                 clctc_real;
extern int                 clctc_pipe;

extern bool            tmb_match_ro;
extern bool            tmb_alct_only_ro;
extern bool            tmb_clct_only_ro;

extern bool            tmb_trig_keep;
extern bool            tmb_non_trig_keep;

extern int             tmb_no_alct_emu;
extern int             tmb_no_clct_emu;
extern int             tmb_one_alct_emu;
extern int             tmb_one_clct_emu;
extern int             tmb_two_alct_emu;
extern int             tmb_two_clct_emu;
extern int             tmb_dupe_alct_emu;
extern int             tmb_dupe_clct_emu;

extern int             alct_dummy;
extern int             clct_dummy;
extern int             clctc_dummy;
extern int             clct_bxn_insert_emu;

extern int             clct0_emu;
extern int             clct1_emu;
extern int             clctc_emu;

extern int             alct0_emu;
extern int             alct1_emu;

extern int             lct0_vpf_emu;
extern int             lct1_vpf_emu;

extern int             alct0_vpf_emu;
extern int             alct0_valid_emu;
extern int             alct0_quality_emu;
extern int             alct0_amu_emu;
extern int             alct0_key_emu;
extern int             alct0_bxn_emu;

extern int             alct1_vpf_emu;
extern int             alct1_valid_emu;
extern int             alct1_quality_emu;
extern int             alct1_amu_emu;
extern int             alct1_key_emu;
extern int             alct1_bxn_emu;

extern int             clct0_valid_emu;
extern int             clct0_bend_emu;

extern int             clct1_valid_emu;
extern int             clct1_bend_emu;

extern int             clct_bxn_emu;
extern int             clct_sync_err_emu;

extern int             alct0_hit_emu;
extern int             alct1_hit_emu;

extern int             clct0_cpat_emu;
extern int             clct1_cpat_emu;

extern int             ACC;
extern int             A;
extern int             C;
extern int             A4;
extern int             C4;
extern int             P;
extern int             CPAT;
extern int             Q;

extern int             lct0_quality_emu;
extern int             lct1_quality_emu;
extern int             tmb_rank_err_emu;

extern int             alct_bx0_emu;
extern int             clct_bx0_emu;

extern int             csc_id_emu;

extern int             trig_mpc_emu;
extern int             trig_mpc0_emu;
extern int             trig_mpc1_emu;

extern int             mpc0_frame0_emu;
extern int             mpc0_frame1_emu;
extern int             mpc1_frame0_emu;
extern int             mpc1_frame1_emu;

extern int             mpc0_frame0_pulse;
extern int             mpc0_frame1_pulse;
extern int             mpc1_frame0_pulse;
extern int             mpc1_frame1_pulse;

extern int             mpc_alct0_key_expect;
extern int             mpc_clct0_pid_expect;
extern int             mpc_lct0_qual_expect;
extern int             mpc_lct0_vpf_expect;

extern int             mpc_clct0_key_expect;
extern int             mpc_clct0_bend_expect;
extern int             mpc_sync_err0_expect;
extern int             mpc_alct0_bxn_expect;
extern int             mpc_bx0_clct_expect;
extern int             mpc_csc_id0_expect;

extern int             mpc_alct1_key_expect;
extern int             mpc_clct1_pid_expect;
extern int             mpc_lct1_qual_expect;
extern int             mpc_lct1_vpf_expect;

extern int             mpc_clct1_key_expect;
extern int             mpc_clct1_bend_expect;
extern int             mpc_sync_err1_expect;
extern int             mpc_alct1_bxn_expect;
extern int             mpc_bx0_alct_expect;
extern int             mpc_csc_id1_expect;

extern int             mpc_alct0_key_vme;
extern int             mpc_clct0_pid_vme;
extern int             mpc_lct0_qual_vme;
extern int             mpc_lct0_vpf_vme;

extern int             mpc_bx0_clct_vme;
extern int             mpc_clct0_key_vme;
extern int             mpc_sync_err0_vme;
extern int             mpc_csc_id0_vme;
extern int             mpc_clct0_bend_vme;
extern int             mpc_alct0_bxn_vme;

extern int             mpc_alct1_key_vme;
extern int             mpc_clct1_pid_vme;
extern int             mpc_lct1_qual_vme;
extern int             mpc_lct1_vpf_vme;

extern int                 mpc_clct1_key_vme;
extern int                 mpc_clct1_bend_vme;
extern int                 mpc_sync_err1_vme;
extern int                 mpc_alct1_bxn_vme;
extern int                 mpc_bx0_alct_vme;
extern int                 mpc_csc_id1_vme;

extern int             lct0_vme;
extern int             lct1_vme;

extern int                 ihs_min;
extern int                 ihs_max;
extern int                 ihs_hit;
extern int                 ihs_ds;

extern int                 sync_err; 
extern int                 clct_bx0_sync_err; 
extern int                 alct_ecc_rx_err_ff; 
extern int                 alct_ecc_tx_err_ff; 
extern int                 bx0_match_err_ff; 
extern int                 sync_err_force; 

extern bool                load_clct_injector_image;
extern int                 dscfeb[mxcfeb];

extern int                 mask_all;
extern int                 mask_cfeb;
extern int                 inj_trig;
extern int                 inj_busy;
extern bool                engage_pattern_finder;

extern int                 wr_data_lsb;
extern int                 wr_data_msb;
extern int                 rd_data_lsb;
extern int                 rd_data_msb;
extern int                 l1a_lookback_data;

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

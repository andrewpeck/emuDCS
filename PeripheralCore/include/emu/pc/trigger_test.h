#ifndef TRIGGER_TEST_H
#define TRIGGER_TEST_H
//#include <cmath>
//#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <iomanip>

//using namespace std; 

//------------------------------------------------------------------------------
//  TMB VME Addresses
//------------------------------------------------------------------------------

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


#define     logical(L)  ((L)?'T':'F')
#define     yesno(L)    ((L)?'y':'n')

//------------------------------------------------------------------------------
// Preprocessor macros for writing fixed width Hex 
// needed because cout sucks
//------------------------------------------------------------------------------
#define HEX2(x) std::setw(2) << std::setfill('0') << std::hex << ( x )
#define HEX4(x) std::setw(4) << std::setfill('0') << std::hex << ( x )
#define HEX8(x) std::setw(8) << std::setfill('0') << std::hex << ( x )


extern                  std::string msg_string; 
extern int              data_read; 
extern int              data_expect; 

//------------------------------------------------------------------------------
// File scope declarations
//------------------------------------------------------------------------------

// VME calls
extern long             status;
extern unsigned long    boot_adr;
extern unsigned long    adr;
extern unsigned short   rd_data;
extern unsigned short   wr_data;

//------------------------------------------------------------------------------
// Event counters
//------------------------------------------------------------------------------
const int               mxcounter = 86;
extern int              cnt_lsb;
extern int              cnt_msb;
extern int              cnt_full;
extern int              cnt[mxcounter];
extern std::string      scnt[mxcounter];
//------------------------------------------------------------------------------
// Local
//------------------------------------------------------------------------------
extern int              id_slot;

// CSC type
extern int              csc_me1ab;
extern int              stagger_hs_csc;
extern int              reverse_hs_csc;
extern int              reverse_hs_me1a;
extern int              reverse_hs_me1b;
extern int              csc_type;

//string           flags_ok;

// Menu
extern char           	line[80];
extern int              ifunc;
extern int              i,j,k,n;

// CCB
extern int              ttc_cmd;
extern int              ccb_cmd;

// Trigger tests
extern int              dmb_thresh_pretrig;
extern int              hit_thresh_pretrig;
extern int              hit_thresh_postdrift;
extern int              pid_thresh_pretrig;
extern int              pid_thresh_postdrift;
extern int              lyr_thresh_pretrig;

extern int              triad_persist;
extern int              drift_delay;
extern int              clct_sep;
extern int              active_feb_src;
extern int              active_feb_list;

extern bool             layer_mode;
extern int              layer_trig_en;

extern int              fifo_mode;
extern int              fifo_tbins;
extern int              fifo_pretrig;

extern int              fifo_tbins_rpc;
extern int              fifo_pretrig_rpc;
extern int              rpc_decouple;

extern int              alct_bx0_en;
extern int              alct_delay;
extern int              clct_width;

extern int              tmb_allow_clct;
extern int              tmb_allow_alct;
extern int              tmb_allow_match;

extern int              tmb_allow_alct_ro;
extern int              tmb_allow_clct_ro;
extern int              tmb_allow_match_ro;

extern int              inj_delay_rat;
extern int              rpc_tbins_test;
extern int              rpc_exists;

extern bool             rrhd;
extern bool             cprr;
extern bool             cprr_ignore;
extern bool             rat_injector_sync;
extern bool             rat_injector_enable;
extern bool             rpcs_in_rdout;
extern bool             pause_on_fail;
extern int              vme_bx0_emu_en_default;

extern int              nclcts_inject;
extern int              nalcts_inject;
extern int              mcl;

const int               mxclcts=8;
extern int              clct_pat_inject[mxclcts];
extern int              clct_pid_inject[mxclcts];
extern int              clct_key_inject[mxclcts];
extern int              clct_hit_inject[mxclcts];
extern bool             clct_blanked[mxclcts];
extern int              clct_hit_inject_clip[mxclcts];
extern bool             loop_keys[mxclcts];
extern bool             loop_pids[mxclcts];
extern int              iclct;

extern int              triad_1st_tbin[6];
extern int              l1a_delay; // hits 0th l1a window bx
extern int              rat_sync_mode;
extern int              rat_injector_delay;

extern int              irpc;
extern int              ibxn;
extern int              rpc_pad;
extern int              rpc_inj_bxn;
extern int              rpc_inj_image[256][2];
extern int              rpc_inj_wen;
extern int              rpc_inj_ren;
extern int              rpc_inj_rwadr;
extern int              rpc_inj_data;

// Scope
extern bool             rdscope;
extern int              scp_arm;
extern int              scp_readout;
extern int              scp_raw_decode;
extern int              scp_silent;
extern int              scp_raw_data[512*160/16];
extern int              scp_auto;
extern int              scp_nowrite;

// Misc
extern int              ipass;

// Pattern cells
extern int              icell;
extern int              ihit;
extern int              ihitp;

extern int              nhits;
extern int              layer;

extern int              ikey;
extern int              ikeylp;
extern int              ikey_min;
extern int              ikey_max;
extern int              ikey_sep;

extern int              ipid;
extern int              ipidlp;
extern int              ipid_min;
extern int              ipid_max;

extern int              icfeb;
extern int              icfebg;
extern int              key;
extern int              ihs[6][160];
extern int              nstag;

extern int              idistrip;
extern int              idslocal;
extern int              itbin;
extern int              itbin0;
extern int              ihstrip;
extern int              iram;
//int                   pat_ram[32][3][5]={0};
extern int              pat_ram[32][3][5];
extern int              wen;
extern int              ren;
extern int              wadr;
extern int              febsel;
extern int              icfeblp;
extern int              ibit;
extern int              wr_data_mem;
extern int              rd_data_mem;

extern std::string      marker;

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
     {0,0,0,0,0,0,0,0,0,0,0}},     // ly5

    {{0,0,0,0,0,0,0,0,0,0,0},      // ly0 Pattern 1, empty
     {0,0,0,0,0,0,0,0,0,0,0},      // ly1
     {0,0,0,0,0,0,0,0,0,0,0},      // ly2
     {0,0,0,0,0,0,0,0,0,0,0},      // ly3
     {0,0,0,0,0,0,0,0,0,0,0},      // ly4
     {0,0,0,0,0,0,0,0,0,0,0}},     // ly5

    {{0,0,0,0,0,0,0,0,2,2,1},      // ly0 Pattern 2, 0=no hit, 1=primary hit, 2=secondary hit
     {0,0,0,0,0,0,2,1,0,0,0},      // ly1
     {0,0,0,0,0,1,0,0,0,0,0},      // ly2
     {0,0,0,1,2,2,0,0,0,0,0},      // ly3
     {0,1,2,2,0,0,0,0,0,0,0},      // ly4
     {1,2,2,0,0,0,0,0,0,0,0}},     // ly5

    {{1,2,2,0,0,0,0,0,0,0,0},      // ly0 Pattern 3
     {0,0,0,1,2,0,0,0,0,0,0},      // ly1
     {0,0,0,0,0,1,0,0,0,0,0},      // ly2
     {0,0,0,0,0,2,2,1,0,0,0},      // ly3
     {0,0,0,0,0,0,0,2,2,1,0},      // ly4
     {0,0,0,0,0,0,0,0,2,2,1}},     // ly5

    {{0,0,0,0,0,0,0,2,2,1,0},      // ly0 Pattern 4
     {0,0,0,0,0,0,2,1,0,0,0},      // ly1
     {0,0,0,0,0,1,0,0,0,0,0},      // ly2
     {0,0,0,1,2,0,0,0,0,0,0},      // ly3
     {0,1,2,2,0,0,0,0,0,0,0},      // ly4
     {0,1,2,2,0,0,0,0,0,0,0}},     // ly5

    {{0,1,2,2,0,0,0,0,0,0,0},      // ly0 Pattern 5
     {0,0,0,1,2,0,0,0,0,0,0},      // ly1
     {0,0,0,0,0,1,0,0,0,0,0},      // ly2
     {0,0,0,0,0,0,2,1,0,0,0},      // ly3
     {0,0,0,0,0,0,0,2,2,1,0},      // ly4
     {0,0,0,0,0,0,0,2,2,1,0}},     // ly5

    {{0,0,0,0,0,0,2,2,1,0,0},      // ly0 Pattern 6
     {0,0,0,0,0,2,1,0,0,0,0},      // ly1
     {0,0,0,0,0,1,0,0,0,0,0},      // ly2
     {0,0,0,0,1,2,0,0,0,0,0},      // ly3
     {0,0,0,1,2,0,0,0,0,0,0},      // ly4
     {0,0,1,2,2,0,0,0,0,0,0}},     // ly5

    {{0,0,1,2,2,0,0,0,0,0,0},      // ly0 Pattern 7
     {0,0,0,0,1,2,0,0,0,0,0},      // ly1
     {0,0,0,0,0,1,0,0,0,0,0},      // ly2
     {0,0,0,0,0,2,1,0,0,0,0},      // ly3
     {0,0,0,0,0,0,2,1,0,0,0},      // ly4
     {0,0,0,0,0,0,2,2,1,0,0}},     // ly5

    {{0,0,0,0,0,2,2,1,0,0,0},      // ly0 Pattern 8
     {0,0,0,0,0,2,1,0,0,0,0},      // ly1
     {0,0,0,0,0,1,0,0,0,0,0},      // ly2
     {0,0,0,0,1,2,0,0,0,0,0},      // ly3
     {0,0,0,1,2,2,0,0,0,0,0},      // ly4
     {0,0,0,1,2,2,0,0,0,0,0}},     // ly5

    {{0,0,0,1,2,2,0,0,0,0,0},      // ly0 Pattern 9
     {0,0,0,0,1,2,0,0,0,0,0},      // ly1
     {0,0,0,0,0,1,0,0,0,0,0},      // ly2
     {0,0,0,0,0,2,1,0,0,0,0},      // ly3
     {0,0,0,0,0,2,2,1,0,0,0},      // ly4
     {0,0,0,0,0,2,2,1,0,0,0}},     // ly5

    {{0,0,0,0,2,1,2,0,0,0,0},      // ly0 Pattern A
     {0,0,0,0,0,1,0,0,0,0,0},      // ly1
     {0,0,0,0,0,1,0,0,0,0,0},      // ly2
     {0,0,0,0,0,1,0,0,0,0,0},      // ly3
     {0,0,0,0,2,1,2,0,0,0,0},      // ly4
     {0,0,0,0,2,1,2,0,0,0,0}}      // ly5
};

//------------------------------------------------------------------------------
extern int             	alct_injector_delay; // experimental

extern int              alct0_rd;
extern int              alct1_rd;
extern int              alct0_prev;
extern int              alct1_prev;

// crc checks
extern long int         din;
extern long int         crc;
extern long int         tmb_crc_lsb;
extern long int         tmb_crc_msb;
extern long int         tmb_crc;
extern bool             crc_match;

extern int              dmb_wdcnt;
extern int              dmb_busy;
extern int              dmb_rdata;
extern int              dmb_rdata_lsb;
extern int              dmb_rdata_msb;

extern int              nlayers_hit;
extern int              scint_veto;

extern int              alct0_inj_rd;
extern int              alct1_inj_rd;
extern int              alct0_inj_wr;
extern int              alct1_inj_wr;

extern int              alct0_vpf_inj;
extern int              alct0_qual_inj;
extern int              alct0_amu_inj;
extern int              alct0_key_inj;
extern int              alct0_bxn_inj;

extern int              alct1_vpf_inj;
extern int              alct1_qual_inj;
extern int              alct1_amu_inj;
extern int              alct1_key_inj;
extern int              alct1_bxn_inj;

extern int              alct0_vpf;
extern int              alct0_qual;
extern int              alct0_amu;
extern int              alct0_key;
extern int              alct0_bxn;

extern int              alct1_vpf;
extern int              alct1_qual;
extern int              alct1_amu;
extern int              alct1_key;
extern int              alct1_bxn;

extern int              vf_data[mxframe];
extern int              iframe;
extern int              ilayer;

extern int              adr_e0b;
extern int              r_nheaders;
extern int              r_ncfebs;
extern int              r_fifo_tbins;

extern int              read_pat[mxtbins][mxly][mxdsabs];
extern int              rdcid;
extern int              rdtbin;
extern int              hits8;
extern int              hits1;
extern int              ids;
extern int              ids_abs;
extern int              jcfeb;
const char              x[]="          ";
extern bool             display_cfeb;
extern int              clct_bxn_expect;

extern int              clct_key_expect[mxclcts];
extern int              clct_pid_expect[mxclcts];
extern int              clct_hit_expect[mxclcts];
extern int              hit_thresh_pretrig_temp;
extern int              hit_thresh_postdrift_temp;

extern int              fmm_state;
extern std::string      sfmm_state[5];

extern bool             rdraw;
extern int              nbxn0;
extern int              ntrig;

extern int              alct_raw_busy;
extern int              alct_raw_done;
extern int              alct_raw_nwords;
extern int              alct_raw_data;

extern int              alct0_raw_lsb;
extern int              alct0_raw_msb;
extern int              alct0_raw;

extern int              alct1_raw_lsb;
extern int              alct1_raw_msb;
extern int              alct1_raw;

extern int              seqdeb_adr;
extern int              seqdeb_rd_mux;
extern unsigned long    deb_adr_diff;
extern unsigned long    deb_wr_buf_adr;
extern unsigned long    deb_buf_push_adr;
extern unsigned long    deb_buf_pop_adr;
extern unsigned long    deb_buf_push_data;
extern unsigned long    deb_buf_pop_data;

extern int              radix;
extern FILE*            ram_file;
extern std::string      ram_file_name;
extern int              iadr;
extern int              blue_flash;
extern bool             fire_injector;
extern bool             err_check;

extern unsigned short   wr_fire_l1a;

extern int              l1a_lookback;
extern int              hdr_wr_continuous;
extern int              lookback_triad_hits[2048];

extern int              first_nonzero_bx;
extern int              last_nonzero_bx;
extern int              first_bx;
extern int              last_bx;
extern int              max_triads;
extern double           scale;
extern int              tck;

// Miniscope
extern int             	mini_read_enable;
extern int             	mini_test;
extern int             	mini_tbins_word;
extern int             	fifo_pretrig_mini;
extern int             	csc_id;
extern int              maxv;
extern int              minv;
extern char             symbol;
extern int              bcb_read_enable;

// Pattern_finder
extern int              cfeb_en[5];
extern int              hs_key_1st_emu;
extern int              hs_pid_1st_emu;
extern int              hs_hit_1st_emu;

extern int              hs_key_2nd_emu;
extern int              hs_pid_2nd_emu;
extern int              hs_hit_2nd_emu;

extern int              nlayers_hit_emu;
extern int              layer_trig_emu;
extern int              cfeb_active_emu[5];

extern bool             emulator_pretrig;
extern bool             emulator_latch_clct0;
extern bool             emulator_latch_clct1;
extern bool             send_emulator_to_tmb;

extern int              clct0_vpf_emu;
extern int              clct0_hit_emu;
extern int              clct0_pid_emu;
extern int              clct0_key_emu;
extern int              clct0_cfeb_emu;
extern int              clctc_sync_emu;

extern int              clct1_vpf_emu;
extern int              clct1_hit_emu;
extern int              clct1_pid_emu;
extern int              clct1_key_emu;
extern int              clct1_cfeb_emu;

extern int              clct0_vpf_vme;
extern int              clct0_hit_vme;
extern int              clct0_pid_vme;
extern int              clct0_key_vme;
extern int              clct0_cfeb_vme;

extern int              clct1_vpf_vme;
extern int              clct1_hit_vme;
extern int              clct1_pid_vme;
extern int              clct1_key_vme;
extern int              clct1_cfeb_vme;

extern int              clctc_bxn_vme;
extern int              clctc_sync_vme;

extern int              nspan;
extern int              pspan;
extern int              busy_min;
extern int              busy_max;
extern bool             clct0_is_on_me1a;
extern int              key_inj;
extern int              busy_key_inj[160];

extern int              clct_hit_inj_expect[2];
extern int              clct_key_inj_expect[2];
extern int              clct_pid_inj_expect[2];
extern int              clct_pat_inj_expect[2];

extern bool             injector_clct0_over;
extern bool             injector_clct1_over;
extern bool             injector_pretrig;
extern bool             injector_latch_clct0;
extern bool             injector_latch_clct1;

extern int              clct0_vpf_vme_expect;
extern int              clct0_hit_vme_expect;
extern int              clct0_pid_vme_expect;
extern int              clct0_key_vme_expect;
extern int              clct0_cfeb_vme_expect;
extern int              clctc_sync_vme_expect;

extern int              clct1_vpf_vme_expect;
extern int              clct1_hit_vme_expect;
extern int              clct1_pid_vme_expect;
extern int              clct1_key_vme_expect;
extern int              clct1_cfeb_vme_expect;

extern int              clct0_vpf_tmb;
extern int              clct0_hit_tmb;
extern int              clct0_pid_tmb;
extern int              clct0_key_tmb;
extern int              clct0_cfeb_tmb;

extern int              clct1_vpf_tmb;
extern int              clct1_hit_tmb;
extern int              clct1_pid_tmb;
extern int              clct1_key_tmb;
extern int              clct1_cfeb_tmb;

extern int              clctc_bxn_tmb;
extern int              clctc_sync_err_tmb;

extern int              key0;
extern int              key1;

extern bool             kill_me1a_clcts;
extern bool             clct0_exists;
extern bool             clct1_exists;
extern bool             clct0_cfeb4;
extern bool             clct1_cfeb4;
extern bool             kill_clct0;
extern bool             kill_clct1;
extern bool             kill_trig;
extern bool             bans;

extern bool             clct_noalct_lost;
extern bool             alct_only_trig;

extern bool             clct_keep;
extern bool             alct_keep;

extern bool             clct_keep_ro;
extern bool             alct_keep_ro;

extern bool             clct_discard;
extern bool             alct_discard;

extern bool             clct_match;
extern bool             clct_noalct;
extern bool             alct_noclct;
extern bool             alct_pulse;

extern bool             trig_pulse;
extern bool             trig_keep;
extern bool             non_trig_keep;
extern bool             alct_only;

extern bool             clct_match_tr;
extern bool             alct_noclct_tr;
extern bool             clct_noalct_tr;
extern bool             clct_match_ro;
extern bool             alct_noclct_ro;
extern bool             clct_noalct_ro;

extern bool             tmb_trig_pulse;
extern bool             tmb_trig_keep_ff;
extern bool             tmb_non_trig_keep_ff;

extern bool             tmb_match;
extern bool             tmb_alct_only;
extern bool             tmb_clct_only;

extern bool             tmb_match_ro_ff;
extern bool             tmb_alct_only_ro_ff;
extern bool             tmb_clct_only_ro_ff;

extern bool             tmb_alct_discard;
extern bool             tmb_clct_discard;

extern int              tmb_alct0;
extern int              tmb_alct1;
extern int              alct0_pipe;
extern int              alct1_pipe;
extern int              alct0_real;
extern int              alct1_real;

extern int              keep_clct;
extern int              clct0_real;
extern int              clct0_pipe;
extern int              clct1_real;
extern int              clct1_pipe;
extern int              clctc_real;
extern int              clctc_pipe;

extern bool             tmb_match_ro;
extern bool             tmb_alct_only_ro;
extern bool             tmb_clct_only_ro;

extern bool             tmb_trig_keep;
extern bool             tmb_non_trig_keep;

extern int              tmb_no_alct_emu;
extern int              tmb_no_clct_emu;
extern int              tmb_one_alct_emu;
extern int              tmb_one_clct_emu;
extern int              tmb_two_alct_emu;
extern int              tmb_two_clct_emu;
extern int              tmb_dupe_alct_emu;
extern int              tmb_dupe_clct_emu;

extern int              alct_dummy;
extern int              clct_dummy;
extern int              clctc_dummy;
extern int              clct_bxn_insert_emu;

extern int              clct0_emu;
extern int              clct1_emu;
extern int              clctc_emu;

extern int              alct0_emu;
extern int              alct1_emu;

extern int              lct0_vpf_emu;
extern int              lct1_vpf_emu;

extern int              alct0_vpf_emu;
extern int              alct0_valid_emu;
extern int              alct0_quality_emu;
extern int              alct0_amu_emu;
extern int              alct0_key_emu;
extern int              alct0_bxn_emu;

extern int              alct1_vpf_emu;
extern int              alct1_valid_emu;
extern int              alct1_quality_emu;
extern int              alct1_amu_emu;
extern int              alct1_key_emu;
extern int              alct1_bxn_emu;

extern int              clct0_valid_emu;
extern int              clct0_bend_emu;

extern int              clct1_valid_emu;
extern int              clct1_bend_emu;

extern int              clct_bxn_emu;
extern int              clct_sync_err_emu;

extern int              alct0_hit_emu;
extern int              alct1_hit_emu;

extern int              clct0_cpat_emu;
extern int              clct1_cpat_emu;

extern int              ACC;
extern int              A;
extern int              C;
extern int              A4;
extern int              C4;
extern int              P;
extern int              CPAT;
extern int              Q;

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

extern int              mpc_alct0_key_vme;
extern int              mpc_clct0_pid_vme;
extern int              mpc_lct0_qual_vme;
extern int              mpc_lct0_vpf_vme;

extern int              mpc_bx0_clct_vme;
extern int              mpc_clct0_key_vme;
extern int              mpc_sync_err0_vme;
extern int              mpc_csc_id0_vme;
extern int              mpc_clct0_bend_vme;
extern int              mpc_alct0_bxn_vme;

extern int              mpc_alct1_key_vme;
extern int              mpc_clct1_pid_vme;
extern int              mpc_lct1_qual_vme;
extern int              mpc_lct1_vpf_vme;

extern int              mpc_clct1_key_vme;
extern int              mpc_clct1_bend_vme;
extern int              mpc_sync_err1_vme;
extern int              mpc_alct1_bxn_vme;
extern int              mpc_bx0_alct_vme;
extern int              mpc_csc_id1_vme;

extern int              lct0_vme;
extern int              lct1_vme;

extern int              ihs_min;
extern int              ihs_max;
extern int              ihs_hit;
extern int              ihs_ds;

extern int              sync_err; 
extern int              clct_bx0_sync_err; 
extern int              alct_ecc_rx_err_ff; 
extern int              alct_ecc_tx_err_ff; 
extern int              bx0_match_err_ff; 
extern int              sync_err_force; 

extern bool             load_clct_injector_image;
extern int              dscfeb[mxcfeb];

extern int              mask_all;
extern int              mask_cfeb;
extern int              inj_trig;
extern int              inj_busy;
extern bool             engage_pattern_finder;

extern int              wr_data_lsb;
extern int              wr_data_msb;
extern int              rd_data_lsb;
extern int              rd_data_msb;
extern int              l1a_lookback_data;
#endif

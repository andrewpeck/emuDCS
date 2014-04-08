#include <stdio.h>
#include <iostream>
#include "emu/pc/trigger_test.h"

//------------------------------------------------------------------------------
//  Common
//------------------------------------------------------------------------------

#define     logical(L)  ((L)?'T':'F')
#define     yesno(L)    ((L)?'y':'n')

//------------------------------------------------------------------------------
// Preprocessor macros for writing fixed width Hex 
// needed because cout sucks
//------------------------------------------------------------------------------
#define HEX2(x)
   setw(2) << setfill('0') << std::hex << ( x )
#define HEX4(x)
   setw(4) << setfill('0') << std::hex << ( x )
#define HEX8(x)
   setw(8) << setfill('0') << std::hex << ( x )


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


std::string     msg_string; 
int             data_read; 
int             data_expect; 

//------------------------------------------------------------------------------
// File scope declarations
//------------------------------------------------------------------------------

// VME calls
long             status;
unsigned long    boot_adr;
unsigned long    adr;
unsigned short   rd_data;
unsigned short   wr_data;

//------------------------------------------------------------------------------
// Event counters
//------------------------------------------------------------------------------
int              cnt_lsb;
int              cnt_msb;
int              cnt_full;
int              cnt[mxcounter];
std::string      scnt[mxcounter];
//------------------------------------------------------------------------------
// Local
//------------------------------------------------------------------------------
int              id_slot;

// CSC type
int              csc_me1ab;
int              stagger_hs_csc;
int              reverse_hs_csc;
int              reverse_hs_me1a;
int              reverse_hs_me1b;
int              csc_type;

//string           flags_ok;

// Menu
char           	line[80];
int              ifunc;
int              i,j,k,n;

// CCB
int              ttc_cmd;
int              ccb_cmd;

// Trigger tests
int              dmb_thresh_pretrig;
int              hit_thresh_pretrig;
int              hit_thresh_postdrift;
int              pid_thresh_pretrig;
int              pid_thresh_postdrift;
int              lyr_thresh_pretrig;

int              triad_persist;
int              drift_delay;
int              clct_sep;
int              active_feb_src;
int              active_feb_list;

bool             layer_mode;
int              layer_trig_en;

int              fifo_mode;
int              fifo_tbins;
int              fifo_pretrig;

int              fifo_tbins_rpc;
int              fifo_pretrig_rpc;
int              rpc_decouple;

int              alct_bx0_en;
int              alct_delay;
int              clct_width;

int              tmb_allow_clct;
int              tmb_allow_alct;
int              tmb_allow_match;

int              tmb_allow_alct_ro;
int              tmb_allow_clct_ro;
int              tmb_allow_match_ro;

int              inj_delay_rat;
int              rpc_tbins_test;
int              rpc_exists;

bool             rrhd;
bool             cprr;
bool             cprr_ignore;
bool             rat_injector_sync;
bool             rat_injector_enable;
bool             rpcs_in_rdout;
bool             pause_on_fail;
int              vme_bx0_emu_en_default;

int              nclcts_inject;
int              nalcts_inject;
int              mcl;

int              clct_pat_inject[mxclcts];
int              clct_pid_inject[mxclcts];
int              clct_key_inject[mxclcts];
int              clct_hit_inject[mxclcts];
bool             clct_blanked[mxclcts];
int              clct_hit_inject_clip[mxclcts];
bool             loop_keys[mxclcts];
bool             loop_pids[mxclcts];
int              iclct;

int              triad_1st_tbin[6];
int              l1a_delay; // hits 0th l1a window bx
int              rat_sync_mode;
int              rat_injector_delay;

int              irpc;
int              ibxn;
int              rpc_pad;
int              rpc_inj_bxn;
int              rpc_inj_image[256][2];
int              rpc_inj_wen;
int              rpc_inj_ren;
int              rpc_inj_rwadr;
int              rpc_inj_data;

// Scope
bool             rdscope;
int              scp_arm;
int              scp_readout;
int              scp_raw_decode;
int              scp_silent;
int              scp_raw_data[512*160/16];
int              scp_auto;
int              scp_nowrite;

// Misc
int              ipass;

// Pattern cells
int              icell;
int              ihit;
int              ihitp;

int              nhits;
int              layer;

int              ikey;
int              ikeylp;
int              ikey_min;
int              ikey_max;
int              ikey_sep;

int              ipid;
int              ipidlp;
int              ipid_min;
int              ipid_max;

int              icfeb;
int              icfebg;
int              key;
int              ihs[6][160];
int              nstag;

int              idistrip;
int              idslocal;
int              itbin;
int              itbin0;
int              ihstrip;
int              iram;
//int                   pat_ram[32][3][5]={0};
int              pat_ram[32][3][5];
int              wen;
int              ren;
int              wadr;
int              febsel;
int              icfeblp;
int              ibit;
int              wr_data_mem;
int              rd_data_mem;

std::string      marker;

int             	alct_injector_delay; // experimental

int              alct0_rd;
int              alct1_rd;
int              alct0_prev;
int              alct1_prev;

// crc checks
long int         din;
long int         crc;
long int         tmb_crc_lsb;
long int         tmb_crc_msb;
long int         tmb_crc;
bool             crc_match;

int              dmb_wdcnt;
int              dmb_busy;
int              dmb_rdata;
int              dmb_rdata_lsb;
int              dmb_rdata_msb;

int              nlayers_hit;
int              scint_veto;

int              alct0_inj_rd;
int              alct1_inj_rd;
int              alct0_inj_wr;
int              alct1_inj_wr;

int              alct0_vpf_inj;
int              alct0_qual_inj;
int              alct0_amu_inj;
int              alct0_key_inj;
int              alct0_bxn_inj;

int              alct1_vpf_inj;
int              alct1_qual_inj;
int              alct1_amu_inj;
int              alct1_key_inj;
int              alct1_bxn_inj;

int              alct0_vpf;
int              alct0_qual;
int              alct0_amu;
int              alct0_key;
int              alct0_bxn;

int              alct1_vpf;
int              alct1_qual;
int              alct1_amu;
int              alct1_key;
int              alct1_bxn;

int              vf_data[mxframe];
int              iframe;
int              ilayer;

int              adr_e0b;
int              r_nheaders;
int              r_ncfebs;
int              r_fifo_tbins;

int              read_pat[mxtbins][mxly][mxdsabs];
int              rdcid;
int              rdtbin;
int              hits8;
int              hits1;
int              ids;
int              ids_abs;
int              jcfeb;
bool             display_cfeb;
int              clct_bxn_expect;

int              clct_key_expect[mxclcts];
int              clct_pid_expect[mxclcts];
int              clct_hit_expect[mxclcts];
int              hit_thresh_pretrig_temp;
int              hit_thresh_postdrift_temp;

int              fmm_state;
std::string      sfmm_state[5];

bool             rdraw;
int              nbxn0;
int              ntrig;

int              alct_raw_busy;
int              alct_raw_done;
int              alct_raw_nwords;
int              alct_raw_data;

int              alct0_raw_lsb;
int              alct0_raw_msb;
int              alct0_raw;

int              alct1_raw_lsb;
int              alct1_raw_msb;
int              alct1_raw;

int              seqdeb_adr;
int              seqdeb_rd_mux;
unsigned long    deb_adr_diff;
unsigned long    deb_wr_buf_adr;
unsigned long    deb_buf_push_adr;
unsigned long    deb_buf_pop_adr;
unsigned long    deb_buf_push_data;
unsigned long    deb_buf_pop_data;

int              radix;
FILE*            ram_file;
std::string      ram_file_name;
int              iadr;
int              blue_flash;
bool             fire_injector;
bool             err_check;

unsigned short   wr_fire_l1a;

int              l1a_lookback;
int              hdr_wr_continuous;
int              lookback_triad_hits[2048];

int              first_nonzero_bx;
int              last_nonzero_bx;
int              first_bx;
int              last_bx;
int              max_triads;
double           scale;
int              tck;

// Miniscope
int             	mini_read_enable;
int             	mini_test;
int             	mini_tbins_word;
int             	fifo_pretrig_mini;
int             	csc_id;
int              maxv;
int              minv;
char             symbol;
int              bcb_read_enable;

// Pattern_finder
int              cfeb_en[5];
int              hs_key_1st_emu;
int              hs_pid_1st_emu;
int              hs_hit_1st_emu;

int              hs_key_2nd_emu;
int              hs_pid_2nd_emu;
int              hs_hit_2nd_emu;

int              nlayers_hit_emu;
int              layer_trig_emu;
int              cfeb_active_emu[5];

bool             emulator_pretrig;
bool             emulator_latch_clct0;
bool             emulator_latch_clct1;
bool             send_emulator_to_tmb;

int              clct0_vpf_emu;
int              clct0_hit_emu;
int              clct0_pid_emu;
int              clct0_key_emu;
int              clct0_cfeb_emu;
int              clctc_sync_emu;

int              clct1_vpf_emu;
int              clct1_hit_emu;
int              clct1_pid_emu;
int              clct1_key_emu;
int              clct1_cfeb_emu;

int              clct0_vpf_vme;
int              clct0_hit_vme;
int              clct0_pid_vme;
int              clct0_key_vme;
int              clct0_cfeb_vme;

int              clct1_vpf_vme;
int              clct1_hit_vme;
int              clct1_pid_vme;
int              clct1_key_vme;
int              clct1_cfeb_vme;

int              clctc_bxn_vme;
int              clctc_sync_vme;

int              nspan;
int              pspan;
int              busy_min;
int              busy_max;
bool             clct0_is_on_me1a;
int              key_inj;
int              busy_key_inj[160];

int              clct_hit_inj_expect[2];
int              clct_key_inj_expect[2];
int              clct_pid_inj_expect[2];
int              clct_pat_inj_expect[2];

bool             injector_clct0_over;
bool             injector_clct1_over;
bool             injector_pretrig;
bool             injector_latch_clct0;
bool             injector_latch_clct1;

int              clct0_vpf_vme_expect;
int              clct0_hit_vme_expect;
int              clct0_pid_vme_expect;
int              clct0_key_vme_expect;
int              clct0_cfeb_vme_expect;
int              clctc_sync_vme_expect;

int              clct1_vpf_vme_expect;
int              clct1_hit_vme_expect;
int              clct1_pid_vme_expect;
int              clct1_key_vme_expect;
int              clct1_cfeb_vme_expect;

int              clct0_vpf_tmb;
int              clct0_hit_tmb;
int              clct0_pid_tmb;
int              clct0_key_tmb;
int              clct0_cfeb_tmb;

int              clct1_vpf_tmb;
int              clct1_hit_tmb;
int              clct1_pid_tmb;
int              clct1_key_tmb;
int              clct1_cfeb_tmb;

int              clctc_bxn_tmb;
int              clctc_sync_err_tmb;

int              key0;
int              key1;

bool             kill_me1a_clcts;
bool             clct0_exists;
bool             clct1_exists;
bool             clct0_cfeb4;
bool             clct1_cfeb4;
bool             kill_clct0;
bool             kill_clct1;
bool             kill_trig;
bool             bans;

bool             clct_noalct_lost;
bool             alct_only_trig;

bool             clct_keep;
bool             alct_keep;

bool             clct_keep_ro;
bool             alct_keep_ro;

bool             clct_discard;
bool             alct_discard;

bool             clct_match;
bool             clct_noalct;
bool             alct_noclct;
bool             alct_pulse;

bool             trig_pulse;
bool             trig_keep;
bool             non_trig_keep;
bool             alct_only;

bool             clct_match_tr;
bool             alct_noclct_tr;
bool             clct_noalct_tr;
bool             clct_match_ro;
bool             alct_noclct_ro;
bool             clct_noalct_ro;

bool             tmb_trig_pulse;
bool             tmb_trig_keep_ff;
bool             tmb_non_trig_keep_ff;

bool             tmb_match;
bool             tmb_alct_only;
bool             tmb_clct_only;

bool             tmb_match_ro_ff;
bool             tmb_alct_only_ro_ff;
bool             tmb_clct_only_ro_ff;

bool             tmb_alct_discard;
bool             tmb_clct_discard;

int              tmb_alct0;
int              tmb_alct1;
int              alct0_pipe;
int              alct1_pipe;
int              alct0_real;
int              alct1_real;

int              keep_clct;
int              clct0_real;
int              clct0_pipe;
int              clct1_real;
int              clct1_pipe;
int              clctc_real;
int              clctc_pipe;

bool             tmb_match_ro;
bool             tmb_alct_only_ro;
bool             tmb_clct_only_ro;

bool             tmb_trig_keep;
bool             tmb_non_trig_keep;

int              tmb_no_alct_emu;
int              tmb_no_clct_emu;
int              tmb_one_alct_emu;
int              tmb_one_clct_emu;
int              tmb_two_alct_emu;
int              tmb_two_clct_emu;
int              tmb_dupe_alct_emu;
int              tmb_dupe_clct_emu;

int              alct_dummy;
int              clct_dummy;
int              clctc_dummy;
int              clct_bxn_insert_emu;

int              clct0_emu;
int              clct1_emu;
int              clctc_emu;

int              alct0_emu;
int              alct1_emu;

int              lct0_vpf_emu;
int              lct1_vpf_emu;

int              alct0_vpf_emu;
int              alct0_valid_emu;
int              alct0_quality_emu;
int              alct0_amu_emu;
int              alct0_key_emu;
int              alct0_bxn_emu;

int              alct1_vpf_emu;
int              alct1_valid_emu;
int              alct1_quality_emu;
int              alct1_amu_emu;
int              alct1_key_emu;
int              alct1_bxn_emu;

int              clct0_valid_emu;
int              clct0_bend_emu;

int              clct1_valid_emu;
int              clct1_bend_emu;

int              clct_bxn_emu;
int              clct_sync_err_emu;

int              alct0_hit_emu;
int              alct1_hit_emu;

int              clct0_cpat_emu;
int              clct1_cpat_emu;

int              ACC;
int              A;
int              C;
int              A4;
int              C4;
int              P;
int              CPAT;
int              Q;

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

int              mpc_alct0_key_vme;
int              mpc_clct0_pid_vme;
int              mpc_lct0_qual_vme;
int              mpc_lct0_vpf_vme;

int              mpc_bx0_clct_vme;
int              mpc_clct0_key_vme;
int              mpc_sync_err0_vme;
int              mpc_csc_id0_vme;
int              mpc_clct0_bend_vme;
int              mpc_alct0_bxn_vme;

int              mpc_alct1_key_vme;
int              mpc_clct1_pid_vme;
int              mpc_lct1_qual_vme;
int              mpc_lct1_vpf_vme;

int              mpc_clct1_key_vme;
int              mpc_clct1_bend_vme;
int              mpc_sync_err1_vme;
int              mpc_alct1_bxn_vme;
int              mpc_bx0_alct_vme;
int              mpc_csc_id1_vme;

int              lct0_vme;
int              lct1_vme;

int              ihs_min;
int              ihs_max;
int              ihs_hit;
int              ihs_ds;

int              sync_err; 
int              clct_bx0_sync_err; 
int              alct_ecc_rx_err_ff; 
int              alct_ecc_tx_err_ff; 
int              bx0_match_err_ff; 
int              sync_err_force; 

bool             load_clct_injector_image;
int              dscfeb[mxcfeb];

int              mask_all;
int              mask_cfeb;
int              inj_trig;
int              inj_busy;
bool             engage_pattern_finder;

int              wr_data_lsb;
int              wr_data_msb;
int              rd_data_lsb;
int              rd_data_msb;
int              l1a_lookback_data;
//------------------------------------------------------------------------------

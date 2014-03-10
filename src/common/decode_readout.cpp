//------------------------------------------------------------------------------
// Decode TMB2005 raw hits dump
//
//	10/02/08 Port from fortran version
//	10/24/08 Add ability to decode any header type + check readout structure
//	11/04/08 Replace c character strings with c++ strings
//	11/09/08 Mod for vmetst_v7_c call
//	11/10/08 Corrected tbins expected, TMB no longer merges tbin bits 3,4
//	11/12/08 Fix l1a counter check
//	12/05/08 Add err check bypass for playback and l1a-only events
//	05/01/09 Add miniscope
//	10/07/09 Fix fifo modes
//	10/19/09 Add duplication checks
//	10/22/09 Mod dup checks
//	01/15/10 Update header 30
//	01/29/10 Trim mpc_alct0_bxn prints to 1 digit
//	02/03/10 Mods for me1a/b cscs
//	02/11/10 Mods for type b csc
//	03/09/10 Add blocked bits unpacking
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Headers
//------------------------------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <cstring>

//------------------------------------------------------------------------------
// Local Headers
//------------------------------------------------------------------------------
#include "emu/pc/pause.h"
#include "emu/pc/scope160c.h"
#include "emu/pc/service.h"
#include "emu/pc/crc22.h"
#include "emu/pc/miniscope16.h"
#include "emu/pc/common.h"

//------------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Entry decode_raw_hits()
//------------------------------------------------------------------------------
void decode_readout(int	vf_data[mxframe],int &dmb_wdcnt, bool &err_check) {
    bool			header_ok;
    bool			header_only_short;
    bool			header_only_long;
    bool			header_full;
    bool			header_filler;
    static bool		first_entry=true;

    const int		wdcnt_short_hdr=12;
    const int		wdcnt_long_hdr=48;

    int				adr_b0c;
    int				adr_e0b;
    int				adr_b04;
    int				adr_e04;
    int				adr_b05;
    int				adr_e05;
    int				adr_b07;
    int				adr_e07;
    int				adr_bcb;
    int				adr_ecb;
    int				adr_e0c;
    int				adr_fil;
    int				adr_e0f;

    // Local
    int				i;
    int				ipair;
    int				last_frame;

    long int		din;
    long int		crc;
    long int		crc_calc;
    long int		tmb_crc_lsb;
    long int		tmb_crc_msb;
    long int		tmb_crc;
    bool			crc_match;

    int				dmb_wdcnt_trun;
    //int				fifo_cfeb_enable;
    int				frame_cnt_expect;
    int				frame_cnt_expect_trun;
    int				frame_cntex_nheaders;
    int				frame_cntex_ntbins;
    int				frame_cntex_b0ce0c;
    int				frame_cntex_rpc;
    int				frame_cntex_b04e04;
    int				frame_cntex_scope;
    int				frame_cntex_b05e05;
    int				frame_cntex_miniscope;
    int				frame_cntex_b07e07;
    int				frame_cntex_bcbecb;
    int				frame_cntex_blockedbits;
    int				frame_cntex_fill;
    int				frame_cntex_trailer;

    int				clct0;
    int				clct0_vpf;
    int				clct0_nhit;
    int				clct0_pat;
    int				clct0_key;
    int				clct0_cfeb;
    int				clct0_fullkey;

    int				clct1;
    int				clct1_vpf;
    int				clct1_nhit;
    int				clct1_pat;
    int				clct1_key;
    int				clct1_cfeb;
    int				clct1_fullkey;

    int				clctc;
    int				clctc_bxn;
    int				clctc_sync;

    int				mpc0_frame0;
    int				mpc0_frame1;
    int				mpc1_frame0;
    int				mpc1_frame1;

    int				mpc_alct0_key;
    int				mpc_clct0_pat;
    int				mpc_lct0_quality;
    int				mpc_lct0_vpf;

    int				mpc_clct0_key;
    int				mpc_clct0_bend;
    int				mpc_sync_err0;
    int				mpc_alct0_bxn;
    int				mpc_bx0_clct;
    int				mpc_csc_id0;

    int				mpc_alct1_key;
    int				mpc_clct1_pat;
    int				mpc_lct1_quality;
    int				mpc_lct1_vpf;

    int				mpc_clct1_key;
    int				mpc_clct1_bend;
    int				mpc_sync_err1;
    int				mpc_alct1_bxn;
    int				mpc_bx0_alct;
    int				mpc_csc_id1;

    int				mpc_clct0_key_expect;
    int				mpc_clct0_pat_expect;
    int				mpc_alct0_key_expect;

    int				mpc_clct1_key_expect;
    int				mpc_clct1_pat_expect;
    int				mpc_alct1_key_expect;

    // Local
    int				icfeb;
    int				itbin;
    int				ilayer;

    int				rdcid;
    int				rdtbin;
    int				itbin_expect;	
    int				hits1;
    int				hits8;
    int				ids;
    int				ids_abs;
    int				read_pat[mxtbins][mxly][mxdsabs];
    bool			triad_skipped;

    char			x[]="          ";

    int				active_febs_expect;
    int				active_febs_flipped;
    int				clctc_bxn_vme;
    int				clctc_bxn_header;
    static int		l1a_rxcount_save;
    int				l1a_rxcount_expect;
    static int		trig_counter_save;
    int				trig_counter_expect;
    int				l1a_type_expect;

    int				rpc_data0;
    int				rpc_data1;
    int				rpc_lsbs;
    int				rpc_tbin;
    int				rpc_msbs;
    int				rpc_bxn;
    int				rpc_flag;
    int				rpc_data;
    int				rpc_id0;
    int				rpc_id1;

    int				bd_status_vec[15];
    int				lhc_cycle=3564;

    string			sfifo_mode;
    string			sreadout_type;
    string			sl1a_type;
    string			scsc_type_inferred;
    char			csc_type_code;

    int				id_rev;
    int				id_rev_day;
    int				id_rev_month;
    int				id_rev_year;
    int				id_rev_fpga;

    int				pretrig_counter;
    int				clct_counter;
    int				trig_counter;
    int				alct_counter;
    int				uptime_counter;
    int				uptime_sec;

    bool			triad_error;
    int				triad_read;
    int				triad_write;

    int				wrtbin;
    //int				ntbins;
    int				ntbinspre;

    int				wdcnt;
    int				data;
    int				first_word;
    int				last_word;

    int				first_rpc_frame;
    int				last_rpc_frame;

    int				ncfebs_met;
    int				icfeb_included;
    bool			non_trig_event;
    bool			non_trig_override;

    int				first_bcb_frame;
    int				last_bcb_frame;
    int				bcb_data[4];
    int				bcb_cfebid[4];
    int				bcb_cfeb_ly[5][6];
    int				blocked_distrips[5][6][8];

    int				hdr_one_alct;
    int				hdr_one_clct;
    int				hdr_two_alct;
    int				hdr_two_clct;
    int				hdr_dupe_alct;
    int				hdr_dupe_clct;

    int				iscp_begin;
    int				iscp_end;
    int				iscp;
    int				scp_arm;
    int				scp_readout;
    int				scp_raw_decode;
    int				scp_silent;
    int				scp_raw_data[512*160/16-1];	//512tbins*160ch/(16ch/frame)
    int				miniscope_data[2048];

    //---------------------------------------------------------------------------------
    //	Error flags
    //---------------------------------------------------------------------------------
    const	int		MXERF=39+1;
    char			error_flag[MXERF];
    static	string	error_msg[MXERF];

    if(first_entry) {
        error_msg[ 0] = "0xDB0C first frame marker not found";
        error_msg[ 1] = "0xDE0C or 0xDEEF last frame marker not found";
        error_msg[ 2] = "Calculated word count does not match caller";
        error_msg[ 3] = "Word count <=0";
        error_msg[ 4] = "Word count not a multiple of 4";
        error_msg[ 5] = "Wordcount does not match short or long header format";
        error_msg[ 6] = "CRC error =  embedded does not match calculated";
        error_msg[ 7] = "First frame missing 0xDB0C marker";
        error_msg[ 8] = "First 4 frames do not have DDU-special codes";
        error_msg[ 9] = "Last  4 frames do not have DDU-special codes";
        error_msg[10] = "Last frame-3 missing EEF marker";
        error_msg[11] = "Last frame-3 missing E0F marker";
        error_msg[12] = "Last 4 frames missing bit[11]=1";
        error_msg[13] = "Fifo mode does not match short header format";
        error_msg[14] = "Readout type does not match short header format";
        error_msg[15] = "Fifo mode does not match long  header format";
        error_msg[16] = "Readout type does not match long header format";
        error_msg[17] = "Readout type does not match full header format";
        error_msg[18] = "Readout type does not match FIFO mode";
        error_msg[19] = "Expected frame count does not match actual frame count";
        error_msg[20] = "E0B marker not found at expected address";
        error_msg[21] = "B04 marker not found at expected address";
        error_msg[22] = "E04 marker not found at expected address";
        error_msg[23] = "B05 marker not found at expected address";
        error_msg[24] = "E05 marker not found at expected address";
        error_msg[25] = "E0C marker not found at expected address";
        error_msg[26] = "2AAA filler not found at expected address";
        error_msg[27] = "5555 filler not found at expected address";
        error_msg[28] = "Bad board hardware status";
        error_msg[29] = "Clock bx0 sync error";
        error_msg[30] = "RAM SEU parity error";
        error_msg[31] = "CLCT0 key does not match LCT0";
        error_msg[32] = "CLCT0 pattern ID does not match LCT0";
        error_msg[33] = "ALCT0 key does not match LCT0";
        error_msg[34] = "CLCT1 key does not match LCT1";
        error_msg[35] = "CLCT1 pattern ID does not match LCT1";
        error_msg[36] = "ALCT1 key does not match LCT1";
        error_msg[37] = "Expected number of CFEBs 0<r_ncfebs<=5";
        error_msg[38] = "Expected number of CFEB tbins 0<r_fifo_tbins";
        error_msg[39] = "Active CFEB list to DMB does not match CFEBs read out";
    }

    //---------------------------------------------------------------------------------
    //	Board status
    //---------------------------------------------------------------------------------
    const	int		MXSTAT=14+1;
    static	string	bd_status_msg[MXSTAT];
    int				bd_status_expect[MXSTAT];

    if(first_entry) {
        bd_status_msg[ 0] = "bd_status_ok  ";
        bd_status_msg[ 1] = "vstat_5p0vs   ";
        bd_status_msg[ 2] = "vstat_3p3v    ";
        bd_status_msg[ 3] = "vstat_1p8v    ";
        bd_status_msg[ 4] = "vstat_1p5v    ";
        bd_status_msg[ 5] = "_t_crit       ";
        bd_status_msg[ 6] = "vsm_ok        ";
        bd_status_msg[ 7] = "vsm_aborted   ";
        bd_status_msg[ 8] = "vsm_cksum_ok  ";
        bd_status_msg[ 9] = "vsm_wdcnt_ok  ";
        bd_status_msg[10] = "jsm_ok        ";
        bd_status_msg[11] = "jsm_aborted   ";
        bd_status_msg[12] = "jsm_cksum_ok  ";
        bd_status_msg[13] = "jsm_wdcnt_ok  ";
        bd_status_msg[14] = "sm_tck_fpga_ok";
    }
    //---------------------------------------------------------------------------------
    //	Header data types
    //---------------------------------------------------------------------------------
    int			ddu[mxframe];
    int			iframe;

    int			boc;
    int			pop_l1a_bxn;
    int			pop_l1a_rx_counter;
    int			readout_counter;
    int			board_id;
    int			csc_id;
    int			run_id;
    int			h4_buf_q_ovf_err;
    int			r_sync_err;

    int			r_nheaders;
    int			fifo_mode;
    int			readout_type;
    int			l1a_type;
    int			r_has_buf;
    int			r_buf_stalled;
    int			bd_status;
    int			revcode;

    int			r_bxn_counter_ff;
    int			r_tmb_clct0_discard;
    int			r_tmb_clct1_discard;
    int			clock_lock_lost;

    int			r_pretrig_counter_lsb;
    int			r_pretrig_counter_msb;

    int			r_clct_counter_lsb;
    int			r_clct_counter_msb;

    int			r_trig_counter_lsb;
    int			r_trig_counter_msb;

    int			r_alct_counter_lsb;
    int			r_alct_counter_msb;

    int			r_orbit_counter_lsb;
    int			r_orbit_counter_msb;

    int			r_ncfebs;
    int			r_fifo_tbins;
    int			fifo_pretrig;
    int			scope_exists;
    int			miniscope_exists;

    int			hit_thresh_pretrig;
    int			pid_thresh_pretrig;
    int			hit_thresh_postdrf;
    int			pid_thresh_postdrf;
    int			stagger_csc;
    //	int			csc_me1ab;

    int			triad_persist;
    int			dmb_thresh;
    int			alct_delay;
    int			clct_window;

    // CLCT Trigger Status
    int			r_trig_source_vec;
    int			r_trig_source_vec9;
    int			r_trig_source_vec10;
    int			r_layers_hit;

    int			r_active_feb_ff;
    int			r_febs_read;
    int			r_l1a_match_win;
    int			hs_layer_trig;
    int			active_feb_src;

    // CLCT+ALCT Match Status
    int			r_tmb_match;
    int			r_tmb_alct_only;
    int			r_tmb_clct_only;
    int			r_tmb_match_win;
    int			r_no_alct_tmb;
    int			r_one_alct_tmb;
    int			r_one_clct_tmb;
    int			r_two_alct_tmb;
    int			r_two_clct_tmb;
    int			r_dupe_alct_tmb;
    int			r_dupe_clct_tmb;
    int			r_rank_err_tmb;

    // CLCT Trigger Data
    int			r_clct0_tmb_lsb;
    int			r_clct1_tmb_lsb;
    int			r_clct0_tmb_msb;
    int			r_clct1_tmb_msb;
    int			r_clctc_tmb;
    int			r_clct0_invp;
    int			r_clct1_invp;
    int			r_clct1_busy;
    int			perr_cfeb_ff;
    int			perr_rpc_ff;
    int			perr_ff;

    // ALCT Trigger Data
    int			r_alct0_valid;
    int			r_alct0_quality;
    int			r_alct0_amu;
    int			r_alct0_key;
    int			r_alct_pretrig_win;

    int			r_alct1_valid;
    int			r_alct1_quality;
    int			r_alct1_amu;
    int			r_alct1_key;
    int			drift_delay;
    int			h_bcb_read_enable;

    int			r_alct_bxn;
    int			alct_ecc_err;
    int			cfeb_badbits_found;
    int			cfeb_badbits_blocked;
    int			alct_cfg_done;
    int			bx0_match;

    // MPC Frames
    int			r_mpc0_frame0_lsb;
    int			r_mpc0_frame1_lsb;
    int			r_mpc1_frame0_lsb;
    int			r_mpc1_frame1_lsb;
    int			r_mpc0_frame0_msb;
    int			r_mpc0_frame1_msb;
    int			r_mpc1_frame0_msb;
    int			r_mpc1_frame1_msb;
    int			mpc_tx_delay;
    int			r_mpc_accept;
    int			cfeb_en;

    // RPC Configuration
    int			rd_rpc_list;
    int			rd_nrpcs;
    int			rpc_read_enable;
    int			fifo_tbins_rpc;
    int			fifo_pretrig_rpc;

    // Buffer Status
    int			r_wr_buf_adr;
    int			r_wr_buf_ready;
    int			wr_buf_ready;
    int			buf_q_full;
    int			buf_q_empty;

    int			r_buf_fence_dist;
    int			buf_q_ovf_err;
    int			buf_q_udf_err;
    int			buf_q_adr_err;
    int			buf_stalled_once;

    // Spare Frames
    int			buf_fence_cnt;
    int			reverse_hs_csc;
    int			reverse_hs_me1a;
    int			reverse_hs_me1b;

    int			buf_fence_cnt_peak;
    int			tmb_trig_pulse;

    int			tmb_allow_alct;
    int			tmb_allow_clct;
    int			tmb_allow_match;
    int			tmb_allow_alct_ro;
    int			tmb_allow_clct_ro;
    int			tmb_allow_match_ro;
    int			tmb_alct_ro;
    int			tmb_clct_ro;
    int			tmb_match_ro;
    int			tmb_trig_keep;
    int			tmb_nontrig_keep;
    int			lyr_thresh_pretrig;
    int			layer_trig_en;

    // E0B marker
    int			eob;
    int			eoc;

    // Optional 2 frames to make word count a multiple of 4
    int			opt2aaa;
    int			opt5555;

    // Last 4 trailer words must conform to DDU specification
    int			eof;
    int			lctype3;
    int			crc22lsb;
    int			lctype2;
    int			crc22msb;
    int			lctype1;
    int			frame_cnt;
    int			lctype0;

    //---------------------------------------------------------------------------------
    //	Entry
    //---------------------------------------------------------------------------------
    fprintf(stdout,"\n");

    // Init check bits
    first_entry       = false;
    header_ok         = true;
    header_only_short = false;
    header_only_long  = false;
    header_full       = false;
    header_filler     = false;

    // Clear error flags
    for (i=0; i<MXERF; ++i) 
        error_flag[i]=0;

    //---------------------------------------------------------------------------------
    //  Scan incoming data stream for begin DB0C and end E0F (or DEEF) markers
    //---------------------------------------------------------------------------------
    wdcnt      =  0;
    data       =  0;
    first_word = -1;
    last_word  = -1;

    // Scan for DB0C and DE0F/DEEF markers, DEEF might be crc in full dump, nb Dxxx markers are not unique, could be bxn or wdcnt
    for (i=0; i<mxframe; ++i) {
        data=vf_data[i]&0xFFFF;		// Trim out-of band bits
        fprintf(stdout,"Adr=%5i Data=%6.5X\n",i,data);
        if ((data==0xDB0C) && (first_word < 0))	first_word=i;
        if (((data==0xDE0F) || (data==0xDEEF)) && (i > first_word+3) && (last_word < 0)) last_word=i+3;
        if (i==last_word) break;
    }

    last_frame=last_word;

    // Did not find DB0C
    if(first_word==-1) {
        header_ok=false;
        error_flag[0]=1;	 // 0xDB0C first frame marker not found
        fprintf(stdout,"ERRs: 0xDB0C first frame marker not found\n");
    }
    else
        fprintf(stdout,"First frame marker found at Adr=%5i Data=%6.5X\n",first_word,vf_data[first_word]);

    // Did not find DE0F/DEEF
    if(last_word ==-1) {
        header_ok=false;
        error_flag[1]=1;	 // 0xDE0C or 0xDEEF last frame marker not found
        fprintf(stdout,"ERRs: 0xDE0C or 0xDEEF last frame marker not found\n");
    }
    else
        fprintf(stdout,"Last  frame marker found at Adr=%5i Data=%6.5X\n",last_word,vf_data[last_word]);

    // Compare word count to callers value [caller may not have supplied a word count]
    wdcnt=1+last_word-first_word;

    fprintf(stdout,"Calculated word count=%5i\n",wdcnt);
    fprintf(stdout,"Callers    word count=%5i\n",dmb_wdcnt);

    if(wdcnt != dmb_wdcnt) {
        header_ok=false;
        error_flag[2]=1;	 // Calculated word count does not match caller
        fprintf(stdout,"ERRs: Calculated word count does not match caller: wdcnt=%5i dmb_wdcnt=%5i\n",wdcnt,dmb_wdcnt);
    }

    //	dmb_wdcnt=wdcnt;	// Uncomment if caller did not supply a word count

    //---------------------------------------------------------------------------------
    //	Check word count matches either short,long or full mode, and is a multiple of 4
    //---------------------------------------------------------------------------------
    // No TMB data
    if(dmb_wdcnt <= 0) {
        header_ok=false;
        error_flag[3]=1;	// Wordcount <=0
        fprintf(stdout,"ERRs: No TMB readout to decode. wdcnt=%i\n",dmb_wdcnt);
        goto exit;
    }

    // Check that wdcnt is a multiple of 4, all TMB readouts are mod(4) for DDU error checking
    if(dmb_wdcnt%4 != 0) {
        header_ok=false;
        error_flag[4]=1;	// Wordcount not multiple of 4
        fprintf(stdout,"ERRs: TMB wdcnt is not a multiple of 4. wdcnt=%i\n",dmb_wdcnt);
    }
    fprintf(stdout,"TMB wdcnt is a multiple of 4. wdcnt=%i\n",dmb_wdcnt);

    // Short header-only mode
    if(dmb_wdcnt == wdcnt_short_hdr) {
        header_only_short=true;
        fprintf(stdout,"TMB readout short header-only mode. wdcnt=%i\n",dmb_wdcnt);
    }

    // Long header-only mode
    if(dmb_wdcnt == wdcnt_long_hdr) {
        header_only_long=true;
        fprintf(stdout,"TMB readout long header-only mode. wdcnt=%i\n",dmb_wdcnt);
    }

    // Long header with raw hits mode
    if(dmb_wdcnt > wdcnt_long_hdr) {
        header_full=true;
        fprintf(stdout,"TMB readout long header with raw hits mode. wdcnt=%i\n",dmb_wdcnt);
    }

    // Check that header format is recognized
    if (!(header_only_short || header_only_long || header_full)) {
        header_ok=false;
        error_flag[5]=1;	// Wordcount does not match short or long header format
        fprintf(stdout,"ERRs: TMB wdcnt does not match a defined header format. wdcnt=%i\n",dmb_wdcnt);
    }

    fprintf(stdout,"header_only_short = %c\n",logical(header_only_short));
    fprintf(stdout,"header_only_long  = %c\n",logical(header_only_long));
    fprintf(stdout,"header_full       = %c\n",logical(header_full));
    fprintf(stdout,"header_filler     = %c\n",logical(header_filler));

    //------------------------------------------------------------------------------
    //	Check CRC
    //------------------------------------------------------------------------------
    // Calculate CRC for data stream
    if(dmb_wdcnt < 12) {	// should not ever get here
        fprintf(stdout,"TMB raw hits dump too short for crc calculation, exiting.\n");
        pause("TMB raw hits dump too short for crc calculation");
        header_ok=false;
        goto exit;
    }

    crc=0;
    for (iframe=0; iframe<=dmb_wdcnt-1; ++iframe) {
        din=vf_data[iframe];
        din=din & 0xFFFF;
        if(iframe==0) crc22a(din,crc,1);				// Reset crc
        crc22a(din,crc,0);								// Calc  crc
        if(iframe==dmb_wdcnt-1-4) crc_calc=crc;			// Latch result prior to de0f marker beco ddu fails to process de0f frame
        fprintf(stdout,"%5i%6.5ld%9.8ld \n",iframe,din,crc);
    }

    // Compare our computed CRC to what TMB computed
    tmb_crc_lsb=vf_data[dmb_wdcnt-1-2] & 0x07FF;	// 11 crc bits per frame
    tmb_crc_msb=vf_data[dmb_wdcnt-1-1] & 0x07FF;	// 11 crc bits per frame

    tmb_crc=tmb_crc_lsb | (tmb_crc_msb << 11);		// Full 22 bit crc
    crc_match=crc_calc==tmb_crc;

    fprintf(stdout,"calc crc=%6.6ld ",crc_calc);
    fprintf(stdout,"tmb crc=%6.6ld ",tmb_crc);
    fprintf(stdout,"crc_match=%c\n",logical(crc_match));

    // CRC mismatch
    if(!crc_match) {
        header_ok=false;
        error_flag[6]=1;	// CRC error, embedded does not match calculated
        fprintf(stdout,"Expect vf_data[%i]=%5.5ld \n",(dmb_wdcnt-1-2),((crc_calc>> 0) & 0x07FF));	
        fprintf(stdout,"Expect vf_data[%i]=%5.5ld \n",(dmb_wdcnt-1-1),((crc_calc>>11) & 0x07FF));	
        pause ("TMB crc ERROR in decode_raw_hits, WTF!?");
    }

    //-------------------------------------------------------------------------------
    //	Unpack first 8 header frames, format is common to short, long, and full modes
    //-------------------------------------------------------------------------------
    // First 4 header words must conform to DDU specification
    iframe=0;
    boc=				(vf_data[iframe] >>  0) & 0xFFF;	// Beginning of Cathode record marker
    ddu[iframe]=		(vf_data[iframe] >> 12) & 0xF;		// DDU special

    iframe=1;
    pop_l1a_bxn=		(vf_data[iframe] >>  0) & 0xFFF;	// Bxn pushed on L1A stack at L1A arrival
    ddu[iframe]=		(vf_data[iframe] >> 12) & 0xF;		// DDU special

    iframe=2;
    pop_l1a_rx_counter=	(vf_data[iframe] >>  0) & 0xFFF;	// L1As received and pushed on L1A stack
    ddu[iframe]=		(vf_data[iframe] >> 12) & 0xF;		// DDU special

    iframe=3;
    readout_counter=	(vf_data[iframe] >>  0) & 0xFFF;	// Readout counter, same as l1a_rx_counter for now
    ddu[iframe]=		(vf_data[iframe] >> 12) & 0xF;		// DDU special

    // Next 4 words for short, long or full header modes
    iframe=4;
    board_id=			(vf_data[iframe] >>  0) & 0x1F;		// TMB module ID number = VME slot
    csc_id=				(vf_data[iframe] >>  5) & 0xF;		// Chamber ID number
    run_id=				(vf_data[iframe] >>  9) & 0xF;		// Run info
    h4_buf_q_ovf_err=	(vf_data[iframe] >> 13) & 0x1;		// Fence queue overflow error
    r_sync_err=			(vf_data[iframe] >> 14) & 0x1;		// BXN sync error
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=5;
    r_nheaders=			(vf_data[iframe] >>  0) & 0x3F;		// Number of header words
    fifo_mode=			(vf_data[iframe] >>  6) & 0x7;		// Trigger type and fifo mode
    readout_type=		(vf_data[iframe] >>  9) & 0x3;		// Readout type: dump,nodump, full header, short header
    l1a_type=			(vf_data[iframe] >> 11) & 0x3;		// L1A Pop type code: buffers, no buffers, clct/alct_only
    r_has_buf=			(vf_data[iframe] >> 13) & 0x1;		// Event has clct and rpc buffer data
    r_buf_stalled=		(vf_data[iframe] >> 14) & 0x1;		// Raw hits buffer was full at pretrigger
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=6;
    bd_status=			(vf_data[iframe] >>  0) & 0x7FFF;	// Board status summary
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=7;
    revcode=			(vf_data[iframe] >>  0) & 0x7FFF;	// Firmware version date code
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    // Short header-only mode ends here, next 4 frames should be trailer
    if (header_only_short) goto unpack_trailer;

    //---------------------------------------------------------------------------------------
    //	Unpack frames 8-47 for long header-only mode or long header with full raw hits readout
    //----------------------------------------------------------------------------------------
    iframe=8;
    r_bxn_counter_ff=	(vf_data[iframe] >>  0) & 0xFFF;	// Full CLCT Bunch Crossing number at pretrig
    r_tmb_clct0_discard=(vf_data[iframe] >> 12) & 0x1;		// TMB discarded CLCT0 from ME1A
    r_tmb_clct1_discard=(vf_data[iframe] >> 13) & 0x1;		// TMB discarded CLCT1 from ME1A
    clock_lock_lost=	(vf_data[iframe] >> 14) & 0x1;		// Clock lock lost
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=9;
    r_pretrig_counter_lsb=(vf_data[iframe] >>  0) & 0x7FFF;	// Counts CLCT pre-triggers [stops on ovf]
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=10;
    r_pretrig_counter_msb=(vf_data[iframe] >>  0) & 0x7FFF;	// Counts CLCT pre-triggers [stops on ovf]
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=11;
    r_clct_counter_lsb=	(vf_data[iframe] >>  0) & 0x7FFF;	// Counts CLCTs post-drift [stops on ovf]
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=12;
    r_clct_counter_msb=	(vf_data[iframe] >>  0) & 0x7FFF;	// Counts CLCTs post-drift [stops on ovf]
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=13;
    r_trig_counter_lsb=	(vf_data[iframe] >>  0) & 0x7FFF;	// Counts TMB triggers to MPC, L1A request to CCB,
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=14;
    r_trig_counter_msb=	(vf_data[iframe] >>  0) & 0x7FFF;	// Counts TMB triggers to MPC, L1A request to CCB,
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=15;
    r_alct_counter_lsb=	(vf_data[iframe] >>  0) & 0x7FFF;	// Counts ALCTs received from ALCT board [stops on ovf]
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=16;
    r_alct_counter_msb=	(vf_data[iframe] >>  0) & 0x7FFF;	// Counts ALCTs received from ALCT board [stops on ovf]
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=17;
    r_orbit_counter_lsb=(vf_data[iframe] >>  0) & 0x7FFF;	// BX0s since last hard reset [stops on ovf]
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=18;
    r_orbit_counter_msb=(vf_data[iframe] >>  0) & 0x7FFF;	// BX0s since last hard reset [stops on ovf]
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    // CLCT Raw Hits Size
    iframe=19;
    r_ncfebs=			(vf_data[iframe] >>  0) & 0x7;		// Number of CFEBs read out
    r_fifo_tbins=		(vf_data[iframe] >>  3) & 0x1F;		// Number of time bins per CFEB in dump
    fifo_pretrig=		(vf_data[iframe] >>  8) & 0x1F;		// # Time bins before pretrigger
    scope_exists=		(vf_data[iframe] >> 13) & 0x1;		// Readout includes logic analyzer scope data
    miniscope_exists=	(vf_data[iframe] >> 14) & 0x1;		// Readout includes miniscope data
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special
    if (r_fifo_tbins==0 && r_ncfebs!=0) r_fifo_tbins=32;	// TMB treats 0 tbins as 32 if cfeb readout is enabled

    // CLCT Configuration
    iframe=20;
    hit_thresh_pretrig=	(vf_data[iframe] >>  0) & 0x7;		// Hits on pattern template pre-trigger threshold
    pid_thresh_pretrig=	(vf_data[iframe] >>  3) & 0xF;		// Pattern shape ID pre-trigger threshold
    hit_thresh_postdrf=	(vf_data[iframe] >>  7) & 0x7;		// Hits on pattern post-drift threshold
    pid_thresh_postdrf=	(vf_data[iframe] >> 10) & 0xF;		// Pattern shape ID post-drift threshold
    stagger_csc=		(vf_data[iframe] >> 14) & 0x1;		// CSC Staggering ON
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=21;
    triad_persist=		(vf_data[iframe] >>  0) & 0xF;		// CLCT Triad persistence
    dmb_thresh=			(vf_data[iframe] >>  4) & 0x7;		// DMB pre-trigger threshold for active-feb
    alct_delay=			(vf_data[iframe] >>  7) & 0xF;		// Delay ALCT for CLCT match window
    clct_window=		(vf_data[iframe] >> 11) & 0xF;		// CLCT match window width
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    // CLCT Trigger Status
    iframe=22;
    r_trig_source_vec=	(vf_data[iframe] >>  0) & 0x1FF;	// Trigger source vector
    r_layers_hit=		(vf_data[iframe] >>  9) & 0x3F;		// CSC layers hit on layer trigger
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=23;
    r_active_feb_ff=	(vf_data[iframe] >>  0) & 0x1F;		// Active CFEB list sent to DMB
    r_febs_read=		(vf_data[iframe] >>  5) & 0x1F;		// CFEBs read out for this event
    r_l1a_match_win=	(vf_data[iframe] >> 10) & 0xF;		// Position of l1a in window
    active_feb_src=		(vf_data[iframe] >> 14) & 0x1;		// Active CFEB list source
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    // CLCT+ALCT Match Status
    iframe=24;
    r_tmb_match=		(vf_data[iframe] >>  0) & 0x1;		// ALCT and CLCT matched in time, pushed on L1A stack
    r_tmb_alct_only=	(vf_data[iframe] >>  1) & 0x1;		// Only ALCT triggered, pushed on L1a stack
    r_tmb_clct_only=	(vf_data[iframe] >>  2) & 0x1;		// Only CLCT triggered, pushed on L1A stack
    r_tmb_match_win=	(vf_data[iframe] >>  3) & 0xF;		// Location of alct in clct window, pushed on L1A stack
    r_no_alct_tmb=		(vf_data[iframe] >>  7) & 0x1;		// No ALCT
    r_one_alct_tmb=		(vf_data[iframe] >>  8) & 0x1;		// One ALCT
    r_one_clct_tmb=		(vf_data[iframe] >>  9) & 0x1;		// One CLCT
    r_two_alct_tmb=		(vf_data[iframe] >> 10) & 0x1;		// Two ALCTs
    r_two_clct_tmb=		(vf_data[iframe] >> 11) & 0x1;		// Two CLCTs
    r_dupe_alct_tmb=	(vf_data[iframe] >> 12) & 0x1;		// ALCT0 copied into ALCT1 to make 2nd LCT
    r_dupe_clct_tmb=	(vf_data[iframe] >> 13) & 0x1;		// CLCT0 copied into CLCT1 to make 2nd LCT
    r_rank_err_tmb=		(vf_data[iframe] >> 14) & 0x1;		// LCT1 has higher quality than LCT0
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    // CLCT Trigger Data
    iframe=25;
    r_clct0_tmb_lsb=	(vf_data[iframe] >>  0) & 0x7FFF;	// CLCT0 after drift lsbs
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=26;
    r_clct1_tmb_lsb=	(vf_data[iframe] >>  0) & 0x7FFF;	// CLCT1 after drift lsbs
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=27;
    r_clct0_tmb_msb=	(vf_data[iframe] >>  0) & 0x1;		// CLCT0 after drift msbs
    r_clct1_tmb_msb=	(vf_data[iframe] >>  1) & 0x1;		// CLCT1 after drift msbs
    r_clctc_tmb=		(vf_data[iframe] >>  2) & 0x7;		// CLCTC after drift
    r_clct0_invp=		(vf_data[iframe] >>  5) & 0x1;		// CLCT0 had invalid pattern after drift delay
    r_clct1_invp=		(vf_data[iframe] >>  6) & 0x1;		// CLCT1 had invalid pattern after drift delay
    r_clct1_busy=		(vf_data[iframe] >>  7) & 0x1;		// 2nd CLCT busy, logic error indicator
    perr_cfeb_ff=		(vf_data[iframe] >>  8) & 0x1F;		// CFEB parity error
    perr_rpc_ff=		(vf_data[iframe] >> 13) & 0x1;		// RPC  parity error
    perr_ff=			(vf_data[iframe] >> 14) & 0x1;		// Parity error summary
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    // ALCT Trigger Data
    iframe=28;
    r_alct0_valid=		(vf_data[iframe] >>  0) & 0x1;		// ALCT0 valid pattern flag
    r_alct0_quality=	(vf_data[iframe] >>  1) & 0x3;		// ALCT0 quality
    r_alct0_amu=		(vf_data[iframe] >>  3) & 0x1;		// ALCT0 accelerator muon flag
    r_alct0_key=		(vf_data[iframe] >>  4) & 0x7F;		// ALCT0 key wire group
    r_alct_pretrig_win=	(vf_data[iframe] >> 11) & 0xF;		// ALCT active_feb_flag position in pretrig window
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=29;
    r_alct1_valid=		(vf_data[iframe] >>  0) & 0x1;		// ALCT1 valid pattern flag
    r_alct1_quality=	(vf_data[iframe] >>  1) & 0x3;		// ALCT1 quality
    r_alct1_amu=		(vf_data[iframe] >>  3) & 0x1;		// ALCT1 accelerator muon flag
    r_alct1_key=		(vf_data[iframe] >>  4) & 0x7F;		// ALCT1 key wire group
    drift_delay=		(vf_data[iframe] >> 11) & 0x3F;		// CLCT drift delay
    h_bcb_read_enable=	(vf_data[iframe] >> 13) & 0x1;		// CFEB blocked bits included in readout
    hs_layer_trig=		(vf_data[iframe] >> 14) & 0x1;		// Layer-mode trigger
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=30;
    r_alct_bxn=			(vf_data[iframe] >>  0) & 0x1F;		// ALCT0/1 bxn
    alct_ecc_err=		(vf_data[iframe] >>  5) & 0x3;		// ALCT trigger path ECC error code
    cfeb_badbits_found=	(vf_data[iframe] >>  7) & 0x1F;		// Bad distrip bits detected in cfeb[n]
    cfeb_badbits_blocked=(vf_data[iframe] >> 12) & 0x1;		// At least one CFEB has a bad bit that was blocked
    alct_cfg_done=		(vf_data[iframe] >> 13) & 0x1;		// ALCT FPGA configuration done
    bx0_match=			(vf_data[iframe] >> 14) & 0x1;		// alct_bx0==clct_bx0, latched at clct_bx0 time
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    // MPC Frames
    iframe=31;
    r_mpc0_frame0_lsb=	(vf_data[iframe] >>  0) & 0x7FFF;	// MPC muon 0 frame 0 LSBs
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=32;
    r_mpc0_frame1_lsb=	(vf_data[iframe] >>  0) & 0x7FFF;	// MPC muon 0 frame 1 LSBs
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=33;
    r_mpc1_frame0_lsb=	(vf_data[iframe] >>  0) & 0x7FFF;	// MPC muon 1 frame 0 LSBs
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=34;
    r_mpc1_frame1_lsb=	(vf_data[iframe] >>  0) & 0x7FFF;	// MPC muon 1 frame 1 LSBs
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=35;
    r_mpc0_frame0_msb=	(vf_data[iframe] >>  0) & 0x1;		// MPC muon 0 frame 0 MSB
    r_mpc0_frame1_msb=	(vf_data[iframe] >>  1) & 0x1;		// MPC muon 0 frame 1 MSB
    r_mpc1_frame0_msb=	(vf_data[iframe] >>  2) & 0x1;		// MPC muon 1 frame 0 MSB
    r_mpc1_frame1_msb=	(vf_data[iframe] >>  3) & 0x1;		// MPC muon 1 frame 1 MSB
    mpc_tx_delay=		(vf_data[iframe] >>  4) & 0xF;		// MPC transmit delay
    r_mpc_accept=		(vf_data[iframe] >>  8) & 0x3;		// MPC muon accept response
    cfeb_en=			(vf_data[iframe] >> 10) & 0x1F;		// CFEBs enabled for triggering
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    // RPC Configuration
    iframe=36;
    rd_rpc_list=		(vf_data[iframe] >>  0) & 0x3;		// RPCs included in read out
    rd_nrpcs=			(vf_data[iframe] >>  2) & 0x3;		// Number of RPCs in readout, 0,1,2
    rpc_read_enable=	(vf_data[iframe] >>  4) & 0x1;		// RPC readout enabled
    fifo_tbins_rpc=		(vf_data[iframe] >>  5) & 0x1F;		// Number RPC FIFO time bins to read out
    fifo_pretrig_rpc=	(vf_data[iframe] >> 10) & 0x1F;		// Number RPC FIFO time bins before pretrigger
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special
    if (fifo_tbins_rpc==0 && rd_nrpcs!=0) fifo_tbins_rpc=32;// TMB treats 0 tbins as 32 if rpc readout is enabled

    // Buffer Status
    iframe=37;
    r_wr_buf_adr=		(vf_data[iframe] >>  0) & 0x7FF;	// Buffer RAM write address at pretrigger
    r_wr_buf_ready=		(vf_data[iframe] >> 11) & 0x1;		// Write buffer was ready at pretrig
    wr_buf_ready=		(vf_data[iframe] >> 12) & 0x1;		// Write buffer ready now
    buf_q_full=			(vf_data[iframe] >> 13) & 0x1;		// All raw hits ram in use, ram writing must stop
    buf_q_empty=		(vf_data[iframe] >> 14) & 0x1;		// No fences remain on buffer stack
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=38;
    r_buf_fence_dist=	(vf_data[iframe] >>  0) & 0x7FF;	// Distance to 1st fence address at pretrigger
    buf_q_ovf_err=		(vf_data[iframe] >> 11) & 0x1;		// Tried to push when stack full
    buf_q_udf_err=		(vf_data[iframe] >> 12) & 0x1;		// Tried to pop when stack empty
    buf_q_adr_err=		(vf_data[iframe] >> 13) & 0x1;		// Fence adr popped from stack doesnt match rls adr
    buf_stalled_once=	(vf_data[iframe] >> 14) & 0x1;		// Buffer stalled at least once
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    // Spare Frames
    iframe=39;
    buf_fence_cnt=		(vf_data[iframe] >>  0) & 0xFFF;	// Number of fences in fence RAM currently
    reverse_hs_csc=		(vf_data[iframe] >> 12) & 0x1;		// 1=Reverse staggered CSC, non-me1
    reverse_hs_me1a=	(vf_data[iframe] >> 13) & 0x1;		// 1=ME1A hstrip order reversed
    reverse_hs_me1b=	(vf_data[iframe] >> 14) & 0x1;		// 1=ME1B hstrip order reversed
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=40;
    buf_fence_cnt_peak=	(vf_data[iframe] >>  0) & 0xFFF;	// Peak number of fences in fence RAM
    r_trig_source_vec9=	(vf_data[iframe] >> 12) & 0x1;		// Pre-trigger was ME1A only
    r_trig_source_vec10=(vf_data[iframe] >> 13) & 0x1;		// Pre-trigger was ME1B only
    tmb_trig_pulse=		(vf_data[iframe] >> 14) & 0x1;		// TMB trig pulse
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=41;
    tmb_allow_alct=		(vf_data[iframe] >>  0) & 0x1;		// Allow ALCT-only  tmb-matching
    tmb_allow_clct=		(vf_data[iframe] >>  1) & 0x1;		// Allow CLCT-only  tmb-matching
    tmb_allow_match=	(vf_data[iframe] >>  2) & 0x1;		// Allow Match-only tmb-matching
    tmb_allow_alct_ro=	(vf_data[iframe] >>  3) & 0x1;		// Allow ALCT-only  tmb-matching, nontrig readout
    tmb_allow_clct_ro=	(vf_data[iframe] >>  4) & 0x1;		// Allow CLCT-only  tmb-matching, nontrig readout
    tmb_allow_match_ro=	(vf_data[iframe] >>  5) & 0x1;		// Allow Match-only tmb-matching, nontrig readout
    tmb_alct_ro=		(vf_data[iframe] >>  6) & 0x1;		// Allow ALCT-only  tmb-matching, nontrig readout
    tmb_clct_ro=		(vf_data[iframe] >>  7) & 0x1;		// Allow CLCT-only  tmb-matching, nontrig readout
    tmb_match_ro=		(vf_data[iframe] >>  8) & 0x1;		// Allow Match-only tmb-matching, nontrig readout
    tmb_trig_keep=		(vf_data[iframe] >>  9) & 0x1;		// Triggering readout
    tmb_nontrig_keep=	(vf_data[iframe] >> 10) & 0x1;		// Non-triggering readout
    lyr_thresh_pretrig=	(vf_data[iframe] >> 11) & 0x7;		// Layer pre-trigger threshold
    layer_trig_en=		(vf_data[iframe] >> 14) & 0x1;		// Layer-trigger mode enabled
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    // EOB marker
    iframe=42;
    eob=				(vf_data[iframe] >>  0) & 0x7FFF;	// EOB
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    // EOC marker
    iframe=last_frame-6;
    eoc=				(vf_data[iframe] >>  0) & 0x7FFF;
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    //------------------------------------------------------------------------------
    // Unpack optional 2 filler frames to make word count a multiple of 4
    //------------------------------------------------------------------------------
    iframe=last_frame-5;
    opt2aaa=			(vf_data[iframe] >>  0) & 0x7FFF;
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    iframe=last_frame-4;
    opt5555=			(vf_data[iframe] >>  0) & 0x7FFF;
    ddu[iframe]=		(vf_data[iframe] >> 15) & 0x1;		// DDU special

    //------------------------------------------------------------------------------
    //	Unpack trailer frames: Last 4 trailer words must conform to DDU specification
    //------------------------------------------------------------------------------
unpack_trailer:

    iframe=last_frame-3;
    eof=				(vf_data[iframe] >>  0) & 0xFFF;
    lctype3=			(vf_data[iframe] >> 11) & 0x1;
    ddu[iframe]=		(vf_data[iframe] >> 12) & 0xF;		// DDU special

    iframe=last_frame-2;
    crc22lsb=			(vf_data[iframe] >>  0) & 0x7FF;
    lctype2=			(vf_data[iframe] >> 11) & 0x1;
    ddu[iframe]=		(vf_data[iframe] >> 12) & 0xF;		// DDU special

    iframe=last_frame-1;
    crc22msb=			(vf_data[iframe] >>  0) & 0x7FF;
    lctype1=			(vf_data[iframe] >> 11) & 0x1;
    ddu[iframe]=		(vf_data[iframe] >> 12) & 0xF;		// DDU special

    iframe=last_frame;
    frame_cnt=			(vf_data[iframe] >>  0) & 0x7FF;
    lctype0=			(vf_data[iframe] >> 11) & 0x1;
    ddu[iframe]=		(vf_data[iframe] >> 12) & 0xF;		// DDU special

    //------------------------------------------------------------------------------
    // Check DDU frame structure
    //------------------------------------------------------------------------------
    // First frame must be DB0C marker
    iframe=0;
    if ((vf_data[iframe]&0xFFFF)!=0xDB0C) {
        header_ok=false; 
        error_flag[7]=1;		// First frame missing 0xDB0C marker
        fprintf(stdout,"ERRs: Expected 0xDB0C marker in vf_data[%i], found %5.5X\n",iframe,vf_data[iframe]);
    }

    // First 4 frames must have DDU special D-code set
    for (iframe=0; iframe<=3; ++iframe) {
        if (ddu[iframe]!=0xD) {
            header_ok=false; 
            error_flag[8]=1;		// First 4 frames do not have DDU-special codes
            fprintf(stdout,"ERRs: Expected DDU special code in vf_data[%iframe], found %5.5X\n",iframe,vf_data[iframe]);
        } //close if 
    } //close for

    // Last 4 frames must have DDU special D-code set
    for (iframe=last_frame-3; iframe<=last_frame; ++iframe) {
        if (ddu[iframe]!=0xD) {
            header_ok=false;
            error_flag[9]=1;		// Last 4 frames do not have DDU-special codes
        }
    }

    // Last frame-3 must be EEF for short header or E0F for long header
    if (header_only_short) {
        if (eof != 0xEEF) {
            header_ok=false;
            error_flag[10]=1;		// Last frame-3 missing EEF marker
            fprintf(stdout,"ERRs: Expected EEF at vf_data[%i], found %3.3X\n",last_frame-3,eof);
        } 
    }
    else {
        if (eof != 0xE0F) {
            header_ok=false;
            error_flag[11]=1;		// Last frame-3 missing E0F marker
            fprintf(stdout,"ERRs: Expected EOF at vf_data[%i], found %3.3X\n",last_frame-3,eof);
        }
    }

    // Last 4 frames must set bit 11 ==1, its implied in EEF/EOF markers in last_frame-3
    if ((lctype0 != 1) || (lctype1 != 1) || (lctype2 != 1) || (lctype3 != 1)) {
        header_ok=false;
        error_flag[12]=1;		// Last 4 frames missing bit[11]=1
        fprintf(stdout,"ERRs: Expected bit11=1 in last 4 frames, found %i%i%i%i\n",lctype0,lctype1,lctype2,lctype3);
    }

    //------------------------------------------------------------------------------
    // Unpack event format codes from first 8 frames
    //------------------------------------------------------------------------------
    // CLCT fifo mode
    sfifo_mode           = "Undefined////          ";
    switch (fifo_mode) {
        case 0x0: sfifo_mode = "Dump=No    Header=Full "; break;
        case 0x1: sfifo_mode = "Dump=Full  Header=Full "; break;
        case 0x2: sfifo_mode = "Dump=Local Header=Full "; break;
        case 0x3: sfifo_mode = "Dump=No    Header=Short"; break;
        case 0x4: sfifo_mode = "Dump=No    Header=No   "; break;
        case 0x5: sfifo_mode = "Undefined////          "; break;
        case 0x6: sfifo_mode = "Undefined////          "; break;
        case 0x7: sfifo_mode = "Undefined////          "; break;
    }

    // Raw hits dump mode
    sreadout_type           = "Undefined////          ";
    switch (readout_type) {
        case 0x0: sreadout_type = "Dump=No    Header=Full "; break;
        case 0x1: sreadout_type = "Dump=Full  Header=Full "; break;
        case 0x2: sreadout_type = "Dump=Local Header=Full "; break;
        case 0x3: sreadout_type = "Dump=No    Header=Short"; break;
    }

    // Raw hits buffer state per l1a_type
    sl1a_type            = "Undefined////                 ";
    switch (l1a_type) {
        case 0x0: sl1a_type  = "Buffer=yes CLCT trig+L1A match"; break;
        case 0x1: sl1a_type  = "Buffer=no  ALCT trig          "; break;
        case 0x2: sl1a_type  = "Buffer=no  L1A  only          "; break;
        case 0x3: sl1a_type  = "Buffer=yes TMB  trig  no L1A  "; break;
    }

    // Firmware revcode
    id_rev			= revcode;
    id_rev_day		= (id_rev >>  0) & 0x001F;
    id_rev_month	= (id_rev >>  5) & 0x000F;
    id_rev_year		= (id_rev >>  9) & 0x000F;
    id_rev_fpga		= (id_rev >> 13) & 0x0007;
    id_rev_fpga		= id_rev_fpga+2;

    // TMB board hardware status summary
    for (i=0; i<=14; ++i) {
        bd_status_vec[i]=(bd_status >> i) & 0x1;
    }

    // CSC type
    scsc_type_inferred = "Indeterminate";
    csc_type_code      = 'X';

    if(header_only_short) {
        fprintf(stdout,"Header inferred CSC Type=%c %s\n",csc_type_code,scsc_type_inferred.c_str());
        goto check_types;
    }

    if (stagger_csc == 1 && reverse_hs_csc  == 0	&& reverse_hs_me1a == 0	&& reverse_hs_me1b == 0) {
        csc_type_code='A'; 
        scsc_type_inferred = "Normal";
    }

    if (stagger_csc     == 1	&& reverse_hs_csc  == 1	&& reverse_hs_me1a == 0	&& reverse_hs_me1b == 0) {
        csc_type_code='B'; 
        scsc_type_inferred = "Normal_Reverse";
    }

    if (stagger_csc     == 0	&& reverse_hs_csc  == 0	&& reverse_hs_me1a == 1	&& reverse_hs_me1b == 0) {
        csc_type_code='C'; 
        scsc_type_inferred = "ME1A_Reverse_ME1B_Normal";
    }

    if (stagger_csc     == 0	&& reverse_hs_csc  == 0	&& reverse_hs_me1a == 0	&& reverse_hs_me1b == 1) {
        csc_type_code='D'; 
        scsc_type_inferred = "ME1A_Normal_ME1B_Reverse";
    }

    fprintf(stdout,"Header inferred CSC Type=%c %s\n",csc_type_code,scsc_type_inferred.c_str());

    //------------------------------------------------------------------------------
    // Check event type codes match actual header size
    //------------------------------------------------------------------------------
check_types:

    // Readout types
    if (header_only_short) {
        if (fifo_mode!=0x3) {
            header_ok=false;
            error_flag[13]=1;	// Fifo mode does not match short header format
            fprintf(stdout,"ERRs: fifo_mode does not match short header format %i%i\n",fifo_mode,header_only_short);
        }
        if (readout_type!=0x3) {
            header_ok=false;
            error_flag[14]=1;	// Readout type does not match short header format
            fprintf(stdout,"ERRs: readout_type does not match short header format %i%i\n",readout_type,header_only_short);
        }
    }

    if(header_only_long || header_full) {
        if (!((fifo_mode==0x0) || (fifo_mode==0x1) || (fifo_mode==0x2))) {
            header_ok=false;
            error_flag[15]=1;	// Fifo mode does not match long header format
            fprintf(stdout,"ERRs: fifo_mode does not match long header format %i%i\n",fifo_mode,header_only_short);
        }
    }

    if(header_only_long) {
        if (readout_type!=0x0) {
            header_ok=false;
            error_flag[16]=1;	// Readout type does not match long header format
            fprintf(stdout,"ERRs: readout_type does not match long header format %i%i\n",readout_type,header_only_short);
        }
    }

    if(header_full) {
        if (!((readout_type==0x1) || (readout_type==0x2))) {
            header_ok=false;
            error_flag[17]=1;	// Readout type does not match full header format
            fprintf(stdout,"ERRs: readout_type does not match full header format %i%i\n",readout_type,header_only_short);
        }
    }

    if (fifo_mode!=readout_type) {
        header_ok=false;
        error_flag[18]=1;
        fprintf(stdout,"ERRs: readout type does not match FIFO mode r_type=%i fifo_mode=%i\n",readout_type,fifo_mode);
    }

    // L1A Types...in progress
    l1a_type_expect=0;

    if ((l1a_type!=l1a_type_expect) && (l1a_type_expect==0)) fprintf(stdout,"ERRs: l1a_type =%1.1i Buffer=yes CLCT trig+L1A match, expected=%1.1i\n",l1a_type,l1a_type_expect);
    if ((l1a_type!=l1a_type_expect) && (l1a_type_expect==1)) fprintf(stdout,"ERRs: l1a_type =%1.1i Buffer=no  ALCT trig,           expected=%1.1i\n",l1a_type,l1a_type_expect);
    if ((l1a_type!=l1a_type_expect) && (l1a_type_expect==2)) fprintf(stdout,"ERRs: l1a_type =%1.1i Buffer=no  L1A  only,           expected=%1.1i\n",l1a_type,l1a_type_expect);
    if ((l1a_type!=l1a_type_expect) && (l1a_type_expect==3)) fprintf(stdout,"ERRs: l1a_type =%1.1i Buffer=yes TMB  trig,           expected=%1.1i\n",l1a_type,l1a_type_expect);

    //------------------------------------------------------------------------------
    // Check word count matches expected, assumes mode 1 full readout for now
    //------------------------------------------------------------------------------
    //fifo_cfeb_enable		= 0; 
    frame_cntex_ntbins		= 0;
    frame_cntex_b0ce0c		= 0;
    frame_cntex_rpc			= 0;
    frame_cntex_b04e04		= 0;
    frame_cntex_scope		= 0;
    frame_cntex_b05e05		= 0;
    frame_cntex_miniscope	= 0;
    frame_cntex_b07e07		= 0;
    frame_cntex_bcbecb		= 0;
    frame_cntex_blockedbits	= 0;

    frame_cntex_nheaders	= r_nheaders;									// Expected Header words
    //fifo_cfeb_enable		= ((fifo_mode==1) || (fifo_mode==2)) && ((readout_type==1) || (readout_type==2));  // CLCT raw hits exist

    if(header_only_long) {
        frame_cntex_b0ce0c		= 2;											// Expected EOB, EOC markers
    }

    if(header_full) {														// Raw hits are only in full readout mode
        frame_cntex_ntbins		= 6*r_fifo_tbins*r_ncfebs;						// Expected CFEB tbins
        frame_cntex_b0ce0c		= 2;											// Expected EOB, EOC markers
        frame_cntex_rpc			= (rd_nrpcs*fifo_tbins_rpc*2)*rpc_read_enable;	// Expected RPC  tbins
        frame_cntex_b04e04		= 2*rpc_read_enable;							// Expected RPC  B04/E04 markers
        frame_cntex_scope		= ((scp_tbins+1)*64*160/16)*scope_exists;		// Expected scope data
        frame_cntex_b05e05		= 2*scope_exists;								// Expected scope B05/E05 markers
        frame_cntex_miniscope	= fifo_tbins_mini*miniscope_exists;				// Expected miniscope data
        frame_cntex_b07e07		= 2*miniscope_exists;							// Expected miniscope B07/E07 markers
        frame_cntex_blockedbits	= 20*h_bcb_read_enable;							// Expected blocked bits data
        frame_cntex_bcbecb		= 2*h_bcb_read_enable;							// Expected blocked bits BCB/ECB markers
    }

    frame_cnt_expect=									// Expected frames before trailer
        frame_cntex_nheaders
        +frame_cntex_ntbins
        +frame_cntex_b0ce0c
        +frame_cntex_rpc
        +frame_cntex_b04e04
        +frame_cntex_scope
        +frame_cntex_b05e05
        +frame_cntex_miniscope
        +frame_cntex_b07e07
        +frame_cntex_bcbecb
        +frame_cntex_blockedbits;

    frame_cntex_fill=0;									// Insert fillers if frames not multiple of 4
    if((frame_cnt_expect%4) != 0)frame_cntex_fill=2;
    if(frame_cntex_fill!=0) header_filler=true;

    frame_cntex_trailer=4;								// Add 4 trailer words

    frame_cnt_expect=
        frame_cnt_expect
        +frame_cntex_fill
        +frame_cntex_trailer;	

    frame_cnt_expect_trun=frame_cnt_expect & 0x07FF;	// Trailer word count is truncated to 11 bits
    dmb_wdcnt_trun=dmb_wdcnt & 0x07FF;

    if((frame_cnt_expect/4)*4 != frame_cnt_expect)		// Check we are still mod 4 happy
        pause ("expected frame count not mod 4..wtf?");

    fprintf(stdout,"Expected header             frames =%5i\n",frame_cntex_nheaders);
    fprintf(stdout,"Expected cfeb  tbins        frames =%5i\n",frame_cntex_ntbins);
    fprintf(stdout,"Expected cfeb  e0b/e0c      frames =%5i\n",frame_cntex_b0ce0c);
    fprintf(stdout,"Expected rpc   tbins        frames =%5i\n",frame_cntex_rpc);
    fprintf(stdout,"Expected rpc   b04e04       frames =%5i\n",frame_cntex_b04e04);
    fprintf(stdout,"Expected scope data         frames =%5i\n",frame_cntex_scope);
    fprintf(stdout,"Expected scope b05e05       frames =%5i\n",frame_cntex_b05e05);
    fprintf(stdout,"Expected miniscope data     frames =%5i\n",frame_cntex_miniscope);
    fprintf(stdout,"Expected miniscope b07e07   frames =%5i\n",frame_cntex_b07e07);
    fprintf(stdout,"Expected blockedbits bcbecb frames =%5i\n",frame_cntex_bcbecb);
    fprintf(stdout,"Expected blockedbits data   frames =%5i\n",frame_cntex_blockedbits);
    fprintf(stdout,"Expected filler             frames =%5i\n",frame_cntex_fill);
    fprintf(stdout,"Expected trailer            frames =%5i\n",frame_cntex_trailer);

    fprintf(stdout,"Expected frame count from header =%5i\n",frame_cnt_expect);
    fprintf(stdout,"Frame count stored in trailer    =%5i\n",frame_cnt);
    fprintf(stdout,"Frame count from DMB RAM         =%5i\n",dmb_wdcnt);

    if((dmb_wdcnt_trun==frame_cnt)&&(frame_cnt==frame_cnt_expect_trun))
        fprintf(stdout,"Frame count OK %5i\n",frame_cnt);
    else {
        header_ok=false;
        error_flag[19]=1;	// Expected frame count does not match actual frame count
        fprintf(stdout,"ERRs: Bad frame count: read=%5i expect=%5i\n",frame_cnt,frame_cnt_expect_trun);
    }

    //------------------------------------------------------------------------------
    // Construct CLCT, RPC, and Scope pointers
    // Do not just scan for the markers, because they are not unique
    //------------------------------------------------------------------------------
    adr_b0c =  0;				// All formats start with b0c
    adr_e0b = -1;				// Short or long header-only events do not have these markers
    adr_b04 = -1;
    adr_e04 = -1;
    adr_b05 = -1;
    adr_e05 = -1;
    adr_b07 = -1;
    adr_e07 = -1;
    adr_bcb = -1;
    adr_ecb = -1;
    adr_e0c = -1;
    adr_fil = -1;
    adr_e0f = dmb_wdcnt-4;		// All formats have e0f or eef near the end

    if (!header_only_short)		// short header has no e0b
        adr_e0b = r_nheaders;
    if (!(header_only_short || header_only_long))	{	// cfeb readouts have these markers
        adr_b04 = (rpc_read_enable  ==1) ? (adr_e0b+frame_cntex_ntbins+1) : -1;
        adr_e04 = (rpc_read_enable  ==1) ? (adr_b04+frame_cntex_rpc+1  )  : -1;
        adr_b05 = (scope_exists     ==1) ? (adr_e0b+frame_cntex_ntbins+frame_cntex_rpc+frame_cntex_b04e04+1) : -1;
        adr_e05 = (scope_exists     ==1) ? (adr_b05+frame_cntex_scope+1)  : -1;
        adr_b07 = (miniscope_exists ==1) ? (adr_e0b+frame_cntex_ntbins+frame_cntex_rpc+frame_cntex_b04e04+frame_cntex_scope+1) : -1;
        adr_e07 = (miniscope_exists ==1) ? (adr_b07+frame_cntex_miniscope+1)   : -1;
        adr_bcb = (h_bcb_read_enable==1) ? (adr_e0b+frame_cntex_ntbins+frame_cntex_rpc+frame_cntex_b04e04+frame_cntex_scope+frame_cntex_b05e05+frame_cntex_miniscope+frame_cntex_b07e07+1) : -1;
        adr_ecb = (h_bcb_read_enable==1) ? (adr_bcb+frame_cntex_blockedbits+1) : -1;
    }
    if (!header_only_short)		// Short header does not have eoc
        adr_e0c = adr_e0b+frame_cntex_ntbins
            +frame_cntex_rpc+frame_cntex_b04e04
            +frame_cntex_scope+frame_cntex_b05e05
            +frame_cntex_miniscope+frame_cntex_b07e07
            +frame_cntex_blockedbits+frame_cntex_bcbecb+1;

    adr_fil =(frame_cntex_fill !=0) ? (frame_cnt_expect-6) : -1;


    //------------------------------------------------------------------------------
    // Check that markers and filler frames exist at the predicted locations
    //------------------------------------------------------------------------------
    fprintf(stdout,"adr_b0c=%5i",adr_b0c); if(adr_b0c!=-1)fprintf(stdout," data=%4.4X",vf_data[adr_b0c]); fprintf(stdout,"\n");
    fprintf(stdout,"adr_e0b=%5i",adr_e0b); if(adr_e0b!=-1)fprintf(stdout," data=%4.4X",vf_data[adr_e0b]); fprintf(stdout,"\n");
    fprintf(stdout,"adr_b04=%5i",adr_b04); if(adr_b04!=-1)fprintf(stdout," data=%4.4X",vf_data[adr_b04]); fprintf(stdout,"\n");
    fprintf(stdout,"adr_e04=%5i",adr_e04); if(adr_e04!=-1)fprintf(stdout," data=%4.4X",vf_data[adr_e04]); fprintf(stdout,"\n");
    fprintf(stdout,"adr_b05=%5i",adr_b05); if(adr_b05!=-1)fprintf(stdout," data=%4.4X",vf_data[adr_b05]); fprintf(stdout,"\n");
    fprintf(stdout,"adr_e05=%5i",adr_e05); if(adr_e05!=-1)fprintf(stdout," data=%4.4X",vf_data[adr_e05]); fprintf(stdout,"\n");
    fprintf(stdout,"adr_b07=%5i",adr_b07); if(adr_b07!=-1)fprintf(stdout," data=%4.4X",vf_data[adr_b07]); fprintf(stdout,"\n");
    fprintf(stdout,"adr_e07=%5i",adr_e07); if(adr_e07!=-1)fprintf(stdout," data=%4.4X",vf_data[adr_e07]); fprintf(stdout,"\n");
    fprintf(stdout,"adr_bcb=%5i",adr_bcb); if(adr_bcb!=-1)fprintf(stdout," data=%4.4X",vf_data[adr_bcb]); fprintf(stdout,"\n");
    fprintf(stdout,"adr_ecb=%5i",adr_ecb); if(adr_ecb!=-1)fprintf(stdout," data=%4.4X",vf_data[adr_ecb]); fprintf(stdout,"\n");
    fprintf(stdout,"adr_e0c=%5i",adr_e0c); if(adr_e0c!=-1)fprintf(stdout," data=%4.4X",vf_data[adr_e0c]); fprintf(stdout,"\n");
    fprintf(stdout,"adr_fil=%5i",adr_fil); if(adr_fil!=-1)fprintf(stdout," data=%4.4X",vf_data[adr_fil]); fprintf(stdout,"\n");
    fprintf(stdout,"adr_e0f=%5i",adr_e0f); if(adr_e0f!=-1)fprintf(stdout," data=%4.4X",vf_data[adr_e0f]); fprintf(stdout,"\n");

    // Check E0B Header
    if ((adr_e0b>0) && ((vf_data[adr_e0b]&0xFFFF)!=0x6E0B)) {
        header_ok=false;
        error_flag[20]=1;	// E0B marker not found at expected address
        fprintf(stdout,"\n");
    }

    // Check B04 | E04 RPC
    if ((adr_b04>0) && ((vf_data[adr_b04]&0xFFFF)!=0x6B04)) {
        header_ok=false;
        error_flag[21]=1;    // B04 marker not found at expected address
        fprintf(stdout,"ERRs: B04 marker not found at expected address\n");
    }

    if ((adr_e04>0) && ((vf_data[adr_e04]&0xFFFF)!=0x6E04)) {
        header_ok=false;
        error_flag[22]=1;	 // E04 marker not found at expected address
        fprintf(stdout,"ERRs: E04 marker not found at expected address\n");
    }

    // Check B05 | E05 Scope
    if ((adr_b05>0) && ((vf_data[adr_b05]&0xFFFF)!=0x6B05)) {
        header_ok=false;
        error_flag[23]=1;	 // B05 marker not found at expected address
        fprintf(stdout,"ERRs: B05 marker not found at expected address\n");
    }

    if ((adr_e05>0) && ((vf_data[adr_e05]&0xFFFF)!=0x6E05)) {
        header_ok=false;
        error_flag[24]=1;	 // E05 marker not found at expected address
        fprintf(stdout,"ERRs: E05 marker not found at expected address\n");
    }

    // Check B07 | E07 Miniscope
    if ((adr_b07>0) && ((vf_data[adr_b07]&0xFFFF)!=0x6B07)) {
        header_ok=false;
        error_flag[23]=1;	 // B07 marker not found at expected address
        fprintf(stdout,"ERRs: B07 marker not found at expected address\n");
    }

    if ((adr_e07>0) && ((vf_data[adr_e07]&0xFFFF)!=0x6E07)) {
        header_ok=false;
        error_flag[24]=1;	 // E07 marker not found at expected address
        fprintf(stdout,"ERRs: E07 marker not found at expected address\n");
    }

    // Check BCB | ECB Blocked bits
    if ((adr_bcb>0) && ((vf_data[adr_bcb]&0xFFFF)!=0x6BCB)) {
        header_ok=false;
        error_flag[23]=1;	 // BCB marker not found at expected address
        fprintf(stdout,"ERRs: BCB marker not found at expected address\n");
    }

    if ((adr_ecb>0) && ((vf_data[adr_ecb]&0xFFFF)!=0x6ECB)) {
        header_ok=false;
        error_flag[24]=1;	 // ECB marker not found at expected address
        fprintf(stdout,"ERRs: ECB marker not found at expected address\n");
    }

    // Check E0C
    if ((adr_e0c>0) && ((vf_data[adr_e0c]&0xFFFF)!=0x6E0C)) {
        header_ok=false;
        error_flag[25]=1;	// E0C marker not found at expected address
        fprintf(stdout,"ERRs: E0C marker not found at expected address\n");
    }

    // Check filler 02AAA 05555
    if ((adr_fil>0) && ((vf_data[adr_fil]&0xFFFF)!=0x2AAA)) {
        header_ok=false;
        error_flag[26]=1;	// 2AAA filler not found at expected address
        fprintf(stdout,"ERRs: Filler frame 0x2AAA not found at expected address\n");
    }

    if ((adr_fil>0) && ((vf_data[adr_fil+1]&0xFFFF)!=0x5555)) {
        header_ok=false;
        error_flag[27]=1;	// 5555 filler not found at expected address
        fprintf(stdout,"ERRs: Filler frame 0x5555 not found at expected address\n");
    }

    //------------------------------------------------------------------------------
    // Check TMB hardware status
    //------------------------------------------------------------------------------
    bd_status_expect[0]=1;		// bd_status_ok
    bd_status_expect[1]=1;		// vstat_5p0vs
    bd_status_expect[2]=1;		// vstat_3p3v
    bd_status_expect[3]=1;		// vstat_1p8v
    bd_status_expect[4]=1;		// vstat_1p5v
    bd_status_expect[5]=1;		// _t_crit
    bd_status_expect[6]=1;		// vsm_ok
    bd_status_expect[7]=0;		// vsm_aborted
    bd_status_expect[8]=1;		// vsm_cksum_ok
    bd_status_expect[9]=1;		// vsm_wdcnt_ok
    bd_status_expect[10]=1;		// jsm_ok
    bd_status_expect[11]=0;		// jsm_aborted
    bd_status_expect[12]=1;		// jsm_cksum_ok
    bd_status_expect[13]=1;		// jsm_wdcnt_ok
    bd_status_expect[14]=1;		// sm_tck_fpga_ok

    for (i=0; i<MXSTAT; ++i) {
        if(bd_status_vec[i]!=bd_status_expect[i]) {
            header_ok=false;
            error_flag[28]=1;
            fprintf(stdout,"ERRs: Bad board status %s=%i expected=%i\n",bd_status_msg[i].c_str(),bd_status_vec[i],bd_status_expect[i]);
        }
    }

    if (header_only_short) goto check_l1a;

    //------------------------------------------------------------------------------
    // Check buffer status, requires long header
    //------------------------------------------------------------------------------
    /*
    // Buffer Status
    iframe=37;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_wr_buf_adr       = %4.3X\n",	r_wr_buf_adr);
    fprintf(stdout,"r_wr_buf_ready     = %4.1X\n",	r_wr_buf_ready);
    fprintf(stdout,"wr_buf_ready       = %4.1X\n",	wr_buf_ready);
    fprintf(stdout,"buf_q_full         = %4.1X\n",	buf_q_full);
    fprintf(stdout,"buf_q_empty        = %4.1X\n",	buf_q_empty);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=38;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_buf_fence_dist   = %4.3X%5i\n",	r_buf_fence_dist,r_buf_fence_dist);
    fprintf(stdout,"buf_q_ovf_err      = %4.1X\n",	buf_q_ovf_err);
    fprintf(stdout,"buf_q_udf_err      = %4.1X\n",	buf_q_udf_err);
    fprintf(stdout,"buf_q_adr_err      = %4.1X\n",	buf_q_adr_err);
    fprintf(stdout,"buf_stalled_once   = %4.1X\n",	buf_stalled_once);

*/
    //------------------------------------------------------------------------------
    // Unpack Long header event counters
    //------------------------------------------------------------------------------
    // Long header counters
    pretrig_counter = r_pretrig_counter_lsb | (r_pretrig_counter_msb << 15);
    clct_counter    = r_clct_counter_lsb    | (r_clct_counter_msb    << 15);
    trig_counter    = r_trig_counter_lsb    | (r_trig_counter_msb    << 15);
    alct_counter    = r_alct_counter_lsb    | (r_alct_counter_msb    << 15);
    uptime_counter  = r_orbit_counter_lsb   | (r_orbit_counter_msb   << 15);

    uptime_sec=int(float(lhc_cycle)*float(uptime_counter)*25.0e-09);

    //------------------------------------------------------------------------------
    // Decode LCTs
    //------------------------------------------------------------------------------
    // Decode CLCT0 Word
    clct0=r_clct0_tmb_lsb | (r_clct0_tmb_msb << 15);			//Reassemble to full 16 bits

    clct0_vpf=	(clct0 >>  0) & 0x1;	// Valid pattern flag
    clct0_nhit=	(clct0 >>  1) & 0x7;	// Hits on pattern 0-6
    clct0_pat=	(clct0 >>  4) & 0xF;	// Pattern shape 0-A
    clct0_key=	(clct0 >>  8) & 0x1F;	// 1/2-strip ID number
    clct0_cfeb=	(clct0 >> 13) & 0x7;

    clct0_fullkey=clct0_key+32*clct0_cfeb;

    // Decode CLCT1 Word
    clct1=r_clct1_tmb_lsb | (r_clct1_tmb_msb << 15);			//Reassemble to full 16 bits

    clct1_vpf=	(clct1 >>  0) & 0x1;	// Valid pattern flag
    clct1_nhit=	(clct1 >>  1) & 0x7;	// Hits on pattern
    clct1_pat=	(clct1 >>  4) & 0xF;	// Pattern shape 0-A
    clct1_key=	(clct1 >>  8) & 0x1F;	// 1/2-strip ID number
    clct1_cfeb=	(clct1 >> 13) & 0x7;

    clct1_fullkey=clct1_key+32*clct1_cfeb;

    // Decode CLCT common data
    clctc=r_clctc_tmb;
    clctc_bxn=	(clctc >> 0) & 0x3;
    clctc_sync=	(clctc >> 2) & 0x1;		// Sync error

    // Decode MPC Frames
    mpc0_frame0 = r_mpc0_frame0_lsb | (r_mpc0_frame0_msb << 15);	//Reassemble to full 16 bits
    mpc0_frame1 = r_mpc0_frame1_lsb | (r_mpc0_frame1_msb << 15);
    mpc1_frame0 = r_mpc1_frame0_lsb | (r_mpc1_frame0_msb << 15);
    mpc1_frame1 = r_mpc1_frame1_lsb | (r_mpc1_frame1_msb << 15);

    mpc_alct0_key		=	(mpc0_frame0 >>  0) & 0x007F;
    mpc_clct0_pat		=	(mpc0_frame0 >>  7) & 0x000F;
    mpc_lct0_quality	=	(mpc0_frame0 >> 11) & 0x000F;
    mpc_lct0_vpf		=	(mpc0_frame0 >> 15) & 0x0001;

    mpc_clct0_key		=	(mpc0_frame1 >>  0) & 0x00FF;
    mpc_clct0_bend		=	(mpc0_frame1 >>  8) & 0x0001;
    mpc_sync_err0		=	(mpc0_frame1 >>  9) & 0x0001;
    mpc_alct0_bxn		=	(mpc0_frame1 >> 10) & 0x0001;
    mpc_bx0_clct		=	(mpc0_frame1 >> 11) & 0x0001;
    mpc_csc_id0			=	(mpc0_frame1 >> 12) & 0x000F;

    mpc_alct1_key		=	(mpc1_frame0 >>  0) & 0x007F;
    mpc_clct1_pat		=	(mpc1_frame0 >>  7) & 0x000F;
    mpc_lct1_quality	=	(mpc1_frame0 >> 11) & 0x000F;
    mpc_lct1_vpf		=	(mpc1_frame0 >> 15) & 0x0001;

    mpc_clct1_key		=	(mpc1_frame1 >>  0) & 0x00FF;
    mpc_clct1_bend		=	(mpc1_frame1 >>  8) & 0x0001;
    mpc_sync_err1		=	(mpc1_frame1 >>  9) & 0x0001;
    mpc_alct1_bxn		=	(mpc1_frame1 >> 10) & 0x0001;
    mpc_bx0_alct		=	(mpc1_frame1 >> 11) & 0x0001;
    mpc_csc_id1			=	(mpc1_frame1 >> 12) & 0x000F;

    //------------------------------------------------------------------------------
    // Check if TMB duplicated the ALCT or CLCT correctly
    //------------------------------------------------------------------------------
    if(!err_check) goto unpack_cfebs;

    // Count clcts and alcts in header, determine expected duplication behavior
    hdr_one_clct  = clct0_vpf & !clct1_vpf;
    hdr_two_clct  = clct0_vpf &  clct1_vpf;

    hdr_one_alct  = r_alct0_valid & !r_alct1_valid;
    hdr_two_alct  = r_alct0_valid &  r_alct1_valid;

    hdr_dupe_clct = hdr_one_clct & hdr_two_alct;
    hdr_dupe_alct = hdr_one_alct & hdr_two_clct;

    // Check header duplication flags are as expected
    if (hdr_one_clct  != expect_one_clct ) fprintf(stdout,"\tERRs: hdr_one_clct =%1i does not match expect_one_clct =%1i\n",hdr_one_clct, expect_one_clct);
    if (hdr_two_clct  != expect_two_clct ) fprintf(stdout,"\tERRs: hdr_two_clct =%1i does not match expect_two_clct =%1i\n",hdr_two_clct, expect_two_clct);
    if (hdr_one_alct  != expect_one_alct ) fprintf(stdout,"\tERRs: hdr_one_alct =%1i does not match expect_one_alct =%1i\n",hdr_one_alct, expect_one_alct);
    if (hdr_two_alct  != expect_two_alct ) fprintf(stdout,"\tERRs: hdr_two_alct =%1i does not match expect_two_alct =%1i\n",hdr_two_alct, expect_two_alct);
    if (hdr_dupe_clct != expect_dupe_clct) fprintf(stdout,"\tERRs: hdr_dupe_clct=%1i does not match expect_dupe_clct=%1i\n",hdr_dupe_clct,expect_dupe_clct);
    if (hdr_dupe_alct != expect_dupe_alct) fprintf(stdout,"\tERRs: hdr_dupe_alct=%1i does not match expect_dupe_alct=%1i\n",hdr_dupe_alct,expect_dupe_alct);

    // Compare expected duplication to what tmb reports for this event
    if (r_one_alct_tmb  != hdr_one_alct ) fprintf(stdout,"\tERRs: r_one_alct_tmb =%1i does not match expected hdr_one_alct =%1i\n",r_one_alct_tmb, hdr_one_alct );
    if (r_one_clct_tmb  != hdr_one_clct ) fprintf(stdout,"\tERRs: r_one_clct_tmb =%1i does not match expected hdr_one_clct =%1i\n",r_one_clct_tmb, hdr_one_clct );
    if (r_two_alct_tmb  != hdr_two_alct ) fprintf(stdout,"\tERRs: r_two_alct_tmb =%1i does not match expected hdr_two_alct =%1i\n",r_two_alct_tmb, hdr_two_alct );
    if (r_two_clct_tmb  != hdr_two_clct ) fprintf(stdout,"\tERRs: r_two_clct_tmb =%1i does not match expected hdr_two_clct =%1i\n",r_two_clct_tmb, hdr_two_clct );
    if (r_dupe_alct_tmb != hdr_dupe_alct) fprintf(stdout,"\tERRs: r_dupe_alct_tmb=%1i does not match expected hdr_dupe_alct=%1i\n",r_dupe_alct_tmb,hdr_dupe_alct);
    if (r_dupe_clct_tmb != hdr_dupe_clct) fprintf(stdout,"\tERRs: r_dupe_clct_tmb=%1i does not match expected hdr_dupe_clct=%1i\n",r_dupe_clct_tmb,hdr_dupe_clct);

    if (r_one_alct_tmb  != hdr_one_alct ) fprintf(stdout,"ERRs: r_one_alct_tmb =%1i does not match expected hdr_one_alct =%1i\n",r_one_alct_tmb, hdr_one_alct );
    if (r_one_clct_tmb  != hdr_one_clct ) fprintf(stdout,"ERRs: r_one_clct_tmb =%1i does not match expected hdr_one_clct =%1i\n",r_one_clct_tmb, hdr_one_clct );
    if (r_two_alct_tmb  != hdr_two_alct ) fprintf(stdout,"ERRs: r_two_alct_tmb =%1i does not match expected hdr_two_alct =%1i\n",r_two_alct_tmb, hdr_two_alct );
    if (r_two_clct_tmb  != hdr_two_clct ) fprintf(stdout,"ERRs: r_two_clct_tmb =%1i does not match expected hdr_two_clct =%1i\n",r_two_clct_tmb, hdr_two_clct );
    if (r_dupe_alct_tmb != hdr_dupe_alct) fprintf(stdout,"ERRs: r_dupe_alct_tmb=%1i does not match expected hdr_dupe_alct=%1i\n",r_dupe_alct_tmb,hdr_dupe_alct);
    if (r_dupe_clct_tmb != hdr_dupe_clct) fprintf(stdout,"ERRs: r_dupe_clct_tmb=%1i does not match expected hdr_dupe_clct=%1i\n",r_dupe_clct_tmb,hdr_dupe_clct);

    //------------------------------------------------------------------------------
    // Check trigger type flags in header
    //------------------------------------------------------------------------------
    if (r_tmb_match    ==1 && tmb_allow_match   !=1) ck("Trigger flag: r_tmb_match     : tmb_allow_match   ",r_tmb_match,     tmb_allow_match   );
    if (r_tmb_alct_only==1 && tmb_allow_alct    !=1) ck("Trigger flag: r_tmb_alct_only : tmb_allow_alct    ",r_tmb_alct_only, tmb_allow_alct    );
    if (r_tmb_clct_only==1 && tmb_allow_clct    !=1) ck("Trigger flag: r_tmb_clct_only : tmb_allow_clct    ",r_tmb_clct_only, tmb_allow_clct    );

    if (tmb_alct_ro    ==1 && tmb_allow_alct_ro !=1) ck("Trigger flag: tmb_alct_ro     : tmb_allow_alct_ro ",tmb_alct_ro,     tmb_allow_alct_ro );
    if (tmb_clct_ro    ==1 && tmb_allow_clct_ro !=1) ck("Trigger flag: tmb_clct_ro     : tmb_allow_clct_ro ",tmb_clct_ro,     tmb_allow_clct_ro );
    if (tmb_match_ro   ==1 && tmb_allow_match_ro!=1) ck("Trigger flag: tmb_match_ro    : tmb_allow_match_ro",tmb_match_ro,    tmb_allow_match_ro);

    if (tmb_trig_keep  ==1 && tmb_nontrig_keep  !=0) ck("Trigger flag: tmb_trig_keep   : tmb_nontrig_keep  ",tmb_trig_keep,   tmb_nontrig_keep  );
    if (tmb_trig_keep  ==0 && tmb_nontrig_keep  !=1) ck("Trigger flag: tmb_trig_keep   : tmb_nontrig_keep  ",tmb_trig_keep,   tmb_nontrig_keep  );

    if (tmb_alct_ro    ==1 && tmb_nontrig_keep  !=1) ck("Trigger flag: tmb_alct_ro     : tmb_nontrig_keep  ",tmb_alct_ro,     tmb_nontrig_keep  );
    if (tmb_clct_ro    ==1 && tmb_nontrig_keep  !=1) ck("Trigger flag: tmb_clct_ro     : tmb_nontrig_keep  ",tmb_clct_ro,     tmb_nontrig_keep  );
    if (tmb_match_ro   ==1 && tmb_nontrig_keep  !=1) ck("Trigger flag: tmb_match_ro    : tmb_nontrig_keep  ",tmb_match_ro,    tmb_nontrig_keep  );

    //------------------------------------------------------------------------------
    // Check if header LCT matches CLCT and ALCT from header
    //------------------------------------------------------------------------------
    non_trig_event    = (tmb_nontrig_keep==1);
    non_trig_override = (mpc_me1a_block  ==0);

    if (non_trig_event) fprintf(stdout,"Info: This is a non-triggering-event readout tmb_alct_ro=%1i tmb_clct_ro=%1i tmb_match_ro=%1i\n",tmb_alct_ro,tmb_clct_ro,tmb_match_ro);

    // Type A: Normal CSC
    if (csc_type_code=='A')
    {
        mpc_clct0_key_expect = clct0_fullkey;
        mpc_clct0_pat_expect = clct0_pat;
        mpc_alct0_key_expect = r_alct0_key;

        mpc_clct1_key_expect = (r_dupe_clct_tmb) ? clct0_fullkey : clct1_fullkey;
        mpc_clct1_pat_expect = (r_dupe_clct_tmb) ? clct0_pat     : clct1_pat;
        mpc_alct1_key_expect = (r_dupe_alct_tmb) ? r_alct0_key   : r_alct1_key;
    }

    // Type B: Reversed CSC
    else if (csc_type_code=='B')
    {
        mpc_clct0_key_expect = clct0_fullkey;
        mpc_clct0_pat_expect = clct0_pat;
        mpc_alct0_key_expect = r_alct0_key;

        mpc_clct1_key_expect = (r_dupe_clct_tmb) ? clct0_fullkey : clct1_fullkey;
        mpc_clct1_pat_expect = (r_dupe_clct_tmb) ? clct0_pat     : clct1_pat;
        mpc_alct1_key_expect = (r_dupe_alct_tmb) ? r_alct0_key   : r_alct1_key;
    }

    // Type C: ME1B strips not reversed on CFEBs 0-3, ME1A on CFEB4 reverses strip numbers
    else if (csc_type_code=='C')
    {
        mpc_clct0_key_expect = ((non_trig_event || clct0_fullkey>=128) && !non_trig_override) ? 0: clct0_fullkey;
        mpc_clct0_pat_expect = ((non_trig_event || clct0_fullkey>=128) && !non_trig_override) ? 0: clct0_pat;
        mpc_alct0_key_expect = ((non_trig_event || clct0_fullkey>=128) && !non_trig_override) ? 0: r_alct0_key;

        mpc_clct1_key_expect = (r_dupe_clct_tmb) ? clct0_fullkey : clct1_fullkey;
        mpc_clct1_pat_expect = (r_dupe_clct_tmb) ? clct0_pat     : clct1_pat;
        mpc_alct1_key_expect = (r_dupe_alct_tmb) ? r_alct0_key   : r_alct1_key;

        mpc_clct1_key_expect = ((non_trig_event || clct1_fullkey>=128) && !non_trig_override) ? 0: mpc_clct1_key_expect;
        mpc_clct1_pat_expect = ((non_trig_event || clct1_fullkey>=128) && !non_trig_override) ? 0: mpc_clct1_pat_expect;
        mpc_alct1_key_expect = ((non_trig_event || clct1_fullkey>=128) && !non_trig_override) ? 0: mpc_alct1_key_expect;
    }

    // Type D: ME1B reverses strips on CFEBs 0-3, ME1A on CFEB4 strips not reversed
    else if (csc_type_code=='D')
    {
        mpc_clct0_key_expect = ((non_trig_event || clct0_fullkey>=128) && !non_trig_override) ? 0: clct0_fullkey;
        mpc_clct0_pat_expect = ((non_trig_event || clct0_fullkey>=128) && !non_trig_override) ? 0: clct0_pat;
        mpc_alct0_key_expect = ((non_trig_event || clct0_fullkey>=128) && !non_trig_override) ? 0: r_alct0_key;

        mpc_clct1_key_expect = (r_dupe_clct_tmb) ? clct0_fullkey : clct1_fullkey;
        mpc_clct1_pat_expect = (r_dupe_clct_tmb) ? clct0_pat     : clct1_pat;
        mpc_alct1_key_expect = (r_dupe_alct_tmb) ? r_alct0_key   : r_alct1_key;

        mpc_clct1_key_expect = ((non_trig_event || clct1_fullkey>=128) && !non_trig_override) ? 0: mpc_clct1_key_expect;
        mpc_clct1_pat_expect = ((non_trig_event || clct1_fullkey>=128) && !non_trig_override) ? 0: mpc_clct1_pat_expect;
        mpc_alct1_key_expect = ((non_trig_event || clct1_fullkey>=128) && !non_trig_override) ? 0: mpc_alct1_key_expect;
    }

    // Type unknown
    else
    {
        mpc_clct0_key_expect = 0;
        mpc_clct0_pat_expect = 0;
        mpc_alct0_key_expect = 0;

        mpc_clct1_key_expect = 0;
        mpc_clct1_pat_expect = 0;
        mpc_alct1_key_expect = 0;
    }

    // CLCT0 key
    if (mpc_clct0_key != mpc_clct0_key_expect)
    {
        header_ok=false;
        error_flag[31]=1;	// CLCT0 key does not match LCT0
        fprintf(stdout,"ERRs: CLCT0 key does not match LCT0: clct0_fullkey=%3i mpc_clct0_key=%3i expect=%3i\n",clct0_fullkey,mpc_clct0_key,mpc_clct0_key_expect);
    }

    // CLCT0 pattern ID
    if (mpc_clct0_pat != mpc_clct0_pat_expect) {
        header_ok=false;
        error_flag[32]=1;	// CLCT0 pattern ID does not match LCT0
        fprintf(stdout,"ERRs: CLCT0 pattern ID does not match LCT0: clct0_pat=%1X mpc_clct0_pat=%1X expect=%1X\n",clct0_pat,mpc_clct0_pat,mpc_clct0_pat_expect);
    }

    // ALCT0 key
    if (mpc_alct0_key != mpc_alct0_key_expect) {
        header_ok=false;
        error_flag[33]=1;	// ALCT0 key does not match LCT0
        fprintf(stdout,"ERRs: ALCT0 key does not match LCT0: r_alct0_key=%3i mpc_alct0_key=%3i expect=%3i\n",r_alct0_key,mpc_alct0_key,mpc_alct0_key_expect);
    }

    // CLCT1 key
    if (mpc_clct1_key != mpc_clct1_key_expect) {
        header_ok=false;
        error_flag[34]=1;	// CLCT1 key does not match LCT1
        fprintf(stdout,"ERRs: CLCT1 key does not match LCT1: clct1_fullkey=%3i mpc_clct1_key=%3i expect=%3i\n",clct1_fullkey,mpc_clct1_key,mpc_clct1_key_expect);
    }

    // CLCT1 pattern ID
    if (mpc_clct1_pat != mpc_clct1_pat_expect) {
        header_ok=false;
        error_flag[35]=1;	// CLCT1 pattern ID does not match LCT1
        fprintf(stdout,"ERRs: CLCT1 pattern ID does not match LCT1: clct1_pat=%1X mpc_clct1_pat=%1X expect=%1X\n",clct1_pat,mpc_clct1_pat,mpc_clct1_pat_expect);
    }

    // ALCT1 key
    if (mpc_alct1_key != mpc_alct1_key_expect) {
        header_ok=false;
        error_flag[36]=1;	// ALCT1 key does not match LCT1
        fprintf(stdout,"ERRs: ALCT1 key does not match LCT1: r_alct1_key=%3i mpc_alct1_key=%3i expect=%3i\n",r_alct1_key,mpc_alct1_key,mpc_alct1_key_expect);
    }

    //------------------------------------------------------------------------------
    // Check if LCT header data matches VME latched data
    //------------------------------------------------------------------------------
    if(err_check) {
        ck("clct0 header", clct0, clct0_vme);
        ck("clct1 header", clct1, clct1_vme);
        ck("clctc header", clctc, clctc_vme);

        // Check if lct bxn matches header bxn
        clctc_bxn_vme	 = (clctc_vme >> 0) & 0x0003;	// Bunch crossing number
        clctc_bxn_header = r_bxn_counter_ff & 0x0003;

        fprintf(stdout,"\nclctc_vme=%8X clctc_bxn_vme=%8X r_bxn_counter_ff=%8X clctc_bxn_header,=%8X pop_l1a_bxn=%8X \n",
                clctc_vme,clctc_bxn_vme,r_bxn_counter_ff,clctc_bxn_header,pop_l1a_bxn);

        ck("clctc lctvmebxn, headerbxn", clctc_bxn_vme, clctc_bxn_header);

        // Check if MPC header matches LCTs and VME
        if ((csc_type_code=='A' || csc_type_code=='B') ||						// Full 160 key csc
                ((csc_type_code=='C' || csc_type_code=='D') && clct0_fullkey<=127))	// ME1A/B do not create mpcs on cfeb4 ikey>=128
        {
            ck("mpc0_frame0 header vs vme", mpc0_frame0, mpc0_frame0_vme);
            ck("mpc0_frame1 header vs vme", mpc0_frame1, mpc0_frame1_vme);
            ck("mpc1_frame0 header vs vme", mpc1_frame0, mpc1_frame0_vme);
            ck("mpc1_frame1 header vs vme", mpc1_frame1, mpc1_frame1_vme);
        }

    }	// close err_check

    //------------------------------------------------------------------------------
    // Check for sequential level 1 accept event numbers, first l1a may not be 1 beco offset
    //------------------------------------------------------------------------------
check_l1a:

    fprintf(stdout,"first_event=%i\n",first_event);

    if(err_check) {
        if (first_event) {
            l1a_rxcount_save	= pop_l1a_rx_counter;	
            l1a_rxcount_expect	= l1a_rxcount_save;
            if (!header_only_short) {	// short header does not have trig counter
                trig_counter_save	= trig_counter;
                trig_counter_expect	= trig_counter_save;
            }
            first_event=false;
        }

        else {
            l1a_rxcount_expect   = (l1a_rxcount_save +1) & 0x0FFF;
            trig_counter_expect  = (trig_counter_save+1) & 0x0FFF;
            ck("l1a rxcount  ", pop_l1a_rx_counter, l1a_rxcount_expect );
            if (!header_only_short) {	// short header does not have trig counter
                ck("l1a rxtxcount", trig_counter,       trig_counter_expect);
            }
            l1a_rxcount_save++;
            trig_counter_save++;
        }
    }

    //------------------------------------------------------------------------------
    // Check sync error, requires long header for clct and mpc sync check
    //------------------------------------------------------------------------------
    if (header_only_short) {
        clctc_sync    = 0;
        mpc_sync_err0 = 0;
        mpc_sync_err1 = 0;
    }

    if	((r_sync_err	!=0) || (clctc_sync	!=0) || (mpc_sync_err0	!=0) || (mpc_sync_err1	!=0)) {
        //	if (err_check && scp_playback) 			// dont flag sync errors from reference tmb beco it always has sync errors	
        if (err_check && (vme_bx0_emu_en==1)) {		    // reference tmb now emulates bx0 so sync errors are detectable
            error_flag[29]=1;	
            fprintf(stdout,"ERRs: Clock bx0 sync error: r_sync_err=%i clctc_sync=%i mpc_sync_err0=%i mpc_sync_err1=%i\n",
                    r_sync_err,clctc_sync,mpc_sync_err0,mpc_sync_err1);
        }
    } 

    //------------------------------------------------------------------------------
    // Check buffer parity error SEU status, requires long header
    //------------------------------------------------------------------------------
    if (!header_only_short) {
        if ((perr_cfeb_ff!=0) || (perr_rpc_ff!=0) || (perr_ff!=0)) {
            header_ok=false;
            error_flag[30]=1;
            fprintf(stdout,"ERRs: RAM SEU parity error: cfeb_ram=%X rpc_ram=%X sum=%X\n",perr_cfeb_ff,perr_rpc_ff,perr_ff);
        }
    }

    if (header_only_short) goto display_header;

    //------------------------------------------------------------------------------
    // Check active_feb list is as expected
    //------------------------------------------------------------------------------
    if (((fifo_mode==1) && (r_febs_read!=0x1F)) ||				// Expect 5 cfebs
            ((fifo_mode==2) && (r_febs_read!=r_active_feb_ff))) {	// Expect hit cfebs
        header_ok=false;
        error_flag[39]=1;
        fprintf(stdout,"ERRs: Active CFEB list sent to DMB does not match CFEBs read out aff=%2.2X read=%2.2X\n",r_active_feb_ff,r_febs_read);
    }

    // Construct expected active_feb from 1st and 2nd clcts
    active_febs_expect = 0;

    // Type A:
    if (csc_type_code=='A')
    {
        if (clct0_vpf==1)
        { 	                                                    active_febs_expect = active_febs_expect | (1 << (clct0_cfeb  ));	// cfebn was hit
            if ((clct0_key < (adjcfeb_dist   )) && (clct0_cfeb!=0)) active_febs_expect = active_febs_expect | (1 << (clct0_cfeb-1));	// cfebn-1 was hit
            if ((clct0_key > (31-adjcfeb_dist)) && (clct0_cfeb!=4)) active_febs_expect = active_febs_expect | (1 << (clct0_cfeb+1));	// cfebn+1 was hit
        }

        if (clct1_vpf==1)
        { 	                                                    active_febs_expect = active_febs_expect | (1 << (clct1_cfeb  ));	// cfebn was hit
            if ((clct1_key < (adjcfeb_dist   )) && (clct1_cfeb!=0)) active_febs_expect = active_febs_expect | (1 << (clct1_cfeb-1));	// cfebn-1 was hit
            if ((clct1_key > (31-adjcfeb_dist)) && (clct1_cfeb!=4)) active_febs_expect = active_febs_expect | (1 << (clct1_cfeb+1));	// cfebn+1 was hit
        }
    }

    // Type B:
    else if (csc_type_code=='B') {
        if (clct0_vpf==1) { 	
            active_febs_expect = active_febs_expect | (1 << (clct0_cfeb  ));	// cfebn was hit
            if ((clct0_key < (adjcfeb_dist   )) && (clct0_cfeb!=0)) active_febs_expect = active_febs_expect | (1 << (clct0_cfeb-1));	// cfebn-1 was hit
            if ((clct0_key > (31-adjcfeb_dist)) && (clct0_cfeb!=4)) active_febs_expect = active_febs_expect | (1 << (clct0_cfeb+1));	// cfebn+1 was hit
        }

        if (clct1_vpf==1) { 	                                                    
            active_febs_expect = active_febs_expect | (1 << (clct1_cfeb  ));	// cfebn was hit
            if ((clct1_key < (adjcfeb_dist   )) && (clct1_cfeb!=0)) active_febs_expect = active_febs_expect | (1 << (clct1_cfeb-1));	// cfebn-1 was hit
            if ((clct1_key > (31-adjcfeb_dist)) && (clct1_cfeb!=4)) active_febs_expect = active_febs_expect | (1 << (clct1_cfeb+1));	// cfebn+1 was hit
        }

        active_febs_flipped=0;
        if (((active_febs_expect >> 0) & 0x1) == 1) active_febs_flipped = active_febs_flipped | 0x10;	// swap bit 0 with 4 
        if (((active_febs_expect >> 1) & 0x1) == 1) active_febs_flipped = active_febs_flipped | 0x8; 	// swap bit 1 with 3 
        if (((active_febs_expect >> 2) & 0x1) == 1) active_febs_flipped = active_febs_flipped | 0x4; 	// swap bit 2 with 2 
        if (((active_febs_expect >> 3) & 0x1) == 1) active_febs_flipped = active_febs_flipped | 0x2; 	// swap bit 3 with 1 
        if (((active_febs_expect >> 4) & 0x1) == 1) active_febs_flipped = active_febs_flipped | 0x1; 	// swap bit 4 with 0 
        active_febs_expect=active_febs_flipped;
    }

    // Type C:
    else if (csc_type_code=='C') {
        if (clct0_vpf==1) { 	                    
            active_febs_expect = active_febs_expect | (1 << (clct0_cfeb  ));	// cfebn was hit
            if (clct0_key < adjcfeb_dist    && clct0_cfeb!=0 && clct0_cfeb!=4) active_febs_expect = active_febs_expect | (1 << (clct0_cfeb-1));	// cfebn-1 was hit
            if (clct0_key > 31-adjcfeb_dist && clct0_cfeb<=2 && clct0_cfeb!=4) active_febs_expect = active_febs_expect | (1 << (clct0_cfeb+1));	// cfebn+1 was hit
        }
        if (clct1_vpf==1) {
            active_febs_expect = active_febs_expect | (1 << (clct1_cfeb  ));	// cfebn was hit
            if (clct1_key < adjcfeb_dist    && clct1_cfeb!=0 && clct1_cfeb!=4) active_febs_expect = active_febs_expect | (1 << (clct1_cfeb-1));	// cfebn-1 was hit
            if (clct1_key > 31-adjcfeb_dist && clct1_cfeb<=2 && clct1_cfeb!=4) active_febs_expect = active_febs_expect | (1 << (clct1_cfeb+1));	// cfebn+1 was hit
        }
    }

    // Type D:
    else if (csc_type_code=='D') {
        if (clct0_vpf==1) {
            active_febs_expect = active_febs_expect | (1 << (clct0_cfeb  ));	// cfebn was hit
            if (clct0_key < adjcfeb_dist    && clct0_cfeb!=0 && clct0_cfeb!=4) active_febs_expect = active_febs_expect | (1 << (clct0_cfeb-1));	// cfebn-1 was hit
            if (clct0_key > 31-adjcfeb_dist && clct0_cfeb<=2 && clct0_cfeb!=4) active_febs_expect = active_febs_expect | (1 << (clct0_cfeb+1));	// cfebn+1 was hit
        }

        if (clct1_vpf==1) {
            active_febs_expect = active_febs_expect | (1 << (clct1_cfeb  ));	// cfebn was hit
            if (clct1_key < adjcfeb_dist    && clct1_cfeb!=0 && clct1_cfeb!=4) active_febs_expect = active_febs_expect | (1 << (clct1_cfeb-1));	// cfebn-1 was hit
            if (clct1_key > 31-adjcfeb_dist && clct1_cfeb<=2 && clct1_cfeb!=4) active_febs_expect = active_febs_expect | (1 << (clct1_cfeb+1));	// cfebn+1 was hit
        }

        active_febs_flipped=0;
        if (((active_febs_expect >> 0) & 0x1) == 1) active_febs_flipped = active_febs_flipped | 0x8;	// swap bit 0 with 3 
        if (((active_febs_expect >> 1) & 0x1) == 1) active_febs_flipped = active_febs_flipped | 0x4; 	// swap bit 1 with 2 
        if (((active_febs_expect >> 2) & 0x1) == 1) active_febs_flipped = active_febs_flipped | 0x2; 	// swap bit 2 with 1 
        if (((active_febs_expect >> 3) & 0x1) == 1) active_febs_flipped = active_febs_flipped | 0x1; 	// swap bit 3 with 0 
        if (((active_febs_expect >> 4) & 0x1) == 1) active_febs_flipped = active_febs_flipped | 0x10; 	// keep bit 4 unchanged 
        active_febs_expect=active_febs_flipped;
    } //close Type D

    // Type Unknown
    else {
        printf("ERRs: Unknown csc_type_code=%c\n",csc_type_code);
        pause("woe is we");
    }

    // Compare expected to indicated active febs
    if (active_febs_expect!=r_active_feb_ff) {
        fprintf(stdout,"ERRs: Active feb list err active_febs_expect=%2.2X r_active_feb_ff=%2.2X\n",active_febs_expect,r_active_feb_ff);
        fprintf(stdout  ,"ERRs: Active feb list err active_febs_expect=%2.2X r_active_feb_ff=%2.2X\n",active_febs_expect,r_active_feb_ff);
    }

    //------------------------------------------------------------------------------
    // Unpack CFEB raw hits
    //------------------------------------------------------------------------------
unpack_cfebs:
    // Check CFEBs exist in readout
    nonzero_triads  = 0;
    ncfebs_met      = 0;

    if ((!header_full) || (r_fifo_tbins<=0) || (r_ncfebs<=0) || (adr_e0b<0)) {
        fprintf(stdout,"Info: No CFEB raw hits in this event\n");
        goto cfeb_done;
    }

    // Check ncfebs is between 1 and 5
    if ((r_ncfebs<=0)||(r_ncfebs>mxcfeb)) {
        header_ok=false;
        error_flag[36]=1; // Expected number of CFEBs 0<r_ncfebs<=5
        fprintf(stdout,"ERRs: Expected number of CFEBs 0<r_ncfebs<=5 %i\n",r_ncfebs);
        goto cfeb_done;
    }

    // Check r_fifo_tbins between 0 and 32
    if (r_fifo_tbins<0 || r_fifo_tbins>32) {
        header_ok=false;
        error_flag[37]=1; // Expected number of CFEB tbins 0<=r_fifo_tbins<=32
        fprintf(stdout,"ERRs: Expected number of CFEB tbins 0<=r_fifo_tbins<=32 %i\n",r_fifo_tbins);
        goto cfeb_done;
    }

    // Clear ds hits array
    for (icfeb  = 0; icfeb  <= mxcfeb-1;       ++icfeb ) {	// Loop over all cfebs
        for (itbin  = 0; itbin  <= r_fifo_tbins-1; ++itbin ) {	// Loop over time bins
            for (ilayer = 0; ilayer <= mxly-1;         ++ilayer) {	// Loop over layers
                for (ids    = 0; ids    <  mxds;           ++ids   ) {	// Loop over hits per block
                    ids_abs=ids+icfeb*8;									// Absolute distrip id
                    read_pat[itbin][ilayer][ids_abs]=0xE;	
                }}}}

    // Loop over cfebs, tbins, layers
    iframe=adr_e0b+1;										// First raw hits frame

    for (icfeb  = 0; icfeb  <= mxcfeb-1;       ++icfeb ) {	// Loop over all cfebs
        icfeb_included = (r_febs_read >> icfeb) & 0x1;			// Extract cfeb included bit
        if (icfeb_included!=1) goto next_cfeb;					// Skip omitted cfebs
        ncfebs_met++;			

        for (itbin  = 0; itbin  <= r_fifo_tbins-1; ++itbin ) {	// Loop over time bins
            for (ilayer = 0; ilayer <= mxly-1;         ++ilayer) {	// Loop over layers

                rdcid  = (vf_data[iframe] >> 12) & 0x7;					// CFEB ID in the dump
                rdtbin = (vf_data[iframe] >>  8) & 0xF;					// Tbin number in the dump
                hits8  =  vf_data[iframe]        & 0xFF;				// 8 triad block

                itbin_expect = itbin & 0xF;								// Tbin numbers wrap at 16

                if(rdcid != icfeb) {
                    fprintf(stdout,"ERRs: cfeb id err in dump: rdcid=%i icfeb=%i iframe=%i\n",rdcid,icfeb,iframe);
                }

                if(rdtbin != itbin_expect) {
                    fprintf(stdout,"ERRs: tbin id err in dump: rdtbin=%i itbin_expect=%i iframe=%i\n",rdtbin,itbin,iframe);
                }

                if((rdcid != icfeb) || (rdtbin != itbin_expect)) 
                    goto cfeb_done;	// If you take this goto, read_pat is not filled

                for (ids=0; ids<mxds; ++ids) {					    // Loop over hits per block
                    hits1=(hits8 >> ids) & 0x1;						// Extract 1 hit
                    ids_abs=ids+icfeb*8;							// Absolute distrip id
                    read_pat[itbin][ilayer][ids_abs]=hits1;			// hit this distrip
                    if(hits1 != 0) nonzero_triads++;				// Count nonzero triads
                    fprintf(stdout,"iframe=%4i vf_data=%5.5X hits8=%i icfeb=%i itbin=%i ids_abs=%i hits1=%i\n",
                            iframe,vf_data[iframe],hits8,icfeb,itbin,ids_abs,hits1);
                }												// Close for ihit

                iframe++;										// Next frame
                if(iframe >= last_frame) {						// Bummer, dude
                    fprintf(stdout,"ERRs: raw hits frame over-run iframe=%i\n",iframe);
                    goto cfeb_done;
                }
            }												// Close for ilayer
        }												// Close for itbin
next_cfeb:
        icfeb_included=0;								// Dummy to keep compiler happy
    }												// Close for icfeb

cfeb_done:
    fprintf(stdout,"Non-zero triad bits=%i\n",nonzero_triads);

    // Check if number of cfebs is correct
    if (ncfebs_met!=r_ncfebs) {
        fprintf(stdout,"ERRs: cfebs in dump do not match included-cfeb list, met=%1i included=%1i\n",ncfebs_met,r_ncfebs);
    }

    //------------------------------------------------------------------------------
    // Unpack RPC raw hits
    //------------------------------------------------------------------------------
    // Check for RPC readout enabled
    if(rd_nrpcs==0 || adr_b04<0) {
        fprintf(stdout,"No RPC frames in this event\n");
        goto rpc_done;
    }

    // Unpack RPC data
    first_rpc_frame = adr_b04+1;
    last_rpc_frame  = first_rpc_frame + (rd_nrpcs*fifo_tbins_rpc*2);

    ipair=0;

    for (iframe=first_rpc_frame; iframe<last_rpc_frame; iframe=iframe+2) {
        rpc_data0=vf_data[iframe];
        rpc_data1=vf_data[iframe+1];

        rpc_lsbs = (rpc_data0 >>  0) & 0xFF;
        rpc_tbin = (rpc_data0 >>  8) & 0x0F;
        rpc_id0  = (rpc_data0 >> 12) & 0x07;

        rpc_msbs = (rpc_data1 >>  0) & 0xFF;
        rpc_bxn  = (rpc_data1 >>  8) & 0x07;
        rpc_flag = (rpc_data1 >> 11) & 0x01;
        rpc_id1  = (rpc_data1 >> 12) & 0x07;

        rpc_data = (rpc_msbs  <<  8) | rpc_lsbs;

        fprintf(stdout,"pair=%3i  ",ipair);
        fprintf(stdout,"frame=%5i  ",iframe); 
        fprintf(stdout,"raw=%5.5X  ",vf_data[iframe]);
        fprintf(stdout,"raw=%5.5X  ",vf_data[iframe+1]);
        fprintf(stdout,"rpc_id0=%1i  ",rpc_id0);
        fprintf(stdout,"rpc_id1=%1i  ",rpc_id1);
        fprintf(stdout,"tbin=%2i  ",rpc_tbin);
        fprintf(stdout,"bxn=%2i  ",rpc_bxn);
        fprintf(stdout,"flag=%1i  ",rpc_flag);
        fprintf(stdout,"pads=%4.4X\n",rpc_data);

        if(rpc_id0 != rpc_id1) {
            fprintf(stdout,"rpc_id mismatch wtf?\n");
        }
        ipair++;
    }	// close for (iframe ..)

rpc_done:

    //------------------------------------------------------------------------------
    // Unpack CFEB Blocked Bits
    //------------------------------------------------------------------------------
    // Check for blocked bits readout enabled
    if (h_bcb_read_enable==1 && adr_bcb==-1) {
        fprintf(stdout,"ERRb: h_bcb_read_enable=%1i adr_bcb=%4i\n",h_bcb_read_enable,adr_bcb);
        if (adr_bcb<0) {
            fprintf(stdout,"No CFEB blocked bit frames in this event\n");
            goto bcb_done;
        }

        // Unpack Blocked bits data
        first_bcb_frame = adr_bcb+1;
        last_bcb_frame  = first_bcb_frame + 20;	// 5 cfebs * 8ds *6ly /12

        for (iframe=first_bcb_frame; iframe<last_bcb_frame; iframe=iframe+4) {
            bcb_data[0] = vf_data[iframe+0];			// 4 frames per cfeb
            bcb_data[1] = vf_data[iframe+1];
            bcb_data[2] = vf_data[iframe+2];
            bcb_data[3] = vf_data[iframe+3];

            bcb_cfebid[0] = (bcb_data[0] >> 12) & 0x7;	// Extract CFEB IDs
            bcb_cfebid[1] = (bcb_data[1] >> 12) & 0x7;
            bcb_cfebid[2] = (bcb_data[2] >> 12) & 0x7;
            bcb_cfebid[3] = (bcb_data[3] >> 12) & 0x7;

            icfeb = (iframe-first_bcb_frame)/4;			// Expected cfeb id

            for (i=0; i<=3; ++i) {
                if (bcb_cfebid[i]!=icfeb) fprintf(stdout,"ERRb: Blocked bits format mismatch: bcb_cfebid=%i icfeb=%i\n",bcb_cfebid[i],icfeb);
            }

            bcb_cfeb_ly[icfeb][0] = ( bcb_data[0] >> 0) & 0xFF;
            bcb_cfeb_ly[icfeb][1] = ((bcb_data[0] >> 8) & 0x0F) | ((bcb_data[1] & 0x0F) << 4);
            bcb_cfeb_ly[icfeb][2] = ( bcb_data[1] >> 4) & 0xFF;
            bcb_cfeb_ly[icfeb][3] = ( bcb_data[2] >> 0) & 0xFF;
            bcb_cfeb_ly[icfeb][4] = ((bcb_data[2] >> 8) & 0x0F) | ((bcb_data[3] & 0x0F) << 4);
            bcb_cfeb_ly[icfeb][5] = ( bcb_data[3] >> 4) & 0xFF;

        }	// close for (iframe..)

        // Expand cfeb blocked bits to 1 bit per word
        for (icfeb =0; icfeb  < mxcfeb; ++ icfeb ) {
            for (ilayer=0; ilayer < mxly;   ++ ilayer) {
                for (ids   =0; ids    < mxds;   ++ ids   ) {
                    blocked_distrips[icfeb][ilayer][ids]=(bcb_cfeb_ly[icfeb][ilayer] >> ids) & 0x1;
                }
            }
        }

        // Display cfeb and ids column markers
        fprintf(stdout,"\n");
        fprintf(stdout,"     Blocked DiStrip Triad Bits Table\n");

        fprintf(stdout,"Cfeb-");
        for (icfeb=0; icfeb < mxcfeb; ++icfeb) { 
            fprintf(stdout,"|"); // display cfeb columns
            for (ids=0;   ids   < mxds;   ++ids  )   
                fprintf(stdout,"%1.1i",icfeb);
        }

        fprintf(stdout,"|\n");

        fprintf(stdout,"Ds---");
        for (icfeb=0; icfeb < mxcfeb; ++icfeb) { 
            fprintf(stdout,"%s|",x);	// display ids columns
            for (ids=0;   ids   < mxds;   ++ids  )   
                fprintf(stdout,"%1.1i",ids%10);}
        fprintf(stdout,"|\n");
        fprintf(stdout,"     ----------------------------------------------\n");

        // Display blocked distrip triads
        for (ilayer=0; ilayer <= mxly-1;         ++ilayer) {
            fprintf(stdout,"Ly%1i  ",ilayer);
            for (ids_abs=0;ids_abs<=39;++ids_abs) {
                if (ids_abs%8==0)
                    fprintf(stdout,"|");

                ids   = ids_abs%8;
                icfeb = ids_abs/8;
                fprintf(stdout,"%1.1x",blocked_distrips[icfeb][ilayer][ids]);
            }	// close for ids_abs
            fprintf(stdout,"|\n");
        }	// close ilayer

        // Read Hot Channel Mask 15 registers, 2 layers each, 3 adrs per cfeb
        bool	display_hcm=false;

        if (display_hcm)
        {
            int hcm001_adr;
            int hcm_adr;
            int hcm_data0;
            int hcm_data1;
            int hot_channel_mask[5][6][8];
            int iadr;
            unsigned long adr;
            long int		vme_read		(unsigned long &adr, unsigned short &rd_data);
            unsigned short rd_data;
            long int	status;

            hcm001_adr=0x4A;

            for (iadr =0; iadr<30; iadr=iadr+2) {

                hcm_adr = hcm001_adr + iadr;
                icfeb   = iadr/6;
                ilayer  = iadr%6;
                adr     = 0xD00000+hcm_adr;
                status  = vme_read(adr,rd_data);

                hcm_data0 = (rd_data >> 0); 
                hcm_data1 = (rd_data >> 8); 

                for (ids=0; ids<=7; ++ids) {
                    hot_channel_mask[icfeb][ilayer+0][ids] = (hcm_data0 >> ids) & 0x1;
                    hot_channel_mask[icfeb][ilayer+1][ids] = (hcm_data1 >> ids) & 0x1;
                }}

            // Display Hot Channel Mask  cfeb and ids column markers
            fprintf(stdout,"\n");
            fprintf(stdout,"     Hot Channel Mask\n");

            fprintf(stdout,"Cfeb-");
            for (icfeb=0; icfeb < mxcfeb; ++icfeb) { fprintf(stdout,"|"); // display cfeb columns
                for (ids=0;   ids   < mxds;   ++ids  )   fprintf(stdout,"%1.1i",icfeb);}
            fprintf(stdout,"|\n");

            fprintf(stdout,"Ds---");
            for (icfeb=0; icfeb < mxcfeb; ++icfeb) { fprintf(stdout,"%s|",x);	// display ids columns
                for (ids=0;   ids   < mxds;   ++ids  )   fprintf(stdout,"%1.1i",ids%10);}
            fprintf(stdout,"|\n");
            fprintf(stdout,"     ----------------------------------------------\n");

            // Display HCM masked distrip triads
            for (ilayer=0; ilayer <= mxly-1;         ++ilayer)        {
                fprintf(stdout,"Ly%1i  ",ilayer);

                for (ids_abs=0;ids_abs<=39;++ids_abs) {
                    if (ids_abs%8==0) {fprintf(stdout,"|");}
                    ids   = ids_abs%8;
                    icfeb = ids_abs/8;
                    fprintf(stdout,"%1.1x",hot_channel_mask[icfeb][ilayer][ids]);
                }	// close for ids_abs
                fprintf(stdout,"|\n");
            }	// close ilayer

        }	// close if display_hcm
    }	// close if h_bcb_read_en

bcb_done:

    //------------------------------------------------------------------------------
    // Display Header Frames
    //------------------------------------------------------------------------------
display_header:

    // First 4 header words must conform to DDU specification
    iframe=0;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"boc                = %4.3X\n",	boc);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=1;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"pop_l1a_bxn        = %4.4X\n",	pop_l1a_bxn);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=2;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"pop_l1a_rx_counter = %4.4X\n",	pop_l1a_rx_counter);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=3;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"readout_counter    = %4.4X\n",	readout_counter);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    // Next 4 words for short header mode
    iframe=4;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"board_id           = %4i\n",		board_id);
    fprintf(stdout,"csc_id             = %4i\n",		csc_id);
    fprintf(stdout,"run_id             = %4i\n",		run_id);
    fprintf(stdout,"h4_buf_q_ovf_err   = %4.1X\n",	h4_buf_q_ovf_err);
    fprintf(stdout,"r_sync_err         = %4.1X\n",	r_sync_err);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=5;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_nheaders         = %4i\n",		r_nheaders);
    fprintf(stdout,"fifo_mode          = %4i  %s\n",	fifo_mode,sfifo_mode.c_str());
    fprintf(stdout,"readout_type       = %4i  %s\n",	readout_type,sreadout_type.c_str());
    fprintf(stdout,"l1a_type           = %4i  %s\n",	l1a_type,sl1a_type.c_str());
    fprintf(stdout,"r_has_buf          = %4i\n",		r_has_buf);
    fprintf(stdout,"r_buf_stalled      = %4i\n",		r_buf_stalled);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=6;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"bd_status_ok       = %4.1i\n",	bd_status_vec[0]);
    fprintf(stdout,"vstat_5p0vs        = %4.1i\n",	bd_status_vec[1]);
    fprintf(stdout,"vstat_3p3v         = %4.1i\n",	bd_status_vec[2]);
    fprintf(stdout,"vstat_1p8v         = %4.1i\n",	bd_status_vec[3]);
    fprintf(stdout,"vstat_1p5v         = %4.1i\n",	bd_status_vec[4]);
    fprintf(stdout,"_t_crit            = %4.1i\n",	bd_status_vec[5]);
    fprintf(stdout,"vsm_ok             = %4.1i\n",	bd_status_vec[6]);
    fprintf(stdout,"vsm_aborted        = %4.1i\n",	bd_status_vec[7]);
    fprintf(stdout,"vsm_cksum_ok       = %4.1i\n",	bd_status_vec[8]);
    fprintf(stdout,"vsm_wdcnt_ok       = %4.1i\n",	bd_status_vec[9]);
    fprintf(stdout,"jsm_ok             = %4.1i\n",	bd_status_vec[10]);
    fprintf(stdout,"jsm_aborted        = %4.1i\n",	bd_status_vec[11]);
    fprintf(stdout,"jsm_cksum_ok       = %4.1i\n",	bd_status_vec[12]);
    fprintf(stdout,"jsm_wdcnt_ok       = %4.1i\n",	bd_status_vec[13]);
    fprintf(stdout,"sm_tck_fpga_ok     = %4.1i\n",	bd_status_vec[14]);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=7;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"revcode            = %4.4X  ",	revcode);
    fprintf(stdout,"Decodes as ");
    fprintf(stdout,"%2.2i/",							id_rev_month);
    fprintf(stdout,"%2.2i/",							id_rev_day);
    fprintf(stdout,"%2.2i xc2v",						id_rev_year);
    fprintf(stdout,"%1.1X000\n",						id_rev_fpga);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    if(header_only_short) goto display_trailer;

    // Full Header-mode words 8-41: Event counters
    iframe=8;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_bxn_counter_ff   = %4.3X\n",	r_bxn_counter_ff);
    fprintf(stdout,"r_tmb_clct0_discard= %4.1X\n",	r_tmb_clct0_discard);
    fprintf(stdout,"r_tmb_clct1_discard= %4.1X\n",	r_tmb_clct1_discard);
    fprintf(stdout,"clock_lock_lost    = %4.1X\n",	clock_lock_lost);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=9;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_pretrig_cnt_lsb  = %4.4X\n",	r_pretrig_counter_lsb);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=10;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_pretrig_cnt_msb  = %4.4X  ",	r_pretrig_counter_msb);
    fprintf(stdout,"Full pretrig_countr= %9i\n",		pretrig_counter);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=11;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_clct_cnt_lsb     = %4.4X\n",	r_clct_counter_lsb);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=12;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_clct_cnt_msb     = %4.4X  ",	r_clct_counter_msb);
    fprintf(stdout,"Full clct_counter  = %9i\n",		clct_counter);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=13;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_trig_cnt_lsb     = %4.4X\n",	r_trig_counter_lsb);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=14;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_trig_cnt_msb     = %4.4X  ",	r_trig_counter_msb);
    fprintf(stdout,"Full trig_counter  = %9i\n",		trig_counter);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=15;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_alct_cnt_lsb     = %4.4X\n",	r_alct_counter_lsb);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=16;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_alct_cnt_msb     = %4.4X  ",	r_alct_counter_msb);
    fprintf(stdout,"Full alct_counter  = %9i\n",		alct_counter);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=17;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_orbit_cnt_lsb    = %4.4X\n",	r_orbit_counter_lsb);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=18;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_orbit_cnt_msb    = %4.4X  ",	r_orbit_counter_msb);
    fprintf(stdout,"Full orbit_counter = %9.8X  =",	uptime_counter);
    fprintf(stdout,"%6i seconds\n",					uptime_sec);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    // CLCT Raw Hits Size
    iframe=19;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_ncfebs           = %4.1X\n",	r_ncfebs);
    fprintf(stdout,"r_fifo_tbins       = %4i\n",		r_fifo_tbins);
    fprintf(stdout,"fifo_pretrig       = %4i\n",		fifo_pretrig);
    fprintf(stdout,"scope_exists       = %4.1X\n",	scope_exists);
    fprintf(stdout,"miniscope_exists   = %4.1X\n",	miniscope_exists);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    // CLCT Configuration
    iframe=20;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"hit_thresh_pretrig = %4.1X\n",	hit_thresh_pretrig);
    fprintf(stdout,"pid_thresh_pretrig = %4.1X\n",	pid_thresh_pretrig);
    fprintf(stdout,"hit_thresh_postdrf = %4.1X\n",	hit_thresh_postdrf);
    fprintf(stdout,"pid_thresh_postdrf = %4.1X\n",	pid_thresh_postdrf);
    fprintf(stdout,"stagger_csc        = %4.1X\n",	stagger_csc);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=21;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"triad_persist      = %4.1X\n",	triad_persist);
    fprintf(stdout,"dmb_thresh         = %4.1X\n",	dmb_thresh);
    fprintf(stdout,"alct_delay         = %4.1X\n",	alct_delay);
    fprintf(stdout,"clct_window        = %4.1X\n",	clct_window);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=22;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);

    fprintf(stdout,"r_trig_source_vec  = %4.4X  ",	r_trig_source_vec);
    fprintf(stdout,"Decodes as ");
    for (i=8;i>=0;--i) fprintf(stdout,"%1i",			(r_trig_source_vec>>i)&0x1);
    fprintf(stdout,"\n");

    fprintf(stdout,"r_layers_hit       = %4.4X  ",	r_layers_hit);
    fprintf(stdout,"Decodes as ");
    for (i=5;i>=0;--i) fprintf(stdout,"%1i",			(r_layers_hit>>i)&0x1);
    fprintf(stdout,"\n");
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=23;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);

    fprintf(stdout,"r_active_feb_ff    = %4.4X  ",	r_active_feb_ff);
    fprintf(stdout,"Decodes as ");
    for (i=4;i>=0;--i) fprintf(stdout,"%1i",			(r_active_feb_ff>>i)&0x1);
    fprintf(stdout,"\n");

    fprintf(stdout,"r_febs_read        = %4.4X  ",	r_febs_read);
    fprintf(stdout,"Decodes as ");
    for (i=4;i>=0;--i) fprintf(stdout,"%1i",			(r_febs_read>>i)&0x1);
    fprintf(stdout,"\n");

    fprintf(stdout,"r_l1a_match_win    = %4.1X\n",	r_l1a_match_win);
    fprintf(stdout,"active_feb_src     = %4.1X\n",	active_feb_src);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);


    // CLCT+ALCT Match Status
    iframe=24;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_tmb_match        = %4.1X\n",	r_tmb_match);
    fprintf(stdout,"r_tmb_alct_only    = %4.1X\n",	r_tmb_alct_only);
    fprintf(stdout,"r_tmb_clct_only    = %4.1X\n",	r_tmb_clct_only);
    fprintf(stdout,"r_tmb_match_win    = %4.1X\n",	r_tmb_match_win);
    fprintf(stdout,"r_no_alct_tmb      = %4.1X\n",	r_no_alct_tmb);
    fprintf(stdout,"r_one_alct_tmb     = %4.1X\n",	r_one_alct_tmb);
    fprintf(stdout,"r_one_clct_tmb     = %4.1X\n",	r_one_clct_tmb);
    fprintf(stdout,"r_two_alct_tmb     = %4.1X\n",	r_two_alct_tmb);
    fprintf(stdout,"r_two_clct_tmb     = %4.1X\n",	r_two_clct_tmb);
    fprintf(stdout,"r_dupe_alct_tmb    = %4.1X\n",	r_dupe_alct_tmb);
    fprintf(stdout,"r_dupe_clct_tmb    = %4.1X\n",	r_dupe_clct_tmb);
    fprintf(stdout,"r_rank_err_tmb     = %4.1X\n",	r_rank_err_tmb);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    // CLCT Trigger Data
    iframe=25;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_clct0_tmb_lsb    = %4.4X\n",	r_clct0_tmb_lsb);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=26;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_clct1_tmb_lsb    = %4.4X\n",	r_clct1_tmb_lsb);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=27;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_clct0_tmb_msb    = %4.2X\n",	r_clct0_tmb_msb);
    fprintf(stdout,"r_clct1_tmb_msb    = %4.2X\n",	r_clct1_tmb_msb);
    fprintf(stdout,"r_clct0_invp       = %4.1X\n",	r_clct0_invp);
    fprintf(stdout,"r_clct1_invp       = %4.1X\n",	r_clct1_invp);
    fprintf(stdout,"r_clct1_busy       = %4.1X\n",	r_clct1_busy);
    fprintf(stdout,"perr_cfeb_ff       = %4.1X  ",	perr_cfeb_ff);
    fprintf(stdout,"Decodes as ");
    for (i=4;i>=0;--i) fprintf(stdout,"%1i",			(perr_cfeb_ff>>i)&0x1);
    fprintf(stdout,"\n");
    fprintf(stdout,"perr_rpc_ff        = %4.1X\n",	perr_rpc_ff);
    fprintf(stdout,"perr_ff            = %4.1X\n",	perr_ff);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    // ALCT Trigger Data
    iframe=28;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_alct0_valid      = %4.1X\n",	r_alct0_valid);
    fprintf(stdout,"r_alct0_quality    = %4.1X\n",	r_alct0_quality);
    fprintf(stdout,"r_alct0_amu        = %4.1X\n",	r_alct0_amu);
    fprintf(stdout,"r_alct0_key        = %4i\n",		r_alct0_key);
    fprintf(stdout,"r_alct_pretrig_win = %4.1X\n",	r_alct_pretrig_win);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=29;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_alct1_valid      = %4.1X\n",	r_alct1_valid);
    fprintf(stdout,"r_alct1_quality    = %4.1X\n",	r_alct1_quality);
    fprintf(stdout,"r_alct1_amu        = %4.1X\n",	r_alct1_amu);
    fprintf(stdout,"r_alct1_key        = %4i\n",		r_alct1_key);
    fprintf(stdout,"drift_delay        = %4i\n",		drift_delay);
    fprintf(stdout,"h_bcb_read_enable  = %4.1X\n",	h_bcb_read_enable);
    fprintf(stdout,"hs_layer_trig      = %4.1X\n",	hs_layer_trig);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=30;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_alct_bxn         = %4.2X\n",	r_alct_bxn);
    fprintf(stdout,"alct_ecc_err       = %4.1X\n",	alct_ecc_err);
    fprintf(stdout,"cfeb_badbits_found = %4.1X\n",	cfeb_badbits_found);
    fprintf(stdout,"cfeb_badbits_blockd= %4.1X\n",	cfeb_badbits_blocked);
    fprintf(stdout,"alct_cfg_done      = %4.1X\n",	alct_cfg_done);
    fprintf(stdout,"bx0_match          = %4.1X\n",	bx0_match);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    // MPC Frames
    iframe=31;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_mpc0_frame0_lsb  = %4.4X\n",	r_mpc0_frame0_lsb);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=32;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_mpc0_frame1_lsb  = %4.4X\n",	r_mpc0_frame1_lsb);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=33;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_mpc1_frame0_lsb  = %4.4X\n",	r_mpc1_frame0_lsb);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=34;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_mpc1_frame1_lsb  = %4.4X\n",	r_mpc1_frame1_lsb);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=35;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_mpc0_frame0_msb  = %4.1X\n",	r_mpc0_frame0_msb);
    fprintf(stdout,"r_mpc0_frame1_msb  = %4.1X\n",	r_mpc0_frame1_msb);
    fprintf(stdout,"r_mpc1_frame0_msb  = %4.1X\n",	r_mpc1_frame0_msb);
    fprintf(stdout,"r_mpc1_frame1_msb  = %4.1X\n",	r_mpc1_frame1_msb);
    fprintf(stdout,"mpc_tx_delay       = %4.1X\n",	mpc_tx_delay);
    fprintf(stdout,"r_mpc_accept       = %4.1X\n",	r_mpc_accept);
    fprintf(stdout,"cfeb_en            = %4.2X  ",	cfeb_en);
    fprintf(stdout,"Decodes as ");
    for (i=4;i>=0;--i) fprintf(stdout,"%1i",			(cfeb_en>>i)&0x1);
    fprintf(stdout,"\n");

    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    // RPC Configuration
    iframe=36;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);

    fprintf(stdout,"rd_rpc_list        = %4.2X  ",	rd_rpc_list);
    fprintf(stdout,"Decodes as ");
    for (i=1;i>=0;--i) fprintf(stdout,"%1i",			(rd_rpc_list>>i)&0x1);
    fprintf(stdout,"\n");

    fprintf(stdout,"rd_nrpcs           = %4.1X\n",	rd_nrpcs);
    fprintf(stdout,"rpc_read_enable    = %4.1X\n",	rpc_read_enable);
    fprintf(stdout,"fifo_tbins_rpc     = %4i\n",		fifo_tbins_rpc);
    fprintf(stdout,"fifo_pretrig_rpc   = %4i\n",		fifo_pretrig_rpc);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    // Buffer Status
    iframe=37;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_wr_buf_adr       = %4.3X\n",	r_wr_buf_adr);
    fprintf(stdout,"r_wr_buf_ready     = %4.1X\n",	r_wr_buf_ready);
    fprintf(stdout,"wr_buf_ready       = %4.1X\n",	wr_buf_ready);
    fprintf(stdout,"buf_q_full         = %4.1X\n",	buf_q_full);
    fprintf(stdout,"buf_q_empty        = %4.1X\n",	buf_q_empty);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=38;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"r_buf_fence_dist   = %4.3X%5i\n",	r_buf_fence_dist,r_buf_fence_dist);
    fprintf(stdout,"buf_q_ovf_err      = %4.1X\n",	buf_q_ovf_err);
    fprintf(stdout,"buf_q_udf_err      = %4.1X\n",	buf_q_udf_err);
    fprintf(stdout,"buf_q_adr_err      = %4.1X\n",	buf_q_adr_err);
    fprintf(stdout,"buf_stalled_once   = %4.1X\n",	buf_stalled_once);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    // Spare Frames
    iframe=39;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"buf_fence_cnt      = %4i\n",		buf_fence_cnt);
    fprintf(stdout,"reverse_hs_csc     = %4.1X\n",	reverse_hs_csc);
    fprintf(stdout,"reverse_hs_me1a    = %4.1X\n",	reverse_hs_me1a);
    fprintf(stdout,"reverse_hs_me1b    = %4.1X\n",	reverse_hs_me1b);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=40;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"buf_fence_cnt_peak = %4i\n",		buf_fence_cnt_peak);
    fprintf(stdout,"r_trig_source_vec9 = %4.1X\n",	r_trig_source_vec9);
    fprintf(stdout,"r_trig_source_vec10= %4.1X\n",	r_trig_source_vec10);
    fprintf(stdout,"tmb_trig_pulse     = %4.1X\n",	tmb_trig_pulse);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=41;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"tmb_allow_alct     = %4.1X\n",	tmb_allow_alct);
    fprintf(stdout,"tmb_allow_clct     = %4.1X\n",	tmb_allow_clct);
    fprintf(stdout,"tmb_allow_match    = %4.1X\n",	tmb_allow_match);
    fprintf(stdout,"tmb_allow_alct_ro  = %4.1X\n",	tmb_allow_alct_ro);
    fprintf(stdout,"tmb_allow_clct_ro  = %4.1X\n",	tmb_allow_clct_ro);
    fprintf(stdout,"tmb_allow_match_ro = %4.1X\n",	tmb_allow_match_ro);
    fprintf(stdout,"tmb_alct_ro        = %4.1X\n",	tmb_alct_ro);
    fprintf(stdout,"tmb_clct_ro        = %4.1X\n",	tmb_clct_ro);
    fprintf(stdout,"tmb_match_ro       = %4.1X\n",	tmb_match_ro);
    fprintf(stdout,"tmb_trig_keep      = %4.1X\n",	tmb_trig_keep);
    fprintf(stdout,"tmb_nontrig_keep   = %4.1X\n",	tmb_nontrig_keep);
    fprintf(stdout,"lyr_thresh_pretrig = %4.1X\n",	lyr_thresh_pretrig);
    fprintf(stdout,"layer_trig_en      = %4.1X\n",	layer_trig_en);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    // EOB Frame
    iframe=42;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"eob                = %4.4X\n",	eob);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    // EOC Frame
    iframe=last_frame-6;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"eoc                = %4.3X\n",	eoc);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    //------------------------------------------------------------------------------
    //	Display Filler Frames
    //------------------------------------------------------------------------------
    if(header_filler) {
        iframe=last_frame-5;
        fprintf(stdout,"\n");
        fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
        fprintf(stdout,"opt2aaa            = %4.4X\n",	opt2aaa);
        fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

        iframe=last_frame-4;
        fprintf(stdout,"\n");
        fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
        fprintf(stdout,"opt5555            = %4.4X\n",	opt5555);
        fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);
    }

    //------------------------------------------------------------------------------
    //	Display Trailer Frames
    //------------------------------------------------------------------------------
display_trailer:
    iframe=last_frame-3;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"lctype3            = %4.1X\n",	lctype3);
    fprintf(stdout,"eof                = %4.3X\n",	eof);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=last_frame-2;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"crc22lsb           = %4.3X\n",	crc22lsb);
    fprintf(stdout,"lctype2            = %4.1X\n",	lctype2);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=last_frame-1;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"crc22msb           = %4.3X\n",	crc22msb);
    fprintf(stdout,"lctype1            = %4.1X\n",	lctype1);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    iframe=last_frame;
    fprintf(stdout,"\n");
    fprintf(stdout,"Header Frame%4i%3.3s %5.5X\n",	iframe,x,vf_data[iframe]);
    fprintf(stdout,"frame_cnt          = %4i\n",		frame_cnt);
    fprintf(stdout,"lctype0            = %4.1X\n",	lctype0);
    fprintf(stdout,"ddu                = %4.1X\n",	ddu[iframe]);

    if(header_only_short) goto exit;
    //------------------------------------------------------------------------------
    //	Display decoded LCT data
    //------------------------------------------------------------------------------
    // CLCT0
    fprintf(stdout,"\n");
    fprintf(stdout,"CLCT0 Decode      %7.7X\n",		clct0);
    fprintf(stdout,"clct0_vpf          = %4i\n",		clct0_vpf);
    fprintf(stdout,"clct0_nhit         = %4i\n",		clct0_nhit);
    fprintf(stdout,"clct0_pat          = %4X\n",		clct0_pat);
    fprintf(stdout,"clct0_key          = %4i\n",		clct0_key);
    fprintf(stdout,"clct0_cfeb         = %4i\n",		clct0_cfeb);
    fprintf(stdout,"clct0_fullkey      = %4i\n",		clct0_fullkey);

    // CLCT1
    fprintf(stdout,"\n");
    fprintf(stdout,"CLCT1 Decode      %7.7X\n",		clct1);
    fprintf(stdout,"clct1_vpf          = %4i\n",		clct1_vpf);
    fprintf(stdout,"clct1_nhit         = %4i\n",		clct1_nhit);
    fprintf(stdout,"clct1_pat          = %4X\n",		clct1_pat);
    fprintf(stdout,"clct1_key          = %4i\n",		clct1_key);
    fprintf(stdout,"clct1_cfeb         = %4i\n",		clct1_cfeb);
    fprintf(stdout,"clct1_fullkey      = %4i\n",		clct1_fullkey);

    // Common
    fprintf(stdout,"\n");
    fprintf(stdout,"clctc_bxn          = %4.3X\n",	clctc_bxn);
    fprintf(stdout,"clctc_sync         = %4i\n",		clctc_sync);

    //------------------------------------------------------------------------------
    //	Display decoded MPC data
    //------------------------------------------------------------------------------
    // MPC0
    fprintf(stdout,"\n");
    fprintf(stdout,"MPC0 Decode From Header\n");
    fprintf(stdout,"mpc_alct0_key      = %4i\n",		mpc_alct0_key);
    fprintf(stdout,"mpc_clct0_pat      = %4.1X\n",	mpc_clct0_pat);
    fprintf(stdout,"mpc_lct0_quality   = %4i\n",		mpc_lct0_quality);
    fprintf(stdout,"mpc_lct0_vpf       = %4.1X\n",	mpc_lct0_vpf);

    fprintf(stdout,"mpc_clct0_key      = %4i\n",		mpc_clct0_key);
    fprintf(stdout,"mpc_clct0_bend     = %4.1X\n",	mpc_clct0_bend);
    fprintf(stdout,"mpc_sync_err0      = %4.1X\n",	mpc_sync_err0);
    fprintf(stdout,"mpc_alct0_bxn      = %4.1X\n",	mpc_alct0_bxn);
    fprintf(stdout,"mpc_bx0_clct       = %4i\n",		mpc_bx0_clct);
    fprintf(stdout,"mpc_csc_id0        = %4i\n",		mpc_csc_id0);

    // MPC1
    fprintf(stdout,"\n");
    fprintf(stdout,"MPC1 Decode From Header\n");
    fprintf(stdout,"mpc_alct1_key      = %4i\n",		mpc_alct1_key);
    fprintf(stdout,"mpc_clct1_pat      = %4.1X\n",	mpc_clct1_pat);
    fprintf(stdout,"mpc_lct1_quality   = %4i\n",		mpc_lct1_quality);
    fprintf(stdout,"mpc_lct1_vpf       = %4.1X\n",	mpc_lct1_vpf);

    fprintf(stdout,"mpc_clct1_key      = %4i\n",		mpc_clct1_key);
    fprintf(stdout,"mpc_clct1_bend     = %4.1X\n",	mpc_clct1_bend);
    fprintf(stdout,"mpc_sync_err1      = %4.1X\n",	mpc_sync_err1);
    fprintf(stdout,"mpc_alct1_bxn      = %4.1X\n",	mpc_alct1_bxn);
    fprintf(stdout,"mpc_bx0_alct       = %4i\n",		mpc_bx0_alct);
    fprintf(stdout,"mpc_csc_id1        = %4i\n",		mpc_csc_id1);

    //------------------------------------------------------------------------------
    //	Display CFEB raw hits triads
    //------------------------------------------------------------------------------
    if (header_only_short || header_only_long) {
        fprintf(stdout,"Skipping triad display for header-only event\n");
        goto check_scope;
    }

    //ntbins=r_fifo_tbins;
    ntbinspre=fifo_pretrig;

    // Display cfeb and ids column markers
    fprintf(stdout,"\n");
    fprintf(stdout,"     Raw Hits Triads\n");

    fprintf(stdout,"Cfeb-");
    for (icfeb=0; icfeb < mxcfeb; ++icfeb) { fprintf(stdout,"|"); // display cfeb columns
        for (ids=0;   ids   < mxds;   ++ids  )   fprintf(stdout,"%1.1i",icfeb);}
    fprintf(stdout,"|\n");

    fprintf(stdout,"Ds---");
    for (icfeb=0; icfeb < mxcfeb; ++icfeb) { 
        fprintf(stdout,"%s|",x);	// display ids columns
        for (ids=0;   ids   < mxds;   ++ids  )   fprintf(stdout,"%1.1i",ids%10);}
    fprintf(stdout,"|\n");
    fprintf(stdout,"Ly Tb\n");

    // Display raw hits
    for (ilayer=0; ilayer <= mxly-1;         ++ilayer)        {
        for (itbin=0;  itbin  <= r_fifo_tbins-1; ++itbin ) { fprintf(stdout,"%1i %2i ",ilayer,itbin);

            for (ids_abs=0;ids_abs<=39;++ids_abs) {
                if (ids_abs%8==0) {fprintf(stdout,"|");}
                fprintf(stdout,"%1.1x",read_pat[itbin][ilayer][ids_abs]);
            }	// close for ids_abs
            fprintf(stdout,"|\n");
        }	// close itbin
        fprintf(stdout,"\n");
    }	// close ilayer

    //------------------------------------------------------------------------------
    // Compare read-back triads to generated triads if running on reference TMB
    //------------------------------------------------------------------------------
    if (err_check) {
        triad_error= false;

        for (ilayer=0;  ilayer<mxly;           ++ilayer ) {
            for (itbin=0;   itbin<=r_fifo_tbins-1; ++itbin  ) {
                for (ids_abs=0; ids_abs<mxdsabs;       ++ids_abs) {

                    wrtbin     = itbin-ntbinspre;
                    triad_read = read_pat[itbin][ilayer][ids_abs];

                    if (wrtbin >=0 ) triad_write = itriad[wrtbin][ids_abs][ilayer];
                    else             triad_write = 0;

                    triad_skipped = (triad_read==0xE) && (triad_write==0);
                    if ((triad_write!=triad_read) && !triad_skipped) {
                        triad_error =true;

                        fprintf(stdout,
                                "Triad readback match error: triad_read=%2X triad_write=%2X ilayer=%3i itbin=%3i ids_abs=%3i ntbinspre=%3i\n",
                                triad_read,triad_write,ilayer,itbin,ids_abs,ntbinspre);
                        fprintf(stdout,
                                "Triad readback match error: triad_read=%2X triad_write=%2X ilayer=%3i itbin=%3i ids_abs=%3i ntbinspre=%3i\n",
                                triad_read,triad_write,ilayer,itbin,ids_abs,ntbinspre);
                    }	// close if triad_write

                }	// close ids_abs
            }	// close itbin
        }	// close ilayer

        if (!triad_error) fprintf(stdout,"Triad readback OK\n");
    }	// close if !scp_playback

    //------------------------------------------------------------------------------
    //	Scope readout
    //------------------------------------------------------------------------------
check_scope:
    // Scan for 06B05 marker
    if(scope_exists !=1) {
        fprintf(stdout,"Skipping scope readout because scope_exists=%1X\n",scope_exists);
        goto skip_scope;
    }

    for (iframe=0; iframe<=last_frame; ++iframe) {
        if(vf_data[iframe] == 0x06B05) {
            iscp_begin=iframe;
            fprintf(stdout,"06B05 scope marker found at frame=%5i\n",iscp_begin);
            goto extract_scope;
        }	// close if vf_data
    }	// close iframe
    fprintf(stdout,"No 06B05 scope marker found\n");
    goto skip_scope;

    // Extract scp data
extract_scope:
    iscp_end=iscp_begin+frame_cntex_scope-1;
    if(iscp_end > last_frame) {
        fprintf(stdout,"Not enough scp data, aborting scp read\n");
        goto skip_scope;
    }

    if(vf_data[iscp_end] == 0x6E05)
        fprintf(stdout,"06E05 scope marker found at frame=%5i\n",iscp_end);
    else {
        fprintf(stdout,"Missing 6E05 marker at adr=%5i. Aborting scope read\n",iscp_end);
        //	goto skip_scope;	////////////////////// TEMPORARY SKIP
    }

    iscp=0;
    for (iframe=iscp_begin+1; iframe<=iscp_end-1; ++iframe) {	//excludes 6B05 and 6E05 markers
        scp_raw_data[iscp]=vf_data[iframe];
        if(iscp > (512*160/16-1)) pause ("iscp ovf in decode_raw_hits.for wtf?");
        fprintf(stdout,"scp debug: %5i%5i%5.4X\n",iscp,iframe,scp_raw_data[iscp]);
        iscp++;
    }

    // Load scope arrays, display channel graph
    fprintf(stdout,"Decode/display raw hits scope data\n");
    scp_arm			= false;
    scp_readout		= false;
    scp_raw_decode	= true;
    scp_silent		= false;
    scp_playback	= true;

    scope160c(scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

skip_scope:
    //------------------------------------------------------------------------------
    //	Miniscope readout
    //------------------------------------------------------------------------------
    // Scan for 06B07 marker
    if(miniscope_exists !=1) {
        fprintf(stdout,"Skipping miniscope readout because miniscope_exists=%1X\n",miniscope_exists);
        goto skip_miniscope;
    }

    for (iframe=0; iframe<=last_frame; ++iframe) {
        if(vf_data[iframe] == 0x06B07) {
            iscp_begin=iframe;
            fprintf(stdout,"06B07 miniscope marker found at frame=%5i\n",iscp_begin);
            goto extract_miniscope;
        }	// close if vf_data
    }	// close iframe
    fprintf(stdout,"No 06B07 scope marker found\n");
    goto skip_miniscope;

    // Extract scp data
extract_miniscope:
    iscp_end=iscp_begin+frame_cntex_miniscope+frame_cntex_b07e07-1;
    if(iscp_end > last_frame) {
        fprintf(stdout,"Not enough miniscp data, aborting miniscp read\n");
        goto skip_miniscope;
    }

    if(vf_data[iscp_end] == 0x6E07)
        fprintf(stdout,"06E07 miniscope marker found at frame=%5i\n",iscp_end);
    else {
        fprintf(stdout,"Missing 6E07 marker at adr=%5i. Aborting miniscope read\n",iscp_end);
    }

    iscp=0;
    for (iframe=iscp_begin+1; iframe<=iscp_end-1; ++iframe) {	//excludes 6B07 and 6E07 markers
        miniscope_data[iscp]=vf_data[iframe];
        fprintf(stdout,"miniscp debug: %5i%5i%5.4X\n",iscp,iframe,miniscope_data[iscp]);
        iscp++;
        if(iscp >= 32) pause ("miniscope iscp ovf in decode_raw_hits.for wtf?");
    }

    // Load miniscope arrays, display channel graph
    fprintf(stdout,"Decode/display miniscope data\n");

    miniscope16(fifo_tbins_mini,miniscope_data);

skip_miniscope:
    //------------------------------------------------------------------------------
    // Future stuff goes here
    //------------------------------------------------------------------------------

    //------------------------------------------------------------------------------
    // All done...wheee
    //------------------------------------------------------------------------------
exit:
    fprintf(stdout,"\n");

    // Check error flags
    for (i=0; i<MXERF; ++ i) {
        if(error_flag[i]!=0) {
            fprintf(stdout,  "Errs: Error flag [%2i]=%i %s\n",i,error_flag[i],error_msg[i].c_str());
        }
        fprintf(stdout,  "Errs: Error flag [%2i]=%i %s\n",i,error_flag[i],error_msg[i].c_str());
    } //close for i<MXERF

    if(header_ok)	
        fprintf(stdout, "Header format OK\n");
    else 			
        fprintf(stdout, "Header format has errors\n");
    
    return;
} //close decode_readout()

//------------------------------------------------------------------------------
// Check data read vs data expected
//------------------------------------------------------------------------------
void ck (char *data_string, int &data_read, int &data_expect)
{
    //Log file
    FILE     *stdout;
    string	stdout_name="vmetst_log.txt";
    stdout = fopen(stdout_name.c_str(),"w");
    
    if (data_read != data_expect) {
        fprintf(stdout,"ERRs: Error in %s: read=%8.8X expect=%8.8X\n",data_string,data_read,data_expect);
        //	pause ("pausing in ck()");
    }

    return;
}

//------------------------------------------------------------------------------
// The bitter end
//------------------------------------------------------------------------------

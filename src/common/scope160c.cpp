//------------------------------------------------------------------------------
//	TMB Internal Scope Readout
// 
//	03/17/03 Initial
//	05/08/03 64 channel version ported from 16ch version
//	05/13/03 96 channel version ported from 64ch version 
//	05/19/03 Mods for auto version
//	05/25/04 128 channel version ported from 96ch version
//	05/24/06 alct channels added
//	10/16/06 Remove rpc2, rpc3 channels, add clct0 key and cfeb
//	10/18/06 Add first_pat
//	11/29/06 Reorder signals for latest firmware
//	10/05/07 Temporary debug signals 127:50
//	12/21/07 Revert to normal signal assigments
//	01/25/08 New channel assignments
//	04/22/08 Added event address channels
//	04/30/08 New channel assigments, new channel name structure
//	07/25/08 Port to C/C++ why
//	09/26/08 Update to 160 channels, fix vme calls
//	09/29/08 Add file close beco can not share io units with fortran
//	10/01/08 Mod for scope demo proggy
//	10/02/08 Mod for playback
//	11/04/08 Replace c character strings with c++ strings
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Headers
//------------------------------------------------------------------------------
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

//------------------------------------------------------------------------------
// Local Headers
//------------------------------------------------------------------------------
#include "emu/pc/common.h"
#include "emu/pc/vme_emulib.h"
#include "emu/pc/scope160c.h"

using namespace std;


//------------------------------------------------------------------------------
// Entry scope160c()
//------------------------------------------------------------------------------
void    scope160c (
        unsigned long	 scp_ctrl_adr,
        unsigned long	 scp_rdata_adr,
        int				 scp_arm,
        int				 scp_readout,
        int				 scp_raw_decode,
        int				 scp_silent,
        int				 scp_playback,
        int				 scp_raw_data[512*160/16]
        ) {

    //Log file
    FILE    *log_file;
    string      log_file_name="vmetst_log.txt";
    log_file =  fopen("vmetst_log.txt","w");

    const int		NCHANNELS	= 160;
    const int		NRAMS		= NCHANNELS/16;
    //const int		NBITS		= NCHANNELS*16;
    const int		NTBINS		= 512;			// tbins per channel
    const int		NDSP		= 512;			// tbins to display
    //const int		NDSP		= 64;			// tbins to display
    const int		DISP_ALL	= 0;			// 1= display hex channel binary levels,0=blank them
    //const int		NFRAMES		= NTBINS*NCHANNELS/16;

    int				i;
    int				itbin;
    int				itbin_max;
    int				iram;
    int				ich;
    int				ibit;
    int				idigit;
    int				iframe;

    int				scope_ram[NTBINS][NRAMS];
    int				scope_ch[NTBINS];
    int				ihex[NTBINS];

    static bool		scp_first_pass = true;
    int				last_bit;
    int				ndigits;

    int				scp_ch_trig_en;
    int				scp_runstop;
    int				scp_forcetrig;
    int				scp_auto;
    int				scp_nowrite;
    int				scp_tbins;
    int				scp_ramsel;
    int				scp_waiting;
    int				scp_trig_done;
    int				scp_state;

    //------------------------------------------------------------------------------
    // Channel labels
    //------------------------------------------------------------------------------
    class label
    {
        public:
            int		nbits;
            int		bit;
            string	tag;
    };
    static label ch[NCHANNELS];

    if (scp_first_pass) {
        //	Channel has nbits       This is bit                  Channel name tag
        //                  |                 |                  |
        // Pre-trigger to DMB
        ch[  0].nbits = 1;	ch[  0].bit = 0;	ch[  0].tag="sequencer pretrig  ";
        ch[  1].nbits = 1;	ch[  1].bit = 0;	ch[  1].tag="triad_tp Cf0Ds1Ly2 ";
        ch[  2].nbits = 1;	ch[  2].bit = 0;	ch[  2].tag="any_cfeb_hit       ";
        ch[  3].nbits = 1;	ch[  3].bit = 0;	ch[  3].tag="active_feb_flag    ";
        ch[  4].nbits = 5;	ch[  4].bit = 0;	ch[  4].tag="active_feb_flag[0] ";
        ch[  5].nbits = 5;	ch[  5].bit = 1;	ch[  5].tag="active_feb_flag[1] ";
        ch[  6].nbits = 5;	ch[  6].bit = 2;	ch[  6].tag="active_feb_flag[2] ";
        ch[  7].nbits = 5;	ch[  7].bit = 3;	ch[  7].tag="active_feb_flag[3] ";
        ch[  8].nbits = 5;	ch[  8].bit = 4;	ch[  8].tag="active_feb_flag[4] ";

        // Pre-trigger CLCT*ALCT matching
        ch[  9].nbits = 1;	ch[  9].bit = 0;	ch[  9].tag="alct_active_feb    ";
        ch[ 10].nbits = 1;	ch[ 10].bit = 0;	ch[ 10].tag="alct_pretrig_win   ";

        // Pre-trigger Processing
        ch[ 11].nbits = 3;	ch[ 11].bit = 0;	ch[ 11].tag="clct_sm_vec[0]     ";
        ch[ 12].nbits = 3;	ch[ 12].bit = 1;	ch[ 12].tag="clct_sm_vec[1]     ";
        ch[ 13].nbits = 3;	ch[ 13].bit = 2;	ch[ 13].tag="clct_sm_vec[2]     ";

        ch[ 14].nbits = 1;	ch[ 14].bit = 0;	ch[ 14].tag="wr_buf_ready       ";
        ch[ 15].nbits = 1;	ch[ 15].bit = 0;	ch[ 15].tag="sequencer pretrig  ";

        ch[ 16].nbits = 12;	ch[ 16].bit = 0;	ch[ 16].tag="bxn_counter[ 0]    ";
        ch[ 17].nbits = 12;	ch[ 17].bit = 1;	ch[ 17].tag="bxn_counter[ 1]    ";
        ch[ 18].nbits = 12;	ch[ 18].bit = 2;	ch[ 18].tag="bxn_counter[ 2]    ";
        ch[ 19].nbits = 12;	ch[ 19].bit = 3;	ch[ 19].tag="bxn_counter[ 3]    ";
        ch[ 20].nbits = 12;	ch[ 20].bit = 4;	ch[ 20].tag="bxn_counter[ 4]    ";
        ch[ 21].nbits = 12;	ch[ 21].bit = 5;	ch[ 21].tag="bxn_counter[ 5]    ";
        ch[ 22].nbits = 12;	ch[ 22].bit = 6;	ch[ 22].tag="bxn_counter[ 6]    ";
        ch[ 23].nbits = 12;	ch[ 23].bit = 7;	ch[ 23].tag="bxn_counter[ 7]    ";
        ch[ 24].nbits = 12;	ch[ 24].bit = 8;	ch[ 24].tag="bxn_counter[ 8]    ";
        ch[ 25].nbits = 12;	ch[ 25].bit = 9;	ch[ 25].tag="bxn_counter[ 9]    ";
        ch[ 26].nbits = 12;	ch[ 26].bit =10;	ch[ 26].tag="bxn_counter[10]    ";
        ch[ 27].nbits = 12;	ch[ 27].bit =11;	ch[ 27].tag="bxn_counter[11]    ";

        ch[ 28].nbits = 1;	ch[ 28].bit = 0;	ch[ 28].tag="discard_nobuf      ";

        // CLCT Pattern Finder Output
        ch[ 29].nbits = 1;	ch[ 29].bit = 0;	ch[ 29].tag="empty              ";
        ch[ 30].nbits = 1;	ch[ 30].bit = 0;	ch[ 30].tag="empty              ";
        ch[ 31].nbits = 1;	ch[ 31].bit = 0;	ch[ 31].tag="sequencer pretrig  ";

        ch[ 32].nbits = 3;	ch[ 32].bit = 0;	ch[ 32].tag="hs_hit_1st[0]      ";
        ch[ 33].nbits = 3;	ch[ 33].bit = 1;	ch[ 33].tag="hs_hit_1st[1]      ";
        ch[ 34].nbits = 3;	ch[ 34].bit = 2;	ch[ 34].tag="hs_hit_1st[2]      ";

        ch[ 35].nbits = 4;	ch[ 35].bit = 0;	ch[ 35].tag="hs_pid_1st[0]      ";
        ch[ 36].nbits = 4;	ch[ 36].bit = 1;	ch[ 36].tag="hs_pid_1st[1]      ";
        ch[ 37].nbits = 4;	ch[ 37].bit = 2;	ch[ 37].tag="hs_pid_1st[2]      ";
        ch[ 38].nbits = 4;	ch[ 38].bit = 3;	ch[ 38].tag="hs_pid_1st[3]      ";

        ch[ 39].nbits = 8;	ch[ 39].bit = 0;	ch[ 39].tag="hs_key_1st[0]      ";
        ch[ 40].nbits = 8;	ch[ 40].bit = 1;	ch[ 40].tag="hs_key_1st[1]      ";
        ch[ 41].nbits = 8;	ch[ 41].bit = 2;	ch[ 41].tag="hs_key_1st[2]      ";
        ch[ 42].nbits = 8;	ch[ 42].bit = 3;	ch[ 42].tag="hs_key_1st[3]      ";
        ch[ 43].nbits = 8;	ch[ 43].bit = 4;	ch[ 43].tag="hs_key_1st[4]      ";
        ch[ 44].nbits = 8;	ch[ 44].bit = 5;	ch[ 44].tag="hs_key_1st[5]      ";
        ch[ 45].nbits = 8;	ch[ 45].bit = 6;	ch[ 45].tag="hs_key_1st[6]      ";
        ch[ 46].nbits = 8;	ch[ 46].bit = 7;	ch[ 46].tag="hs_key_1st[7]      ";

        ch[ 47].nbits = 1;	ch[ 47].bit = 0;	ch[ 47].tag="sequencer pretrig  ";

        ch[ 48].nbits = 3;	ch[ 48].bit = 0;	ch[ 48].tag="hs_hit_2nd[0]      ";
        ch[ 49].nbits = 3;	ch[ 49].bit = 1;	ch[ 49].tag="hs_hit_2nd[1]      ";
        ch[ 50].nbits = 3;	ch[ 50].bit = 2;	ch[ 50].tag="hs_hit_2nd[2]      ";

        ch[ 51].nbits = 4;	ch[ 51].bit = 0;	ch[ 51].tag="hs_pid_2nd[0]      ";
        ch[ 52].nbits = 4;	ch[ 52].bit = 1;	ch[ 52].tag="hs_pid_2nd[1]      ";
        ch[ 53].nbits = 4;	ch[ 53].bit = 2;	ch[ 53].tag="hs_pid_2nd[2]      ";
        ch[ 54].nbits = 4;	ch[ 54].bit = 3;	ch[ 54].tag="hs_pid_2nd[3]      ";

        ch[ 55].nbits = 8;	ch[ 55].bit = 0;	ch[ 55].tag="hs_key_2nd[0]      ";
        ch[ 56].nbits = 8;	ch[ 56].bit = 1;	ch[ 56].tag="hs_key_2nd[1]      ";
        ch[ 57].nbits = 8;	ch[ 57].bit = 2;	ch[ 57].tag="hs_key_2nd[2]      ";
        ch[ 58].nbits = 8;	ch[ 58].bit = 3;	ch[ 58].tag="hs_key_2nd[3]      ";
        ch[ 59].nbits = 8;	ch[ 59].bit = 4;	ch[ 59].tag="hs_key_2nd[4]      ";
        ch[ 60].nbits = 8;	ch[ 60].bit = 5;	ch[ 60].tag="hs_key_2nd[5]      ";
        ch[ 61].nbits = 8;	ch[ 61].bit = 6;	ch[ 61].tag="hs_key_2nd[6]      ";
        ch[ 62].nbits = 8;	ch[ 62].bit = 7;	ch[ 62].tag="hs_key_2nd[7]      ";

        ch[ 63].nbits = 1;	ch[ 63].bit = 0;	ch[ 63].tag="sequencer pretrig  ";

        // CLCT Builder
        ch[ 64].nbits = 1;	ch[ 64].bit = 0;	ch[ 64].tag="clct0_really_valid ";
        ch[ 65].nbits = 1;	ch[ 65].bit = 0;	ch[ 65].tag="clct0_vpf          ";
        ch[ 66].nbits = 1;	ch[ 66].bit = 0;	ch[ 66].tag="clct1_vpf          ";
        ch[ 67].nbits = 1;	ch[ 67].bit = 0;	ch[ 67].tag="clct_push_xtmb     ";
        ch[ 68].nbits = 1;	ch[ 68].bit = 0;	ch[ 68].tag="discard_invp       ";

        // TMB Matching
        ch[ 69].nbits = 1;	ch[ 69].bit = 0;	ch[ 69].tag="alct0_valid        ";
        ch[ 70].nbits = 1;	ch[ 70].bit = 0;	ch[ 70].tag="alct1_valid        ";

        ch[ 71].nbits = 1;	ch[ 71].bit = 0;	ch[ 71].tag="alct_vpf_tprt      ";
        ch[ 72].nbits = 1;	ch[ 72].bit = 0;	ch[ 72].tag="clct_vpf_tprt      ";
        ch[ 73].nbits = 1;	ch[ 73].bit = 0;	ch[ 73].tag="clct_window_tprt   ";

        ch[ 74].nbits = 4;	ch[ 74].bit = 0;	ch[ 74].tag="tmb_match_win[0]   ";
        ch[ 75].nbits = 4;	ch[ 75].bit = 1;	ch[ 75].tag="tmb_match_win[1]   ";
        ch[ 76].nbits = 4;	ch[ 76].bit = 2;	ch[ 76].tag="tmb_match_win[2]   ";
        ch[ 77].nbits = 4;	ch[ 77].bit = 3;	ch[ 77].tag="tmb_match_win[3]   ";

        ch[ 78].nbits = 1;	ch[ 78].bit = 0;	ch[ 78].tag="tmb_alct_discard   ";
        ch[ 79].nbits = 1;	ch[ 79].bit = 0;	ch[ 79].tag="sequencer pretrig  ";
        ch[ 80].nbits = 1;	ch[ 80].bit = 0;	ch[ 80].tag="tmb_clct_discard   ";

        // TMB Match Results
        ch[ 81].nbits = 1;	ch[ 81].bit = 0;	ch[ 81].tag="tmb_trig_pulse     ";
        ch[ 82].nbits = 1;	ch[ 82].bit = 0;	ch[ 82].tag="tmb_trig_keep      ";
        ch[ 83].nbits = 1;	ch[ 83].bit = 0;	ch[ 83].tag="tmb_match          ";
        ch[ 84].nbits = 1;	ch[ 84].bit = 0;	ch[ 84].tag="tmb_alct_only      ";
        ch[ 85].nbits = 1;	ch[ 85].bit = 0;	ch[ 85].tag="tmb_clct_only      ";
        ch[ 86].nbits = 1;	ch[ 86].bit = 0;	ch[ 86].tag="discard_tmbreject  ";

        // MPC
        ch[ 87].nbits = 1;	ch[ 87].bit = 0;	ch[ 87].tag="mpc_xmit_lct0      ";
        ch[ 88].nbits = 1;	ch[ 88].bit = 0;	ch[ 88].tag="mpc_xmit_lct1      ";
        ch[ 89].nbits = 1;	ch[ 89].bit = 0;	ch[ 89].tag="mpc_response_ff    ";

        ch[ 90].nbits = 2;	ch[ 90].bit = 0;	ch[ 90].tag="mpc_accept[0]      ";
        ch[ 91].nbits = 2;	ch[ 91].bit = 1;	ch[ 91].tag="mpc_accept[1]      ";

        // L1A
        ch[ 92].nbits = 1;	ch[ 92].bit = 0;	ch[ 92].tag="l1a_pulse          ";
        ch[ 93].nbits = 1;	ch[ 93].bit = 0;	ch[ 93].tag="l1a_window_open    ";
        ch[ 94].nbits = 1;	ch[ 94].bit = 0;	ch[ 94].tag="l1a_match          ";

        ch[ 95].nbits = 1;	ch[ 95].bit = 0;	ch[ 95].tag="sequencer pretrig  ";

        // Buffer push at L1A
        ch[ 96].nbits = 1;	ch[ 96].bit = 0;	ch[ 96].tag="buf_push at l1a    ";

        ch[ 97].nbits = 7;	ch[ 97].bit = 0;	ch[ 97].tag="buf_push_adr[0]l1a ";
        ch[ 98].nbits = 7;	ch[ 98].bit = 1;	ch[ 98].tag="buf_push_adr[1]l1a ";
        ch[ 99].nbits = 7;	ch[ 99].bit = 2;	ch[ 99].tag="buf_push_adr[2]l1a ";
        ch[100].nbits = 7;	ch[100].bit = 3;	ch[100].tag="buf_push_adr[3]l1a ";
        ch[101].nbits = 7;	ch[101].bit = 4;	ch[101].tag="buf_push_adr[4]l1a ";
        ch[102].nbits = 7;	ch[102].bit = 5;	ch[102].tag="buf_push_adr[5]l1a ";
        ch[103].nbits = 7;	ch[103].bit = 6;	ch[103].tag="buf_push_adr[6]l1a ";

        // DMB Readout
        ch[104].nbits = 1;	ch[104].bit = 0;	ch[104].tag="dmb_dav            ";
        ch[105].nbits = 1;	ch[105].bit = 0;	ch[105].tag="dmb_busy           ";

        ch[106].nbits = 5;	ch[106].bit = 0;	ch[106].tag="read_sm_vec[0]     ";
        ch[107].nbits = 5;	ch[107].bit = 1;	ch[107].tag="read_sm_vec[1]     ";
        ch[108].nbits = 5;	ch[108].bit = 2;	ch[108].tag="read_sm_vec[2]     ";
        ch[109].nbits = 5;	ch[109].bit = 3;	ch[109].tag="read_sm_vec[3]     ";
        ch[110].nbits = 5;	ch[110].bit = 4;	ch[110].tag="read_sm_vec[4]     ";

        ch[111].nbits = 1;	ch[111].bit = 0;	ch[111].tag="sequencer pretrig  ";

        ch[112].nbits = 15;	ch[112].bit = 0;	ch[112].tag="dmb_seq_wdata[0]   ";
        ch[113].nbits = 15;	ch[113].bit = 1;	ch[113].tag="dmb_seq_wdata[1]   ";
        ch[114].nbits = 15;	ch[114].bit = 2;	ch[114].tag="dmb_seq_wdata[2]   ";
        ch[115].nbits = 15;	ch[115].bit = 3;	ch[115].tag="dmb_seq_wdata[3]   ";
        ch[116].nbits = 15;	ch[116].bit = 4;	ch[116].tag="dmb_seq_wdata[4]   ";
        ch[117].nbits = 15;	ch[117].bit = 5;	ch[117].tag="dmb_seq_wdata[5]   ";
        ch[118].nbits = 15;	ch[118].bit = 6;	ch[118].tag="dmb_seq_wdata[6]   ";
        ch[119].nbits = 15;	ch[119].bit = 7;	ch[119].tag="dmb_seq_wdata[7]   ";
        ch[120].nbits = 15;	ch[120].bit = 8;	ch[120].tag="dmb_seq_wdata[8]   ";
        ch[121].nbits = 15;	ch[121].bit = 9;	ch[121].tag="dmb_seq_wdata[9]   ";
        ch[122].nbits = 15;	ch[122].bit = 10;	ch[122].tag="dmb_seq_wdata[10]  ";
        ch[123].nbits = 15;	ch[123].bit = 11;	ch[123].tag="dmb_seq_wdata[11]  ";
        ch[124].nbits = 15;	ch[124].bit = 12;	ch[124].tag="dmb_seq_wdata[12]  ";
        ch[125].nbits = 15;	ch[125].bit = 13;	ch[125].tag="dmb_seq_wdata[13]  ";
        ch[126].nbits = 15;	ch[126].bit = 14;	ch[126].tag="dmb_seq_wdata[14]  ";

        ch[127].nbits = 1;	ch[127].bit = 0;	ch[127].tag="sequencer pretrig  ";

        ch[128].nbits = 1;	ch[128].bit = 0;	ch[128].tag="dmb_seq_wdata[15]  ";

        // CLCT+TMB Pipelines
        ch[129].nbits = 4;	ch[129].bit = 0;	ch[129].tag="wr_buf_adr[0]      ";
        ch[130].nbits = 4;	ch[130].bit = 1;	ch[130].tag="wr_buf_adr[1]      ";
        ch[131].nbits = 4;	ch[131].bit = 2;	ch[131].tag="wr_buf_adr[2]      ";
        ch[132].nbits = 4;	ch[132].bit = 3;	ch[132].tag="wr_buf_adr[3]      ";

        ch[133].nbits = 1;	ch[133].bit = 0;	ch[133].tag="wr_push_xtmb       ";
        ch[134].nbits = 4;	ch[134].bit = 0;	ch[134].tag="wr_adr_xtmb[0]     ";
        ch[135].nbits = 4;	ch[135].bit = 1;	ch[135].tag="wr_adr_xtmb[1]     ";
        ch[136].nbits = 4;	ch[136].bit = 2;	ch[136].tag="wr_adr_xtmb[2]     ";
        ch[137].nbits = 4;	ch[137].bit = 3;	ch[137].tag="wr_adr_xtmb[3]     ";

        ch[138].nbits = 1;	ch[138].bit = 0;	ch[138].tag="wr_push_rtmb       ";
        ch[139].nbits = 4;	ch[139].bit = 0;	ch[139].tag="wr_adr_rtmb[0]     ";
        ch[140].nbits = 4;	ch[140].bit = 1;	ch[140].tag="wr_adr_rtmb[1]     ";
        ch[141].nbits = 4;	ch[141].bit = 2;	ch[141].tag="wr_adr_rtmb[2]     ";
        ch[142].nbits = 4;	ch[142].bit = 3;	ch[142].tag="wr_adr_rtmb[3]     ";

        ch[143].nbits = 1;	ch[143].bit = 0;	ch[143].tag="sequencer pretrig  ";

        ch[144].nbits = 1;	ch[144].bit = 0;	ch[144].tag="wr_push_xmpc       ";
        ch[145].nbits = 4;	ch[145].bit = 0;	ch[145].tag="wr_adr_xmpc[0]     ";
        ch[146].nbits = 4;	ch[146].bit = 1;	ch[146].tag="wr_adr_xmpc[1]     ";
        ch[147].nbits = 4;	ch[147].bit = 2;	ch[147].tag="wr_adr_xmpc[2]     ";
        ch[148].nbits = 4;	ch[148].bit = 3;	ch[148].tag="wr_adr_xmpc[3]     ";

        ch[149].nbits = 1;	ch[149].bit = 0;	ch[149].tag="wr_push_rmpc       ";
        ch[150].nbits = 4;	ch[150].bit = 0;	ch[150].tag="wr_adr_rmpc[0]     ";
        ch[151].nbits = 4;	ch[151].bit = 1;	ch[151].tag="wr_adr_rmpc[1]     ";
        ch[152].nbits = 4;	ch[152].bit = 2;	ch[152].tag="wr_adr_rmpc[2]     ";
        ch[153].nbits = 4;	ch[153].bit = 3;	ch[153].tag="wr_adr_rmpc[3]     ";

        // Buffer pop at readout completion
        ch[154].nbits = 1;	ch[154].bit = 0;	ch[154].tag="buf_pop            ";
        ch[155].nbits = 4;	ch[155].bit = 0;	ch[155].tag="buf_pop_adr[0]     ";
        ch[156].nbits = 4;	ch[156].bit = 1;	ch[156].tag="buf_pop_adr[1]     ";
        ch[157].nbits = 4;	ch[157].bit = 2;	ch[157].tag="buf_pop_adr[2]     ";
        ch[158].nbits = 4;	ch[158].bit = 3;	ch[158].tag="buf_pop_adr[3]     ";

        ch[159].nbits = 1;	ch[159].bit = 0;	ch[159].tag="sequencer pretrig  ";

        scp_first_pass=false;
    }
    //------------------------------------------------------------------------------
    //	scp_status for debugging
    //------------------------------------------------------------------------------
    rd_data=0;
    if(!scp_playback) {
        adr = scp_ctrl_adr;
        status = vme_read(adr,rd_data);
    }

    scp_ch_trig_en	= (rd_data >> 0) & 0x1;
    scp_runstop		= (rd_data >> 1) & 0x1;
    scp_forcetrig	= (rd_data >> 2) & 0x1;
    scp_auto		= (rd_data >> 3) & 0x1;
    scp_nowrite		= (rd_data >> 4) & 0x1;
    scp_tbins		= (rd_data >> 5) & 0x7;
    scp_ramsel		= (rd_data >> 8) & 0xF;
    scp_waiting		= (rd_data >>12) & 0x1;
    scp_trig_done	= (rd_data >>13) & 0x1;

    fprintf(log_file,"\nScope Call\n");
    fprintf(log_file,"----------\n");
    fprintf(log_file,"scp_arm        =%c\n",logical(scp_arm));
    fprintf(log_file,"scp_readout    =%c\n",logical(scp_readout));
    fprintf(log_file,"scp_raw_decode =%c\n",logical(scp_raw_decode));
    fprintf(log_file,"scp_silent     =%c\n",logical(scp_silent));
    fprintf(log_file,"scp_playback   =%c\n",logical(scp_playback));
    fprintf(log_file,"\n");
    fprintf(log_file,"scp_ch_trig_en =%i\n",scp_ch_trig_en);
    fprintf(log_file,"scp_runstop    =%i\n",scp_runstop);
    fprintf(log_file,"scp_forcetrig  =%i\n",scp_forcetrig);
    fprintf(log_file,"scp_auto       =%i\n",scp_auto);
    fprintf(log_file,"scp_nowrite    =%i\n",scp_nowrite);
    fprintf(log_file,"scp_tbins      =%i\n",scp_tbins);
    fprintf(log_file,"scp_ramsel     =%i\n",scp_ramsel);
    fprintf(log_file,"scp_waiting    =%i\n",scp_waiting);
    fprintf(log_file,"scp_trig_done  =%i\n",scp_trig_done);
    //------------------------------------------------------------------------------
    //	scp_arm
    //------------------------------------------------------------------------------
    if (scp_arm)
    {
        // Get current scope control state
        adr = scp_ctrl_adr;
        status = vme_read(adr,rd_data);
        scp_runstop		= (rd_data >> 1) & 0x1;
        scp_forcetrig	= (rd_data >> 2) & 0x1;

        // Reset scope state to idle
        scp_runstop=0;
        scp_forcetrig=0;

        wr_data=rd_data & 0xFFF9;	//clear bits 1,2
        wr_data=wr_data | (scp_runstop   << 1);
        wr_data=wr_data | (scp_forcetrig << 2);
        status = vme_write(adr,wr_data);

        //	status = vme_read(adr),rd_data);	//read scope status
        //	printf("\tScope status %4.4X\n",rd_data);

        // Arm scope for triggering, send scope state to wait_trig
        scp_runstop=1;

        wr_data=rd_data & 0xFFF9;	//clear bits 1,2
        wr_data=wr_data | (scp_runstop   << 1);
        wr_data=wr_data | (scp_forcetrig << 2);
        status = vme_write(adr,wr_data);
        goto exit;
    }	//close scp_arm

    //------------------------------------------------------------------------------
    //	scp_readout
    //------------------------------------------------------------------------------
    // Wait for scope to trigger
    if(scp_readout)
    {
        // Get current scope control state, exit if in auto mode
        adr = scp_ctrl_adr;
        status = vme_read(adr,rd_data);
        scp_auto = (rd_data >> 3) & 0x1;
        if(scp_auto==1) goto exit;					// Exit if in auto mode

        for (i=1; i<=20; ++i) {						// Give it time to store 512 words
            adr = scp_ctrl_adr;
            status = vme_read(adr,rd_data);				// Read scope status
            //	printf("\tScope status %4.4X\n",rd_data);
            scp_trig_done = (rd_data >>13) & 0x1;
            if(scp_trig_done != 0) goto triggered;		// Triggered and done
            if(!scp_silent)printf("Waiting for scope trigger %4i\n",i);
        }

        //	if(!scp_silent) {
        fprintf(stdout, "\tScope never triggered\n");// Bummer, dude
        fprintf(log_file, "Scope never triggered\n");// Bummer, dude
        goto exit;
        //	}

        // Read back embedded scope data
triggered:
        if(!scp_silent)fprintf(stdout,"\tScope triggered\n");

        adr = scp_ctrl_adr;
        status = vme_read(adr,rd_data);				// Read scope status
        scp_state=rd_data;

        for (itbin=0; itbin<=NTBINS-1; ++itbin) {	// Loop over ram addresses
            for (iram=0;  iram <=NRAMS-1;  ++iram )	{	// Loop over ram chips
                adr = scp_ctrl_adr;
                wr_data=scp_state & 0xF0FF;					// Clear ramsel bits 8,9.10,11
                wr_data=wr_data | (iram << 8);				// Ram block select
                status = vme_write(adr,wr_data);

                adr = scp_rdata_adr;
                wr_data=itbin;								// Write ram address
                status = vme_write(adr,wr_data);

                status = vme_read(adr,rd_data);				// Read scope data at this address
                scope_ram[itbin][iram]=rd_data;				// Store 16 parallel in local array

                //	fprintf(log_file,"%3.3i %1i %4.4X\n",itbin,iram,rd_data);
            }		//close iram
        }		//close itbin
        goto display;
    }		//close scp_readout

    //------------------------------------------------------------------------------
    //	scp_raw_decode
    //------------------------------------------------------------------------------
    // load scope_ram from raw-hits format readout
    if(scp_raw_decode) {
        if((scp_auto!=1) && !scp_playback) goto exit;	// Exit if not auto mode

        iframe=0;
        itbin_max=(scp_tbins+1)*64-1;					// user limited number of tbins

        for (iram=0;  iram <=NRAMS-1; ++iram)  {		// Loop over RAM chips
            for (itbin=0; itbin<=NTBINS-1;++itbin) {		// Loop over time bins in ram chip	
                scope_ram[itbin][iram]=0;						// Pad 0s for shorter tbin runs
                if(itbin <= itbin_max) {
                    scope_ram[itbin][iram]=scp_raw_data[iframe];	// Raw hits scope data
                    iframe++;
                }
            }}

        //	iframe=0;
        //	for (iram=0;  iram <=NRAMS-1;  ++iram)  {	// Loop over RAM chips
        //	for (itbin=0; itbin<=itbin_max;++itbin) {	// Loop over time bins in ram chip	
        //	fprintf(log_file,"scope160c frame=%4i%5.4X\n",iframe,scope_ram[itbin][iram]);
        //	iframe++;
        //	}}

    }	//close scp_raw_decode

    //------------------------------------------------------------------------------
    //	Display
    //------------------------------------------------------------------------------
    // Construct waveform
display:
    fprintf(log_file,"\n");

    for (ich=0; ich<=NCHANNELS-1; ++ich)    {	//Loop over scope channels
        iram=ich/16;								//RAM chip has 16 channels
        for (itbin=0; itbin<=NTBINS-1; ++itbin) {	//Time bins per channel

            // Construct binary waveforms for single-bit channels	
            ibit=(scope_ram[itbin][iram] >> (ich % 16)) & 0x1; //Logic levels vs tbin for this channel
            if(ibit==0) scope_ch[itbin]='_';			//Display symbol for logic 0					
            if(ibit==1) scope_ch[itbin]='-';			//Display symbol for logic 1	

            // Build integer for special channel groups
            if (ch[ich].nbits > 1) {
                if (ch[ich].bit == 0) ihex[itbin]=0;
                ihex[itbin] = ihex[itbin] | (ibit << ch[ich].bit);
            }
        } //close itbin

        // Display binary waveforms
        int chblank=(ch[ich].nbits!=1) && !DISP_ALL;	// dont display channels that are hex digits

        if(!chblank) {
            fprintf(log_file,"ch%3.2i  %s",ich,ch[ich].tag.c_str());
            for(i=0;i<NDSP;++i) fprintf(log_file,"%c",scope_ch[i]);
            fprintf(log_file,"\n");
        }

        // Display hex integers for special channel groups
        if (ch[ich].nbits > 1) {
            last_bit=(ch[ich].nbits == (ch[ich].bit + 1));
            if (last_bit) {
                ndigits=(ch[ich].nbits+3)/4;
                for (idigit=ndigits-1; idigit>=0; --idigit) {
                    fprintf(log_file,"ch%3.2i  %s",ich,ch[ich].tag.c_str());
                    for(i=0;i<NDSP;++i) fprintf(log_file,"%1.1X",(ihex[i] >> (4*idigit)) & 0xF);
                    fprintf(log_file,"\n");
                }}}
    }	//close ich

    fprintf(log_file,"\n");

    //------------------------------------------------------------------------------
    // We be done
    //------------------------------------------------------------------------------
exit:
    return;
}
//------------------------------------------------------------------------------
// The bitter end
//------------------------------------------------------------------------------

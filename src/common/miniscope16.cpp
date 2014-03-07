//------------------------------------------------------------------------------
//	TMB Internal Miniscope Readout
// 
//	05/01/09 Initial
//	06/28/10 New channel assignments
//	06/28/10 skip 1st tbin if it contains the wordcount

//------------------------------------------------------------------------------
// Headers
//------------------------------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include <iostream>

//------------------------------------------------------------------------------
// Local Headers
//------------------------------------------------------------------------------
#include "emu/pc/common.h"
#include "emu/pc/miniscope16.h"

//------------------------------------------------------------------------------

using namespace std;

//------------------------------------------------------------------------------
// Entry miniscope16()
//------------------------------------------------------------------------------
void miniscope16 (int ntbins, int miniscope_data[16]) {

    //Log file
    FILE     *log_file;
    string	log_file_name="vmetst_log.txt";
    log_file = fopen(log_file_name.c_str(),"w");

    const int		NCHANNELS	= 16;
    const int		MXTBINS		= 32;			// tbins per channel
    //const int		MXDSP		= 32;			// max tbins to display
    const bool		DISP_ALL	= false;		// true= display hex channel binary levels, false=blank them

    int				itbin;
    int				ich;
    int				ibit;
    int				idigit;
    int				ndisp;

    int				scope_ch[MXTBINS] = {0};
    int				ihex[MXTBINS]     = {0};

    static bool		scp_first_pass    = true;
    bool			skip_1st_tbin     = true;
    int				last_bit;
    int				ndigits;
    bool			chblank;
    int				irow;
    int				it;
    int				tb0;

    char			state[8]={'s','i','p','t','f','h','6','7'};

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
        //	Channel has nbits      This is bit              Channel name tag
        //                 |                 |              |
        ch[ 0].nbits = 1;	ch[ 0].bit = 0;	ch[ 0].tag="any_cfeb_hit       ";

        ch[ 1].nbits = 3;	ch[ 1].bit = 0;	ch[ 1].tag="clct_state_machine ";
        ch[ 2].nbits = 3;	ch[ 2].bit = 1;	ch[ 2].tag="clct_state_machine ";
        ch[ 3].nbits = 3;	ch[ 3].bit = 2;	ch[ 3].tag="clct_state_machine ";

        ch[ 4].nbits = 1;	ch[ 4].bit = 0;	ch[ 4].tag="clct0_vpf          ";
        ch[ 5].nbits = 1;	ch[ 5].bit = 0;	ch[ 5].tag="clct1_vpf          ";
        ch[ 6].nbits = 1;	ch[ 6].bit = 0;	ch[ 6].tag="alct0_vpf          ";
        ch[ 7].nbits = 1;	ch[ 7].bit = 0;	ch[ 7].tag="alct1_vpf          ";
        ch[ 8].nbits = 1;	ch[ 8].bit = 0;	ch[ 8].tag="clct_window        ";
        ch[ 9].nbits = 1;	ch[ 9].bit = 0;	ch[ 9].tag="wr_push_rtmb       ";

        ch[10].nbits = 1;	ch[10].bit = 0;	ch[10].tag="tmb_push_dly       ";
        ch[11].nbits = 1;	ch[11].bit = 0;	ch[11].tag="l1a_pulse          ";
        ch[12].nbits = 1;	ch[12].bit = 0;	ch[12].tag="l1a_window_open    ";
        ch[13].nbits = 1;	ch[13].bit = 0;	ch[13].tag="l1a_push_me        ";

        ch[14].nbits = 1;	ch[14].bit = 0;	ch[14].tag="tmb_special        ";
        ch[15].nbits = 1;	ch[15].bit = 0;	ch[15].tag="ddu_special        ";

        scp_first_pass=false;
    }

    //------------------------------------------------------------------------------
    //	Display
    //------------------------------------------------------------------------------
    fprintf(log_file,"\n");
    ndisp = min(ntbins,MXTBINS);
    tb0   = (skip_1st_tbin) ? 1:0;

    // Display tbin numbers or x if 1st channel blanked
    for (irow =0; irow <=1;       ++irow ) {
        fprintf(log_file,"                          ");
        for (itbin=tb0; itbin<=ndisp-1; ++itbin) {
            if  (irow==0) it = itbin/16;
            else          it = itbin%16;
            fprintf(log_file,"%X",it);
        }	// close itbin
        fprintf(log_file,"\n");
    }	// close irow

    // Construct waveform
    for (ich=0;     ich  <=NCHANNELS-1; ++ich)	{		// Loop over scope channels
        for (itbin=tb0; itbin<=ndisp-1;     ++itbin)	{		// Time bins per channel

            // Construct binary waveforms for single-bit channels	
            ibit=(miniscope_data[itbin] >> (ich % 16)) & 0x1;	// Logic levels vs tbin for this channel
            if(ibit==0) scope_ch[itbin]='_';					// Display symbol for logic 0					
            if(ibit==1) scope_ch[itbin]='-';					// Display symbol for logic 1	

            // Build integer for special channel groups
            if (ch[ich].nbits > 1) {
                if (ch[ich].bit == 0) ihex[itbin]=0;
                ihex[itbin] = ihex[itbin] | (ibit << ch[ich].bit);
            }
        } //close itbin

        // Display binary waveforms
        chblank=(ch[ich].nbits!=1) && !DISP_ALL;		// Dont display channels that are hex digits

        if(!chblank) {
            fprintf(log_file,"ch%3.2i  %s",ich,ch[ich].tag.c_str());
            for(itbin=tb0;itbin<ndisp;++itbin) fprintf(log_file,"%c",scope_ch[itbin]);
            fprintf(log_file,"\n");
        }

        // Display hex integers for special channel groups
        if (ch[ich].nbits > 1) {
            last_bit=(ch[ich].nbits == (ch[ich].bit + 1));
            if (last_bit) {
                ndigits=(ch[ich].nbits+3)/4;
                for (idigit=ndigits-1; idigit>=0; --idigit) {
                    fprintf(log_file,"ch%3.2i  %s",ich,ch[ich].tag.c_str());
                    for(itbin=tb0;itbin<ndisp;++itbin) fprintf(log_file,"%1.1X",(ihex[itbin] >> (4*idigit)) & 0xF);
                    fprintf(log_file,"\n");
                }}}

        // Display machine state ascii
        if(ich==3) {
            fprintf(log_file,"ch%3.2i  %s",ich,ch[ich].tag.c_str());
            for(itbin=tb0;itbin<ndisp;++itbin) fprintf(log_file,"%c",state[ihex[itbin]]);
            fprintf(log_file,"\n");
        }	// close if ich
    }	// close ich

    fprintf(log_file,"\n");

    //------------------------------------------------------------------------------
    // We be done
    //------------------------------------------------------------------------------
    return;
}
//------------------------------------------------------------------------------
// The bitter end
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	CLCT Pattern Finder Emulator
//
// Algorithm: 9-Pattern Front-end 80 MHz pattern-unit duplexing
//
// Process 5 CFEBs:
//		Input	32 1/2-Strips x 6 CSC Layers x 5 CFEBs
//		Output	Best 2 of 160 CLCTs
//			   +DMB pre-trigger signals
//
//	03/30/10 Initial translation from verilog
//------------------------------------------------------------------------------
// Headers
//------------------------------------------------------------------------------
#include <stdio.h>
#include <iostream>
//------------------------------------------------------------------------------
// Local Headers
//------------------------------------------------------------------------------
#include "emu/pc/pause.h"
#include "emu/pc/common.h"
#include "emu/pc/pattern_finder.h"
#include "emu/pc/pattern_unit.h"
using namespace std;

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
const int	MXCFEB		= 	5;				// Number of CFEBs on CSC
const int	MXLY		=	6;				// Number Layers in CSC
const int	MXHS		=	32;				// Number of 1/2-Strips per layer on 1 CFEB
const int	MXHSX		=	MXCFEB*MXHS;	// Number of 1/2-Strips per layer on 5 CFEBs
const int	MXKEYBX		=	8;				// Number of 1/2-strip key bits on 5 CFEBs
const int	MXPIDB		=	4;				// Pattern ID bits

//------------------------------------------------------------------------------
//	Prototypes
//------------------------------------------------------------------------------

void pattern_finder 
(
 // Inputs
 int hs[6][160], 

 int &csc_type, 
 int &clct_sep, 
 int &adjcfeb_dist,
 int	&layer_trig_en,
 int	cfeb_en[5],

 int &hit_thresh_pretrig,
 int &pid_thresh_pretrig,
 int &dmb_thresh_pretrig,
 int &lyr_thresh_pretrig,

 // Outputs
 int cfeb_active[5],
 int &nlayers_hit,
 int	&layer_trig,

 int &hs_key_1st,
 int &hs_pid_1st,
 int &hs_hit_1st,

 int &hs_key_2nd,
 int &hs_pid_2nd,
 int &hs_hit_2nd
 )
{
    //------------------------------------------------------------------------------
    // Local
    //------------------------------------------------------------------------------
    int	i;
    int ihs;

    int	csc_me1ab;
    int	stagger_hs_csc;
    int	reverse_hs_csc;
    int	reverse_hs_me1a;
    int	reverse_hs_me1b;

    //-------------------------------------------------------------------------------------------------------------------
    // Re-Map input array into verilog pattern_finder format
    //-------------------------------------------------------------------------------------------------------------------
    // Alias stage0 array names
    int	*cfeb0_ly0hs=&hs[0][0], *cfeb1_ly0hs=&hs[0][32], *cfeb2_ly0hs=&hs[0][64], *cfeb3_ly0hs=&hs[0][96], *cfeb4_ly0hs=&hs[0][128];
    int	*cfeb0_ly1hs=&hs[1][0], *cfeb1_ly1hs=&hs[1][32], *cfeb2_ly1hs=&hs[1][64], *cfeb3_ly1hs=&hs[1][96], *cfeb4_ly1hs=&hs[1][128];
    int	*cfeb0_ly2hs=&hs[2][0], *cfeb1_ly2hs=&hs[2][32], *cfeb2_ly2hs=&hs[2][64], *cfeb3_ly2hs=&hs[2][96], *cfeb4_ly2hs=&hs[2][128];
    int	*cfeb0_ly3hs=&hs[3][0], *cfeb1_ly3hs=&hs[3][32], *cfeb2_ly3hs=&hs[3][64], *cfeb3_ly3hs=&hs[3][96], *cfeb4_ly3hs=&hs[3][128];
    int	*cfeb0_ly4hs=&hs[4][0], *cfeb1_ly4hs=&hs[4][32], *cfeb2_ly4hs=&hs[4][64], *cfeb3_ly4hs=&hs[4][96], *cfeb4_ly4hs=&hs[4][128];
    int	*cfeb0_ly5hs=&hs[5][0], *cfeb1_ly5hs=&hs[5][32], *cfeb2_ly5hs=&hs[5][64], *cfeb3_ly5hs=&hs[5][96], *cfeb4_ly5hs=&hs[5][128];

    int	*cfebs_ly0hs=&hs[0][0];
    int	*cfebs_ly1hs=&hs[1][0];
    int	*cfebs_ly2hs=&hs[2][0];
    int	*cfebs_ly3hs=&hs[3][0];
    int	*cfebs_ly4hs=&hs[4][0];
    int	*cfebs_ly5hs=&hs[5][0];

    int	me1234_ly0hs[MXHS*5];
    int	me1234_ly1hs[MXHS*5];
    int	me1234_ly2hs[MXHS*5];
    int	me1234_ly3hs[MXHS*5];
    int	me1234_ly4hs[MXHS*5];
    int	me1234_ly5hs[MXHS*5];

    int	me1b_ly0hs[MXHS*4];
    int	me1b_ly1hs[MXHS*4];
    int	me1b_ly2hs[MXHS*4];
    int	me1b_ly3hs[MXHS*4];
    int	me1b_ly4hs[MXHS*4];
    int	me1b_ly5hs[MXHS*4];

    int	me1a_ly0hs[MXHS*1];
    int	me1a_ly1hs[MXHS*1];
    int	me1a_ly2hs[MXHS*1];
    int	me1a_ly3hs[MXHS*1];
    int	me1a_ly4hs[MXHS*1];
    int	me1a_ly5hs[MXHS*1];

    printf(stdout,"dbg: ly0="); for (i=0; i<MXHS; ++i) printf(stdout,"%1i",cfeb0_ly0hs[i]); printf(stdout,"\n");
    printf(stdout,"dbg: ly1="); for (i=0; i<MXHS; ++i) printf(stdout,"%1i",cfeb0_ly1hs[i]); printf(stdout,"\n");
    printf(stdout,"dbg: ly2="); for (i=0; i<MXHS; ++i) printf(stdout,"%1i",cfeb0_ly2hs[i]); printf(stdout,"\n");
    printf(stdout,"dbg: ly3="); for (i=0; i<MXHS; ++i) printf(stdout,"%1i",cfeb0_ly3hs[i]); printf(stdout,"\n");
    printf(stdout,"dbg: ly4="); for (i=0; i<MXHS; ++i) printf(stdout,"%1i",cfeb0_ly4hs[i]); printf(stdout,"\n");
    printf(stdout,"dbg: ly5="); for (i=0; i<MXHS; ++i) printf(stdout,"%1i",cfeb0_ly5hs[i]); printf(stdout,"\n");

    //-------------------------------------------------------------------------------------------------------------------
    // Generate mask for marking adjacent cfeb as hit if nearby keys are over thresh
    //-------------------------------------------------------------------------------------------------------------------
    int adjcfeb_mask_nm1[MXHS];		// Adjacent CFEB active feb flag mask
    int	adjcfeb_mask_np1[MXHS];

    for (ihs=0; ihs<=31; ++ihs) {
        adjcfeb_mask_nm1[ihs]	 = (ihs<adjcfeb_dist) ? 1:0;
        adjcfeb_mask_np1[31-ihs] = (ihs<adjcfeb_dist) ? 1:0;
    }

    //-------------------------------------------------------------------------------------------------------------------
    // Stage 4A1: CSC_TYPE_A Normal CSC
    //-------------------------------------------------------------------------------------------------------------------
    if (csc_type==0xA)
    {

        // Orientation flags
        csc_me1ab	    = 0;			// 1= ME1A or ME1B CSC
        stagger_hs_csc  = 1;			// 1=Staggered CSC non-me1
        reverse_hs_csc  = 1;			// 1=Reversed  CSC non-me1
        reverse_hs_me1a = 0;			// 1=reverse me1a hstrips prior to pattern sorting
        reverse_hs_me1b = 0;			// 1=reverse me1b hstrips prior to pattern sorting

        for (i=0; i<MXHS*5; ++i)
        {
            me1234_ly0hs[i] = cfebs_ly0hs[i];
            me1234_ly1hs[i] = cfebs_ly1hs[i];
            me1234_ly2hs[i] = cfebs_ly2hs[i];
            me1234_ly3hs[i] = cfebs_ly3hs[i];
            me1234_ly4hs[i] = cfebs_ly4hs[i];
            me1234_ly5hs[i] = cfebs_ly5hs[i];
        }
    }	// close  if csc_type

    //-------------------------------------------------------------------------------------------------------------------
    // Stage 4A2: CSC_TYPE_B Reversed CSC
    //-------------------------------------------------------------------------------------------------------------------
    else if (csc_type==0xB)
    {

        // Orientation flags
        csc_me1ab	    = 0;			// 1= ME1A or ME1B CSC
        stagger_hs_csc  = 1;			// 1=Staggered CSC non-me1
        reverse_hs_csc  = 1;			// 1=Reversed  CSC non-me1
        reverse_hs_me1a = 0;			// 1=reverse me1a hstrips prior to pattern sorting
        reverse_hs_me1b = 0;			// 1=reverse me1b hstrips prior to pattern sorting

        // Reverse all CFEBs and reverse layers
        for (i=0; i<MXHS*5; ++i)
        {
            me1234_ly5hs[i] = cfebs_ly0hs[MXHS*5-1-i];
            me1234_ly4hs[i] = cfebs_ly1hs[MXHS*5-1-i];
            me1234_ly3hs[i] = cfebs_ly2hs[MXHS*5-1-i];
            me1234_ly2hs[i] = cfebs_ly3hs[MXHS*5-1-i];
            me1234_ly1hs[i] = cfebs_ly4hs[MXHS*5-1-i];
            me1234_ly0hs[i] = cfebs_ly5hs[MXHS*5-1-i];
        }	// close for i
    }	// close  if csc_type

    //-------------------------------------------------------------------------------------------------------------------
    // Stage 4A3: CSC_TYPE_C Normal ME1B reversed ME1A
    //-------------------------------------------------------------------------------------------------------------------
    else if (csc_type==0xC)
    {

        // Orientation flags
        csc_me1ab		= 1;			// 1= ME1A or ME1B CSC
        stagger_hs_csc  = 0;			// 1=Staggered CSC non-me1
        reverse_hs_csc  = 0;			// 1=Reversed  CSC non-me1
        reverse_hs_me1a = 1;			// 1=reverse me1a hstrips prior to pattern sorting
        reverse_hs_me1b = 0;			// 1=reverse me1b hstrips prior to pattern sorting

        // Reversed ME1A cfebs
        for (i=0; i<MXHS; ++i)
        {
            me1a_ly0hs[i] = cfeb4_ly0hs[MXHS-1-i];
            me1a_ly1hs[i] = cfeb4_ly1hs[MXHS-1-i];
            me1a_ly2hs[i] = cfeb4_ly2hs[MXHS-1-i];
            me1a_ly3hs[i] = cfeb4_ly3hs[MXHS-1-i];
            me1a_ly4hs[i] = cfeb4_ly4hs[MXHS-1-i];
            me1a_ly5hs[i] = cfeb4_ly5hs[MXHS-1-i];
        }	// close for i

        // Normal ME1B cfebs
        for (i=0; i<MXHS*4; ++i)
        {
            me1b_ly0hs[i] = cfebs_ly0hs[i];
            me1b_ly1hs[i] = cfebs_ly1hs[i];
            me1b_ly2hs[i] = cfebs_ly2hs[i];
            me1b_ly3hs[i] = cfebs_ly3hs[i];
            me1b_ly4hs[i] = cfebs_ly4hs[i];
            me1b_ly5hs[i] = cfebs_ly5hs[i];
        }	// close for i
    }	// close  if csc_type

    //-------------------------------------------------------------------------------------------------------------------
    // Stage 4A4: CSC_TYPE_D Normal ME1A reversed ME1B
    //-------------------------------------------------------------------------------------------------------------------
    else if (csc_type==0xD)
    {

        // Orientation flags
        csc_me1ab	    = 1;			// 1= ME1A or ME1B CSC
        stagger_hs_csc  = 0;			// 1=Staggered CSC non-me1
        reverse_hs_csc  = 0;			// 1=Reversed  CSC non-me1
        reverse_hs_me1a = 0;			// 1=reverse me1a hstrips prior to pattern sorting
        reverse_hs_me1b = 1;			// 1=reverse me1b hstrips prior to pattern sorting

        // Normal ME1A cfebs
        for (i=0; i<MXHS; ++i)
        {
            me1a_ly0hs[i] = cfeb4_ly0hs[i];
            me1a_ly1hs[i] = cfeb4_ly1hs[i];
            me1a_ly2hs[i] = cfeb4_ly2hs[i];
            me1a_ly3hs[i] = cfeb4_ly3hs[i];
            me1a_ly4hs[i] = cfeb4_ly4hs[i];
            me1a_ly5hs[i] = cfeb4_ly5hs[i];
        }	// close for i

        // Reversed ME1B cfebs
        for (i=0; i<MXHS*4; ++i)
        {
            me1b_ly0hs[i] = cfebs_ly0hs[MXHS*4-1-i];
            me1b_ly1hs[i] = cfebs_ly1hs[MXHS*4-1-i];
            me1b_ly2hs[i] = cfebs_ly2hs[MXHS*4-1-i];
            me1b_ly3hs[i] = cfebs_ly3hs[MXHS*4-1-i];
            me1b_ly4hs[i] = cfebs_ly4hs[MXHS*4-1-i];
            me1b_ly5hs[i] = cfebs_ly5hs[MXHS*4-1-i];
        }	// close for i
    }	// close  if csc_type

    //-------------------------------------------------------------------------------------------------------------------
    // Stage 4A5: CSC_TYPE_X Undefined
    //-------------------------------------------------------------------------------------------------------------------
    else
        pause("csc_type undefined in pattern_finder.cpp");

    //-------------------------------------------------------------------------------------------------------------------
    // Stage 4B: Correct for CSC layer stagger: 565656 is a straight track, becomes 555555 on key layer 2
    //
    //	ly0hs:   -2 -1 | 00 01 02 03 04 05 06 ... 152 153 154 155 156 157 158 159 | 160 no shift
    //	ly1hs:   -1 00 | 01 02 03 04 05 06 07 ... 153 154 155 156 157 158 159 160 | 161 
    //	ly2hs:   -2 -1 | 00 01 02 03 04 05 06 ... 152 153 154 155 156 157 158 159 | 160 no shift, key layer
    //	ly3hs:   -1 00 | 01 02 03 04 05 06 07 ... 153 154 155 156 157 158 159 160 | 161 
    //	ly4hs:   -2 -1 | 00 01 02 03 04 05 06 ... 152 153 154 155 156 157 158 159 | 160 no shift
    //	ly5hs:   -1 00 | 01 02 03 04 05 06 07 ... 153 154 155 156 157 158 159 160 | 161 
    //-------------------------------------------------------------------------------------------------------------------
    // Create hs arrays with 0s padded at left and right csc edges
    const int k=5;				// Shift negative array indexes positive to compensate for pattern bends off the edges
    const int j=0;				// Shift negative array indexes positive to compensate for stagger

    int ly0hs_pad[k+MXHSX+j+k]={0};
    int ly1hs_pad[k+MXHSX+j+k]={0};
    int ly2hs_pad[k+MXHSX+j+k]={0};
    int ly3hs_pad[k+MXHSX+j+k]={0};
    int ly4hs_pad[k+MXHSX+j+k]={0};
    int ly5hs_pad[k+MXHSX+j+k]={0};

    if (stagger_hs_csc)			// Stagger correction
    {
        for (i=0; i<MXHSX; ++i) {
            ly0hs_pad[i-0+j+k] = me1234_ly0hs[i];
            ly1hs_pad[i-1+j+k] = me1234_ly1hs[i];
            ly2hs_pad[i-0+j+k] = me1234_ly2hs[i];
            ly3hs_pad[i-1+j+k] = me1234_ly3hs[i];
            ly4hs_pad[i-0+j+k] = me1234_ly4hs[i];
            ly5hs_pad[i-1+j+k] = me1234_ly5hs[i];
        }}
    else						// No stagger correction
    {
        for (i=0; i<MXHSX; ++i) {
            ly0hs_pad[i-0+j+k] = (i>=128) ? me1a_ly0hs[i%32] : me1b_ly0hs[i%128];
            ly1hs_pad[i-0+j+k] = (i>=128) ? me1a_ly1hs[i%32] : me1b_ly1hs[i%128];
            ly2hs_pad[i-0+j+k] = (i>=128) ? me1a_ly2hs[i%32] : me1b_ly2hs[i%128];
            ly3hs_pad[i-0+j+k] = (i>=128) ? me1a_ly3hs[i%32] : me1b_ly3hs[i%128];
            ly4hs_pad[i-0+j+k] = (i>=128) ? me1a_ly4hs[i%32] : me1b_ly4hs[i%128];
            ly5hs_pad[i-0+j+k] = (i>=128) ? me1a_ly5hs[i%32] : me1b_ly5hs[i%128];
        }}

    printf(stdout,"dbg: ly0hs_pad="); for (i=0; i<(k+MXHSX+j+k); ++i) printf(stdout,"%1i",ly0hs_pad[i]); printf(stdout,"\n");
    printf(stdout,"dbg: ly1hs_pad="); for (i=0; i<(k+MXHSX+j+k); ++i) printf(stdout,"%1i",ly1hs_pad[i]); printf(stdout,"\n");
    printf(stdout,"dbg: ly2hs_pad="); for (i=0; i<(k+MXHSX+j+k); ++i) printf(stdout,"%1i",ly2hs_pad[i]); printf(stdout,"\n");
    printf(stdout,"dbg: ly3hs_pad="); for (i=0; i<(k+MXHSX+j+k); ++i) printf(stdout,"%1i",ly3hs_pad[i]); printf(stdout,"\n");
    printf(stdout,"dbg: ly4hs_pad="); for (i=0; i<(k+MXHSX+j+k); ++i) printf(stdout,"%1i",ly4hs_pad[i]); printf(stdout,"\n");
    printf(stdout,"dbg: ly5hs_pad="); for (i=0; i<(k+MXHSX+j+k); ++i) printf(stdout,"%1i",ly5hs_pad[i]); printf(stdout,"\n");

    //-------------------------------------------------------------------------------------------------------------------
    // Stage 4C:  Layer-trigger mode
    //-------------------------------------------------------------------------------------------------------------------
    // Layer Trigger Mode, delay 1bx for FF
    int layer_or_s0[MXLY]={0};

    for (i=0; i<MXHSX; ++i)
    {
        layer_or_s0[0] = layer_or_s0[0] | cfebs_ly0hs[i];
        layer_or_s0[1] = layer_or_s0[1] | cfebs_ly1hs[i];
        layer_or_s0[2] = layer_or_s0[2] | cfebs_ly2hs[i];
        layer_or_s0[3] = layer_or_s0[3] | cfebs_ly3hs[i];
        layer_or_s0[4] = layer_or_s0[4] | cfebs_ly4hs[i];
        layer_or_s0[5] = layer_or_s0[5] | cfebs_ly5hs[i];
    }

    // Sum number of layers hit into a binary pattern number
    nlayers_hit=0;

    for (i=0; i<=5; ++i) {
        if (layer_or_s0[i]==1) nlayers_hit++;
    }

    layer_trig = (nlayers_hit >= lyr_thresh_pretrig);

    printf(stdout,"dbg: layer_trig =%1i\n",layer_trig );
    printf(stdout,"dbg: nlayers_hit=%1i\n",nlayers_hit);

    //-------------------------------------------------------------------------------------------------------------------
    // Stage 4D: 1/2-Strip Pattern Finder
    //			 Finds number of hits in pattern templates for each key 1/2-strip.
    //
    //			hs	0123456789A
    //	ly0[10:0]	xxxxxkxxxxx    5+1+5 =11
    //	ly1[ 7:3]	   xxkxx       2+1+2 = 5
    //	ly2[ 5:5]	     k         0+1+0 = 1
    //	ly3[ 7:3]	   xxkxx       2+1+2 = 5
    //	ly4[ 9:1]	 xxxxkxxxx     4+1+4 = 9
    //	ly5[10:0]	xxxxxkxxxxx    5+1+5 =11
    //                                                                   11111111 11111
    //              nnnnn            77777777 88888     77777 88888888   55555555 66666
    //          hs  54321 01234567   23456789 01234     56789 01234567   23456789 01234
    //	ly0[10:0]	00000|aaaaaaaa...aaaaaaaa|bbbbb     aaaaa|bbbbbbbb...bbbbbbbb|00000
    //	ly1[ 7:3]	   0s|aaaaaaaa...aaaaaaaa|bb           aa|bbbbbbbb...bbbbbbb0|00
    //	ly2[ 5:5]	     |aaaaaaaa...aaaaaaaa|               |bbbbbbbb...bbbbbbbb|
    //	ly3[ 7:3]	   0s|aaaaaaaa...aaaaaaaa|bb           aa|bbbbbbbb...bbbbbbb0|00
    //	ly4[ 9:1]	 0000|aaaaaaaa...aaaaaaaa|bbbb       aaaa|bbbbbbbb...bbbbbbbb|0000
    //	ly5[10:0]	0000s|aaaaaaaa...aaaaaaaa|bbbbb     aaaaa|bbbbbbbb...bbbbbbb0|00000
    //
    //-------------------------------------------------------------------------------------------------------------------
    // Find pattern hits for each 1/2-strip key
    int hs_hit [MXHSX];
    int hs_pid [MXHSX];

    for (ihs=0; ihs<MXHSX; ++ihs)
    {
        pattern_unit(
                &ly0hs_pad[ihs],
                &ly1hs_pad[ihs],
                &ly2hs_pad[ihs],	//key on ly2
                &ly3hs_pad[ihs],
                &ly4hs_pad[ihs],
                &ly5hs_pad[ihs],
                hs_hit[ihs],
                hs_pid[ihs]);
        printf(stdout,"dbg: hs_hit[%3i]=%1i hs_pid[%3i]=%1X\n",ihs,hs_hit[ihs],ihs,hs_pid[ihs]);
    }

    // Convert s0 pattern IDs and hits into sortable pattern numbers, [6:4]=nhits, [3:0]=pattern id
    int hs_pat[MXHSX];

    for (ihs=0; ihs<MXHSX; ++ihs)
    {
        hs_pat[ihs] = (hs_hit[ihs]<<4) | hs_pid[ihs];
    }

    //-------------------------------------------------------------------------------------------------------------------
    // Stage 5A: Pre-Trigger Look-ahead
    // 			 Set active FEB bit ASAP if any pattern is over threshold. 
    //			 It comes out before the priority encoder result
    //-------------------------------------------------------------------------------------------------------------------
    // Alias stage0 array names
    int	(&hs_hit_pre_s0)[160](hs_hit);
    int	(&hs_pid_pre_s0)[160](hs_pid);

    // Flag keys with pattern hits over threshold, use fast-out hit numbers before s0 latch
    int	hs_key_hit0[MXHS], hs_key_pid0[MXHS], hs_key_dmb0[MXHS];
    int	hs_key_hit1[MXHS], hs_key_pid1[MXHS], hs_key_dmb1[MXHS];
    int	hs_key_hit2[MXHS], hs_key_pid2[MXHS], hs_key_dmb2[MXHS];
    int	hs_key_hit3[MXHS], hs_key_pid3[MXHS], hs_key_dmb3[MXHS];
    int	hs_key_hit4[MXHS], hs_key_pid4[MXHS], hs_key_dmb4[MXHS];

    // Flag keys with pattern hits over threshold, use fast-out hit numbers before s0 latch
    for (ihs=0; ihs<MXHS; ++ ihs)
    {
        if (csc_type==0xA || csc_type==0xC)										// Unreversed CSC or unreversed ME1B
        {
            hs_key_hit0[ihs] = (hs_hit_pre_s0[ihs+MXHS*0] >= hit_thresh_pretrig);	// Normal CSC
            hs_key_hit1[ihs] = (hs_hit_pre_s0[ihs+MXHS*1] >= hit_thresh_pretrig);
            hs_key_hit2[ihs] = (hs_hit_pre_s0[ihs+MXHS*2] >= hit_thresh_pretrig);
            hs_key_hit3[ihs] = (hs_hit_pre_s0[ihs+MXHS*3] >= hit_thresh_pretrig);
            hs_key_hit4[ihs] = (hs_hit_pre_s0[ihs+MXHS*4] >= hit_thresh_pretrig);

            hs_key_pid0[ihs] = (hs_pid_pre_s0[ihs+MXHS*0] >= pid_thresh_pretrig);
            hs_key_pid1[ihs] = (hs_pid_pre_s0[ihs+MXHS*1] >= pid_thresh_pretrig);
            hs_key_pid2[ihs] = (hs_pid_pre_s0[ihs+MXHS*2] >= pid_thresh_pretrig);
            hs_key_pid3[ihs] = (hs_pid_pre_s0[ihs+MXHS*3] >= pid_thresh_pretrig);
            hs_key_pid4[ihs] = (hs_pid_pre_s0[ihs+MXHS*4] >= pid_thresh_pretrig);

            hs_key_dmb0[ihs] = (hs_hit_pre_s0[ihs+MXHS*0] >= dmb_thresh_pretrig);
            hs_key_dmb1[ihs] = (hs_hit_pre_s0[ihs+MXHS*1] >= dmb_thresh_pretrig);
            hs_key_dmb2[ihs] = (hs_hit_pre_s0[ihs+MXHS*2] >= dmb_thresh_pretrig);
            hs_key_dmb3[ihs] = (hs_hit_pre_s0[ihs+MXHS*3] >= dmb_thresh_pretrig);
            hs_key_dmb4[ihs] = (hs_hit_pre_s0[ihs+MXHS*4] >= dmb_thresh_pretrig);
        }
        else if (csc_type==0xB) 												// Reversed CSC
        {
            hs_key_hit0[ihs] = (hs_hit_pre_s0[MXHS*5-1-ihs] >= hit_thresh_pretrig);	// Reversed CSC
            hs_key_hit1[ihs] = (hs_hit_pre_s0[MXHS*4-1-ihs] >= hit_thresh_pretrig);
            hs_key_hit2[ihs] = (hs_hit_pre_s0[MXHS*3-1-ihs] >= hit_thresh_pretrig);
            hs_key_hit3[ihs] = (hs_hit_pre_s0[MXHS*2-1-ihs] >= hit_thresh_pretrig);
            hs_key_hit4[ihs] = (hs_hit_pre_s0[MXHS*1-1-ihs] >= hit_thresh_pretrig);

            hs_key_pid0[ihs] = (hs_pid_pre_s0[MXHS*5-1-ihs] >= pid_thresh_pretrig);
            hs_key_pid1[ihs] = (hs_pid_pre_s0[MXHS*4-1-ihs] >= pid_thresh_pretrig);
            hs_key_pid2[ihs] = (hs_pid_pre_s0[MXHS*3-1-ihs] >= pid_thresh_pretrig);
            hs_key_pid3[ihs] = (hs_pid_pre_s0[MXHS*2-1-ihs] >= pid_thresh_pretrig);
            hs_key_pid4[ihs] = (hs_pid_pre_s0[MXHS*1-1-ihs] >= pid_thresh_pretrig);

            hs_key_dmb0[ihs] = (hs_hit_pre_s0[MXHS*5-1-ihs] >= dmb_thresh_pretrig);
            hs_key_dmb1[ihs] = (hs_hit_pre_s0[MXHS*4-1-ihs] >= dmb_thresh_pretrig);
            hs_key_dmb2[ihs] = (hs_hit_pre_s0[MXHS*3-1-ihs] >= dmb_thresh_pretrig);
            hs_key_dmb3[ihs] = (hs_hit_pre_s0[MXHS*2-1-ihs] >= dmb_thresh_pretrig);
            hs_key_dmb4[ihs] = (hs_hit_pre_s0[MXHS*1-1-ihs] >= dmb_thresh_pretrig);
        }
        else																	// Reversed ME1B
        {
            hs_key_hit0[ihs] = (hs_hit_pre_s0[MXHS*4-1-ihs] >= hit_thresh_pretrig);	// Reversed ME1B, not reversed ME1A
            hs_key_hit1[ihs] = (hs_hit_pre_s0[MXHS*3-1-ihs] >= hit_thresh_pretrig);
            hs_key_hit2[ihs] = (hs_hit_pre_s0[MXHS*2-1-ihs] >= hit_thresh_pretrig);
            hs_key_hit3[ihs] = (hs_hit_pre_s0[MXHS*1-1-ihs] >= hit_thresh_pretrig);
            hs_key_hit4[ihs] = (hs_hit_pre_s0[ihs+MXHS*4]   >= hit_thresh_pretrig);

            hs_key_pid0[ihs] = (hs_pid_pre_s0[MXHS*4-1-ihs] >= pid_thresh_pretrig);
            hs_key_pid1[ihs] = (hs_pid_pre_s0[MXHS*3-1-ihs] >= pid_thresh_pretrig);
            hs_key_pid2[ihs] = (hs_pid_pre_s0[MXHS*2-1-ihs] >= pid_thresh_pretrig);
            hs_key_pid3[ihs] = (hs_pid_pre_s0[MXHS*1-1-ihs] >= pid_thresh_pretrig);
            hs_key_pid4[ihs] = (hs_pid_pre_s0[ihs+MXHS*4]   >= pid_thresh_pretrig);

            hs_key_dmb0[ihs] = (hs_hit_pre_s0[MXHS*4-1-ihs] >= dmb_thresh_pretrig);
            hs_key_dmb1[ihs] = (hs_hit_pre_s0[MXHS*3-1-ihs] >= dmb_thresh_pretrig);
            hs_key_dmb2[ihs] = (hs_hit_pre_s0[MXHS*2-1-ihs] >= dmb_thresh_pretrig);
            hs_key_dmb3[ihs] = (hs_hit_pre_s0[MXHS*1-1-ihs] >= dmb_thresh_pretrig);
            hs_key_dmb4[ihs] = (hs_hit_pre_s0[ihs+MXHS*4]   >= dmb_thresh_pretrig);
        }
    }

    // Output active FEB signal, and adjacent FEBs if hit is near board boundary
    int	cfebnm1_hit[5];	// Adjacent CFEB-1 has a pattern over threshold
    int	cfebnp1_hit[5];	// Adjacent CFEB+1 has a pattern over threshold

    int hs_key_hitpid0[MXHS];
    int hs_key_hitpid1[MXHS];
    int hs_key_hitpid2[MXHS];
    int hs_key_hitpid3[MXHS];
    int hs_key_hitpid4[MXHS];
    int cfeb_hit[MXCFEB];

    for (i=0; i<MXHS; ++i) {
        hs_key_hitpid0[i] = hs_key_hit0[i] & hs_key_pid0[i];	// hits on key satify both hit and pid thresholds
        hs_key_hitpid1[i] = hs_key_hit1[i] & hs_key_pid1[i];
        hs_key_hitpid2[i] = hs_key_hit2[i] & hs_key_pid2[i];
        hs_key_hitpid3[i] = hs_key_hit3[i] & hs_key_pid3[i];
        hs_key_hitpid4[i] = hs_key_hit4[i] & hs_key_pid4[i];
    }

    int cfeb_layer_trigger = layer_trig && layer_trig_en;

    cfeb_hit[0] = (arrayOr(hs_key_hitpid0) || cfeb_layer_trigger) && cfeb_en[0];
    cfeb_hit[1] = (arrayOr(hs_key_hitpid1) || cfeb_layer_trigger) && cfeb_en[1];
    cfeb_hit[2] = (arrayOr(hs_key_hitpid2) || cfeb_layer_trigger) && cfeb_en[2];
    cfeb_hit[3] = (arrayOr(hs_key_hitpid3) || cfeb_layer_trigger) && cfeb_en[3];
    cfeb_hit[4] = (arrayOr(hs_key_hitpid4) || cfeb_layer_trigger) && cfeb_en[4];

    cfebnm1_hit[1]	= arrayOr(arrayAnd(hs_key_hitpid1, adjcfeb_mask_nm1));
    cfebnm1_hit[2]	= arrayOr(arrayAnd(hs_key_hitpid2, adjcfeb_mask_nm1));
    cfebnm1_hit[3]	= arrayOr(arrayAnd(hs_key_hitpid3, adjcfeb_mask_nm1));
    cfebnm1_hit[4]	= arrayOr(arrayAnd(hs_key_hitpid4, adjcfeb_mask_nm1)) && !csc_me1ab;	// Turn off adjacency for me1ab

    cfebnp1_hit[0]	= arrayOr(arrayAnd(hs_key_hitpid0, adjcfeb_mask_np1));
    cfebnp1_hit[1]	= arrayOr(arrayAnd(hs_key_hitpid1, adjcfeb_mask_np1));
    cfebnp1_hit[2]	= arrayOr(arrayAnd(hs_key_hitpid2, adjcfeb_mask_np1));
    cfebnp1_hit[3]	= arrayOr(arrayAnd(hs_key_hitpid3, adjcfeb_mask_np1)) && !csc_me1ab;	// Turn off adjacency for me1ab

    // Output active FEB signal, and adjacent FEBs if hit is near board boundary
    cfeb_active[0]	=	(cfeb_hit[0] || cfebnm1_hit[1] || arrayOr(hs_key_dmb0)) && cfeb_en[0];
    cfeb_active[1]	=	(cfeb_hit[1] || cfebnp1_hit[0] || cfebnm1_hit[2] || arrayOr(hs_key_dmb1)) && cfeb_en[1];
    cfeb_active[2]	=	(cfeb_hit[2] || cfebnp1_hit[1] || cfebnm1_hit[3] || arrayOr(hs_key_dmb2)) && cfeb_en[2];
    cfeb_active[3]	=	(cfeb_hit[3] || cfebnp1_hit[2] || cfebnm1_hit[4] || arrayOr(hs_key_dmb3)) && cfeb_en[3];
    cfeb_active[4]	=	(cfeb_hit[4] || cfebnp1_hit[3]                   || arrayOr(hs_key_dmb4)) && cfeb_en[4];

    //-------------------------------------------------------------------------------------------------------------------
    // Stage 5B: 1/2-Strip Priority Encoder
    // 			 Select the 1st best pattern from 160 Key 1/2-Strips
    //-------------------------------------------------------------------------------------------------------------------
    // Best 1 of 160 1/2-strip patterns
    int	hs_pat_1st=0;	// pat[6:4]=nhits, pat[3:0]=pattern id

    hs_key_1st=0;
    hs_pid_1st=0;
    hs_hit_1st=0;

    for (ihs=0; ihs<MXHSX; ++ihs)
    {
        if (hs_pat[ihs]>hs_pat_1st)
        {
            hs_pat_1st=hs_pat[ihs];
            hs_key_1st=ihs;
        }
    }

    hs_pid_1st = (hs_pat_1st >> 0     ) & 0xF;
    hs_hit_1st = (hs_pat_1st >> MXPIDB) & 0x7;

    printf(stdout,"dbg: hs_key_1st=%3i\n",hs_key_1st);
    printf(stdout,"dbg: hs_pid_1st=%3X\n",hs_pid_1st);
    printf(stdout,"dbg: hs_hit_1st=%3i\n",hs_hit_1st);

    //-------------------------------------------------------------------------------------------------------------------
    // Stage 6B: Mark key 1/2-strips near the 1st CLCT key as busy to exclude them from 2nd CLCT priority encoding
    //-------------------------------------------------------------------------------------------------------------------
    int	nspan;
    int	pspan;
    int	busy_min;
    int	busy_max;
    int	clct0_is_on_me1a;
    int	hs_key_s2;

    nspan = clct_sep;
    pspan = clct_sep;
    hs_key_s2 = hs_key_1st;

    // CSC Type A or B delimiters for excluding 2nd clct span hs0-159
    if (csc_type==0xA || csc_type==0xB) {
        busy_max = (hs_key_s2 <= 159-pspan) ? hs_key_s2+pspan : 159;	// Limit busy list to range 0-159
        busy_min = (hs_key_s2 >= nspan    ) ? hs_key_s2-nspan : 0;
    }

    // CSC Type C or D delimiters for excluding 2nd clct span ME1B hs0-127  ME1A hs128-159
    else if (csc_type==0xC || csc_type==0xD) {

        clct0_is_on_me1a = hs_key_1st >> (MXKEYBX-1);

        if (clct0_is_on_me1a) {		// CLCT0 is on ME1A cfeb4, limit blanking region to 128-159
            busy_max = (hs_key_s2 <= 159-pspan) ? hs_key_s2+pspan : 159;
            busy_min = (hs_key_s2 >= 128+nspan) ? hs_key_s2-nspan : 128;
        }
        else {						// CLCT0 is on ME1B cfeb0-cfeb3, limit blanking region to 0-127
            busy_max = (hs_key_s2 <= 127-pspan) ? hs_key_s2+pspan : 127;
            busy_min = (hs_key_s2 >=     nspan) ? hs_key_s2-nspan : 0;
        }
    }

    // CSC Type missing
    else
        pause("CSC_TYPE undefined for 2nd clct delimiters in pattern_finder.v: Halting");

    // Latch busy key 1/2-strips for excluding 2nd clct
    int	busy_key[MXHSX];
    int	ikey;

    for (ikey=0; ikey<MXHSX; ++ikey) {
        busy_key[ikey] = (ikey>=busy_min && ikey<=busy_max);
    }

    printf(stdout,"\n");
    printf(stdout,"dbg: busy_min=%3i\n",busy_min);
    printf(stdout,"dbg: busy_max=%3i\n",busy_max);
    printf(stdout,"dbg: busy_key="); for (i=0; i<MXHSX; ++i) printf(stdout,"%1i",busy_key[i]); printf(stdout,"\n");

    //-------------------------------------------------------------------------------------------------------------------
    // Stage 7A: 1/2-Strip Priority Encoder
    // 			Find 2nd best of 160 patterns, excluding busy region around 1st best key
    //-------------------------------------------------------------------------------------------------------------------
    // Best 1 of 160 1/2-strip patterns
    int hs_key_s5=0;
    int hs_pat_s5=0;	// pat[6:4]=nhits, pat[3:0]=pattern id
    int hs_pid_s5=0;
    int hs_hit_s5=0;
    int	hs_bsy_s5=0;

    for (ihs=0; ihs<MXHSX; ++ihs)
    {
        if (hs_pat[ihs]>hs_pat_s5 && !busy_key[ihs])
        {
            hs_pat_s5 = hs_pat[ihs];
            hs_key_s5 = ihs;
            hs_bsy_s5 = busy_key[ihs];
        }
    }

    // Latch final 2nd CLCT
    bool	blank_2nd;
    int		hs_bsy_2nd;
    int		clct_blanking=1;

    hs_hit_s5    = (hs_pat_s5 >> MXPIDB);
    blank_2nd    = (hs_hit_s5==0 && clct_blanking==1);

    if (blank_2nd) {
        hs_pid_2nd	= 0;
        hs_hit_2nd	= 0;
        hs_key_2nd	= 0;
        hs_bsy_2nd	= hs_bsy_s5;
    }
    else {									// else assert final 2nd clct
        hs_pid_2nd	= (hs_pat_s5 >> 0) & 0xF;
        hs_hit_2nd	= (hs_pat_s5 >> 4) & 0x7;
        hs_key_2nd	=  hs_key_s5;
        hs_bsy_2nd	=  hs_bsy_s5;
    }
    if (hs_bsy_2nd!=0) pause("pattern_finder hs_busy_2nd!=0 wtf?!");

    printf(stdout,"dbg: hs_key_2nd=%3i\n",hs_key_2nd);
    printf(stdout,"dbg: hs_pid_2nd=%3X\n",hs_pid_2nd);
    printf(stdout,"dbg: hs_hit_2nd=%3i\n",hs_hit_2nd);

    return;
}

//------------------------------------------------------------------------------------------
// Logical OR returns the OR of all the array elements as a single integer
//------------------------------------------------------------------------------------------
inline int arrayOr(int array[32])
{
    int ior = 0;
    int	i;

    for (i=0; i<32; ++i) {
        ior = ior | array[i];
    }

    return ior;

}
//------------------------------------------------------------------------------------------
// Array AND returns an array of the AND of two arrays
//------------------------------------------------------------------------------------------
inline int *arrayAnd(int array1[32], int array2[32])
{
    int *iand = new int[32];
    int	 i;

    for (i=0; i<32; ++i) {
        iand[i] = array1[i] & array2[i];
    }
    return iand;
}

//------------------------------------------------------------------------------------------
// End pattern_finder
//------------------------------------------------------------------------------------------

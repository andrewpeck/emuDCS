//------------------------------------------------------------------------------
// LCT Quality
//
// 01/17/08	Initial
// 01/17/08 Q=4 reserved, Q=3-1 shifted down
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// System Headers
//------------------------------------------------------------------------------
	#include <stdio.h>
//------------------------------------------------------------------------------
// Local Headers
//------------------------------------------------------------------------------
    #include "emu/pc/common.h"
    #include "emu/pc/lct_quality.h"

//------------------------------------------------------------------------------------------
// Entry lct_quality()
//------------------------------------------------------------------------------------------
	void lct_quality(int &ACC, int &A, int &C, int &A4, int &C4, int &P, int &CPAT, int &Q) {
//------------------------------------------------------------------------------------------
// Ports
//	input			ACC;		// ALCT accelerator muon bit
//	input			A;			// bit: ALCT was found
//	input			C;			// bit: CLCT was found
//	input			A4;			// bit (N_A>=4), where N_A=number of ALCT layers
//	input			C4;			// bit (N_C>=4), where N_C=number of CLCT layers
//	input	[3:0]	P;			// 4-bit CLCT pattern number that is presently 1 for n-layer triggers, 2-10 for current patterns, 11-15 "for future expansion".
//	input			CPAT;		// bit for cathode .pattern trigger., i.e. (P>=2  &&  P<=10) at present
//	output	[3:0]	Q;			// 4-bit TMB quality output
//
// Quality-by-quality definition
//	reg [3:0] Q;
//
//	always @* begin
//
    //------------------------------------------------------------------------------
    // Log File
    //------------------------------------------------------------------------------
    FILE     *log_file;
    string	log_file_name="vmetst_log.txt";
    log_file = fopen(log_file_name.c_str(),"w");
	if		(C4 && (P == 10)          && A4 && !ACC)	Q=15;	// HQ muon, straight
	else if	(C4 && (P == 8 || P == 9) && A4 && !ACC)	Q=14;	// HQ muon, slight bend
	else if	(C4 && (P == 6 || P == 7) && A4 && !ACC)	Q=13;	// HQ muon, more	"
	else if	(C4 && (P == 4 || P == 5) && A4 && !ACC)	Q=12;	// HQ muon, more	"
	else if	(C4 && (P == 2 || P == 3) && A4 && !ACC)	Q=11;	// HQ muon, more	"
//														Q=10;	// reserved for HQ muons with future patterns
//														Q=9;	// reserved for HQ muons with future patterns
	else if	(C4 && CPAT             && A4 && ACC)		Q=8;	// HQ muon, but accel ALCT
	else if	(C4 && CPAT             && A  && !A4)		Q=7;	// HQ cathode, but marginal anode
	else if	(C  && !C4 && CPAT && A4 )					Q=6;	// HQ anode, but marginal cathode
	else if	(C  && !C4 && CPAT && A && !A4)				Q=5;	// marginal anode and cathode
//														Q=4;	// reserved for LQ muons with 2D information in the future
	else if	(C  && A && (P == 1))						Q=3;	// any match but layer CLCT
	else if(C  && !A)									Q=2;	// some CLCT, no ALCT (unmatched)
	else if(A  && !C)									Q=1;	// some ALCT, no CLCT (unmatched)
	else												Q=0;	// should never be assigned
	
//	endmodule
	fprintf(log_file,"ACC=%3i A=%3i C=%3i A4=%3i C4=%3i P=%3i CPAT=%3i,Q=%3i\n",ACC,A,C,A4,C4,P,CPAT,Q);
	return;
}

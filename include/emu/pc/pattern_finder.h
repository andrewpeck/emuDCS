#ifndef PATTERN_FINDER_H
#define PATTERN_FINDER_H
//------------------------------------------------------------------------------
//	CLCT Pattern Finder Emulator
//
// Algorithm: 9-Pattern Front-end 80 MHz pattern-unit duplexing
//
// Process 5 CFEBs:
//		Input	32 1/2-Strips x 6 CSC Layers x 5 CFEBs
//		Output	Best 2 of 160 CLCTs
//			   +DMB pre-trigger signals
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Headers
//------------------------------------------------------------------------------
	#include <stdio.h>
	#include <iostream>

//------------------------------------------------------------------------------
//	Debug print mode
//------------------------------------------------------------------------------
//	#define debug 1	// comment this line to turn off debug print

	#ifdef debug
	 #define dprintf fprintf
	#else
	 #define dprintf //
	#endif

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
	); 
//------------------------------------------------------------------------------------------
// Logical OR returns the OR of all the array elements as a single integer
//------------------------------------------------------------------------------------------
	inline int arrayOr(int array[32]); 
//------------------------------------------------------------------------------------------
// Array AND returns an array of the AND of two arrays
//------------------------------------------------------------------------------------------
	inline int *arrayAnd(int array1[32], int array2[32]); 
//------------------------------------------------------------------------------------------
// End pattern_finder
//------------------------------------------------------------------------------------------
#endif

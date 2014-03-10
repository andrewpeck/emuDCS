#ifndef PATTERN_UNIT_H
#define PATTERN_UNIT_H
//------------------------------------------------------------------------------
//	CLCT Pattern Unit Emulator
//
//	Finds:	  Best matching pattern template and number of layers hit on that pattern for 1 key 1/2-strip
//	Returns:  Best matching pattern template ID, and number of hits on the pattern
//
//	03/30/10 Initial translation from verilog
//	09/07/10 Mod for vs2008
//------------------------------------------------------------------------------
// Headers
//------------------------------------------------------------------------------
	#include <stdio.h>
	#include <iostream>
	using namespace std;
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
//	Prototypes
//------------------------------------------------------------------------------
	const int	A=10;
//------------------------------------------------------------------------------
	void pattern_unit 
	(
// Inputs
	int ly0[], 
	int ly1[], 
	int ly2[], 
	int ly3[], 
	int ly4[],
	int ly5[],
// Outputs
	int	&pat_nhits,
	int	&pat_id
	);
//------------------------------------------------------------------------------------------------------------------------
// Function to sum number of layers hit
// Returns 	count1s = (inp[5]+inp[4]+inp[3])+(inp[2]+inp[1]+inp[0]);
//------------------------------------------------------------------------------------------------------------------------
	int count1s(int pat[6]);
//------------------------------------------------------------------------------------------
// End pattern_unit
//------------------------------------------------------------------------------------------
#endif

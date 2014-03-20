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
    #include "emu/pc/pattern_unit.h"
    #include "emu/pc/trigger_test.h"
	using namespace std;


//------------------------------------------------------------------------------
//	Prototypes
//------------------------------------------------------------------------------
	int			count1s		(int pat[]);

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
	)
{
    A=10;
        
//------------------------------------------------------------------------------
// Local
//------------------------------------------------------------------------------

    const int	MXLY	= 6;			// Number of CSC layers
    const int	MXPID	= 0xA+1;		// Number of patterns
    int			pat[MXPID][MXLY];
    int			nhits_array[MXPID];

    int			nhits_s3;
    int			pid_s3;
    int			pid;


	#ifdef debug
	int i;
	printf(stdout,"\n");
	printf(stdout,"dbg: pattern_unit:\n");
	printf(stdout,"dbg: hs ="); for (i=0; i<=0xA; ++i) printf(stdout,"%1X",i     ); printf(stdout,"\n");
	printf(stdout,"dbg: ly0="); for (i=0; i<=0xA; ++i) printf(stdout,"%1i",ly0[i]); printf(stdout,"\n");
	printf(stdout,"dbg: ly1="); for (i=0; i<=0xA; ++i) printf(stdout,"%1i",ly1[i]); printf(stdout,"\n");
	printf(stdout,"dbg: ly2="); for (i=0; i<=0xA; ++i) printf(stdout,"%1i",ly2[i]); printf(stdout,"\n");
	printf(stdout,"dbg: ly3="); for (i=0; i<=0xA; ++i) printf(stdout,"%1i",ly3[i]); printf(stdout,"\n");
	printf(stdout,"dbg: ly4="); for (i=0; i<=0xA; ++i) printf(stdout,"%1i",ly4[i]); printf(stdout,"\n");
	printf(stdout,"dbg: ly5="); for (i=0; i<=0xA; ++i) printf(stdout,"%1i",ly5[i]); printf(stdout,"\n");
	#endif

//------------------------------------------------------------------------------------------------------------------------
// Finds best 1-of-9 1/2-strip patterns for 1 key 1/2-strip 
// Returns pattern number 2-10 and number of layers hit on that pattern 0-6.
// Pattern LSB = bend direction
// Hit pattern LUTs for 1 layer: - = don't care, xx= one hit or the other or both
//
// Pattern Templates:
//
// Pattern       id=2        id=3        id=4        id=5        id=6        id=7        id=8        id=9        idA
// Bend dir      bd=0        bd=1        bd=0        bd=1        bd=0        bd=1        bd=0        bd=1        bd=0
//               |           |           |           |           |           |           |           |           |
// ly0      --------xxx xxx-------- -------xxx- -xxx------- ------xxx-- --xxx------ -----xxx--- ---xxx----- ----xxx----
// ly1      ------xx--- ---xx------ ------xx--- ---xx------ -----xx---- ----xx----- -----xx---- ----xx----- -----x-----
// ly2 key  -----x----- -----x----- -----x----- -----x----- -----x----- -----x----- -----x----- -----x----- -----x-----
// ly3      ---xxx----- -----xxx--- ---xx------ ------xx--- ----xx----- -----xx---- ----xx----- -----xx---- -----x-----
// ly4      -xxx------- -------xxx- -xxx------- -------xxx- ---xx------ ------xx--- ---xxx----- -----xxx--- ----xxx----
// ly5      xxx-------- --------xxx -xxx------- -------xxx- --xxx------ ------xxx-- ---xxx----- -----xxx--- ----xxx----
//               |           |           |           |           |           |           |           |           |
// Extent   0123456789A 0123456789A 0123456789A 0123456789A 0123456789A 0123456789A 0123456789A 0123456789A 0123456789A
// Avg.bend - 8.0 hs    + 8.0 hs    -6.0 hs     +6.0 hs     -4.0 hs     +4.0 hs     -2.0 hs     +2.0 hs      0.0 hs
// Min.bend -10.0 hs    + 6.0 hs    -8.0 hs     +4.0 hs     -6.0 hs     +2.0 hs     -4.0 hs      0.0 hs     -1.0 hs
// Max.bend - 6.0 hs    +10.0 hs    -4.0 hs     +8.0 hs     -2.0 hs     +6.0 hs      0.0 hs     +4.0 hs     +1.0 hs
//------------------------------------------------------------------------------------------------------------------------
// Pattern A								       0123456789A
	pat[A][0] = ly0[4]|ly0[5]|ly0[6];		// ly0 ----xxx----
	pat[A][1] =        ly1[5];				// ly1 -----x-----
	pat[A][2] =        ly2[5];				// ly2 -----x-----
	pat[A][3] =        ly3[5];				// ly3 -----x-----
	pat[A][4] = ly4[4]|ly4[5]|ly4[6];		// ly4 ----xxx---
	pat[A][5] = ly5[4]|ly5[5]|ly5[6];		// ly5 ----xxx---

// Pattern 9										       0123456789A
	pat[9][0] = ly0[3]|ly0[4]|ly0[5];				// ly0 ---xxx-----
	pat[9][1] =        ly1[4]|ly1[5];				// ly1 ----xx-----
	pat[9][2] =               ly2[5];				// ly2 -----x-----
	pat[9][3] =               ly3[5]|ly3[6];		// ly3 -----xx----
	pat[9][4] =               ly4[5]|ly4[6]|ly4[7];	// ly4 -----xxx---
	pat[9][5] =               ly5[5]|ly5[6]|ly5[7];	// ly5 -----xxx---
	
// Pattern 8										       0123456789A
	pat[8][0] =               ly0[5]|ly0[6]|ly0[7];	// ly0 -----xxx---
	pat[8][1] =               ly1[5]|ly1[6];		// ly1 -----xx----
	pat[8][2] =               ly2[5];				// ly2 -----x-----
	pat[8][3] =        ly3[4]|ly3[5];				// ly3 ----xx-----
	pat[8][4] = ly4[3]|ly4[4]|ly4[5];				// ly4 ---xxx-----
	pat[8][5] = ly5[3]|ly5[4]|ly5[5];				// ly5 ---xxx-----

// Pattern 7														       0123456789A
	pat[7][0] = ly0[2]|ly0[3]|ly0[4];								// ly0 --xxx------
	pat[7][1] =               ly1[4]|ly1[5];						// ly1 ----xx-----
	pat[7][2] =                      ly2[5];						// ly2 -----x-----
	pat[7][3] =                      ly3[5]|ly3[6];					// ly3 -----xx----
	pat[7][4] =                             ly4[6]|ly4[7];			// ly4 ------xx---
	pat[7][5] =                             ly5[6]|ly5[7]|ly5[8];	// ly5 ------xxx--

// Pattern 6														       0123456789A
	pat[6][0] =                             ly0[6]|ly0[7]|ly0[8];	// ly0 ------xxx--
	pat[6][1] =                      ly1[5]|ly1[6];					// ly1 -----xx----
	pat[6][2] =                      ly2[5];						// ly2 -----x-----
	pat[6][3] =               ly3[4]|ly3[5];						// ly3 ----xx-----
	pat[6][4] =        ly4[3]|ly4[4];								// ly4 ---xx------
	pat[6][5] = ly5[2]|ly5[3]|ly5[4];								// ly5 --xxx------

// Pattern 5																	       0123456789A
	pat[5][0] = ly0[1]|ly0[2]|ly0[3];											// ly0 -xxx-------
	pat[5][1] =               ly1[3]|ly1[4];									// ly1 ---xx------
	pat[5][2] =                             ly2[5];								// ly2 -----x-----
	pat[5][3] =                                    ly3[6]|ly3[7];				// ly3 ------xx---
	pat[5][4] =                                           ly4[7]|ly4[8]|ly4[9];	// ly4 -------xxx-
	pat[5][5] =                                           ly5[7]|ly5[8]|ly5[9];	// ly5 -------xxx-
	
// Pattern 4																	       0123456789A
	pat[4][0] =                                            ly0[7]|ly0[8]|ly0[9];// ly0 -------xxx-
	pat[4][1] =                                     ly1[6]|ly1[7];				// ly1 ------xx---
	pat[4][2] =                              ly2[5];							// ly2 -----x-----
	pat[4][3] =               ly3[3]|ly3[4];									// ly3 ---xx------
	pat[4][4] = ly4[1]|ly4[2]|ly4[3];											// ly4 -xxx-------
	pat[4][5] = ly5[1]|ly5[2]|ly5[3];											// ly5 -xxx-------

// Pattern 3																					       0123456789A
	pat[3][0] = ly0[0]|ly0[1]|ly0[2];															// ly0 xxx--------
	pat[3][1] =                      ly1[3]|ly1[4];												// ly1 ---xx------
	pat[3][2] =                                    ly2[5];										// ly2 -----x-----
	pat[3][3] =                                    ly3[5]|ly3[6]|ly3[7];						// ly3 -----xxx---
	pat[3][4] =                                                  ly4[7]|ly4[8]|ly4[9];			// ly4 -------xxx-
	pat[3][5] =                                                         ly5[8]|ly5[9]|ly5[A];	// ly5 --------xxx

// Pattern 2																					       0123456789A
	pat[2][0] =                                                         ly0[8]|ly0[9]|ly0[A];	// ly0 --------xxx
	pat[2][1] =                                           ly1[6]|ly1[7];						// ly1 ------xx---
	pat[2][2] =                                    ly2[5];										// ly2 -----x-----
	pat[2][3] =                      ly3[3]|ly3[4]|ly3[5];										// ly3 ---xxx-----
	pat[2][4] =        ly4[1]|ly4[2]|ly4[3];													// ly4 -xxx-------
	pat[2][5] = ly5[0]|ly5[1]|ly5[2];															// ly5 xxx--------

// Count number of layers hit for each pattern
	for (pid=0x2; pid<=0xA; ++pid) {
	nhits_array[pid] = count1s(pat[pid]);
#ifdef debug
	printf(stdout,"dbg: pid=%1X nhits=%1i\n",pid,nhits_array[pid]);
#endif
	}

// Best 1 of 8 Priority Encoder, perfers higher pattern number if hits are equal
	nhits_s3 = 0;
	pid_s3   = 0;

	for (pid=0x2; pid<=0xA; ++ pid) {
	if (nhits_array[pid]>=nhits_s3)
	nhits_s3 = nhits_array[pid];
	pid_s3   = pid;
	}

// Add 2 to pid to shift to range 2-10, not needed in c++ version
	pat_nhits = nhits_s3;
	pat_id	  = pid_s3;

	return;
}
//------------------------------------------------------------------------------------------------------------------------
// Function to sum number of layers hit
// Returns 	count1s = (inp[5]+inp[4]+inp[3])+(inp[2]+inp[1]+inp[0]);
//------------------------------------------------------------------------------------------------------------------------
int count1s(int pat[6])
{
	int	ly;
	int	ones;

	ones=0;

	for (ly=0; ly<=5; ++ly) {
	if (pat[ly]==1) ones++;
	}

	return ones;
}
//------------------------------------------------------------------------------------------
// End pattern_unit
//------------------------------------------------------------------------------------------

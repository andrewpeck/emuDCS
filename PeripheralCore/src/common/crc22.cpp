#ifndef CRC22_H
#define CRC22_H

#include "emu/pc/crc22.h"
//------------------------------------------------------------------------------
//	CRC-22 with 16-bit parallel loading data and synchronous reset
//	Polynomial = x22+x1+1
//
// 12/03/02 Initial (verilog)
// 01/16/03 Reverse output bit order to suit OSU
// 03/19/03 Un-reverse output bits per OSU
// 07/16/04 Ported to fortran
// 10/02/08 Port to cpp
// 10/03/08 Array bounds fix

//------------------------------------------------------------------------------
// Entry crc22a()	
//------------------------------------------------------------------------------
	void crc22a (
	long int	&din,		// 16 bit integer
	long int	&crc,		// 22 bit integer
	int			reset)
//------------------------------------------------------------------------------
{
	const  int	data_width = 16;
	const  int	crc_width  = 22;

	static int	bcrc[crc_width];
	int			bnewcrc[crc_width];
	int			bdin[data_width];

// Decompose input data into an array of bits
	int i;
	for (i=0; i<=data_width-1;++i) {
	bdin[i]=(din >> i) & 0x1;
	}

// Clear crc on reset
	if(reset==1) {
	for (i=0; i<=crc_width-1;++i) {
	bcrc[i]=0;
	}}

// Calculate CRC
	else {
	nextCRC22_D16(bdin,bcrc,bnewcrc);
	}

// Convert CRC array of bits back to integer
	crc=0;

	for (i=0; i<=crc_width-1;++i) {
	crc=crc | ((bcrc[i] & 0x1) << i);
	}

	return;
}

//------------------------------------------------------------------------------
// File:  CRC22_D16.v
// Date:  Tue Dec  3 00:44:19 2002
//
// Purpose: Verilog module containing a synthesizable CRC function
//   * polynomial: (0 1 22)
//   * data width: 16
//   * convention: the first serial data bit is D[15]
// Info: jand@easics.be (Jan Decaluwe)
//	   http://www.easics.com
//
// 07/16/04 Ported to fortran
// 08/26/08 Portet to c/cpp
// 10/03/08 Move new crc array to caller
// 10/05/08 Add explicit array dimensions to prototype
//------------------------------------------------------------------------------
	void nextCRC22_D16(int din[16], int crc[22], int newcrc[22])
{
	newcrc[ 0] = din[ 0] ^ crc[ 6];
	newcrc[ 1] = din[ 1] ^ din[ 0] ^ crc[ 6] ^ crc[ 7];
	newcrc[ 2] = din[ 2] ^ din[ 1] ^ crc[ 7] ^ crc[ 8];
	newcrc[ 3] = din[ 3] ^ din[ 2] ^ crc[ 8] ^ crc[ 9];
	newcrc[ 4] = din[ 4] ^ din[ 3] ^ crc[ 9] ^ crc[10];
	newcrc[ 5] = din[ 5] ^ din[ 4] ^ crc[10] ^ crc[11];
	newcrc[ 6] = din[ 6] ^ din[ 5] ^ crc[11] ^ crc[12];
	newcrc[ 7] = din[ 7] ^ din[ 6] ^ crc[12] ^ crc[13];
	newcrc[ 8] = din[ 8] ^ din[ 7] ^ crc[13] ^ crc[14];
	newcrc[ 9] = din[ 9] ^ din[ 8] ^ crc[14] ^ crc[15];
	newcrc[10] = din[10] ^ din[ 9] ^ crc[15] ^ crc[16];
	newcrc[11] = din[11] ^ din[10] ^ crc[16] ^ crc[17];
	newcrc[12] = din[12] ^ din[11] ^ crc[17] ^ crc[18];
	newcrc[13] = din[13] ^ din[12] ^ crc[18] ^ crc[19];
	newcrc[14] = din[14] ^ din[13] ^ crc[19] ^ crc[20];
	newcrc[15] = din[15] ^ din[14] ^ crc[20] ^ crc[21];
	newcrc[16] = din[15] ^ crc[ 0] ^ crc[21];
	newcrc[17] = crc[ 1];
	newcrc[18] = crc[ 2];
	newcrc[19] = crc[ 3];
	newcrc[20] = crc[ 4];
	newcrc[21] = crc[ 5];

	for (int i=0; i<=21;++i) {
	crc[i]=newcrc[i];
	}

	return;
}
//------------------------------------------------------------------------------
// The bitter end..
//------------------------------------------------------------------------------
#endif

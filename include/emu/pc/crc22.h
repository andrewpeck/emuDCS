//------------------------------------------------------------------------------
//	CRC-22 with 16-bit parallel loading data and synchronous reset
//	Polynomial = x22+x1+1
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Entry crc22a()	
//------------------------------------------------------------------------------
	void crc22a (
	long int	&din,		// 16 bit integer
	long int	&crc,		// 22 bit integer
	int			reset);
//------------------------------------------------------------------------------

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
	void nextCRC22_D16(int din[16], int crc[22], int newcrc[22]);
//------------------------------------------------------------------------------

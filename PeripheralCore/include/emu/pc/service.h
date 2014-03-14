#ifndef SERVICE_H
#define SERVICE_H
//------------------------------------------------------------------------------
// Service routines for main
//------------------------------------------------------------------------------
#include <cstdlib>
#include <cstring> 
#include <stdio.h>
#include <iostream>
#include <cmath>
using namespace std; 

//------------------------------------------------------------------------------
//   Flip pattern ID numbers, because Im too lazy to flip the hs image
//------------------------------------------------------------------------------
int flip_pid(int pid); 
//------------------------------------------------------------------------------
//   Check data read vs data expected
//------------------------------------------------------------------------------
void ck(string msg_string, int data_read, int data_expect); 
//------------------------------------------------------------------------------
//   Check data read vs data expected, with status return
//------------------------------------------------------------------------------
int cks(string msg_string, int data_read, int data_expect); 
//------------------------------------------------------------------------------
//   Check data read vs data expected, floating point version  with tolerance
//------------------------------------------------------------------------------
void tok(string msg_string, double fdata_read, double fdata_expect, double tolerance, int &status); 
//------------------------------------------------------------------------------
//   Inquire prompt for integer
//------------------------------------------------------------------------------
int inquire(string test, string val, const int &minv, const int &maxv, const int &radix, int &now); 
//------------------------------------------------------------------------------
//   Inquire prompt for two integers
//------------------------------------------------------------------------------
int inquir2(string test, string val, const int &minv, const int &maxv, const int &radix, int &num, int &now); 
//------------------------------------------------------------------------------
//   Inquire prompt for long integer
//------------------------------------------------------------------------------
int inquirl(string test, string val, const long int &minv, const long int &maxv, const int &radix, long int &now); 
//------------------------------------------------------------------------------
//   Inquire prompt for bool
//------------------------------------------------------------------------------
void inquirb(string test, string val, bool &now); 
//------------------------------------------------------------------------------
//   Inquire prompt for string
//------------------------------------------------------------------------------
void inquirs(string test, string val, string &now); 
//------------------------------------------------------------------------------
//   Convert integer bit string to an array
//------------------------------------------------------------------------------
void bit_to_array(const int &idata, int iarray[], const int &n); 
//------------------------------------------------------------------------------------------
// Logical OR returns the OR of all the array elements as a single integer
//------------------------------------------------------------------------------------------
int arrayOr(int array[32]); 
//------------------------------------------------------------------------------------------
// Array AND returns an array of the AND of two arrays
//------------------------------------------------------------------------------------------
int *arrayAnd(int array1[32], int array2[32]); 
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
//------------------------------------------------------------------------------
// The bitter end
//------------------------------------------------------------------------------

#endif

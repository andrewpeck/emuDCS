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
void inquire(string prompt, const int &minv, const int &maxv, const int &radix, int &now); 
//------------------------------------------------------------------------------
//   Inquire prompt for two integers
//------------------------------------------------------------------------------
void inquir2(string prompt, const int &min, const int &max, const int &radix, int &num, int &now); 
//------------------------------------------------------------------------------
//   Inquire prompt for long integer
//------------------------------------------------------------------------------
void inquirl(string prompt, const int &min, const int &max, const int &radix, long int &now); 
//------------------------------------------------------------------------------
//   Inquire prompt for bool
//------------------------------------------------------------------------------
void inquirb(string prompt, bool &now); 
//------------------------------------------------------------------------------
//   Pass Fail prompt
//------------------------------------------------------------------------------
bool pass_fail(string prompt); 
//------------------------------------------------------------------------------
//   Display Test OK
//------------------------------------------------------------------------------
void aok(string msg_string); 
//------------------------------------------------------------------------------
//   Display Test OK or FAIL
//------------------------------------------------------------------------------
void aokf(string msg_string, const int itest, const int status); 
//------------------------------------------------------------------------------
//   Convert integer bit string to an array
//------------------------------------------------------------------------------
void bit_to_array(const int &idata, int iarray[], const int &n); 
//--------------------------------------------------------------------------------
// Wait for specified number of milliseconds, probably MS Visual Studio specific
//--------------------------------------------------------------------------------
//void sleep(clock_t msec); 
//------------------------------------------------------------------------------
// The bitter end
//------------------------------------------------------------------------------
#endif

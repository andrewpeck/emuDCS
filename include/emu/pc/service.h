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
#ifndef STOP_H
#define STOP_H
//------------------------------------------------------------------------------
// Pause emulator cuz C sux
//------------------------------------------------------------------------------
using namespace std; 
void stop (string s); 
//------------------------------------------------------------------------------
#endif
#ifndef PAUSE_H
#define PAUSE_H
//------------------------------------------------------------------------------
// Pause emulator cuz C sux
//------------------------------------------------------------------------------
void pause (std::string s);
//------------------------------------------------------------------------------
#endif

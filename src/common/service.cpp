//------------------------------------------------------------------------------
// Service routines for main
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Headers
//------------------------------------------------------------------------------

#include <cstdlib>
#include <cstring> 
#include <stdio.h>
#include <iostream>
#include <cmath>

//------------------------------------------------------------------------------
// Local Headers
//------------------------------------------------------------------------------
#include "emu/pc/common.h"
#include "emu/pc/service.h"
#include "emu/pc/minIni.h"

//------------------------------------------------------------------------------

using namespace std; 

//------------------------------------------------------------------------------
//   Flip pattern ID numbers, because Im too lazy to flip the hs image
//------------------------------------------------------------------------------
int flip_pid(int pid)
{    
    return (pid==0xA) ? 0xA : pid^0x1;   // invert pid lsb unless its pattern A
}

//------------------------------------------------------------------------------
//   Check data read vs data expected
//------------------------------------------------------------------------------
void ck(string msg_string, int data_read, int data_expect)
{    
    if (data_read != data_expect) {
        fprintf(stdout,  "ERRm: in %s: read=%8.8X expect=%8.8X\n",msg_string.c_str(),data_read,data_expect);
    }
    //return;
}
//------------------------------------------------------------------------------
//   Check data read vs data expected, with status return
//------------------------------------------------------------------------------
int cks(string msg_string, int data_read, int data_expect)
{
    int status;

    status = 0;  // good return
    if (data_read != data_expect) {
        status = 1;  // bad return
        fprintf(stdout,  "\tERRm: in %s: read=%8.8X expect=%8.8X\n",msg_string.c_str(),data_read,data_expect);
    }

    return status;
}
//------------------------------------------------------------------------------
//   Check data read vs data expected, floating point version  with tolerance
//------------------------------------------------------------------------------
void tok(string msg_string, double fdata_read, double fdata_expect, double tolerance, int &status)
{
    double err;
    double errpct;

    err    = (fdata_read-fdata_expect)/max(fdata_expect,.01);
    errpct = err*100.;

    status=0;
    if (abs(err)>tolerance) {
        status=1;
        fprintf(stdout,  "\tERRm: in %s: read=%10.4g expect=%10.4g %10.2f\n",msg_string.c_str(),fdata_read,fdata_expect,errpct);
    }

    return;
}
//------------------------------------------------------------------------------
//   Inquire prompt for integer
//------------------------------------------------------------------------------
void inquire(string test, string var, const int &minv, const int &maxv, const int &radix, int &now) {
	int i; 
	i=ini_getl(test,var,-1,inifile); 
	if ( (i<minv) || (i>maxv) ) {
		std::cout << "ERROR: Config value " << test << " :: " << var << " out of range." << std::endl; 
		return EXIT_FAILURE;
	}
	else {
		now = i; 
		return EXIT_SUCCESS; 
	}
}
//------------------------------------------------------------------------------
//   Inquire prompt for two integers
//------------------------------------------------------------------------------
void inquir2(string test, string var1, string var2, const int &min, const int &max, const int &radix, int &num, int &now)
{
	int i; 
	int j; 


	i=ini_getl(test,var1,-1,inifile); 
	j=ini_getl(test,var2,-1,inifile); 
	if ( (i<minv) || (i>maxv) ) {
		std::cout << "ERROR: Config value " << test << " :: " << var1 << " out of range." << std::endl; 
		return EXIT_FAILURE;
	}
	if ( (j<minv) || (j>maxv) ) {
		std::cout << "ERROR: Config value " << test << " :: " << var2 << " out of range." << std::endl; 
		return EXIT_FAILURE;
	}

	num=i; 
	now=j; 
	return EXIT_SUCCESS; 
}
//------------------------------------------------------------------------------
//   Inquire prompt for long integer
//------------------------------------------------------------------------------
void inquirl(string test, string var, const int &min, const int &max, const int &radix, long int &now)
{
	long int i; 
	i=ini_getl(test,var,-1,inifile); 
	if ( (i<minv) || (i>maxv) ) {
		std::cout << "ERROR: Config value " << test << " :: " << var << " out of range." << std::endl; 
		return EXIT_FAILURE;
	}
	else {
		now = i; 
		return EXIT_SUCCESS; 
	}
}

//------------------------------------------------------------------------------
//   Inquire prompt for bool
//------------------------------------------------------------------------------
void inquirb(string test, string val, bool &now) {
	bool i; 
	i = ini_getbool(test,var,0,inifile); 
	now = i; 
}

//------------------------------------------------------------------------------
//   Convert integer bit string to an array
//------------------------------------------------------------------------------
void bit_to_array(const int &idata, int iarray[], const int &n) {
    int i;

    for (i=0; i<n; ++i) {
        iarray[i]=(idata >> i) & 0x00000001;
    }
    return;
}
//--------------------------------------------------------------------------------
// Wait for specified number of milliseconds, probably MS Visual Studio specific
//--------------------------------------------------------------------------------
//void sleep(clock_t msec)
//{
//    clock_t goal;
//    goal = msec + clock();
//    while (goal > clock());
//}

//------------------------------------------------------------------------------
// The bitter end
//------------------------------------------------------------------------------

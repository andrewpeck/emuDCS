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
#include <wordexp.h>

//------------------------------------------------------------------------------
// Local Headers
//------------------------------------------------------------------------------
#include "emu/pc/trigger_test.h"
#include "emu/pc/service.h"
#include "emu/pc/minIni.h"

//------------------------------------------------------------------------------
// Create minIni object
//------------------------------------------------------------------------------

//set config file location
const char* config_file = "~/config.ini";

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
void ck(std::string msg_string, int data_read, int data_expect)
{    
    if (data_read != data_expect) {
        fprintf(stdout,  "ERRm: in %s: read=%8.8X expect=%8.8X\n",msg_string.c_str(),data_read,data_expect);
    }
    //return;
}

//------------------------------------------------------------------------------
//   Check data read vs data expected, with status return
//------------------------------------------------------------------------------
int cks(std::string msg_string, int data_read, int data_expect)
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
void tok(std::string msg_string, double fdata_read, double fdata_expect, double tolerance, int &status)
{
    double err;
    double errpct;

    err    = (fdata_read-fdata_expect)/std::max(fdata_expect,.01);
    errpct = err*100.;

    status=0;
    if (std::abs(err)>tolerance) {
        status=1;
        fprintf(stdout,  "\tERRm: in %s: read=%10.4g expect=%10.4g %10.2f\n",msg_string.c_str(),fdata_read,fdata_expect,errpct);
    }

    return;
}
//------------------------------------------------------------------------------
//   Inquire for integer from Config File
//------------------------------------------------------------------------------
int inquire(std::string test, std::string var, const int &minv, const int &maxv, const int &radix, int &now) {

    //bash-like expansion of config file name
    wordexp_t p;
    wordexp(config_file, &p, 0);
    //create minIni config file object
    minIni ini(p.we_wordv[0]);

    int i; 
    i=ini.getl(test,var,-999); 
    if ( (i<minv) || (i>maxv) ) {
        std::cout << "ERROR: Config value " << var << " = " << i << " :: out of range." << std::endl; 
        return EXIT_FAILURE;
    }
    else {
        now = i; 
        return EXIT_SUCCESS; 
    }
}
//------------------------------------------------------------------------------
//   Inquire for two integers from Config File
//------------------------------------------------------------------------------
int inquir2(std::string test, std::string var1, std::string var2, const int &minv, 
        const int &maxv, const int &radix, int &num, int &now)
{

//bash-like expansion of config file name
wordexp_t p;
wordexp(config_file, &p, 0);
//create minIni config file object
minIni ini(p.we_wordv[0]);

    int i; 
    int j; 


    i=ini.getl(test,var1,-999); 
    j=ini.getl(test,var2,-999); 
    if ( (i<minv) || (i>maxv) ) {
        std::cout << "ERROR: Config value " << var1 << " = " << i << " :: out of range." << std::endl; 
        return EXIT_FAILURE;
    }
    if ( (j<minv) || (j>maxv) ) {
        std::cout << "ERROR: Config value " << var2 << " = " << j << " :: out of range." << std::endl; 
        return EXIT_FAILURE;
    }

    num=i; 
    now=j; 
    return EXIT_SUCCESS; 
}

//------------------------------------------------------------------------------
//   Inquire for long integer from Config file
//------------------------------------------------------------------------------
int inquirl(std::string test, std::string var, const long int &minv, const long int &maxv, const int &radix, long int &now)
{

//bash-like expansion of config file name
wordexp_t p;
wordexp(config_file, &p, 0);
//create minIni config file object
minIni ini(p.we_wordv[0]);

    long int i; 
    i=ini.getl(test,var,-1); 
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
//   Inquire for bool from Config file
//------------------------------------------------------------------------------
void inquirb(std::string test, std::string var, bool &now) {

//bash-like expansion of config file name
wordexp_t p;
wordexp(config_file, &p, 0);
//create minIni config file object
minIni ini(p.we_wordv[0]);

    bool b; 
    b = ini.getbool(test,var,0); 
    now = b; 
}

//------------------------------------------------------------------------------
//   Inquire for string from Config file
//------------------------------------------------------------------------------
void inquirs(std::string test, std::string var, std::string &now) {

//bash-like expansion of config file name
wordexp_t p;
wordexp(config_file, &p, 0);
//create minIni config file object
minIni ini(p.we_wordv[0]);

    std::string s; 
    s = ini.gets(test,var,0); 
    now = s; 
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

//------------------------------------------------------------------------------------------
// Logical OR returns the OR of all the array elements as a single integer
//------------------------------------------------------------------------------------------
int arrayOr(int array[32])
{
    int ior = 0;
    int	i;

    for (i=0; i<32; ++i) {
        ior = ior | array[i];
    }

    return ior;

}

//------------------------------------------------------------------------------------------
// Array AND returns an array of the AND of two arrays
//------------------------------------------------------------------------------------------
int *arrayAnd(int array1[32], int array2[32])
{
    int *iand = new int[32];
    int	 i;

    for (i=0; i<32; ++i) {
        iand[i] = array1[i] & array2[i];
    }
    return iand;
}

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
//------------------------------------------------------------------------------
// The bitter end
//------------------------------------------------------------------------------

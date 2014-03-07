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
void inquire(string prompt, const int &minv, const int &maxv, const int &radix, int &now)
{
    char line[80];
    int      i;
    int      n;

ask:
    printf(prompt.c_str(),now);
    fgets(line, 80, stdin);
    n=strlen(line);
    if (radix==16)   sscanf(line,"%x",&i);
    else         sscanf(line,"%i",&i);

    if ((n!=0) && ((i<minv) || (i>maxv))) {
        if (radix==16) printf("Out of range. Expect %X to %X\n",minv,maxv);
        else           printf("Out of range. Expect %i to %i\n",minv,maxv);
        goto ask;
    }
    if (n!=0) now = i;
    return;
}
//------------------------------------------------------------------------------
//   Inquire prompt for two integers
//------------------------------------------------------------------------------
void inquir2(string prompt, const int &min, const int &max, const int &radix, int &num, int &now)
{
    char line[80];
    int      i;
    int      n;

ask:
    printf(prompt.c_str(),num,now);
    fgets(line, 80, stdin);
    n=strlen(line);
    if (radix==16)   sscanf(line,"%x",&i);
    else         sscanf(line,"%i",&i);

    if ((n!=0) && ((i<min) || (i>max))) {
        if (radix==16) printf("Out of range. Expect %X to %X\n",min,max);
        else           printf("Out of range. Expect %i to %i\n",min,max);
        goto ask;
    }
    if (n!=0) now = i;
    return;
}
//------------------------------------------------------------------------------
//   Inquire prompt for long integer
//------------------------------------------------------------------------------
void inquirl(string prompt, const int &min, const int &max, const int &radix, long int &now)
{
    char     line[80];
    long int i;
    int          n;

ask:
    printf(prompt.c_str(),now);
    fgets(line, 80, stdin);
    n=strlen(line);
    if (radix==16)   sscanf(line,"%ld",&i);
    else         sscanf(line,"%ld",&i);

    if ((n!=0) && ((i<min) || (i>max))) {
        if (radix==16) printf("Out of range. Expect %X to %X\n",min,max);
        else           printf("Out of range. Expect %i to %i\n",min,max);
        goto ask;
    }
    if (n!=0) now = i;
    return;
}
//------------------------------------------------------------------------------
//   Inquire prompt for bool
//------------------------------------------------------------------------------
void inquirb(string prompt, bool &now)
{
    char line[80];
    char i;
    int      n;

ask:
    printf(prompt.c_str(),yesno(now&0x1));
    fgets(line, 80, stdin);
    n=strlen(line);
    sscanf(line,"%c",&i);

    if (n!=0)
    {
        if (i!='y' && i!='Y' && i!='n' && i!='N') {printf("\tEnter y or n\n"); goto ask;} 
        if (i=='y' || i=='Y') now = true;
        if (i=='n' || i=='N') now = false;
    }
    return;
}
//------------------------------------------------------------------------------
//   Pass Fail prompt
//------------------------------------------------------------------------------
bool pass_fail(string prompt)
{
    char line[80];
    int      i;
    int      n;
    bool ans;

ask:
    printf(prompt.c_str());
    fgets(line, 80, stdin);
    n = strlen(line);
    i = line[0];

    if (n!=0 && (i=='p' || i=='P')) ans = true; 
    else
        if (n!=0 && (i=='f' || i=='F')) ans = false;
        else {
            printf("Enter P or F\n");
            goto ask;
        }
    return ans;
}
//------------------------------------------------------------------------------
//   Display Test OK
//------------------------------------------------------------------------------
void aok(string msg_string)
{    
    int      tab           = 45;
    int      msg_spaces    = msg_string.length();
    int      insert_spaces = tab-msg_spaces;
    string   spaces        = " ";

    for (int i=0; i<=insert_spaces; ++i) spaces.append(string(" "));

    fprintf(stdout,  "\t%s %s OK\n",msg_string.c_str(),spaces.c_str());  // screen
    return;
}
//------------------------------------------------------------------------------
//   Display Test OK or FAIL
//------------------------------------------------------------------------------
void aokf(string msg_string, const int itest, const int status)  
{
    string   sstat[2]={"FAIL","PASS"};

    int      tab           = 45;
    int      msg_spaces    = msg_string.length();
    int      insert_spaces = tab-msg_spaces;
    string   spaces        = " ";

    for (int i=0; i<=insert_spaces; ++i) spaces.append(string(" "));

    fprintf(stdout,  "\t%3i %s %s %s\n",itest,msg_string.c_str(),spaces.c_str(),sstat[status].c_str());  // screen

    return;
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

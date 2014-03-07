//------------------------------------------------------------------------------
// Pause emulator cuz C sux
//------------------------------------------------------------------------------
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "emu/pc/stop.h"
using namespace std;

string s; 
void stop (string s)
{
    fprintf(stdout,"%s %s","\tSTOP:",s.c_str());
    //char dummy=getchar();
    exit(EXIT_FAILURE);
}
//------------------------------------------------------------------------------

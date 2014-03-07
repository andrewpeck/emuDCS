//------------------------------------------------------------------------------
// Pause emulator cuz C sux
//------------------------------------------------------------------------------
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include "emu/pc/pause.h"

using namespace std;

void pause (string s)
{
    fprintf(stdout,"%s %s","\tPause:",s.c_str());
    char key=getchar();
    if (key=='e' || key=='E') 
        exit(EXIT_FAILURE);
}
//------------------------------------------------------------------------------

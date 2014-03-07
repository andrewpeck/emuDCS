#ifndef DECODE_READOUT_H
#define DECODE_READOUT_H

//------------------------------------------------------------------------------
// Decode TMB2005 raw hits dump
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Headers
//------------------------------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <cstring>

//------------------------------------------------------------------------------
// Local Headers
//------------------------------------------------------------------------------
#include "pause.h"
#include "scope160c.h"
#include "common.h"
#include "service.h"
#include "crc22.h"
#include "miniscope16.h"

void decode_readout(int	vf_data[mxframe],int &dmb_wdcnt, bool &err_check);
void ck (char *data_string, int &data_read, int &data_expect);

#endif

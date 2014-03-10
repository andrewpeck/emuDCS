//------------------------------------------------------------------------------
//  Headers
//------------------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS 1
#include <cstdio>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <cstring>                      
#include <stdio.h>                     
#include <cstdlib>                     
#include <cmath>
#include <cstdlib>

//------------------------------------------------------------------------------
//  Local Includes
//------------------------------------------------------------------------------

#include "emu/pc/decode_readout.h"
#include "emu/pc/scope160c.h"
#include "emu/pc/stop.h"
#include "emu/pc/pause.h"
#include "emu/pc/vme_emulib.h"
#include "emu/pc/crc22.h"
#include "emu/pc/service.h"
#include "emu/pc/pattern_finder.h"
#include "emu/pc/lct_quality.h"
#include "emu/pc/trigger_test.h"
#include "emu/pc/common.h"


#ifdef INTERACTIVE
#include INTERACTIVE_CONFIG_H
bool interactive=true; 
#endif 

bool interactive=true; 

//------------------------------------------------------------------------------
//  Namespace
//------------------------------------------------------------------------------
using namespace std;

//------------------------------------------------------------------------------
//  Trigger Test Menu
//------------------------------------------------------------------------------
int main() {
    //assign defaults 
    rat_injector_sync=false;
    rat_injector_enable=true;
    nclcts_inject=1;
    nalcts_inject=1;
    for (int i=0; i<6; i++) 
        triad_1st_tbin[i]=0;
    l1a_delay=119;  // hits 0th l1a window bx
    for (int i=0; i<32; i++) { 
        for (int j=0; i<3; i++)  { 
            for (int k=0; i<5; i++)  {
                pat_ram[i][j][k]=0; }}}

    marker="AOXOMOXOA";
    sfmm_state[0]="Startup";
    sfmm_state[1]="Resync ";
    sfmm_state[2]="Stop   ";
    sfmm_state[3]="WaitBXO";
    sfmm_state[4]="Run    ";
    fire_injector=false;
    err_check=true;
    for (int i=0; i<6; i++) 
        lookback_triad_hits[i]=0;
    for (int i=0; i<5; i++) 
        cfeb_en[i]=1;

    //Log file
    FILE     *log_file;
    string  log_file_name="vmetst_log.txt";
    log_file = fopen(log_file_name.c_str(),"w");

    //Get VME slot number, used for csc id
    //adr    = base_adr+vme_idreg_adr;
    //status = vme_read(adr,rd_data);
    //id_slot= (rd_data >> 8) & 0x00FF;

    // accept argv command
    //if (argc) {
    //    ifunc=argv[1];
    //    i=abs(ifunc);
    //}
    
    // interactive mode
    if (interactive) {
        printf("\t   TMB Trigger Test Menu"); 
        printf("\t 1:  Inject ALCT\n");
        printf("\t 2:  Inject CLCT\n");
        printf("\t 3:  Inject ALCT+CLCT\n");
        printf("\t 4:  Inject ALCT+CLCT + readout\n");
        printf("\t 5:  Fire ext_trig + L1A to ALCT + readout ALCT RAM\n");
        printf("\t 6:  Fire CLCT ext trig linked with ALCT ext trig\n");
        printf("\t 7:  Fire CLCT pattern injector, check CLCT0\n");
        printf("\t 8:  External Trigger ALCT+CLCT\n");
        printf("\t 9:  External Trigger ALCT\n");
        printf("\t10:  External Trigger CLCT\n");
        printf("\t11:  Ext Trig ALCT+CLCT with GTL pulser\n");
        printf("\t12:  Ext Trig ALCT+CLCT with GTL pulser check CRC\n");
        printf("\t13:  Check bxn reset and increment\n");
        printf("\t14:  Fire L1A-only event. check readout\n");
        printf("\t15:  Fire VME trigger + readout\n");
        printf("\t<cr> Exit\n");
        printf("       > ");

        fgets(line, 80, stdin);
        if (line[0]==NULL) return 0;
        sscanf(line,"%i",&ifunc);
        i=abs(ifunc);
    } // close interactive mode

        if (i== 1) InjectALCT();
        if (i== 2) InjectCLCT();
        if (i== 3) InjectALCTCLCT();
        if (i== 4) InjectALCTCLCT_Readout ();
        if (i== 5) Fire_L1A_to_ALCT ();
        if (i== 6) fire_CLCT_ext_trig_with_ALCT ();
        if (i== 7) Inject_walking_CLCT();
        if (i== 8) ExternalTriggerALCTCLCT ();
        if (i== 9) ExternalTriggerALCT ();
        if (i==10) ExternalTriggerCLCT ();
        if (i==11) ExternalTriggerALCTCLCTwithGTLpulser ();
        if (i==12) ExternalALCTCLCTwithGTLpulserCheckCRC ();
        if (i==13) TestBXNCounter();
        if (i==14) FireL1A();
        if (i==15) ForceCLCTtriggerandReadout();
}

//------------------------------------------------------------------------------
//  Inject ALCT
//------------------------------------------------------------------------------
void InjectALCT() {

    // Turn off CCB backplane inputs, turn on L1A emulator
    adr     = ccb_cfg_adr+base_adr;
    wr_data = 0x003D;
    status  = vme_write(adr,wr_data);

    // Enable sequencer trigger, set internal l1a delay
    adr     = ccb_trig_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFF00;
    wr_data = wr_data | 0x0004;
    //  wr_data = wr_data | (114<<8);
    status = vme_write(adr,wr_data);

    // Turn off ALCT cable inputs, disable synchronized alct+clct triggers
    adr     = alct_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x0000;
    wr_data = wr_data | 0x0001;
    wr_data = wr_data | (alct_injector_delay << 5); // post-rat firmware
    status  = vme_write(adr,wr_data);

    // Turn off CLCT cable inputs
    adr     = cfeb_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFE0;
    status  = vme_write(adr,wr_data);

    // Turn on CFEB enables to over-ride mask_all
    adr     = seq_trig_en_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x03FF;     // clear old cfeb_en and source
    wr_data = wr_data | 0x7C00;     // ceb_en_source=0,cfeb_en=1F
    status  = vme_write(adr,wr_data);

    // Turn off internal level 1 accept for sequencer
    adr     = seq_l1a_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x0FFF;
    status  = vme_write(adr,wr_data);

    // Select ALCT pattern trigger
    adr     = seq_trig_en_adr+base_adr;
    status  = vme_read(adr,rd_data);
    rd_data = rd_data & 0xFF00;
    wr_data = rd_data | 0x0002;
    status  = vme_write(adr,wr_data);

    // Set start_trigger state for FMM
    ttc_cmd = 6;            // start_trigger
    adr     = base_adr+ccb_cmd_adr;
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);
    wr_data = 0x0003 | (ttc_cmd << 8);
    status  = vme_write(adr,wr_data);
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);

    ttc_cmd = 1;            // bx0
    wr_data = 0x0003 | (ttc_cmd << 8);
    status  = vme_write(adr,wr_data);
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);

    // Arm scope trigger
    scp_arm        = true;
    scp_readout    = false;
    scp_raw_decode = false;
    scp_silent     = false;
    scp_playback   = false;
    if (rdscope)
        scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

    // Clear previous inject
    adr     = alct_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFFD;
    status  = vme_write(adr,wr_data);

    // Fire ALCT injector
    wr_data = wr_data | 0x0002; // Fire ALCT inject
    status  = vme_write(adr,wr_data);

    // Clear previous inject
    wr_data = rd_data & 0xFFFD;     
    status  = vme_write(adr,wr_data);

    // Check for blue flash
    adr    = base_adr+vme_led_adr;
    status = vme_read(adr,rd_data);
    blue_flash = rd_data & 0x1;

    if (ifunc>0) {
        if (blue_flash==0) printf("\tNO BLUE FLASH. Rats =:-(\n");
        if (blue_flash==1) printf("\tBlue flash. Cool\n");
    }

    // Check scintillator veto is set
    adr    = base_adr+seqmod_adr;
    status = vme_read(adr,rd_data);
    scint_veto = (rd_data>>13) & 0x1;
    if (scint_veto!=1) pause("scint veto failed to set");

    // Clear scintillator veto
    wr_data = rd_data | (1<<12);
    status  = vme_write(adr,wr_data);
    wr_data = wr_data ^ (1<<12);
    status  = vme_write(adr,wr_data);
    status  = vme_read(adr,rd_data);
    scint_veto = (rd_data>>12) & 0x1;
    if (scint_veto!=0) pause("scint veto failed to clear");

    // Read ALCT data wot triggered
    adr    = alct_alct0_adr+base_adr;
    status = vme_read(adr,rd_data);
    alct0_inj_rd = rd_data;

    adr    = alct_alct1_adr+base_adr;
    status = vme_read(adr,rd_data);
    alct1_inj_rd = rd_data;

    // Read injector generated ALCT data
    adr    = alct0_inj_adr+base_adr;
    status = vme_read(adr,rd_data);
    alct0_inj_wr = rd_data;

    adr    = alct1_inj_adr+base_adr;
    status = vme_read(adr,rd_data);
    alct1_inj_wr = rd_data;

    // Compare injected ALCT readback with generated ALCT
    ck("alct0_inj  ",alct0_inj_wr,alct0_inj_rd);
    ck("alct1_inj  ",alct1_inj_wr,alct1_inj_rd);

    // Decompose injected ALCT
    alct0_vpf   = (alct0_inj_rd >> 0) & 0x0001; // Valid pattern flag
    alct0_qual  = (alct0_inj_rd >> 1) & 0x0003; // Pattern quality
    alct0_amu   = (alct0_inj_rd >> 3) & 0x0001; // Accelerator muon
    alct0_key   = (alct0_inj_rd >> 4) & 0x007F; // Wire group ID number
    alct0_bxn   = (alct0_inj_rd >>11) & 0x0003; // Bunch crossing number

    alct1_vpf   = (alct1_inj_rd >> 0) & 0x0001; // Valid pattern flag
    alct1_qual  = (alct1_inj_rd >> 1) & 0x0003; // Pattern quality
    alct1_amu   = (alct1_inj_rd >> 3) & 0x0001; // Accelerator muon
    alct1_key   = (alct1_inj_rd >> 4) & 0x007F; // Wire group ID number
    alct1_bxn   = (alct1_inj_rd >>11) & 0x0003; // Bunch crossing number

    // Display ALCTs found
    if (ifunc>0) {
        printf("\tExpect:ALCT0:Key7 Q3 Bxn1 ALCT1:Key61 Q2 Bxn1\n");

        printf("\tALCT0: ");
        printf("vpf =%1i ", alct0_vpf );
        printf("qual=%1i ", alct0_qual);
        printf("amu =%1i ", alct0_amu );
        printf("key =%3i ", alct0_key );
        printf("bxn =%1i ", alct0_bxn );
        printf("\n");

        printf("\tALCT1: ");
        printf("vpf =%1i ", alct1_vpf );
        printf("qual=%1i ", alct1_qual);
        printf("amu =%1i ", alct1_amu );
        printf("key =%3i ", alct1_key );
        printf("bxn =%1i ", alct1_bxn );
        printf("\n");
    }

    // Read back embedded scope data
    scp_arm        = false;
    scp_readout    = true;
    scp_raw_decode = false;
    scp_silent     = true;
    scp_playback   = false;
    if (rdscope)
        scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);
} // End InjectALCT()

//------------------------------------------------------------------------------
//  Inject CLCT
//------------------------------------------------------------------------------
void InjectCLCT(){

    // Turn off CCB backplane inputs, turn on L1A emulator
    adr     = ccb_cfg_adr+base_adr;
    wr_data = 0x003D;
    status  = vme_write(adr,wr_data);

    // Enable sequencer trigger, turn off dmb trigger, set internal l1a delay
    adr     = ccb_trig_adr+base_adr;
    wr_data = 0x0004;
    wr_data = wr_data | (114 << 8);
    status  = vme_write(adr,wr_data);

    // Turn off ALCT cable inputs, disable synchronized alct+clct triggers
    adr     = alct_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x0000;
    wr_data = wr_data | 0x0001;
    wr_data = wr_data | (alct_injector_delay << 5);
    status  = vme_write(adr,wr_data);

    // Turn off CLCT cable inputs
    adr     = cfeb_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFE0;
    status  = vme_write(adr,wr_data);

    // Turn on CFEB enables to over-ride mask_all
    adr     = seq_trig_en_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x03FF; // clear old cfeb_en and source
    wr_data = wr_data | 0x7C00; // ceb_en_source=0,cfeb_en=1F
    status  = vme_write(adr,wr_data);

    // Turn off internal level 1 accept for sequencer
    adr     = seq_l1a_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x0FFF;
    status  = vme_write(adr,wr_data);

    // Select pattern trigger
    adr     = seq_trig_en_adr+base_adr;
    status  = vme_read(adr,rd_data);
    rd_data = rd_data & 0xFF00;
    wr_data = rd_data | 0x0001;
    status  = vme_write(adr,wr_data);

    // Clear previous ALCT inject
    adr     = alct_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFFD;
    status  = vme_write(adr,wr_data);

    // Set start_trigger state for FMM
    ttc_cmd = 6;            // start_trigger
    adr     = base_adr+ccb_cmd_adr;
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);
    wr_data = 0x0003 | (ttc_cmd << 8);
    status  = vme_write(adr,wr_data);
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);

    ttc_cmd = 1;            // bx0
    wr_data = 0x0003 | (ttc_cmd << 8);
    status  = vme_write(adr,wr_data);
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);

    // Arm scope trigger
    scp_arm        = true;
    scp_readout    = false;
    scp_raw_decode = false;
    scp_silent     = false;
    scp_playback   = false;
    if (rdscope)
        scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

    // Clear previous  CLCT inject
    adr     = cfeb_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x7FFF;
    status  = vme_write(adr,wr_data);

    // Fire CLCT Injector
    wr_data = wr_data | 0x8000;
    status  = vme_write(adr,wr_data);

    // Clear previous inject    
    wr_data = rd_data & 0x7FFF; 
    status  = vme_write(adr,wr_data);

    // Check scintillator veto is set
    adr    = base_adr+seqmod_adr;
    status = vme_read(adr,rd_data);
    scint_veto = (rd_data >> 13) & 0x0001;
    if (scint_veto!=1) pause ("scint veto failed to set");

    // Clear scintillator veto
    wr_data = rd_data | (1 << 12);
    status  = vme_write(adr,wr_data);
    wr_data = wr_data ^ (1 << 12);
    status  = vme_write(adr,wr_data);
    status  = vme_read (adr,rd_data);
    scint_veto = (rd_data >> 12) & 0x1;
    if (scint_veto!=0) pause ("scint veto failed to clear");

    // Read back embedded scope data
    scp_arm        = false;
    scp_readout    = true;
    scp_raw_decode = false;
    scp_silent     = true;
    scp_playback   = false;
    if (rdscope)
        scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);
} // close InjectCLCT()

//------------------------------------------------------------------------------
//  Inject ALCT+CLCT
//------------------------------------------------------------------------------
void InjectALCTCLCT() {

    // Turn off CCB backplane inputs, turn on L1A emulator
    adr     = ccb_cfg_adr+base_adr;
    wr_data = 0x003D;
    status  = vme_write(adr,wr_data);

    // Enable sequencer trigger, turn off dmb trigger, set internal l1a delay
    adr     = ccb_trig_adr+base_adr;
    wr_data = 0x0004;
    wr_data = wr_data | (114<<8);
    status  = vme_write(adr,wr_data);

    // Turn off ALCT cable inputs, enable synchronized alct+clct triggers
    adr     = alct_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x0000;
    wr_data = wr_data | 0x0005;
    wr_data = wr_data | (alct_injector_delay << 5);
    status  = vme_write(adr,wr_data);

    // Set ALCT delay for TMB matching
    adr     = tmbtim_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFF0;
    wr_data = wr_data | 0x0003;
    status  = vme_write(adr,wr_data);

    // Turn off CLCT cable inputs
    adr     = cfeb_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFE0;
    status  = vme_write(adr,wr_data);

    // Turn on CFEB enables to over-ride mask_all
    adr     = seq_trig_en_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x03FF;     // clear old cfeb_en and source
    wr_data = wr_data | 0x7C00;     // ceb_en_source=0,cfeb_en=1F
    status  = vme_write(adr,wr_data);

    // Turn off internal level 1 accept for sequencer
    adr     = seq_l1a_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x0FFF;
    status  = vme_write(adr,wr_data);

    // Select pattern trigger
    adr     = seq_trig_en_adr+base_adr;
    status  = vme_read(adr,rd_data);
    rd_data = rd_data & 0xFF00;
    wr_data = rd_data | 0x0001;
    status  = vme_write(adr,wr_data);

    // Clear previous ALCT inject
    adr     = alct_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFFD;
    status  = vme_write(adr,wr_data);

    // Set start_trigger state for FMM
    ttc_cmd = 6;            // start_trigger
    adr     = base_adr+ccb_cmd_adr;
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);
    wr_data = 0x0003 | (ttc_cmd << 8);
    status  = vme_write(adr,wr_data);
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);

    ttc_cmd = 1;            // bx0
    wr_data = 0x0003 | (ttc_cmd << 8);
    status  = vme_write(adr,wr_data);
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);

    // Arm scope trigger
    scp_arm        = true;
    scp_readout    = false;
    scp_raw_decode = false;
    scp_silent     = false;
    scp_playback   = false;
    if (rdscope)
        scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

    // Clear previous  CLCT inject
    adr     = cfeb_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x7FFF;
    status  = vme_write(adr,wr_data);

    // Fire CLCT Injector
    wr_data = wr_data | 0x8000;
    status  = vme_write(adr,wr_data);

    // Clear previous inject    
    wr_data = rd_data & 0x7FFF;
    status  = vme_write(adr,wr_data);

    // Check scintillator veto is set
    adr    = base_adr+seqmod_adr;
    status = vme_read(adr,rd_data);
    scint_veto = (rd_data>>13) & 0x1;
    if (scint_veto!=1) pause("scint veto failed to set");

    // Clear scintillator veto
    wr_data = rd_data | (1<<12);
    status  = vme_write(adr,wr_data);
    wr_data = wr_data ^ (1<<12);
    status  = vme_write(adr,wr_data);
    status  = vme_read(adr,rd_data);
    scint_veto = (rd_data>>12) & 0x1;
    if (scint_veto!=0) pause("scint veto failed to clear");

    // Read back embedded scope data
    scp_arm        = false;
    scp_readout    = true;
    scp_raw_decode = false;
    scp_silent     = true;
    scp_playback   = false;
    if (rdscope)
        scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);
} // close InjectALCTCLCT() {

//------------------------------------------------------------------------------
//  Inject ALCT+CLCT then readout raw hits
//------------------------------------------------------------------------------
//L16400:
int InjectALCTCLCT_Readout () {

    if (ifunc<0) rdscope = false;
    else         rdscope = true;

    // Turn off CCB backplane inputs, turn on L1A emulator
    adr     = ccb_cfg_adr+base_adr;
    wr_data = 0x003D;
    status  = vme_write(adr,wr_data);

    // Enable sequencer trigger, turn off dmb trigger, set internal l1a delay
    adr     = ccb_trig_adr+base_adr;
    wr_data = 0x0004;
    wr_data = wr_data | (l1a_delay << 8);
    status  = vme_write(adr,wr_data);

    // Turn off ALCT cable inputs, enable synchronized alct+clct triggers
    adr     = alct_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x0000;
    wr_data = wr_data | 0x0005;
    wr_data = wr_data | (alct_injector_delay << 5);
    status  = vme_write(adr,wr_data);

    // Set ALCT delay for TMB matching
    adr     = tmbtim_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFF0;
    wr_data = wr_data | 0x0003;
    status  = vme_write(adr,wr_data);

    // Turn off CLCT cable inputs
    adr     = cfeb_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFE0;
    status  = vme_write(adr,wr_data);

    // Turn on CFEB enables to over-ride mask_all
    adr     = seq_trig_en_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x03FF;     // clear old cfeb_en and source
    wr_data = wr_data | 0x7C00;     // ceb_en_source=0,cfeb_en=1F
    status  = vme_write(adr,wr_data);

    // Turn off internal level 1 accept for sequencer, set l1a window width
    adr     = seq_l1a_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x00FF;
    wr_data = wr_data | 0x0300;     // l1a window width
    status  = vme_write(adr,wr_data);

    // Take RAT out of sync mode
    adr     = vme_ratctrl_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFFE;
    status  = vme_write(adr,wr_data);

    // Enable RPC injector
    adr     = rpc_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFF80; // clear out old bits
    wr_data = wr_data | 0x0001; // rpc_mask_all 1=enable inputs from RPC

    if (rat_injector_sync) wr_data=wr_data | 0x0002;    // 1=enable RAT board injector
    else                   wr_data=wr_data | 0x0004;    // 1=enable RPC RAM internal injector

    rat_injector_delay=6;
    wr_data = wr_data | (rat_injector_delay << 3); // delay CLCT injector to wait for RAT
    status  = vme_write(adr,wr_data);

    // Select pattern trigger
    adr     = seq_trig_en_adr+base_adr;
    status  = vme_read(adr,rd_data);
    rd_data = rd_data & 0xFF00;
    wr_data = rd_data | 0x0001;
    status  = vme_write(adr,wr_data);
    // Clear previous ALCT inject
    adr     = alct_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFFD;
    status  = vme_write(adr,wr_data);

    // Set start_trigger state for FMM
    ttc_cmd = 6;            // start_trigger
    adr     = base_adr+ccb_cmd_adr;
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);
    wr_data = 0x0003 | (ttc_cmd << 8);
    status  = vme_write(adr,wr_data);
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);

    ttc_cmd = 1;            // bx0
    wr_data = 0x0003 | (ttc_cmd << 8);
    status  = vme_write(adr,wr_data);
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);

    // Clear DMB RAM write-address
    adr     = dmb_ram_adr+base_adr;
    wr_data = 0x2000;   //reset RAM write address
    status  = vme_write(adr,wr_data);
    wr_data = 0x0000;   // unreset
    status  = vme_write(adr,wr_data);

    // Arm scope trigger
    scp_arm        = true;
    scp_readout    = false;
    scp_raw_decode = false;
    scp_silent     = false;
    scp_playback   = false;
    if (rdscope)
        scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

    // Fire CLCT+ALCT Injectors
    adr     = cfeb_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data | 0x8000; // fire injector
    status  = vme_write(adr,wr_data);
    wr_data = rd_data & 0x7FFF; // unfire
    status  = vme_write(adr,wr_data);

    // Read back embedded scope data
    scp_arm        = false;
    scp_readout    = true;
    scp_raw_decode = false;
    scp_silent     = true;
    scp_playback   = false;
    if (rdscope)
        scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

    // Get DMB RAM word count and busy bit
    adr       = dmb_wdcnt_adr+base_adr;
    status    = vme_read(adr,rd_data);
    dmb_wdcnt = rd_data & 0x0FFF;
    dmb_busy  = (rd_data >> 14) & 0x0001;

    printf("\tdmb word count = %4i\n",dmb_wdcnt);
    printf("\tdmb busy       = %4i\n",dmb_busy);

    if (dmb_busy!=0) {
        pause ("Can not read RAM: dmb reports busy");
        return EXIT_FAILURE; 
    }

    if (dmb_wdcnt<=0) {
        pause ("Can not read RAM: dmb reports word count <=0");
        return EXIT_FAILURE; 
    }

    pause("<cr> to process dump");

    // Write RAM read address to TMB
    for (iadr=0; iadr<=dmb_wdcnt-1; ++iadr) {
        adr     = dmb_ram_adr+base_adr;
        wr_data = iadr & 0xFFFF;
        status  = vme_write(adr,wr_data);

        // Read RAM data from TMB
        adr    = dmb_rdata_adr+base_adr;
        status = vme_read(adr,rd_data);         // read lsbs
        dmb_rdata_lsb=rd_data;

        adr    = dmb_wdcnt_adr+base_adr;
        status = vme_read(adr,rd_data);         // read msbs
        dmb_rdata_msb = (rd_data >> 12) & 0x3;  // rdata msbs

        dmb_rdata = dmb_rdata_lsb | (dmb_rdata_msb << 16);

        fprintf(stdout,  "\tAdr=%4i Data=%5.5X\n",iadr,dmb_rdata);

    }   // close iadr

    // Clear RAM address for next event
    adr     = dmb_ram_adr+base_adr;
    wr_data = 0x2000;   // reset RAM write address
    status  = vme_write(adr,wr_data);
    wr_data = 0x0000;   // unreset
    status  = vme_write(adr,wr_data);
    return 0; 
} // close InjectALCTCLCT_Readout()

//------------------------------------------------------------------------------
//  Fire L1A to ALCT for timing-in clock + read
//------------------------------------------------------------------------------
int Fire_L1A_to_ALCT () {
    //L16500:
    fprintf(stdout, "ALCT test started\n");

    // Turn off CCB inputs to zero alct_adb_sync and ext_trig
    adr     = ccb_cfg_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFBF;         // Clear previous l1a
    wr_data = wr_data | 0x1;            // Turn off CCB backplane
    status  = vme_write(adr,wr_data);

    // Enable ALCT cable ports
    adr     = vme_loopbk_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data | 0x000C;         // alct_rxoe=txoe=1
    status  = vme_write(adr,wr_data);

    // Clear last event
    adr     = alctfifo1_adr+base_adr;
    wr_data = 1;                        // reset word counter
    status  = vme_write(adr,wr_data);
    wr_data = 0;                        // enable word counter
    status  = vme_write(adr,wr_data);

    // Make sure alct fifo went unbusy
    adr    = alct_fifo_adr+base_adr;
    status = vme_read(adr,rd_data);
    alct_raw_busy = (rd_data >> 0) & 0x0001;
    alct_raw_done = (rd_data >> 1) & 0x0001;
    if (alct_raw_busy==1) pause("alct fifo failed to clear");

    // Fire ext_trig to ALCT board
    //L16510:
    adr     = alct_cfg_adr+base_adr;
    status  = vme_read(adr,rd_data);    // get current state
    wr_data = rd_data & 0xFFF0;         // clear bits[3:0] alct ext trig
    wr_data = wr_data | 0x0004;         // fire alct ext trig
    //  wr_data = wr_data | 0x0008;         // or fire alct ext inject
    status  = vme_write(adr,wr_data);
    wr_data = rd_data & 0xFFF0;         // clear bits[3:0] alct ext trig
    status  = vme_write(adr,wr_data);

    // Read ALCT trigger words
    adr    = alct_alct0_adr+base_adr;
    status = vme_read(adr,rd_data);     // get current state
    alct0_rd = rd_data;

    adr    = alct_alct1_adr+base_adr;
    status = vme_read(adr,rd_data);     // get current state
    alct1_rd = rd_data;

    if (ifunc>0)
    {
        fprintf(stdout,"\tALCT0=%4.4X\n",alct0_rd);
        fprintf(stdout,"\tALCT1=%4.4X\n",alct1_rd);
        if (alct0_rd==alct0_prev) printf("\tALCT LCT0 unchanged\n");
        if (alct1_rd==alct1_prev) printf("\tALCT LCT1 unchanged\n");
    }
    alct0_prev = alct0_rd;
    alct1_prev = alct1_rd;

    // Fire CCB L1A oneshot to ALCT
    adr     = ccb_cfg_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFBF;         // Clear previous l1a
    wr_data = wr_data | 0x1;            // Turn off CCB backplane
    wr_data = wr_data | 0x0040;         // Fire ccb L1A oneshot
    status  = vme_write(adr,wr_data);
    wr_data = wr_data & 0xFFBF;         // Clear previous l1a   
    status  = vme_write(adr,wr_data);

    // Check alct fifo status
    for (i=1; i<=100; ++i)              // cheap readout delay 
    {
        adr    = alct_fifo_adr+base_adr;
        status = vme_read(adr,rd_data);
        alct_raw_busy = (rd_data >> 0) & 0x0001;
        alct_raw_done = (rd_data >> 1) & 0x0001;
        if (alct_raw_busy==0) 
            break; 
    }
    pause("alct fifo stuck busy");

    //L16520:
    printf("\tALCT L1A alct_raw_done waits=%5i\n",i);
    if (alct_raw_done!=1) pause("alct fifo not done");

    // Get alct word count
    adr    = alct_fifo_adr+base_adr;    // alct word count
    status = vme_read(adr,rd_data);
    alct_raw_nwords = (rd_data >> 2) & 0x07FF;
    fprintf(stdout,"\talct_raw_nwords=%5i\n",alct_raw_nwords);

    // Read alct fifo data
    for (i=0; i<=max(alct_raw_nwords-1,0); ++i) {
        //  for (i=1; i<=alct_raw_nwords; ++i) 
        adr     = alctfifo1_adr+base_adr;
        wr_data = (i<<1);                   // ram read address
        status  = vme_write(adr,wr_data);

        adr     = alctfifo2_adr+base_adr;   // alct raw data lsbs
        status  = vme_read(adr,rd_data);
        alct_raw_data = rd_data;
        fprintf(stdout,"adr=4i alct raw lsbs=%4.4X\n",rd_data);

        adr     = alct_fifo_adr+base_adr;   // alct raw data msbs
        status  = vme_read(adr,rd_data);
        fprintf(stdout,"adr=4i alct raw msbs=%4.4X\n",rd_data);
        rd_data = (rd_data>>13) & 0x0003;
        alct_raw_data = alct_raw_data | (rd_data<<16);

        if (i<mxframe) vf_data[i] = alct_raw_data;
        if (i<=3 || i>=alct_raw_nwords-4)
            fprintf(stdout,  "\t%5i %5.5X\n",i,alct_raw_data);
    }

    // calculate CRC for data stream
    dmb_wdcnt = alct_raw_nwords;
    if (dmb_wdcnt<5) {
        printf("Raw hits dump too short for crc calc dmb_wdcnt=%i\n",dmb_wdcnt);
        pause("<cr> to resume");
        return EXIT_FAILURE; 
    }

    for (iframe=0; iframe<=dmb_wdcnt-1-4; ++iframe) // dont include last 4 frames
    {
        din = vf_data[iframe];
        if (iframe==0) crc22a(din,crc,1);               // reset crc
        else           crc22a(din,crc,0);
    }

    // compare our computed CRC to what TMB computed
    tmb_crc_lsb = vf_data[dmb_wdcnt-1-3] & 0x07FF;  // 11 crc bits per frame
    tmb_crc_msb = vf_data[dmb_wdcnt-1-2] & 0x07FF;  // 11 crc bits per frame
    tmb_crc     = tmb_crc_lsb | (tmb_crc_msb<<11);  // full 22 bit crc
    crc_match   = crc==tmb_crc;

    //fprintf(stdout,"\tcalc_crc=%6.6X alct_crc=%6.6X match=%c\n",crc,tmb_crc,crc_match,logical(crc_match));
    fprintf(stdout,"\tcalc_crc=%6.6ld alct_crc=%6.6ld match=%i\n",crc,tmb_crc,crc_match);

    if (!crc_match) pause("ALCT crc error, WTF!");
    return EXIT_FAILURE; 
} // close void Fire_L1A_to_ALCT ()

//------------------------------------------------------------------------------
//  Fire CLCT ext trig linked with ALCT ext trig
//------------------------------------------------------------------------------
void fire_CLCT_ext_trig_with_ALCT () {
    while(true) {
        //L16600:
        adr     = ccb_trig_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFFEF;         // Clear previous trigger
        status  = vme_write(adr,wr_data);
        wr_data = wr_data | 0x0030;         // Fire CLCT ext trig linked with ALCT ext trig
        status  = vme_write(adr,wr_data);
        wr_data = rd_data & 0xFFEF;         // Clear previous trigger   
        status  = vme_write(adr,wr_data);
        if (ifunc<0) break; 
    }
} //close fire_CLCT_ext_trig_with_ALCT ()

//------------------------------------------------------------------------------
//  Inject walking CLCT muon check CLCT0 1/2strip
//------------------------------------------------------------------------------
void Inject_walking_CLCT() {
    first_event = true;
    first_scn   = true;

    if (ifunc<0) rdscope = false;
    else         rdscope = true;

    // Get csc_id
    adr    = base_adr+seq_id_adr;
    status = vme_read(adr,rd_data);
    csc_id = (rd_data >> 5) & 0x1F;

    // Get current thresholds
    adr    = seq_clct_adr+base_adr;
    status = vme_read(adr,rd_data);
    triad_persist       = (rd_data >>  0) & 0xF;    // 4 bits
    hit_thresh_pretrig  = (rd_data >>  4) & 0x7;    // 3 bits
    dmb_thresh_pretrig  = (rd_data >>  7) & 0x7;    // 3 bits
    hit_thresh_postdrift= (rd_data >> 10) & 0x7;    // 3 bits
    drift_delay         = (rd_data >> 13) & 0x3;    // 2 bits

    adr    = temp0_adr+base_adr;
    status = vme_read(adr,rd_data);
    pid_thresh_pretrig  = (rd_data >> 2) & 0xF;     // 4 bits
    pid_thresh_postdrift= (rd_data >> 6) & 0xF;     // 4 bits

    adr    = layer_trig_adr+base_adr;
    status = vme_read(adr,rd_data);
    lyr_thresh_pretrig = (rd_data >> 1) & 0xF;      // 4 bits

    // Get current adjacent cfeb separation
    adr    = temp0_adr+base_adr;
    status = vme_read(adr,rd_data);
    adjcfeb_dist=(rd_data >> 10) & 0x3F;

    // Get current CLCT separation
    adr    = temp1_adr+base_adr;
    status = vme_read(adr,rd_data);
    clct_sep = (rd_data >> 8) & 0xFF;

    // Get current active_feb_list source
    adr    = seqmod_adr+base_adr;
    status = vme_read(adr,rd_data);
    active_feb_src=(rd_data >> 14) & 0x1;

    // Get current cfeb tbins
    adr    = base_adr+seq_fifo_adr;
    status = vme_read(adr,rd_data);

    fifo_mode       = (rd_data >>  0) & 0x07;   // 3 bits
    fifo_tbins      = (rd_data >>  3) & 0x1F;   // 5 bits
    fifo_pretrig    = (rd_data >>  8) & 0x1F;   // 5 bits
    bcb_read_enable = (rd_data >> 15) & 0x1;    // 1 bit
    bcb_read_enable = 1;    //! set to 1 manually until it becomes the default
    if (fifo_tbins==0) fifo_tbins=32;

    // Get current match parameters
    adr    = base_adr+tmbtim_adr;
    status = vme_read(adr,rd_data);

    alct_delay      = (rd_data >> 0) & 0xF;     // 4 bits
    clct_width      = (rd_data >> 4) & 0xF;     // 4 bits

    // Get current tmb_match mode
    adr    = base_adr+tmb_trig_adr;
    status = vme_read(adr,rd_data);

    tmb_allow_alct  = (rd_data >> 2) & 0x1;     // 1 bit
    tmb_allow_clct  = (rd_data >> 3) & 0x1;     // 1 bit
    tmb_allow_match = (rd_data >> 4) & 0x1;     // 1 bit

    // Get current tmb_match mode for ME1AB
    adr    = base_adr+non_trig_adr;
    status = vme_read(adr,rd_data);

    tmb_allow_alct_ro   = (rd_data >> 0) & 0x1; // 1=Allow ALCT-only non-triggering readout
    tmb_allow_clct_ro   = (rd_data >> 1) & 0x1; // 1=Allow CLCT-only non-triggering readout
    tmb_allow_match_ro  = (rd_data >> 2) & 0x1; // 1=Allow ALCT*CLCT non-triggering readout
    mpc_me1a_block      = (rd_data >> 3) & 0x1; // Block ME1A LCTs from MPC, still queue for readout

    // Get current alct injector delay
    adr    = base_adr+alct_inj_adr;
    status = vme_read(adr,rd_data);
    alct_injector_delay=(rd_data >> 5) & 0x1F;  // 5 bits

    // Get current alct bx0 enable
    adr    = base_adr+bx0_delay_adr;
    status = vme_read(adr,rd_data);
    alct_bx0_en=(rd_data >> 8) & 0x1;           // 1 bit

    // Get current rat injector delay and data=address mode
    adr    = base_adr+rpc_inj_adr;
    status = vme_read(adr,rd_data);
    inj_delay_rat  = (rd_data >>  3) & 0xF;     // 4 bits
    rpc_tbins_test = (rd_data >> 15) & 0x1;     // 1bit

    // Get current RPC readout list
    adr    = base_adr+rpc_cfg_adr;
    status = vme_read(adr,rd_data);
    rpc_exists=(rd_data >> 0) & 0x3;            // 2 bits

    // Get current RPC tbins
    adr    = base_adr+rpc_tbins_adr;
    status = vme_read(adr,rd_data);

    fifo_tbins_rpc  = (rd_data >> 0) & 0x1F;    // 5 bits
    fifo_pretrig_rpc= (rd_data >> 5) & 0x1F;    // 5 bits
    rpc_decouple    = (rd_data >>10) & 0x1F;    // 5 bits
    if (fifo_tbins_rpc==0) fifo_tbins_rpc=32;

    // Get scope-in-readout
    adr    = base_adr+scp_ctrl_adr;
    status = vme_read(adr,rd_data);
    scp_auto   =(rd_data >> 3) & 0x1;           // 1 bit
    scp_nowrite=(rd_data >> 4) & 0x1;           // 1 bit
    scp_tbins  =(rd_data >> 5) & 0x7;           // 3 bits

    // Get miniscope-in-readout
    adr    = base_adr+miniscope_adr;
    status = vme_read(adr,rd_data);
    mini_read_enable    =(rd_data >> 0) & 0x1;  // 1 bit
    mini_test           =(rd_data >> 1) & 0x1;  // 1 bit
    mini_tbins_word     =(rd_data >> 2) & 0x1;  // 1 bit
    fifo_tbins_mini     =(rd_data >> 3) & 0x1F; // 5 bits
    fifo_pretrig_mini   =(rd_data >> 8) & 0x1F; // 5 bits

    // Get L1A delay
    adr     = ccb_trig_adr+base_adr;
    status  = vme_read(adr,rd_data);
    l1a_delay = (rd_data >> 8) & 0xFF;

    if (first_event)l1a_delay=119;              // hits 0th l1a window bx, override TMB default

    // Get L1A lookback
    adr    = base_adr+l1a_lookback_adr;
    status = vme_read(adr,rd_data);
    l1a_lookback = (rd_data & 0x07FF);

    // Get sync error forced
    adr    = base_adr+sync_err_ctrl_adr;
    status = vme_read(adr,rd_data);
    sync_err_force = (rd_data>>15) & 0x1;

    // Inquire
    cprr           = true;
    cprr_ignore    = false;
    layer_mode     = false;
    pause_on_fail  = false;
    rpcs_in_rdout  = true;
    rrhd           = true;
    mcl            = mxclcts-1;
    vme_bx0_emu_en = vme_bx0_emu_en_default;
    send_emulator_to_tmb     = true;
    load_clct_injector_image = false;

    for (iclct=0; iclct<mxclcts; ++iclct)
    {
        clct_key_inject[iclct] = -1;
        clct_pid_inject[iclct] = 0xA;
        clct_hit_inject[iclct] = 0;
        clct_blanked[iclct]    = false;
        loop_keys[iclct]       = false;
        loop_pids[iclct]       = false;
        clct_hit_inject_clip[iclct] = 0;
    }

    inquirb("\tCheck Pattern RAM readback [y|n]? cr=%3c", cprr);
    inquire("\tBX0 emulator enable             ? cr=%3i", minv= 0, maxv=  1, radix=10, vme_bx0_emu_en);
    inquire("\tBX0 sync error force            ? cr=%3i", minv= 0, maxv=  1, radix=10, sync_err_force);
    inquire("\tCSC ID                          ? cr=%3i", minv= 0, maxv= 31, radix=10, csc_id);
    inquirb("\tLayer-trigger mode         [y|n]? cr=%3c", layer_mode);

    if (layer_mode)
        inquire("\tLyr_thresh_pretrig              ? cr=%3i", minv= 0, maxv= 7, radix=10, lyr_thresh_pretrig);

    inquire("\tHit_thresh_pretrig              ? cr=%3i", minv= 0, maxv=  7, radix=10, hit_thresh_pretrig);
    inquire("\tPid_thresh_pretrig              ? cr=%3X", minv= 0, maxv= 15, radix=16, pid_thresh_pretrig);
    inquire("\tDmb_thresh_pretrig              ? cr=%3i", minv= 0, maxv=  7, radix=10, dmb_thresh_pretrig);
    inquire("\tHit_thresh_postdrift            ? cr=%3i", minv= 0, maxv=  7, radix=10, hit_thresh_postdrift);
    inquire("\tPid_thresh_postdrift            ? cr=%3X", minv= 0, maxv= 15, radix=16, pid_thresh_postdrift);
    inquire("\tAdjacent cfeb distance          ? cr=%3i", minv= 0, maxv= 63, radix=10, adjcfeb_dist);
    inquire("\tActive_feb_list 0|1=pre|tmb     ? cr=%3i", minv= 0, maxv=  1, radix=10, active_feb_src);
    inquire("\tTriad persistence               ? cr=%3i", minv= 0, maxv= 15, radix=10, triad_persist);
    inquire("\tDrift delay                     ? cr=%3i", minv= 0, maxv=  3, radix=10, drift_delay);
    inquire("\tAlct*clct match window          ? cr=%3i", minv= 0, maxv= 15, radix=10, clct_width);
    inquire("\tAlct Delay                      ? cr=%3i", minv= 0, maxv= 15, radix=10, alct_delay);
    inquire("\tAlct Injector_delay nom=13      ? cr=%3i", minv= 0, maxv= 15, radix=10, alct_injector_delay);
    inquire("\tAlct Enable alct_bx0            ? cr=%3i", minv= 0, maxv=  2, radix=10, alct_bx0_en);
    inquirb("\tClct Send emulator to tmb  [y|n]? cr=%3c", send_emulator_to_tmb);
    inquire("\tClct Mpc_me1a_block             ? cr=%3i", minv= 0, maxv=mcl, radix=10, mpc_me1a_block);
    inquire("\tClct Separation (blanking)      ? cr=%3i", minv= 0, maxv=255, radix=10, clct_sep);
    inquirb("\tClct Load InjectorRAM image[y|n]? cr=%3c", load_clct_injector_image);
    inquire("\tAlct Inject 0,1,2 alcts         ? cr=%3i", minv= 0, maxv=  2, radix=10, nalcts_inject);
    inquire("\tClct Inject 1 or 2 clcts        ? cr=%3i", minv= 0, maxv=mcl, radix=10, nclcts_inject);

    for (iclct=0; iclct<nclcts_inject; ++iclct)
    {
        if (nclcts_inject>=mxclcts) stop("nclcts_inject>=mxclct");
        if (iclct==0) {
            clct_hit_inject[iclct] = 6;
            inquir2("\tClct%1i Key half-strip0-159 -1=all? cr=%3i", minv=-1, maxv=159, radix=10, iclct,clct_key_inject[iclct]);
            inquir2("\tClct%1i Pattern ID   2-A    -1=all? cr=%3X", minv=-1, maxv=0xA, radix=16, iclct,clct_pid_inject[iclct]);
            inquir2("\tClct%1i Pattern Hits 0-6          ? cr=%3i", minv= 0, maxv=6,   radix=10, iclct,clct_hit_inject[iclct]);
        }
        if (iclct==1) {
            clct_hit_inject[iclct] = 5;
            inquir2("\tClct%1i Key half-strip159-0 -1=all? cr=%3i", minv=-1, maxv=159, radix=10, iclct,clct_key_inject[iclct]);
            inquir2("\tClct%1i Pattern ID   2-A    -1=all? cr=%3X", minv=-1, maxv=0xA, radix=16, iclct,clct_pid_inject[iclct]);
            inquir2("\tClct%1i Pattern Hits 0-6          ? cr=%3i", minv= 0, maxv=6,   radix=10, iclct,clct_hit_inject[iclct]);
        }
        if (iclct>=2) {
            clct_hit_inject[iclct] = 4;
            inquir2("\tClct%1i Key half-strip0-159       ? cr=%3i", minv=-1, maxv=159, radix=10, iclct,clct_key_inject[iclct]);
            inquir2("\tClct%1i Pattern ID   2-A          ? cr=%3X", minv=-1, maxv=0xA, radix=16, iclct,clct_pid_inject[iclct]);
            inquir2("\tClct%1i Pattern Hits 0-6          ? cr=%3i", minv= 0, maxv=6,   radix=10, iclct,clct_hit_inject[iclct]);
        }
    }   // close for iclct


    // interactive mode 
    if (interactive) {
        printf ("\tClct Triad 1st tbin ly5:ly0     ? cr=000000 ");
        fgets(line, 80, stdin);
        n=strlen(line);
        sscanf(line,"%1i,%1i,%1i,%1i,%1i,%1i",&i,&i,&i,&i,&i,&i);
        if (n!=0) {
            for (i=0; i<=5; ++i) {
                triad_1st_tbin[i]=0;
            } // close for i
        } // close if n!=0
    } // close interactive mode


    inquire("\tAllow TMB alct*clct match       ? cr=%3i", minv= 0, maxv=  1, radix=10, tmb_allow_match);
    inquire("\tAllow TMB alct-only             ? cr=%3i", minv= 0, maxv=  1, radix=10, tmb_allow_alct);
    inquire("\tAllow TMB clct-only             ? cr=%3i", minv= 0, maxv=  1, radix=10, tmb_allow_clct);
    inquire("\tAllow TMB alct*clct match_ro    ? cr=%3i", minv= 0, maxv=  1, radix=10, tmb_allow_match_ro);
    inquire("\tAllow TMB alct-only_ro          ? cr=%3i", minv= 0, maxv=  1, radix=10, tmb_allow_alct_ro);
    inquire("\tAllow TMB clct-only_ro          ? cr=%3i", minv= 0, maxv=  1, radix=10, tmb_allow_clct_ro);
    inquirb("\tCFEB Read out raw hits dump[y|n]? cr=%3c", rrhd);
    inquire("\tCFEB Tbins                      ? cr=%3i", minv= 0, maxv= 31, radix=10, fifo_tbins);
    inquire("\tCFEB Tbins before pretrig       ? cr=%3i", minv= 0, maxv= 31, radix=10, fifo_pretrig);
    inquire("\tCFEB FIFO mode                  ? cr=%3i", minv= 0, maxv=  7, radix=10, fifo_mode);
    inquire("\tCFEB Blockedbits readout enable ? cr=%3i", minv= 0, maxv=  1, radix=10, bcb_read_enable);
    inquire("\tL1A  Delay                      ? cr=%3i", minv= 0, maxv=256, radix=10, l1a_delay);
    inquire("\tL1A  Lookback                   ? cr=%3i", minv= 0, maxv=256, radix=10, l1a_lookback);

    // interactive mode
    if (interactive) {
        printf ("\tRPC Injector rat,tmb,sync,none  ? (r/t/s/n)");
        fgets(line, 80, stdin);
        n=strlen(line);
        sscanf(line,"%d",&i);
    } //close interactive mode

    rat_injector_sync   = false;
    rat_injector_enable = true;
    rat_sync_mode=0;

    if ((n!=0) && (i=='r' || i=='R')) rat_injector_sync   = true;
    if ((n!=0) && (i=='n' || i=='N')) rat_injector_enable = false;
    if ((n!=0) && (i=='s' || i=='S')) rat_sync_mode       = 1;

    inquire("\tRAT injector delay              ? cr=%3i", minv= 0, maxv=256, radix=10, inj_delay_rat);
    inquire("\tRPC data=address mode           ? cr=%3i", minv= 0, maxv=  1, radix=10, rpc_tbins_test);
    inquirb("\tRPCs in readout            [y|n]? cr=%3c", rpcs_in_rdout);

    printf ("\tRPC list to readout             ? cr= %1i%1i",(rpc_exists>>1)&0x1,rpc_exists&0x1);
    fgets(line, 80, stdin);
    n=strlen(line);
    sscanf(line,"%i,%i",&i,&j);
    if (n!=0) rpc_exists= (j<<1) | i; 

    inquire("\tRPC TBins Decouple              ? cr=%3i", minv= 0, maxv=  1, radix=10, rpc_decouple);
    inquire("\tRPC Tbins                       ? cr=%3i", minv= 0, maxv= 31, radix=10, fifo_tbins_rpc);
    inquire("\tRPC Tbins before pretrig        ? cr=%3i", minv= 0, maxv= 31, radix=10, fifo_pretrig_rpc);
    inquire("\tRPC Scope in readout            ? cr=%3i", minv= 0, maxv=  1, radix=10, scp_auto);
    inquire("\tScope tbins/64                  ? cr=%3i", minv= 0, maxv= 31, radix=10, scp_tbins);
    inquire("\tScope nowrite                   ? cr=%3i", minv= 0, maxv=  1, radix=10, scp_nowrite);    // nowrite=1 uses tmb scope ram initial test pattern
    inquire("\tMiniscope in readout            ? cr=%3i", minv= 0, maxv= 31, radix=10, mini_read_enable);

    if (mini_read_enable==1) {
        inquire("\tMiniscope test mode             ? cr=%3i", minv= 0, maxv= 31, radix=10, mini_test);
        inquire("\tMiniscope 1st word=tbins        ? cr=%3i", minv= 0, maxv= 31, radix=10, mini_tbins_word);
        inquire("\tMiniscope tbins                 ? cr=%3i", minv= 0, maxv= 31, radix=10, fifo_tbins_mini);
        inquire("\tMiniscope tbins pretrig         ? cr=%3i", minv= 0, maxv= 31, radix=10, fifo_pretrig_mini);
    }
    inquirb("\tPause on fail              [y|n]? cr=%3c", pause_on_fail);


    // Turn off CFEB cable inputs
    //L16705:
    while(true) {  //CLCT Bang Mode loop
        adr       = cfeb_inj_adr+base_adr;
        mask_all  = 0x00;   // 5'b00000 disables cfeb inputs
        febsel    = 0x00;   // Injector RAM select
        mask_cfeb = 0x00;   // Injector RAMs to fire
        inj_trig  = 0;      // Injector fire bit       
        wr_data   = (mask_all  << 0)  |
            (febsel    << 5)  |
            (mask_cfeb << 10) |
            (inj_trig  << 15);
        status    = vme_write(adr,wr_data);

        // Turn off GTX optical receiver inputs
        adr     = adr_virtex6_gtx_rx_all+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFFFE;     // Turn of enable_all bt
        status  = vme_write(adr,wr_data);

        // Turn off ALCT cable inputs, enable synchronized alct+clct triggers
        adr     = alct_inj_adr+base_adr;
        status  = vme_read(adr,rd_data);

        alct_injector_delay=13;

        wr_data = rd_data & 0x0000;
        wr_data = wr_data | 0x0005;
        wr_data = wr_data | (alct_injector_delay << 5);
        status  = vme_write(adr,wr_data);

        // Turn on CFEB enables to over-ride mask_all
        adr     = seq_trig_en_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x03FF;                 // clear old cfeb_en and source
        wr_data = wr_data | 0x7C00;                 // ceb_en_source=0,cfeb_en=1F
        status  = vme_write(adr,wr_data);

        // Reset bad bits
        adr     = cfeb_badbits_ctrl_adr+base_adr;   // CFEB  Bad Bit Control/Status
        status  = vme_read(adr,rd_data);            // read current
        rd_data = rd_data & 0xFFE0;                 // clean out old reset bits
        wr_data = rd_data | 0x001F;                 // assert reset[4:0]
        status  = vme_write(adr,wr_data);           // write reset
        wr_data = rd_data;                          // retrieve original register contents
        status  = vme_write(adr,wr_data);           // restore register with resets off

        // Enable sequencer trigger, turn off dmb trigger, set internal l1a delay
        adr     = ccb_trig_adr+base_adr;
        wr_data = 0x0004;
        wr_data = wr_data | (l1a_delay << 8);
        status  = vme_write(adr,wr_data);

        // Set csc_id
        adr    = base_adr+seq_id_adr;
        status = vme_read(adr,rd_data);
        wr_data = rd_data & ~(0x1F << 5);           // clear old csc_id
        wr_data = wr_data | (csc_id << 5);          // new csc_id
        status  = vme_write(adr,wr_data);

        // Set L1A lookback
        adr     = base_adr+l1a_lookback_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & ~0x07FF;
        wr_data = wr_data | (l1a_lookback << 0);
        status  = vme_write(adr,wr_data);

        // Turn off CCB backplane inputs, turn on L1A emulator, do this after turning off cfeb and alct cable inputs
        adr     = ccb_cfg_adr+base_adr;
        wr_data = 0x0000;
        wr_data = wr_data | 0x0001; // ccb_ignore_rx
        wr_data = wr_data | 0x0004; // ccb_int_l1a_en
        wr_data = wr_data | 0x0008; // ccb_status_oe_vme
        wr_data = wr_data | 0x0010; // alct_status_en
        wr_data = wr_data | 0x0020; // clct_status_en
        wr_data = wr_data | (vme_bx0_emu_en << 15); // bx0 emualtor
        status  = vme_write(adr,wr_data);

        vme_bx0_emu_en_default = vme_bx0_emu_en;    // update default so u dont have to keep entering

        // Set ALCT delay for TMB matching
        adr     = tmbtim_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFF00;
        wr_data = wr_data | (alct_delay << 0);
        wr_data = wr_data | (clct_width << 4);
        status  = vme_write(adr,wr_data);

        // Set tmb_match mode
        adr     = base_adr+tmb_trig_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFFE7;     // clear bits 4,3
        wr_data = wr_data | (tmb_allow_alct  << 2);
        wr_data = wr_data | (tmb_allow_clct  << 3);
        wr_data = wr_data | (tmb_allow_match << 4);
        status  = vme_write(adr,wr_data);

        // Set tmb_match mode for ME1AB
        adr     = base_adr+non_trig_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFFF0;
        wr_data = wr_data | (tmb_allow_alct_ro  << 0);  // 1=Allow ALCT-only non-triggering readout
        wr_data = wr_data | (tmb_allow_clct_ro  << 1);  // 1=Allow CLCT-only non-triggering readout
        wr_data = wr_data | (tmb_allow_match_ro << 2);  // 1=Allow ALCT*CLCT non-triggering readout
        wr_data = wr_data | (mpc_me1a_block     << 3);  // Block ME1A LCTs from MPC, still queue for readout
        status  = vme_write(adr,wr_data);

        // Set alct bx0 enable
        adr     = base_adr+bx0_delay_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & ~(1 << 8);
        wr_data = wr_data | (alct_bx0_en << 8);
        status  = vme_write(adr,wr_data);

        // Turn on layer trigger mode if its selected
        layer_trig_en=0;
        if (layer_mode) layer_trig_en=1;
        adr     = layer_trig_adr+base_adr;
        wr_data = layer_trig_en | (lyr_thresh_pretrig << 1);
        status  = vme_write(adr,wr_data);

        // Turn off internal level 1 accept for sequencer, set l1a window width
        adr     = seq_l1a_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x00FF;
        wr_data = wr_data | 0x0300;         //  l1a window width
        status  = vme_write(adr,wr_data);

        // Set fifo tbins
        adr     = base_adr+seq_fifo_adr;
        status  = vme_read(adr,rd_data);    // get current
        wr_data = rd_data & 0xF000;         // clear lower bits

        wr_data=wr_data
            | ((fifo_mode       & 0x7 ) <<  0)  // [2:0]
            | ((fifo_tbins      & 0x1F) <<  3)  // [7:3]
            | ((fifo_pretrig    & 0x1F) <<  8)  // [12:8]
            | ((bcb_read_enable & 0x1 ) << 15); // [15]

        status = vme_write(adr,wr_data);

        // Set pid_thresh_pretrig, pid_thresh_postdrift
        adr    = temp0_adr+base_adr;
        status = vme_read(adr,rd_data);

        wr_data=rd_data & 0xFC03;
        wr_data=wr_data | (pid_thresh_pretrig   << 2);  // 4 bits
        wr_data=wr_data | (pid_thresh_postdrift << 6);  // 4 bits

        status = vme_write(adr,wr_data);

        // Set adjcfeb_dist
        adr     = temp0_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x03FF;         // adjcfeb_dist[5:0] is in [15:10]
        wr_data = wr_data | (adjcfeb_dist << 10);
        status  = vme_write(adr,wr_data);

        // Set CLCT separation
        adr     = temp1_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x00FF;
        wr_data = wr_data | (clct_sep << 8);
        status  = vme_write(adr,wr_data);

        // Set active_feb_list source
        adr     = seqmod_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & ~(1 << 14);
        wr_data = wr_data | (active_feb_src << 14);
        status  = vme_write(adr,wr_data);

        // Set RAT out of sync mode
        adr     = vme_ratctrl_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFFFE;
        wr_data = wr_data | rat_sync_mode;
        status  = vme_write(adr,wr_data);

        // Set miniscope
        adr     = base_adr+miniscope_adr;
        wr_data = 0;
        wr_data = wr_data | (mini_read_enable  << 0);   // 1 bit
        wr_data = wr_data | (mini_test         << 1);   // 1 bit
        wr_data = wr_data | (mini_tbins_word   << 2);   // 1 bit
        wr_data = wr_data | (fifo_tbins_mini   << 3);   // 5 bits
        wr_data = wr_data | (fifo_pretrig_mini << 8);   // 5 bits
        status  = vme_write(adr,wr_data);

        // Include RPCs in readout
        adr     = rpc_cfg_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFFEC;                     //  Turn off bits4,1,0
        wr_data = wr_data | (rpc_exists & 0x3);         //  Turn on existing RPCs
        if (rpcs_in_rdout) wr_data=wr_data | (1 << 4);  //  Turn on bit 4 to enable
        status  = vme_write(adr,wr_data);

        // Set RPC tbins
        adr     = base_adr+rpc_tbins_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xF800;                     // Clear out old values
        wr_data = wr_data | (fifo_tbins_rpc     <<  0); // 5 bits
        wr_data = wr_data | (fifo_pretrig_rpc   <<  5); // 5 bits
        wr_data = wr_data | (rpc_decouple       << 10); // 5 bits
        status  = vme_write(adr,wr_data);

        // Enable RPC injector
        adr     = rpc_inj_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFF80;                 // Clear out old bits
        wr_data = wr_data | 0x0001;                 // rpc_mask_all 1=enable inputs from RPC
        if (rat_injector_enable) {
            if (rat_injector_sync  ) {
                wr_data = wr_data | 0x0002; }               // 1=enable RAT board injector
            else {
                wr_data = wr_data & 0xFFFE;                 // Turn off bit[0], masks RPC signals from RAT
                wr_data = wr_data | 0x0004;                 // 1=enable TMBs RPC RAM internal injector
                printf("\tUsing TMBs RPC Injector RAM, RAT disconnected.\n");
            }}

        //  rat_injector_delay=7;
        wr_data = wr_data | (inj_delay_rat  <<  3); // delay CLCT injector to wait for RAT
        wr_data = wr_data | (rpc_tbins_test << 15); // Data=daddress mode
        status  = vme_write(adr,wr_data);

        // Load RPC injector RAM
        if (rat_injector_enable && !rat_injector_sync) {

            adr     = rpc_inj_adr+base_adr;             // Get current injector control
            status  = vme_read(adr,rd_data);
            wr_data = rd_data & 0x807F;                 // Clear injector data, set inj_sel=0 to point to pad rams
            status  = vme_write(adr,wr_data);

            for (irpc=0; irpc<=1;   ++irpc) {           // Create rpc pad+bxn image
                for (ibxn=0; ibxn<=255; ++ibxn) {

                    if (ibxn <= 255) {
                        if (irpc==0) rpc_pad=ibxn | (0xAB << 8);    // rpc0 pads AB00-ABFF
                        if (irpc==1) rpc_pad=ibxn | (0xCD << 8);    // rpc1 pads CD00-CDFF
                        rpc_inj_bxn=7-ibxn;
                        rpc_inj_image[ibxn][irpc]=rpc_pad | (rpc_inj_bxn << 16);}
                    else {
                        rpc_inj_image[ibxn][irpc]=0;                // zero the rest of the ram
                    }   // close if ibxn
                }   // close do ibxn
            }   // close do irpc

            for (irpc=0; irpc<=1;   ++ irpc) {
                for (ibxn=0; ibxn<=255; ++ ibxn) {
                    fprintf(stdout,"rpc_inj_image writing%2i%2i%6.5X\n",ibxn,irpc,rpc_inj_image[ibxn][irpc]);
                    adr     = rpc_inj_wdata_adr+base_adr;       // pad data to write to ram
                    wr_data = rpc_inj_image[ibxn][irpc] & 0x0000FFFF;
                    status  = vme_write(adr,wr_data);

                    adr     = rpc_inj_adr+base_adr;             // get current injector control
                    status  = vme_read(adr,rd_data);
                    wr_data = rd_data & 0x807F;                 // clear bxn data
                    rpc_inj_bxn=(rpc_inj_image[ibxn][irpc] >> 16) & 0x7;
                    wr_data = wr_data | (rpc_inj_bxn << 8);     // new bxn
                    wr_data = wr_data | (1           << 7);     // set inj_sel=1
                    status  = vme_write(adr,wr_data);

                    adr     = rpc_inj_adr_adr+base_adr;         // ram write strobes
                    rpc_inj_wen   = (1 << irpc);                // select this ram
                    rpc_inj_ren   = 0;
                    rpc_inj_rwadr = ibxn;                       // at this address
                    wr_data = rpc_inj_wen | (rpc_inj_ren << 4) | (rpc_inj_rwadr << 8) | (rpc_tbins_test << 15);     // set wen=1
                    status  = vme_write(adr,wr_data);
                    rpc_inj_wen=0;
                    wr_data = rpc_inj_wen | (rpc_inj_ren << 4) | (rpc_inj_rwadr << 8) | (rpc_tbins_test << 15);     // set wen=0
                    status  = vme_write(adr,wr_data);
                }   // close for ibxn
            }   // close for irpc
        }   // if (rat_injector_enable

        // Verify RPC injector RAM
        if (rat_injector_enable & !rat_injector_sync) {

            for (irpc=0; irpc<=1; ++irpc) {
                for (ibxn=0; ibxn<=7; ++ibxn) {
                    adr = rpc_inj_adr_adr+base_adr;         // ram read strobes
                    rpc_inj_wen   = 0;                      // select this ram
                    rpc_inj_ren   = (1 << irpc);
                    rpc_inj_rwadr = ibxn;                   // at this address
                    wr_data = rpc_inj_wen | (rpc_inj_ren << 4) | (rpc_inj_rwadr << 8) | (rpc_tbins_test << 15);     // set ren=1
                    status  = vme_write(adr,wr_data);

                    adr    = rpc_inj_rdata_adr+base_adr;    // read pad data
                    status = vme_read(adr,rd_data);
                    rpc_inj_data=rd_data;

                    adr    = rpc_inj_adr+base_adr;          // read bxn data
                    status = vme_read(adr,rd_data);
                    rpc_inj_bxn = (rd_data >> 11) & 0x7;
                    rpc_inj_data = rpc_inj_data | (rpc_inj_bxn << 16);

                    adr = rpc_inj_adr_adr+base_adr;         // ram read strobes
                    rpc_inj_ren=0;
                    wr_data = rpc_inj_wen | (rpc_inj_ren << 4) | (rpc_inj_rwadr << 8) | (rpc_tbins_test << 15);     // set ren=0
                    status  = vme_write(adr,wr_data);

                    fprintf(stdout ,"rpc_inj_data reading%2i%2i%6.5X\n",ibxn,irpc,rpc_inj_data);

                    if (rpc_inj_data != rpc_inj_image[ibxn][irpc])
                        fprintf(stdout,  "RPC injector RAM error at adr=%2.2i expect=%5.5X read=%5.5X\n",rpc_inj_rwadr,rpc_inj_image[ibxn][irpc],rpc_inj_data);

                }   // close for ibxn
            }   // close for irpc
        }   // if (rat_injector_enable

        // Select clct pattern trigger
        adr     = seq_trig_en_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFF00;
        wr_data = wr_data | 0x0001;
        //  if (!layer_mode) wr_data=wr_data |0x0001;   //  TURN OFF PATTERN TRIGGER IN LAYER MODE TEMPORARY!!!
        status  = vme_write(adr,wr_data);

        // Set start_trigger state for FMM
        adr     = base_adr+ccb_cmd_adr;

        ttc_cmd = 3;            // ttc_resync
        wr_data = 0x0003 | (ttc_cmd << 8);
        status  = vme_write(adr,wr_data);
        wr_data = 0x0001;
        status  = vme_write(adr,wr_data);

        ttc_cmd = 6;            // start_trigger
        wr_data = 0x0003 | (ttc_cmd << 8);
        status  = vme_write(adr,wr_data);
        wr_data = 0x0001;
        status  = vme_write(adr,wr_data);

        ttc_cmd = 1;            // bx0
        wr_data = 0x0003 | (ttc_cmd << 8);
        status  = vme_write(adr,wr_data);
        wr_data =0x0001;
        status  = vme_write(adr,wr_data);

        // Force a sync error if requested
        adr     = base_adr+sync_err_ctrl_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & ~(1<<15);       // clear old
        wr_data = rd_data |  (sync_err_force<<15);
        status  = vme_write(adr,wr_data);   // set new

        // Clear sync error if bx0 emulator is turned on
        if (vme_bx0_emu_en==1)
        {
            adr     = base_adr+sync_err_ctrl_adr;
            status  = vme_read(adr,rd_data);
            wr_data = rd_data |  0x1;   // assert clear
            status  = vme_write(adr,wr_data);
            wr_data = rd_data & ~0x1;   // deassert
            status  = vme_write(adr,wr_data);

            status  = vme_read(adr,rd_data);
            sync_err             = (rd_data >>  9) & 0x1; 
            clct_bx0_sync_err    = (rd_data >> 10) & 0x1; 
            alct_ecc_rx_err_ff   = (rd_data >> 11) & 0x1; 
            alct_ecc_tx_err_ff   = (rd_data >> 12) & 0x1; 
            bx0_match_err_ff     = (rd_data >> 13) & 0x1; 
            sync_err_force       = (rd_data >> 15) & 0x1;

            if (sync_err==0 && sync_err_force==1) printf("ERR: TMB failed to force sync_err, sync_err=%1i sync_err_force=%1i\n",sync_err,sync_err_force);
            if (sync_err==1 && vme_bx0_emu_en==1) printf("ERR: TMB failed to clear sync_err, sync_err=%1i vme_bx0_emu_en=%1i\n",sync_err,vme_bx0_emu_en);
        }

        // Loop key and pid modes
        for (iclct=0; iclct<nclcts_inject; ++iclct)
        {
            if (clct_key_inject[0]==-1) loop_keys[iclct]=true;
            else                        loop_keys[iclct]=false;

            if (clct_pid_inject[0]==-1) loop_pids[iclct]=true;
            else                        loop_pids[iclct]=false;
        }

        // CLCT: Key hs range to span in event loop
        iclct=0;

        if (loop_keys[iclct])               // clct0 hits all keys 0 to 159
        {
            ikey_min = 0;
            ikey_max = 159;
        }   
        else                                // hit only 1 key
        {
            ikey_min = clct_key_inject[iclct];
            ikey_max = clct_key_inject[iclct];
        }

        // CLCT: Pattern ID range to span in event loop
        if (loop_pids[iclct])               // do all pattern IDs
        {
            ipid_min = 2;
            ipid_max = 10;
        }               
        else                                // just do 1 pid
        {
            ipid_min = clct_pid_inject[iclct];
            ipid_max = clct_pid_inject[iclct];
        }

        if (loop_pids[iclct]) {             // do all pattern IDs
            if (hit_thresh_pretrig   > 3) printf("hit_thresh_pretrig   too high to trigger edge keys\n");
            if (hit_thresh_postdrift > 3) printf("hit_thresh_postdrift too high to trigger edge keys\n");
        }

        // Loop over trigger events: clct0 keys and clct0 pids
        for (ikeylp=ikey_min; ikeylp<=ikey_max; ++ikeylp)
        {
            for (ipidlp=ipid_min; ipidlp<=ipid_max; ++ipidlp)
            {

                // Clear clct 1/2-strip image for this event
                for (layer=0; layer<=5; ++layer) {
                    for (key=0;   key<=159; ++key  ) {
                        ihs[layer][key]=0;
                    }}

                // Loop over clcts to inject for this event
                ipid  = 0;
                ikey  = 0;
                icfeb = 0;

                for (iclct=0; iclct<nclcts_inject; ++iclct)
                {
                    // Construct current key and pid for this clct
                    ikey = clct_key_inject[iclct];
                    ipid = clct_pid_inject[iclct];
                    clct_blanked[iclct] = false;

                    if (iclct==0 && loop_keys[iclct]) {
                        ipid = ipidlp;
                        ikey = ikeylp;
                    }

                    if (iclct==1) {
                        if (loop_keys[1]) ikey = 159-ikeylp;        // clct1 uses clct0 159-key if clct0 is looping over keys
                        if (loop_pids[1]) ipid = flip_pid(ipidlp);  // clct1 uses clct0 flipped pid if clct0 is looping over pids
                    }

                    clct_key_inject[iclct] = ikey;
                    clct_pid_inject[iclct] = ipid;

                    fprintf(stdout ,"dbg: clct_key_inject[%1i]=%3i\n",iclct,clct_key_inject[iclct]);
                    fprintf(stdout ,"dbg: clct_pid_inject[%1i]=%3X\n",iclct,clct_pid_inject[iclct]);

                    if (iclct==1 && loop_keys[1]) {
                        ikey_sep = abs(clct_key_inject[0]-clct_key_inject[1]);
                        if (ikey_sep<=1) {
                            clct_blanked[1]    = true;
                        }
                        fprintf(stdout ,"dbg: key=%3.3i clct1 key separation ikey_sep=%3i clct_blanked[1]=%c\n",ikeylp,ikey_sep,logical(clct_blanked[1]));
                    }
                    if (clct_blanked[iclct]) continue;  // skip this clct if its blanked 

                    // Insert pattern image for this clcts key and pid + stagger hits 
                    ihitp = 0;
                    icfeb = ikey/32;
                    clct_hit_expect[iclct] = 0;
                    clct_key_expect[iclct] = ikey;
                    clct_pid_expect[iclct] = ipid;

                    for (layer=0; layer<=5;  ++layer) {
                        for (icell=0; icell<=10; ++icell) {

                            key=ikey+icell-5;                               // ikey -5,-4,-3,-2,-1,0,+1,+2,+3,+4,+5 cell hits
                            if (stagger_hs_csc==1 && (layer%2)!=0  ) key++; // Type A stagger odd layers if key is on ly2
                            if (stagger_hs_csc==1 && reverse_hs_csc) key--; // Tybe B shift 1hs beco csc is facing backwards

                            ids     = key/4;                                // Distrip for this key
                            ihs_min = ids*4;                                // First hs on this ds
                            ihs_max = ihs_min+3;                            // Last  hs on this ds
                            ihs_hit = 0;                                    // Number of hs hits on this ds must be 0 or 1, comparators can only encode 1 of 4 hs

                            for (ihs_ds=ihs_min; ihs_ds<=ihs_max; ++ihs_ds){// Loop over the 4 hs on this ds
                                if (ihs_ds>=0 && ihs_ds<=159) {                 // Bugfix 12/13/2010
                                    if (ihs[layer][ihs_ds]==1) ihs_hit++;           // Hs that were already set on this ds
                                }}
                            ihit  = pattern_image[ipid][layer][icell];      // Extract pattern hits

                            if (ihit==1   ) ihitp++;                        // Count primary pattern hits
                            if (ihit==1 && ihs_hit>=1) printf("ERRx: Hs collision at ikey=%3i ly=%1i icell=%2i\n",ikey,layer,icell);    
                            if (ihs_hit>=1) ihit = 0;                       // Do not hit same distrip twice, comparators can only encode 1 of 4 hs
                            if (ihitp>clct_hit_inject[iclct]) ihit = 0;     // Limit clct[n] hs hits to user-set

                            if (key>=0 && key<=159 && ihit==1) {            // Set hs hit
                                ihs[layer][key]=1; 
                                clct_hit_expect[iclct]++;
                            }

                        }   // close icell
                    }   // close layer

                    fprintf(stdout ,"CLCT%1i: Key=%3i Pattern=%2X primary hits=%2i expected hits=%2i\n",iclct,ikey,ipid,ihitp,clct_hit_expect[iclct]);

                    if (ihitp!=6) {
                        printf("CLCT%1i: Key=%3i Pattern=%2X primary hits=%2i expected hits=%2i\n",iclct,ikey,ipid,ihitp,clct_hit_expect[iclct]);
                        pause("clct error in primary hit count, expected 6 hits.");
                    }

                    // Close loops for this event
                }   // close for iclct

                // Display 1/2-strip image
                //img_file=log_file;
                //  img_file=sum_file;

                fprintf(stdout ,"1/2-Strip Image:\n");
                for (layer=0; layer<=5; ++layer) {
                    nstag=((layer+1)%2)*stagger_hs_csc;             // stagger even layers
                    fprintf(stdout ,"%1i: ",layer);             // layer number
                    for (i=1; i<=nstag; ++i) fprintf(stdout ," ");  // insert 1 or 2 spaces for staggering
                    fprintf(stdout ,"|");
                    for (icfebg=0; icfebg<=4; ++icfebg) {           // cfeb groups horizontally
                        for (i=0; i<=31; ++i) fprintf(stdout ,"%1i",ihs[layer][i+32*icfebg]);
                        fprintf(stdout ,"|");}
                    fprintf(stdout ,"\n");
                }

                // Clear triad image
                for (layer=0;    layer    < mxly;    ++layer   ) {
                    for (idistrip=0; idistrip < mxdsabs; ++idistrip) {
                        for (itbin=0;    itbin    < mxtbins;  ++itbin  ) {
                            itriad[itbin][idistrip][layer]=0;
                        }}}

                // Convert key 1/2-strips to triads
                itbin0=0;   // first tbin, default is 0

                for (layer=0; layer<=5; ++layer) {
                    itbin0=triad_1st_tbin[layer];
                    for (key=0; key<=159; ++key) {

                        idistrip=key/4;
                        ihstrip =key%4;

                        if (ihs[layer][key]==1) {
                            itriad[itbin0+0][idistrip][layer] = 1;
                            itriad[itbin0+1][idistrip][layer] = ihstrip/2 & 0x0001;
                            itriad[itbin0+2][idistrip][layer] = ihstrip   & 0x0001;
                        }   // close if ihs
                    }   // close for key
                }   // close for layer

                // Load CLCT injector pattern RAM triads from an image file, format="0  0 |00000000|00000000|00000000|00000000|00000000|"
                if (load_clct_injector_image)
                {
                    ram_file_name = "clct_injector_ram_image.txt";          // Open RAM imagae file
                    fprintf(stdout,"\n\tOpening CLCT injector RAM image file %s\n",ram_file_name.c_str());

                    ram_file      = fopen(ram_file_name.c_str(),"r");
                    if (ram_file==NULL) {pause("Unable to open CLCT injector RAM image file"); return;}

                    fgets(line,81,ram_file);                                // Skip comment lines
                    fgets(line,81,ram_file);
                    fgets(line,81,ram_file);

                    for (itbin=0; itbin<mxtbins; ++itbin)
                    {
                        if  (feof(ram_file)) break;                             // Hit end of file
                        fgets(line,81,ram_file);                                // Get a new line
                        sscanf(line,     "%1i%3i |%8X|%8X|%8X|%8X|%8X|",            &layer,&itbin,&dscfeb[0],&dscfeb[1],&dscfeb[2],&dscfeb[3],&dscfeb[4]);
                        fprintf(stdout ,"%1i%3i |%8.8X|%8.8X|%8.8X|%8.8X|%8.8X|\n", layer, itbin, dscfeb[0], dscfeb[1], dscfeb[2], dscfeb[3], dscfeb[4]);

                        if (layer<0 || layer>5       ) stop("layer out of range in clct injector image file");
                        if (itbin<0 || itbin>=mxtbins) stop("itbin out of range in clct injector image file");

                        for (icfeblp =0; icfeblp <=4;   ++icfeblp) {
                            for (idistrip=0; idistrip<=39; ++idistrip) {
                                icfeblp = idistrip/8;
                                ids     = idistrip%8;
                                ibit    = (dscfeb[icfeblp] >> 4*(7-ids)) & 0x1;
                                itriad[itbin][idistrip][layer] = ibit;
                            }}

                    }   // close for itbin
                    fclose(ram_file);
                }   // close if load

                // Check for multiple hstrip hits that could not be conveted to triads, cfeb hardware can only fire one hs per ds
                for (layer=0; layer<=5; ++layer) {
                    for (key=0; key<=159; key=key+4) {          // distrip steps
                        nhits=0;
                        for (i=0; i<=3; ++i) {                      // count hs bits hit
                            nhits=nhits+ihs[layer][key+i];
                        }
                        if (nhits>1) {
                            printf("Multi triad hits=%1i at ly=%1i key=%1i unable to encode all hs hits\n",nhits,layer,key);
                            pause (" "); }
                    }
                }

                // Display Triads
                fprintf(stdout ,"\nbegin triad for key%3.3i",ikeylp); 
                for(i=0;i<nclcts_inject;++i) fprintf(stdout ,"  clct%1i: key%3.3i hit%1i pid%1X",i,clct_key_expect[i],clct_hit_expect[i],clct_pid_expect[i]); 
                fprintf(stdout ,"\n");

                for (layer=0; layer<=5; ++layer) {
                    for (itbin=0; itbin<=2; ++itbin) {
                        for (icfebg  =0; icfebg  <=4; ++icfebg  ) {
                            for (idistrip=0; idistrip<=7; ++idistrip) {
                                fprintf(stdout,"%1i",itriad[itbin][idistrip+8*icfebg][layer]); 
                            } //close idistrip
                            fprintf(stdout ," ");
                        } //close icfebg
                        fprintf(stdout ,"\n");
                    } //close itbin
                    fprintf(stdout ,"\n");
                } //close layer

                // Pack triads into pattern RAM
                wr_data=0;

                for (layer=0; layer<=5; layer=layer+2) {
                    iram=layer/2;
                    for (itbin=0; itbin<=31; ++itbin) {
                        for (idistrip=0; idistrip<=39; ++idistrip) {
                            icfeblp=idistrip/8;
                            idslocal=idistrip%8;
                            if (idslocal==0) wr_data=0;         // clear for each cfeb

                            ibit=itriad[itbin][idistrip][layer];
                            wr_data=wr_data | (ibit << idslocal);

                            ibit=itriad[itbin][idistrip][layer+1];
                            wr_data=wr_data | (ibit << (idslocal+8));

                            pat_ram[itbin][iram][icfeblp]=wr_data;
                            fprintf(stdout,"pat_ram tbin=%2i ram=%1i wr_data=%4.4X\n",itbin,iram,wr_data);
                        }
                    }
                }

                // Write muon data to Injector
                adr     = l1a_lookback_adr+base_adr;
                status  = vme_read(adr,rd_data);                // Read lookback register
                l1a_lookback_data = rd_data & ~(0x3 << 11);     // Clear out injector RAM data [17:16] in [12:11]

                for (icfeblp=0; icfeblp<=4;  ++icfeblp) {
                    for (iram   =0; iram   <=2;  ++iram   ) {
                        for (itbin  =0; itbin  <=31; ++itbin  ) {

                            wr_data_mem = pat_ram[itbin][iram][icfeblp];    // RAM data to write at this cfeb and tbin
                            wr_data_lsb = (wr_data_mem >>  0) & 0x0FFFF;    // RAM write data [15:0]
                            wr_data_msb = (wr_data_mem >> 16) & 0x3;        // RAM write data [17:16];
                            wadr        = itbin;

                            adr    = cfeb_inj_adr+base_adr;                 // Select injector RAM
                            status = vme_read(adr,rd_data);                 // Get current data

                            wr_data = rd_data & 0xFC1F;                     // Zero   CFEB select
                            febsel  = (1 << icfeblp);                       // Select CFEB
                            wr_data = wr_data | (febsel << 5) | 0x7C00;     // Set febsel, enable injectors
                            status  = vme_write(adr,wr_data);               // Select CFEB

                            adr     = cfeb_inj_adr_adr+base_adr;
                            ren     = 0;
                            wen     = 0;
                            wr_data = wen | (ren << 3) | (wadr << 6);
                            status  = vme_write(adr,wr_data);               // Set RAM Address + No write

                            adr     = cfeb_inj_wdata_adr+base_adr;
                            wr_data = wr_data_lsb;
                            status  = vme_write(adr,wr_data);               // Store RAM Data lsb [15:0]

                            adr     = l1a_lookback_adr+base_adr;
                            wr_data = l1a_lookback_data | (wr_data_msb << 11);
                            status  = vme_write(adr,wr_data);               // Store RAM Data msb [17:16]

                            adr     = cfeb_inj_adr_adr+base_adr;
                            wen     = (1 << iram);
                            wr_data = wen | (ren << 3) | (wadr << 6);
                            status  = vme_write(adr,wr_data);               // Set RAM Address + Assert write

                            wen     = 0;
                            wr_data = wen | (ren << 3) | (wadr << 6);
                            status  = vme_write(adr,wr_data);               // Set RAM Address + No write

                            ren     = (1 << iram);
                            wr_data = wen | (ren << 3) | (wadr << 6);
                            status  = vme_write(adr,wr_data);               // Set RAM Address + Read enable

                            adr     = cfeb_inj_rdata_adr+base_adr;
                            status  = vme_read(adr,rd_data);                // Read RAM data lsbs [15:0]
                            rd_data_lsb = rd_data;

                            adr     = l1a_lookback_adr+base_adr;
                            status  = vme_read(adr,rd_data);                // Read RAM data msbs [17:16]
                            rd_data_msb = (rd_data >> 13) & 0x3;
                            rd_data_mem = rd_data_lsb | (rd_data_msb << 16); 

                            if (cprr && !cprr_ignore && (rd_data_mem != wr_data_mem)){
                                printf("\tInjector Verify Err: cfeb%1i key%3i RAM%2i Tbin%2i wr=%5.5X rd=%5.5X\n",icfeblp,ikey,iram,itbin,wr_data_mem,rd_data_mem);
                                printf("\tSkip, Continue <cr> ");
                                fgets(line, 80, stdin);
                                n=strlen(line);
                                sscanf(line,"%d",&i);
                                if (n==1 && (i=='S' || i=='s')) cprr_ignore=true;
                            }

                        }   // close itbin
                    }   // close iram
                }   // close icfebl

                // Set ALCT first muon to inject:
                if (nalcts_inject >= 1) {
                    alct0_vpf_inj   = 1;            //  Valid pattern flag
                    alct0_qual_inj  = 3;            //  Pattern quality
                    alct0_amu_inj   = 0;            //  Accelerator muon
                    alct0_key_inj   = (ikey+5)%128; //  Wire group ID number (just some offset wrt clct key for now)
                    alct0_bxn_inj   = 1;            //  Bunch crossing number
                }
                else {                          //  No 1st alct muon
                    alct0_vpf_inj   = 0;            //  Valid pattern flag
                    alct0_qual_inj  = 0;            //  Pattern quality
                    alct0_amu_inj   = 0;            //  Accelerator muon
                    alct0_key_inj   = 0;            //  Wire group ID number (just some offset wrt clct key for now)
                    alct0_bxn_inj   = 0;            //  Bunch crossing number
                }

                alct0_inj_wr    = (alct0_vpf_inj  <<  0);
                alct0_inj_wr    = (alct0_qual_inj <<  1) | alct0_inj_wr;
                alct0_inj_wr    = (alct0_amu_inj  <<  3) | alct0_inj_wr;
                alct0_inj_wr    = (alct0_key_inj  <<  4) | alct0_inj_wr;
                alct0_inj_wr    = (alct0_bxn_inj  << 11) | alct0_inj_wr;

                wr_data = alct0_inj_wr;
                adr     = alct0_inj_adr+base_adr;
                status  = vme_write(adr,wr_data);

                fprintf(stdout ,"alct0_inj_wr=%4.4X\n",alct0_inj_wr);

                // Set ALCT second muon to inject:
                if (nalcts_inject == 2) {
                    alct1_vpf_inj   = 1;            //  Valid pattern flag
                    alct1_qual_inj  = 2;            //  Pattern quality
                    alct1_amu_inj   = 0;            //  Accelerator muon
                    alct1_key_inj   = (ikey+9)%128; //  Wire group ID number (just some offset wrt clct key for now)
                    alct1_bxn_inj   = 3;            //  Bunch crossing number
                }
                else {                      //  No 2nd alct muon
                    alct1_vpf_inj   = 0;            //  Valid pattern flag
                    alct1_qual_inj  = 0;            //  Pattern quality
                    alct1_amu_inj   = 0;            //  Accelerator muon
                    alct1_key_inj   = 0;            //  Wire group ID number
                    alct1_bxn_inj   = 0;            //  Bunch crossing number
                }

                alct1_inj_wr    = (alct1_vpf_inj  <<  0);
                alct1_inj_wr    = (alct1_qual_inj <<  1) | alct1_inj_wr;
                alct1_inj_wr    = (alct1_amu_inj  <<  3) | alct1_inj_wr;
                alct1_inj_wr    = (alct1_key_inj  <<  4) | alct1_inj_wr;
                alct1_inj_wr    = (alct1_bxn_inj  << 11) | alct1_inj_wr;

                wr_data = alct1_inj_wr; 
                adr     = alct1_inj_adr+base_adr;
                status  = vme_write(adr,wr_data);

                fprintf(stdout ,"alct1_inj_wr=%4.4X\n",alct1_inj_wr);

                // Lower pattern threshold temporarily so edge key 1/2-strips will trigger, set it back later
                if (loop_keys[0] && (ikey<=4 || ikey>=154))
                {
                    hit_thresh_pretrig_temp   = clct_hit_expect[0];
                    hit_thresh_postdrift_temp = clct_hit_expect[0];

                    if (nclcts_inject==2) {
                        hit_thresh_pretrig_temp   = min(clct_hit_expect[0],clct_hit_expect[1]);
                        hit_thresh_postdrift_temp = min(clct_hit_expect[0],clct_hit_expect[1]);}
                }
                else
                {
                    hit_thresh_pretrig_temp   = hit_thresh_pretrig;
                    hit_thresh_postdrift_temp = hit_thresh_postdrift;
                }

                adr    = seq_clct_adr+base_adr;
                status = vme_read(adr,rd_data);

                wr_data = rd_data & 0x8000; // clear hit_thresh,nph_pattern,drift
                wr_data = wr_data | (triad_persist             <<  0);
                wr_data = wr_data | (hit_thresh_pretrig_temp   <<  4);
                wr_data = wr_data | (dmb_thresh_pretrig        <<  7);
                wr_data = wr_data | (hit_thresh_postdrift_temp << 10);
                wr_data = wr_data | (drift_delay               << 13);
                status  = vme_write(adr,wr_data);

                // Clear previous event aff, clct, mpc registers
                adr     = seqmod_adr+base_adr;
                status  = vme_read(adr,rd_data);
                wr_data = rd_data |  (1<<15);       // clear   = set bit 15
                status  = vme_write(adr,wr_data);
                wr_data = rd_data & ~(1<<15);       // unclear = clr bit 15
                status  = vme_write(adr,wr_data);

                // Set scope-in-readout
                adr     = base_adr+scp_ctrl_adr;
                status  = vme_read(adr,rd_data);
                wr_data = rd_data & 0xFF07;  // clear bits 3,4,5,6,7
                wr_data = wr_data | (scp_auto    << 3);
                wr_data = wr_data | (scp_nowrite << 4);
                wr_data = wr_data | (scp_tbins   << 5);
                status  = vme_write(adr,wr_data);

                // Arm scope trigger
                scp_arm        = true;
                scp_readout    = false;
                scp_raw_decode = false;
                scp_silent     = false;
                scp_playback   = false;
                if (rdscope)
                    scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

                // Fire CLCT+ALCT Injectors
                adr     = cfeb_inj_adr+base_adr;
                status  = vme_read(adr,rd_data);
                wr_data = rd_data | 0x8000;     // fire injector
                status  = vme_write(adr,wr_data);
                wr_data = rd_data & 0x7FFF;     // unfire
                status  = vme_write(adr,wr_data);

                // Wait for injector to complete
                for (i=0; i<=10; ++i) {
                    adr      = cfeb_inj_adr+base_adr;
                    status   = vme_read(adr,rd_data);
                    inj_busy = (rd_data >> 15) & 0x1;
                    if (inj_busy==0) break;
                    if (i==10) pause("CFEB injector RAM failed to go unbusy after 10 VME reads");
                }

                // CLCTemu: Predict expected clct pattern-finder results for these 1/2-strip hits
                engage_pattern_finder=true;

                if (engage_pattern_finder)
                {
                    pattern_finder
                        (
                         ihs,               // inputs

                         csc_type, 
                         clct_sep, 
                         adjcfeb_dist,
                         layer_trig_en,
                         cfeb_en,

                         hit_thresh_pretrig,
                         pid_thresh_pretrig,
                         dmb_thresh_pretrig,
                         lyr_thresh_pretrig,

                         cfeb_active_emu,   // outputs
                         nlayers_hit_emu,
                         layer_trig_emu,

                         hs_key_1st_emu,    
                         hs_pid_1st_emu,
                         hs_hit_1st_emu,

                         hs_key_2nd_emu,
                         hs_pid_2nd_emu,
                         hs_hit_2nd_emu
                             );
                }
                else
                {
                    hs_key_1st_emu = 0;
                    hs_pid_1st_emu = 0;
                    hs_hit_1st_emu = 0;

                    hs_key_2nd_emu = 0;
                    hs_pid_2nd_emu = 0;
                    hs_hit_2nd_emu = 0;

                    nlayers_hit_emu = 0;
                    layer_trig_emu = 0;

                    cfeb_active_emu[0]=0;
                    cfeb_active_emu[1]=0;
                    cfeb_active_emu[2]=0;
                    cfeb_active_emu[3]=0;
                    cfeb_active_emu[4]=0;
                }

                fprintf(stdout ,"\n");
                fprintf(stdout ,"CLCTemu hs_key_1st_emu=%3i hs_pid_1st_emu =%1X hs_hit_1st_emu=%1i\n",hs_key_1st_emu, hs_pid_1st_emu, hs_hit_1st_emu);
                fprintf(stdout ,"CLCTemu hs_key_2nd_emu=%3i hs_pid_2nd_emu =%1X hs_hit_2nd_emu=%1i\n",hs_key_2nd_emu, hs_pid_2nd_emu, hs_hit_2nd_emu);
                fprintf(stdout ,"CLCTemu layer_trig_emu=%3i nlayers_hit_emu=%1i\n",layer_trig_emu, nlayers_hit_emu);
                fprintf(stdout ,"CLCTemu cfeb_active_emu[4:0]=");
                for(i=4;i>=0;--i)
                    fprintf(stdout ,"%1i",cfeb_active_emu[i]); 
                fprintf(stdout ,"\n");

                // CLCTemu: Latch pattern finder emulator results
                emulator_pretrig     = (hs_hit_1st_emu>=hit_thresh_pretrig_temp   && hs_pid_1st_emu>=pid_thresh_pretrig  );
                emulator_latch_clct0 = (hs_hit_1st_emu>=hit_thresh_postdrift_temp && hs_pid_1st_emu>=pid_thresh_postdrift) && emulator_pretrig;
                emulator_latch_clct1 = (hs_hit_2nd_emu>=hit_thresh_postdrift_temp && hs_pid_2nd_emu>=pid_thresh_postdrift) && emulator_latch_clct0;

                if (emulator_latch_clct0) {
                    clct0_vpf_emu   = 1;                        //  Valid pattern flag
                    clct0_hit_emu   = hs_hit_1st_emu;           //  Hits on pattern
                    clct0_pid_emu   = hs_pid_1st_emu;           //  Pattern number
                    clct0_key_emu   = hs_key_1st_emu;           //  1/2-strip ID number
                    clct0_cfeb_emu  = hs_key_1st_emu>>5;        //  CFEB ID
                    clctc_sync_emu  = ~vme_bx0_emu_en & 0x1;}   //  Sync error
                else {
                    clct0_vpf_emu   = 0;
                    clct0_hit_emu   = 0;
                    clct0_pid_emu   = 0;
                    clct0_key_emu   = 0;
                    clct0_cfeb_emu  = 0;
                    clctc_sync_emu  = 0;}

                if (emulator_latch_clct1) {
                    clct1_vpf_emu   = 1;                        //  Valid pattern flag
                    clct1_hit_emu   = hs_hit_2nd_emu;           //  Hits on pattern
                    clct1_pid_emu   = hs_pid_2nd_emu;           //  Pattern number
                    clct1_key_emu   = hs_key_2nd_emu;           //  1/2-strip ID number
                    clct1_cfeb_emu  = hs_key_2nd_emu>>5;}       //  CFEB ID
                else {
                    clct1_vpf_emu   = 0;
                    clct1_hit_emu   = 0;
                    clct1_pid_emu   = 0;
                    clct1_key_emu   = 0;
                    clct1_cfeb_emu  = 0;}

                // CLCTvme: Get VME latched CLCT words
                adr    = seq_clct0_adr+base_adr;
                status = vme_read(adr,rd_data);
                clct0_vme = rd_data;

                adr    = seq_clct1_adr+base_adr;
                status = vme_read(adr,rd_data);
                clct1_vme = rd_data;

                adr    = seq_clctm_adr+base_adr;
                status = vme_read(adr,rd_data);
                clctc_vme = (rd_data >> 0) & 0x0007;

                fprintf(stdout,"clct0_vme=%6.6X\n",clct0_vme);
                fprintf(stdout,"clct1_vme=%6.6X\n",clct1_vme);
                fprintf(stdout,"clctm_vme=%6.6X\n",rd_data);

                // CLCTvme: Get VME clct bxn stored at pretrigger
                adr    = bxn_clct_adr+base_adr;
                status = vme_read(adr,rd_data);
                clct_bxn_expect = rd_data & 0x3;
                fprintf(stdout,"CLCT pretrigger bxn=%4.4Xh truncated=%4.4Xh\n",rd_data,clct_bxn_expect);

                // CLCTvme: Get VME  number of layers hit
                adr    = layer_trig_adr+base_adr;
                status = vme_read(adr,rd_data);
                nlayers_hit = (rd_data >> 4) & 0x7;

                fprintf(stdout,"nlayers_hit=%1i\n",nlayers_hit);

                // CLCTvme: Get VME active CFEB list
                adr    = seq_clctm_adr+base_adr;
                status = vme_read(adr,rd_data);
                active_feb_list = (rd_data >> 3) & 0x1F;

                // CLCTvme: Decompose VME CLCTs
                clct0_vpf_vme   = (clct0_vme >>  0) & 0x0001;   //  Valid pattern flag
                clct0_hit_vme   = (clct0_vme >>  1) & 0x0007;   //  Hits on pattern
                clct0_pid_vme   = (clct0_vme >>  4) & 0x000F;   //  Pattern number
                clct0_key_vme   = (clct0_vme >>  8) & 0x00FF;   //  1/2-strip ID number
                clct0_cfeb_vme  = (clct0_vme >> 13) & 0x0007;   //  CFEB ID

                clct1_vpf_vme   = (clct1_vme >>  0) & 0x0001;   //  Valid pattern flag
                clct1_hit_vme   = (clct1_vme >>  1) & 0x0007;   //  Hits on pattern
                clct1_pid_vme   = (clct1_vme >>  4) & 0x000F;   //  Pattern number
                clct1_key_vme   = (clct1_vme >>  8) & 0x00FF;   //  1/2-strip ID number
                clct1_cfeb_vme  = (clct1_vme >> 13) & 0x0007;   //  CFEB ID

                clctc_bxn_vme   = (clctc_vme >>  0) & 0x0003;   //  Bunch crossing number
                clctc_sync_vme  = (clctc_vme >>  2) & 0x0001;   //  Sync error

                // CLCTemu+vme: Compare VME CLCTs to emulator CLCTs
                ck("clct0_vpf  read vme.expect emulator", clct0_vpf_vme,    clct0_vpf_emu );
                ck("clct0_hit  read vme.expect emulator", clct0_hit_vme,    clct0_hit_emu );
                ck("clct0_pid  read vme.expect emulator", clct0_pid_vme,    clct0_pid_emu );
                ck("clct0_key  read vme.expect emulator", clct0_key_vme,    clct0_key_emu );
                ck("clct0_cfeb read vme.expect emulator", clct0_cfeb_vme,   clct0_cfeb_emu);

                ck("clct1_vpf  read vme.expect emulator", clct1_vpf_vme,    clct1_vpf_emu );
                ck("clct1_hit  read vme.expect emulator", clct1_hit_vme,    clct1_hit_emu );
                ck("clct1_pid  read vme.expect emulator", clct1_pid_vme,    clct1_pid_emu );
                ck("clct1_key  read vme.expect emulator", clct1_key_vme,    clct1_key_emu );
                ck("clct1_cfeb read vme.expect emulator", clct1_cfeb_vme,   clct1_cfeb_emu);

                ck("clctc_sync read vme.expect emulator", clctc_sync_vme,   clctc_sync_emu);

                // CLCTinj: Predict number of hits on injected CLCTs due to staggering losses at CSC edges
                for (i=0; i<nclcts_inject; ++i)
                {
                    clct_hit_inject_clip[i]=clct_hit_inject[i];         // no edge clipping

                    if ((csc_type==0xA && clct_key_inject[i]==159) ||   // Normal   CSC
                            (csc_type==0xB && clct_key_inject[i]==0  ))     // Reversed CSC
                    {
                        if (clct_hit_inject[i]==6) clct_hit_inject_clip[i]=clct_hit_inject[i]-3;    // hit ly0,1,2,3,4,5 clipped ly1,3,5    
                        if (clct_hit_inject[i]==5) clct_hit_inject_clip[i]=clct_hit_inject[i]-2;    // hit ly0,1,2,3,4   clipped ly1,3  
                        if (clct_hit_inject[i]==4) clct_hit_inject_clip[i]=clct_hit_inject[i]-2;    // hit ly0,1,2,3     clipped ly1,3  
                        if (clct_hit_inject[i]==3) clct_hit_inject_clip[i]=clct_hit_inject[i]-1;    // hit ly0,1,2       clipped ly1    
                        if (clct_hit_inject[i]==2) clct_hit_inject_clip[i]=clct_hit_inject[i]-1;    // hit ly0,1         clipped ly1    
                        if (clct_hit_inject[i]==1) clct_hit_inject_clip[i]=clct_hit_inject[i]-0;    // hit ly0           clipped none   
                        fprintf(stdout ,"clct_hit_inject[%1i] clipped from %1i hits to %1i hits at csc edge due to staggering\n",i,clct_hit_inject[i],clct_hit_inject_clip[i]);
                    }
                    fprintf(stdout ,"clct_hit_inject[%1i]=%1i clct_hit_inject_clip[%1i]=%1i\n",i,clct_hit_inject[i],i,clct_hit_inject_clip[i]);
                }   // close for i

                // CLCTinj: Find expected clct0 from the injector
                clct_hit_inj_expect[0] = 0;
                clct_key_inj_expect[0] = 0;
                clct_pid_inj_expect[0] = 0;
                clct_pat_inj_expect[0] = 0;

                for (i=0; i<nclcts_inject; ++i) {
                    clct_pat_inject[i] = (clct_hit_inject_clip[i]<<4) | clct_pid_inject[i];

                    if ((clct_pat_inject[i] >  clct_pat_inj_expect[0]) ||       // found a better pattern
                            ((clct_pat_inject[i] == clct_pat_inj_expect[0]) &&      // found an equal pattern 
                             (clct_key_inject[i] <  clct_key_inj_expect[0])))       // take the equal pattern at the lower key hs
                    {
                        clct_hit_inj_expect[0] = clct_hit_inject_clip[i];
                        clct_key_inj_expect[0] = clct_key_inject[i];
                        clct_pid_inj_expect[0] = clct_pid_inject[i];
                        clct_pat_inj_expect[0] = clct_pat_inject[i];
                    }
                }

                fprintf(stdout ,"clct_hit_inj_expect[0]=%3i\n",clct_hit_inj_expect[0]);
                fprintf(stdout ,"clct_key_inj_expect[0]=%3i\n",clct_key_inj_expect[0]);
                fprintf(stdout ,"clct_pid_inj_expect[0]=%3X\n",clct_pid_inj_expect[0]);
                fprintf(stdout ,"clct_pat_inj_expect[0]=%3X\n",clct_pat_inj_expect[0]);

                // CLCTinj: Create key 1/2-strip blanking region around clct0 from injector
                nspan = clct_sep;
                pspan = clct_sep;

                key_inj = clct_key_inj_expect[0];
                clct0_is_on_me1a = (key_inj>=128);

                if (csc_type==0xA || csc_type==0xB)     // CSC Type A or B limit busy list to range 0-159
                {
                    busy_max = (key_inj <= 159-pspan) ? key_inj+pspan : 159;    // Limit busy list to range 0-159
                    busy_min = (key_inj >= nspan    ) ? key_inj-nspan : 0;
                }

                else if (csc_type==0xC || csc_type==0xD)                    // CSC Type C or D delimiters for excluding 2nd clct span ME1B hs0-127  ME1A hs128-159
                {
                    if (clct0_is_on_me1a) {     // CLCT0 is on ME1A cfeb4, limit blanking region to 128-159
                        busy_max = (key_inj <= 159-pspan) ? key_inj+pspan : 159;
                        busy_min = (key_inj >= 128+nspan) ? key_inj-nspan : 128;
                    }
                    else {                      // CLCT0 is on ME1B cfeb0-cfeb3, limit blanking region to 0-127
                        busy_max = (key_inj <= 127-pspan) ? key_inj+pspan : 127;
                        busy_min = (key_inj >=     nspan) ? key_inj-nspan : 0;
                    }
                }

                for (i=0; i<160; ++i) {
                    busy_key_inj[i] = (i>=busy_min && i<=busy_max);
                }

                fprintf(stdout ,"busy_key_inj="); 
                for (i=0; i<160; ++i) 
                    fprintf(stdout,"%1i",busy_key_inj[i]); 
                fprintf(stdout,"\n");

                // CLCTinj: Find expected clct1 from the injector
                clct_hit_inj_expect[1] = 0;
                clct_key_inj_expect[1] = 0;
                clct_pid_inj_expect[1] = 0;
                clct_pat_inj_expect[1] = 0;

                for (i=0; i<nclcts_inject; ++i) {
                    if (nclcts_inject<=1) break;
                    if (clct_blanked[i] ) continue;

                    if ((clct_pat_inject[i] >  clct_pat_inj_expect[1]) ||       // found a better pattern
                            ((clct_pat_inject[i] == clct_pat_inj_expect[1]) &&      // found an equal pattern 
                             (clct_key_inject[i] <  clct_key_inj_expect[1])))       // take the equal pattern at the lower key hs
                    {
                        key_inj = clct_key_inject[i];
                        fprintf(stdout,"dbg: key_inj=%3i\n",key_inj);
                        fprintf(stdout,"dbg: busy_key[key_inj]=%3i\n",busy_key_inj[key_inj]);

                        if (busy_key_inj[key_inj]==0)                           // accept only non-busy keys far enough away from clct0
                        {
                            clct_hit_inj_expect[1] = clct_hit_inject_clip[i];
                            clct_key_inj_expect[1] = clct_key_inject[i];
                            clct_pid_inj_expect[1] = clct_pid_inject[i];
                            clct_pat_inj_expect[1] = clct_pat_inject[i];
                        }
                    }
                }

                fprintf(stdout,"clct_hit_inj_expect[1]=%3i\n",clct_hit_inj_expect[1]);
                fprintf(stdout,"clct_key_inj_expect[1]=%3i\n",clct_key_inj_expect[1]);
                fprintf(stdout,"clct_pid_inj_expect[1]=%3X\n",clct_pid_inj_expect[1]);
                fprintf(stdout,"clct_pat_inj_expect[1]=%3X\n",clct_pat_inj_expect[1]);

                // CLCTinj: Predict pre-trigger and post-drift behavior for injected CLCTs
                injector_clct0_over  = false;   // clct0 over thresholds
                injector_clct1_over  = false;   // clct1 over thresholds
                injector_pretrig     = false;   // tmb should pretrigger
                injector_latch_clct0 = false;   // tmb should latch clct0
                injector_latch_clct1 = false;   // tmb should latch clct1

                if (nclcts_inject>=1)
                {
                    injector_clct0_over  = (clct_hit_inj_expect[0]>=hit_thresh_pretrig_temp   && clct_pid_inj_expect[0]>=pid_thresh_pretrig);
                    injector_clct1_over  = (clct_hit_inj_expect[1]>=hit_thresh_pretrig_temp   && clct_pid_inj_expect[1]>=pid_thresh_pretrig);
                    injector_pretrig     = injector_clct0_over;
                    injector_latch_clct0 = injector_pretrig && injector_clct0_over;
                    injector_latch_clct1 = injector_pretrig && injector_clct1_over;
                }

                fprintf(stdout,"injector_clct0_over =%c\n",logical(injector_clct0_over ));
                fprintf(stdout,"injector_clct1_over =%c\n",logical(injector_clct1_over ));
                fprintf(stdout,"injector_pretrig    =%c\n",logical(injector_pretrig    ));
                fprintf(stdout,"injector_latch_clct0=%c\n",logical(injector_latch_clct0));
                fprintf(stdout,"injector_latch_clct1=%c\n",logical(injector_latch_clct1));

                // CLCTinj: Latch pattern finder emulator results
                clct0_vpf_vme_expect = 0;
                clct0_hit_vme_expect = 0;
                clct0_pid_vme_expect = 0;
                clct0_key_vme_expect = 0;
                clct0_cfeb_vme_expect= 0;
                clctc_sync_vme_expect= 0;

                clct1_vpf_vme_expect = 0;
                clct1_hit_vme_expect = 0;
                clct1_pid_vme_expect = 0;
                clct1_key_vme_expect = 0;
                clct1_cfeb_vme_expect= 0;

                if (injector_latch_clct0) {
                    clct0_vpf_vme_expect = 1;
                    clct0_hit_vme_expect = clct_hit_inj_expect[0];
                    clct0_pid_vme_expect = clct_pid_inj_expect[0];
                    clct0_key_vme_expect = clct_key_inj_expect[0];
                    clct0_cfeb_vme_expect= clct_key_inj_expect[0]/32;
                    clctc_sync_vme_expect= ~vme_bx0_emu_en & 0x1;
                }

                if (injector_latch_clct1) {
                    clct1_vpf_vme_expect = 1;
                    clct1_hit_vme_expect = clct_hit_inj_expect[1];
                    clct1_pid_vme_expect = clct_pid_inj_expect[1];
                    clct1_key_vme_expect = clct_key_inj_expect[1];
                    clct1_cfeb_vme_expect= clct_key_inj_expect[1]/32;
                }

                // CLCTinj: Construct expected VME CLCTs from injected CLCTs
                key0 = clct0_key_vme_expect;
                key1 = clct1_key_vme_expect;

                if (injector_latch_clct0) {
                    if (key0 <= 127 && reverse_hs_me1b == 1) {clct0_key_vme_expect  = 127- key0;        clct0_pid_vme_expect=flip_pid(clct0_pid_vme_expect);}
                    if (key0 >= 128 && reverse_hs_me1a == 1) {clct0_key_vme_expect  = 159-(key0-128);   clct0_pid_vme_expect=flip_pid(clct0_pid_vme_expect);}
                    if (reverse_hs_csc==1)                   {clct0_key_vme_expect  = 159- key0;        clct0_pid_vme_expect=flip_pid(clct0_pid_vme_expect);}
                    clct0_cfeb_vme_expect = clct0_key_vme_expect/32;
                }

                if (injector_latch_clct1) {
                    if (key1 <= 127 && reverse_hs_me1b == 1) {clct1_key_vme_expect  = 127- key1;        clct1_pid_vme_expect=flip_pid(clct1_pid_vme_expect);}
                    if (key1 >= 128 && reverse_hs_me1a == 1) {clct1_key_vme_expect  = 159-(key1-128);   clct1_pid_vme_expect=flip_pid(clct1_pid_vme_expect);}
                    if (reverse_hs_csc==1)                   {clct1_key_vme_expect  = 159- key1;        clct1_pid_vme_expect=flip_pid(clct1_pid_vme_expect);}
                    clct1_cfeb_vme_expect = clct1_key_vme_expect/32;
                }

                // CLCTinj: Layer mode expects pattern 1 in 1st clct and no 2nd clct
                if (layer_mode)
                {
                    clct0_vpf_vme_expect = 1;
                    clct0_hit_vme_expect = clct0_hit_vme_expect;
                    clct0_pid_vme_expect = 1;
                    clct0_key_vme_expect = 0;
                    clct0_cfeb_vme_expect= clct0_key_vme_expect/32;

                    clct1_vpf_vme_expect = 0;
                    clct1_hit_vme_expect = 0;
                    clct1_pid_vme_expect = 0;
                    clct1_key_vme_expect = 0;
                    clct1_cfeb_vme_expect= 0;
                }

                // CLCTinj+vme: Compare VME CLCTs to expected VME CLCTs from injector
                ck("clct0_vpf  read vme.expect injected", clct0_vpf_vme,    clct0_vpf_vme_expect );
                ck("clct0_hit  read vme.expect injected", clct0_hit_vme,    clct0_hit_vme_expect );
                ck("clct0_pid  read vme.expect injected", clct0_pid_vme,    clct0_pid_vme_expect );
                ck("clct0_key  read vme.expect injected", clct0_key_vme,    clct0_key_vme_expect );
                ck("clct0_cfeb read vme.expect injected", clct0_cfeb_vme,   clct0_cfeb_vme_expect);
                ck("clctc_bxn  read vme.expect injected", clctc_bxn_vme,    clct_bxn_expect      );

                ck("clct1_vpf  read vme.expect injected", clct1_vpf_vme,    clct1_vpf_vme_expect );
                ck("clct1_hit  read vme.expect injected", clct1_hit_vme,    clct1_hit_vme_expect );
                ck("clct1_pid  read vme.expect injected", clct1_pid_vme,    clct1_pid_vme_expect );
                ck("clct1_key  read vme.expect injected", clct1_key_vme,    clct1_key_vme_expect );
                ck("clct1_cfeb read vme.expect injected", clct1_cfeb_vme,   clct1_cfeb_vme_expect);

                ck("clctc_sync read vme.expect injected", clctc_sync_vme,   clctc_sync_vme_expect);

                // TMBemu: Take either the CLCT injector or emulator result for the TMB stage
                if (send_emulator_to_tmb) {
                    fprintf(stdout,"CLCT: Sending emulator CLCTs to TMB checking\n");
                    clct0_vpf_tmb  = clct0_vpf_emu;
                    clct0_hit_tmb  = clct0_hit_emu;
                    clct0_pid_tmb  = clct0_pid_emu;
                    clct0_key_tmb  = clct0_key_emu;
                    clct0_cfeb_tmb = clct0_cfeb_emu;

                    clct1_vpf_tmb  = clct1_vpf_emu;
                    clct1_hit_tmb  = clct1_hit_emu;
                    clct1_pid_tmb  = clct1_pid_emu;
                    clct1_key_tmb  = clct1_key_emu;
                    clct1_cfeb_tmb = clct1_cfeb_emu;

                    clctc_bxn_tmb      = clctc_bxn_vme;
                    clctc_sync_err_tmb = ~vme_bx0_emu_en & 0x1;
                }
                else {
                    fprintf(stdout,"CLCT: Sending injector CLCTs to TMB checking\n");
                    clct0_vpf_tmb  = clct0_vpf_vme_expect;
                    clct0_hit_tmb  = clct0_hit_vme_expect;
                    clct0_pid_tmb  = clct0_pid_vme_expect;
                    clct0_key_tmb  = clct0_key_vme_expect;
                    clct0_cfeb_tmb = clct0_cfeb_vme_expect;

                    clct1_vpf_tmb  = clct1_vpf_vme_expect;
                    clct1_hit_tmb  = clct1_hit_vme_expect;
                    clct1_pid_tmb  = clct1_pid_vme_expect;
                    clct1_key_tmb  = clct1_key_vme_expect;
                    clct1_cfeb_tmb = clct1_cfeb_vme_expect;

                    clctc_bxn_tmb      = clct_bxn_expect;
                    clctc_sync_err_tmb = ~vme_bx0_emu_en & 0x1;
                }

                // TMBemu: Pseudo pipes for emulator
                clct0_pipe = (clct0_vpf_tmb << 0) |
                    (clct0_hit_tmb << 1) |
                    (clct0_pid_tmb << 4) |
                    (clct0_key_tmb << 8);

                clct1_pipe = (clct1_vpf_tmb << 0) |
                    (clct1_hit_tmb << 1) |
                    (clct1_pid_tmb << 4) |
                    (clct1_key_tmb << 8);

                clctc_pipe = (clctc_bxn_tmb      << 0) |
                    (clctc_sync_err_tmb << 2);

                alct0_pipe = alct0_inj_wr;
                alct1_pipe = alct1_inj_wr;

                // TMBemu: Predict expected clct and alct duplication
                expect_zero_alct = 0;
                expect_zero_clct = 0;
                expect_one_alct  = 0;
                expect_one_clct  = 0;
                expect_two_alct  = 0;
                expect_two_clct  = 0;
                expect_dupe_alct = 0;
                expect_dupe_clct = 0;

                if (clct0_vpf_tmb==0 && clct1_vpf_tmb==0) expect_zero_clct = 1;
                if (clct0_vpf_tmb==1 && clct1_vpf_tmb==0) expect_one_clct  = 1;
                if (clct0_vpf_tmb==1 && clct1_vpf_tmb==1) expect_two_clct  = 1;
                if (clct0_vpf_tmb==0 && clct1_vpf_tmb==1) pause("clct ranking error, wtf!?");

                if (nalcts_inject==0) expect_zero_alct = 1;
                if (nalcts_inject==1) expect_one_alct  = 1;
                if (nalcts_inject==2) expect_two_alct  = 1;

                if (expect_one_clct==1 && nalcts_inject==2) expect_dupe_clct = 1;
                if (expect_two_clct==1 && nalcts_inject==1) expect_dupe_alct = 1;

                fprintf(stdout,"\n");
                fprintf(stdout,"Setting expect_zero_alct = %c\n",logical(expect_zero_alct));
                fprintf(stdout,"Setting expect_zero_clct = %c\n",logical(expect_zero_clct));
                fprintf(stdout,"Setting expect_one_alct  = %c\n",logical(expect_one_alct));
                fprintf(stdout,"Setting expect_one_clct  = %c\n",logical(expect_one_clct));
                fprintf(stdout,"Setting expect_two_alct  = %c\n",logical(expect_two_alct));
                fprintf(stdout,"Setting expect_two_clct  = %c\n",logical(expect_two_clct));
                fprintf(stdout,"Setting expect_dupe_alct = %c\n",logical(expect_dupe_alct));
                fprintf(stdout,"Setting expect_dupe_clct = %c\n",logical(expect_dupe_clct));

                // TMBemu: Event trigger disposition
                clct_noalct_lost= false;
                alct_only_trig  = (expect_zero_clct==1);

                clct_match      = (!expect_zero_clct && !expect_zero_alct);
                clct_noalct     = (!expect_zero_clct &&  expect_zero_alct);
                alct_noclct     = ( expect_zero_clct && !expect_zero_alct);
                alct_pulse      = (!expect_zero_alct);

                clct_keep       =((clct_match && tmb_allow_match   ) || (clct_noalct &&  tmb_allow_clct    && !clct_noalct_lost));
                alct_keep       = (clct_match && tmb_allow_match   ) || (alct_noclct &&  tmb_allow_alct);

                clct_keep_ro    = (clct_match && tmb_allow_match_ro) || (clct_noalct &&  tmb_allow_clct_ro && !clct_noalct_lost);
                alct_keep_ro    = (clct_match && tmb_allow_match_ro) || (alct_noclct &&  tmb_allow_alct_ro);

                clct_discard    = (clct_match && !tmb_allow_match  ) || (clct_noalct && !tmb_allow_clct) || clct_noalct_lost;
                alct_discard    =  alct_pulse && !alct_keep;

                trig_pulse      = clct_match || clct_noalct || clct_noalct_lost || alct_only_trig;  // Event pulse

                trig_keep       = (clct_keep    || alct_keep);                                      // Keep event for trigger and readout
                non_trig_keep   = (clct_keep_ro || alct_keep_ro);                                   // Keep non-triggering event for readout only

                alct_only       = (alct_noclct && tmb_allow_alct) && !clct_keep;                    // An alct-only trigger

                clct_match_tr   = clct_match  && trig_keep;     // ALCT and CLCT matched in time, nontriggering event
                alct_noclct_tr  = alct_noclct && trig_keep;     // Only ALCT triggered, nontriggering event
                clct_noalct_tr  = clct_noalct && trig_keep;     // Only CLCT triggered, nontriggering event

                clct_match_ro   = clct_match  && non_trig_keep; // ALCT and CLCT matched in time, nontriggering event
                alct_noclct_ro  = alct_noclct && non_trig_keep; // Only ALCT triggered, nontriggering event
                clct_noalct_ro  = clct_noalct && non_trig_keep; // Only CLCT triggered, nontriggering event

                alct_only_trig  = (alct_noclct && tmb_allow_alct) || (alct_noclct_ro && tmb_allow_alct_ro);// ALCT-only triggers are allowed

                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: clct_match     = %c\n",logical(clct_match    ));
                fprintf(stdout,"TMBemu: clct_noalct    = %c\n",logical(clct_noalct   ));
                fprintf(stdout,"TMBemu: alct_noclct    = %c\n",logical(alct_noclct   ));
                fprintf(stdout,"TMBemu: alct_pulse     = %c\n",logical(alct_pulse    ));
                fprintf(stdout,"TMBemu: clct_keep      = %c\n",logical(clct_keep     ));
                fprintf(stdout,"TMBemu: alct_keep      = %c\n",logical(alct_keep     ));
                fprintf(stdout,"TMBemu: clct_keep_ro   = %c\n",logical(clct_keep_ro  ));
                fprintf(stdout,"TMBemu: alct_keep_ro   = %c\n",logical(alct_keep_ro  ));
                fprintf(stdout,"TMBemu: clct_discard   = %c\n",logical(clct_discard  ));
                fprintf(stdout,"TMBemu: alct_discard   = %c\n",logical(alct_discard  ));
                fprintf(stdout,"TMBemu: trig_pulse     = %c\n",logical(trig_pulse    ));
                fprintf(stdout,"TMBemu: trig_keep      = %c\n",logical(trig_keep     ));
                fprintf(stdout,"TMBemu: non_trig_keep  = %c\n",logical(non_trig_keep ));
                fprintf(stdout,"TMBemu: alct_only      = %c\n",logical(alct_only     ));
                fprintf(stdout,"TMBemu: clct_match_tr  = %c\n",logical(clct_match_tr ));
                fprintf(stdout,"TMBemu: alct_noclct_tr = %c\n",logical(alct_noclct_tr));
                fprintf(stdout,"TMBemu: clct_noalct_tr = %c\n",logical(clct_noalct_tr));
                fprintf(stdout,"TMBemu: clct_match_ro  = %c\n",logical(clct_match_ro ));
                fprintf(stdout,"TMBemu: alct_noclct_ro = %c\n",logical(alct_noclct_ro));
                fprintf(stdout,"TMBemu: clct_noalct_ro = %c\n",logical(clct_noalct_ro));
                fprintf(stdout,"TMBemu: alct_only_trig = %c\n",logical(alct_only_trig));

                // TMBemu: Latch clct match results for TMB and MPC pathways
                tmb_trig_pulse      = trig_pulse;                           // ALCT or CLCT or both triggered
                tmb_trig_keep_ff    = trig_keep;                            // ALCT or CLCT or both triggered, and trigger is allowed
                tmb_non_trig_keep_ff= non_trig_keep;                        // Event did not trigger but is kept for readout

                tmb_match           = clct_match_tr  && tmb_allow_match;    // ALCT and CLCT matched in time
                tmb_alct_only       = alct_noclct_tr && tmb_allow_alct;     // Only ALCT triggered
                tmb_clct_only       = clct_noalct_tr && tmb_allow_clct;     // Only CLCT triggered

                tmb_match_ro_ff     = clct_match_ro  && tmb_allow_match_ro; // ALCT and CLCT matched in time, nontriggering event
                tmb_alct_only_ro_ff = alct_noclct_ro && tmb_allow_alct_ro;  // Only ALCT triggered, nontriggering event
                tmb_clct_only_ro_ff = clct_noalct_ro && tmb_allow_clct_ro;  // Only CLCT triggered, nontriggering event

                tmb_alct_discard    = alct_discard;                         // ALCT was not used for LCT
                tmb_clct_discard    = clct_discard;                         // CLCT was not used for LCT

                tmb_alct0           = alct0_pipe;                           // Copy of ALCT for header
                tmb_alct1           = alct1_pipe;

                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: tmb_trig_pulse       = %c\n",logical(tmb_trig_pulse      ));
                fprintf(stdout,"TMBemu: tmb_trig_keep_ff     = %c\n",logical(tmb_trig_keep_ff    ));
                fprintf(stdout,"TMBemu: tmb_non_trig_keep_ff = %c\n",logical(tmb_non_trig_keep_ff));
                fprintf(stdout,"TMBemu: tmb_match            = %c\n",logical(tmb_match           ));
                fprintf(stdout,"TMBemu: tmb_alct_only        = %c\n",logical(tmb_alct_only       ));
                fprintf(stdout,"TMBemu: tmb_clct_only        = %c\n",logical(tmb_clct_only       ));
                fprintf(stdout,"TMBemu: tmb_match_ro_ff      = %c\n",logical(tmb_match_ro_ff     ));
                fprintf(stdout,"TMBemu: tmb_alct_only_ro_ff  = %c\n",logical(tmb_alct_only_ro_ff ));
                fprintf(stdout,"TMBemu: tmb_clct_only_ro_ff  = %c\n",logical(tmb_clct_only_ro_ff ));
                fprintf(stdout,"TMBemu: tmb_alct_discard     = %c\n",logical(tmb_alct_discard    ));
                fprintf(stdout,"TMBemu: tmb_clct_discard     = %c\n",logical(tmb_clct_discard    ));
                fprintf(stdout,"TMBemu: tmb_alct0            = %4.4X\n",tmb_alct0);
                fprintf(stdout,"TMBemu: tmb_alct1            = %4.4X\n",tmb_alct1);

                // TMBemu: Kill CLCTs from ME1A, TMB firmware handles this incorrectly as of 4/6/2010
                kill_me1a_clcts = (mpc_me1a_block==1 && csc_me1ab==1);

                clct0_exists = clct0_vpf_tmb==1;
                clct1_exists = clct1_vpf_tmb==1;

                clct0_cfeb4  = clct0_cfeb_tmb==4;               // CLCT0 is on CFEB4 hence ME1A
                clct1_cfeb4  = clct1_cfeb_tmb==4;               // CLCT1 is on CFEB4 hence ME1A

                kill_clct0   = clct0_cfeb4 && kill_me1a_clcts;  // Delete CLCT0 from ME1A
                kill_clct1   = clct1_cfeb4 && kill_me1a_clcts;  // Delete CLCT1 from ME1A

                kill_trig    = ((kill_clct0 && clct0_exists) && (kill_clct1 && clct1_exists))   // Kill both clcts
                    ||             ((kill_clct0 && clct0_exists) && !clct1_exists)
                    ||             ((kill_clct1 && clct1_exists) && !clct0_exists);

                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: clct0_exists         = %c\n",logical(clct0_exists));
                fprintf(stdout,"TMBemu: clct1_exists         = %c\n",logical(clct1_exists));
                fprintf(stdout,"TMBemu: clct0_cfeb4          = %c\n",logical(clct0_cfeb4 ));
                fprintf(stdout,"TMBemu: clct1_cfeb4          = %c\n",logical(clct1_cfeb4 ));
                fprintf(stdout,"TMBemu: kill_clct0           = %c\n",logical(kill_clct0  ));
                fprintf(stdout,"TMBemu: kill_clct1           = %c\n",logical(kill_clct1  ));
                fprintf(stdout,"TMBemu: kill_trig            = %c\n",logical(kill_trig   ));

                // TMBemu: Had to wait for kill signal to go valid
                tmb_match_ro     = tmb_match_ro_ff     & kill_trig; // ALCT and CLCT matched in time, nontriggering event
                tmb_alct_only_ro = tmb_alct_only_ro_ff & kill_trig; // Only ALCT triggered, nontriggering event
                tmb_clct_only_ro = tmb_clct_only_ro_ff & kill_trig; // Only CLCT triggered, nontriggering event

                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: tmb_match_ro         = %c\n",logical(tmb_match_ro    ));
                fprintf(stdout,"TMBemu: tmb_alct_only_ro     = %c\n",logical(tmb_alct_only_ro));
                fprintf(stdout,"TMBemu: tmb_clct_only_ro     = %c\n",logical(tmb_clct_only_ro));

                // TMBemu: Post FF mod trig_keep for me1a
                tmb_trig_keep     = tmb_trig_keep_ff && (!kill_trig || tmb_alct_only);
                tmb_non_trig_keep = tmb_non_trig_keep_ff && !tmb_trig_keep;

                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: tmb_trig_keep        = %c\n",logical(tmb_trig_keep    ));
                fprintf(stdout,"TMBemu: tmb_non_trig_keep    = %c\n",logical(tmb_non_trig_keep));

                // TMBemu: Pipelined CLCTs, aligned in time with trig_pulse
                keep_clct  = trig_pulse && (trig_keep || non_trig_keep);

                clct0_real = clct0_pipe * keep_clct;
                clct1_real = clct1_pipe * keep_clct;
                clctc_real = clctc_pipe * keep_clct;

                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: keep_clct            = %c\n",logical(keep_clct));
                fprintf(stdout,"TMBemu: clct0_real           = %c\n",logical(clct0_real   ));
                fprintf(stdout,"TMBemu: clct1_real           = %c\n",logical(clct1_real   ));
                fprintf(stdout,"TMBemu: clctc_real           = %c\n",logical(clctc_real   ));

                // TMBemu: Latch pipelined ALCTs, aligned in time with CLCTs because CLCTs are delayed 1bx in the SRLs
                alct0_real = alct0_pipe;
                alct1_real = alct1_pipe;

                // TMBemu: Fill in missing ALCT if CLCT has 2 muons, missing CLCT if ALCT has 2 muons
                alct0_vpf_emu = (alct0_real >> 0) & 0x1;            // Extract valid pattern flags
                alct1_vpf_emu = (alct1_real >> 0) & 0x1;
                clct0_vpf_emu = (clct0_real >> 0) & 0x1;
                clct1_vpf_emu = (clct1_real >> 0) & 0x1;

                clct_bxn_insert_emu = clctc_real & 0x3;     // CLCT bunch crossing number for events missing alct

                tmb_no_alct_emu  = !alct0_vpf_emu;
                tmb_no_clct_emu  = !clct0_vpf_emu;

                tmb_one_alct_emu = alct0_vpf_emu && !alct1_vpf_emu;
                tmb_one_clct_emu = clct0_vpf_emu && !clct1_vpf_emu;

                tmb_two_alct_emu = alct0_vpf_emu && alct1_vpf_emu;
                tmb_two_clct_emu = clct0_vpf_emu && clct1_vpf_emu;

                tmb_dupe_alct_emu = tmb_one_alct_emu && tmb_two_clct_emu;   // Duplicate alct if there are 2 clcts
                tmb_dupe_clct_emu = tmb_one_clct_emu && tmb_two_alct_emu;   // Duplicate clct if there are 2 alcts

                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: tmb_no_alct_emu      = %c\n",logical(tmb_no_alct_emu  ));
                fprintf(stdout,"TMBemu: tmb_no_clct_emu      = %c\n",logical(tmb_no_clct_emu  ));
                fprintf(stdout,"TMBemu: tmb_one_alct_emu     = %c\n",logical(tmb_one_alct_emu ));
                fprintf(stdout,"TMBemu: tmb_one_clct_emu     = %c\n",logical(tmb_one_clct_emu ));
                fprintf(stdout,"TMBemu: tmb_two_alct_emu     = %c\n",logical(tmb_two_alct_emu ));
                fprintf(stdout,"TMBemu: tmb_two_clct_emu     = %c\n",logical(tmb_two_clct_emu ));
                fprintf(stdout,"TMBemu: tmb_dupe_alct_emu    = %c\n",logical(tmb_dupe_alct_emu));
                fprintf(stdout,"TMBemu: tmb_dupe_clct_emu    = %c\n",logical(tmb_dupe_clct_emu));

                // TMBemu: Duplicate alct and clct
                alct_dummy  = (clct_bxn_insert_emu << 11);                  // Insert clct bxn for clct-only events
                clct_dummy  = 0;                                            // Blank  clct for alct-only events
                clctc_dummy = 0;                                            // Blank  clct common for alct-only events

                if       (tmb_no_clct_emu  ) {clct0_emu = clct_dummy;  clct1_emu = clct_dummy;  clctc_emu = clctc_dummy;}   // clct0 and clct1 do not exist, use dummy clct 
                else if  (tmb_dupe_clct_emu) {clct0_emu = clct0_real;  clct1_emu = clct0_real;  clctc_emu = clctc_real; }   // clct0 exists, but clct1 does not exist, copy clct0 into clct1
                else                         {clct0_emu = clct0_real;  clct1_emu = clct1_real;  clctc_emu = clctc_real; }   // clct0 and clct1 exist, so use them

                if      (tmb_no_alct_emu  ) {alct0_emu = alct_dummy;  alct1_emu = alct_dummy;} // alct0 and alct1 do not exist, use dummy alct
                else if (tmb_dupe_alct_emu) {alct0_emu = alct0_real;  alct1_emu = alct0_real;} // alct0 exists, but alct1 does not exist, copy alct0 into alct1
                else                        {alct0_emu = alct0_real;  alct1_emu = alct1_real;} // alct0 and alct1 exist, so use them

                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: clct0_emu            = %2.2X\n",clct0_emu);
                fprintf(stdout,"TMBemu: clct1_emu            = %2.2X\n",clct1_emu);
                fprintf(stdout,"TMBemu: alct0_emu            = %2.2X\n",alct0_emu);
                fprintf(stdout,"TMBemu: alct1_emu            = %2.2X\n",alct1_emu);

                // TMBemu: LCT valid pattern flags
                lct0_vpf_emu    = alct0_vpf_emu || clct0_vpf_emu;   // First muon exists
                lct1_vpf_emu    = alct1_vpf_emu || clct1_vpf_emu;   // Second muon exists

                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: lct0_vpf_emu         = %1.1X\n",lct0_vpf_emu);
                fprintf(stdout,"TMBemu: lct1_vpf_emu         = %1.1X\n",lct1_vpf_emu);

                // TMBemu: Decompose ALCT muons
                alct0_valid_emu     = (alct0_emu >>  0) & 0x1;      // Valid pattern flag
                alct0_quality_emu   = (alct0_emu >>  1) & 0x3;      // Pattern quality
                alct0_amu_emu       = (alct0_emu >>  3) & 0x1;      // Accelerator muon
                alct0_key_emu       = (alct0_emu >>  4) & 0x7F;     // Key Wire Group
                alct0_bxn_emu       = (alct0_emu >> 11) & 0x1;      // Bunch crossing number

                alct1_valid_emu     = (alct1_emu >>  0) & 0x1;      // Valid pattern flag
                alct1_quality_emu   = (alct1_emu >>  1) & 0x3;      // Pattern quality
                alct1_amu_emu       = (alct1_emu >>  3) & 0x1;      // Accelerator muon
                alct1_key_emu       = (alct1_emu >>  4) & 0x7F;     // Key Wire Group
                alct1_bxn_emu       = (alct1_emu >> 11) & 0x1;      // Bunch crossing number

                alct_bx0_emu        = alct0_bxn_emu==0;

                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: alct0_valid_emu      = %1.1X\n",alct0_valid_emu);
                fprintf(stdout,"TMBemu: alct0_quality_emu    = %1.1X\n",alct0_quality_emu);
                fprintf(stdout,"TMBemu: alct0_amu_emu        = %1.1X\n",alct0_amu_emu);
                fprintf(stdout,"TMBemu: alct0_key_emu        = %2.2X\n",alct0_key_emu);
                fprintf(stdout,"TMBemu: alct0_bxn_emu        = %2.2X\n",alct0_bxn_emu);
                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: alct1_valid_emu      = %1.1X\n",alct1_valid_emu);
                fprintf(stdout,"TMBemu: alct1_quality_emu    = %1.1X\n",alct1_quality_emu);
                fprintf(stdout,"TMBemu: alct1_amu_emu        = %1.1X\n",alct1_amu_emu);
                fprintf(stdout,"TMBemu: alct1_key_emu        = %2.2X\n",alct1_key_emu);
                fprintf(stdout,"TMBemu: alct1_bxn_emu        = %2.2X\n",alct1_bxn_emu);
                fprintf(stdout,"TMBemu: alct_bx0_emu         = %1.1X\n",alct_bx0_emu);

                // TMBemu: Decompose CLCT muons
                clct0_valid_emu     = (clct0_emu >>  0) & 0x1;      // Valid pattern flag
                clct0_hit_emu       = (clct0_emu >>  1) & 0x7;      // Hits on pattern 0-6
                clct0_pid_emu       = (clct0_emu >>  4) & 0xF;      // Pattern shape 0-A
                clct0_key_emu       = (clct0_emu >>  8) & 0x1F;     // 1/2-strip ID number
                clct0_cfeb_emu      = (clct0_emu >> 13) & 0x7;      // Key CFEB ID

                clct_bxn_emu        = (clctc_emu >>  0) & 0x3;      // Bunch crossing number
                clct_sync_err_emu   = (clctc_emu >>  2) & 0x1;      // Bx0 disagreed with bxn counter

                clct1_valid_emu     = (clct1_emu >>  0) & 0x1;      // Valid pattern flag
                clct1_hit_emu       = (clct1_emu >>  1) & 0x7;      // Hits on pattern 0-6
                clct1_pid_emu       = (clct1_emu >>  4) & 0xF;      // Pattern shape 0-A
                clct1_key_emu       = (clct1_emu >>  8) & 0x1F;     // 1/2-strip ID number
                clct1_cfeb_emu      = (clct1_emu >> 13) & 0x7;      // Key CFEB ID

                clct0_bend_emu = (clct0_pid_emu >> 0) & 0x1;
                clct1_bend_emu = (clct1_pid_emu >> 0) & 0x1;

                csc_id_emu     = csc_id;
                clct_bx0_emu   = clct_bxn_emu==0;

                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: clct0_valid_emu     = %1.1X\n",clct0_valid_emu);
                fprintf(stdout,"TMBemu: clct0_hit_emu       = %1.1X\n",clct0_hit_emu);
                fprintf(stdout,"TMBemu: clct0_pid_emu       = %1.1X\n",clct0_pid_emu);
                fprintf(stdout,"TMBemu: clct0_key_emu       = %2.2X\n",clct0_key_emu);
                fprintf(stdout,"TMBemu: clct0_cfeb_emu      = %1.1X\n",clct0_cfeb_emu);
                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: clct1_valid_emu     = %1.1X\n",clct1_valid_emu);
                fprintf(stdout,"TMBemu: clct1_hit_emu       = %1.1X\n",clct1_hit_emu);
                fprintf(stdout,"TMBemu: clct1_pid_emu       = %1.1X\n",clct1_pid_emu);
                fprintf(stdout,"TMBemu: clct1_key_emu       = %2.2X\n",clct1_key_emu);
                fprintf(stdout,"TMBemu: clct1_cfeb_emu      = %1.1X\n",clct1_cfeb_emu);
                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: clct0_bend_emu      = %1.1X\n",clct0_bend_emu);
                fprintf(stdout,"TMBemu: clct1_bend_emu      = %1.1X\n",clct1_bend_emu);
                fprintf(stdout,"TMBemu: clct_bxn_emu        = %1.1X\n",clct_bxn_emu);
                fprintf(stdout,"TMBemu: clct_bx0_emu        = %1.1X\n",clct_bx0_emu);
                fprintf(stdout,"TMBemu: csc_id_emu          = %1.1X\n",csc_id_emu);
                fprintf(stdout,"TMBemu: clct_sync_err_emu   = %1.1X\n",clct_sync_err_emu);
                fprintf(stdout,"\n");

                // LCT Quality
                alct0_hit_emu  = alct0_quality_emu + 3;             // Convert alct quality to number of hits
                alct1_hit_emu  = alct1_quality_emu + 3;

                clct0_cpat_emu = (clct0_hit_emu >= 2);
                clct1_cpat_emu = (clct1_hit_emu >= 2);

                ACC  = alct0_amu_emu;               // In   ALCT accelerator muon bit
                A    = alct0_valid_emu;             // In   bit: ALCT was found
                C    = clct0_valid_emu;             // In   bit: CLCT was found
                A4   = alct0_hit_emu>>2 & 0x1;      // In   bit (N_A>=4), where N_A=number of ALCT layers
                C4   = clct0_hit_emu>>2 & 0x1;      // In   bit (N_C>=4), where N_C=number of CLCT layers
                P    = clct0_pid_emu;               // In   4-bit CLCT pattern number that is presently 1 for n-layer triggers, 2-10 for current patterns, 11-15 "for future expansion".
                CPAT = clct0_cpat_emu;              // In   bit for cathode .pattern trigger., i.e. (P>=2 && P<=10) at present

                lct_quality (ACC, A, C, A4, C4, P, CPAT, Q);

                lct0_quality_emu = Q;

                ACC  = alct1_amu_emu;               // In   ALCT accelerator muon bit
                A    = alct1_valid_emu;             // In   bit: ALCT was found
                C    = clct1_valid_emu;             // In   bit: CLCT was found
                A4   = alct1_hit_emu>>2 & 0x1;      // In   bit (N_A>=4), where N_A=number of ALCT layers
                C4   = clct1_hit_emu>>2 & 0x1;      // In   bit (N_C>=4), where N_C=number of CLCT layers
                P    = clct1_pid_emu;               // In   4-bit CLCT pattern number that is presently 1 for n-layer triggers, 2-10 for current patterns, 11-15 "for future expansion".
                CPAT = clct1_cpat_emu;              // In   bit for cathode .pattern trigger., i.e. (P>=2 && P<=10) at present

                lct_quality (ACC, A, C, A4, C4, P, CPAT, Q);

                lct1_quality_emu = Q;

                // TMB is supposed to rank LCTs, but doesn't yet, this is a bug, should be after the mpc0_frame0_pulse section below
                tmb_rank_err_emu = (lct0_quality_emu*lct0_vpf_emu) < (lct1_quality_emu * lct1_vpf_emu);

                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: tmb_rank_err_emu  = %4.1X\n",tmb_rank_err_emu);

                // TMBemu: Format MPC output words
                mpc0_frame0_emu =
                    (alct0_key_emu    <<  0)    |
                    (clct0_pid_emu    <<  7)    |
                    (lct0_quality_emu << 11)    |
                    (lct0_vpf_emu     << 15);

                mpc0_frame1_emu =
                    (clct0_cfeb_emu<<5) | clct0_key_emu|
                    (clct0_bend_emu       <<  8)    |
                    (clct_sync_err_emu    <<  9)    |
                    ((alct0_bxn_emu & 0x1) << 10)   |
                    ((clct_bx0_emu  & 0x1) << 11)   |   // bx0 gets replaced after mpc_tx_delay, keep here to mollify xst
                    ((csc_id_emu    & 0xF) << 12);

                mpc1_frame0_emu =
                    (alct1_key_emu    <<  0)    |
                    (clct1_pid_emu    <<  7)    |
                    (lct1_quality_emu << 11)    |
                    (lct1_vpf_emu     << 15);

                mpc1_frame1_emu =
                    (clct1_cfeb_emu<<5) | clct1_key_emu|
                    (clct1_bend_emu       <<  8)    |
                    (clct_sync_err_emu    <<  9)    |
                    ((alct1_bxn_emu & 0x1) << 10)   |
                    ((alct_bx0_emu  & 0x1) << 11)   |   // bx0 gets replaced after mpc_tx_delay, keep here to mollify xst
                    ((csc_id_emu    & 0xF) << 12);

                // TMBemu: Construct MPC output words for MPC, blanked if no muons present, except bx0 [inserted after mpc_tx_delay]
                trig_mpc_emu  = tmb_trig_pulse && tmb_trig_keep;                // Trigger this event
                trig_mpc0_emu = trig_mpc_emu && lct0_vpf_emu && !kill_clct0;    // LCT 0 is valid, send to mpc
                trig_mpc1_emu = trig_mpc_emu && lct1_vpf_emu && !kill_clct1;    // LCT 1 is valid, send to mpc

                mpc0_frame0_pulse = (trig_mpc0_emu) ? mpc0_frame0_emu : 0;
                mpc0_frame1_pulse = (trig_mpc0_emu) ? mpc0_frame1_emu : 0;
                mpc1_frame0_pulse = (trig_mpc1_emu) ? mpc1_frame0_emu : 0;
                mpc1_frame1_pulse = (trig_mpc1_emu) ? mpc1_frame1_emu : 0;

                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: trig_mpc_emu         = %1.1X\n",trig_mpc_emu );
                fprintf(stdout,"TMBemu: trig_mpc0_emu        = %1.1X\n",trig_mpc0_emu);
                fprintf(stdout,"TMBemu: trig_mpc1_emu        = %1.1X\n",trig_mpc1_emu);

                fprintf(stdout,"\n");
                fprintf(stdout,"TMBemu: mpc0_frame0_pulse    = %4.4X\n",mpc0_frame0_pulse);
                fprintf(stdout,"TMBemu: mpc0_frame1_pulse    = %4.4X\n",mpc0_frame1_pulse);
                fprintf(stdout,"TMBemu: mpc1_frame0_pulse    = %4.4X\n",mpc1_frame0_pulse);
                fprintf(stdout,"TMBemu: mpc1_frame1_pulse    = %4.4X\n",mpc1_frame1_pulse);
                fprintf(stdout,"\n");

                // Read latched MPC data
                adr    = mpc0_frame0_adr+base_adr;
                status = vme_read(adr,rd_data);
                mpc0_frame0_vme = rd_data;

                adr    = mpc0_frame1_adr+base_adr;
                status = vme_read(adr,rd_data);
                mpc0_frame1_vme = rd_data;

                adr    = mpc1_frame0_adr+base_adr;
                status = vme_read(adr,rd_data);
                mpc1_frame0_vme = rd_data;

                adr    = mpc1_frame1_adr+base_adr;
                status = vme_read(adr,rd_data);
                mpc1_frame1_vme = rd_data;

                fprintf(stdout,"VME:    mpc0_frame0_vme      = %4.4X\n",mpc0_frame0_vme);
                fprintf(stdout,"VME:    mpc0_frame1_vme      = %4.4X\n",mpc0_frame1_vme);
                fprintf(stdout,"VME:    mpc1_frame0_vme      = %4.4X\n",mpc1_frame0_vme);
                fprintf(stdout,"VME:    mpc1_frame1_vme      = %4.4X\n",mpc1_frame1_vme);

                // TMBemu: Decompose expected MPC frames
                mpc_alct0_key_expect    =   (mpc0_frame0_pulse >>  0) & 0x007F;
                mpc_clct0_pid_expect    =   (mpc0_frame0_pulse >>  7) & 0x000F;
                mpc_lct0_qual_expect    =   (mpc0_frame0_pulse >> 11) & 0x000F;
                mpc_lct0_vpf_expect     =   (mpc0_frame0_pulse >> 15) & 0x0001;

                mpc_clct0_key_expect    =   (mpc0_frame1_pulse >>  0) & 0x00FF;
                mpc_clct0_bend_expect   =   (mpc0_frame1_pulse >>  8) & 0x0001;
                mpc_sync_err0_expect    =   (mpc0_frame1_pulse >>  9) & 0x0001;
                mpc_alct0_bxn_expect    =   (mpc0_frame1_pulse >> 10) & 0x0001;
                mpc_bx0_clct_expect     =   (mpc0_frame1_pulse >> 11) & 0x0001;
                mpc_csc_id0_expect      =   (mpc0_frame1_pulse >> 12) & 0x000F;

                mpc_alct1_key_expect    =   (mpc1_frame0_pulse >>  0) & 0x007F;
                mpc_clct1_pid_expect    =   (mpc1_frame0_pulse >>  7) & 0x000F;
                mpc_lct1_qual_expect    =   (mpc1_frame0_pulse >> 11) & 0x000F;
                mpc_lct1_vpf_expect     =   (mpc1_frame0_pulse >> 15) & 0x0001;

                mpc_clct1_key_expect    =   (mpc1_frame1_pulse >>  0) & 0x00FF;
                mpc_clct1_bend_expect   =   (mpc1_frame1_pulse >>  8) & 0x0001;
                mpc_sync_err1_expect    =   (mpc1_frame1_pulse >>  9) & 0x0001;
                mpc_alct1_bxn_expect    =   (mpc1_frame1_pulse >> 10) & 0x0001;
                mpc_bx0_alct_expect     =   (mpc1_frame1_pulse >> 11) & 0x0001;
                mpc_csc_id1_expect      =   (mpc1_frame1_pulse >> 12) & 0x000F;

                // Decompose MPC frames latched by VME
                mpc_alct0_key_vme       =   (mpc0_frame0_vme >>  0) & 0x007F;
                mpc_clct0_pid_vme       =   (mpc0_frame0_vme >>  7) & 0x000F;
                mpc_lct0_qual_vme       =   (mpc0_frame0_vme >> 11) & 0x000F;
                mpc_lct0_vpf_vme        =   (mpc0_frame0_vme >> 15) & 0x0001;

                mpc_clct0_key_vme       =   (mpc0_frame1_vme >>  0) & 0x00FF;
                mpc_clct0_bend_vme      =   (mpc0_frame1_vme >>  8) & 0x0001;
                mpc_sync_err0_vme       =   (mpc0_frame1_vme >>  9) & 0x0001;
                mpc_alct0_bxn_vme       =   (mpc0_frame1_vme >> 10) & 0x0001;
                mpc_bx0_clct_vme        =   (mpc0_frame1_vme >> 11) & 0x0001;
                mpc_csc_id0_vme         =   (mpc0_frame1_vme >> 12) & 0x000F;

                mpc_alct1_key_vme       =   (mpc1_frame0_vme >>  0) & 0x007F;
                mpc_clct1_pid_vme       =   (mpc1_frame0_vme >>  7) & 0x000F;
                mpc_lct1_qual_vme       =   (mpc1_frame0_vme >> 11) & 0x000F;
                mpc_lct1_vpf_vme        =   (mpc1_frame0_vme >> 15) & 0x0001;

                mpc_clct1_key_vme       =   (mpc1_frame1_vme >>  0) & 0x00FF;
                mpc_clct1_bend_vme      =   (mpc1_frame1_vme >>  8) & 0x0001;
                mpc_sync_err1_vme       =   (mpc1_frame1_vme >>  9) & 0x0001;
                mpc_alct1_bxn_vme       =   (mpc1_frame1_vme >> 10) & 0x0001;
                mpc_bx0_alct_vme        =   (mpc1_frame1_vme >> 11) & 0x0001;
                mpc_csc_id1_vme         =   (mpc1_frame1_vme >> 12) & 0x000F;

                // Compare expected to found MPC frames
                ck("mpc: mpc_alct0_key  read vme.expect tmbemu ",mpc_alct0_key_vme  ,mpc_alct0_key_expect); 
                ck("mpc: mpc_clct0_pid  read vme.expect tmbemu ",mpc_clct0_pid_vme  ,mpc_clct0_pid_expect);
                ck("mpc: mpc_lct0_qual  read vme.expect tmbemu ",mpc_lct0_qual_vme  ,mpc_lct0_qual_expect);
                ck("mpc: mpc_lct0_vpf   read vme.expect tmbemu ",mpc_lct0_vpf_vme   ,mpc_lct0_vpf_expect);

                ck("mpc: mpc_clct0_key  read vme.expect tmbemu ",mpc_clct0_key_vme  ,mpc_clct0_key_expect);
                ck("mpc: mpc_clct0_bend read vme.expect tmbemu ",mpc_clct0_bend_vme ,mpc_clct0_bend_expect);
                ck("mpc: mpc_sync_err0  read vme.expect tmbemu ",mpc_sync_err0_vme  ,mpc_sync_err0_expect);
                ck("mpc: mpc_alct0_bxn  read vme.expect tmbemu ",mpc_alct0_bxn_vme  ,mpc_alct0_bxn_expect);
                ck("mpc: mpc_bx0_clct   read vme.expect tmbemu ",mpc_bx0_clct_vme   ,0);
                ck("mpc: mpc_csc_id0    read vme.expect tmbemu ",mpc_csc_id0_vme    ,mpc_csc_id0_expect);

                ck("mpc: mpc_alct1_key  read vme.expect tmbemu ",mpc_alct1_key_vme  ,mpc_alct1_key_expect);
                ck("mpc: mpc_clct1_pid  read vme.expect tmbemu ",mpc_clct1_pid_vme  ,mpc_clct1_pid_expect);
                ck("mpc: mpc_lct1_qual  read vme.expect tmbemu ",mpc_lct1_qual_vme  ,mpc_lct1_qual_expect);
                ck("mpc: mpc_lct1_vpf   read vme.expect tmbemu ",mpc_lct1_vpf_vme   ,mpc_lct1_vpf_expect);

                ck("mpc: mpc_clct1_key  read vme.expect tmbemu ",mpc_clct1_key_vme  ,mpc_clct1_key_expect);
                ck("mpc: mpc_clct1_bend read vme.expect tmbemu ",mpc_clct1_bend_vme ,mpc_clct1_bend_expect);
                ck("mpc: mpc_sync_err1  read vme.expect tmbemu ",mpc_sync_err1_vme  ,mpc_sync_err1_expect);
                ck("mpc: mpc_alct1_bxn  read vme.expect tmbemu ",mpc_alct1_bxn_vme  ,mpc_alct1_bxn_expect);
                ck("mpc: mpc_bx0_alct   read vme.expect tmbemu ",mpc_bx0_alct_vme   ,0);
                ck("mpc: mpc_csc_id1    read vme.expect tmbemu ",mpc_csc_id1_vme    ,mpc_csc_id1_expect);

                // LCTs for display
                lct0_vme = (mpc0_frame1_vme<<16) | mpc0_frame0_vme;
                lct1_vme = (mpc1_frame1_vme<<16) | mpc1_frame0_vme;

                // Marker state indicats CLCTs found by TMB match injected CLCTs for display
                marker = "ER";

                if (clct0_vpf_vme == clct0_vpf_vme_expect &&
                        clct0_hit_vme == clct0_hit_vme_expect &&
                        clct0_pid_vme == clct0_pid_vme_expect &&
                        clct0_key_vme == clct0_key_vme_expect &&
                        clct1_vpf_vme == clct1_vpf_vme_expect &&
                        clct1_hit_vme == clct1_hit_vme_expect &&
                        clct1_pid_vme == clct1_pid_vme_expect &&
                        clct1_key_vme == clct1_key_vme_expect)
                    marker = "OK";

                // Display CLCTs
                fprintf(stdout,"Key=%3i %s ",ikeylp,marker.c_str());
                fprintf(stdout,"clct0: vpf=%1i nhit=%1i pid=%1X key=%3i  ",clct0_vpf_tmb, clct0_hit_tmb, clct0_pid_tmb, clct0_key_tmb);
                fprintf(stdout,"clct1: vpf=%1i nhit=%1i pid=%1X key=%3i  ",clct1_vpf_tmb, clct1_hit_tmb, clct1_pid_tmb, clct1_key_tmb);
                fprintf(stdout,"lct0=%8.8X ", lct0_vme);
                fprintf(stdout,"lct1=%8.8X\n",lct1_vme);

                // write CLCT0 and CLCT1
                fprintf(stdout,"CFEB%1i Key=%3i %s ",icfeb,ikeylp,marker.c_str());
                fprintf(stdout,"clct0: vpf=%1i nhit=%1i pid=%1X key=%3i cfeb=%1i bxn=%1i  ",clct0_vpf_tmb, clct0_hit_tmb,  clct0_pid_tmb, clct0_key_tmb, clct0_cfeb_tmb, clctc_bxn_tmb);
                fprintf(stdout,"clct1: vpf=%1i nhit=%1i pid=%1X key=%3i cfeb=%1i bxn=%1i  ",clct1_vpf_tmb, clct1_hit_tmb,  clct1_pid_tmb, clct1_key_tmb, clct1_cfeb_tmb, clctc_bxn_tmb);
                fprintf(stdout,"alct0: vpf=%1i qual=%1X amu=%1i key=%3i bxn=%1i   ",        alct0_vpf_inj, alct0_qual_inj, alct0_amu_inj, alct0_key_inj, alct0_bxn_inj);
                fprintf(stdout,"alct1: vpf=%1i qual=%1X amu=%1i key=%3i bxn=%1i   ",        alct1_vpf_inj, alct1_qual_inj, alct1_amu_inj, alct1_key_inj, alct1_bxn_inj);
                fprintf(stdout,"lct0=%8.8X ", lct0_vme);
                fprintf(stdout,"lct1=%8.8X\n",lct1_vme);

                // Display CLCT keys generated vs found
                if (first_scn) {    // Display column heading, show 1/2 strip numbers 0-159
                    fprintf(stdout,"CLCT_sep=%3i",clct_sep);
                    fprintf(stdout,"                         ");
                    fprintf(stdout,"<------------------------------ CLCT key 1/2strips found by TMB ------------------------------->\n");

                    fprintf(stdout,"     "); for (i=0; i<=159; ++i) {symbol=' '; if (i%32<14||i%32>18) fprintf(stdout,"%c",symbol); if (i%32==14) fprintf(stdout,"CFEB%1i",i/32);}  fprintf(stdout,"\n");
                    fprintf(stdout,"     "); for (i=0; i<=159; ++i) {symbol='-'; if (i%32==0||i%32==31) symbol='|'; fprintf(stdout,"%c",symbol);}  fprintf(stdout,"\n");
                    fprintf(stdout,"     "); for (i=0; i<=159; ++i) {symbol=' '; if (i>=100) symbol='0'+(i/100)%10; fprintf(stdout,"%c",symbol);}  fprintf(stdout,"\n");
                    fprintf(stdout,"     "); for (i=0; i<=159; ++i) {symbol=' '; if (i>= 10) symbol='0'+(i/10 )%10; fprintf(stdout,"%c",symbol);}  fprintf(stdout,"\n");
                    fprintf(stdout,"     "); for (i=0; i<=159; ++i) {symbol=' '; if (i>=  0) symbol='0'+(i/1  )%10; fprintf(stdout,"%c",symbol);}  fprintf(stdout,"\n");
                    first_scn=false;
                }

                symbol=' ';                                                         // display row heading
                if (ikey%32==0||ikey%32==31) symbol='-';
                fprintf(stdout,"%c%c%3i",symbol,symbol,ikey);

                for (i=0; i<=159; ++i) {
                    symbol=' '; 
                    if ((1 << (i/32) & active_feb_list)!=0)
                        symbol='.';                 // cfeb is in the active feb list

                    if (clct0_vpf_tmb==1 && clct0_key_tmb==i) 
                        symbol='0'+clct0_hit_tmb;   // clct0 hits 0-6
                    if (clct1_vpf_tmb==1 && clct1_key_tmb==i) 
                        symbol='A'+clct1_hit_tmb;   // clct1 hits A-G
                    if (clct0_vpf_tmb==1 && clct0_key_tmb==i  && clct1_vpf_tmb==1 && clct1_key_tmb==i) 
                        symbol='X';                 // clct0 and clct1 on the same key, should not happen

                    fprintf(stdout,"%c",symbol);
                } 

                fprintf(stdout,"\n");                                       // cr on current ikey line

                if (ikey==159) {                                            // partial column headings again after last row
                    fprintf(stdout,"     "); 
                    for (i=0; i<=159; ++i) {
                        symbol=' '; 
                        if (i>=  0) symbol='0'+(i/1  )%10; 
                        fprintf(stdout,"%c",symbol);
                    }  
                    fprintf(stdout,"\n");
                }

                do {  //replaced goto statement with do {...} while (false) 
                    // Get DMB RAM word count and busy bit
                    if (!rrhd)      break; 

                    adr    = dmb_wdcnt_adr+base_adr;
                    status = vme_read(adr,rd_data);
                    dmb_wdcnt = rd_data & 0x0FFF;
                    dmb_busy  = (rd_data >> 14) & 0x0001;

                    fprintf(stdout,"Raw Hits Dump: ikey=%3i\n",ikey);
                    fprintf(stdout,"word count = %4i\n",dmb_wdcnt);
                    fprintf(stdout,"busy       = %4i\n",dmb_busy);

                    if (dmb_busy) {
                        fprintf(stdout,"Can not read RAM: dmb reports busy\n");
                        break; 
                    }

                    if (dmb_wdcnt <= 0) {
                        fprintf(stdout,"Can not read RAM: dmb reports word count <=0\n");
                        break; 
                    }

                    // Write RAM read address to TMB
                    for (i=0; i<=dmb_wdcnt-1; ++i) {
                        adr     = dmb_ram_adr+base_adr;
                        wr_data = i & 0xffff;
                        status  = vme_write(adr,wr_data);

                        // Read RAM data from TMB
                        adr    = dmb_rdata_adr+base_adr;
                        status = vme_read(adr,rd_data);             // read lsbs
                        dmb_rdata_lsb = rd_data;

                        adr    = dmb_wdcnt_adr+base_adr;
                        status = vme_read(adr,rd_data);             // read msbs
                        dmb_rdata_msb = (rd_data >> 12) & 0x3;      // rdata msbs
                        dmb_rdata     = dmb_rdata_lsb | (dmb_rdata_msb << 16);

                        vf_data[i]=dmb_rdata;
                        fprintf(stdout,"Adr=%5i Data=%6.5X\n",i,dmb_rdata);
                    } // close i

                    // Clear RAM address for next event
                    adr     = dmb_ram_adr+base_adr;
                    wr_data = 0x2000;   // reset RAM write address
                    status  = vme_write(adr,wr_data);
                    wr_data = 0x0000;   // unreset
                    status  = vme_write(adr,wr_data);

                    // Decode raw hits dump, variables passed by common block struct
                    decode_readout(vf_data,dmb_wdcnt,err_check=true);

                    fprintf(stdout,"Non-zero triad bits=%i\n",nonzero_triads);
                } while (false); //close do loop


                // Take snapshot of current counter state
                //L16708:
                adr = base_adr+cnt_ctrl_adr;
                wr_data=0x0022; //snap
                status = vme_write(adr,wr_data);
                wr_data=0x0020; //unsnap
                status = vme_write(adr,wr_data);

                // Read counters
                for (i=0; i<mxcounter; ++i) {
                    for (j=0; j<=1; ++j) {
                        adr = base_adr+cnt_ctrl_adr;
                        wr_data=(i << 9) | 0x0020 | (j << 8);
                        status = vme_write(adr,wr_data);
                        adr = base_adr+cnt_rdata_adr;
                        status = vme_read(adr,rd_data);

                        // Combine lsbs+msbs
                        if (j==0)           // Even addresses contain counter LSBs
                            cnt_lsb=rd_data;
                        else {              // Odd addresses contain counter MSBs
                            cnt_msb=rd_data;
                            cnt_full=cnt_lsb | (cnt_msb << 16);
                            cnt[i]=cnt_full;    // Assembled counter MSB,LSB
                        }
                    }}  //close j,i

                // Dislay counters
                for (i=0; i<mxcounter; ++i) {
                    fprintf(stdout,"\t%2.2i %10i %s\n",i,cnt[i],scnt[i].c_str());
                }

                // Read back embedded scope data
                scp_arm        = false;
                scp_readout    = true;
                scp_raw_decode = false;
                scp_silent     = true;
                scp_playback   = false;
                if (rdscope && scp_auto==0)
                    scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

                // Close big key-stepping loops
                if (pause_on_fail) pause("<cr> to resume");
            }   // L16709 continue
        }   // L16710 continue
        // L16730 continue

        // Set pattern thresholds back to default
        adr    = seq_clct_adr+base_adr;
        status = vme_read(adr,rd_data);

        hit_thresh_pretrig_temp   = hit_thresh_pretrig;
        hit_thresh_postdrift_temp = hit_thresh_postdrift;

        wr_data = rd_data & 0x8000;     // clear hit_thresh,nph_pattern,drift
        wr_data = wr_data | (triad_persist             <<  0);
        wr_data = wr_data | (hit_thresh_pretrig_temp   <<  4);
        wr_data = wr_data | (dmb_thresh_pretrig        <<  7);
        wr_data = wr_data | (hit_thresh_postdrift_temp << 10);
        wr_data = wr_data | (drift_delay               << 13);
        status  = vme_write(adr,wr_data);

        // Bang mode
        if (ifunc < 0)      usleep(1500000);
        else                break; 
    } //close Bang Mode loop
} //close Inject_walking_CLCT()

//------------------------------------------------------------------------------
//  External Trigger ALCT+CLCT
//------------------------------------------------------------------------------
void ExternalTriggerALCTCLCT () {
    //L16800:
    while(true) { //bang mode loop
        if (ifunc<0) rdscope = false;
        else         rdscope = true;

        // Turn off CCB backplane inputs, enable L1A emulator
        adr     = ccb_cfg_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data | 0x0005;
        status  = vme_write(adr,wr_data);

        // Turn off ALCT cable inputs
        adr     = alct_inj_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x0000;
        wr_data = wr_data | 0x0001;
            wr_data = wr_data | (alct_injector_delay << 5);
        status  = vme_write(adr,wr_data);

        // Turn off CFEB cable inputs
        adr     = cfeb_inj_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFFE0;
        status  = vme_write(adr,wr_data);

        // Turn on CFEB enables to over-ride mask_all
        adr     = seq_trig_en_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x03FF;     // clear old cfeb_en and source
        wr_data = wr_data | 0x7C00;     // ceb_en_source=0,cfeb_en=1F
        status  = vme_write(adr,wr_data);

        // Select sequencer to take clct ext or alct ext trig
        adr     = seq_trig_en_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFF00;
        wr_data = wr_data | 0x0060;     // Select alct or clct ext trig mode
        status  = vme_write(adr,wr_data);

        // Arm scope trigger
        scp_arm        = true;
        scp_readout    = false;
        scp_raw_decode = false;
        scp_silent     = false;
        scp_playback   = false;
        if (rdscope)
            scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

        // Fire pseudo-CCB external clct trigger linked to alct
        adr     = ccb_trig_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data | 0x0014;     // fire clct ext trig
        status  = vme_write(adr,wr_data);
        wr_data = rd_data & 0xFFE7;     // unfire
        status  = vme_write(adr,wr_data);

        // Read back trigger register to see what triggered
        adr     = seq_trig_src_adr+base_adr;
        status  = vme_read(adr,rd_data);

        if (rd_data!=0x0020) printf("\tTrigger source error rd_data=%4.4X\n",rd_data);

        // Read back embedded scope data
        scp_arm        = false;
        scp_readout    = true;
        scp_raw_decode = false;
        scp_silent     = true;
        scp_playback   = false;
        if (rdscope && scp_auto==0)
            scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

        // Bang Mode
        if (ifunc<0)    usleep (1500000);
        else            break;          //exit loop if bang mode == false
    } // close Bang mode loop
} //close ExternalTriggerALCTCLCT ()

//------------------------------------------------------------------------------
//  External Trigger ALCT
//------------------------------------------------------------------------------
void ExternalTriggerALCT () {
    while(true) { //bang mode loop
        if (ifunc<0) rdscope = false;
        else         rdscope = true;

        // Turn off CCB backplane inputs, enable L1A emulator
        adr     = ccb_cfg_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data | 0x0005;
        status  = vme_write(adr,wr_data);

        // Turn off ALCT cable inputs
        adr     = alct_inj_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x0000;
        wr_data = wr_data | 0x0001;
        wr_data = wr_data | (alct_injector_delay << 5);
        status  = vme_write(adr,wr_data);

        // Turn off CFEB cable inputs
        adr     = cfeb_inj_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFFE0;
        status  = vme_write(adr,wr_data);

        // Turn on CFEB enables to over-ride mask_all
        adr     = seq_trig_en_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x03FF;     // clear old cfeb_en and source
        wr_data = wr_data | 0x7C00;     // ceb_en_source=0,cfeb_en=1F
        status  = vme_write(adr,wr_data);

        // Select sequencer to take clct ext or alct ext trig
        adr      = seq_trig_en_adr+base_adr;
        status   = vme_read(adr,rd_data);
        wr_data  = rd_data & 0xFF00;
        wr_data  = wr_data | 0x0060;    // Select alct or clct ext trig mode
        status   = vme_write(adr,wr_data);

        // Arm scope trigger
        scp_arm        = true;
        scp_readout    = false;
        scp_raw_decode = false;
        scp_silent     = false;
        scp_playback   = false;
        if (rdscope)
            scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

        // Fire pseudo-CCB external trigger alct
        adr     = ccb_trig_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data | 0x000C;     // fire alct ext trig
        //  wr_data = rd_data | 0x0014;     // fire clct ext trig !!!!TEMPORARY !!!!!!
        status = vme_write(adr,wr_data);
        wr_data = rd_data | 0xFFF3; // unfire
        status = vme_write(adr,wr_data);

        // Read back trigger register to see what triggered
        adr    = seq_trig_src_adr+base_adr;
        status = vme_read(adr,rd_data);

        if (rd_data!=0x0040) printf("\tTrigger source error rd_data=%4.4X\n",rd_data);

        // Read back embedded scope data
        scp_arm        = false;
        scp_readout    = true;
        scp_raw_decode = false;
        scp_silent     = true;
        scp_playback   = false;
        if (rdscope && scp_auto==0)
            scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

        // Bang mode
        if (ifunc<0)        usleep (1500000);
        else                break; 
    } //close bang mode loop
} //close ExternalTriggerALCT ()

//------------------------------------------------------------------------------
//  External Trigger CLCT
//------------------------------------------------------------------------------
//L96000:
void ExternalTriggerCLCT () {
    while(true) { //bang mode loop
        if (ifunc<0) rdscope = false;
        else         rdscope = true;

        // Turn off CCB backplane inputs, enable L1A emulator
        adr     = ccb_cfg_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data | 0x0005;
        status  = vme_write(adr,wr_data);

        // Turn off ALCT cable inputs
        adr     = alct_inj_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x0000;
        wr_data = wr_data | 0x0001;
        wr_data = wr_data | (alct_injector_delay << 5);
        status  = vme_write(adr,wr_data);

        // Turn off CFEB cable inputs
        adr     = cfeb_inj_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFFE0;
        status  = vme_write(adr,wr_data);

        // Turn on CFEB enables to over-ride mask_all
        adr     = seq_trig_en_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x03FF; // clear old cfeb_en and source
        wr_data = wr_data | 0x7C00; // ceb_en_source=0,cfeb_en=1F
        status  = vme_write(adr,wr_data);

        // Select sequencer to take clct ext, turn on all cfebs
        adr     = seq_trig_en_adr+base_adr;
        wr_data = 0x0220;
        status  = vme_write(adr,wr_data);

        // Arm scope trigger
        scp_arm        = true;
        scp_readout    = false;
        scp_raw_decode = false;
        scp_silent     = false;
        scp_playback   = false;
        if (rdscope)
            scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

        // Fire pseudo-CCB external trigger clct
        adr     = ccb_trig_adr+base_adr;
        status  = vme_read(adr,rd_data);
        //  wr_data = rd_data | 0x000C;     // fire alct ext trig
        wr_data = rd_data | 0x0014;     // fire clct ext trig
        status  = vme_write(adr,wr_data);
        wr_data = rd_data & 0xFFE3;     // unfire
        status  = vme_write(adr,wr_data);

        // Read back trigger register to see what triggered
        adr    = seq_trig_src_adr+base_adr;
        status = vme_read(adr,rd_data);

        if (rd_data!=0x0020) printf("\tTrigger source error rd_data=%4.4X\n",rd_data);

        // Read back embedded scope data
        scp_arm        = false;
        scp_readout    = true;
        scp_raw_decode = false;
        scp_silent     = true;
        scp_playback   = false;
        if (rdscope && scp_auto==0)
            scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

        // Loop mode
        if (ifunc<0)        usleep(1500000);
        else                break; 
    } //close bang mode loop
} //close ExternalTriggerCLCT ()

//------------------------------------------------------------------------------
//  External Trigger ALCT+CLCT with GTL pulser
//------------------------------------------------------------------------------
void ExternalTriggerALCTCLCTwithGTLpulser () {
    //L96100:

    if (ifunc<0) 
        rdscope = false;
    else         
        rdscope = true;

    // Turn off CCB backplane inputs
    adr     = ccb_cfg_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data | 0x0005;
    status  = vme_write(adr,wr_data);

    // Enable GTL ccb_clct_ext_trig
    adr     = ccb_trig_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data | 0x0040;
    status  = vme_write(adr,wr_data);

    // Turn off ALCT cable inputs
    adr     = alct_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x0000;
    wr_data = wr_data | 0x0001;
    wr_data = wr_data | (alct_injector_delay << 5);
    status  = vme_write(adr,wr_data);

    // Turn off CFEB cable inputs
    adr     = cfeb_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFE0;
    status  = vme_write(adr,wr_data);

    // Turn on CFEB enables to over-ride mask_all
    adr     = seq_trig_en_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x03FF;     // clear old cfeb_en and source
    wr_data = wr_data | 0x7C00;     // ceb_en_source=0,cfeb_en=1F
    status  = vme_write(adr,wr_data);

    // Select sequencer to take clct ext or alct ext trig
    adr     = seq_trig_en_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFF00;
    wr_data = wr_data | 0x0060; // Select alct or clct ext trig mode
    status  = vme_write(adr,wr_data);

    // Turn on sequencer internal L1A
    adr     = seq_l1a_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data | 0x1000;
    status  = vme_write(adr,wr_data);

    // Fire CCB external clct trigger with pulse generator
    pause("Connect GTL pulse generator to P2A D10, monitor TP382-6");

    // Set start_trigger state for FMM
    adr     = base_adr+ccb_cmd_adr;

    ttc_cmd = 6;            // start_trigger
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);
    wr_data = 0x0003 | (ttc_cmd << 8);
    status  = vme_write(adr,wr_data);
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);

    ttc_cmd = 1;            // bx0
    wr_data = 0x0003 | (ttc_cmd << 8);
    status  = vme_write(adr,wr_data);
    wr_data =0x0001;
    status  = vme_write(adr,wr_data);

    // Arm scope trigger
    scp_arm        = true;
    scp_readout    = false;
    scp_raw_decode = false;
    scp_silent     = false;
    scp_playback   = false;
    if (rdscope)
        scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

    // Read back embedded scope data
    scp_arm        = false;
    scp_readout    = true;
    scp_raw_decode = false;
    scp_silent     = true;
    scp_playback   = false;
    if (rdscope && scp_auto==0)
        scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);


} // close ExternalTriggerALCTCLCTwithGTLpulser()

//------------------------------------------------------------------------------
//  External Trigger ALCT+CLCT with GTL pulser, check CRC
//------------------------------------------------------------------------------
//L96200:
void ExternalALCTCLCTwithGTLpulserCheckCRC () {
    first_event = true;

    if (ifunc<0) {
        rdscope = false;
        rdraw   = false;
    }
    else {
        rdscope = true;
        rdraw   = true;
    }

    // Turn off CCB backplane inputs
    adr     = ccb_cfg_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data | 0x0005;
    status  = vme_write(adr,wr_data);

    // Enable GTL ccb_clct_ext_trig
    adr     = ccb_trig_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data | 0x0040;
    status  = vme_write(adr,wr_data);

    // Turn off ALCT cable inputs
    adr     = alct_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x0000;
    wr_data = wr_data | 0x0001;
    wr_data = wr_data | (alct_injector_delay << 5);
    status  = vme_write(adr,wr_data);

    // Turn off CFEB cable inputs
    adr     = cfeb_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFE0;
    status  = vme_write(adr,wr_data);

    // Turn on CFEB enables to over-ride mask_all
    adr     = seq_trig_en_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x03FF;     // clear old cfeb_en and source
    wr_data = wr_data | 0x7C00;     // ceb_en_source=0,cfeb_en=1F
    status  = vme_write(adr,wr_data);

    // Select sequencer to take clct ext or alct ext trig
    adr     = seq_trig_en_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFF00;
    wr_data = wr_data | 0x0060;     // Select alct or clct ext trig mode
    status  = vme_write(adr,wr_data);

    // Turn on sequencer internal L1A
    adr     = seq_l1a_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data | 0x1000;
    status  = vme_write(adr,wr_data);

    // Set start_trigger state for FMM
    adr     = base_adr+ccb_cmd_adr;

    ttc_cmd = 6;            // start_trigger
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);
    wr_data = 0x0003 | (ttc_cmd << 8);
    status  = vme_write(adr,wr_data);
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);

    ttc_cmd = 1;            // bx0
    wr_data = 0x0003 | (ttc_cmd << 8);
    status  = vme_write(adr,wr_data);
    wr_data =0x0001;
    status  = vme_write(adr,wr_data);

    // Fire CCB external clct trigger with pulse generator
    pause("Connect GTL pulse generator to P2A D10, monitor TP382-6");

    // Event loop option
    //L96210:

    // Arm scope trigger
    scp_arm        = true;
    scp_readout    = false;
    scp_raw_decode = false;
    scp_silent     = false;
    scp_playback   = false;
    if (rdscope)
        scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

    do {            //do{...} while(false) to replace goto
        // Get DMB RAM word count and busy bit
        if (!rdraw)     break; 

        adr       = dmb_wdcnt_adr+base_adr;
        status    = vme_read(adr,rd_data);
        dmb_wdcnt = rd_data & 0x0FFF;
        dmb_busy  = (rd_data>>14) & 0x0001;

        fprintf(stdout,"\tword count = %4i\n",dmb_wdcnt);
        fprintf(stdout,"\tbusy       = %4i\n",dmb_busy );

        if (dmb_busy==1) {
            fprintf(stdout,"\tCan not read RAM: dmb reports busy\n");
            break; 
        }
        if (dmb_wdcnt<=0) {
            fprintf(stdout,"\tCan not read RAM: dmb reports word count <=0");
            break; 
        }

        // Write RAM read address to TMB
        //L96211:
        for (i=0; i<=dmb_wdcnt-1; ++i) {
            adr     = dmb_ram_adr+base_adr;
            wr_data = i & 0xFFFF;
            status  = vme_write(adr,wr_data);

            // Read RAM data from TMB
            adr    = dmb_rdata_adr+base_adr;
            status = vme_read(adr,rd_data);         // read lsbs
            dmb_rdata_lsb = rd_data;

            adr    = dmb_wdcnt_adr+base_adr;
            status = vme_read(adr,rd_data);         // read msbs

            dmb_rdata_msb = (rd_data>>12) & 0x3;    // rdata msbs
            dmb_rdata     = dmb_rdata_lsb | (dmb_rdata_msb<<16);

            vf_data[i]=dmb_rdata;
            fprintf(stdout,"\tAdr=%4i Data=%5.5X\n",i,dmb_rdata);
        }   // close for i

        // Clear RAM address for next event
        adr     = dmb_ram_adr+base_adr;
        wr_data = 0x2000;   // reset RAM write address
        status  = vme_write(adr,wr_data);
        wr_data = 0x0000;   // unreset
        status  = vme_write(adr,wr_data);
    } while(false); //close do{} while().. 

    // Read back embedded scope data
    //L96220:
    scp_arm        = false;
    scp_readout    = true;
    scp_raw_decode = false;
    scp_silent     = true;
    scp_playback   = false;
    if (rdscope && scp_auto==0)
        scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

    // Decode raw hits dump
    decode_readout(vf_data,dmb_wdcnt,err_check=false);

} // close externalALCTCLCTwithGTLpulserCheckCRC

//------------------------------------------------------------------------------
//  Test BXN Counter
//------------------------------------------------------------------------------
void TestBXNCounter() {
    //L96300:
    //bang mode loop
    while(true) { 
        if (ifunc<0) rdscope = false;
        else         rdscope = true;

        // Turn off CCB backplane inputs, turn on L1A emulator
        adr     = ccb_cfg_adr+base_adr;
        wr_data = 0x003D;
        status  = vme_write(adr,wr_data);

        // Enable sequencer trigger, turn off dmb trigger, set internal l1a delay
        adr     = ccb_trig_adr+base_adr;
        wr_data = 0x0004;
        wr_data = wr_data | (114<<8);
        status  = vme_write(adr,wr_data);

        // Turn off ALCT cable inputs, enable synchronized alct+clct triggers
        adr     = alct_inj_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x0000;
        wr_data = wr_data | 0x0005;
        wr_data = wr_data | (alct_injector_delay << 5);
        status  = vme_write(adr,wr_data);

        // Set ALCT delay for TMB matching
        adr     = tmbtim_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFFF0;
        wr_data = wr_data | 0x0003;
        status  = vme_write(adr,wr_data);

        // Turn off CLCT cable inputs
        adr     = cfeb_inj_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFFE0;
        status  = vme_write(adr,wr_data);

        // Turn on CFEB enables to over-ride mask_all
        adr     = seq_trig_en_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x03FF; // clear old cfeb_en and source
        wr_data = wr_data | 0x7C00; // ceb_en_source=0,cfeb_en=1F
        status  = vme_write(adr,wr_data);

        // Turn off internal level 1 accept for sequencer
        adr     = seq_l1a_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x0FFF;
        status  = vme_write(adr,wr_data);

        // Select pattern trigger
        adr     = seq_trig_en_adr+base_adr;
        status  = vme_read(adr,rd_data);
        rd_data = rd_data & 0xFF00;
        wr_data = rd_data | 0x0001;
        status  = vme_write(adr,wr_data);

        // Clear previous ALCT inject
        adr=alct_inj_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFFFD;
        status  = vme_write(adr,wr_data);

        // Set l1reset, which takes FMM to stop trigger
        ttc_cmd = 3;        // l1reset
        adr     = base_adr+ccb_cmd_adr;
        wr_data = 0x0001;
        status  = vme_write(adr,wr_data);
        wr_data = 0x0003 | (ttc_cmd<<8);
        status  = vme_write(adr,wr_data);
        wr_data =0x0001;
        status  = vme_write(adr,wr_data);

        // Check FMM machine is in stop_trigger state after l1reset
        adr     = base_adr+ccb_cmd_adr;
        status  = vme_read(adr,rd_data);
        fmm_state = (rd_data >> 4) & 0x0007;

        if (fmm_state!=2) pause("FMM failed to go to stop_trigger");

        // Set FMM start_trigger (wait for bx0) after l1reset
        ttc_cmd=6;      // start_trigger

        adr     = base_adr+ccb_cmd_adr;
        wr_data = 0x0001;
        status  = vme_write(adr,wr_data);
        wr_data = 0x0003 | (ttc_cmd<<8);
        status  = vme_write(adr,wr_data);
        wr_data = 0x0001;
        status  = vme_write(adr,wr_data);

        // Check FMM machine is waiting for bx0 after start trigger
        adr     = base_adr+ccb_cmd_adr;
        status  = vme_read(adr,rd_data);
        fmm_state = (rd_data>>4) & 0x0007;

        if (fmm_state!=3) pause("FMM failed to go to fmm_wait_bx0");

        // Set FMM to start_trigger by sending bx0
        ttc_cmd = 1;
        adr     = base_adr+ccb_cmd_adr;
        wr_data = 0x0003 | (ttc_cmd<<8);
        status  = vme_write(adr,wr_data);
        wr_data = 0x0001;
        status  = vme_write(adr,wr_data);

        // Check FMM machine is in start_trigger state
        adr     = base_adr+ccb_cmd_adr;
        status  = vme_read(adr,rd_data);
        fmm_state = (rd_data>>4) & 0x0007;

        if (fmm_state!=4) pause("FMM failed to go to start_trigger");

        // Send some triggers, expect bxn stays at 0 1st pass, rarely 0 2nd pass
        ntrig = 100;

        for (ipass=1; ipass<=2; ++ipass)
        {
            nbxn0 = 0;
            for (i=1; i<=ntrig; ++i)
            {
                // Clear previous  CLCT inject
                adr     = cfeb_inj_adr+base_adr;
                status  = vme_read(adr,rd_data);
                wr_data = rd_data & 0x7FFF;
                status  = vme_write(adr,wr_data);

                // Fire CLCT Injector
                wr_data = wr_data | 0x8000;
                status  = vme_write(adr,wr_data);

                // Clear previous inject    
                wr_data = rd_data & 0x7FFF; 
                status  = vme_write(adr,wr_data);

                // Read back latched CLCT bxn
                adr    = seq_clctm_adr+base_adr;
                status = vme_read(adr,rd_data);
                clctc_vme = (rd_data >> 0) & 0x0007;

                // Check bxn is always 0
                clctc_bxn_vme  =  clctc_vme & 0x0003;       // Bunch crossing number

                // Print output
                if (clctc_bxn_vme==0) nbxn0++;
                fprintf(stdout,"\tPass %5i bxn=%5i\n",ipass,clctc_bxn_vme);
            } // close for ntrig

            // Print output
            fprintf(stdout,"\tBXN was 0 %4i/%4i times\n",nbxn0,ntrig);

            // Set FMM bxn after l1reset to resume bxn counting
            ttc_cmd = 1;        // bx0
            adr     = base_adr+ccb_cmd_adr;
            wr_data = 0x0001;
            status  = vme_write(adr,wr_data);
            wr_data = 0x0003 | (ttc_cmd<<8);
            status  = vme_write(adr,wr_data);
            wr_data = 0x0001;
            status  = vme_write(adr,wr_data);

        } // close for  ipass

        if (ifunc<0)        usleep(100000); 
        else                break; 
    } // close bang mode loop
}  // Close TestBXNCounter()

//------------------------------------------------------------------------------
//  Fire L1A-only event check short  or long header readout
//------------------------------------------------------------------------------
int FireL1A() {
    // Get current fifo_mode, l1a_lookback, hdr_wr_continuous, scope
    adr    = base_adr+seq_fifo_adr;
    status = vme_read(adr,rd_data);

    fifo_mode       = (rd_data >> 0) & 0x07;    // 3 bits
    fifo_tbins      = (rd_data >> 3) & 0x1F;    // 5 bits
    fifo_pretrig    = (rd_data >> 8) & 0x1F;    // 5 bits
    if (fifo_tbins==0) fifo_tbins=32;

    adr    = base_adr+l1a_lookback_adr;
    status = vme_read(adr,rd_data);
    l1a_lookback = (rd_data & 0x07FF);

    adr    = base_adr+seqmod_adr;
    status = vme_read(adr,rd_data);
    hdr_wr_continuous = (rd_data >> 5) & 0x1;

    adr    = base_adr+scp_ctrl_adr;
    status = vme_read(adr,rd_data);
    scp_auto   =(rd_data >> 3) & 0x1;           // 1 bit
    scp_nowrite=(rd_data >> 4) & 0x1;           // 1 bit
    scp_tbins  =(rd_data >> 5) & 0x7;           // 3 bits

    // Display CLCT fifo mode reminder
    printf("\n");
    printf("\tfifo_mode=0:  Dump=No    Header=Full \n");
    printf("\tfifo_mode=1:  Dump=Full  Header=Full \n");
    printf("\tfifo_mode=2:  Dump=Local Header=Full \n");
    printf("\tfifo_mode=3:  Dump=No    Header=Short\n");
    printf("\tfifo_mode=4:  Dump=No    Header=No   \n");
    printf("\n");

    // Inquire
    printf("\tFifo_mode           ? cr=%3i",fifo_mode);
    fgets(line, 80, stdin);
    n=strlen(line);
    sscanf(line,"%i",&i);
    if (n!=0) fifo_mode=i;

    printf("\tTbins               ? cr=%3i",fifo_tbins);
    fgets(line, 80, stdin);
    n=strlen(line);
    sscanf(line,"%i",&i);
    if (n!=0) fifo_tbins=i;

    printf("\tTbins before pretrig? cr=%3i",fifo_pretrig);
    fgets(line, 80, stdin);
    n=strlen(line);
    sscanf(line,"%i",&i);
    if (n!=0) fifo_pretrig=i;

    printf("\tL1A_lookback        ? cr=%3i",l1a_lookback);
    fgets(line, 80, stdin);
    n=strlen(line);
    sscanf(line,"%i",&i);
    if (n!=0) l1a_lookback=i;

    printf("\tHdr_wr_continuous   ? cr=%3i",hdr_wr_continuous);
    fgets(line, 80, stdin);
    n=strlen(line);
    sscanf(line,"%i",&i);
    if (n!=0) hdr_wr_continuous=i;

    printf("\tFire CLCT injectors ? cr=%3i",fire_injector);
    fgets(line, 80, stdin);
    n=strlen(line);
    sscanf(line,"%i",&i);
    if (n!=0) fire_injector=true;

    printf("\tScope tbins/64      ? cr=%3i",scp_tbins);
    fgets(line, 80, stdin);
    n=strlen(line);
    sscanf(line,"%i",&i);
    if (n!=0) scp_tbins=i; 

    // Non-inquire defaults
    alct_injector_delay = 14;
    l1a_delay           = 119;
    alct_delay          = 4;
    clct_width          = 3;
    tmb_allow_clct      = 1;
    tmb_allow_match     = 1;
    pid_thresh_pretrig  = 0;
    pid_thresh_postdrift= 0;
    adjcfeb_dist        = 5;
    clct_sep            = 10;
    active_feb_src      = 0;
    rat_sync_mode       = 0;

    triad_persist       = 6;
    dmb_thresh_pretrig  = 1;
    hit_thresh_pretrig  = 1;
    hit_thresh_postdrift= 1;
    drift_delay         = 2;

    // Turn off CFEB cable inputs
    //L96405:
    while(true) { //loop until L1Loopback > 800
        adr     = cfeb_inj_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFFE0;     // mask_all=5'b00000
        status  = vme_write(adr,wr_data);

        // Turn off ALCT cable inputs, enable synchronized alct+clct triggers
        adr     = alct_inj_adr+base_adr;
        status  = vme_read(adr,rd_data);

        wr_data = rd_data & 0x0000;
        wr_data = wr_data | 0x0005;
        wr_data = wr_data | (alct_injector_delay << 5);
        status  = vme_write(adr,wr_data);

        // Turn on CFEB enables to over-ride mask_all
        adr     = seq_trig_en_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x03FF;         // clear old cfeb_en and source
        wr_data = wr_data | 0x7C00;         // ceb_en_source=0,cfeb_en=1F
        status  = vme_write(adr,wr_data);

        // Enable sequencer trigger, turn off dmb trigger, set internal l1a delay
        adr     = ccb_trig_adr+base_adr;
        wr_data = 0x0004;
        wr_data = wr_data | (l1a_delay << 8);
        status  = vme_write(adr,wr_data);

        // Set L1A lookback
        adr     = base_adr+l1a_lookback_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & ~0x07FF;
        wr_data = wr_data | (l1a_lookback << 0);
        status  = vme_write(adr,wr_data);

        // Set readout machine to respond to unexpected L1As
        adr    = seqmod_adr+base_adr;
        status = vme_read(adr,rd_data);     // current ccb reg state

        wr_data = (rd_data | 0x0200);       // turn on l1a_allow_notmb bit 9
        status  = vme_write(adr,wr_data);

        // Set new hdr_wr_continuous
        adr    = base_adr+seqmod_adr;
        status = vme_read(adr,rd_data);
        wr_data = rd_data & ~0x0020;
        wr_data = wr_data | (hdr_wr_continuous << 5);
        status  = vme_write(adr,wr_data);

        // Turn off CCB backplane inputs, turn on L1A emulator, do this after turning off cfeb and alct cable inputs
        adr     = ccb_cfg_adr+base_adr;
        wr_data = 0x0000;
        wr_data = wr_data | 0x0001; // ccb_ignore_rx
        //  wr_data = wr_data | 0x0004; // ccb_int_l1a_en
        wr_data = wr_data | 0x0008; // ccb_status_oe_vme
        wr_data = wr_data | 0x0010; // alct_status_en
        wr_data = wr_data | 0x0020; // clct_status_en
        status  = vme_write(adr,wr_data);

        // Set ALCT delay for TMB matching
        adr     = tmbtim_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFF00;
        wr_data = wr_data | (alct_delay << 0);
        wr_data = wr_data | (clct_width << 4);
        status  = vme_write(adr,wr_data);

        // Set tmb_match mode
        adr     = base_adr+tmb_trig_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFFE7;     // clear bits 4,3
        wr_data = wr_data | (tmb_allow_clct  << 3);
        wr_data = wr_data | (tmb_allow_match << 4);
        status  = vme_write(adr,wr_data);

        // Turn off internal level 1 accept for sequencer, set l1a window width
        adr     = seq_l1a_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x00FF;
        wr_data = wr_data | 0x0300;         //  l1a window width
        status  = vme_write(adr,wr_data);

        // Set fifo tbins
        adr     = base_adr+seq_fifo_adr;
        status  = vme_read(adr,rd_data);    // get current
        wr_data = rd_data & 0xF000;         // clear lower bits

        wr_data=wr_data
            | ((fifo_mode    & 0x07) << 0)      // [2:0]
            | ((fifo_tbins   & 0x1F) << 3)      // [7:3]
            | ((fifo_pretrig & 0x1F) << 8);     // [12:8]

        status = vme_write(adr,wr_data);

        // Set pid_thresh_pretrig, pid_thresh_postdrift
        adr    = temp0_adr+base_adr;
        status = vme_read(adr,rd_data);

        wr_data=rd_data & 0xFC03;
        wr_data=wr_data | (pid_thresh_pretrig   << 2);  // 4 bits
        wr_data=wr_data | (pid_thresh_postdrift << 6);  // 4 bits

        status = vme_write(adr,wr_data);

        // Set adjcfeb_dist
        adr     = temp0_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x03FF;         // adjcfeb_dist[5:0] is in [15:10]
        wr_data = wr_data | (adjcfeb_dist << 10);
        status  = vme_write(adr,wr_data);

        // Set CLCT separation
        adr     = temp1_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0x00FF;
        wr_data = wr_data | (clct_sep << 8);
        status  = vme_write(adr,wr_data);

        // Set active_feb_list source
        adr     = seqmod_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & ~(1 << 14);
        wr_data = wr_data | (active_feb_src << 14);
        status  = vme_write(adr,wr_data);

        // Set RAT out of sync mode
        adr     = vme_ratctrl_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFFFE;
        wr_data = wr_data | rat_sync_mode;
        status  = vme_write(adr,wr_data);

        // Select clct pattern trigger
        adr     = seq_trig_en_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFF00;
        wr_data = wr_data | 0x0001;
        status  = vme_write(adr,wr_data);

        // Set start_trigger state for FMM
        adr     = base_adr+ccb_cmd_adr;

        ttc_cmd = 3;            // l1reset
        wr_data = 0x0003 | (ttc_cmd << 8);
        status  = vme_write(adr,wr_data);
        wr_data = 0x0001;
        status  = vme_write(adr,wr_data);

        ttc_cmd = 6;            // start_trigger
        wr_data = 0x0003 | (ttc_cmd << 8);
        status  = vme_write(adr,wr_data);
        wr_data = 0x0001;
        status  = vme_write(adr,wr_data);

        ttc_cmd = 1;            // bx0
        wr_data = 0x0003 | (ttc_cmd << 8);
        status  = vme_write(adr,wr_data);
        wr_data =0x0001;
        status  = vme_write(adr,wr_data);

        // Lower pattern threshold temporarily so edge key 1/2-strips will trigger, set it back later
        adr    = seq_clct_adr+base_adr;
        status = vme_read(adr,rd_data);

        wr_data = rd_data & 0x8000; // clear hit_thresh,nph_pattern,drift
        wr_data = wr_data | (triad_persist             <<  0);
        wr_data = wr_data | (hit_thresh_pretrig        <<  4);
        wr_data = wr_data | (dmb_thresh_pretrig        <<  7);
        wr_data = wr_data | (hit_thresh_postdrift      << 10);
        wr_data = wr_data | (drift_delay               << 13);
        status  = vme_write(adr,wr_data);

        // Set scope-in-readout
        adr     = base_adr+scp_ctrl_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data & 0xFF07;  // clear bits 3,4,5,6,7
        wr_data = wr_data | (scp_auto    << 3);
        wr_data = wr_data | (scp_nowrite << 4);
        wr_data = wr_data | (scp_tbins   << 5);
        status  = vme_write(adr,wr_data);

        // Arm scope trigger
        scp_arm        = true;
        scp_readout    = false;
        scp_raw_decode = false;
        scp_silent     = false;
        scp_playback   = false;
        if (rdscope)
            scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

        // Prepare to fire L1A
        adr     = ccb_cfg_adr+base_adr;
        status  = vme_read(adr,rd_data);        // current ccb reg state

        wr_data = (rd_data & ~0x0040);          // turn off l1a oneshot bit 6 in case it was on
        status  = vme_write(adr,wr_data);

        wr_fire_l1a = (rd_data | 0x0040);       // ready to fire bit 6 l1a oneshot;

        // Fire CLCT+ALCT Injectors
        fprintf(stdout,"\nFiring injectors for L1A-only event\n");

        adr     = cfeb_inj_adr+base_adr;
        status  = vme_read(adr,rd_data);
        wr_data = rd_data | 0x8000;             // fire injector
        status  = vme_write(adr,wr_data);
        wr_data = rd_data & 0x7FFF;             // unfire
        status  = vme_write(adr,wr_data);

        // Fire L1A
        adr     = ccb_cfg_adr+base_adr;
        wr_data = wr_fire_l1a;
        status  = vme_write(adr,wr_data);       // fire L1A

        status  = vme_read(adr,rd_data);        // current ccb reg state
        wr_data = (rd_data & ~0x0040);          // turn off l1a oneshot
        status  = vme_write(adr,wr_data);

        // Wait for DMB readout to complete
        sleep(1);

        // Get DMB RAM word count and busy bit
        adr    = dmb_wdcnt_adr+base_adr;
        status = vme_read(adr,rd_data);
        dmb_wdcnt = rd_data & 0x0FFF;
        dmb_busy  = (rd_data >> 14) & 0x0001;

        fprintf(stdout,"Raw Hits Dump: L1A-only event\n");
        fprintf(stdout,"word count = %4i\n",dmb_wdcnt);
        fprintf(stdout,"busy       = %4i\n",dmb_busy);

        do {  //do{...} while(false) to replace gotos
            if (dmb_busy) {
                fprintf(stdout,"Can not read RAM: dmb reports busy\n");
                fprintf(stdout,  "Can not read RAM: dmb reports busy\n");
                break; 
            }

            if (dmb_wdcnt <= 0) {
                fprintf(stdout,"Can not read RAM: dmb reports word count <=0\n");
                fprintf(stdout,  "Can not read RAM: dmb reports word count <=0\n");
                break;
            }

            // Write RAM read address to TMB
            for (i=0; i<=dmb_wdcnt-1; ++i) {
                adr     = dmb_ram_adr+base_adr;
                wr_data = i & 0xffff;
                status  = vme_write(adr,wr_data);

                // Read RAM data from TMB
                adr    = dmb_rdata_adr+base_adr;
                status = vme_read(adr,rd_data);             // read lsbs
                dmb_rdata_lsb = rd_data;

                adr    = dmb_wdcnt_adr+base_adr;
                status = vme_read(adr,rd_data);             // read msbs
                dmb_rdata_msb = (rd_data >> 12) & 0x3;      // rdata msbs
                dmb_rdata     = dmb_rdata_lsb | (dmb_rdata_msb << 16);

                vf_data[i]=dmb_rdata;
                fprintf(stdout,"Adr=%5i Data=%6.5X\n",i,dmb_rdata);
            } // close i

            // Clear RAM address for next event
            adr     = dmb_ram_adr+base_adr;
            wr_data = 0x2000;   // reset RAM write address
            status  = vme_write(adr,wr_data);
            wr_data = 0x0000;   // unreset
            status  = vme_write(adr,wr_data);

            // Decode raw hits dump, variables passed by common block struct
            decode_readout(vf_data,dmb_wdcnt,err_check=false);

            // Scanning for non-zero triads
            fprintf(stdout,"\tl1a_lookback=%4i  ",l1a_lookback);
            fprintf(stdout,"Non-zero triad bits=%4i ",nonzero_triads);
            lookback_triad_hits[l1a_lookback%2048]=nonzero_triads;
            if (ifunc<0) l1a_lookback++;

            // Read sequencer Debug register
            for (seqdeb_adr=0; seqdeb_adr<=1; ++seqdeb_adr) {
                adr     = seq_debug_adr+base_adr;
                wr_data = seqdeb_adr;
                status  = vme_write(adr,wr_data);   // write sub adr
                status  = vme_read (adr,rd_data);   // read data
                seqdeb_rd_mux = (rd_data >> 4);     // Extract mux data

                switch (seqdeb_adr) {
                    case 0x0: deb_wr_buf_adr    = seqdeb_rd_mux; break; // [10:0]   Buffer write address at last pretrig
                    case 0x1: deb_buf_push_adr  = seqdeb_rd_mux; break; // [10:0]   Queue push address at last push
                }}
            deb_adr_diff = abs(long(deb_buf_push_adr-deb_wr_buf_adr));

            printf("push_adr-pretrig_adr=%8.4ld\n",deb_adr_diff);

            // Take snapshot of current counter state
        } while(false); //close do{...} while(false) 
        //L96708:

        adr = base_adr+cnt_ctrl_adr;
        wr_data=0x0022; //snap
        status = vme_write(adr,wr_data);
        wr_data=0x0020; //unsnap
        status = vme_write(adr,wr_data);

        // Read counters
        for (i=0; i<mxcounter; ++i) {
            for (j=0; j<=1; ++j) {
                adr = base_adr+cnt_ctrl_adr;
                wr_data=(i << 9) | 0x0020 | (j << 8);
                status = vme_write(adr,wr_data);
                adr = base_adr+cnt_rdata_adr;
                status = vme_read(adr,rd_data);

                // Combine lsbs+msbs
                if (j==0)           // Even addresses contain counter LSBs
                    cnt_lsb=rd_data;
                else {              // Odd addresses contain counter MSBs
                    cnt_msb=rd_data;
                    cnt_full=cnt_lsb | (cnt_msb << 16);
                    cnt[i]=cnt_full;    // Assembled counter MSB,LSB
                }
            }   //close j 
        }       //close i

        // Dislay counters
        for (i=0; i<mxcounter; ++i) {
            fprintf(stdout,"\t%2.2i %10i %s\n",i,cnt[i],scnt[i].c_str());
        }

        // Read back embedded scope data
        scp_arm        = false;
        scp_readout    = true;
        scp_raw_decode = false;
        scp_silent     = true;
        scp_playback   = false;
        if (rdscope && scp_auto==0)
            scope160c(base_adr,scp_ctrl_adr,scp_rdata_adr,scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_playback,scp_raw_data);

        // Bang mode
        if (ifunc >= 0)
            return EXIT_SUCCESS; 
        //goto L1600;

        sleep(1);

        // End of bang scan, look for nonzero triads
        //  if (l1a_lookback < 2048) goto L96405;   // Full range of lookbacks
        if (l1a_lookback >  800)   break;           // Shorten cuz we know its in range of 600 to 700

        first_nonzero_bx = 0;
        last_nonzero_bx  = 0;
        max_triads       = 0;

        for (i=0; i<=2047; ++i) {
            if (lookback_triad_hits[i] !=0) {
                if (first_nonzero_bx==0) first_nonzero_bx=i;
                if (lookback_triad_hits[i] > max_triads) max_triads=lookback_triad_hits[i];
                last_nonzero_bx = i;
            } //close loopback_triad_hits
        } //close i

        if (first_nonzero_bx!=0) {
            printf("\n");
            printf("\tfirst nonzero triad at l1a_lookback = %i\n",first_nonzero_bx);
            printf("\tlast  nonzero triad at l1a_lookback = %i\n",last_nonzero_bx);
            printf("\tpeak triad bits                     = %i\n",max_triads);
            printf("\n");
            pause ("<cr> to finish display");
        }
        else {
            printf("\tOh noes! I can has no triads =:-(.\n");
            return EXIT_FAILURE; 
        }

        // Display lookback values that have nonzero triads
        first_bx = max(first_nonzero_bx-10,0);
        last_bx  = min(last_nonzero_bx+10,2047);
        scale    = 50./float(max_triads);

        for (i=first_bx; i<= last_bx; ++i) {
            printf("lookback%4ibx %3i triads|",i,lookback_triad_hits[i]);
            if (lookback_triad_hits[i]>0) for (j=0; j<=lookback_triad_hits[i]*scale; ++j) printf("x");
            printf("\n");
        }
        // Bang mode
        if (ifunc < 0)      usleep(1500000);
        else                break; 
    } //close Bang Mode loop
    return 0; 
} //close FireL1A()


//------------------------------------------------------------------------------
//  Forced CLCT trigger and readout
//------------------------------------------------------------------------------
int ForceCLCTtriggerandReadout() {
    icfeb=1;
    display_cfeb=true;

    // Get current cfeb tbins
    adr    = base_adr+seq_fifo_adr;
    status = vme_read(adr,rd_data);

    fifo_mode       = (rd_data >> 0) & 0x07;    // 3 bits
    fifo_tbins      = (rd_data >> 3) & 0x1F;    // 5 bits
    fifo_pretrig    = (rd_data >> 8) & 0x1F;    // 5 bits
    if (fifo_tbins==0) fifo_tbins=32;

    // Turn off CCB backplane inputs, turn on L1A emulator
    adr     = ccb_cfg_adr+base_adr;
    wr_data = 0x003D;
    status  = vme_write(adr,wr_data);

    // Enable l1a on sequencer trigger, turn off dmb trigger, set internal l1a delay
    adr     = ccb_trig_adr+base_adr;
    wr_data = 0x0004;
    wr_data = wr_data | (l1a_delay << 8);
    status  = vme_write(adr,wr_data);

    // Turn off ALCT cable inputs, enable synchronized alct+clct triggers
    adr     = alct_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x0000;
    wr_data = wr_data | 0x0005;
    wr_data = wr_data | (alct_injector_delay << 5);
    status  = vme_write(adr,wr_data);

    // Set ALCT delay for TMB matching
    adr     = tmbtim_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFF0;
    wr_data = wr_data | 0x0003;
    status  = vme_write(adr,wr_data);

    // Turn on all CFEB inputs so we can check for crosstalk
    adr     = cfeb_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFE0;
    wr_data = wr_data | 0x001F;
    status  = vme_write(adr,wr_data);

    // Turn on CFEB enables to over-ride mask_all
    adr     = seq_trig_en_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x03FF;     // clear old cfeb_en and source
    wr_data = wr_data | 0x7C00;     // ceb_en_source=0,cfeb_en=1F
    status  = vme_write(adr,wr_data);

    // Turn off internal level 1 accept for sequencer, set l1a window width
    adr     = seq_l1a_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0x00FF;
    wr_data = wr_data | 0x0300;     // l1a window width
    status  = vme_write(adr,wr_data);

    // Turn off CLCT pattern trigger
    adr     = seq_trig_en_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFF00;
    status  = vme_write(adr,wr_data);

    // Clear previous ALCT inject
    adr     = alct_inj_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFFFD;
    status  = vme_write(adr,wr_data);

    // Set start_trigger state for FMM
    ttc_cmd = 6;            // start_trigger
    adr     = base_adr+ccb_cmd_adr;
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);
    wr_data = 0x0003 | (ttc_cmd << 8);
    status  = vme_write(adr,wr_data);
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);

    ttc_cmd = 1;            // bx0
    wr_data = 0x0003 | (ttc_cmd << 8);
    status  = vme_write(adr,wr_data);
    wr_data = 0x0001;
    status  = vme_write(adr,wr_data);

    // Clear DMB RAM write-address
    adr     = dmb_ram_adr+base_adr;
    wr_data = 0x2000;   //reset RAM write address
    status  = vme_write(adr,wr_data);
    wr_data = 0x0000;   // unreset
    status  = vme_write(adr,wr_data);

    // Fire VME trigger
    adr     = seq_trig_en_adr+base_adr;
    status  = vme_read(adr,rd_data);
    wr_data = rd_data & 0xFF00;
    wr_data = wr_data | (1 << 7);   // fire vme trigger
    status  = vme_write(adr,wr_data);
    wr_data = rd_data & 0xFF00;     // unfire vme trigger
    status  = vme_write(adr,wr_data);

    // Wait for TMB to read out to DMB
    sleep(1);

    // Get DMB RAM word count and busy bit
    adr       = dmb_wdcnt_adr+base_adr;
    status    = vme_read(adr,rd_data);
    dmb_wdcnt = rd_data & 0x0FFF;
    dmb_busy  = (rd_data >> 14) & 0x0001;

    printf("\tdmb word count = %4i\n",dmb_wdcnt);
    printf("\tdmb busy       = %4i\n",dmb_busy);

    if (dmb_busy  != 0) pause ("Can not read RAM: dmb reports busy");
    if (dmb_wdcnt <= 0) pause ("Can not read RAM: dmb reports word count <=0");

    pause("<cr> to process dump");

    // Write RAM read address to TMB
    for (iadr=0; iadr<=dmb_wdcnt-1; ++iadr) {
        adr     = dmb_ram_adr+base_adr;
        wr_data = iadr & 0xFFFF;
        status  = vme_write(adr,wr_data);

        // Read RAM data from TMB
        adr    = dmb_rdata_adr+base_adr;
        status = vme_read(adr,rd_data);         // read lsbs
        dmb_rdata_lsb=rd_data;

        adr    = dmb_wdcnt_adr+base_adr;
        status = vme_read(adr,rd_data);         // read msbs
        dmb_rdata_msb = (rd_data >> 12) & 0x3;  // rdata msbs

        dmb_rdata = dmb_rdata_lsb | (dmb_rdata_msb << 16);
        vf_data[iadr]=dmb_rdata;

        fprintf(stdout,"Adr=%4i Data=%5.5X\n",iadr,dmb_rdata);
    }   // close iadr

    // Clear RAM address for next event
    adr     = dmb_ram_adr+base_adr;
    wr_data = 0x2000;   // reset RAM write address
    status  = vme_write(adr,wr_data);
    wr_data = 0x0000;   // unreset
    status  = vme_write(adr,wr_data);

    // Point to start of CFEB data
    iframe     = 5;
    r_nheaders = vf_data[iframe] & 0x3F;                    // Number of header words
    adr_e0b    = r_nheaders;

    fprintf(stdout,"r_nheaders=%i\n",r_nheaders);
    if (adr_e0b <=0) pause ("Unreasonable nheaders");

    iframe=19;
    r_ncfebs     =  (vf_data[iframe] >>  0) & 0x7;      // Number of CFEBs read out
    r_fifo_tbins =  (vf_data[iframe] >>  3) & 0x1F;     // Number of time bins per CFEB in dump


    fprintf(stdout,"r_fifo_tbins=%i\n",r_fifo_tbins);
    if (r_fifo_tbins<=0) pause ("Unreasonable ntbins");

    // Copy triad bits to a holding array
    iframe = adr_e0b+1;                                     // First raw hits frame for cfeb0
    //  iframe = iframe + icfeb*(r_fifo_tbins*6);               // First frame for icfeb

    for (jcfeb  = 0; jcfeb  <= r_ncfebs-1;     ++jcfeb ) {  // Loop over all cfebs so we can see crosstalk
        for (itbin  = 0; itbin  <= r_fifo_tbins-1; ++itbin ) {  // Loop over time bins
            for (ilayer = 0; ilayer <= mxly-1;         ++ilayer) {  // Loop over layers

                rdcid  = (vf_data[iframe] >> 12) & 0x7;                 // CFEB ID in the dump
                rdtbin = (vf_data[iframe] >>  8) & 0xF;                 // Tbin number in the dump
                hits8  =  vf_data[iframe]        & 0xFF;                // 8 triad block

                for (ids=0; ids< mxds; ++ids) {                         // Loop over hits per block
                    hits1=(hits8 >> ids) & 0x1;                             // Extract 1 hit
                    ids_abs=ids+jcfeb*8;                                    // Absolute distrip id
                    read_pat[itbin][ilayer][ids_abs]=hits1;                 // hit this distrip
                    if (hits1 != 0) nonzero_triads++;                       // Count nonzero triads
                    fprintf(stdout,"iframe=%4i vf_data=%5.5X hits8=%i jcfeb=%i itbin=%i ids_abs=%i hits1=%i\n",iframe,vf_data[iframe],hits8,jcfeb,itbin,ids_abs,hits1);
                }                                                       // Close ids
                iframe++;                                               // Next frame
            }                                                       // Close for ilayer
        }                                                       // Close for itbin
    }                                                       // Close for jcfeb

    // Display cfeb and ids column markers
    if (display_cfeb) {
        fprintf(stdout,"\n");
        fprintf(stdout,"     Raw Hits Triads\n");
        fprintf(stdout,"Cfeb-");
        for (jcfeb=0; jcfeb<mxcfeb; ++jcfeb) { 
            fprintf(stdout,"|"); // display cfeb columms
            for (ids=0; ids<mxds; ++ids)   
                fprintf(stdout,"%1.1i",jcfeb);
        } //close jcfeb
        fprintf(stdout,"|\n");
        fprintf(stdout,"Ds---");
        for (jcfeb=0; jcfeb < mxcfeb; ++jcfeb) { 
            fprintf(stdout,"|");    // display ids columns
            for (ids=0; ids<mxds; ++ids)
                fprintf(stdout,"%1.1i",ids%10);
        }//close jcfeb
        fprintf(stdout,"|\n");
        fprintf(stdout,"Ly Tb\n");

        // Display CFEB raw hits
        for (ilayer=0; ilayer <= mxly-1; ++ilayer){
            for (itbin=0;  itbin  <= r_fifo_tbins-1; ++itbin ) {
                fprintf(stdout,"%1i %2i ",ilayer,itbin);
                for (ids_abs=0;ids_abs<=39;++ids_abs) {
                    if (ids_abs%8==0) {
                        fprintf(stdout,"|");
                    } //close if
                    fprintf(stdout,"%1.1i",read_pat[itbin][ilayer][ids_abs]);
                }   // close for ids_abs
                fprintf(stdout,"|\n");
            }   // close for itbin
            fprintf(stdout,"\n");
        }   // close ilayer
    }   // close display cfeb
    return 0; 
} //close ForceCLCTtriggerandReadout


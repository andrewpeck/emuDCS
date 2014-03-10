#ifndef SCOPE160C_H
#define SCOPE160C_H

//------------------------------------------------------------------------------
//	TMB Internal Scope Readout
//------------------------------------------------------------------------------

void scope160c (
        unsigned long	 scp_ctrl_adr,
        unsigned long	 scp_rdata_adr,
        int				 scp_arm,
        int				 scp_readout,
        int				 scp_raw_decode,
        int				 scp_silent,
        int				 scp_playback,
        int				 scp_raw_data[512*160/16]
        ); 
#endif

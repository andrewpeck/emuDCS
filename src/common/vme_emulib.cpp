//------------------------------------------------------------------------------
// Headers
//------------------------------------------------------------------------------
#include <cstdlib>                      
#include "emu/pc/vme_emulib.h"
#include "emu/pc/TMB.h"

namespace emu {
	namespace pc {

//------------------------------------------------------------------------------
// vme_write:   Wrapper around emuLIB native TMB write register. Handles type 
//              conversion of addresses and data. 
//              TMB.cc places hard limit on data word size: 
//                  (data_to_write < 0x10000)
//------------------------------------------------------------------------------
long int vme_write(unsigned long &adr, unsigned short &wr_data) {
	//int reg = static_cast<int>(adr);                    //typecast long adr to int
	//int value = static_cast<int>(wr_data);              //typecast short wr_data to int
	//TMB::WriteRegister(int reg, int value);         //write to VME register using emuLib
	return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------
// vme_read:    Wrapper around emuLIB native TMB write register. Handles type 
//              conversion of addresses and data. 
//------------------------------------------------------------------------------
long int vme_read(unsigned long &adr, unsigned short &rd_data) {
	//int reg = static_cast<int>(adr);                    //typcast long adr to int
	//rd_data = (unsigned short) TMB::ReadRegister(reg);  //read VME register using emuLib 
	return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------
// dummy functions
//------------------------------------------------------------------------------
long int vme_open() { return EXIT_SUCCESS; };
long int vme_bwrite(unsigned long &adr, unsigned short wr_data[], long &nwords) { return EXIT_SUCCESS; }
long int vme_bread(unsigned long &adr, unsigned short rd_data[], long &nwords)   { return EXIT_SUCCESS; }
long int vme_sysreset() { return EXIT_SUCCESS; }
long int vme_close() { return EXIT_SUCCESS; };
long int vme_errs(const int &print_mode) { return EXIT_SUCCESS; };

}
} //close namespace 

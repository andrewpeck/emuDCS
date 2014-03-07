#ifndef VME_EMULIB_H
#define VME_EMULIB_H

long int vme_open(); 
long int vme_write(unsigned long &adr, unsigned short &wr_data);
long int vme_read(unsigned long &adr, unsigned short &rd_data);
long int vme_bwrite(unsigned long &adr, unsigned short wr_data[], long &nwords);
long int vme_bread(unsigned long &adr, unsigned short rd_data[], long &nwords);
long int vme_sysreset();
long int vme_close();
long int vme_errs(const int &print_mode);

#endif

#include <eemem.h>
#include <config.h>

unsigned long  mlIpEEP = IP(192,168,100,20);
unsigned long  mlNetmaskEEP = IP(255,255,255,0);
unsigned long  mlGatewayEEP = IP(0,0,0,0);
unsigned long  mlDNSserverEEP = IP(0,0,0,0); //0x0302a8c0;

#ifdef FIXED_TFTP_SRV
unsigned long  mlTFTPipEEP = IP(192,168,100,1);
#endif

//************
// remember to update TFTPReqStrSize in eemem.h if you ever change this
TFTPREQ maTFTPReqStr  = {0x0100, CONFIG_REQ_FILENAME_3SYM"\0octet"};
//************
// remember to update TFTPErrStrSize in eemem.h if you ever change this
TFTPERR maTFTPErrStr  = {0x0500, 0x0500, "Sorry, wasn't talking to you!"};

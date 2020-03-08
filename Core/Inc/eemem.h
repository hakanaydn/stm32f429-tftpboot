#ifndef _EEMEM_H_
#define _EEMEM_H_

extern unsigned long  mlIpEEP;
extern unsigned long  mlNetmaskEEP;
extern unsigned long  mlGatewayEEP;
extern unsigned long  mlDNSserverEEP;

#ifdef FIXED_TFTP_SRV
extern unsigned long mlTFTPipEEP;
#endif

typedef struct 
{
	unsigned short opcode;
	unsigned char fnandmode[100];
} TFTPREQ;

extern TFTPREQ maTFTPReqStr;
#define TFTPReqStrSize 		16		// stupid compiler does not want to calculate 
									// sizeof(maTFTPReqStr), has to be done by hand 

typedef struct 
{
	unsigned short opcode;
	unsigned short errcode;
	unsigned char errstring[100];
} TFTPERR;
extern TFTPERR maTFTPErrStr;
#define TFTPErrStrSize 		34		// stupid compiler does not want to calculate 
									// sizeof(maTFTPErrStr), has to be done by hand 

#if USE_ENC28J60	
extern unsigned char enc28j60_config[];
#define ENC28J60_CONFIG_OFFSET_MAC 33
#endif

#endif

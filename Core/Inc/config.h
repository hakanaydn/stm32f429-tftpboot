#ifndef _CONFIG_H_
#define _CONFIG_H_

#define	MYMAC1 0xAA
#define	MYMAC2 0xAA
#define	MYMAC3 0xAA
#define	MYMAC4 0xBB
#define	MYMAC5 0xBB
#define	MYMAC6 0xBC

//#define CONFIG_REQ_FILENAME_3SYM "hakan.txt"
#define CONFIG_REQ_FILENAME_3SYM "hakan.bin"

//convert config ip to long:
#define IP(a0,a1,a2,a3) ((((unsigned long)(a3))<<24)|(((unsigned long)(a2))<<16)|(((unsigned long)(a1))<<8)|(unsigned long)(a0))

// BOOTLOADER_FLAVOR selects the version to build (see docs)
// allowed values are:  SMALL, MEDIUM, LARGE

#define BL_VERSION_SMALL  1
#define BL_VERSION_MEDIUM 2
#define BL_VERSION_LARGE  3

// makefile will pass the definition thru the compiler
// #define BOOTLOADER_FLAVOR BL_VERSION_SMALL

#if BOOTLOADER_FLAVOR >= BL_VERSION_MEDIUM
  #define USE_DHCP
  #define DHCP_PARSE_TFTP_PARAMS
#else
  #define FIXED_TFTP_SRV
#endif


#define lo8(x) ((x   )&0xFF)
#define hi8(x) (((x)>>8)&0xFF)

#define MTU_SIZE 1200

#define USE_ENC28J60	1

#endif

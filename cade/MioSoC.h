/*
 *  Copyright (C) 2005-2007 Jiri Slaby <jirislaby@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#ifndef __CADE6104A_H
#define __CADE6104A_H

#include <linux/types.h>


#define  MIO_DEBUG(...) do{printk(KERN_INFO "[%s:%d]\t",__FILE__,__LINE__);printk(__VA_ARGS__);} while(0)
#define  MIO_ERROR(...) do{printk(KERN_ERR "[%s:%d]\t",__FILE__,__LINE__);printk(__VA_ARGS__);} while(0)

struct Para_EnableMioChannel {
    int iSetNum;
    int iSlave0Flag;
    int iSlave1Flag;
    int iSlave2Flag;
};

struct Para_WriteLioStatus {
    int iNum;
    int iValue;
};

struct Para_WriteLio2Status {
    int iNum;
    int iValue;
};

struct Para_DisableLdiInt {
    int iNum;
    int iAxis;
};

struct Para_StartDda {
    int iBaseAddress;
    int iMiniSec;
};

struct Para_MioSoCinit {
    int iCardType;
    int iMiniSec;
    int iAxis;
};

//Motion Code Ver: 93.12.23(5)
//void	SetPage(int iPage);
//int	GetPage(void);
//extern int giBaseAddress;
#define	SetPage(iPage) outw((u16)iPage, pmio->ioaddr+15*2  )
#define GetPage()      inw( pmio->ioaddr+15*2 )
#define	SetFpgaPage(iPage) outw((u16)iPage ,pmio->ioaddr + 0x20 +0x1e )


#define INT_RISE_EDGE       0x00
#define INT_FALL_EDGE       0x01
#define INT_LEVEL_HIGH      0x02
#define INT_LEVEL_LOW       0x03


//
//#define	IRQ 		5
#define	MAX_REMOTE_SLAVE  3
#define	MAX_REMOTE_SET  2
#define	MAX_ENCODER	9					   //最大Encoder数
#define	MAX_LDI_INT  	8
#define	MAX_FPGA_LDO_NUM  	32
#define	ON		1
#define MAX_AXIS 6
#define MIO_IOC_MAGIC		'p'

#define MioSoCinit             _IOW(MIO_IOC_MAGIC, 99 , int *)
#define GetMotionCardType  	   _IOR(MIO_IOC_MAGIC, 46, int *) //int 	GetMotionCardType(int iFpgaBaseAddress);
#define ResetAllModules	       _IO(MIO_IOC_MAGIC, 9 )      //void	ResetAllModules( int iBaseAddress );
#define SetInterruptChannel	   _IO(MIO_IOC_MAGIC, 7 )      //void	SetInterruptChannel( int iBaseAddress );
#define SetIntMode        	   _IO(MIO_IOC_MAGIC, 34)   //int	SetIntMode(unsigned short mode);
#define SetAxisCounter	       _IO(MIO_IOC_MAGIC, 5 )    //void	SetAxisCounter( int iBaseAddress );
#define EncoderFilter	       _IO(MIO_IOC_MAGIC, 11)    //void	EncoderFilter(int iBaseAddress);
#define InterruptLineInit      _IO(MIO_IOC_MAGIC, 74)
#define DdaUnlatch        	   _IO(MIO_IOC_MAGIC, 24 )  //int 	DdaUnlatch(void);
#define SetIntClear       	   _IO(MIO_IOC_MAGIC, 38)   //int	SetIntClear();
#define EnableErrCounter	   _IO(MIO_IOC_MAGIC, 12 )  //void	EnableErrCounter(int iBaseAddress);
#define EnableDAPort	       _IO(MIO_IOC_MAGIC, 13 )  //void	EnableDAPort(int iBaseAddress);
#define ClearAbsoluteCounter   _IOW(MIO_IOC_MAGIC, 15, int *)//int	ClearAbsoluteCounter(int);
#define EnableMioChannel	   _IOW(MIO_IOC_MAGIC, 10, struct Para_EnableMioChannel *)//int 	EnableMioChannel(int iSetNum, int iSlave0Flag, int iSlave1Flag, int iSlave2Flag);
#define SetABPhase	           _IO(MIO_IOC_MAGIC, 4 )   //void	SetABPhase( int iBaseAddress );
#define StartDda	           _IOW(MIO_IOC_MAGIC, 8, int *)//void	StartDda( int iBaseAddress, int iMiniSec );
#define GetCardAxisNum    	   _IOR(MIO_IOC_MAGIC, 47, int * )//int	GetCardAxisNum(int iFpgaBaseAddress);

#define DisableIndexInt    	   _IOW(MIO_IOC_MAGIC, 25, int )//int 	DisableIndexInt(int iAxis);
#define DisableLdiInt          _IOW(MIO_IOC_MAGIC, 52, struct Para_DisableLdiInt *)//int DisableLdiInt(int iNum, int iAxis);
#define WriteLioStatus    	   _IOW(MIO_IOC_MAGIC, 37, struct Para_WriteLioStatus *)//int	WriteLioStatus(int iNum, int iValue);
#define WriteLio2Status        _IOW(MIO_IOC_MAGIC, 64, struct Para_WriteLio2Status *)//int WriteLio2Status(int iNum, int iValue);
#define ResetIRQNo        	   _IOW(MIO_IOC_MAGIC, 35,int )//int	ResetIRQNo();


int 	ReadIntSourceFunc(void);

/*
#define IOCTL_SetTriggerMode	       _IOWR(MIO_IOC_MAGIC, 6, struct phm_regs *)//void	SetTriggerMode();
#define IOCTL_SetAxisGain   	       _IOWR(MIO_IOC_MAGIC, 14, struct phm_regs *)//void	SetAxisGain(int iAxis,int iGain1,int iGain2);
#define IOCTL_CheckHardware     	   _IOWR(MIO_IOC_MAGIC, 16, struct phm_regs *)//int	CheckHardware(void);
#define IOCTL_NewGetPosition    	   _IOWR(MIO_IOC_MAGIC, 17, struct phm_regs *)//long	NewGetPosition(int iAxis);
#define IOCTL_OutputPulse       	   _IOWR(MIO_IOC_MAGIC, 18, struct phm_regs *)//void	OutputPulse(int iAxis , short sPulse);
#define IOCTL_GetPosition       	   _IOWR(MIO_IOC_MAGIC, 19, struct phm_regs *)//long	GetPosition( int iAxis );
#define IOCTL_ServoLag          	   _IOWR(MIO_IOC_MAGIC, 20, struct phm_regs *)//long	ServoLag(int iAxis);
#define IOCTL_IRead             	   _IOWR(MIO_IOC_MAGIC, 21, struct phm_regs *)//int	IRead(int,int);
#define IOCTL_ReadIntSource     	   _IOWR(MIO_IOC_MAGIC, 22, struct phm_regs *)//int 	ReadIntSource(void);

#define IOCTL_ENCODERUnlatch    	   _IOWR(MIO_IOC_MAGIC, 23, struct phm_regs *)//long	ENCODERUnlatch(int iGroup);
#define IOCTL_EnableIndexInt	       _IOWR(MIO_IOC_MAGIC, 26, struct phm_regs *)//int  EnableIndexInt(int iAxis);
#define IOCTL_EnableIndexLatch	       _IOWR(MIO_IOC_MAGIC, 27, struct phm_regs *)//int  	EnableIndexLatch(int iAxis);
#define IOCTL_OutputDAValue     	   _IOWR(MIO_IOC_MAGIC, 28, struct phm_regs *)//int	OutputDAValue(int iAxis , double dDAValue);
#define IOCTL_OWrite            	   _IOWR(MIO_IOC_MAGIC, 29, struct phm_regs *)//int	OWrite( int iSetNum,int iONum, short sValue );
#define IOCTL_SetEncoderMultiplier	   _IOWR(MIO_IOC_MAGIC, 30, struct phm_regs *)//int	SetEncoderMultiplier(int iAxis, int iType,int iDir,int iMR);
#define IOCTL_SetPulseTypeAndWidth	   _IOWR(MIO_IOC_MAGIC, 31, struct phm_regs *)//int	SetPulseTypeAndWidth(int iAxis, int iType, int iDir, float fPulseWidth);

#define IOCTL_ReadFIFOStock     	   _IOWR(MIO_IOC_MAGIC, 32, struct phm_regs *)//int	ReadFIFOStock(short *sStock01, short *sStock23, short *sStock45);
#define IOCTL_GetLioStatus      	   _IOWR(MIO_IOC_MAGIC, 33, struct phm_regs *)//char	GetLioStatus(int iNum);
#define IOCTL_SetLDIOrLDO       	   _IOWR(MIO_IOC_MAGIC, 36, struct phm_regs *)//int	SetLDIOrLDO(int iSet, int iVaule);

#define IOCTL_ClearErrCounter   	   _IOWR(MIO_IOC_MAGIC, 39, struct phm_regs *)//int	ClearErrCounter(int iAxis);
#define IOCTL_GetFIFOStockNum   	   _IOWR(MIO_IOC_MAGIC, 40, struct phm_regs *)//int	GetFIFOStockNum(int iAxis);
#define IOCTL_EraseFIFOStockPulse	   _IOWR(MIO_IOC_MAGIC, 41, struct phm_regs *)//int	EraseFIFOStockPulse(int iAxis);

#define IOCTL_SetFpgaPage              _IOWR(MIO_IOC_MAGIC, 42, struct phm_regs *)//void	SetFpgaPage(int iPage);
#define IOCTL_GetFpgaPage       	   _IOWR(MIO_IOC_MAGIC, 43, struct phm_regs *)//int	GetFpgaPage(void);
#define IOCTL_GetFpgaLioStatus   	   _IOWR(MIO_IOC_MAGIC, 44, struct phm_regs *)//char	GetFpgaLioStatus(int iNum);
#define IOCTL_GetMultiCardNum   	   _IOWR(MIO_IOC_MAGIC, 45, struct phm_regs *)//int	GetMultiCardNum(int iFpgaBaseAddress);

#define IOCTL_SetEncoderComparatorValue	   _IOWR(MIO_IOC_MAGIC, 48, struct phm_regs *)//int	SetEncoderComparatorValue(int iAxis, long lComPos);
#define IOCTL_EnableEncoderComparatorInt	   _IOWR(MIO_IOC_MAGIC, 49, struct phm_regs *)//int	EnableEncoderComparatorInt(int iAxis);
#define IOCTL_DisableEncoderComparatorInt	   _IOWR(MIO_IOC_MAGIC, 50, struct phm_regs *)//int	DisableEncoderComparatorInt(int iAxis);
#define IOCTL_EnableLdiInt      	   _IOWR(MIO_IOC_MAGIC, 51, struct phm_regs *)//int EnableLdiInt(int iNum, int iCase, int iAxis);


#define IOCTL_LdiDfiIntUnlatch          	   _IOWR(MIO_IOC_MAGIC, 53, struct phm_regs *)//int LdiDfiIntUnlatch(void);
#define IOCTL_EnableLdiIntLatch         	   _IOWR(MIO_IOC_MAGIC, 54, struct phm_regs *)//int EnableLdiIntLatch(int iNum, int iAxis);
#define IOCTL_EnableRemoteInputInt      	   _IOWR(MIO_IOC_MAGIC, 55, struct phm_regs *)//int EnableRemoteInputInt(int iSet, int iSlave, int iNum, int iCase, int iAxis);
#define IOCTL_DisableRemoteInputInt     	   _IOWR(MIO_IOC_MAGIC, 56, struct phm_regs *)//int DisableRemoteInputInt(int iSet, int iSlave, int iNum, int iAxis);
#define IOCTL_EnableRemoteInputIntLatch  	   _IOWR(MIO_IOC_MAGIC, 57, struct phm_regs *)//int EnableRemoteInputIntLatch(int iSet, int iSlave, int iNum, int iAxis);

#define IOCTL_RemoteInputIntUnlatch     	   _IOWR(MIO_IOC_MAGIC, 58, struct phm_regs *)//int RemoteInputIntUnlatch(int iSet);
#define IOCTL_EnableRemoteInputOutput   	   _IOWR(MIO_IOC_MAGIC, 59, struct phm_regs *)//int EnableRemoteInputOutput(int iSetNum, int iSlave0Flag, int iSlave1Flag, int iSlave2Flag);
#define IOCTL_DisableRemoteInputOutput  	   _IOWR(MIO_IOC_MAGIC, 60, struct phm_regs *)//int DisableRemoteInputOutput(int iSetNum);
#define IOCTL_GetRemoteSetStatus        	   _IOWR(MIO_IOC_MAGIC, 61, struct phm_regs *)//int GetRemoteSetStatus(int iSetNum, int iCase);
#define IOCTL_SetDacSource              	   _IOWR(MIO_IOC_MAGIC, 62, struct phm_regs *)//int SetDacSource(int iDaNum, int iSource);

#define IOCTL_IRead16                    	   _IOWR(MIO_IOC_MAGIC, 63, struct phm_regs *)//int IRead16(int iSetNum, int iWordNum ,int *iOutput);


#define IOCTL_StopDDAOutput             	   _IOWR(MIO_IOC_MAGIC, 65, struct phm_regs *)//int StopDDAOutput(int iAxis);
#define IOCTL_RestartDDAOutput          	   _IOWR(MIO_IOC_MAGIC, 66, struct phm_regs *)//int RestartDDAOutput(int iAxis);
#define IOCTL_StopVcmdOutput            	   _IOWR(MIO_IOC_MAGIC, 67, struct phm_regs *)//int StopVcmdOutput(int iAxis);
#define IOCTL_RestartVcmdOutput         	   _IOWR(MIO_IOC_MAGIC, 68, struct phm_regs *)//int RestartVcmdOutput(int iAxis);
#define IOCTL_GetBreakLineStatus        	   _IOWR(MIO_IOC_MAGIC, 69, struct phm_regs *)//int GetBreakLineStatus(void);

#define IOCTL_SetBreakLine              	   _IOWR(MIO_IOC_MAGIC, 70, struct phm_regs *)//int SetBreakLine(int iAxis, int iOnOff);
#define IOCTL_DdaInterrupt              	   _IOWR(MIO_IOC_MAGIC, 71, struct phm_regs *)//int DdaInterrupt(int iOnOff);
#define IOCTL_EnableErrCounterInt       	   _IOWR(MIO_IOC_MAGIC, 72, struct phm_regs *)//int EnableErrCounterInt(int iAxis, int iOnOff);
#define IOCTL_ErrorCounterUnlatch       	   _IOWR(MIO_IOC_MAGIC, 73, struct phm_regs *)//int ErrorCounterUnlatch(void);
*/
#endif



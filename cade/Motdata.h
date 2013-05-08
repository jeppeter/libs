//==============================全域变量之数据===================================
unsigned short sWriteLioState1;	//WriteLioStatus
unsigned short sWriteLioState2;	//WriteLioStatus

unsigned short sWriteLio2State1;	//WriteLio2Status

unsigned short sWriteLio2State2;	//WriteLio2Status

//===================================Remote Input Latch之资料=================================
unsigned int	giEnableRemoteSlave[MAX_REMOTE_SET][MAX_REMOTE_SLAVE];			//Enable Remote Slave [0~1 Set][0~2 Slave]

//===================================Trigger Counter Latch之资料=================================
unsigned int	giEnableTriggerCounterLatch[MAX_ENCODER];			//Enable Trigger Counter Latch用

//===================================回原点之资料=================================
int	giEnableIndexAxes;					//	EnableIndexInt()用

//===================================Encoder Latch之资料=================================
unsigned int	giEnableEncoderComparatorAxes;					//	EnableEncoderComparatorInt()用

//===================================Local Input Latch之资料=================================
unsigned int	giEnableLdiIntAxes;					//	EnableLdiInt()用

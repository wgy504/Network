//ICC-AVR application builder : 2007-8-11 20:37:32
// Target : M16
// Crystal: 8.0000Mhz
#include <iom16v.h>
#include <macros.h>
//------------------------------------------------------------------------------
#define uint  unsigned int
#define uchar unsigned char
//------------------------------AD变量------------------------------------------
uchar  AD_num,AD_data[4];
unsigned char table[]={0,1,2,3,4,5,6,7,8,9};
//------------------------------------------------------------------------------
uchar seg[10]={0xC0,0xCF,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};         //0~~9段码
uchar seg1[10]={0x40,0x4F,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
//******************************************************************************
//***********************NRF905端口设置*****************************************
//******************************************************************************
///////////////////////////模式控制定义/////////////////////////////////
//#define  nrf905_TX_EN	  PB0	    //输出1
#define   Hign_nrf905_TX_EN    PORTB |= (1 << PB0);   
#define   Low_nrf905_TX_EN     PORTB &= ~(1 << PB0); 
//#define  nrf905_TRX_CE  PD4 	    //输出1
#define   Hign_nrf905_TRX_CE    PORTD |= (1 << PD4);   
#define   Low_nrf905_TRX_CE     PORTD &= ~(1 << PD4); 
//#define  nrf905_PWR     PB1 	    //输出1
#define   Hign_nrf905_PWR    PORTB |= (1 << PB1);   
#define   Low_nrf905_PWR     PORTB &= ~(1 << PB1); 
///////////////////////////SPI口定义////////////////////////////////////
//#define  nrf905_MISO	  PB6   	//输入0
//#define  nrf905_MOSI    PB5	    //输出1
//#define  nrf905_SCK     PB7	    //输出1
#define   Low_nrf905_SCK     PORTB &= ~(1 << PB7); 
////////////////////////////////////////////////////////////////////////
//#define  nrf905_CSN     PB4		//输出1		
#define   Hign_nrf905_CSN    PORTB |= (1 << PB4);   
#define   Low_nrf905_CSN     PORTB &= ~(1 << PB4); 						
///////////////////////////状态输出口///////////////////////////////////
//#define  nrf905_CD      PD3	    //输入0
#define   Hign_nrf905_CD    PORTD |= (1 << PD3);   
#define   Low_nrf905_CD     PORTD &= ~(1 << PD3); 
#define   Read_nrf905_CD    PINB & (1 << PD4);
//#define  nrf905_AM      PB3	    //输入0
#define   Hign_nrf905_AM    PORTB |= (1 << PB3);   
#define   Low_nrf905_AM     PORTB &= ~(1 << PB3); 
#define   Read_nrf905_AM    PINB & (1 << PB3);
//#define  nrf905_DR      PB2  		//输入0
#define   Hign_nrf905_DR     PORTB |= (1 << PB2);   
#define   Low_nrf905_DR     PORTB &= ~(1 << PB2); 
#define   Read_nrf905_DR    PINB & (1 << PB2);   
//------------------------------数码管位选--------------------------------------
//#define Display1   PA1  		    //输出1
#define   Hign_Display1     PORTA |= (1 << PA1);   
#define   Low_Display1    PORTA &= ~(1 << PA1); 
//#define Display2      PA2  		//输出1
#define   Hign_Display2     PORTA |= (1 << PA2);   
#define   Low_Display2    PORTA &= ~(1 << PA2); 
//#define Display3     PA3  		//输出1
#define   Hign_Display3     PORTA |= (1 << PA3);   
#define   Low_Display3     PORTA &= ~(1 << PA3); 
//#define Display4      PA4 		//输出1
#define   Hign_Display4     PORTA |= (1 << PA4);   
#define   Low_Display4     PORTA &= ~(1 << PA4);   
//-------------------------------BELL蜂明器-------------------------------------
#define   Hign_BELL  PORTD |= (1 << PD5);   
#define   Low_BELL     PORTD &= ~(1 << PD5); 
//-------------------------------JDQ继电器--------------------------------------
#define   Hign_JDQ  PORTD |= (1 << PD7)
#define   Low_JDQ     PORTD &= ~(1 << PD7) 
//----------------------------------905-SPI指令---------------------------------
#define WC	0x00
#define RRC	0x10
#define WTP	0x20
#define RTP	0x21
#define WTA	0x22
#define RTA	0x23
#define RRP	0x24
//---------------------------------发送数据-------------------------------------
uchar TxRxBuf[4] ;
//----------------------------------接收地址------------------------------------
uchar TxAddress[4]={0xcc,0xcc,0xcc,0xcc };    
//----------------------------------寄存器配置----------------------------------
uchar RFConf[11]=
{
  0x00,                             //配置命令//
  0x4c,                             //CH_NO,配置频段在430MHZ
  0x0C,                             //输出功率为10db,不重发，节电为正常模式
  0x44,                             //地址宽度设置，为4字节
  0x04,0x04,                        //接收发送有效数据长度为4字节
  0xCC,0xCC,0xCC,0xCC,              //接收地址
  0x58,                              //CRC充许，8位CRC校验，外部时钟信号不使能，16M晶振
};
//----------------------------------函数申明------------------------------------
void delay(uint x);
void Spi_initial();
uchar SpiReadSpiWrite(uchar DATA);
void system_Init(void);
void Config905(void);
void TxPacket(void);
void SetTxMode(void);
void TX(void);
//------------------------------------------------------------------------------
//******************************************************************************
//**********************************NRF905驱动**********************************
//******************************************************************************
//----------------------------------100us延时子程序-----------------------------
void delay(uint x)
{
	uint i;
	while(x--)
	for(i=0;i<80;i++);
}
//----------------------------------SPI初始化-----------------------------------
void Spi_initial()
{
	//SPCR=0x50;
	//SPSR=0x00;	
	SPCR   = (1<<SPE)|(1<<MSTR)|(0<<CPOL)|(0<<SPR0);   // 主机模式，fck/16, SPI方式0
}
//---------------------------------SPI读写程序----------------------------------	
uchar SpiReadSpiWrite(unsigned char cData)//r
{
	SPDR = cData;
	while(!(SPSR & (1<<SPIF) ))
	{};			// 等待SPI发送完毕
	return SPDR;
}
//---------------------------------系统状态初始化-------------------------------
void system_Init(void)			//r
{

   	Hign_nrf905_CSN;				// Spi 	disable
	Low_nrf905_SCK;				// Spi clock line init low
	Low_nrf905_DR;				// Init DR for input
	Low_nrf905_AM;				// Init AM for input
	Low_nrf905_CD;				// Init nrf905_CDfor input
	Hign_nrf905_PWR;				// nRF905 power on
	Low_nrf905_TRX_CE;			// Set nRF905 in standby mode
	Low_nrf905_TX_EN ;			// set radio in Rx mode
}
//--------------------------------NRF905寄存器初始化函数------------------------
void Config905(void)
{
	uchar i;
	Low_nrf905_CSN;	
	delay(1);				// Spi enable for write a spi command
	//SpiWrite(WC);				// Write config command写放配置命令
	for (i=0;i<11;i++)			// Write configration words  写放配置字
	{
	 SpiReadSpiWrite(RFConf[i]);
	}
	Hign_nrf905_CSN;					// Disable Spi
}
//--------------------------------NRF905待发数据打包----------------------------
void TxPacket(void)
{
	uchar i;
	Low_nrf905_CSN;		   				// 使能Spi，允许对nrf905进行读写操作
	delay(1);
	SpiReadSpiWrite(WTP);				// 写数据前先写写数据命令
	for (i=0;i<4;i++)
	{
	SpiReadSpiWrite( AD_data[i]);		// 待发送的32字节数据
	}						
	Hign_nrf905_CSN;
	delay(1);							// 关闭Spi，不允许对nrf905进行读写操作
	Low_nrf905_CSN;								// 使能Spi
	SpiReadSpiWrite(WTA);				// 写地址前首先先写地址命令
	for (i=0;i<4;i++)					// 写 4 bytes 地址
	{
	SpiReadSpiWrite(TxAddress[i]);
	}
	Hign_nrf905_CSN;					// 关闭Spi
	Hign_nrf905_TRX_CE;					// Set TRX_CE high,start Tx data transmission
	delay(1);							// 等待DR变高
	Low_nrf905_TRX_CE;							// 设置TRX_CE=0
}
//-------------------------------发送模式激发-----------------------------------
void SetTxMode(void)
{
	Low_nrf905_TRX_CE;				//
	Hign_nrf905_TX_EN;				//发送使能
	delay(2); 				// delay for mode change(>=650us)根据晶振不同要改变
}
//------------------------------------------------------------------------------
void TX(void)
{
     SetTxMode();	   // Set nRF905 in Tx mode
	 TxPacket();			   // Send data by nRF905
}
//******************************************************************************
//**********************************AD数据采集*****************************
//******************************************************************************
//-----------------------------AD转换函数----------------------------------------
unsigned char GetADResult(void)
{
	unsigned char i;
/**********************ADC 多工选择寄存器－ ADMUX配置方法********************/
/*      Bit   7     6     5     4    3    2    1    0     (Page203)        */
/*          REFS1 REFS0 ADLAR MUX4 MUX3 MUX2 MUX1 MUX0                    */
/*            1     1     1     0    0    1    1    1                    */
/************************************************************************/
	ADMUX |= (1 << REFS1);  //2.56V 的片内基准电压源， AREF 引脚外加滤波电容
	ADMUX |= (1 << REFS0);  //2.56V 的片内基准电压源， AREF 引脚外加滤波电容
	ADMUX |= (1 << ADLAR);  //ADC 转换结果 左对齐
	ADMUX &= ~(1 << MUX4);  //ADC0单端输入
	ADMUX &= ~(1 << MUX3);  //ADC0单端输入
	ADMUX &= ~(1 << MUX2);  //ADC0单端输入
	ADMUX &= ~(1 << MUX1);  //ADC0单端输入
	ADMUX &= ~(1 << MUX0);  //ADC0单端输入

/********************ADC 控制和状态寄存器A -ADCSRA配置方法*************************/
/*      Bit   7     6     5     4    3    2    1    0     (Page205)               */
/*          ADEN ADSC ADATE ADIF ADIE ADPS2 ADPS1 ADPS0                           */
/*            1     1     0     0    0    0    1    1                             */
/**********************************************************************************/
	ADCSRA |= (1 << ADEN);  //ADC 使能
	ADCSRA |= (1 << ADSC);  //ADC 开始转换
	ADCSRA &= ~(1 << ADATE);//无ADC 自动触发使能
	ADCSRA &= ~(1 << ADIF); //ADC 中断标志
	ADCSRA &= ~(1 << ADIE); //不使用中断
	ADCSRA |= (1 << ADPS2);//128分频时钟
	ADCSRA |= (1 << ADPS1); //128分频时钟
	ADCSRA |= (1 << ADPS0);//128分频时钟
	
//------------------------------------------------------------------------------
	while(!(ADCSRA & (1 << ADIF)));	//等待转换完成后ADIF自动置位
	i = ADCH;
	ADCSRA &= ~(1 << ADIF);		/*清标志*/
	ADCSRA &= ~(1 << ADEN);		/*关闭转换*/

	return i;
}
//--------------------------------//AD数据采集百位------------------------------
unsigned char GetBaiwei(unsigned char temp)   
{
	temp = temp/100;
	return temp;
}
//-----------------------------//AD数据采集十位---------------------------------
unsigned char GetShiwei(unsigned char temp)  
{
	if(temp > 99)
	{
		temp = temp - GetBaiwei(temp)*100;
	}
	temp = temp/10;
	return temp;
}
//---------------------------- //AD数据采集个位---------------------------------
unsigned char GetGewei(unsigned char temp)   
{
	temp = temp%10;
	return temp;
}
//------------------------------------------------------------------------------
void dis_zhuye()
{
	 PORTC=seg1[AD_data[1]];
	 Hign_Display4;
	 Hign_Display3;
	 Hign_Display2;
	 Low_Display1;
	 delay(80);
	 Hign_Display1;
//------------------------------------------------------------------------------
	 PORTC=seg[AD_data[2]];
	 Low_Display2;
	 delay(80);
	 Hign_Display2;
//------------------------------------------------ ---------------------------
	 PORTC=seg[AD_data[3]];
	 Low_Display3;
	 delay(80);
	 Hign_Display3;
//----------------------------------------------------------------------------- 
	 PORTC=0xc1;
	 Low_Display4;
	 delay(80);
	 Hign_Display4;	 
}
//----------------------------------主函数--------------------------------------
main()
{
//------------------------------------------------------------------------------
	DDRD=0xb2;		  //
	DDRB=0xB3;		  //
	DDRA=0x1e;       //
	DDRC=0xff; 		//LED端口设置
	PORTA = 0xff;
	Hign_BELL;	
	Hign_JDQ ;
//------------------------------------------------------------------------------
	Spi_initial();
	system_Init();
	Config905();
//------------------------------------------------------------------------------
	delay(1000);
while(1)
	{
	AD_num=GetADResult();
	AD_data[0]=0xAA;                    
	AD_data[1]=table[GetBaiwei( AD_num)];
	AD_data[2]=table[GetShiwei( AD_num)];
	AD_data[3]=table[GetGewei( AD_num)];
 	TX();					//AD数据发送
	dis_zhuye();
    }
}

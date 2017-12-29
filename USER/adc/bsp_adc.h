#ifndef __ADC_H
#define	__ADC_H

#include "stm32f4xx.h"


#define uchar unsigned char
#define uint unsigned int

/*Registers' Address*/
#define REG_STATUS  0   
#define REG_MUX     1	
#define REG_ADCON   2	
#define REG_DRATE   3
#define REG_IO      4	
#define REG_OFC0    5
#define REG_OFC1    6
#define REG_OPC2    7
#define REG_FSC0    8
#define REG_FSC1    9
#define REG_FSC2    10

#define ADS1256_STATUS       0x00   
#define ADS1256_MUX          0x01   
#define ADS1256_ADCON        0x02   
#define ADS1256_DRATE        0x03   
#define ADS1256_IO           0x04   
#define ADS1256_OFC0         0x05   
#define ADS1256_OFC1         0x06   
#define ADS1256_OFC2         0x07   
#define ADS1256_FSC0         0x08   
#define ADS1256_FSC1         0x09   
#define ADS1256_FSC2         0x0A

/*Operation Command*/
#define CMD_WAKEUP     0x00
#define CMD_RDATA      0x01
#define CMD_RDATAC     0x03
#define CMD_SDATAC     0x0F																												  
#define CMD_RREG       0x10
#define CMD_WREG       0x50
#define CMD_SELFCAL    0xf0
#define CMD_SELFOCAL   0xf1
#define CMD_SELFGCAL   0xf2
#define CMD_SYSOCAL    0xf3
#define CMD_SYSGCAL    0xf4
#define CMD_SYNC       0xfc
#define CMD_STANDBY    0xfd
#define CMD_RESET      0xfe
//#define CMD_WAKEUP     0xFF
// define commands 
#define ADS1256_CMD_WAKEUP   0x00 
#define ADS1256_CMD_RDATA    0x01 
#define ADS1256_CMD_RDATAC   0x03 
#define ADS1256_CMD_SDATAC   0x0f 
#define ADS1256_CMD_RREG     0x10 
#define ADS1256_CMD_WREG     0x50 
#define ADS1256_CMD_SELFCAL  0xf0 
#define ADS1256_CMD_SELFOCAL 0xf1 
#define ADS1256_CMD_SELFGCAL 0xf2 
#define ADS1256_CMD_SYSOCAL  0xf3 
#define ADS1256_CMD_SYSGCAL  0xf4 
#define ADS1256_CMD_SYNC     0xfc 
#define ADS1256_CMD_STANDBY  0xfd 
#define ADS1256_CMD_REST    0xfe 


#define PGA_1            0x00
#define PGA_2            0x01
#define PGA_4            0x02
#define PGA_8            0x03
#define PGA_16           0x04
#define PGA_32           0x05
#define PGA_64           0x06

#define POSITIVE_AIN0            (0X00<<4)
#define POSITIVE_AIN1            (0X01<<4)
#define POSITIVE_AIN2            (0X02<<4)
#define POSITIVE_AIN3            (0X03<<4)
#define POSITIVE_AIN4            (0X04<<4)
#define POSITIVE_AIN5            (0X05<<4)
#define POSITIVE_AIN6            (0X06<<4)
#define POSITIVE_AIN7            (0X07<<4)
#define POSITIVE_AINCOM          (0X08<<4)        

#define NEGTIVE_AIN0              0X00
#define NEGTIVE_AIN1              0X01
#define NEGTIVE_AIN2              0X02
#define NEGTIVE_AIN3              0X03
#define NEGTIVE_AIN4              0X04
#define NEGTIVE_AIN5              0X05
#define NEGTIVE_AIN6              0X06
#define NEGTIVE_AIN7              0X07
#define NEGTIVE_AINCOM            0X08

/*For fclkin=7.68MHz, data rate*/
#define DATARATE_30K              0xf0
#define DATARATE_15K              0xe0
#define DATARATE_7_5K             0xd0
#define DATARATE_3_7_5K           0xc0
#define DATARATE_2K               0xb0

/*STATUS REGISTER*/
#define MSB_FRIST                (0x00<<3)
#define LSB_FRIST                (0x01<<3)
#define ACAL_OFF                 (0x00<<2)
#define ACAL_ON                  (0x01<<2)
#define BUFEN_OFF                (0x00<<1)
#define BUFEN_ON                 (0x01<<1)

/*ADCON REGISTER*/
#define CLKOUT_OFF               (0x00<<5)
#define CLKOUT_CLKIN             (0x01<<5)
#define DETECT_OFF               (0x00<<3)
#define DETECT_ON_2UA            (0x02<<3)

//define chip1 io port
#define SetADS1256_CS1     GPIO_SetBits(ADS1256_CHIP1_CS_GPIO_PORT,ADS1256_CHIP1_CS_PIN)
#define ClrADS1256_CS1     GPIO_ResetBits(ADS1256_CHIP1_CS_GPIO_PORT,ADS1256_CHIP1_CS_PIN)
#define SetADS1256_RESET1   GPIO_SetBits(ADS1256_CHIP1_RESET_GPIO_PORT,ADS1256_CHIP1_RESET_PIN)
#define ClrADS1256_RESET1   GPIO_ResetBits(ADS1256_CHIP1_RESET_GPIO_PORT,ADS1256_CHIP1_RESET_PIN)

#define SetADS1256_DRDY1    GPIO_SetBits(ADS1256_CHIP1_DRDY_GPIO_PORT,ADS1256_CHIP1_DRDY_PIN)
#define SetADS1256_DO1      GPIO_SetBits(ADS1256_CHIP1_DOUT_GPIO_PORT,ADS1256_CHIP1_DOUT_PIN)

#define SetADS1256_IN1     GPIO_SetBits(ADS1256_CHIP1_DIN_GPIO_PORT,ADS1256_CHIP1_DIN_PIN)
#define ClrADS1256_IN1     GPIO_ResetBits(ADS1256_CHIP1_DIN_GPIO_PORT,ADS1256_CHIP1_DIN_PIN)

#define SetADS1256_CLK1    GPIO_SetBits(ADS1256_CHIP1_SCLK_GPIO_PORT,ADS1256_CHIP1_SCLK_PIN)
#define ClrADS1256_CLK1    GPIO_ResetBits(ADS1256_CHIP1_SCLK_GPIO_PORT,ADS1256_CHIP1_SCLK_PIN)

#define ADS1256_DRDY1      GPIO_ReadInputDataBit(ADS1256_CHIP1_DRDY_GPIO_PORT, ADS1256_CHIP1_DRDY_PIN)
#define ADS1256_DO1        GPIO_ReadInputDataBit(ADS1256_CHIP1_DOUT_GPIO_PORT, ADS1256_CHIP1_DOUT_PIN)

/*ADS1256 chip1 管脚宏定义*/
#define ADS1256_CHIP1_SCLK_RCC                RCC_AHB1Periph_GPIOB
#define ADS1256_CHIP1_SCLK_PIN                GPIO_Pin_3
#define ADS1256_CHIP1_SCLK_GPIO_PORT          GPIOB
#define ADS1256_CHIP1_DIN_RCC                 RCC_AHB1Periph_GPIOB
#define ADS1256_CHIP1_DIN_PIN                 GPIO_Pin_5
#define ADS1256_CHIP1_DIN_GPIO_PORT           GPIOB
#define ADS1256_CHIP1_DOUT_RCC                RCC_AHB1Periph_GPIOB
#define ADS1256_CHIP1_DOUT_PIN                GPIO_Pin_4
#define ADS1256_CHIP1_DOUT_GPIO_PORT          GPIOB
#define ADS1256_CHIP1_DRDY_RCC                RCC_AHB1Periph_GPIOD
#define ADS1256_CHIP1_DRDY_PIN                GPIO_Pin_3
#define ADS1256_CHIP1_DRDY_GPIO_PORT          GPIOD
#define ADS1256_CHIP1_RESET_RCC               RCC_AHB1Periph_GPIOG
#define ADS1256_CHIP1_RESET_PIN               GPIO_Pin_15
#define ADS1256_CHIP1_RESET_GPIO_PORT         GPIOG
#define ADS1256_CHIP1_CS_RCC                  RCC_AHB1Periph_GPIOE
#define ADS1256_CHIP1_CS_PIN                  GPIO_Pin_2
#define ADS1256_CHIP1_CS_GPIO_PORT            GPIOE


//define chip2 io port
#define SetADS1256_CS2     GPIO_SetBits(ADS1256_CHIP2_CS_GPIO_PORT,ADS1256_CHIP2_CS_PIN)
#define ClrADS1256_CS2     GPIO_ResetBits(ADS1256_CHIP2_CS_GPIO_PORT,ADS1256_CHIP2_CS_PIN)
#define SetADS1256_RESET2   GPIO_SetBits(ADS1256_CHIP2_RESET_GPIO_PORT,ADS1256_CHIP2_RESET_PIN)
#define ClrADS1256_RESET2   GPIO_ResetBits(ADS1256_CHIP2_RESET_GPIO_PORT,ADS1256_CHIP2_RESET_PIN)

#define SetADS1256_DRDY2    GPIO_SetBits(ADS1256_CHIP2_DRDY_GPIO_PORT,ADS1256_CHIP2_DRDY_PIN)
#define SetADS1256_DO2      GPIO_SetBits(ADS1256_CHIP2_DOUT_GPIO_PORT,ADS1256_CHIP2_DOUT_PIN)

#define SetADS1256_IN2     GPIO_SetBits(ADS1256_CHIP2_DIN_GPIO_PORT,ADS1256_CHIP2_DIN_PIN)
#define ClrADS1256_IN2     GPIO_ResetBits(ADS1256_CHIP2_DIN_GPIO_PORT,ADS1256_CHIP2_DIN_PIN)

#define SetADS1256_CLK2    GPIO_SetBits(ADS1256_CHIP2_SCLK_GPIO_PORT,ADS1256_CHIP2_SCLK_PIN)
#define ClrADS1256_CLK2    GPIO_ResetBits(ADS1256_CHIP2_SCLK_GPIO_PORT,ADS1256_CHIP2_SCLK_PIN)

#define ADS1256_DRDY2      GPIO_ReadInputDataBit(ADS1256_CHIP2_DRDY_GPIO_PORT, ADS1256_CHIP2_DRDY_PIN)
#define ADS1256_DO2        GPIO_ReadInputDataBit(ADS1256_CHIP2_DOUT_GPIO_PORT, ADS1256_CHIP2_DOUT_PIN)

/*ADS1256 chip2 管脚宏定义*/
#define ADS1256_CHIP2_SCLK_RCC                RCC_AHB1Periph_GPIOC
#define ADS1256_CHIP2_SCLK_PIN                GPIO_Pin_0
#define ADS1256_CHIP2_SCLK_GPIO_PORT          GPIOC
#define ADS1256_CHIP2_DIN_RCC                 RCC_AHB1Periph_GPIOB
#define ADS1256_CHIP2_DIN_PIN                 GPIO_Pin_1
#define ADS1256_CHIP2_DIN_GPIO_PORT           GPIOB
#define ADS1256_CHIP2_DOUT_RCC                RCC_AHB1Periph_GPIOC
#define ADS1256_CHIP2_DOUT_PIN                GPIO_Pin_6
#define ADS1256_CHIP2_DOUT_GPIO_PORT          GPIOC
#define ADS1256_CHIP2_DRDY_RCC                RCC_AHB1Periph_GPIOB
#define ADS1256_CHIP2_DRDY_PIN                GPIO_Pin_0
#define ADS1256_CHIP2_DRDY_GPIO_PORT          GPIOB
#define ADS1256_CHIP2_RESET_RCC               RCC_AHB1Periph_GPIOB
#define ADS1256_CHIP2_RESET_PIN               GPIO_Pin_14
#define ADS1256_CHIP2_RESET_GPIO_PORT         GPIOB
#define ADS1256_CHIP2_CS_RCC                  RCC_AHB1Periph_GPIOE
#define ADS1256_CHIP2_CS_PIN                  GPIO_Pin_2
#define ADS1256_CHIP2_CS_GPIO_PORT            GPIOE



//define chip3 io port
#define SetADS1256_CS3     GPIO_SetBits(ADS1256_CHIP3_CS_GPIO_PORT,ADS1256_CHIP3_CS_PIN)
#define ClrADS1256_CS3     GPIO_ResetBits(ADS1256_CHIP3_CS_GPIO_PORT,ADS1256_CHIP3_CS_PIN)
#define SetADS1256_RESET3   GPIO_SetBits(ADS1256_CHIP3_RESET_GPIO_PORT,ADS1256_CHIP3_RESET_PIN)
#define ClrADS1256_RESET3   GPIO_ResetBits(ADS1256_CHIP3_RESET_GPIO_PORT,ADS1256_CHIP3_RESET_PIN)

#define SetADS1256_DRDY3    GPIO_SetBits(ADS1256_CHIP3_DRDY_GPIO_PORT,ADS1256_CHIP3_DRDY_PIN)
#define SetADS1256_DO3      GPIO_SetBits(ADS1256_CHIP3_DOUT_GPIO_PORT,ADS1256_CHIP3_DOUT_PIN)

#define SetADS1256_IN3     GPIO_SetBits(ADS1256_CHIP3_DIN_GPIO_PORT,ADS1256_CHIP3_DIN_PIN)
#define ClrADS1256_IN3     GPIO_ResetBits(ADS1256_CHIP3_DIN_GPIO_PORT,ADS1256_CHIP3_DIN_PIN)

#define SetADS1256_CLK3    GPIO_SetBits(ADS1256_CHIP3_SCLK_GPIO_PORT,ADS1256_CHIP3_SCLK_PIN)
#define ClrADS1256_CLK3    GPIO_ResetBits(ADS1256_CHIP3_SCLK_GPIO_PORT,ADS1256_CHIP3_SCLK_PIN)

#define ADS1256_DRDY3      GPIO_ReadInputDataBit(ADS1256_CHIP3_DRDY_GPIO_PORT, ADS1256_CHIP3_DRDY_PIN)
#define ADS1256_DO3        GPIO_ReadInputDataBit(ADS1256_CHIP3_DOUT_GPIO_PORT, ADS1256_CHIP3_DOUT_PIN)
/*ADS1256 chip3 管脚宏定义*/
#define ADS1256_CHIP3_SCLK_RCC                RCC_AHB1Periph_GPIOA
#define ADS1256_CHIP3_SCLK_PIN                GPIO_Pin_3
#define ADS1256_CHIP3_SCLK_GPIO_PORT          GPIOA
#define ADS1256_CHIP3_DIN_RCC                 RCC_AHB1Periph_GPIOF
#define ADS1256_CHIP3_DIN_PIN                 GPIO_Pin_10
#define ADS1256_CHIP3_DIN_GPIO_PORT           GPIOF
#define ADS1256_CHIP3_DOUT_RCC                RCC_AHB1Periph_GPIOB
#define ADS1256_CHIP3_DOUT_PIN                GPIO_Pin_15
#define ADS1256_CHIP3_DOUT_GPIO_PORT          GPIOB
#define ADS1256_CHIP3_DRDY_RCC                RCC_AHB1Periph_GPIOA
#define ADS1256_CHIP3_DRDY_PIN                GPIO_Pin_6
#define ADS1256_CHIP3_DRDY_GPIO_PORT          GPIOA
#define ADS1256_CHIP3_RESET_RCC               RCC_AHB1Periph_GPIOC
#define ADS1256_CHIP3_RESET_PIN               GPIO_Pin_2
#define ADS1256_CHIP3_RESET_GPIO_PORT         GPIOC
#define ADS1256_CHIP3_CS_RCC                  RCC_AHB1Periph_GPIOE
#define ADS1256_CHIP3_CS_PIN                  GPIO_Pin_2
#define ADS1256_CHIP3_CS_GPIO_PORT            GPIOE

#define ADS1256_SPI                           SPI1
#define ADS1256_SPI_CLK                       RCC_APB2Periph_SPI1
#define ADS1256_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define ADS1256_SPI_SCK_PIN                   GPIO_Pin_3                  
#define ADS1256_SPI_SCK_GPIO_PORT             GPIOB                       
#define ADS1256_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define ADS1256_SPI_SCK_PINSOURCE             GPIO_PinSource3
#define ADS1256_SPI_SCK_AF                    GPIO_AF_SPI1

#define ADS1256_SPI_MISO_PIN                  GPIO_Pin_4                
#define ADS1256_SPI_MISO_GPIO_PORT            GPIOB                   
#define ADS1256_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define ADS1256_SPI_MISO_PINSOURCE            GPIO_PinSource4
#define ADS1256_SPI_MISO_AF                   GPIO_AF_SPI1

#define ADS1256_SPI_MOSI_PIN                  GPIO_Pin_5                
#define ADS1256_SPI_MOSI_GPIO_PORT            GPIOB                     
#define ADS1256_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define ADS1256_SPI_MOSI_PINSOURCE            GPIO_PinSource5
#define ADS1256_SPI_MOSI_AF                   GPIO_AF_SPI1


void Init_ADS_GPIO(void);
void ADS1256_Init_No_SPI(void);
void ADS1256_Write_Byte(unsigned char d);
void  ADS1256_Write_Chip1_Reg(unsigned char reg_name, unsigned char reg_data);
void  ADS1256_Write_Chip2_Reg(unsigned char reg_name, unsigned char reg_data);
void  ADS1256_Write_Chip3_Reg(unsigned char reg_name, unsigned char reg_data);
char ADS1256_Read_Chip3_Byte(void);
char ADS1256_Read_Chip2_Byte(void);
char ADS1256_Read_Chip1_Byte(void);
void ADS1256_Read_dat(void);
void Write_Reg_Mux(uchar x);
unsigned int Get_chx_dat(unsigned char channe);
void SPI_ADS1256_Init(void);
void ADS1256_Init(void);
void ADS1256WREG(unsigned char regaddr,unsigned char databyte);
unsigned char SPI_WriteByte(unsigned char TxData);
void Init_ADS_GPIO_NO_SPI(void);
void Get_Ads1256_Data_No_SPI(unsigned char channel);

#endif /* __LED_H */



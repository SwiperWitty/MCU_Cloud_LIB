#include "IIC.h"


void IIC_SDA_Satar (char GPIO_Mode)
{
#ifdef Exist_IIC
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin = IIC_SDA;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode;

    GPIO_Init(GPIO_IIC, &GPIO_InitStructure);
#endif
}

void IIC_Init(int SET)
{
#ifdef Exist_IIC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    GPIO_InitTypeDef  GPIO_InitStructure;
    if (SET)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //GPIOB
        GPIO_InitStructure.GPIO_Pin = IIC_SCL;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIO_IIC, &GPIO_InitStructure);

        IIC_SDA_Satar (IIC_Mode_OUT);
    }
    else
    {
        GPIO_InitStructure.GPIO_Pin = IIC_SCL | IIC_SDA;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
        GPIO_Init(GPIO_IIC, &GPIO_InitStructure);
    }
#endif
}
// **   //

static void IIC_Delay (int time)
{
    int temp;
    for (int i = 0; i < time; ++i)
    {
        temp = IIC_Base_Speed;            //SET
        while((--temp) > 0);
    }
}

void IIC_StartBit(void)//  ��ʼ
{
    IIC_SCL_L();  // Set SCL line
    IIC_SDA_Satar (IIC_Mode_OUT);
    IIC_SDA_H();  // Set SDA line
    IIC_Delay(1); // Wait a few microseconds
    IIC_SCL_H();  // Set SCL line
    IIC_Delay(1); // Generate bus free time between Stop
    IIC_SDA_L();  // Clear SDA line
    IIC_Delay(1); // Hold time after (Repeated) Start
    // Condition. After this period, the first clock is generated.
    //(Thd:sta=4.0us min)��SCK=1ʱ����⵽SDA��1��0��ʾͨ�ſ�ʼ���½��أ�
    IIC_SCL_L();  // Clear SCL line
    IIC_Delay(1); // Wait a few microseconds
}

void IIC_StopBit(void)//  ֹͣ
{
    IIC_SCL_L();  // Clear SCL line
    IIC_SDA_Satar (IIC_Mode_OUT);
    IIC_Delay(1); // Wait a few microseconds
    IIC_SDA_L();  // Clear SDA line
    IIC_Delay(1); // Wait a few microseconds
    IIC_SCL_H();  // Set SCL line           /*  END    */
    IIC_Delay(1); // Stop condition setup time(Tsu:sto=4.0us min)
    IIC_SDA_H();  // Set SDA line��SCL=1ʱ����⵽SDA��0��1��ʾͨ�Ž����������أ�
}

void IIC_ASK(void)      //�ٶȿ죬���ں��Ƿ��д��豸
{
    IIC_SCL_L();
    IIC_SDA_L();
    IIC_Delay(2);
    IIC_SCL_H();
    IIC_Delay(2);
}

void IIC_NASK(void)     //��������
{
    IIC_SCL_L();
    IIC_SDA_H();
    IIC_Delay(2);
    IIC_SCL_H();
    IIC_Delay(2);
}

char IIC_WaitASK(void)  //һ��Ҫ�д��豸��Ӧ
{
    char temp = 0;
    int Time = 0;
    IIC_SCL_L();
    IIC_Delay(2);
    IIC_SDA_Satar (IIC_Mode_IN);
    IIC_SCL_H();
    do {
        IIC_Delay(1);
        Time++;
        if (IIC_SDA_IN() == 0)      //�ҵ����ݣ���������
        {
            temp = 1;
            break;
        }
    } while (Time < 20);
    return temp;
}

void IIC_Write_DATA(char DATA,int Speed)
{
#ifdef Exist_IIC
    char temp;
    IIC_SDA_Satar (IIC_Mode_OUT);
    for (int i = 0; i < 8; i++) {
        IIC_SCL_L();      //׼�����ݱ��
        IIC_Delay(Speed);
        temp = (DATA << i) & 0x80;
        if (temp)
            IIC_SDA_H();
        else
            IIC_SDA_L();
        IIC_Delay(Speed);
        IIC_SCL_H();      //���ݱ�����
        IIC_Delay(Speed);
    }
#endif
}

char IIC_Read_DATA(char DATA,int Speed)
{
    char temp = 0;
#ifdef Exist_IIC

    IIC_SDA_Satar (IIC_Mode_IN);
    for (int i = 0; i < 8; i++) {
        IIC_SCL_L();      //׼�����ݱ��
        IIC_Delay(Speed);
        temp = ((char)IIC_SDA_IN() << i);
        IIC_Delay(Speed);
        IIC_SCL_H();      //���ݱ�����
        IIC_Delay(Speed);
    }
#endif
    return temp;
}

char IIC_Send_DATA(char Addr,const char *Data,char ACK,int Length,int Speed)
{
    char BK = 0;
#ifdef Exist_IIC
    IIC_StartBit();
    IIC_Write_DATA((Addr << 1) & 0xFE,Speed);      //дģʽ
    if(ACK && BK == 0)      //��Ҫʹ��ACK
    {
        BK = IIC_WaitASK();
        if (BK == 0)
        {
            IIC_StopBit();
            return BK;
        }
    }
    else if(ACK == 0)
    {
        IIC_ASK();
        BK = 1;
    }
    //��ַ����
    for (int i = 0; i < Length; ++i)
    {
        IIC_Write_DATA(Data[i],Speed);
        if(ACK && BK == 0)
        {
            BK = IIC_WaitASK();
            if (BK == 0)    //��һ�β��ɹ�
            {
                break;
            }
        }
        else if(ACK == 0)
            IIC_ASK();
    }
    //���ݽ���
    IIC_StopBit();
//IIC����
#endif
    return BK;
}

char IIC_Receive_DATA(char Addr,const char *Data,char *Target,char ACK,int Length,int Speed)
{
    char BK = 0;
#ifdef Exist_IIC
    IIC_StartBit();
    IIC_Write_DATA((Addr << 1) | 0x01,Speed);      //��ģʽ
    if(ACK && BK == 0)      //��Ҫʹ��ACK
    {
        BK = IIC_WaitASK();
        if (BK == 0)
        {
            IIC_StopBit();
            return BK;
        }
    }
    else if(ACK == 0)       //����Ҫʹ��ACK
    {
        IIC_ASK();
        BK = 1;
    }
    //��ַ����
    Target[0] = IIC_Read_DATA(Data[0],Speed);

    IIC_StopBit();
//IIC����
#endif
    return BK;
}

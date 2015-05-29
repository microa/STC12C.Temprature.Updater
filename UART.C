#define FOSC 12000000
#define BAUD 9600

#define NONE_PARITY     0   //��У��λ
#define ODD_PARITY      1   //��У��
#define EVEN_PARITY     2   //żУ��
#define MARK_PARITY     3   //���У��
#define SPACE_PARITY    4   //��У��

#define PARITYBIT NONE_PARITY

#define S2RI  0x01
#define S2TI  0x02
#define S2RB8 0x04
#define S2TB8 0x08


bit busy;

void SendData(char dat);
void SendString(char *s);

void UART_Init()
{
#if (PARITYBIT == NONE_PARITY)
    S2CON = 0x5a;               //8λ�ɱ䲨���� (��У��λ)
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    S2CON = 0xda;               //9λ�ɱ䲨����,У��λ��ʼΪ1
#elif (PARITYBIT == SPACE_PARITY)
    S2CON = 0xd5;               //9λ�ɱ䲨����,У��λ��ʼΪ0
#endif

    BRT = -(FOSC/32/BAUD);      //���ö��������ʷ����������س�ֵ
    AUXR = 0x14;                //���������ʷ�����������1Tģʽ
    IE2 = 0x01;                 //ʹ�ܴ���2�ж�
    EA = 1;                     //�����ж�
		
		//SendString("Tsiov Test");
    
}

void Uart2() interrupt 8 using 1
{
    if (S2CON & S2RI)
    {
        S2CON &= ~S2RI;         //���������ɱ�־
        Rcv = S2BUF;             //P0��ʾ��������
//        P2 = (S2CON & S2RB8);   //P2.2��ʾУ��λ
    }
    if (S2CON & S2TI)
    {
        S2CON &= ~S2TI;         //���������ɱ�־
        busy = 0;
    }
}

void SendData(char dat)
{
    while (busy);               //�ȴ��ϸ����ݷ������
    ACC = dat;                  //ȡ��żУ��λP
    if (P)						//����P�����ô������ݵ�У��λ
    {
#if (PARITYBIT == ODD_PARITY)
        S2CON &= ~S2TB8;        //��У��λΪ0
#elif (PARITYBIT == EVEN_PARITY)
        S2CON |= S2TB8;         //��У��λΪ1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        S2CON |= S2TB8;         //��У��λΪ1
#elif (PARITYBIT == EVEN_PARITY)
        S2CON &= ~S2TB8;        //��У��λΪ0
#endif
    }
    busy = 1;
    S2BUF = ACC;                //��������
}

void SendString(char *s)
{
    while (*s)                  //�ж��ַ���������־
    {
        SendData(*s++);         //�����ַ�
    }
}
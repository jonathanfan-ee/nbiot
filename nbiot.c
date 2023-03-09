#include "nbiot.h"
#include "gpio.h"
#include "usart.h"
#include "cmsis_os.h"
#include "string.h"
#include "NB_AT_Bulk.h"

extern int Global_test4;
extern int Global_test5;

extern int SYS_Status_selfadd;

char NB_uart[20][100]={0};
unsigned int NB_uart_current=0;
char AT_test[2]="AT";

typedef struct{
	unsigned char NB_Started;
	unsigned char NB_Ready_Status; 	
	unsigned char NB_Status;
	
	unsigned char NB_CGREG_Status;
	unsigned char NB_CGREG_InRun_Down;
	
	unsigned char NB_Connect_Status;
	
	unsigned char NB_Exchange_OK;
	
	unsigned char NB_Sleep_Status;
		
}NB_Status;


NB_Status NB_Status_temp ={0};



void NB_Send(void)
{
	int i;
	
	int nb_send_test_count=0;
	
	char test_json[100]="AT+IPSEND=0,0,\"{\"c\":24.8,\"h\":29,\"voc\": 0,\"o\":0,\"m\":1,\"p\":3.3,\"i\":'N055',\"t\":'',\"imei\":''}\"\r\n";
	char test_add[100]="AT+IPSEND=0,0,\"000\"\r\n";
	
	NB_Status_temp.NB_Started = 0;
	NB_Status_temp.NB_Ready_Status = 0;
	NB_Status_temp.NB_Status = 0;
	NB_Status_temp.NB_CGREG_Status = 0;
	NB_Status_temp.NB_CGREG_InRun_Down = 0;
	
	NB_Status_temp.NB_Connect_Status = 0;
	NB_Status_temp.NB_Exchange_OK = 0;
	NB_Status_temp.NB_Sleep_Status = 0;
	
//	NB_Status_temp.NB_PDN_Status = 0;

	//������ ��RESET������
	osDelay(2000);
	NB_Internal_Reset();
	osDelay(2000);
	
	EnableUsart1_IT();
	
	//NB_No_RESET;
	NB_Internal_POWER_ON();
	NB_Status_temp.NB_Started = 1;
	
	//����ָ�None
	//������Ӧ��+CPIN: READY
	while(!NB_Status_temp.NB_Ready_Status){
		if(Usart1type.UsartRecFlag)  
		{
//				for(i=0;i<Usart1type.UsartRecLen;i++)NB_uart[NB_uart_current][i]=Usart1type.Usart1DMARecBuffer[i];
//				NB_uart_current++;
			
				if(strstr((const char*)Usart1type.Usart1DMARecBuffer,"+CPIN: READY") != NULL)
				{
						NB_Status_temp.NB_Ready_Status = 1;
				}
				Usart1type.UsartRecFlag = 0; 
				Usart1type.UsartRecLen = 0; 
				for(i=0;i<USART1_DMA_REC_SIE;i++) Usart1type.Usart1DMARecBuffer[i]=0;
		}
		osDelay(10);
	}
	
	i=0;
	while(i<10)
	{
			//����ָ�AT
			//������Ӧ��OK
			NB_Status_temp.NB_Status=1+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT\r\n",4,"OK",30);
			osDelay(100);//temp
			i++;
	}
	
//	//����ָ�AT+SM=LOCK
//	//������Ӧ��OK
//	NB_Status_temp.NB_Status=1+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+SM=LOCK\r\n",12,"OK",30);
//	osDelay(100);//temp
	
	//����ָ�AT+SM=UNLOCK_FOREVER
	//������Ӧ��OK
	NB_Status_temp.NB_Status=1+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+SM=UNLOCK_FOREVER\r\n",22,"OK",30);
	osDelay(100);//temp
	
	//������WAKE�Ż��Ѻ� ��������ʱ��
	//����ָ�AT*WAKETIME=10
	//������Ӧ��OK
	NB_Status_temp.NB_Status=4+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT*WAKETIME=10\r\n",16,"OK",30);
	osDelay(100);//temp
	
	//����ָ�AT+CMSYSCTRL=0,2
	//������Ӧ��OK
	NB_Status_temp.NB_Status=3+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+CMSYSCTRL=0,2\r\n",18,"OK",30);
	osDelay(100);//temp
	
	//����ָ�AT+ICCID
	//������Ӧ��+ICCID: 89860473102070392694
	NB_Status_temp.NB_Status=2+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+ICCID\r\n",10,"+ICCID: ",30);
	osDelay(100);//temp

	//����ָ�AT*SLEEP=1
	//������Ӧ��OK
	NB_Status_temp.NB_Status=5+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT*SLEEP=1\r\n",12,"OK",30);
	osDelay(100);//temp
	
	//����ָ�AT+CEDRXS=0,5
	//������Ӧ��OK
	NB_Status_temp.NB_Status=6+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+CEDRXS=0,5\r\n",15,"OK",30);
	osDelay(100);//temp
	
	//����ָ�AT+CPSMS=1,,,"01000101","00000000"
	//������Ӧ��OK
	NB_Status_temp.NB_Status=7+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+CPSMS=1,,,\"01000101\",\"00000000\"\r\n",36,"OK",30);
	osDelay(100);//temp
	
	//�鿴�ź�ǿ�� RSSI BER
	//����ָ�AT+CSQ
	//������Ӧ��OK
	NB_Status_temp.NB_Status=8+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+CSQ\r\n",8,"+CSQ: ",30);
	osDelay(100);//temp
	
	//����ָ�AT+CGREG=1
	//������Ӧ��OK
	NB_Status_temp.NB_Status=9+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+CGREG=1\r\n",12,"OK",30);
	osDelay(100);//temp
	
	//����ָ�AT+CGREG?
	//������Ӧ��OK
	while(!NB_Status_temp.NB_CGREG_Status)
	{
			NB_U1_Command_Send_Only((uint8_t*)"AT+CGREG?\r\n",11);
			if(NB_U1_Expect_Receive_Single_Match_Return("+CGREG: ",'+',10,20)==('0'+1))
			{
					NB_Status_temp.NB_CGREG_Status=1;
			}
			
			//����ָ�AT+CCLK?
			//������Ӧ��OK
			NB_Status_temp.NB_Status=9+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+CCLK?\r\n",10,"OK",30);
			osDelay(100);//temp
	}
	
//	//����ָ�AT+CEDRXS=0,5
//	//������Ӧ��OK
//	NB_Status_temp.NB_ICCID_Status = NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+CEDRXS=0,5\r\n",15,"OK",30);
//	osDelay(100);//temp
	
//	//����ָ�AT+CCLK?
//	//������Ӧ��OK
//	NB_Status_temp.NB_Status=9+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+CCLK?\r\n",10,"OK",30);
//	osDelay(100);//temp

//while(1)
//{

	while(NB_Status_temp.NB_Connect_Status!=1)
	{
			
			//�����Լ������״̬
			NB_U1_Command_Send_Only((uint8_t*)"AT+CGREG?\r\n",11);
			if(NB_U1_Expect_Receive_Single_Match_Return("+CGREG: ",'+',10,20)!=('0'+1))
			{
					NB_Status_temp.NB_CGREG_InRun_Down=1;
			}
			//����ʱ��������
			if(NB_Status_temp.NB_CGREG_InRun_Down)
			{
			//����ָ�AT+CGREG=1
			//������Ӧ��OK
			NB_Status_temp.NB_Status=9+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+CGREG=1\r\n",12,"OK",30);
			osDelay(100);//temp
			while(NB_Status_temp.NB_CGREG_InRun_Down)
					{
							NB_U1_Command_Send_Only((uint8_t*)"AT+CGREG?\r\n",11);
							if(NB_U1_Expect_Receive_Single_Match_Return("+CGREG: ",'+',10,20)==('0'+1))
							{
									NB_Status_temp.NB_CGREG_InRun_Down=0;
							}
							
							//����ָ�AT+CCLK?
							//������Ӧ��OK
							NB_Status_temp.NB_Status=9+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+CCLK?\r\n",10,"OK",30);
							osDelay(100);//temp
					}
			}
		
			//����ָ�AT+IPSTART=0,"TCP","test.rancode.com",8888
			//������Ӧ1��OK
			//������Ӧ1��CONNECT OK
			if(NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+IPSTATUS=0\r\n",15,"0,\"\",\"\",0,\"\"",30)==1)
			{
					NB_Status_temp.NB_Status=10+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+IPSTART=0,\"TCP\",\"root.g000000gle.com\",8800\r\n",47,"OK",30);
					osDelay(100);//temp
					NB_Status_temp.NB_Connect_Status=NB_U1_Expect_Receive_Dual_Match("CONNECT OK","CONNECT FAIL",2,100);
			}else
			{
					NB_Status_temp.NB_Connect_Status = 0;
			}
			//�������⴦��
			if(NB_Status_temp.NB_Connect_Status == 0)
			{
					NB_U1_Command_Send_Only((uint8_t*)"AT+IPSTATUS=0\r\n",11);
					NB_Status_temp.NB_Status = NB_U1_Expect_Receive_Dual_Match("CONNECTED","CONNECTING",2,100);
					if(NB_Status_temp.NB_Status == 2)
					{
							//��ʱ��������
							do {NB_U1_Command_Send_Only((uint8_t*)"AT+IPSTATUS=0\r\n",11);}
							while (NB_U1_Expect_Receive_Dual_Match("CONNECTED","CONNECTING",2,100)==2);
					}
					if(NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+IPSTATUS=0\r\n",15,"CONNECTED",30)==1)NB_Status_temp.NB_Connect_Status=1;
			}
		
	}
	
	//����ָ�AT+IPSEND=0,0,PUT_JSON_HERE
	//������Ӧ��OK
	//test_json
	//����ʧ�� ERROR ����������
	NB_Status_temp.NB_Status=9+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)test_json,sizeof(test_json),"OK",30);
	osDelay(100);//temp
	
	//����ָ�AT+IPRCFG=1,0,0
	//������Ӧ��OK	
	NB_Status_temp.NB_Status=12+NB_U1_Command_Send_Only((uint8_t*)"AT+IPRCFG=1,0,0\r\n",17);
	while(!NB_Status_temp.NB_Exchange_OK)
	{
			if(NB_U1_Expect_Receive_Single_Match_Return("+IPRD: 0,",'H',1,20)==('e'))NB_Status_temp.NB_Exchange_OK=1;
			Global_test4++;
	}
	
	//����ָ�AT+IPCLOSE=0
	//������Ӧ��OK
	//test_json
	NB_Status_temp.NB_Status=9+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+IPCLOSE=0\r\n",14,"OK",30);
	NB_Status_temp.NB_Status=11+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+IPSTATUS=0\r\n",15,"0,\"\",\"\",0,\"\"",30);
	NB_Status_temp.NB_Connect_Status=0;
	NB_Status_temp.NB_Ready_Status=0;
	osDelay(100);//temp

	
osDelay(2000);	
	
	//����ָ�AT*ENTERSLEEP
	//������Ӧ��OK
	NB_Status_temp.NB_Status=9+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT*ENTERSLEEP\r\n",15,"OK",30);
	osDelay(100);//temp
	
	while(!NB_Status_temp.NB_Sleep_Status)
	{
			NB_Status_temp.NB_Sleep_Status=NB_U1_Expect_Receive_Dual_Match("*GOTOSLEEP","",1,100);
			if(!NB_Status_temp.NB_Sleep_Status)
			{
					NB_Status_temp.NB_Status=9+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT*ENTERSLEEP\r\n",15,"OK",10);
			}
	}
	
//}	
	
	//NB_WAKE_UP();
	
	while(1)
	{
		
		nb_send_test_count++;
		if(nb_send_test_count>999)nb_send_test_count=1;
		test_add[15]='0'+nb_send_test_count/100;
		test_add[16]='0'+nb_send_test_count%100/10;
		test_add[17]='0'+nb_send_test_count%10;
		//char test_add[100]="AT+IPSEND=0,0,\"000\"";
		
			osDelay(60000);
			NB_WAKE_UP();
		
							//����ָ�None
							//������Ӧ��+CPIN: READY
							while(!NB_Status_temp.NB_Ready_Status){
								if(Usart1type.UsartRecFlag)  
								{
										for(i=0;i<Usart1type.UsartRecLen;i++)NB_uart[NB_uart_current][i]=Usart1type.Usart1DMARecBuffer[i];
										NB_uart_current++;
									
										if(strstr((const char*)Usart1type.Usart1DMARecBuffer,"+CPIN: READY") != NULL)
										{
												NB_Status_temp.NB_Ready_Status = 1;
										}
										Usart1type.UsartRecFlag = 0; 
										Usart1type.UsartRecLen = 0; 
										for(i=0;i<USART1_DMA_REC_SIE;i++) Usart1type.Usart1DMARecBuffer[i]=0;
								}
								osDelay(10);
							}
		
					//----------------------------------------------copy start
						while(NB_Status_temp.NB_Connect_Status!=1)
						{
								
								//�����Լ������״̬
								NB_U1_Command_Send_Only((uint8_t*)"AT+CGREG?\r\n",11);
								if(NB_U1_Expect_Receive_Single_Match_Return("+CGREG: ",'+',10,20)!=('0'+1))
								{
										NB_Status_temp.NB_CGREG_InRun_Down=1;
								}
								//����ʱ��������
								if(NB_Status_temp.NB_CGREG_InRun_Down)
								{
								//����ָ�AT+CGREG=1
								//������Ӧ��OK
								NB_Status_temp.NB_Status=9+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+CGREG=1\r\n",12,"OK",30);
								osDelay(100);//temp
								while(NB_Status_temp.NB_CGREG_InRun_Down)
										{
												NB_U1_Command_Send_Only((uint8_t*)"AT+CGREG?\r\n",11);
												if(NB_U1_Expect_Receive_Single_Match_Return("+CGREG: ",'+',10,20)==('0'+1))
												{
														NB_Status_temp.NB_CGREG_InRun_Down=0;
												}
												
												//����ָ�AT+CCLK?
												//������Ӧ��OK
												NB_Status_temp.NB_Status=9+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+CCLK?\r\n",10,"OK",30);
												osDelay(100);//temp
										}
								}
							
								//����ָ�AT+IPSTART=0,"TCP","test.rancode.com",8888
								//������Ӧ1��OK
								//������Ӧ1��CONNECT OK
								if(NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+IPSTATUS=0\r\n",15,"0,\"\",\"\",0,\"\"",30)==1)
								{
										NB_Status_temp.NB_Status=10+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+IPSTART=0,\"TCP\",\"root.g000000gle.com\",8800\r\n",47,"OK",10);
										osDelay(100);//temp
										NB_Status_temp.NB_Connect_Status=NB_U1_Expect_Receive_Dual_Match("CONNECT OK","CONNECT FAIL",2,100);
								}else
								{
										NB_Status_temp.NB_Connect_Status = 0;
								}
								//�������⴦��
								if(NB_Status_temp.NB_Connect_Status == 0)
								{
										NB_U1_Command_Send_Only((uint8_t*)"AT+IPSTATUS=0\r\n",11);
										NB_Status_temp.NB_Status = NB_U1_Expect_Receive_Dual_Match("CONNECTED","CONNECTING",2,100);
										if(NB_Status_temp.NB_Status == 2)
										{
												//��ʱ��������
												do {NB_U1_Command_Send_Only((uint8_t*)"AT+IPSTATUS=0\r\n",11);}
												while (NB_U1_Expect_Receive_Dual_Match("CONNECTED","CONNECTING",2,100)==2);
										}
										if(NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+IPSTATUS=0\r\n",15,"CONNECTED",30)==1)NB_Status_temp.NB_Connect_Status=1;
								}
							
						}
						
						//����ָ�AT+IPSEND=0,0,PUT_JSON_HERE
						//������Ӧ��OK
						//test_json
						//����ʧ�� ERROR ����������
						NB_Status_temp.NB_Status=9+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)test_add,sizeof(test_add),"OK",30);
						osDelay(100);//temp
						
						//����ָ�AT+IPRCFG=1,0,0
						//������Ӧ��OK	
						NB_Status_temp.NB_Status=12+NB_U1_Command_Send_Only((uint8_t*)"AT+IPRCFG=1,0,0\r\n",17);
						while(!NB_Status_temp.NB_Exchange_OK)
						{
								if(NB_U1_Expect_Receive_Single_Match_Return("+IPRD: 0,",'H',1,20)==('e'))NB_Status_temp.NB_Exchange_OK=1;
								Global_test4++;
						}
						
						//����ָ�AT+IPCLOSE=0
						//������Ӧ��OK
						//test_json
						NB_Status_temp.NB_Status=9+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+IPCLOSE=0\r\n",14,"OK",30);
						NB_Status_temp.NB_Status=11+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT+IPSTATUS=0\r\n",15,"0,\"\",\"\",0,\"\"",30);
						NB_Status_temp.NB_Connect_Status=0;
						osDelay(100);//temp

						
					osDelay(2000);	
						
						//����ָ�AT*ENTERSLEEP
						//������Ӧ��OK
						NB_Status_temp.NB_Status=9+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT*ENTERSLEEP\r\n",15,"OK",30);
						osDelay(100);//temp
						
						while(!NB_Status_temp.NB_Sleep_Status)
						{
								NB_Status_temp.NB_Sleep_Status=NB_U1_Expect_Receive_Dual_Match("*GOTOSLEEP","",1,150);
								if(!NB_Status_temp.NB_Sleep_Status)
								{
										NB_Status_temp.NB_Status=9+NB_U1_Command_Send_Single_Match_Retry((uint8_t*)"AT*ENTERSLEEP\r\n",15,"OK",30);
								}
						}

							
							
					//----------------------------------------------cpoy end
	
	}
	
	
	
	//Global_test5=100;
	
	
	
	while(1){
		
		osDelay(100);
		
	}
		

}
	

	






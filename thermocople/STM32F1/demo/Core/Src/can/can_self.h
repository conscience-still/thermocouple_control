#ifndef __CAN_SELF_H
#define __CAN_SELF_H
#include "main.h"

//CAN1����RX0�ж�ʹ��
#define CAN_RX0_INT_ENABLE	1		//0,��ʹ��;1,ʹ��.								    	
#define CAN_RX_BUF_LEN	8	//���յ��ֽ������Ϊ8.

typedef enum {

	Control_Data                =    0x210,
}CANFilter_ID;

__packed typedef struct{
	uint16_t CAN_ID;                 //
	void(*f)(uint8_t*);        //
}CANAnalyzTypeDef;  
/*************************************************************/
/*CAN���� ��������*/
__packed typedef struct{
  uint8_t  ctrl_sta	;    //����sta
  uint8_t  res[7] ;    //Ԥ������
}CANRecv210;

typedef void ( *CAN_Send_general_fun )( void );/*����ָ����ú���*/
typedef struct _STRUCT_SELECT_FUN
{
	uint16_t Type;
	uint8_t  flag;
	uint8_t  src;
	uint16_t  count;
	CAN_Send_general_fun CAN_Send_Fun;
} CAN_SendTypeDef;

/*CAN Э����ͷ��Ϣ���ֶ�lyn*/
__packed typedef struct{//�ô˽ṹ��ʱ����ٴ����� ������ͬ��������и���
	uint32_t TboxStdId;
	uint32_t TboxExtId;
	uint8_t  TboxIDE;
	uint8_t  TboxRTR;
	uint8_t  TboxDLC;
}CAN_SendCommtypedef;

/*CAN���͸�·���ص��¶���Ϣ*/
__packed typedef struct{
	uint8_t  Temperature_one   ;             //byte 0  
	uint8_t  Temperature_Two   ;             //byte 1  
	uint8_t  Temperature_Three ;             //byte 2  
	uint8_t  Temperature_Four  ;             //byte 3
	uint8_t  Temperature_Five  ;             //byte 4
	uint8_t  Temperature_Six  ;              //byte 5
	uint8_t  Temperature_Seven  ;            //byte 6
	uint8_t  Temperature_Eight  ;            //byte 7
}CANSend33B;
/*CAN���� ��������*/
__packed typedef struct{
	uint8_t  ctrl_sta_one        :1;    //�ȵ�ż����״̬1
	uint8_t  ctrl_tim_one        :7;    //�ȵ�ż����ʱ��1 
	uint8_t  ctrl_sta_two        :1;    //�ȵ�ż����״̬2
	uint8_t  ctrl_tim_two        :7;    //�ȵ�ż����ʱ��2 
	uint8_t  ctrl_sta_three      :1;    //�ȵ�ż����״̬3
	uint8_t  ctrl_tim_three      :7;    //�ȵ�ż����ʱ��3 
	uint8_t  ctrl_sta_four       :1;    //�ȵ�ż����״̬4
	uint8_t  ctrl_tim_four       :7;    //�ȵ�ż����ʱ��4 
	uint8_t  ctrl_sta_five       :1;    //�ȵ�ż����״̬5
	uint8_t  ctrl_tim_five       :7;    //�ȵ�ż����ʱ��5 
	uint8_t  ctrl_sta_six        :1;    //�ȵ�ż����״̬6
	uint8_t  ctrl_tim_six        :7;    //�ȵ�ż����ʱ��6 
	uint8_t  ctrl_sta_seven      :1;    //�ȵ�ż����״̬7
	uint8_t  ctrl_tim_seven      :7;    //�ȵ�ż����ʱ��7 
	uint8_t  ctrl_sta_eight      :1;    //�ȵ�ż����״̬8
	uint8_t  ctrl_tim_eight      :7;    //�ȵ�ż����ʱ��8 
}CANSend226;
/****************************************************************************/
/** 
  * @brief  CAN Tx message structure definition  
  */

typedef struct
{
  uint32_t StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

  uint32_t ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

  uint8_t IDE;     /*!< Specifies the type of identifier for the message that 
                        will be transmitted. This parameter can be a value 
                        of @ref CAN_identifier_type */

  uint8_t RTR;     /*!< Specifies the type of frame for the message that will 
                        be transmitted. This parameter can be a value of 
                        @ref CAN_remote_transmission_request */

  uint8_t DLC;     /*!< Specifies the length of the frame that will be 
                        transmitted. This parameter can be a value between 
                        0 to 8 */

  uint8_t Data[8]; /*!< Contains the data to be transmitted. It ranges from 0 
                        to 0xFF. */
} CanTxMsg;

typedef struct
{
  uint32_t StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

  uint32_t ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

  uint8_t IDE;     /*!< Specifies the type of identifier for the message that 
                        will be received. This parameter can be a value of 
                        @ref CAN_identifier_type */

  uint8_t RTR;     /*!< Specifies the type of frame for the received message.
                        This parameter can be a value of 
                        @ref CAN_remote_transmission_request */

  uint8_t DLC;     /*!< Specifies the length of the frame that will be received.
                        This parameter can be a value between 0 to 8 */

  uint8_t Data[8]; /*!< Contains the data to be received. It ranges from 0 to 
                        0xFF. */

  uint8_t FMI;     /*!< Specifies the index of the filter the message stored in 
                        the mailbox passes through. This parameter can be a 
                        value between 0 to 0xFF */
} CanRxMsg;

typedef struct{//�޷���__packed
  uint8_t   CRecvFlag0;
  uint8_t   CRecvFlag1;
  CanRxMsg RecvMessage0;
}CAN_RecvTypeDef;
extern CAN_RecvTypeDef CAN_RecvInf; 


uint8_t CAN_Mode_Init(uint8_t tsjw,uint8_t tbs2,uint8_t tbs1,uint16_t brp,uint8_t mode);//CAN��ʼ��
uint8_t CAN_Tx_Msg(uint32_t id,uint8_t ide,uint8_t rtr,uint8_t len,uint8_t *dat);		//��������
uint8_t CAN_Msg_Pend(uint8_t fifox);								//��ѯ���䱨��
void CAN_Rx_Msg(uint8_t fifox,uint32_t *id,uint8_t *ide,uint8_t *rtr,uint8_t *len,uint8_t *dat);//��������
uint8_t CAN_Tx_Staus(uint8_t mbox);  								//���ط���״̬
uint8_t CAN_Send_Msg(uint8_t* msg,uint8_t len);						//��������
uint8_t CAN_Receive_Msg(uint8_t *buf);							//��������

void CANSendTime(void);
CAN_Send_general_fun CAN_ProSend(void);
uint8_t CANScan(void);
#endif

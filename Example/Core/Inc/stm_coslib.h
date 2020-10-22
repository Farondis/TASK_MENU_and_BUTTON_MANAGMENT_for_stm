
/* Consecutive Operating System
*  COS Library Functions
*  Task and button managment
*  Including basic MultiTasking algorithm
*/

#ifndef __STM_COSLIB_H__
#define __STM_COSLIB_H__


#include "main.h"
#include "stddef.h"
#include "stdarg.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"

#define TASK_STACK_SIZE				         10
#define MENU_STACK_SIZE                5

#define MENU_GOTO_BACKWARD             0
#define MENU_GOTO_FORWARD              1

/* Button oku*/
#define __COS_BUTTON_READ(x)			(ButtonPushType_enum)(((x.GPIOx->IDR)&((uint32_t)x.Pin))>0?Rising_Edge:Falling_Edge)
/* Button port-pin ID getir*/
#define __COS_BUTTON_GET_TYPEDEF(x)		(0x01<<x)		

#define __COS_BUTTON_StackSize		  	16
#define __COS_BUTTON_ButonTypeSize		1
#define __COS_BUTTON_TICK_PRESCALER		20
	
/* 8 bit Flag*/		
typedef union __bts8_variables{
 struct bts8_struct_variables{
   uint8_t BIT_0:1;
	 uint8_t BIT_1:1;
	 uint8_t BIT_2:1;
	 uint8_t BIT_3:1;
	 uint8_t BIT_4:1;
	 uint8_t BIT_5:1;
	 uint8_t BIT_6:1;
   uint8_t BIT_7:1;
 }Bits;
 uint8_t Bulk;
}bts8_t;

/* 16 bit Flag*/
typedef union __bts16_variables{
 struct bts16_struct_variables{
   uint16_t BIT_0:1;
	 uint16_t BIT_1:1;
	 uint16_t BIT_2:1;
	 uint16_t BIT_3:1;
	 uint16_t BIT_4:1;
	 uint16_t BIT_5:1;
	 uint16_t BIT_6:1;
   uint16_t BIT_7:1;
	 uint16_t BIT_8:1;
	 uint16_t BIT_9:1;
	 uint16_t BIT_10:1;
	 uint16_t BIT_11:1;
	 uint16_t BIT_12:1;
	 uint16_t BIT_13:1;
	 uint16_t BIT_14:1;
   uint16_t BIT_15:1;
 }Bits;
 uint16_t Bulk;
}bts16_t;

/* 32 bit Flag*/
typedef union __bts32_variables{
 struct bts32_struct_variables{
    uint32_t BIT_0:1;
	uint32_t BIT_1:1;
	uint32_t BIT_2:1;
	uint32_t BIT_3:1;
	uint32_t BIT_4:1;
	uint32_t BIT_5:1;
	uint32_t BIT_6:1;
  uint32_t BIT_7:1;
	uint32_t BIT_8:1;
	uint32_t BIT_9:1;
	uint32_t BIT_10:1;
	uint32_t BIT_11:1;
	uint32_t BIT_12:1;
	uint32_t BIT_13:1;
	uint32_t BIT_14:1;
    uint32_t BIT_15:1;
	uint32_t BIT_16:1;
	uint32_t BIT_17:1;
	uint32_t BIT_18:1;
	uint32_t BIT_19:1;
	uint32_t BIT_20:1;
	uint32_t BIT_21:1;
	uint32_t BIT_22:1;
  uint32_t BIT_23:1;
	uint32_t BIT_24:1;
	uint32_t BIT_25:1;
	uint32_t BIT_26:1;
	uint32_t BIT_27:1;
	uint32_t BIT_28:1;
    uint32_t BIT_29:1;
	uint32_t BIT_30:1;
    uint32_t BIT_31:1;
 }Bits;
 uint32_t Bulk;
}bts32_t;


/* 64 bit Flag*/
typedef union __bts64_variables{
 struct bts64_struct_variables{
  uint64_t BIT_0:1;
	uint64_t BIT_1:1;
	uint64_t BIT_2:1;
	uint64_t BIT_3:1;
	uint64_t BIT_4:1;
	uint64_t BIT_5:1;
	uint64_t BIT_6:1;
  uint64_t BIT_7:1;
	uint64_t BIT_8:1;
	uint64_t BIT_9:1;
	uint64_t BIT_10:1;
	uint64_t BIT_11:1;
	uint64_t BIT_12:1;
	uint64_t BIT_13:1;
	uint64_t BIT_14:1;
  uint64_t BIT_15:1;
	uint64_t BIT_16:1;
	uint64_t BIT_17:1;
	uint64_t BIT_18:1;
	uint64_t BIT_19:1;
	uint64_t BIT_20:1;
	uint64_t BIT_21:1;
	uint64_t BIT_22:1;
  uint64_t BIT_23:1;
	uint64_t BIT_24:1;
	uint64_t BIT_25:1;
	uint64_t BIT_26:1;
	uint64_t BIT_27:1;
	uint64_t BIT_28:1;
  uint64_t BIT_29:1;
	uint64_t BIT_30:1;
  uint64_t BIT_31:1;
 }Bits;
 uint64_t Bulk;
}bts64_t;

/* Universal type def*/
typedef union __universal_void_union{
 uint8_t Integer8u[4];
 int8_t Integer8s[4];
 uint16_t Integer16u[2];
 int16_t Integer16s[2];
 uint32_t Integer32u;
 int32_t Integer32s;
 float Floating;
}void_handle;

/* Hysteresis filtresi*/
typedef struct __hysteresis_variables{
    float ActualValue;
    float Hysteresis;
    float BorderSize;
    uint8_t Step;
}hfilter_t;

/* Weighted smooth filtresi*/
typedef struct __weighted_smooth_variables{
    float SmoothedValue;
    float *Sample;  
    uint8_t InstanceCount;
}sfilter_t;

/* Kalman filtresi*/
typedef struct __kalman_variables{
	float MeasuredVal;
	float ExpectedVal;
	float KalmanGain;
	float ProcessNoise;
	float MeasurementNoise;
	float ErrorRate;
	float P_ErrorRate;
	float R,Q;
}kfilter_t;

/* Button push type*/
typedef enum __button_irq_variables
{
    IRQ_OverFlow,
	IRQ_Break,
	IRQ_ContinuousUpdate,
	IRQ_StatusUpdate
}ButtonIrq_enum;

/* Edge define*/
typedef enum __button_pushtype_variables
{
	Falling_Edge=GPIO_PIN_RESET,
	Rising_Edge=GPIO_PIN_SET
}ButtonPushType_enum;

/* Button State*/
typedef enum __button_activestate_variables
{
	Passive,
	Active
}ButtonState_enum;

/* Buton pin ve portlari tanimla*/
typedef struct __button_typedef_variables
{
 GPIO_TypeDef *GPIOx;
 uint16_t Pin;
}button_typedef;
/* port - pin tanimlari end*/

/* Task durum enum*/
typedef enum __task_state_variables_enum{
    TASK_SUSPEND,
    TASK_RUN
}TaskStateEnum;

/* Task manager değişkenleri*/
typedef struct __taskmanager_variables{
    uint32_t CallBackTime,LastCallTime;
    TaskStateEnum State;
}task_t;

/* Menü değişkenleri*/
typedef struct __menu_variables{
  struct __menu_variables *firstchild;
  struct __menu_variables *sibling;
}menu_t;

/* Button değişkenleri*/
typedef struct __button_struct_variables
{
	uint16_t ButtonTypedef;
	uint16_t Tick;
	uint16_t Counter;
	bool IsPushed;
	ButtonPushType_enum PushType;
	ButtonIrq_enum IRQ;
	ButtonState_enum State;
}button_t;

/* Stack değişkenleri*/
typedef struct __stack_variables{
  uint8_t Button_CompStack,ButtonStackSize;
	uint8_t TaskManagerStackCounter; 
  uint8_t MenuStackCounter; 
  menu_t *MenuStack[MENU_STACK_SIZE];
  button_t *ButtonStack[__COS_BUTTON_StackSize];
	task_t *TaskStack[TASK_STACK_SIZE];
}stack_t;

#if __COS_BUTTON_StackSize<8
extern __IO bts8_t BUTTON_IRQ_FLAG;
#elif __COS_BUTTON_StackSize<16
extern __IO bts16_t BUTTON_IRQ_FLAG;
#elif __COS_BUTTON_StackSize<32
extern __IO bts32_t BUTTON_IRQ_FLAG;
#endif

extern stack_t COS_Stack;


/*Hysteresis filter ------------------------------------------------------------------------*/
extern void COS_FILTER_CreateHysteresis(hfilter_t *Filter, float BorderSize, float Hysteresis);
extern float COS_FILTER_CalcHysteresis(hfilter_t *Filter, float Value);
/*end of Hysteresis filter -----------------------------------------------------------------*/

/*Smooth filter ----------------------------------------------------------------------------*/
extern void  COS_FILTER_CreateWeiSmooth(sfilter_t *Filter, uint8_t InstanceCount);
extern float COS_FILTER_CalcWeiSmooth(sfilter_t *Filter, float NewSample);
/*end of Smooth filter ---------------------------------------------------------------------*/

/*Kalman filter ----------------------------------------------------------------------------*/
extern void  COS_FILTER_CreateKalman(kfilter_t *Filter, float R, float Q, float Meas_Noise, float Proce_Noise);
extern float COS_FILTER_CalcKalman(kfilter_t *Filter, float Sample);
extern void  COS_FILTER_SetKalmanConstants(kfilter_t *Filter,float R, float Q);
extern void  COS_FILTER_SetKalmanNoise(kfilter_t *Filter,float Meas_Noise, float Proce_Noise);
/*end of Kalman filter ---------------------------------------------------------------------*/

/* Range filter hesapla ---------------------------------------------------------------------------------*/
extern float COS_FILTER_ReMapping(float Value, float Actual_Max, float Actual_Min, float New_Max, float New_Min);
/* end of Range filter hesapla --------------------------------------------------------------------------*/


/* Menü olay fonksiyonu */
extern void COS_MENU_EventHandle(menu_t *Menu);
/* Görev olay fonksiyonlari */
extern void COS_TASK_TimeEllapsedCallBack(task_t *Task);
extern void COS_TASK_FastDriveCallBack(void);


/*             MENÜ functions           */
/* ROOT belirle */
extern void COS_MENU_SetRoot(menu_t *Menu);

/* Alt menüye ekle */
extern void COS_MENU_AddTo(menu_t *Menu, menu_t *NewChild);

/* Alt menüye gir */
extern void COS_MENU_MoveToChild(uint8_t Child);

/* Menü sirasinda gez */
extern void COS_MENU_Move(bool GoTo);

/* Üst menüye git */
extern void COS_MENU_GetBack(void);

/* Root git */
extern void COS_MENU_BackToRoot(void);

/* Menü execute*/ 
extern void COS_MENU_Execute(void);


/*             BUTTON functions           */
/* Yeni buton olustur*/ 
extern void COS_BUTTON_Create(button_t *Button,
							  uint16_t ButtonTypedef,
							  uint16_t Tick,										
							  ButtonPushType_enum PushType,
							  ButtonIrq_enum IRQ);
/* Buttonları resetle*/
extern void COS_BUTTON_ResetAll(void);

/* Button scheduler*/ 
extern void COS_BUTTON_Scheduler(void);


/*             TASK functions           */
/* Task zamanini sifirla*/ 
extern void COS_TASK_RefreshTimer(task_t *Task);

/* Task durumunu değiştir*/ 
extern void COS_TASK_State(task_t *Task, TaskStateEnum State);

/* Yeni Task olustur*/ 
extern void COS_TASK_Create(task_t *Task, uint32_t CallBackTime, TaskStateEnum State);

/* Task zaman kontrolü*/
extern bool COS_TASK_IsTimeEllapsed(task_t *Task);

/* Task scheduler*/ 
extern void COS_TASK_Scheduler(void);

/* Timeout kontrolü*/
extern bool COS_TASK_TimeoutCheck(uint32_t *LastCollapse, uint32_t TimeMs);

/* Delay ms*/
extern void COS_DelayMs(uint32_t TimeMs);



#endif /*____STM_COSLIB_H___*/


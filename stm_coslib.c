#include "stm_coslib.h"


#if __COS_BUTTON_StackSize<8
 __IO bts8_t BUTTON_IRQ_FLAG;
#elif __COS_BUTTON_StackSize<16
 __IO bts16_t BUTTON_IRQ_FLAG;
#elif __COS_BUTTON_StackSize<32
 __IO bts32_t BUTTON_IRQ_FLAG;
#endif



/* Button port and pin definitions*/
button_typedef Button_Def[__COS_BUTTON_ButonTypeSize]=
{
	{B1_GPIO_Port,B1_Pin}
};

stack_t COS_Stack;
task_t Default_ButtonTask;
task_t Default_ScreenRefresh={1000,0,TASK_RUN}; //Menu callback, 1sec
task_t FastDriveTask={150,0,TASK_RUN}; //150 tick high priority task


/* set ROOT */
void COS_MENU_SetRoot(menu_t *Menu){
  COS_Stack.MenuStack[0]=Menu;
}

/* Add to sublayer */
void COS_MENU_AddTo(menu_t *Menu, menu_t *NewChild)
{
  if(Menu->firstchild==NULL){Menu->firstchild=NewChild;}
  else{
   menu_t *p; p=Menu->firstchild;
   if(p->sibling==NULL){
       p->sibling=NewChild;
   }else{
      do{
        p=p->sibling;
      }while(p->sibling!=NULL);
      p->sibling=NewChild;
   }
  }
}

/* Go to sublayer */
/*@param Child
    Child -> order of child, 1 ->first child, 2-> firt child sibling, etc...
*/
void COS_MENU_MoveToChild(uint8_t Child){
  if(Child>0){
    menu_t *p; 

  if(Child==1){p=COS_Stack.MenuStack[COS_Stack.MenuStackCounter]->firstchild;}
  else  if(Child==2){p=COS_Stack.MenuStack[COS_Stack.MenuStackCounter]->firstchild->sibling;}
  else{
    Child-=3;
    p=COS_Stack.MenuStack[COS_Stack.MenuStackCounter]->firstchild->sibling;
      do{
        p=p->sibling;
      }while(Child--);
  }
  COS_Stack.MenuStackCounter++;
  COS_Stack.MenuStack[COS_Stack.MenuStackCounter]=p;
}
}

/* Walk on the tree */
/*@param GoTo
    MENU_GOTO_FORWARD -> go to right
	MENU_GOTO_BACKWARD -> go to left
*/
void COS_MENU_Move(bool GoTo)
{
  menu_t *p; 
  if(COS_Stack.MenuStack[0]!=NULL)
  {
	p=COS_Stack.MenuStack[COS_Stack.MenuStackCounter];
	if(GoTo==MENU_GOTO_FORWARD)
	{
			if(p->sibling!=NULL)
			{
				p=(p->sibling);
			}
			else
			{
			    p=(COS_Stack.MenuStack[COS_Stack.MenuStackCounter-1]->firstchild);
			}
			
			COS_Stack.MenuStack[COS_Stack.MenuStackCounter]=p;
	}
	else if(GoTo==MENU_GOTO_BACKWARD)
	{
		if(COS_Stack.MenuStack[COS_Stack.MenuStackCounter-1]->firstchild==COS_Stack.MenuStack[COS_Stack.MenuStackCounter])
		{
			do
			{
				p=p->sibling;
			}
			while(p->sibling!=NULL);
		}
		else
		{
			p=COS_Stack.MenuStack[COS_Stack.MenuStackCounter-1]->firstchild;
			while(p->sibling!=COS_Stack.MenuStack[COS_Stack.MenuStackCounter])
			{
				p=p->sibling;
			}
		}
	    COS_Stack.MenuStack[COS_Stack.MenuStackCounter]=p;
	}
  }
}

/* Go to upper layer */
void COS_MENU_GetBack(void){
  if(COS_Stack.MenuStackCounter>0)
  {
     COS_Stack.MenuStack[COS_Stack.MenuStackCounter]=NULL;
	   COS_Stack.MenuStackCounter--;
  }
}

/* Go to Root*/
void COS_MENU_BackToRoot(void){
  while(COS_Stack.MenuStackCounter>0)
  {
     COS_Stack.MenuStack[COS_Stack.MenuStackCounter]=NULL;
	   COS_Stack.MenuStackCounter--;
  }
}

/* Menü execute*/ 
void COS_MENU_Execute(void)
{
  COS_MENU_EventHandle((COS_Stack.MenuStack[COS_Stack.MenuStackCounter]));
  BUTTON_IRQ_FLAG.Bulk=0;
}

/* Yeni button olustur*/ 
void COS_BUTTON_Create(	 button_t *Button,
						 uint16_t ButtonTypedef,
						 uint16_t Tick,										
						 ButtonPushType_enum PushType,
						 ButtonIrq_enum IRQ)
{
	Button->ButtonTypedef=ButtonTypedef;
	Button->Tick=Tick/__COS_BUTTON_TICK_PRESCALER;
	Button->PushType=PushType;
	Button->IRQ=IRQ;
	Button->State=Active;
	
	COS_Stack.ButtonStack[COS_Stack.ButtonStackSize]=Button;
	COS_Stack.ButtonStackSize++;
}



bts8_t MultibuttonState;
bool ButtonIsPushed;
/* Button execute*/ 
void COS_BUTTON_Scheduler(void){


 /*Button 1 kontrol et
	 basildiyse counter ++
	 counter = tick ise, button push true
	 overflow IRQ ise flag kaldir ve reset için button birakilmasini bekle
   break IRQ ise button birakilmasini bekle ve flag kaldir
	 countinuous IRQ ise buton birakilana kadar flag kaldir
	 status IRQ ise durum degisiminde flag kaldir
	*/
	

	for(size_t k=0; k<__COS_BUTTON_ButonTypeSize; k++)
	{
		if(Button_Def[k].GPIOx==NULL) break;
		
			if(__COS_BUTTON_READ(Button_Def[k])==Falling_Edge)
			{
				MultibuttonState.Bulk|=(0x01<<k);
			}
			else
			{
				MultibuttonState.Bulk&=(~(0x01<<k));
			}
	}


for(size_t i=0; i<COS_Stack.ButtonStackSize; i++)
{
 if(!ButtonIsPushed)
 {
		if(COS_Stack.ButtonStack[i]->State==Active)
		{
					if(MultibuttonState.Bulk==COS_Stack.ButtonStack[i]->ButtonTypedef
						&& !COS_Stack.ButtonStack[i]->IsPushed)
					{
						if(COS_Stack.ButtonStack[i]->Counter<COS_Stack.ButtonStack[i]->Tick)
						{
								COS_Stack.ButtonStack[i]->Counter++;
						}
						else
						{
							  COS_Stack.ButtonStack[i]->IsPushed=true;
						}
					}
					else if(MultibuttonState.Bulk==COS_Stack.ButtonStack[i]->ButtonTypedef 
									&& COS_Stack.ButtonStack[i]->IsPushed)
					{
							if(COS_Stack.ButtonStack[i]->IRQ==IRQ_OverFlow || COS_Stack.ButtonStack[i]->IRQ==IRQ_StatusUpdate)
							{
								BUTTON_IRQ_FLAG.Bulk=(0x01<<(i));
								ButtonIsPushed=true;
							}
					}
					else if((MultibuttonState.Bulk&COS_Stack.ButtonStack[i]->ButtonTypedef)!=COS_Stack.ButtonStack[i]->ButtonTypedef 
									&& COS_Stack.ButtonStack[i]->IsPushed)
					{
						if(COS_Stack.ButtonStack[i]->IRQ==IRQ_Break || COS_Stack.ButtonStack[i]->IRQ==IRQ_StatusUpdate)
						{
							BUTTON_IRQ_FLAG.Bulk=(0x01<<(i));
							ButtonIsPushed=true;
						}
						COS_Stack.ButtonStack[i]->IsPushed=false;
					}
					else
					{
						COS_Stack.ButtonStack[i]->IsPushed=false;
						COS_Stack.ButtonStack[i]->Counter=0;
					}
		}		
  }
	else
	{
		if(MultibuttonState.Bulk==0)
		{
				COS_BUTTON_ResetAll();
				ButtonIsPushed=false;
		}
	}	
}	

	
}


/* Reset all buttons*/
void COS_BUTTON_ResetAll()
{
	for(size_t z=0; z<COS_Stack.ButtonStackSize; z++)
	{
			  COS_Stack.ButtonStack[z]->Counter=0;
				COS_Stack.ButtonStack[z]->IsPushed=false;
	}
}

/* Create task */
/* @param *Task,CallBackTime,State,
    *Task -> new task pointer
	CallBackTime -> task callback time
	State -> task run state
			 TASK_RUN
			 TASK_SUSPEND
*/
void COS_TASK_Create(task_t *Task, uint32_t CallBackTime, TaskStateEnum State)
{
    Task->CallBackTime=CallBackTime;
    Task->State=State;
    COS_Stack.TaskStack[COS_Stack.TaskManagerStackCounter]=Task;
    COS_Stack.TaskManagerStackCounter++;
}

/*Change task state, TASK_RUN - TASK_SUSPEND */
void COS_TASK_State(task_t *Task, TaskStateEnum State)
{
    Task->State=State;
}

/*Refresh task counter*/
void COS_TASK_RefreshTimer(task_t *Task)
{
    Task->LastCallTime=uwTick;
}

extern task_t Refresh_Screen;
/* Task Manager */
void COS_TASK_Scheduler()
{
Default_ButtonTask.CallBackTime=__COS_BUTTON_TICK_PRESCALER;
    while(1)
	{
			    /* Button check*/
			    if(COS_TASK_IsTimeEllapsed(&Default_ButtonTask))
			    {
			    				COS_BUTTON_Scheduler();
	        }  
			    	
			    if(BUTTON_IRQ_FLAG.Bulk>0)
			    {
			    	/* Button execute*/
			          COS_MENU_Execute();
			    	/* Display execute*/
			    	    COS_MENU_Execute();
		        }
					
					/* Menu refresh control*/
			    if(COS_TASK_IsTimeEllapsed(&Default_ScreenRefresh))
			    {
			    		COS_MENU_Execute();
	        } 

					/*Task kontrolü*/
				for(uint8_t i=0; i<COS_Stack.TaskManagerStackCounter; i++)
				{
					if(COS_TASK_IsTimeEllapsed(COS_Stack.TaskStack[i]) && (COS_Stack.TaskStack[i]->State==TASK_RUN))
					{
					COS_TASK_TimeEllapsedCallBack(COS_Stack.TaskStack[i]);
					}
					
					if(COS_TASK_IsTimeEllapsed(&FastDriveTask))
					{
					   COS_TASK_FastDriveCallBack();
					}
							
				}
	   /*end of task kontrolü*/
    }
}

/* New Hysteresis filter*/
void COS_FILTER_CreateHysteresis(hfilter_t *Filter, float BorderSize, float Hysteresis)
{
    Filter->Hysteresis=Hysteresis;
    Filter->Step=1;
    Filter->BorderSize=BorderSize;
}

/* Calculate new value */
float COS_FILTER_CalcHysteresis(hfilter_t *Filter, float Value)
{
  if(Value>(Filter->Step*Filter->BorderSize))
  {
    if(Value>((Filter->Step*Filter->BorderSize)+Filter->Hysteresis))
	{
      Filter->Step++;
    }
    Filter->ActualValue=(Filter->Step*Filter->BorderSize)-Filter->Hysteresis;
  }
  else if(Value<((Filter->Step-1)*Filter->BorderSize))
  {
    if(Value<(((Filter->Step-1)*Filter->BorderSize)-Filter->Hysteresis))
	{
      Filter->Step--;
    }
     Filter->ActualValue=(Filter->Step*Filter->BorderSize)+Filter->Hysteresis;
  }
  else
  {
   Filter->ActualValue=Value;
  }
return Filter->ActualValue;
}

/* New Weighted Smooth Filter*/
void COS_FILTER_CreateWeiSmooth(sfilter_t *Filter, uint8_t InstanceCount)
{
    float *Buffer;
    Buffer=(float*)malloc(sizeof(float)*(InstanceCount+1));
    Filter->InstanceCount=InstanceCount;
    Filter->Sample=Buffer;
}

volatile uint8_t k;
/* Calculate Weighted Smooth value*/
float COS_FILTER_CalcWeiSmooth(sfilter_t *Filter, float NewSample)
{
    auto float value;
    k=1;
    auto float *p;
    p=Filter->Sample;
    for(auto uint8_t i=0; i<(Filter->InstanceCount-1);i++)
    {
		*p=*(p+1); p++;
	}

    *p=NewSample;
    p=Filter->Sample;
    for(auto uint8_t i=0; i<Filter->InstanceCount; i++)
	{
        if(i>(Filter->InstanceCount>>1))
	    {
           value+=(*p)*(Filter->InstanceCount-i); k+=(Filter->InstanceCount-i);
        }
	    else
	    {
           value+=(*p)*(i+1); k+=(i+1);
        }
        p++;
    }
         Filter->SmoothedValue=value/k;
   return Filter->SmoothedValue;
}

/* Create Kalman filter */
void COS_FILTER_CreateKalman(kfilter_t *Filter, float R, float Q, float Meas_Noise, float Proce_Noise)
{
	Filter->R=R;
	Filter->Q=Q;
	Filter->MeasurementNoise=Meas_Noise;
	Filter->ProcessNoise=Proce_Noise;
	Filter->P_ErrorRate=1.0f;
}

/* Calculate Kalman value */
float COS_FILTER_CalcKalman(kfilter_t *Filter, float Sample)
{
  Filter->MeasuredVal=Sample+Filter->MeasurementNoise;
  Filter->ExpectedVal=Filter->ExpectedVal+Filter->ProcessNoise;
	Filter->ErrorRate=Filter->P_ErrorRate+Filter->Q;
	
  Filter->KalmanGain=(Filter->ErrorRate/(Filter->ErrorRate+Filter->R));
  Filter->ExpectedVal=Filter->ExpectedVal+(Filter->KalmanGain*(Filter->MeasuredVal-Filter->ExpectedVal));
	
  Filter->ErrorRate=((1.0f-Filter->KalmanGain)*Filter->P_ErrorRate);
 return Filter->ExpectedVal;
}

/* Kalman Constants*/
void COS_FILTER_SetKalmanConstants(kfilter_t *Filter, float R, float Q)
{
	Filter->R=R;
	Filter->Q=Q;
}

/* Kalman noises*/
void COS_FILTER_SetKalmanNoise(kfilter_t *Filter,float Meas_Noise, float Proce_Noise)
{
	Filter->MeasurementNoise=Meas_Noise;
	Filter->ProcessNoise=Proce_Noise;
}

/* Remapping */
float COS_FILTER_ReMapping(float Value, float Actual_Max, float Actual_Min, float New_Max, float New_Min)
{
	auto float first=Actual_Max-Actual_Min;
	auto float second=New_Max-New_Min;
	Value=Value-first;
return ((second*Value/first)+second);
}

/* Time control*/
bool COS_TASK_IsTimeEllapsed(task_t *Task)
{
    if(((Task->LastCallTime)>uwTick && (((0xFFFFFFF0-(Task->LastCallTime))+Task->CallBackTime)<=uwTick)) 
         || (Task->LastCallTime<uwTick && ((Task->LastCallTime+Task->CallBackTime)<=uwTick)))
    {
		 Task->LastCallTime=uwTick;
	    return true;
    }
  return false;
}

/* Timeout control*/
bool COS_TASK_TimeoutCheck(uint32_t *LastCollapse, uint32_t TimeMs)
{
    if(((*LastCollapse)>HAL_GetTick() && (((0xFFFFFFF0-(*LastCollapse))+TimeMs)<=HAL_GetTick())) 
      || (*LastCollapse<HAL_GetTick() && ((*LastCollapse+TimeMs)<=HAL_GetTick())))
	{
		 (*LastCollapse)=HAL_GetTick();
	  return true;
    }
 return false;
}

/* Delay ms*/
void COS_DelayMs(uint32_t TimeMs)
{
  auto uint32_t LastCallTime=uwTick;
  while(!COS_TASK_TimeoutCheck(&LastCallTime,TimeMs))
	{

	 		/* Button kontrolü*/
			if(COS_TASK_IsTimeEllapsed(&Default_ButtonTask))
			{
				COS_BUTTON_Scheduler();
	        }
				
			if(BUTTON_IRQ_FLAG.Bulk>0)
			{
				/* Button işle*/
			    COS_MENU_Execute();
				COS_MENU_Execute();
		    }
	}
}


/* Menu event handle */
__weak void COS_MENU_EventHandle(menu_t *Menu)
{

		// To do...
	
}

/* Task fast drive callback*/
__weak void COS_TASK_FastDriveCallBack(void)
{
	
	  // To do...

}
/* Task time ellapsed callback*/
__weak void COS_TASK_TimeEllapsedCallBack(task_t *Task)
{

		// To do...
	
}

/* Redefinition of hal function*/
void HAL_IncTick(void)
{
	if(uwTick>0xFFFFFFF0)
	{	
		uwTick=0;
	}
    else
	{
	    uwTick += uwTickFreq;
	}
}





/*********************************************************************
 * @file        APP_SYS.c
 * @brief       Template_BriefDescription.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      xxxxxx
 * @date        jj/mm/yyyy
 * @version     1.0
 */






// ********************************************************************
// *                      Includes
// ********************************************************************
#include "./APP_CFG/ConfigFiles/APPSYS_ConfigPrivate.h"
#include "Library/SafeMem/SafeMem.h"
#include "./APP_SYS.h"

// ********************************************************************
// *                      Defines
// ********************************************************************
#define APPSYS_FILE_NAME_LEN ((t_uint8)64)
#define APPSYS_SAFE_BLOCK_MAX_ATTEMPT ((t_uint8)4)
// ********************************************************************
// *                      Types
// ********************************************************************
typedef struct 
{
    t_uint16 debugInfo_u16;
    char file_ac[APPSYS_FILE_NAME_LEN];
    t_uint32 line_u32;
} t_sAPPSYS_AssertInfo;
/* CAUTION : Automatic generated code section for Enum: Start */

/* CAUTION : Automatic generated code section for Enum: End */
//-----------------------------ENUM TYPES-----------------------------//


/* CAUTION : Automatic generated code section for Structure: Start */

/* CAUTION : Automatic generated code section for Structure: End */
//-----------------------------STRUCT TYPES---------------------------//
/* CAUTION : Automatic generated code section : Start */

/* CAUTION : Automatic generated code section : End */
//-----------------------------TYPEDEF TYPES---------------------------//
// ********************************************************************
// *                      Prototypes
// ********************************************************************
	
// ********************************************************************
// *                      Variables
// ********************************************************************
static t_eCyclicModState g_ModuleState_ae[APPSYS_MODULE_NB];
static t_cbAPPSYS_FastTask * g_ModFastTask_apcb[APPSYS_MODULE_NB];
static t_eCyclicModState g_AppSysModuleState_e = STATE_CYCLIC_PREOPE;
static t_uint32 g_CyclicDuration_u32 = (t_uint32)0;
static t_uint32 g_fastTaskDuration_u32 = (t_uint32)0;
static t_sSafeMem_BlockInfo g_sfbk_isFastTaskOn_s;
static t_sSafeMem_BlockInfo g_sfbk_mskfastTask_s;
static t_bool g_isFastTaskON_b = (t_bool)False;
static t_uint16 g_mskFastTaskCall_u16 = (t_uint16)0; /**< to know the people to call */
static t_bool g_lockAssert_b = (t_bool)False;
static t_sAPPSYS_AssertInfo g_AssertInfo_s;
//********************************************************************************
//                      Local functions - Prototypes
//********************************************************************************
/**
*
*	@brief  ResAlloc 
*
*/
static t_eReturnCode s_APPSYS_ResAlloc(void); 
/**
*
*	@brief  Call driver cyclic function
*
*/
static t_eReturnCode s_APPSYS_PreOperational();
/**
*
*	@brief  Call driver cyclic function
*
*/
static t_eReturnCode s_APPSYS_Operational();
/**
*
*	@brief  Call driver cyclic function
*
*/
static void s_APPSYS_Set_ModulesCyclic();
/**
*
*	@brief  Call driver cyclic function
    @note 15 * 4 compute Ramp & Compute Freq = 5 ms
*
*/
static void s_APPSYS_FastTask(t_eFMKTIM_InterruptLineType f_InterruptType_e, t_uint8 f_InterruptLine_u8);
//****************************************************************************
//                      Public functions - Implementation
//********************************************************************************
/*********************************
 * APPSYS_Init
 *********************************/
void APPSYS_Init(void)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint8 modIndex_u8 = 0;
    // set sys confgiguration
    Ret_e = s_APPSYS_ResAlloc();

    if(Ret_e == RC_OK)
    {
        Ret_e = FMKCPU_Set_HardwareInit();
    }
    if(Ret_e == RC_OK)
    {
        Ret_e = FMKCPU_Set_SysClockCfg(APPSYS_SYSTEM_CORE_SPEED);
    }
    if(Ret_e == RC_OK)
    {
        //Ret_e = FMKCPU_Set_WwdgCfg((t_eFMKCPu_WwdgResetPeriod)FMKCPU_WWDG_RESET_CFG);
    }
    if(Ret_e == RC_OK)
    {
        for(modIndex_u8 = (t_uint8)0 ; (modIndex_u8 < APPSYS_MODULE_NB) ; modIndex_u8++)
        {
            g_ModFastTask_apcb[modIndex_u8] = NULL_FUNCTION;
            Ret_e = c_AppSys_ModuleFunc_apf[modIndex_u8].Init_pcb();

            if(Ret_e != RC_OK)
            {
                ASSERT((t_uint16)modIndex_u8);
            }
        }
    }
    if(Ret_e == RC_OK)
    {
        Ret_e = FMKTIM_Set_EvntTimerCfg(APPSYS_ITLINE_FASTTASK,
                                        APPSYS_ELASPED_TIME_FASTTASK,
                                        s_APPSYS_FastTask);
    }

    g_AssertInfo_s.debugInfo_u16 = (t_uint16)0;
    g_AssertInfo_s.line_u32 = (t_uint32)0;

    if(Ret_e < RC_OK)
    {    
        g_AppSysModuleState_e = STATE_CYCLIC_ERROR;
    }
    return;
}
/*********************************
 * APPSYS_Init
 *********************************/
void APPSYS_Cyclic(void)
{
    t_eReturnCode Ret_e = RC_OK;
     
    switch(g_AppSysModuleState_e)
    {
        case STATE_CYCLIC_PREOPE:
        {/* In Preope Mode AppSys called every cycle and wait every module are ready for Ope Mode*/
            Ret_e = s_APPSYS_PreOperational();
            //---------Update Module State------------//
            if(Ret_e == RC_OK)
            {
                g_AppSysModuleState_e = STATE_CYCLIC_OPE; 
            }
            else if(Ret_e < RC_OK)
            {
                ASSERT((t_uint16)Ret_e);
                g_AppSysModuleState_e = STATE_CYCLIC_ERROR;
            }
            break;
        }
        case STATE_CYCLIC_OPE:
        {
            Ret_e = s_APPSYS_Operational();
            break;
        }
        case STATE_CYCLIC_BUSY:
        {
            break;
        }
        case STATE_CYCLIC_CFG:
        case STATE_CYCLIC_WAITING:
        case STATE_CYCLIC_ERROR:
        default:
        {
            // Nothing to do infinite loop
            break;
        }
    }

    return;
}

/*********************************
 * APPSYS_AssertionTrap
 *********************************/
void APPSYS_AssertionTrap(  t_uint16 f_Info_u16, 
                            const char * f_file_str, 
                            t_uint32 f_line_u32,
                            t_uint32 f_captureTime_u32)
{

    if(g_lockAssert_b == (t_bool)False)
    {
        g_AssertInfo_s.debugInfo_u16 = f_Info_u16;
        strncpy(g_AssertInfo_s.file_ac, f_file_str, APPSYS_FILE_NAME_LEN - 1);
        g_AssertInfo_s.file_ac[APPSYS_FILE_NAME_LEN - 1] = '\0';  // Assurer la terminaison
        g_AssertInfo_s.line_u32 = f_line_u32;
    }
    return;
}

/*********************************
* APPSYS_AddFastTask
*********************************/
t_eReturnCode APPSYS_AddFastTask(t_eAppSys_ModuleList f_ModuleId_e, t_cbAPPSYS_FastTask * f_moduleFastTask_pcb)
{
    t_eReturnCode Ret_e = RC_OK;
    if((f_ModuleId_e < APPSYS_MODULE_NB)
    && (f_moduleFastTask_pcb != NULL_FUNCTION))
    {
        g_ModFastTask_apcb[(t_uint8)(f_ModuleId_e)] = f_moduleFastTask_pcb;
    }
    else 
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
        ASSERT((t_uint16)(f_ModuleId_e));
    }

    return Ret_e;
}

/*********************************
 * APPSYS_SetFastTaskState
 *********************************/
t_eReturnCode APPSYS_SetFastTaskState(t_eAppSys_ModuleList f_ModuleId_e,  t_eAPPSYS_FastTaskState f_state_e)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint16 mskfastTaskCall_u16;

    Ret_e = SMB_Read(&g_sfbk_mskfastTask_s, &mskfastTaskCall_u16);


    if((f_ModuleId_e >= APPSYS_MODULE_NB)
    || (f_state_e > APPSYS_FAST_TASK_ENABLE))
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
        ASSERT((t_uint16)0);
    }
    else if(Ret_e == RC_OK)
    {
        if(f_state_e == APPSYS_FAST_TASK_ENABLE)
        {
            SETBIT_16B(mskfastTaskCall_u16, (t_uint8)f_ModuleId_e);
        }
        else if(f_state_e == APPSYS_FAST_TASK_DISABLE)
        {
            RESETBIT_16B(mskfastTaskCall_u16, (t_uint8)f_ModuleId_e);
        }
        else 
        {
            Ret_e = RC_WARNING_NO_OPERATION;
        }
        if(Ret_e == RC_OK)
        {
            Ret_e = SMB_Write(&g_sfbk_mskfastTask_s, &mskfastTaskCall_u16);
        }
    }

    return Ret_e;
}
//********************************************************************************
//                      Local functions - Implementation
//********************************************************************************
/*********************************
 * s_APPSYS_Set_ModulesCyclic
 *********************************/
static void s_APPSYS_Set_ModulesCyclic(void)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint8 modIndex_u8;

    for(modIndex_u8 = (t_uint8)0 ; (modIndex_u8 < (t_uint8)APPSYS_MODULE_NB) && (Ret_e >= RC_OK) ; modIndex_u8++)
    {
        if(c_AppSys_ModuleFunc_apf[modIndex_u8].Cyclic_pcb != NULL_FUNCTION)
        {
            Ret_e = c_AppSys_ModuleFunc_apf[modIndex_u8].Cyclic_pcb();
        }
        if(Ret_e < RC_OK)
        {
            ASSERT((t_uint32)modIndex_u8);
        }
        //---- update mod State ----//
        if(c_AppSys_ModuleFunc_apf[modIndex_u8].GetState_pcb != NULL_FUNCTION)
        {
            (void)c_AppSys_ModuleFunc_apf[modIndex_u8].GetState_pcb(&g_ModuleState_ae[modIndex_u8]);
        }
    }

    //---- reset lock assert ----//
    g_lockAssert_b = (t_bool)False;

    return;
}

/*********************************
 * s_APPSYS_ResAlloc
 *********************************/
static t_eReturnCode s_APPSYS_ResAlloc(void)
{
    t_eReturnCode Ret_e;

    Ret_e = SMB_SecureBlockInit(&g_sfbk_isFastTaskOn_s,
                                    &g_isFastTaskON_b,
                                    sizeof(g_isFastTaskON_b),
                                    APPSYS_SAFE_BLOCK_MAX_ATTEMPT);
    if(Ret_e == RC_OK)
    {
        Ret_e = SMB_SecureBlockInit(&g_sfbk_mskfastTask_s,
                                        &g_mskFastTaskCall_u16,
                                        sizeof(g_mskFastTaskCall_u16),
                                        APPSYS_SAFE_BLOCK_MAX_ATTEMPT);
    }
    return Ret_e;
}

/*********************************
 * s_APPSYS_PreOperational
 *********************************/
static t_eReturnCode s_APPSYS_PreOperational(void)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint8 modIndex_u8;
    t_uint8 ModuleInitCnt_u8 = 0;

    s_APPSYS_Set_ModulesCyclic();

    for(modIndex_u8 = (t_uint8)0 ; (modIndex_u8 <  (t_uint8)APPSYS_MODULE_NB) && (Ret_e == RC_OK) ; modIndex_u8++)
    {
        if(g_ModuleState_ae[modIndex_u8] == STATE_CYCLIC_WAITING)
        {
            ModuleInitCnt_u8 += 1;
        }
    }
    
    if(ModuleInitCnt_u8 >= (t_uint8)APPSYS_MODULE_NB)
    {// set the all state module to pre-ope
        for(modIndex_u8 = (t_uint8)0 ; (modIndex_u8 <  (t_uint8)APPSYS_MODULE_NB) && (Ret_e == RC_OK) ; modIndex_u8++)
        {
            if(g_ModuleState_ae[modIndex_u8] == STATE_CYCLIC_WAITING)
            {
                Ret_e = c_AppSys_ModuleFunc_apf[modIndex_u8].SetState_pcb(STATE_CYCLIC_PREOPE);
            }
        }
        Ret_e = RC_OK;
        
    }
    else 
    {
        Ret_e = RC_WARNING_PENDING;
    }
    return Ret_e;
}

/*********************************
 * s_APPSYS_Operational
 *********************************/
static t_eReturnCode s_APPSYS_Operational(void)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint32 currentCnt_u32 = 0;
    static t_uint32 s_previousCnt_u32 = 0;
    t_uint32 elapsedTime_u32 =  0;
    t_bool isFastTaskON_b = False;
    t_uint16 mskfastTask_u16 = (t_uint16)0;

    FMKCPU_GetTick(&currentCnt_u32);

    //Ret_e = FMKCPU_ResetWwdg();
    
    Ret_e = SMB_Read(&g_sfbk_mskfastTask_s, &mskfastTask_u16);
    if(Ret_e ==  RC_OK)
    {
        Ret_e = SMB_Read(&g_sfbk_isFastTaskOn_s, &isFastTaskON_b);
    }
    if(Ret_e == RC_OK)
    {
        elapsedTime_u32 = (t_uint32)(currentCnt_u32 - s_previousCnt_u32);
        if((elapsedTime_u32) > APPSYS_ELAPSED_TIME_CYCLIC)
        {
            // reset whatchdog for fmk/app cycle
            s_previousCnt_u32 = currentCnt_u32;
            s_APPSYS_Set_ModulesCyclic();

            FMKCPU_GetTick(&currentCnt_u32); 
            g_CyclicDuration_u32 = (t_uint32)(currentCnt_u32 - s_previousCnt_u32);

            if(g_CyclicDuration_u32 > APPSYS_ELAPSED_TIME_CYCLIC)
            {
               Ret_e = APPSDM_ReportDiagEvnt(   APPSDM_DIAG_ITEM_APP_CYCLIC_TIMEOUT,
                                                APPSDM_DIAG_ITEM_REPORT_FAIL,
                                                Mu16ExtractByte1from32(g_CyclicDuration_u32),
                                                Mu16ExtractByte0from32(g_CyclicDuration_u32));
            }
            else
            {
                Ret_e = APPSDM_ReportDiagEvnt(  APPSDM_DIAG_ITEM_APP_CYCLIC_TIMEOUT,
                                                APPSDM_DIAG_ITEM_REPORT_PASS,
                                                (t_uint16)0,
                                                (t_uint16)0);
            }
            
        }
    }
    //---- fast task managment ----//
    if((mskfastTask_u16 != (t_uint16)0)
    && (isFastTaskON_b == (t_bool)False))
    {
        Ret_e = FMKTIM_Set_EvntLineState(   APPSYS_ITLINE_FASTTASK,
                                            FMKTIM_EVNT_OPE_START_TIMER);
        if(Ret_e == RC_OK)
        {
            isFastTaskON_b = False;
            Ret_e = SMB_Write(&g_sfbk_isFastTaskOn_s, &isFastTaskON_b);
            //---- ASSERTION already deal upon state machine function ----//
        }
    }
    
    return Ret_e;
}

/*********************************
 * s_APPSYS_FastTask
 *********************************/
static void s_APPSYS_FastTask(t_eFMKTIM_InterruptLineType f_InterruptType_e, t_uint8 f_InterruptLine_u8)
{
    t_eReturnCode Ret_e;
    t_uint16 idxModule_u16;
    t_uint32 startTime_u32;
    t_uint32 endTime_u32;
    t_bool isFastTaskON_b = False;
    t_uint16 mskfastTaskCall_u16;

    Ret_e = SMB_Read(&g_sfbk_mskfastTask_s, &mskfastTaskCall_u16);
    if(Ret_e != RC_OK)
    {
        ASSERT((t_uint16)Ret_e);
    }
    else 
    {
        if((f_InterruptType_e == FMKTIM_INTERRUPT_LINE_TYPE_EVNT)
        && (f_InterruptLine_u8 == APPSYS_ITLINE_FASTTASK))
        {
            // no fast task to call, shut down timer for now 
            if(mskfastTaskCall_u16 == (t_uint16)0)
            {
                Ret_e = FMKTIM_Set_EvntLineState( APPSYS_ITLINE_FASTTASK,
                                                FMKTIM_EVNT_OPE_STOP_TIMER);
                if(Ret_e != RC_OK)
                {
                    ASSERT((t_uint16)Ret_e);
                }
                else 
                {
                    Ret_e = SMB_Write(  &g_sfbk_isFastTaskOn_s,
                                        (void *)(&isFastTaskON_b));
                }
            }
            else 
            {
                FMKCPU_GetTick(&startTime_u32);
                for(idxModule_u16 = (t_uint16)0 ; idxModule_u16 < APPSYS_MODULE_NB ; idxModule_u16++)
                {
                    if(GETBIT(mskfastTaskCall_u16, idxModule_u16) == BIT_IS_SET_16B)
                    {
                        g_ModFastTask_apcb[idxModule_u16]();
                    }
                }
                FMKCPU_GetTick(&endTime_u32);

                g_fastTaskDuration_u32 = (endTime_u32 - startTime_u32);

                if(g_fastTaskDuration_u32 > APPSYS_ELASPED_TIME_FASTTASK)
                {
                    APPSDM_ReportDiagEvnt(  APPSDM_DIAG_ITEM_APP_FASTTASK_TIMEOUT,
                                            APPSDM_DIAG_ITEM_REPORT_FAIL,
                                            Mu16ExtractByte1from32(g_fastTaskDuration_u32),
                                            Mu16ExtractByte0from32(g_fastTaskDuration_u32));
                }
                else 
                {
                    APPSDM_ReportDiagEvnt(  APPSDM_DIAG_ITEM_APP_FASTTASK_TIMEOUT,
                                            APPSDM_DIAG_ITEM_REPORT_PASS,
                                            (t_uint16)0,
                                            (t_uint16)0);
                } 
            }
        }
    }

    return;
}
//************************************************************************************
// End of File
//************************************************************************************

/**
 *
 *	@brief
 *	@note   
 *
 *
 *	@param[in] 
 *	@param[out]
 *	 
 *
 *
 */
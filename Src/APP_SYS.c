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
#include "APP_CFG/ConfigApp/SoftwareVersion.h"
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
///@brief FSM for Cfg state
typedef enum 
{
    APPSYS_FSM_CFGSTATE_GET_ECU_POS = 0,                //---- get ecu position for the first time ----//
    APPSYS_FSM_CFGSTATE_WAIT_RCV_PRM,                   //---- get waiting receive param ---//
    APPSYS_FSM_CFGSTATE_GET_MACH,                       //---- get the machine Id ----//
    APPSYS_FSM_CFGSTATE_GET_MACH_SYS_OPT_DEFAULT,       //---- get the system option default
    APPSYS_FSM_CFGSTATE_GET_MACH_SYS_OPT_PRM,           //---- get the system option from EEPROM ----//
} t_eAPPSYS_FsmCfgSts;

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
///@brief State of each module available
static t_eCyclicModState g_ModuleState_ae[APPSYS_MODULE_NB];
///@brief Fast Task Function call 
static t_cbAPPSYS_FastTask * g_ModFastTask_apcb[APPSYS_MODULE_NB];
///@brief App Sys module state
static t_eCyclicModState g_AppSysModuleState_e = STATE_CYCLIC_CFG;
/// @brief Cyclic Duration
static t_uint32 g_CyclicDuration_u32 = (t_uint32)0;
/// @brief Cpu load
static t_float32 g_CpuLoad_f32 = (t_uint32)0;
/// @brief How many times the fast task take in ms
static t_uint32 g_fastTaskDuration_u32 = (t_uint32)0;

/// @brief SafeMem stuff
static t_sSafeMem_BlockInfo g_sfbk_isFastTaskOn_s;
static t_sSafeMem_BlockInfo g_sfbk_mskfastTask_s;

static t_bool g_isFastTaskON_b = (t_bool)False;
static t_uint16 g_mskFastTaskCall_u16 = (t_uint16)0; /**< to know the people to call */
static t_bool g_lockAssert_b = (t_bool)False;
static t_sAPPSYS_AssertInfo g_AssertInfo_s;


//-------------- Machine System varaible -----------------//
static t_uint8 g_MachSysOptValues_ua8[APPSYS_OPT_ID_NB];
static t_eAPPSYS_MachineList g_MachineID_e;
static t_eAPPSYS_FsmCfgSts g_FsmCfgSts_e = APPSYS_FSM_CFGSTATE_GET_ECU_POS;
static t_eAPPSYS_EcuPos g_ecuPos_e = APPSYS_ECU_POS_NB;
static t_bool g_isEcuPosValid_b = FALSE;
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
static t_eReturnCode s_APPSYS_ConfigurationState();
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
static t_eReturnCode s_APPSYS_UpdateEcuPos();
/**
*
*	@brief  Call driver cyclic function
*
*/
static t_eReturnCode s_APPSYS_ConvertAnaToEcuPos(t_float32 f_anaValue_f32, t_eAPPSYS_EcuPos * f_ecuPos_pe);
/**
*
*	@brief  Call driver cyclic function
*
*/
static void s_APPSYS_SigAnaCallback(t_eFMKIO_SigType f_sigType_e,
                                    t_uint8 f_sigId_u8,
                                    t_uint16 f_debugInfo1_u16, 
                                    t_uint16 f_debugInfo2_u16);

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
    t_sFMKSRL_DrvSerialCfg SrlCfg_s;
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
    if((Ret_e == RC_OK)
    && (APPSYS_WATCHDOG_ENABLE == TRUE))
    {
        Ret_e = FMKCPU_Set_WwdgCfg((t_eFMKCPu_WwdgResetPeriod)FMKCPU_WWDG_RESET_CFG);
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
    //---- set diag log serial line ----//
    if(FMKSRL_DEBUG_UART_ENABLE == M_TRUE
    && (Ret_e == RC_OK))
    {
        SrlCfg_s.runMode_e = FMKSRL_LINE_RUNMODE_DMA;
        SrlCfg_s.hwProtType_e = FMKSRL_HW_PROTOCOL_UART;
        SrlCfg_s.hwCfg_s.Baudrate_e = FMKSRL_LINE_BAUDRATE_115200,
        SrlCfg_s.hwCfg_s.Mode_e = FMKSRL_LINE_MODE_RX_TX;
        SrlCfg_s.hwCfg_s.Parity_e = FMKSRL_LINE_PARITY_NONE,
        SrlCfg_s.hwCfg_s.Stopbit_e = FMKSRL_LINE_STOPBIT_1,
        SrlCfg_s.hwCfg_s.wordLenght_e = FMKSRL_LINE_WORDLEN_8BITS,
        SrlCfg_s.CfgSpec_u.uartCfg_s.hwFlowCtrl_e = FMKSRL_UART_HW_FLOW_CTRL_NONE;
        SrlCfg_s.CfgSpec_u.uartCfg_s.Type_e = FMKSRL_UART_TYPECFG_UART;

        Ret_e = FMKSRL_InitDrv( FMKSRL_DEBUG_SERIAL_LINE,
                                SrlCfg_s,
                                (t_cbFMKSRL_RcvMsgEvent *)NULL_FUNCTION,
                                (t_cbFMKSRL_TransmitMsgEvent *)NULL_FUNCTION);  
    }
    if(Ret_e == RC_OK)
    {
        Ret_e = FMKIO_Set_InAnaSigCfg(APPSYS_IO_ANALOG_SIGNAL, s_APPSYS_SigAnaCallback);
    }
    if(Ret_e == RC_OK)
    {
        FMKSRL_LOG("STM32 startup, version %d\r\n", SOFTWARE_VERSION);
    }
    //---- set fast tasl timer ope ----//
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
        ASSERT((t_uint16)Ret_e);
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
    t_uint32 currentCnt_u32 = 0;
    static t_uint32 s_previousCnt_u32 = 0;
    t_uint32 elapsedTime_u32 =  0;

    FMKCPU_GetTick(&currentCnt_u32);

    elapsedTime_u32 = (t_uint32)(currentCnt_u32 - s_previousCnt_u32);
    if((elapsedTime_u32) > APPSYS_ELAPSED_TIME_CYCLIC)
    {
        // reset whatchdog for fmk/app cycle
        s_previousCnt_u32 = currentCnt_u32;

        switch(g_AppSysModuleState_e)
        {
            case STATE_CYCLIC_CFG:
            {
                Ret_e = s_APPSYS_ConfigurationState();

                if(Ret_e == RC_OK)
                {
                    g_AppSysModuleState_e = STATE_CYCLIC_OPE;
                }
                else if(Ret_e < RC_OK)
                {
                    g_AppSysModuleState_e = STATE_CYCLIC_ERROR;
                }
            }
            break;
            case STATE_CYCLIC_OPE:
            {
                Ret_e = s_APPSYS_Operational();
                break;
            }
            case STATE_CYCLIC_BUSY:
            {
                break;
            }
            case STATE_CYCLIC_PREOPE:
            case STATE_CYCLIC_ERROR:
            default:
            {
                // Nothing to do infinite loop
                break;
            }
        }

        FMKCPU_GetTick(&currentCnt_u32);
        g_CyclicDuration_u32 = (t_uint32)(currentCnt_u32 - s_previousCnt_u32);
        g_CpuLoad_f32 = (t_float32)(g_CyclicDuration_u32 / APPSYS_ELAPSED_TIME_CYCLIC);

        if(g_CyclicDuration_u32 > APPSYS_ELAPSED_TIME_CYCLIC)
        {
            APPSDM_ReportDiagEvnt(   APPSDM_DIAG_ITEM_APP_CYCLIC_TIMEOUT,
                                    APPSDM_DIAG_ITEM_REPORT_FAIL,
                                    Mu16ExtractByte1from32(g_CyclicDuration_u32),
                                    Mu16ExtractByte0from32(g_CyclicDuration_u32));
        }
        else
        {
            APPSDM_ReportDiagEvnt(  APPSDM_DIAG_ITEM_APP_CYCLIC_TIMEOUT,
                                    APPSDM_DIAG_ITEM_REPORT_PASS,
                                    (t_uint16)0,
                                    (t_uint16)0);
        }
        //---- send signal g_cyclic_duration ----//
        Ret_e = APPSIG_SetSignalValue(APPSIG_SIGNAL_CYCLIC_DURATION, (t_float32)g_CyclicDuration_u32);

        if(Ret_e == RC_OK)
        {
            Ret_e = APPSIG_SetSignalValue(APPSIG_SIGNAL_FASTTASKDURATION, (t_float32)g_fastTaskDuration_u32);
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
        g_lockAssert_b = (t_bool)True;
        g_AssertInfo_s.debugInfo_u16 = f_Info_u16;
        strncpy(g_AssertInfo_s.file_ac, f_file_str, APPSYS_FILE_NAME_LEN - 1);
        g_AssertInfo_s.file_ac[APPSYS_FILE_NAME_LEN - 1] = '\0';  // Assurer la terminaison
        g_AssertInfo_s.line_u32 = f_line_u32;

        FMKSRL_LOG("[%d] Assertion in file %s line : %d, info : %d\r\n", 
                    f_captureTime_u32,
                    g_AssertInfo_s.file_ac, 
                    f_line_u32,
                    f_Info_u16);
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

    Ret_e = SMB_Read(&g_sfbk_mskfastTask_s, &mskfastTaskCall_u16, sizeof(t_uint16));


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
            Ret_e = SMB_Write(&g_sfbk_mskfastTask_s, &mskfastTaskCall_u16, sizeof(t_uint16));
        }
    }

    return Ret_e;
}

/*********************************
 * APPSYS_GetSysOption
 *********************************/
t_eReturnCode APPSYS_GetSysOption(t_eAPPSYS_SysOptionList f_OptionID_e, t_uint8 * f_OptVal_pu8)
{
    t_eReturnCode Ret_e;

    if(f_OptionID_e >= APPSYS_OPT_ID_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
        ASSERT((t_uint16)0);
    }
    else if(f_OptVal_pu8 == (t_uint8 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
        ASSERT((t_uint16)0);
    }
    else if((g_AppSysModuleState_e != STATE_CYCLIC_PREOPE)
    &&     (g_AppSysModuleState_e != STATE_CYCLIC_OPE)) 
    {
        Ret_e = RC_WARNING_BUSY;
    }
    else
    {
        Ret_e = RC_OK;
        *f_OptVal_pu8 = g_MachSysOptValues_ua8[f_OptionID_e];
    }

    return Ret_e;
}

/*********************************
 * APPSYS_GetEcuPosition
 *********************************/
t_eReturnCode APPSYS_GetEcuPosition(t_eAPPSYS_EcuPos * f_ecuPos_pe)
{
    t_eReturnCode Ret_e;

    if(f_ecuPos_pe == NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
        ASSERT((t_uint16)0);
    }
    else if(g_isEcuPosValid_b == FALSE)
    {
        Ret_e = RC_WARNING_BUSY;
    }
    else 
    {
        if(g_ecuPos_e != APPSYS_ECU_POS_NB)
        {
            Ret_e = RC_OK;
            *f_ecuPos_pe = g_ecuPos_e;
        }
        else 
        {
            *f_ecuPos_pe = APPSYS_ECU_POS_NB;
            Ret_e = RC_WARNING_BUSY;
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
            Ret_e = c_AppSys_ModuleFunc_apf[modIndex_u8].GetState_pcb(&g_ModuleState_ae[modIndex_u8]);

            if((Ret_e == RC_OK)
            && (c_AppSys_ModuleFunc_apf[modIndex_u8].signal_e < APPSIG_SIGNAL_NB))
            {
                Ret_e = APPSIG_SetSignalValue(  c_AppSys_ModuleFunc_apf[modIndex_u8].signal_e,
                                                (t_float32)g_ModuleState_ae[modIndex_u8]);
            }
        }
    }

    //---- reset lock assert ----//
    g_lockAssert_b = (t_bool)False;
    if(APPSYS_WATCHDOG_ENABLE == (t_bool)TRUE)
    {
        (void)FMKCPU_RearmWwdg();
    }

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
 * s_APPSYS_ConfigurationState
 *********************************/
static t_eReturnCode s_APPSYS_ConfigurationState()
{
    t_eReturnCode Ret_e;
    t_uint32 lastTime_u32 = (t_uint32)0;
    t_uint32 currentTime_u32;
    t_uint8 idxSysOpt_u8;
    t_uAPPSPM_PrmValType sysOptValue_u = {.prmVal_u16 = 0};

    s_APPSYS_Set_ModulesCyclic();
    FMKCPU_GetTick(&currentTime_u32);

    switch(g_FsmCfgSts_e)
    {
        case APPSYS_FSM_CFGSTATE_GET_ECU_POS:
            Ret_e = s_APPSYS_UpdateEcuPos();
            if(Ret_e == RC_OK)
            {
                Ret_e = RC_WARNING_PENDING;
                g_FsmCfgSts_e = APPSYS_FSM_CFGSTATE_WAIT_RCV_PRM;
            }
            else if((Ret_e != RC_WARNING_BUSY)
            &&      (Ret_e != RC_WARNING_PENDING))
            {
                g_AppSysModuleState_e = STATE_CYCLIC_ERROR;
            }
            else if(Ret_e == RC_WARNING_NO_OPERATION)
            {
                Ret_e = RC_WARNING_INIT_PROBLEM;
            }
            // else propagate retcode 
        break;
        case APPSYS_FSM_CFGSTATE_WAIT_RCV_PRM:
            if((currentTime_u32 - lastTime_u32) > APPSYS_WAIT_PRM_TIMEOUT)
            {
                g_FsmCfgSts_e = APPSYS_FSM_CFGSTATE_GET_MACH;
            }
            Ret_e = RC_WARNING_PENDING;
        break;
        case APPSYS_FSM_CFGSTATE_GET_MACH:
            if(APPSYS_SYS_OPT_EEPROM_PARAM_ENABLE == TRUE)
            {
                Ret_e = APPSPM_GetParam(APPSPM_PRM_SYS_MACHINE_ID, &sysOptValue_u);
                if(Ret_e == RC_OK)
                {
                    Ret_e = RC_WARNING_PENDING;
                    g_MachineID_e = (t_eAPPSYS_MachineList)sysOptValue_u.prmVal_u16;
                    g_FsmCfgSts_e = APPSYS_FSM_CFGSTATE_GET_MACH_SYS_OPT_DEFAULT;
                }
                else if (Ret_e >= RC_OK) 
                {
                    Ret_e = RC_WARNING_PENDING;
                }
            }
            else
            {
                //---- machine id based on ecu position ----//
                Ret_e = RC_WARNING_PENDING;
                g_MachineID_e = (t_eAPPSYS_MachineList)((t_uint8)g_ecuPos_e);
                g_FsmCfgSts_e = APPSYS_FSM_CFGSTATE_GET_MACH_SYS_OPT_DEFAULT;
            }
        break;
        case APPSYS_FSM_CFGSTATE_GET_MACH_SYS_OPT_DEFAULT:
            for(idxSysOpt_u8 = (t_uint8)0 ; idxSysOpt_u8 < (t_uint8)APPSYS_OPT_ID_NB ; idxSysOpt_u8++)
            {
                g_MachSysOptValues_ua8[idxSysOpt_u8] = c_AppSys_MachOptCfg_ua8[g_MachineID_e][idxSysOpt_u8];
            }
            if(APPSYS_SYS_OPT_EEPROM_PARAM_ENABLE == TRUE)
            {
                Ret_e = RC_WARNING_PENDING;
                g_FsmCfgSts_e = APPSYS_FSM_CFGSTATE_GET_MACH_SYS_OPT_PRM;
            }
            else 
            {   
                Ret_e = RC_OK;
                g_FsmCfgSts_e = APPSYS_FSM_CFGSTATE_GET_MACH;
            }
        break;
        case APPSYS_FSM_CFGSTATE_GET_MACH_SYS_OPT_PRM:
            Ret_e = RC_OK;
            for(idxSysOpt_u8 = (t_uint8)0 ; 
            (idxSysOpt_u8 < (t_uint8)APPSYS_OPT_ID_NB) && (Ret_e == RC_OK) ; 
            idxSysOpt_u8++)
            {
                Ret_e = APPSPM_GetParam(c_AppSys_SysOpt_ItemPrmID_ae[idxSysOpt_u8],
                                        &sysOptValue_u);
                if(Ret_e == RC_OK)
                {
                    g_MachSysOptValues_ua8[idxSysOpt_u8] = (t_uint8)sysOptValue_u.prmVal_u16;
                }
            }
            if(Ret_e == RC_OK)
            {
                g_FsmCfgSts_e = APPSYS_FSM_CFGSTATE_GET_MACH;
            }
            else if(Ret_e > RC_OK)
            {
                Ret_e = RC_WARNING_PENDING;
            }
            //--- else propagete error ----//
        break;
        default:
            Ret_e = RC_ERROR_NOT_ALLOWED;
        break;
    }

    return Ret_e;  
}

/*********************************
 * s_APPSYS_Operational
 *********************************/
static t_eReturnCode s_APPSYS_Operational(void)
{
    t_eReturnCode Ret_e = RC_OK;    
    t_bool isFastTaskON_b = False;
    t_uint16 mskfastTask_u16 = (t_uint16)0;

    Ret_e = SMB_Read(&g_sfbk_mskfastTask_s, &mskfastTask_u16, sizeof(t_uint16));
    if(Ret_e ==  RC_OK)
    {
        Ret_e = SMB_Read(&g_sfbk_isFastTaskOn_s, &isFastTaskON_b, sizeof(t_bool));
    }
    if(Ret_e == RC_OK)
    {
        Ret_e = s_APPSYS_UpdateEcuPos();
    }
    if(Ret_e >= RC_OK)
    {       
        s_APPSYS_Set_ModulesCyclic();
    }
    //---- fast task managment ----//
    if((mskfastTask_u16 != (t_uint16)0)
    && (isFastTaskON_b == (t_bool)FALSE))
    {
        Ret_e = FMKTIM_Set_EvntLineState(   APPSYS_ITLINE_FASTTASK,
                                            FMKTIM_EVNT_OPE_START_TIMER);
        if(Ret_e == RC_OK)
        {
            isFastTaskON_b = TRUE;
            Ret_e = SMB_Write(&g_sfbk_isFastTaskOn_s, &isFastTaskON_b, sizeof(t_bool));
            //---- ASSERTION already deal upon state machine function ----//
        }
    }
    
    return Ret_e;
}

/*********************************
 * s_APPSYS_UpdateEcuPos
 *********************************/
static t_eReturnCode s_APPSYS_UpdateEcuPos(void)
{
    t_eReturnCode Ret_e = RC_OK; 
    t_float32 anaValue_f32;
    t_eAPPSYS_EcuPos ecuPosition_e = APPSYS_ECU_POS_NB;

    Ret_e = FMKIO_Get_InAnaSigValue(APPSYS_IO_ANALOG_SIGNAL, &anaValue_f32);

    if(Ret_e == RC_OK)
    {
        Ret_e = s_APPSYS_ConvertAnaToEcuPos(anaValue_f32, &ecuPosition_e);

        if(Ret_e == RC_OK)
        {
            //--- first time ecu is valid ----//
            if(g_isEcuPosValid_b == FALSE)
            {
                g_isEcuPosValid_b = TRUE;
                g_ecuPos_e = ecuPosition_e;
            }
            else 
            {
                //---- if a changement of position occured ----//
                if(ecuPosition_e != g_ecuPos_e)
                {
                    APPSDM_ReportDiagEvnt(  APPSDM_DIAG_ITEM_APPSYS_ECU_POS_ERROR,
                                            APPSDM_DIAG_ITEM_REPORT_FAIL,
                                            anaValue_f32,
                                            (t_uint16)0);
                }
            }
        }
        else 
        {
            g_ecuPos_e = APPSYS_ECU_POS_NB;
            g_isEcuPosValid_b = FALSE;
            APPSDM_ReportDiagEvnt(  APPSDM_DIAG_ITEM_APPSYS_ECU_POS_ERROR,
                                    APPSDM_DIAG_ITEM_REPORT_FAIL,
                                    anaValue_f32,
                                    (t_uint16)0);
        }
    }
    else if(Ret_e != RC_WARNING_BUSY)
    {
        g_isEcuPosValid_b = FALSE;
        APPSDM_ReportDiagEvnt(  APPSDM_DIAG_ITEM_APPSYS_ECU_POS_ERROR,
                                APPSDM_DIAG_ITEM_REPORT_FAIL,
                                Ret_e,
                                (t_uint16)0);
    }

    return Ret_e;
}

/*********************************
 * s_APPSYS_ConvertAnaToEcuPos
 *********************************/
static t_eReturnCode s_APPSYS_ConvertAnaToEcuPos(t_float32 f_anaValue_f32, t_eAPPSYS_EcuPos * f_ecuPos_pe)
{
    t_eReturnCode Ret_e = RC_WARNING_NO_OPERATION;
    t_uint8 idxAnaRange_u8;

    for(idxAnaRange_u8 = (t_uint8)0 ; idxAnaRange_u8 < (t_uint8)APPSYS_ECU_POS_MAX ; idxAnaRange_u8++)
    {
        if((f_anaValue_f32 >= c_EcuPosAnaRange_as[idxAnaRange_u8].min_f32)
        && (f_anaValue_f32 <= c_EcuPosAnaRange_as[idxAnaRange_u8].max_f32))
        {
            if(idxAnaRange_u8 >= APPSYS_ECU_POS_NB)
            {
                Ret_e = RC_ERROR_WRONG_RESULT;
                ASSERT((t_uint16)idxAnaRange_u8);
                break;
            }
            else 
            {
                *f_ecuPos_pe = (t_eAPPSYS_EcuPos)idxAnaRange_u8;
                Ret_e = RC_OK;
                break;
            }
        }
    }
    if(Ret_e == RC_WARNING_NO_OPERATION)
    {
        ASSERT((t_uint16)0);
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

    Ret_e = SMB_Read(&g_sfbk_mskfastTask_s, &mskfastTaskCall_u16, sizeof(t_uint16));
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
                                        (void *)(&isFastTaskON_b),
                                        sizeof(t_bool));
                }
            }
            else 
            {
                FMKCPU_GetTick(&startTime_u32);
                for(idxModule_u16 = (t_uint16)0 ; idxModule_u16 < APPSYS_MODULE_NB ; idxModule_u16++)
                {
                    if((GETBIT(mskfastTaskCall_u16, idxModule_u16) == BIT_IS_SET_16B)
                    && g_ModFastTask_apcb[idxModule_u16] != (t_cbAPPSYS_FastTask *)NULL_FUNCTION)
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
            }
        }
    }

    return;
}

/*********************************
 * s_APPSYS_SigAnaCallback
 *********************************/
static void s_APPSYS_SigAnaCallback(t_eFMKIO_SigType f_sigType_e,
                                    t_uint8 f_sigId_u8,
                                    t_uint16 f_debugInfo1_u16, 
                                    t_uint16 f_debugInfo2_u16)
{
    if((f_sigType_e == FMKIO_SIGTYPE_INPUT_ANA)
    && (f_sigId_u8 == (t_uint8)APPSYS_IO_ANALOG_SIGNAL))
    {
        APPSDM_ReportDiagEvnt(  APPSDM_DIAG_ITEM_APPSYS_ECU_POS_ERROR,
                                APPSDM_DIAG_ITEM_REPORT_FAIL,
                                f_debugInfo1_u16,
                                f_debugInfo2_u16);
    }
    else 
    {
        ASSERT((t_uint16)f_sigType_e);
    }
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
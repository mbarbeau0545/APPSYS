/*********************************************************************
 * @file        CAN.h
 * @brief       Template_BriefDescription.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      xxxxxx
 * @date        jj/mm/yyyy
 * @version     1.0
 */
  
#ifndef APPSYS_CONFIGPRIVATE_H_INCLUDED
#define APPSYS_CONFIGPRIVATE_H_INCLUDED





    // ********************************************************************
    // *                      Includes
    // ********************************************************************
    #include "./APPSYS_ConfigPublic.h"
    #include "1_FMK/FMK_HAL/FMK_IO/Src/FMK_IO.h"
    #include "1_FMK/FMK_HAL/FMK_HRT/Src/FMK_HRT.h"
    #include "1_FMK/FMK_HAL/FMK_CDA/Src/FMK_CDA.h"
    #include "1_FMK/FMK_HAL/FMK_CPU/Src/FMK_CPU.h"
    #include "1_FMK/FMK_HAL/FMK_TIM/Src/FMK_TIM.h"
    #include "1_FMK/FMK_HAL/FMK_CAN/Src/FMK_FDCAN.h"
    #include "1_FMK/FMK_HAL/FMK_SRL/Src/FMK_SRL.h"
    #include "3_APP/APP_CTRL/APP_ACT/Src/APP_ACT.h"
    #include "3_APP/APP_CTRL/APP_SDM/Src/APP_SDM.h"
    #include "3_APP/APP_CTRL/APP_SPM/Src/APP_SPM.h"
    #include "3_APP/APP_CTRL/APP_SNS/Src/APP_SNS.h"
    #include "3_APP/APP_CTRL/APP_UDS/Src/APP_UDS.h"
    #include "3_APP/APP_CTRL/APP_SIG/Src/APP_SIG.h"
    #include "3_APP/APP_LGC/Src/APP_LGC.h"
    // ********************************************************************
    // *                      Defines
    // ********************************************************************

    #define APPSYS_SYSTEM_CORE_SPEED FMKCPU_CORE_CLOCK_SPEED_128MHZ

    #define APPSYS_ELAPSED_TIME_CYCLIC          ((t_uint8)40)                   /**< Elapsed time (in ms) between cyclic function call*/
    #define APPSYS_ITLINE_FASTTASK              FMKTIM_INTERRUPT_LINE_EVNT_1    /**< Timer Line use for FastTask */
    #define APPSYS_ELASPED_TIME_FASTTASK        ((t_uint32)5)                   /**< Fast Task every 5 ms */
    #define APPSYS_WATCHDOG_ENABLE              (FALSE)
    // ********************************************************************
    // *                      Types
    // ********************************************************************
	/* CAUTION : Automatic generated code section for Enum: Start */

	/* CAUTION : Automatic generated code section for Enum: End */
	//-----------------------------ENUM TYPES-----------------------------//


	/* CAUTION : Automatic generated code section for Structure: Start */

	/* CAUTION : Automatic generated code section for Structure: End */
	//-----------------------------STRUCT TYPES---------------------------//
	/* CAUTION : Automatic generated code section : Start */

	/* CAUTION : Automatic generated code section : End */
	//-----------------------------TYPEDEF TYPES---------------------------//
    /**
    *
    *	@brief      Set a module function init 

    */
    typedef t_eReturnCode (t_cbAppSys_FuncInit)(void);
    /**
    *
    *	@brief      Set a module cyclic function

    */
    typedef t_eReturnCode (t_cbAppSys_FuncCyclic)(void);
    /**
    *
    *	@brief Function to know the module state.\n 
    *
    *	@param[in]  f_State_pe : store the value, value from @ref t_eCyclicModState
    *
    *   @retval RC_OK                             @ref RC_OK
    *   @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NUL
    */
    typedef t_eReturnCode (t_cbAppSys_GetState)(t_eCyclicModState *f_state_pe);
    /**
    *
    *	@brief Function to update the module state.\n
    *
    *	@param[in]  f_State_e : the new value, value from @ref t_eCyclicModState
    *
    *   @retval RC_OK                             @ref RC_OK
    */
    typedef t_eReturnCode (t_cbAppSys_SetState)(t_eCyclicModState f_state_pe);

    /* Structure to store Init and Cyclic Func*/
    typedef struct 
    {
        t_cbAppSys_FuncInit   * Init_pcb;        
        t_cbAppSys_FuncCyclic * Cyclic_pcb;
        t_cbAppSys_GetState   * GetState_pcb;
        t_cbAppSys_SetState   * SetState_pcb;
        t_eAPPSIG_Signal        signal_e;
    } t_sAppSys_SysFunc;
    // ********************************************************************
    // *                      Prototypes
    // ********************************************************************
        
    // ********************************************************************
    // *                      Variables
    // ********************************************************************
    
    // Flag automatic generate code
    /**< variable to store modules functions */
    t_sAppSys_SysFunc c_AppSys_ModuleFunc_apf[APPSYS_MODULE_NB] = {
        //----- FrameWork module -----//
        {FMKCPU_Init,    FMKCPU_Cyclic,     FMKCPU_GetState,   FMKCPU_SetState,     APPSIG_SIGNAL_FMKCPU_MODSTATE},
        {FMKTIM_Init,    FMKTIM_Cyclic,     FMKTIM_GetState,   FMKTIM_SetState,     APPSIG_SIGNAL_FMKTIM_MODSTATE},
        {FMKHRT_Init,    FMKHRT_Cyclic,     FMKHRT_GetState,   FMKHRT_SetState,     APPSIG_SIGNAL_FMKHRT_MODSTATE},
        {FMKCDA_Init,    FMKCDA_Cyclic,     FMKCDA_GetState,   FMKCDA_SetState,     APPSIG_SIGNAL_FMKCDA_MODSTATE},
        {FMKIO_Init,     FMKIO_Cyclic,      FMKIO_GetState,    FMKIO_SetState,      APPSIG_SIGNAL_FMKIO_MODSTATE},
#ifdef APPSYS_MODULE_FMKCAN_ENABLE
        {FMKFDCAN_Init,  FMKFDCAN_Cyclic,   FMKFDCAN_GetState, FMKFDCAN_SetState,   APPSIG_SIGNAL_FMKFDCAN_MODSTATE},
#endif // APPSYS_MODULE_FMKCAN_ENABLE
#ifdef APPSYS_MODULE_FMKSRL_ENABLE
        {FMKSRL_Init,   FMKSRL_Cyclic,      FMKSRL_GetState,   FMKSRL_SetState,     APPSIG_SIGNAL_FMKSRL_MODSTATE},
#endif // APPSYS_MODULE_FMKSRL_ENABLE

        //----- Application module -----//
        {APPSDM_Init,    APPSDM_Cyclic,     APPSDM_GetState,   APPSDM_SetState,      APPSIG_SIGNAL_APPSDM_MODSTATE},
        {APPSPM_Init,    APPSPM_Cyclic,     APPSPM_GetState,   APPSPM_SetState,      APPSIG_SIGNAL_APPSPM_MODSTATE},
        {APPSIG_Init,    APPSIG_Cyclic,     APPSIG_GetState,   APPSIG_SetState,      APPSIG_SIGNAL_APPSIG_MODSTATE},
        {APPSNS_Init,    APPSNS_Cyclic,     APPSNS_GetState,   APPSNS_SetState,      APPSIG_SIGNAL_APPSNS_MODSTATE},
        {APPACT_Init,    APPACT_Cyclic,     APPACT_GetState,   APPACT_SetState,      APPSIG_SIGNAL_APPACT_MODSTATE},
        {APPLGC_Init,    APPLGC_Cyclic,     APPLGC_GetState,   APPLGC_SetState,      APPSIG_SIGNAL_APPLGC_MODSTATE},
    };

    //********************************************************************************
    //                      Public functions - Prototyupes
    //********************************************************************************

#endif // APPSYS_CONFIGPRIVATE_H_INCLUDED           
//************************************************************************************
// End of File
//************************************************************************************

/**
 *
 *	@brief
 *	@note   
 *
 *
 *	@params[in] 
 *	@params[out]
 *	 
 *
 *
 */

/**
 * @file        APP_SYS.h
 * @brief       Template_BriefDescription.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      xxxxxx
 * @date        jj/mm/yyyy
 * @version     1.0
 */
  
#ifndef APP_SYS_H_INCLUDED
#define APP_SYS_H_INCLUDED





    // ********************************************************************
    // *                      Includes
    // ********************************************************************
    #include "FMK_HAL/FMK_CPU/Src/FMK_CPU.h"
    #include "./APP_CFG/ConfigFiles/APPSYS_ConfigPublic.h"
    #include "FMK_HAL/FMK_SRL/Src/FMK_SRL.h"

    #include <string.h>
    // ********************************************************************
    // *                      Defines
    // ********************************************************************
    
    #define ASSERT(info) do { \
        t_uint32 _tick; \
        FMKCPU_GetTick(&_tick); \
        APPSYS_AssertionTrap(info, SHORTEN_PATH(__FILE__), __LINE__, _tick); \
    } while(0)

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
    typedef enum 
    {
        APPSYS_FAST_TASK_DISABLE = 0x00,
        APPSYS_FAST_TASK_ENABLE,
    } t_eAPPSYS_FastTaskState;
    /*
    *
    *	@brief  Perform Application system init
    *	@note   
    */
    typedef void (t_cbAPPSYS_FastTask)(void);
    // ********************************************************************
    // *                      Prototypes
    // ********************************************************************
        
    // ********************************************************************
    // *                      Variables
    // ********************************************************************

    //********************************************************************************
    //                      Public functions - Prototyupes
    //********************************************************************************
    /*
    *
    *	@brief  Perform Application system init
    *	@note   
    */
    void APPSYS_Init(void);
    /*
    *
    *	@brief  Perform Application system cyclic
    *	@note   
    *
    */
    void APPSYS_Cyclic(void);

    /**
    *
    *	@brief  Call driver cyclic function
    *
    */
    void APPSYS_AssertionTrap(  t_uint16 f_Info_u16, 
                                const char * f_file_str, 
                                t_uint32 f_line_u32,
                                t_uint32 f_captureTime_u32);
    /**
    *
    *	@brief  Add a Fast Task to be call every APPSYS_ELASPED_TIME_FASTTASK
    *   @note   Once you register it, the fast task will be considered enable 
    *           in the PreOPerationnal state of your module
    *
    */
    t_eReturnCode APPSYS_AddFastTask(t_eAppSys_ModuleList f_ModuleId_e, t_cbAPPSYS_FastTask * f_moduleFastTask_pcb);

    /**
    *
    *	@brief  Add a Fast Task to be call every APPSYS_ELASPED_TIME_FASTTASK
    *
    */
   t_eReturnCode APPSYS_SetFastTaskState(t_eAppSys_ModuleList f_ModuleId_e,  t_eAPPSYS_FastTaskState f_state_e);
    

#endif // APP_SYS_H_INCLUDED           
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

/*********************************************************************
 * @file        APPSYS_ConfigPublic.h
 * @brief       Template_BriefDescription.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      xxxxxx
 * @date        jj/mm/yyyy
 * @version     1.0
 */
  
#ifndef APPSYS_CONFIGPUBLIC_H_INCLUDED
#define APPSYS_CONFIGPUBLIC_H_INCLUDED





    // ********************************************************************
    // *                      Includes
    // ********************************************************************
    #include "TypeCommon.h"
    #include "string.h"
    // ********************************************************************
    // *                      Defines
    // ********************************************************************
    #define BASE_PATH "src\\"
    #define SHORTEN_PATH(path) (strstr(path, BASE_PATH) ? strstr(path, BASE_PATH) : path)
    #define APPSYS_MODULE_FMKCAN_ENABLE
    #define APPSYS_MODULE_FMKSRL_ENABLE
    #define APPSYS_MODULE_APPLGC_ENABLE

    #define APPSYS_ELAPSED_TIME_CYCLIC ((t_uint8)40)     /**< Elapsed time (in ms) between cyclic function call*/
    #define APPSYS_ITLINE_FASTTASK FMKTIM_INTERRUPT_LINE_EVNT_1 /**< Timer Line use for FastTask */
    #define APPSYS_ELASPED_TIME_FASTTASK ((t_uint32)5)  /**< Fast Task every 5 ms */
    
    // ********************************************************************
    // *                      Types
    // ********************************************************************
	/* CAUTION : Automatic generated code section for Enum: Start */

	/* CAUTION : Automatic generated code section for Enum: End */
	//-----------------------------ENUM TYPES-----------------------------//
    // Flag automatic generate code
    /**< enum for module list */
    typedef enum
    {
        APPSYS_MODULE_FMK_CPU = 0,
        APPSYS_MODULE_FMK_TIM,
        APPSYS_MODULE_FMK_HRT,
        APPSYS_MODULE_FMK_CDA,
        APPSYS_MODULE_FMK_IO,
#ifdef APPSYS_MODULE_FMKCAN_ENABLE
        APPSYS_MODULE_FMK_CAN,
#endif // APPSYS_MODULE_FMKCAN_ENABLE

#ifdef APPSYS_MODULE_FMKSRL_ENABLE
        APPSYS_MODULE_FMK_SERIAL,
#endif // APPSYS_MODULE_FMKSRL_ENABLE
        APPSYS_MODULE_APP_SDM,
        APPSYS_MODULE_APP_SPM,
        APPSYS_MODULE_APP_SNS,
        APPSYS_MODULE_APP_ACT,
        APPSYS_MODULE_APP_LGC,
        APPSYS_MODULE_APP_UDS,

        APPSYS_MODULE_NB
    } t_eAppSys_ModuleList;

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

    //********************************************************************************
    //                      Public functions - Prototyupes
    //********************************************************************************

#endif // APPSYS_CONFIGPUBLIC_H_INCLUDED           
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

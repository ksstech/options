// options.h

#pragma once

#include "struct_union.h"

// ############################ building undefined values/messages defaults ########################

#include "../private/options_enum.h"				// default enumerated 1/2/3/4/8 bit option numbers

#if __has_include("app_options.h")					// if application options header exists, include it	
	#include "app_options.h"						// to define application specific option values & names
#else
	#warning "Application options not used !!!"
#endif

#include "../private/options_val.h"					// set defaults for undefined option values
#include "../private/options_mes.h"					// set defaults for undefined option names

#ifdef __cplusplus
extern "C" {
#endif

// ###################################### Special Macros ###########################################

#if __has_include("app_options.h")
	#define appOPTIONS		1
	#define OPT_GET(opt)	xOptionGet(opt)
#else
	#define appOPTIONS		0
	#define OPT_GET(opt)	0		
#endif

// ############################################ Macros #############################################

// ####################################### public variables ########################################

#if (appOPTIONS > 0)

extern int (* pfAppHandler)(int, int);

#endif

// ####################################### public functions ########################################

#if (appOPTIONS > 0)

/**
 * @brief	Set new value for 1~8 bit options
 * @param	ON - option number
 * @param	OV - option value
 */
void vOptionSet(int ON, int OV);

/**
 * @brief	Set new value for 1~8 bit and custom option values
 * @param	ON - option number
 * @param	OV - option value
 * @return	AppHandler result, 1/changed, 0/same or error code
 */
int xOptionSet(int ON, int OV);

/**
 * @brief   Set the value of any option, [optionally) persist the new value
 * @param   ON - Option number
 * @param   OV - Option value
 * @param   PF - Persist flag (1 to persist)
 * @return
 */
int	xOptionSetPersist(int ON, int OV, int PF);

/**
 * @brief   return current value of the specified option
 * @param   ON - Option number
 * @return  current option value
 */
int xOptionGet(int ON);

/**
 * @brief   return default value of the specified option
 * @param   ON - Option number
 * @return  current option value
 */
int xOptionGetDefault(int ON);

/**
 * @brief   report current option values
 * @param   psR - report control structure
 */
struct report_t;
void vOptionsShow(struct report_t * psR);

#endif

#ifdef __cplusplus
}
#endif

/**************************************************************************************************

// Concatenate 'val' with the option
#define _VAL_NAME(option) val##option

// Check if symbol is not defined, and define it as 0 if necessary
#define _CHK_DEF_0(symbol) \
	#ifndef symbol		\
		#define symbol 0  \
	#endif

// Concatenate 'mes' with the option
#define _MES_NAME(option) mes##option

// Check if symbol is not defined, and define it as "\0" if necessary
#define _CHK_DEF_Z(symbol)	\
	#ifndef symbol		 	\
		#define symbol "\0"	\
	#endif

// Check and define the two symbols
#define CHECK_VAL_MES(option)		\
	_CHK_DEF_0(_VAL_NAME(option))	\
	_CHK_DEF_Z(_MES_NAME(option))

// Call the macro for 'test'
// CHECK_VAL_MES(test)

**************************************************************************************************/

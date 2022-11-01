
/**
 *  \file
 *
 *  \par Purpose
 *
 *  Standard output methods for reporting debug information, warnings and
 *  errors, which can be easily be turned on/off.
 *
 *  \par Usage
 *  -# Uses the trace_debug(), trace_info(), trace_warning(), trace_error()
 *     trace_fatal() macros to output traces throughout the program.
 *  -# Each type of trace has a level : Debug 5, Info 4, Warning 3, Error 2
 *     and Fatal 1. Disable a group of traces by changing the value of
 *     TRACE_LEVEL during compilation; traces with a level bigger than TRACE_LEVEL
 *     are not generated. To generate no trace, use the reserved value 0.
 *  -# Trace disabling can be dynamic. The trace level can be modified in
 *  runtime but messages with a level higher that TRACE_LEVEL are compiled-out
 *  an will not be displayed regardless of the value of trace_level.
 *
 *  \par traceevels Trace level description
 *  -# trace_debug (5): Traces whose only purpose is for debugging the program,
 *     and which do not produce meaningful information otherwise.
 *  -# trace_info (4): Informational trace about the program execution. Should
 *     enable the user to see the execution flow.
 *  -# trace_warning (3): Indicates that a minor error has happened. In most case
 *     it can be discarded safely; it may even be expected.
 *  -# trace_error (2): Indicates an error which may not stop the program execution,
 *     but which indicates there is a problem with the code.
 *  -# trace_fatal (1): Indicates a major error which prevents the program from going
 *     any further. Program will stop after the fatal trace message is displayed.
 */

#ifndef _TRACE_H_
#define _TRACE_H_

/* ------------------------------------------------------------------------------
 *         Headers
 * ----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>

#define _STR(x) _VAL(x)
#define _VAL(x) #x

/* ------------------------------------------------------------------------------
 *         Exported Definitions
 * ----------------------------------------------------------------------------*/

#define TRACE_LEVEL_DEBUG      5
#define TRACE_LEVEL_INFO       4
#define TRACE_LEVEL_WARNING    3
#define TRACE_LEVEL_ERROR      2
#define TRACE_LEVEL_FATAL      1
#define TRACE_LEVEL_SILENT     0

#undef TRACE_LEVEL
//#define TRACE_LEVEL TRACE_LEVEL_DEBUG
/* By default, all traces are output except the debug one. */
#ifndef TRACE_LEVEL
#define TRACE_LEVEL TRACE_LEVEL_INFO
#endif

/* ------------------------------------------------------------------------------
 *         Exported variables
 * ----------------------------------------------------------------------------*/

/** Trace level is modifable at runtime */
extern uint32_t trace_level;

/* ------------------------------------------------------------------------------
 *         Exported functions
 * ----------------------------------------------------------------------------*/

/**
 *  Outputs a formatted string using 'printf' if the log level is high
 *  enough. Can be disabled by defining TRACE_LEVEL=0 during compilation.
 *  \param ...  Additional parameters depending on formatted string.
 */

#if (TRACE_LEVEL >= TRACE_LEVEL_FATAL)
#define trace_fatal(...)              { if (trace_level >= TRACE_LEVEL_FATAL) { printf("[FATAL]: " __VA_ARGS__); while(1); } }
#define trace_fatal_wp(...)           { if (trace_level >= TRACE_LEVEL_FATAL) { printf(__VA_ARGS__); while(1); } }
#else
#define trace_fatal(...)              { while (1); }
#define trace_fatal_wp(...)           { while (1); }
#endif

#if (TRACE_LEVEL >= TRACE_LEVEL_ERROR)
#define trace_error(...)              { if (trace_level >= TRACE_LEVEL_ERROR) { printf("[ERROR]: " __VA_ARGS__); } }
#define trace_error_wp(...)           { if (trace_level >= TRACE_LEVEL_ERROR) { printf(__VA_ARGS__); } }
#else
#define trace_error(...)              { }
#define trace_error_wp(...)           { }
#endif

#if (TRACE_LEVEL >= TRACE_LEVEL_WARNING)
#define trace_warning(...)            { if (trace_level >= TRACE_LEVEL_WARNING) { printf("[WARN]: " __VA_ARGS__); } }
#define trace_warning_wp(...)         { if (trace_level >= TRACE_LEVEL_WARNING) { printf(__VA_ARGS__); } }
#else
#define trace_warning(...)            { }
#define trace_warning_wp(...)         { }
#endif

#if (TRACE_LEVEL >= TRACE_LEVEL_INFO)
#define trace_info(...)               { if (trace_level >= TRACE_LEVEL_INFO) { printf("[INFO]: " __VA_ARGS__); } }
#define trace_info_wp(...)            { if (trace_level >= TRACE_LEVEL_INFO) { printf(__VA_ARGS__); } }
#else
#define trace_info(...)               { }
#define trace_info_wp(...)            { }
#endif

#if (TRACE_LEVEL >= TRACE_LEVEL_DEBUG)
#define trace_debug(...)              { if (trace_level >= TRACE_LEVEL_DEBUG) { printf("[DEBUG]: " __FILE__ ":" _STR(__LINE__) " " __VA_ARGS__); } }
#define trace_debug_wp(...)           { if (trace_level >= TRACE_LEVEL_DEBUG) { printf(__VA_ARGS__); } }
#else
#define trace_debug(...)              { }
#define trace_debug_wp(...)           { }
#endif

#endif /* _TRACE_H_ */

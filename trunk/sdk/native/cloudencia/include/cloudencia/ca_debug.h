#ifndef _CLOUDENCIA_DEBUG_H
#define _CLOUDENCIA_DEBUG_H

#if !defined(DEBUG_LEVEL_INFO)
#define DEBUG_LEVEL_INFO		4
#endif
#if !defined(DEBUG_LEVEL_WARN)
#define DEBUG_LEVEL_WARN		3
#endif
#if !defined(DEBUG_LEVEL_ERROR)
#define DEBUG_LEVEL_ERROR		2
#endif
#if !defined(DEBUG_LEVEL_FATAL)
#define DEBUG_LEVEL_FATAL		1
#endif

#ifdef __cplusplus
#	define CA_DEBUG_EXTERN extern "C"
#else
#	define CA_DEBUG_EXTERN extern
#endif

CA_DEBUG_EXTERN void tsk_debug_set_arg_data(const void*);
CA_DEBUG_EXTERN const void* tsk_debug_get_arg_data();
CA_DEBUG_EXTERN void tsk_debug_set_info_cb(int(*tsk_debug_f)(const void* arg, const char* fmt, ...));
CA_DEBUG_EXTERN int(*tsk_debug_get_info_cb())(const void* arg, const char* fmt, ...);
CA_DEBUG_EXTERN void tsk_debug_set_warn_cb(int(*tsk_debug_f)(const void* arg, const char* fmt, ...));
CA_DEBUG_EXTERN int(*tsk_debug_get_warn_cb())(const void* arg, const char* fmt, ...);
CA_DEBUG_EXTERN void tsk_debug_set_error_cb(int(*tsk_debug_f)(const void* arg, const char* fmt, ...));
CA_DEBUG_EXTERN int(*tsk_debug_get_error_cb())(const void* arg, const char* fmt, ...);
CA_DEBUG_EXTERN void tsk_debug_set_fatal_cb(int(*tsk_debug_f)(const void* arg, const char* fmt, ...));
CA_DEBUG_EXTERN int(*tsk_debug_get_fatal_cb())(const void* arg, const char* fmt, ...);
CA_DEBUG_EXTERN int tsk_debug_get_level();
CA_DEBUG_EXTERN void tsk_debug_set_level(int);


/* INFO */
#define CA_DEBUG_INFO(FMT, ...)		\
	if (tsk_debug_get_level() >= DEBUG_LEVEL_INFO) { \
		if (tsk_debug_get_info_cb()) \
			tsk_debug_get_info_cb()(tsk_debug_get_arg_data(), "*[CLOUDENCIA INFO]: " FMT "\n", ##__VA_ARGS__); \
								else \
			fprintf(stderr, "*[CLOUDENCIA INFO]: " FMT "\n", ##__VA_ARGS__); \
			}

/* WARN */
#define CA_DEBUG_WARN(FMT, ...)		\
	if (tsk_debug_get_level() >= DEBUG_LEVEL_WARN) { \
		if (tsk_debug_get_warn_cb()) \
			tsk_debug_get_warn_cb()(tsk_debug_get_arg_data(), "**[CLOUDENCIA WARN]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nMSG: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__); \
								else \
			fprintf(stderr, "**[CLOUDENCIA WARN]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nMSG: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__); \
			}

/* ERROR */
#define CA_DEBUG_ERROR(FMT, ...) 		\
	if (tsk_debug_get_level() >= DEBUG_LEVEL_ERROR) { \
		if (tsk_debug_get_error_cb()) \
			tsk_debug_get_error_cb()(tsk_debug_get_arg_data(), "***[CLOUDENCIA ERROR]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nMSG: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__); \
								else \
			fprintf(stderr, "***[CLOUDENCIA ERROR]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nMSG: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__); \
			}


/* FATAL */
#define CA_DEBUG_FATAL(FMT, ...) 		\
	if (tsk_debug_get_level() >= DEBUG_LEVEL_FATAL) { \
		if (tsk_debug_get_fatal_cb()) \
			tsk_debug_get_fatal_cb()(tsk_debug_get_arg_data(), "****[CLOUDENCIA FATAL]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nMSG: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__); \
								else \
			fprintf(stderr, "****[CLOUDENCIA FATAL]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nMSG: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__); \
			}

#define CA_DEBUG_INFO_EX(MODULE, FMT, ...) CA_DEBUG_INFO("[" MODULE "] " FMT, ##__VA_ARGS__)
#define CA_DEBUG_WARN_EX(MODULE, FMT, ...) CA_DEBUG_WARN("[" MODULE "] " FMT, ##__VA_ARGS__)
#define CA_DEBUG_ERROR_EX(MODULE, FMT, ...) CA_DEBUG_ERROR("[" MODULE "] " FMT, ##__VA_ARGS__)
#define CA_DEBUG_FATAL_EX(MODULE, FMT, ...) CA_DEBUG_FATAL("[" MODULE "] " FMT, ##__VA_ARGS__)

#endif /* _CLOUDENCIA_DEBUG_H */

/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_MMDB.c
 * Version    : $Id$
 * Copyright  : 2019-2019 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc MaxMindDB database wrapper
 *    - decoupling databases from main binary
 *    - optional support of dynamic library loading (based on config.h)
 *    - contains some generic wrapper functions
 */

#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#include "config.h"

#include "libipv6calcdebug.h"
#include "libipv6addr.h"

#include "libipv6calc_db_wrapper.h"

#ifdef SUPPORT_MMDB
#include "libipv6calc_db_wrapper_MMDB.h"

#ifdef SUPPORT_MMDB_DYN
char mmdb_lib_file[NI_MAXHOST] = MMDB_DYN_LIB;
static const char* wrapper_mmdb_info = "dyn-load";

/* define status and dynamic load functions */
static int dl_status_MMDB_open = IPV6CALC_DL_STATUS_UNKNOWN;
typedef int (*dl_MMDB_open_t)(const char *filename, int flags, MMDB_s *const mmdb);
static union { dl_MMDB_open_t func; void * obj; } dl_MMDB_open;

static int dl_status_MMDB_close = IPV6CALC_DL_STATUS_UNKNOWN;
typedef void (*dl_MMDB_close_t)(MMDB_s *const mmdb);
static union { dl_MMDB_close_t func; void * obj; } dl_MMDB_close;

static int dl_status_MMDB_lib_version = IPV6CALC_DL_STATUS_UNKNOWN;
typedef char *(*dl_MMDB_lib_version_t)(void);
static union { dl_MMDB_lib_version_t func; void * obj; } dl_MMDB_lib_version;

static int dl_status_MMDB_aget_value = IPV6CALC_DL_STATUS_UNKNOWN;
typedef int (*dl_MMDB_aget_value_t)(MMDB_entry_s *const start, MMDB_entry_data_s *const entry_data, const char *const *const path);
static union { dl_MMDB_aget_value_t func; void * obj; } dl_MMDB_aget_value;

static int dl_status_MMDB_get_entry_data_list = IPV6CALC_DL_STATUS_UNKNOWN;
typedef int (*dl_MMDB_get_entry_data_list_t)(MMDB_entry_s *start, MMDB_entry_data_list_s **const entry_data_list);
static union { dl_MMDB_get_entry_data_list_t func; void * obj; } dl_MMDB_get_entry_data_list;

static int dl_status_MMDB_free_entry_data_list = IPV6CALC_DL_STATUS_UNKNOWN;
typedef int (*dl_MMDB_free_entry_data_list_t)(MMDB_entry_data_list_s *const entry_data_list);
static union { dl_MMDB_free_entry_data_list_t func; void * obj; } dl_MMDB_free_entry_data_list;

static int dl_status_MMDB_dump_entry_data_list = IPV6CALC_DL_STATUS_UNKNOWN;
typedef int (*dl_MMDB_dump_entry_data_list_t)(FILE *const stream, MMDB_entry_data_list_s *const entry_data_list, int indent);
static union { dl_MMDB_dump_entry_data_list_t func; void * obj; } dl_MMDB_dump_entry_data_list;

static int dl_status_MMDB_lookup_sockaddr = IPV6CALC_DL_STATUS_UNKNOWN;
typedef MMDB_lookup_result_s (*dl_MMDB_lookup_sockaddr_t)(MMDB_s *const mmdb, const struct sockaddr *const sockaddr, int *const mmdb_error);
static union { dl_MMDB_lookup_sockaddr_t func; void * obj; } dl_MMDB_lookup_sockaddr;

static int dl_status_MMDB_strerror = IPV6CALC_DL_STATUS_UNKNOWN;
typedef const char *(*dl_MMDB_strerror_t)(int error_code);
static union { dl_MMDB_strerror_t func; void * obj; } dl_MMDB_strerror;

#else // SUPPORT_MMDB_DYN
static const char* wrapper_mmdb_info = "built-in";
#endif // SUPPORT_MMDB_DYN

static void *dl_MMDB_handle = NULL;

#define CHECK_STORE(MAXLEN, STORE, DESC) \
	if (entry_data.has_data) { \
		if (entry_data.type == MMDB_DATA_TYPE_UTF8_STRING) { \
			int max = (entry_data.data_size + 1 > MAXLEN) ? MAXLEN : entry_data.data_size +1; \
			snprintf(STORE, max , "%s", entry_data.utf8_string); \
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "%s: %s", DESC, STORE); \
		} else { \
			ERRORPRINT_WA("Lookup result from MaxMindDB has unexpected type for %s: %u", DESC, entry_data.type); \
		}; \
	} else { \
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "%s not found", DESC); \
	};

#define CHECK_STORE_DOUBLE(STORE, DESC) \
	if (entry_data.has_data) { \
		if (entry_data.type == MMDB_DATA_TYPE_DOUBLE) { \
			STORE = entry_data.double_value; \
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "%s: %lf", DESC, STORE); \
		} else { \
			ERRORPRINT_WA("Lookup result from MaxMindDB has unexpected type for %s: %u", DESC, entry_data.type); \
		}; \
	} else { \
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "%s not found", DESC); \
	};

#define CHECK_STORE_UINT32(STORE, DESC) \
	if (entry_data.has_data) { \
		if (entry_data.type == MMDB_DATA_TYPE_UINT32) { \
			STORE = entry_data.uint32; \
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "%s: %u", DESC, STORE); \
		} else { \
			ERRORPRINT_WA("Lookup result from MaxMindDB has unexpected type for %s: %u", DESC, entry_data.type); \
		}; \
	} else { \
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "%s not found", DESC); \
	};

#define CHECK_STORE_UINT16(STORE, DESC) \
	if (entry_data.has_data) { \
		if (entry_data.type == MMDB_DATA_TYPE_UINT16) { \
			STORE = entry_data.uint16; \
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "%s: %u", DESC, STORE); \
		} else { \
			ERRORPRINT_WA("Lookup result from MaxMindDB has unexpected type for %s: %u", DESC, entry_data.type); \
		}; \
	} else { \
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "%s not found", DESC); \
	};

/*
 * function initialise the MMDB wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error, 2=not found, 3=unknown
 */
int libipv6calc_db_wrapper_MMDB_wrapper_init(void) {
	int result = 3;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called");

#ifdef SUPPORT_MMDB_DYN
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Load library: %s", mmdb_lib_file);

	dlerror();    /* Clear any existing error */

	dl_MMDB_handle = dlopen(mmdb_lib_file, RTLD_NOW | RTLD_LOCAL);

	if (dl_MMDB_handle == NULL) {
		const char *error = dlerror();
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Load of library not successful: %s (%s)", mmdb_lib_file, error);
		if (strstr(error, "such file") != NULL) {
			result = 2;
			goto END_libipv6calc_db_wrapper;
		};
		if ((strcmp(mmdb_lib_file, MMDB_DYN_LIB) != 0) || (ipv6calc_verbose > 0)) {
			NONQUIETPRINT_WA("MMDB dynamic library load failed by problem with specified library (disable support): %s (%s)", mmdb_lib_file, error)
		};
		result = 1;
	} else {
		result = 0;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Loaded library successful: %s", mmdb_lib_file);
END_libipv6calc_db_wrapper:
#else // SUPPORT_MMDB_DYN
	// todo: library version check
	result = 0;
#endif // SUPPORT_MMDB_DYN

	return(result);
};


/*
 * function cleanup the MMDB wrapper
 *
 * in : (nothing)
 * out: (nothing)
 */
void libipv6calc_db_wrapper_MMDB_wrapper_cleanup(void) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called");

#ifdef SUPPORT_MMDB
	//libipv6calc_db_wrapper_MMDB_cleanup();
#endif

	dl_MMDB_handle = NULL; // disable handle

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "Finished");
	return;
};


/*
 * function info of MMDB wrapper
 *
 * in : ptr and size of string to be filled
 * out: modified string;
 */
void libipv6calc_db_wrapper_MMDB_wrapper_info(char* string, const size_t size) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called");

#ifdef SUPPORT_MMDB
	snprintf(string, size, "MMDB support active");
#else
	snprintf(string, size, "No MMDB support built-in");
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "Finished");
	return;
};


/***********************************************
 * Generic wrapper functions for MMDB for alignment
 ***********************************************/

/*
 * function cleanup the MMDB wrapper
 *
 * in : (nothing)
 * out: (nothing)
 */
void libipv6calc_db_wrapper_MMDB_cleanup(void) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called");

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "Not required for MMDB");

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "Finished");
	return;
};


/***********************************************
 * Wrapper functions for MMDB library
 *  autoswitch between dynamically linked/loaded
 ***********************************************/

/*
 * wrapper: MMDB_lib_version
 */
const char *libipv6calc_db_wrapper_MMDB_lib_version(void) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called: %s", wrapper_mmdb_info);

#ifdef SUPPORT_MMDB_DYN
	char *result_MMDB_lib_version = "unknown";
	const char *dl_symbol = "MMDB_lib_version";
	char *error;

        if (dl_MMDB_handle == NULL) {
                result_MMDB_lib_version = "LIBRARY-NOT-LOADED";
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_MMDB_lib_version == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_MMDB_lib_version.obj) = dlsym(dl_MMDB_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_MMDB_lib_version = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_MMDB_lib_version = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_MMDB_lib_version == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

	result_MMDB_lib_version = (*dl_MMDB_lib_version.func)();

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_MMDB_lib_version);
#else // SUPPORT_MMDB_DYN
	return(MMDB_lib_version());
#endif
};


/*
 * wrapper: MMDB_open
 */
int libipv6calc_db_wrapper_MMDB_open(const char *const filename, uint32_t flags, MMDB_s *const mmdb) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called: %s filename=%s", wrapper_mmdb_info, filename);

#ifdef SUPPORT_MMDB_DYN
	int r = MMDB_FILE_OPEN_ERROR;
	const char *dl_symbol = "MMDB_open";
	char *error;

	if (dl_MMDB_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "dl_MMDB_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_MMDB_open == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_MMDB_open.obj) = dlsym(dl_MMDB_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_MMDB_open = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_MMDB_open = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_MMDB_open == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

	r = (*dl_MMDB_open.func)(filename, flags, mmdb);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(r);
#else
	return(MMDB_open(filename, flags, mmdb));
#endif
};


/*
 * wrapper: MMDB_close
 */ 
void libipv6calc_db_wrapper_MMDB_close(MMDB_s* mmdb) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called: %s", wrapper_mmdb_info);

	if (mmdb == NULL) {
		return;
	};

#ifdef SUPPORT_MMDB_DYN
	const char *dl_symbol = "MMDB_close";
	char *error;

	if (dl_MMDB_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "dl_MMDB_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_MMDB_close == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_MMDB_close) = dlsym(dl_MMDB_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_MMDB_close = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_MMDB_close = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_MMDB_close == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

	(*dl_MMDB_close.func)(mmdb);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
#else
	MMDB_close(mmdb);
#endif

	return;
};


/*
 * wrapper: MMDB_aget_value
 */
int libipv6calc_db_wrapper_MMDB_aget_value (MMDB_entry_s *const start, MMDB_entry_data_s *const entry_data, const char *const *const path) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called: %s", wrapper_mmdb_info);

#ifdef SUPPORT_MMDB_DYN
	int result_MMDB_aget_value = 0;
	const char *dl_symbol = "MMDB_aget_value";
	char *error;

	if (dl_MMDB_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "dl_MMDB_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_MMDB_aget_value == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_MMDB_aget_value.obj) = dlsym(dl_MMDB_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_MMDB_aget_value = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_MMDB_aget_value = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_MMDB_aget_value == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

	result_MMDB_aget_value = (*dl_MMDB_aget_value.func)(start, entry_data, path);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_MMDB_aget_value);
#else
	return(MMDB_aget_value(start, entry_data, path));
#endif
};


/*
 * wrapper: MMDB_get_entry_data_list
 */
int libipv6calc_db_wrapper_MMDB_get_entry_data_list (MMDB_entry_s *start, MMDB_entry_data_list_s **const entry_data_list) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called: %s", wrapper_mmdb_info);

#ifdef SUPPORT_MMDB_DYN
	int result_MMDB_get_entry_data_list = 0;
	const char *dl_symbol = "MMDB_get_entry_data_list";
	char *error;

	if (dl_MMDB_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "dl_MMDB_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_MMDB_get_entry_data_list == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_MMDB_get_entry_data_list.obj) = dlsym(dl_MMDB_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_MMDB_get_entry_data_list = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_MMDB_get_entry_data_list = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_MMDB_get_entry_data_list == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

	result_MMDB_get_entry_data_list = (*dl_MMDB_get_entry_data_list.func)(start, entry_data_list);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_MMDB_get_entry_data_list);
#else
	return(MMDB_get_entry_data_list(start, entry_data_list));
#endif
};


/*
 * wrapper: MMDB_free_entry_data_list
 */
void libipv6calc_db_wrapper_MMDB_free_entry_data_list(MMDB_entry_data_list_s *const entry_data_list) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called: %s", wrapper_mmdb_info);

#ifdef SUPPORT_MMDB_DYN
	const char *dl_symbol = "MMDB_free_entry_data_list";
	char *error;

	if (dl_MMDB_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "dl_MMDB_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_MMDB_free_entry_data_list == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_MMDB_free_entry_data_list.obj) = dlsym(dl_MMDB_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_MMDB_free_entry_data_list = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_MMDB_free_entry_data_list = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_MMDB_free_entry_data_list == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

	(*dl_MMDB_free_entry_data_list.func)(entry_data_list);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return;
#else
	MMDB_free_entry_data_list(entry_data_list);
#endif
};


/*
 * wrapper: MMDB_dump_entry_data_list
 */
int libipv6calc_db_wrapper_MMDB_dump_entry_data_list(FILE *const stream, MMDB_entry_data_list_s *const entry_data_list, int indent) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called: %s", wrapper_mmdb_info);

#ifdef SUPPORT_MMDB_DYN
	const char *dl_symbol = "MMDB_dump_entry_data_list";
	char *error;
	int result_MMDB_dump_entry_data_list = 0;

	if (dl_MMDB_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "dl_MMDB_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_MMDB_dump_entry_data_list == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_MMDB_dump_entry_data_list.obj) = dlsym(dl_MMDB_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_MMDB_dump_entry_data_list = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_MMDB_dump_entry_data_list = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_MMDB_dump_entry_data_list == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

	result_MMDB_dump_entry_data_list = (*dl_MMDB_dump_entry_data_list.func)(stream, entry_data_list, indent);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_MMDB_dump_entry_data_list);
#else
	return MMDB_dump_entry_data_list(stream, entry_data_list, indent);
#endif
};


/*
 * wrapper: MMDB_lookup_sockaddr
 */
MMDB_lookup_result_s libipv6calc_db_wrapper_MMDB_lookup_sockaddr (MMDB_s *const mmdb, const struct sockaddr *const sockaddr, int *const mmdb_error) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called: %s", wrapper_mmdb_info);

#ifdef SUPPORT_MMDB_DYN
	MMDB_lookup_result_s result_MMDB_lookup_sockaddr;
	const char *dl_symbol = "MMDB_lookup_sockaddr";
	char *error;

	if (dl_MMDB_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "dl_MMDB_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_MMDB_lookup_sockaddr == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_MMDB_lookup_sockaddr.obj) = dlsym(dl_MMDB_handle, "MMDB_lookup_sockaddr");

		if ((error = dlerror()) != NULL)  {
			dl_status_MMDB_lookup_sockaddr = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_MMDB_lookup_sockaddr = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_MMDB_lookup_sockaddr == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

	result_MMDB_lookup_sockaddr = (*dl_MMDB_lookup_sockaddr.func)(mmdb, sockaddr, mmdb_error);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_MMDB_lookup_sockaddr);
#else
	return(MMDB_lookup_sockaddr(mmdb, sockaddr, mmdb_error));
#endif
};


/*
 * wrapper: MMDB_strerror
 */
const char *libipv6calc_db_wrapper_MMDB_strerror (int error_code) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called: %s", wrapper_mmdb_info);

#ifdef SUPPORT_MMDB_DYN
	const char *result_MMDB_strerror = "";
	const char *dl_symbol = "MMDB_strerror";
	char *error;

	if (dl_MMDB_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "dl_MMDB_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_MMDB_strerror == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_MMDB_strerror.obj) = dlsym(dl_MMDB_handle, "MMDB_strerror");

		if ((error = dlerror()) != NULL)  {
			dl_status_MMDB_strerror = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_MMDB_strerror = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_MMDB_strerror == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

	result_MMDB_strerror = (*dl_MMDB_strerror.func)(error_code);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_MMDB_strerror);
#else
	return(MMDB_strerror(error_code));
#endif
};


/***********************************************
 * Special wrapper functions for MMDB to avoid duplicate code
 ***********************************************/

/* Lookup By Addr
 * in : ipaddrp, mmdb
 * mod: mmdb_error
 * out: MMDB_lookup_result_s
 */
MMDB_lookup_result_s libipv6calc_db_wrapper_MMDB_wrapper_lookup_by_addr (const ipv6calc_ipaddr *ipaddrp, MMDB_s *const mmdb, int *const mmdb_error) {
	MMDB_lookup_result_s lookup_result;
	lookup_result.found_entry = false;

	// convert ipaddrp into sockaddr
	union sockaddr_u {
		struct sockaddr_in in;
		struct sockaddr_in6 in6;
		struct sockaddr sockaddr;
	} su;

	if (ipaddrp->proto == 4) {
		su.sockaddr.sa_family = AF_INET;
		su.in.sin_addr.s_addr = htonl(ipaddrp->addr[0]);
	} else if (ipaddrp->proto == 6) {
		su.sockaddr.sa_family = AF_INET6;
		su.in6.sin6_addr.s6_addr32[0] = htonl(ipaddrp->addr[0]);
		su.in6.sin6_addr.s6_addr32[1] = htonl(ipaddrp->addr[1]);
		su.in6.sin6_addr.s6_addr32[2] = htonl(ipaddrp->addr[2]);
		su.in6.sin6_addr.s6_addr32[3] = htonl(ipaddrp->addr[3]);
	} else {
		*mmdb_error = MMDB_INVALID_DATA_ERROR;
		goto END_libipv6calc_db_wrapper;
	};

	lookup_result = libipv6calc_db_wrapper_MMDB_lookup_sockaddr(mmdb, &su.sockaddr, mmdb_error);

END_libipv6calc_db_wrapper:
	return(lookup_result);
};


/* Country Code By Addr
 * in : ipaddrp, country_len, country, mmdb
 * mod: country
 * out: mmdb_error
 */
int libipv6calc_db_wrapper_MMDB_country_code_by_addr(const ipv6calc_ipaddr *ipaddrp, char *country, const size_t country_len, MMDB_s *const mmdb) {
	MMDB_lookup_result_s lookup_result;
	MMDB_entry_data_s entry_data;
	int mmdb_error = MMDB_INVALID_DATA_ERROR;

	lookup_result = libipv6calc_db_wrapper_MMDB_wrapper_lookup_by_addr(ipaddrp, mmdb, &mmdb_error);

	if (mmdb_error != MMDB_SUCCESS) {
		goto END_libipv6calc_db_wrapper;
	};

	// fetch CountryCode
	const char *lookup_path_country_code[] = { "country", "iso_code", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_country_code);
	if (entry_data.has_data) {
		if (entry_data.type == MMDB_DATA_TYPE_UTF8_STRING) {
			int max = (entry_data.data_size + 1 > country_len) ? country_len : entry_data.data_size +1;
			snprintf(country, max , "%s", entry_data.utf8_string);
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "CountryCode: %s", country);
		} else {
			ERRORPRINT_WA("Lookup result from MaxMindDB has unexpected type for CountryCode: %u", entry_data.type);
		};
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "CountryCode not found");
		mmdb_error = MMDB_INVALID_DATA_ERROR;
	};

END_libipv6calc_db_wrapper:
	return(mmdb_error);
};


/* ASN By Addr
 * in : ipaddrp, mmdb
 * out: asn
 */
uint32_t libipv6calc_db_wrapper_MMDB_asn_by_addr(const ipv6calc_ipaddr *ipaddrp, MMDB_s *const mmdb) {
	MMDB_lookup_result_s lookup_result;
	MMDB_entry_data_s entry_data;
	int mmdb_error = MMDB_INVALID_DATA_ERROR;
	uint32_t result = ASNUM_AS_UNKNOWN;

	lookup_result = libipv6calc_db_wrapper_MMDB_wrapper_lookup_by_addr(ipaddrp, mmdb, &mmdb_error);

	if (mmdb_error != MMDB_SUCCESS) {
		goto END_libipv6calc_db_wrapper;
	};

	// fetch ASN
	if(strstr(mmdb->metadata.database_type, "ASN")) {
		// GeoLite2-ASN
		const char *lookup_path_asn[] = { "autonomous_system_number", NULL };
		libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_asn);
	} else {
		const char *lookup_path_asn[] = { "traits", "autonomous_system_number", NULL };
		libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_asn);
	};
	if (entry_data.has_data) {
		if (entry_data.type == MMDB_DATA_TYPE_UINT32) {
			result = entry_data.uint32;
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "ASN: %u", result);
		} else {
			ERRORPRINT_WA("Lookup result from MaxMindDB has unexpected type for ASN: %u", entry_data.type);
		};
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_MMDB, "ASN not found");
		mmdb_error = MMDB_INVALID_DATA_ERROR;
	};

END_libipv6calc_db_wrapper:
	return(result);
};


/* GeonameID By Addr
 * in : ipaddrp, mmdb
 * mod: source
 * out: GeonameID
 */
uint32_t libipv6calc_db_wrapper_MMDB_GeonameID_by_addr(const ipv6calc_ipaddr *ipaddrp, MMDB_s *const mmdb, int *source_ptr) {
	MMDB_lookup_result_s lookup_result;
	MMDB_entry_data_s entry_data;
	int mmdb_error = MMDB_INVALID_DATA_ERROR;
	uint32_t result = IPV6CALC_DB_GEO_GEONAMEID_UNKNOWN;
	int source = IPV6CALC_DB_GEO_GEONAMEID_SOURCE_UNKNOWN;

	int limit_24bit = 0;

	if ((source_ptr != NULL) \
		&& ((*source_ptr & IPV6CALC_DB_GEO_GEONAMEID_SOURCE_FLAG_MASK) == IPV6CALC_DB_GEO_GEONAMEID_SOURCE_FLAG_24BIT)) {
			limit_24bit = 1;
	};

	lookup_result = libipv6calc_db_wrapper_MMDB_wrapper_lookup_by_addr(ipaddrp, mmdb, &mmdb_error);

	if (mmdb_error != MMDB_SUCCESS) {
		goto END_libipv6calc_db_wrapper;
	};

	// fetch GeonameID (nearest to global)
	// city
	const char *lookup_path_city_geonameid[] = { "city", "geoname_id", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_city_geonameid);
	CHECK_STORE_UINT32(result, "City/GeonameId")
	source = IPV6CALC_DB_GEO_GEONAMEID_SOURCE_CITY;
	if ((result != IPV6CALC_DB_GEO_GEONAMEID_UNKNOWN) && ((limit_24bit == 0) || (source < 0x1000000))) { goto END_libipv6calc_db_wrapper; };

	// district
	const char *lookup_path_district_geonameid[] = { "subdivisions", "1", "geoname_id", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_district_geonameid);
	CHECK_STORE_UINT32(result, "District(subdivision#1)/GeonameId")
	source = IPV6CALC_DB_GEO_GEONAMEID_SOURCE_DISTRICT;
	if ((result != IPV6CALC_DB_GEO_GEONAMEID_UNKNOWN) && ((limit_24bit == 0) || (source < 0x1000000))) { goto END_libipv6calc_db_wrapper; };

	// stateprov
	const char *lookup_path_stateprov_geonameid[] = { "subdivisions", "0", "geoname_id", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_stateprov_geonameid);
	CHECK_STORE_UINT32(result, "State/Prov(subdivsion#0)/GeonameId")
	source = IPV6CALC_DB_GEO_GEONAMEID_SOURCE_STATEPROV;
	if ((result != IPV6CALC_DB_GEO_GEONAMEID_UNKNOWN) && ((limit_24bit == 0) || (source < 0x1000000))) { goto END_libipv6calc_db_wrapper; };

	// country
	const char *lookup_path_country_geonameid[] = { "country", "geoname_id", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_country_geonameid);
	CHECK_STORE_UINT32(result, "Country/GeonameId")
	source = IPV6CALC_DB_GEO_GEONAMEID_SOURCE_COUNTRY;
	if ((result != IPV6CALC_DB_GEO_GEONAMEID_UNKNOWN) && ((limit_24bit == 0) || (source < 0x1000000))) { goto END_libipv6calc_db_wrapper; };

	// continent
	const char *lookup_path_continent_geonameid[] = { "continent", "geoname_id", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_continent_geonameid);
	CHECK_STORE_UINT32(result, "Continent/GeonameId")
	source = IPV6CALC_DB_GEO_GEONAMEID_SOURCE_CONTINENT;
	if ((result != IPV6CALC_DB_GEO_GEONAMEID_UNKNOWN) && ((limit_24bit == 0) || (source < 0x1000000))) { goto END_libipv6calc_db_wrapper; };

	source = IPV6CALC_DB_GEO_GEONAMEID_SOURCE_UNKNOWN;

END_libipv6calc_db_wrapper:
	if (source_ptr != NULL) {
		*source_ptr = source;
	};
	return(result);
};


/* all information by addr
 * in : ipaddrp, recordp
 * mod: recordp
 * out: mmdb_error
 */
int libipv6calc_db_wrapper_MMDB_all_by_addr(const ipv6calc_ipaddr *ipaddrp, libipv6calc_db_wrapper_geolocation_record *recordp, MMDB_s *const mmdb) {
	MMDB_lookup_result_s lookup_result;
	MMDB_entry_data_s entry_data;
	int mmdb_error;

	static char resultstring[NI_MAXHOST];

	libipv6calc_db_wrapper_geolocation_record_clear(recordp);

	// convert ipaddrp into sockaddr
	union sockaddr_u {
		struct sockaddr_in in;
		struct sockaddr_in6 in6;
		struct sockaddr sockaddr;
	} su;

	if (ipaddrp->proto == 4) {
		su.sockaddr.sa_family = AF_INET;
		su.in.sin_addr.s_addr = htonl(ipaddrp->addr[0]);
	} else if (ipaddrp->proto == 6) {
		su.sockaddr.sa_family = AF_INET6;
		su.in6.sin6_addr.s6_addr32[0] = htonl(ipaddrp->addr[0]);
		su.in6.sin6_addr.s6_addr32[1] = htonl(ipaddrp->addr[1]);
		su.in6.sin6_addr.s6_addr32[2] = htonl(ipaddrp->addr[2]);
		su.in6.sin6_addr.s6_addr32[3] = htonl(ipaddrp->addr[3]);
	} else {
		mmdb_error = MMDB_INVALID_DATA_ERROR;
		goto END_libipv6calc_db_wrapper;
	};

	lookup_result = libipv6calc_db_wrapper_MMDB_lookup_sockaddr(mmdb, &su.sockaddr, &mmdb_error);

	if (mmdb_error != MMDB_SUCCESS) {
		ERRORPRINT_WA("Lookup results in error from MaxMindDB library: %s", libipv6calc_db_wrapper_MMDB_strerror(mmdb_error));
		goto END_libipv6calc_db_wrapper;
	};

	// dump contents for debug
	DEBUGSECTION_BEGIN(DEBUG_libipv6calc_db_wrapper_MMDB)
		MMDB_entry_data_list_s *entry_data_list = NULL;
		int status = libipv6calc_db_wrapper_MMDB_get_entry_data_list(&lookup_result.entry, &entry_data_list);

		if (status == MMDB_SUCCESS) {
			libipv6calc_db_wrapper_MMDB_dump_entry_data_list(stderr, entry_data_list, 2);
		};
		libipv6calc_db_wrapper_MMDB_free_entry_data_list(entry_data_list);
	DEBUGSECTION_END

	// fetch CountryCode
	const char *lookup_path_country_code[] = { "country", "iso_code", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_country_code);
	CHECK_STORE(IPV6CALC_DB_SIZE_COUNTRY_CODE, recordp->country_code, "CountryCode")

	// fetch CountryName
	const char *lookup_path_country_name[] = { "country", "names", "en", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_country_name);
	CHECK_STORE(IPV6CALC_DB_SIZE_COUNTRY_LONG, recordp->country_long, "CountryName")

	// fetch ContinentCode
	const char *lookup_path_continent_code[] = { "continent", "code", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_continent_code);
	CHECK_STORE(IPV6CALC_DB_SIZE_CONTINENT_CODE, recordp->continent_code, "ContinentCode")

	// fetch ContinentName
	const char *lookup_path_continent_name[] = { "continent", "names", "en", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_continent_name);
	CHECK_STORE(IPV6CALC_DB_SIZE_CONTINENT_LONG, recordp->continent_long, "ContinentName")

	// fetch Location Latitude/Longitude
	const char *lookup_path_location_latitude[] = { "location", "latitude", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_location_latitude);
	CHECK_STORE_DOUBLE(recordp->latitude, "Latitude")

	const char *lookup_path_location_longitude[] = { "location", "longitude", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_location_longitude);
	CHECK_STORE_DOUBLE(recordp->longitude, "Longitude")

	// fetch accuracy_radius
	const char *lookup_path_location_radius[] = { "location", "accuracy_radius", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_location_radius);
	CHECK_STORE_UINT16(recordp->accuracy_radius, "Radius")

	// weather_code
	const char *lookup_path_location_weather_code[] = { "location", "weather_code", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_location_weather_code);
	CHECK_STORE(IPV6CALC_DB_SIZE_WEATHERSTATIONCODE, recordp->weatherstationcode, "WeatherStationCode")

	// fetch City
	const char *lookup_path_city_name[] = { "city", "names", "en", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_city_name);
	CHECK_STORE(IPV6CALC_DB_SIZE_CITY, recordp->city, "City")

	// fetch GeonameId of Continent
	const char *lookup_path_continent_geonameid[] = { "continent", "geoname_id", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_continent_geonameid);
	CHECK_STORE_UINT32(recordp->continent_geoname_id, "Continent/GeonameId")

	// fetch GeonameId of Country
	const char *lookup_path_country_geonameid[] = { "country", "geoname_id", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_country_geonameid);
	CHECK_STORE_UINT32(recordp->country_geoname_id, "Country/GeonameId")

	// fetch GeonameId of City
	const char *lookup_path_city_geonameid[] = { "city", "geoname_id", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_city_geonameid);
	CHECK_STORE_UINT32(recordp->geoname_id, "City/GeonameId")

	// fetch postal code
	const char *lookup_path_zipcode[] = { "postal", "code", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_zipcode);
	CHECK_STORE(IPV6CALC_DB_SIZE_ZIPCODE, recordp->zipcode, "PostalCode")

	// time zone
	const char *lookup_path_timezone[] = { "location", "time_zone", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_timezone);
	CHECK_STORE(IPV6CALC_DB_SIZE_TIMEZONE_NAME, recordp->timezone_name, "TimeZoneName")

	// ISP
	const char *lookup_path_isp[] = { "traits", "isp", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_isp);
	CHECK_STORE(IPV6CALC_DB_SIZE_ISP_NAME, recordp->isp_name, "ISP")

	// connection type
	const char *lookup_path_connection_type[] = { "traits", "connection_type", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_connection_type);
	CHECK_STORE(IPV6CALC_DB_SIZE_CONN_TYPE, recordp->connection_type, "ConnectionType")

	// fetch ASN
	if(strstr(mmdb->metadata.database_type, "ASN")) {
		// GeoLite2-ASN
		const char *lookup_path_asn[] = { "autonomous_system_number", NULL };
		libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_asn);
	} else {
		const char *lookup_path_asn[] = { "traits", "autonomous_system_number", NULL };
		libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_asn);
	};
	CHECK_STORE_UINT32(recordp->asn, "Autonomous System Number");

	// organization name
	if(strstr(mmdb->metadata.database_type, "ASN")) {
		// GeoLite2-ASN
		const char *lookup_path_organization_name[] = { "autonomous_system_organization", NULL };
		libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_organization_name);
	} else {
		const char *lookup_path_organization_name[] = { "traits", "autonomous_system_organization", NULL };
		libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_organization_name);
	};
	CHECK_STORE(IPV6CALC_DB_SIZE_ORG_NAME, recordp->organization_name, "AutonomousSystemOrganization")

	// fetch StateProv / District (Subdivision) - which is a list
	const char *lookup_path_stateprov[] = { "subdivisions", "0", "names", "en", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_stateprov);
	CHECK_STORE(IPV6CALC_DB_SIZE_STATEPROV, recordp->stateprov, "State/Province(subdivision#0)")

	const char *lookup_path_stateprov_geonameid[] = { "subdivisions", "0", "geoname_id", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_stateprov_geonameid);
	CHECK_STORE_UINT32(recordp->stateprov_geoname_id, "State/Prov(subdivsion#0)/GeonameId")

	const char *lookup_path_distinct[] = { "subdivisions", "1", "names", "en", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_distinct);
	CHECK_STORE(IPV6CALC_DB_SIZE_DISTRICT, recordp->district, "District(subdivsion#1)")

	const char *lookup_path_district_geonameid[] = { "subdivisions", "1", "geoname_id", NULL };
	libipv6calc_db_wrapper_MMDB_aget_value(&lookup_result.entry, &entry_data, lookup_path_district_geonameid);
	CHECK_STORE_UINT32(recordp->district_geoname_id, "District(subdivision#1)/GeonameId")

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_MMDB, "resultstring=%s", resultstring);

END_libipv6calc_db_wrapper:
	return(mmdb_error);
};

#endif  // MMDB

/**
 ****************************************************************************************************
 * @author      正点原子团队(ALIENTEK)
 * @date        2023-07-18
 * @license     Copyright (c) 2023-2035, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:zhengdianyuanzi.tmall.com
 *
 ****************************************************************************************************
 */


#ifndef __ATK_DECODER_H__
#define __ATK_DECODER_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stdarg.h>

/* glib ---------------------------------------------- */
typedef struct _atk_gslist
{
  void                  *data;
  struct _atk_gslist    *next;
}atk_GSList;   /* == GSList */

typedef void        atk_GVariant;  /* == GVariant */

typedef void        atk_GArray;     /* == GArray */

typedef void        atk_GThread;    /* == atk_GThread */

typedef int         atk_gboolean;   /* == gboolean */

typedef union _atk_GMutex
{
  /*< private >*/
  void* p;
  unsigned int i[2];
}atk_GMutex;

typedef struct _atk_GCond
{
  /*< private >*/
  void* p;
  unsigned int i[2];
}atk_GCond;


typedef void        atk_GVariantType; /* ==GVariantType */

typedef void        atk_GHashTable;  /*  == GHashTable */

#define ATK_GPOINTER_TO_SIZE(data)	    ((size_t)(data))  /* == GPOINTER_TO_SIZE */

/* --------------------------------------------------- */




/*decoder ------------------------------------------------------------------------------------------------------ */

/** Status/error codes returned by libsigrokdecode functions. */
enum atk_error_code {
	ATK_OK               =  0, /**< No error */
	ATK_ERR              = -1, /**< Generic/unspecified error */
	ATK_ERR_MALLOC       = -2, /**< Malloc/calloc/realloc error */
	ATK_ERR_ARG          = -3, /**< Function argument error */
	ATK_ERR_BUG          = -4, /**< Errors hinting at internal bugs */
	ATK_ERR_PYTHON       = -5, /**< Python C API error */
	ATK_ERR_DECODERS_DIR = -6, /**< Protocol decoder path invalid */
	ATK_ERR_TERM_REQ     = -7, /**< Termination requested */

	/*
	 * Note: When adding entries here, don't forget to also update the
	 * atk_strerror() and atk_strerror_name() functions in error.c.
	 */
};


/* libsigrokdecode loglevels. */
enum atk_loglevel {
	ATK_LOG_NONE = 0, /**< Output no messages at all. */
	ATK_LOG_ERR  = 1, /**< Output error messages. */
	ATK_LOG_WARN = 2, /**< Output warnings. */
	ATK_LOG_INFO = 3, /**< Output informational messages. */
	ATK_LOG_DBG  = 4, /**< Output debug messages. */
	ATK_LOG_SPEW = 5, /**< Output very noisy debug messages. */
};


enum atk_output_type {
	ATK_OUTPUT_ANN,
	ATK_OUTPUT_PYTHON,
	ATK_OUTPUT_BINARY,
	ATK_OUTPUT_LOGIC,
	ATK_OUTPUT_META,
};

enum atk_configkey {
	ATK_CONF_SAMPLERATE = 10000,
};



typedef void        atk_session;

struct atk_decoder {
	/** The decoder ID. Must be non-NULL and unique for all decoders. */
	char *id;

	/** The (short) decoder name. Must be non-NULL. */
	char *name;

	/** The (long) decoder name. Must be non-NULL. */
	char *longname;

	/** A (short, one-line) description of the decoder. Must be non-NULL. */
	char *desc;

	/**
	 * The license of the decoder. Valid values: "gplv2+", "gplv3+".
	 * Other values are currently not allowed. Must be non-NULL.
	 */
	char *license;

	/** List of possible decoder input IDs. */
	atk_GSList *inputs;

	/** List of possible decoder output IDs. */
	atk_GSList *outputs;

	/** List of tags associated with this decoder. */
	atk_GSList *tags;

	/** List of channels required by this decoder. */
	atk_GSList *channels;

	/** List of optional channels for this decoder. */
	atk_GSList *opt_channels;

	/**
	 * List of annotation classes. Each list item is a atk_GSList itself, with
	 * two NUL-terminated strings: name and description.
	 */
	atk_GSList *annotations;

	/**
	 * List of annotation rows (row items: id, description, and a list
	 * of annotation classes belonging to this row).
	 */
	atk_GSList *annotation_rows;

	/**
	 * List of binary classes. Each list item is a atk_GSList itself, with
	 * two NUL-terminated strings: name and description.
	 */
	atk_GSList *binary;

	/**
	 * List of logic output channels (item: id, description).
	 */
	atk_GSList *logic_output_channels;

	/** List of decoder options. */
	atk_GSList *options;

	/** Python module. */
	void *py_mod;

	/** sigrokdecode.Decoder class. */
	void *py_dec;
};


struct atk_channel {
	/** The ID of the channel. Must be non-NULL. */
	char *id;
	/** The name of the channel. Must not be NULL. */
	char *name;
	/** The description of the channel. Must not be NULL. */
	char *desc;
	/** The index of the channel, i.e. its order in the list of channels. */
	int order;
};

struct atk_decoder_option {
	char *id;
	char *desc;
	atk_GVariant *def;
	atk_GSList *values;
};

struct atk_decoder_annotation_row {
	char *id;
	char *desc;
	atk_GSList *ann_classes;
};

struct atk_decoder_logic_output_channel {
	char *id;
	char *desc;
};


struct atk_input_data {
    uint8_t *data;
    uint8_t constant;
};

struct atk_decoder_inst {
	struct atk_decoder *decoder;
	atk_session *sess;
	void *py_inst;
	char *inst_id;
	atk_GSList *pd_output;
	int dec_num_channels;
	int *dec_channelmap;
	// int data_unitsize;
	uint8_t *channel_samples;
	atk_GSList *next_di;

	/** List of conditions a PD wants to wait for. */
	atk_GSList *condition_list;

	/** Array of booleans denoting which conditions matched. */
	atk_GArray *match_array;

	/** Absolute start sample number. */
	uint64_t abs_start_samplenum;

	/** Absolute end sample number. */
	uint64_t abs_end_samplenum;

	/** Pointer to the buffer/chunk of input samples. */
	const struct atk_input_data *inbuf;

	/** Length (in bytes) of the input sample buffer. */
	// uint64_t inbuflen;

	/** Absolute current samplenumber. */
	uint64_t    abs_cur_samplenum;

    atk_gboolean    is_input_const;

    atk_gboolean    is_skip_cond;
    atk_gboolean        have_skip_cond;

    uint64_t    skip_min_num;

	/** Array of "old" (previous sample) pin values. */
	atk_GArray *old_pins_array;

	/** Handle for this PD stack's worker thread. */
	atk_GThread *thread_handle;

	/** Indicates whether new samples are available for processing. */
	atk_gboolean got_new_samples;

	/** Indicates whether the worker thread has handled all samples. */
	atk_gboolean handled_all_samples;

	/** Requests termination of wait() and decode(). */
	atk_gboolean want_wait_terminate;

	/** Requests that .wait() terminates a Python iteration. */
	atk_gboolean communicate_eof;

	/** Indicates the current state of the decoder stack. */
	int decoder_state;

	atk_GCond got_new_samples_cond;
	atk_GCond handled_all_samples_cond;
	atk_GMutex data_mutex;
};

struct atk_pd_output {
	int pdo_id;
	int output_type;
	struct atk_decoder_inst *di;
	char *proto_id;
	/* Only used for OUTPUT_META. */
	const atk_GVariantType *meta_type;
	char *meta_name;
	char *meta_descr;
};

struct atk_proto_data {
	uint64_t start_sample;
	uint64_t end_sample;
	struct atk_pd_output *pdo;
	void *data;
};
struct atk_proto_data_annotation {
	int ann_class; /* Index into "struct atk_decoder"->annotations. */
    int ann_row;
	char **ann_text;
};
struct atk_proto_data_binary {
	int bin_class; /* Index into "struct atk_decoder"->binary. */
	uint64_t size;
	const uint8_t *data;
};
struct atk_proto_data_logic {
	int logic_group;
	uint64_t repeat_count; /* Number of times the value in data was repeated. */
	const uint8_t *data; /* Bitfield containing the states of the logic outputs */
};

typedef void (*atk_pd_output_callback)(struct atk_proto_data *pdata,
					void *cb_data);

struct atk_pd_callback {
	int output_type;
	atk_pd_output_callback cb;
	void *cb_data;
};

/* ---------------------------------------------------------------------------------------------------------------------------------------- */





/* atk_decoder.c */
#define ATK_DECODER_VERSION_STRING          "1.0"
#define ATK_DECODER_VERSION_MAJOR           1
#define ATK_DECODER_VERSION_MINOR           0

char *atk_decoder_version_string_get(void);
int atk_decoder_version_major_get(void);
int atk_decoder_version_minor_get(void);

/**
 * @brief       获取GVariant的值，以字符串新式返回
 *              如果数据为浮点数，字符串需要补一个小数位 ".0"
 * 
 * @retval        返回字符串需要使用 atk_get_gvariant_free 函数释放内存
 */
char* atk_get_gvariant_value(atk_GVariant* var);

void atk_get_gvariant_free(char *str);

/**
 * @brief       获取变量类型
 * @retval      -1 : 获取失败
 *              1  : 数值 
 *              2  : 字符串
 */
int atk_get_gvariant_type(atk_GVariant* var);





/* srd.c */
/**
 * @brief       设置三个环境的路径 (最好为：./ ), 参数设置无效
 *               
 *              1. decode_path       解码库
 *              2. python_lib path   python标准库
 *              3. run_lib path      glib python动态库路径 （注意：windows的运行环境路径和exe放在同一级目录）
 */
int atk_decoder_init(const char *path);
int atk_decoder_exit(void);
atk_GSList *atk_searchpaths_get(void);


/* session.c */
atk_session *atk_decoder_session_new(void);
int atk_decoder_session_start(atk_session *sess);
int atk_decoder_session_metadata_set_samplerate(atk_session *sess, uint64_t samplerate_hz);
int atk_decoder_session_send(atk_session *sess,
                            uint64_t abs_start_samplenum, uint64_t abs_end_samplenum,
                            struct atk_input_data *inbuf);
int atk_decoder_session_send_eof(atk_session *sess);
int atk_decoder_session_terminate_reset(atk_session *sess);
int atk_decoder_session_destroy(atk_session *sess);
int atk_decoder_pd_output_callback_add(atk_session *sess,
                                       int output_type, atk_pd_output_callback cb, void *cb_data);



/* decoder.c */
const atk_GSList *atk_decoder_list(void);
struct atk_decoder *atk_decoder_get_by_id(const char *id);
int atk_decoder_load(const char *name);
char *atk_decoder_doc_get(const struct atk_decoder *dec);
int atk_decoder_unload(struct atk_decoder *dec);
int atk_decoder_load_all(void);
int atk_decoder_unload_all(void);


/* hash table */
atk_GHashTable* atk_decoder_hashtable_create(void);
void atk_decoder_hashtable_destroy(atk_GHashTable *hashtable);
int atk_decoder_hashtable_set_option(atk_GHashTable *hashtable, struct atk_decoder *decoder, const char *key, const char *val);
void atk_decoder_hashtable_set_channel(atk_GHashTable *hashtable, const char *key, int val);


/* instance.c */
int atk_decoder_inst_option_set(struct atk_decoder_inst *di,
        atk_GHashTable *options);
int atk_decoder_inst_channel_set_all(struct atk_decoder_inst *di,
        atk_GHashTable *channels);
struct atk_decoder_inst *atk_decoder_inst_new(atk_session *sess,
                        const char *id, atk_GHashTable *options);
int atk_decoder_inst_stack(atk_session *sess,
        struct atk_decoder_inst *di_bottom, struct atk_decoder_inst *di_top);
struct atk_decoder_inst *atk_decoder_inst_find_by_id(atk_session *sess,
        const char *inst_id);
int atk_decoder_inst_initial_pins_set_all(struct atk_decoder_inst *di,
        atk_GArray *initial_pins);



/* log.c */
typedef int (*atk_log_callback)(void *cb_data, int loglevel,
				  const char *format, va_list args);
int atk_log_loglevel_set(int loglevel);
int atk_log_loglevel_get(void);
int atk_log_callback_get(atk_log_callback *cb, void **cb_data);
int atk_log_callback_set(atk_log_callback cb, void *cb_data);
int atk_log_callback_set_default(void);




/* error.c */
const char *atk_strerror(int error_code);
const char *atk_strerror_name(int error_code);



/* version.c */
int atk_package_version_major_get(void);
int atk_package_version_minor_get(void);
int atk_package_version_micro_get(void);
const char *atk_package_version_string_get(void);
int atk_lib_version_current_get(void);
int atk_lib_version_revision_get(void);
int atk_lib_version_age_get(void);
const char *atk_version_string_get(void);
atk_GSList *atk_buildinfo_libs_get(void);
char *atk_buildinfo_host_get(void);



#ifdef __cplusplus
}
#endif



#endif






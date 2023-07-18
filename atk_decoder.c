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


#include "atk_decoder.h"
#include <stdlib.h>
#include "libsigrokdecode.h"
#include "libsigrokdecode-internal.h"
#include "glib.h"




/**
 * @brief       获取GVariant的值，以字符串新式返回
 *              如果数据为浮点数，字符串需要补一个小数位 ".0"
 * 
 * @retval        返回字符串需要使用 atk_get_gvariant_free 函数释放内存
 */
char* atk_get_gvariant_value(atk_GVariant* var)
{
    char data[100];
    char *value_str;

    if (g_variant_is_of_type(var, G_VARIANT_TYPE("d")))
    {
        gdouble value = g_variant_get_double(var);
        sprintf(data, "%.1lf", value);
    }
    else if (g_variant_is_of_type(var, G_VARIANT_TYPE("x")))
    {
        int64_t int_val = 0;
        const GVariantType* const type = g_variant_get_type(var);
        if (g_variant_type_equal(type, G_VARIANT_TYPE_BYTE)) {
            int_val = g_variant_get_byte(var);
            sprintf(data, "%ld", int_val);
        }
        else if (g_variant_type_equal(type, G_VARIANT_TYPE_INT16)){
            int_val = g_variant_get_int16(var);
            sprintf(data, "%ld", int_val);
        }
        else if (g_variant_type_equal(type, G_VARIANT_TYPE_UINT16)){
            int_val = g_variant_get_uint16(var);
            sprintf(data, "%ld", int_val);
        }
        else if (g_variant_type_equal(type, G_VARIANT_TYPE_INT32)){
            int_val = g_variant_get_int32(var);
            sprintf(data, "%ld", int_val);
        }
        else if (g_variant_type_equal(type, G_VARIANT_TYPE_UINT32)){
            int_val = g_variant_get_uint32(var);
            sprintf(data, "%ld", int_val);
        }
        else if (g_variant_type_equal(type, G_VARIANT_TYPE_INT64)){
            int_val = g_variant_get_int64(var);
            sprintf(data, "%ld", int_val);
        }
        else if (g_variant_type_equal(type, G_VARIANT_TYPE_UINT64)){
            int_val = g_variant_get_uint64(var);
            sprintf(data, "%ld", int_val);
        }
        else
            return NULL;
    }
    else if (g_variant_is_of_type(var, G_VARIANT_TYPE("s"))) {
        const char *str = g_variant_get_string(var, NULL);

        value_str = g_malloc( strlen(str) + 1 );
        if( value_str == NULL )
            return NULL;

        strcpy(value_str, str);
        return value_str;
    }
    else
        return NULL;

    value_str = g_malloc( strlen(data) + 1 );
    if( value_str == NULL )
        return NULL;

    strcpy(value_str, data);
    
    return value_str;
}

void atk_get_gvariant_free(char *str)
{
    if( str )
        g_free(str);
}


/**
 * @brief       获取变量类型
 * @retval      -1 : 获取失败
 *              1  : 数值
 *              2  : 字符串
 */
int atk_get_gvariant_type(atk_GVariant* var)
{
    if (g_variant_is_of_type(var, G_VARIANT_TYPE("d")))
        return 1;
    
    if (g_variant_is_of_type(var, G_VARIANT_TYPE("x")))
        return 1;
    
    if (g_variant_is_of_type(var, G_VARIANT_TYPE("s")))
        return 2;

    return -1;
}




/**
 * @brief        根据sample数据类型，新建一个值为 var 的 GVariant变量
 * @retval      
 */
static GVariant* set_GVariant_val(GVariant* sample, const char *var)
{
    if (g_variant_is_of_type(sample, G_VARIANT_TYPE("d")))
        return g_variant_new_double(atof(var));
    else if (g_variant_is_of_type(sample, G_VARIANT_TYPE("x")))
    {
        const GVariantType* const type = g_variant_get_type(sample);
        if (g_variant_type_equal(type, G_VARIANT_TYPE_BYTE))
            return g_variant_new_byte(atoi(var));
        else if (g_variant_type_equal(type, G_VARIANT_TYPE_INT16))
            return g_variant_new_int16(atoi(var));
        else if (g_variant_type_equal(type, G_VARIANT_TYPE_UINT16))
            return g_variant_new_uint16(atoi(var));
        else if (g_variant_type_equal(type, G_VARIANT_TYPE_INT32))
            return g_variant_new_int32(atol(var));
        else if (g_variant_type_equal(type, G_VARIANT_TYPE_UINT32))
            return g_variant_new_uint32(strtoul(var, NULL, 10));
        else if (g_variant_type_equal(type, G_VARIANT_TYPE_INT64))
            return g_variant_new_int64(strtoll(var, NULL, 10));
        else if (g_variant_type_equal(type, G_VARIANT_TYPE_UINT64))
            return g_variant_new_uint64(strtoull(var, NULL, 10));
    }
    else if (g_variant_is_of_type(sample, G_VARIANT_TYPE("s")))
        return g_variant_new_string(var);
    return NULL;
}


#ifdef _WIN32
    #include <windows.h>
    static int _decoder_setenv(const char *name, const char *value, int overwrite)
    {
        int errcode = 0;
        if(!overwrite) {
            size_t envsize = 0;
            errcode = getenv_s(&envsize, NULL, 0, name);
            if(errcode || envsize) return errcode;
        }
        return _putenv_s(name, value);
    }
#elif __APPLE__
static int _decoder_setenv(const char *name, const char *value, int overwrite)
{
    return setenv(name, value, overwrite);
}
#elif __ANDROID__

#elif __linux__
    static int _decoder_setenv(const char *name, const char *value, int overwrite)
    {
        return setenv(name, value, overwrite);
    }
#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown"
#endif





/*********************  srd.c **************************/
/* path支持中文路径，  path必须为UTF-8编码 */
int atk_decoder_init(const char *path)
{
    char new_path[1024]; 

    _decoder_setenv("PYTHONHOME",path, 1);

    sprintf(new_path, "%s%s", path, "/atk_python");
    _decoder_setenv("PYTHONPATH",new_path, 1);

    srd_info("PYTHONHOME : %s", getenv("PYTHONHOME")); 
    srd_info("PYTHONPATH : %s", getenv("PYTHONPATH")); 

    sprintf(new_path, "%s%s", path, "/decoders");

    return srd_init(new_path);
}

int atk_decoder_exit(void)
{
    return srd_exit();
}

/*******************************************************/





/******************** session.c ************************/
atk_session *atk_decoder_session_new(void)
{
    int ret;
    atk_session *sess;

    ret = srd_session_new((struct srd_session **)&sess);
    if( ret != SRD_OK )
        return NULL;

    return sess;
}

int atk_decoder_session_start(atk_session *sess)
{
    return srd_session_start((struct srd_session *)sess);
}

/**
 * @brief       设置采样率，单位：Hz
 * @retval      
 */
int atk_decoder_session_metadata_set_samplerate(atk_session *sess, uint64_t samplerate_hz)
{
    return srd_session_metadata_set( (struct srd_session *)sess, SRD_CONF_SAMPLERATE, g_variant_new_uint64( samplerate_hz ) );
}

/**
 * @brief       向会话发送数据
 * 
 *   Correct example (4096 samples total, 4 chunks @ 1024 samples each):
 *      srd_session_send(s, 0,    1024, inbuf);
 *      srd_session_send(s, 1024, 2048, inbuf);
 *      srd_session_send(s, 2048, 3072, inbuf);
 *      srd_session_send(s, 3072, 4096, inbuf);
 * 
 * @param abs_start_samplenum   样本起始采样位置：绝对位置
 * @param abs_end_samplenum     样本结束采样位置：绝对位置
 * @param inbuf                 采样数据，  inbuf[0] 存放通道0数据
 *                                         inbuf[1] 存放通道1数据
 *                                         inbuf[n] 存放通道n数据
 * 
 * @retval      
 */
int atk_decoder_session_send(atk_session *sess,
                            uint64_t abs_start_samplenum, uint64_t abs_end_samplenum,
                            struct atk_input_data *inbuf)
{
    return srd_session_send( (struct srd_session *)sess, abs_start_samplenum, abs_end_samplenum, 
                            (struct srd_input_data *)inbuf);
}

int atk_decoder_session_send_eof(atk_session *sess)
{
    return srd_session_send_eof((struct srd_session *)sess);
}

int atk_decoder_session_terminate_reset(atk_session *sess)
{
    return srd_session_terminate_reset((struct srd_session *)sess);
}

int atk_decoder_session_destroy(atk_session *sess)
{
    return srd_session_destroy((struct srd_session *)sess);
}

int atk_decoder_pd_output_callback_add(atk_session *sess,
                                       int output_type, atk_pd_output_callback cb, void *cb_data)
{
    return srd_pd_output_callback_add((struct srd_session *)sess, output_type, (srd_pd_output_callback)cb, cb_data);
}

/*******************************************************/


/******************** decoder.c ************************/
const atk_GSList *atk_decoder_list(void)
{
    return (atk_GSList *)srd_decoder_list();
}

struct atk_decoder *atk_decoder_get_by_id(const char *id)
{
    return (struct atk_decoder *)srd_decoder_get_by_id(id);
}

int atk_decoder_load(const char *name)
{
    return srd_decoder_load(name);
}

char *atk_decoder_doc_get(const struct atk_decoder *dec)
{
    return srd_decoder_doc_get((const struct srd_decoder *)dec);
}

int atk_decoder_unload(struct atk_decoder *dec)
{
    return srd_decoder_unload((struct srd_decoder *)dec);
}

int atk_decoder_load_all(void)
{
    return srd_decoder_load_all();
}

int atk_decoder_unload_all(void)
{
    return srd_decoder_unload_all();
}
/*******************************************************/







atk_GHashTable* atk_decoder_hashtable_create(void)
{
    return g_hash_table_new_full(g_str_hash, g_str_equal, g_free, (GDestroyNotify)g_variant_unref);
}

void atk_decoder_hashtable_destroy(atk_GHashTable *hashtable)
{
    if( hashtable == NULL )
        return;

    g_hash_table_destroy(hashtable);
}


/**
 * @brief       设置协议选项参数
 * 
 * @retval      SRD_OK      设置成功
 *              SRD_ERR     未设置成功
 */
int atk_decoder_hashtable_set_option(atk_GHashTable *hashtable, struct atk_decoder *decoder, const char *key, const char *val)
{
    GSList *l;
    struct srd_decoder_option *sdo;
    GVariant* temp;

    for (l = (GSList *)decoder->options; l; l = l->next) {
        
        sdo = l->data;
        if( strcmp(sdo->id, key) != 0 )
            continue;
        
        temp = set_GVariant_val(sdo->def, val);
        // g_variant_ref(temp);
        g_hash_table_insert(hashtable, (void*)g_strdup(key), temp);
    }

    if( l )
        return SRD_OK;
    
    return SRD_ERR;
}

/**
 * @brief       channels   or   option optional_channels
 * @retval      
 */
void atk_decoder_hashtable_set_channel(atk_GHashTable *hashtable, const char *key, int val)
{
    GVariant* temp = g_variant_new_int32(val);
    g_hash_table_insert(hashtable, (void*)g_strdup(key), temp);
}








int atk_decoder_inst_option_set(struct atk_decoder_inst *di,
        atk_GHashTable *options)
{
    return srd_inst_option_set((struct srd_decoder_inst *)di, options);
}

int atk_decoder_inst_channel_set_all(struct atk_decoder_inst *di,
        atk_GHashTable *channels)
{
    return srd_inst_channel_set_all((struct srd_decoder_inst *)di, channels);
}

struct atk_decoder_inst *atk_decoder_inst_new(atk_session *sess,
                        const char *id, atk_GHashTable *options)
{
    return (struct atk_decoder_inst *)srd_inst_new((struct srd_session *)sess, id, options);
}


int atk_decoder_inst_stack(atk_session *sess,
        struct atk_decoder_inst *di_bottom, struct atk_decoder_inst *di_top)
{
    return srd_inst_stack((struct srd_session *)sess, (struct srd_decoder_inst *)di_bottom, (struct srd_decoder_inst *)di_top);
}

struct atk_decoder_inst *atk_decoder_inst_find_by_id(atk_session *sess,
        const char *inst_id)
{
    return (struct atk_decoder_inst *)srd_inst_find_by_id((struct srd_session *)sess, inst_id);
}

int atk_decoder_inst_initial_pins_set_all(struct atk_decoder_inst *di,
        atk_GArray *initial_pins)
{
    return srd_inst_initial_pins_set_all((struct srd_decoder_inst *)di, initial_pins);
}

/*******************************************************/


/******************** log.c ****************************/
int atk_log_loglevel_set(int loglevel)
{
    return srd_log_loglevel_set(loglevel);
}

int atk_log_loglevel_get(void)
{
    return srd_log_loglevel_get();
}

int atk_log_callback_get(atk_log_callback *cb, void **cb_data)
{
    return srd_log_callback_get(cb, cb_data);
}

int atk_log_callback_set(atk_log_callback cb, void *cb_data)
{
    return srd_log_callback_set(cb, cb_data);
}

int atk_log_callback_set_default(void)
{
    return srd_log_callback_set_default();
}
/*******************************************************/



/******************** error.c **************************/
const char *atk_strerror(int error_code)
{
    return srd_strerror(error_code);
}
const char *atk_strerror_name(int error_code)
{
    return srd_strerror_name(error_code);
}
/*******************************************************/




/******************** version.c ***********************/
int atk_package_version_major_get(void)
{
    return srd_package_version_major_get();
}

int atk_package_version_minor_get(void)
{
    return srd_package_version_minor_get();
}

int atk_package_version_micro_get(void)
{
    return srd_package_version_micro_get();
}

const char *atk_package_version_string_get(void)
{
    return srd_package_version_string_get();
}

int atk_lib_version_current_get(void)
{
    return srd_lib_version_current_get();
}

int atk_lib_version_revision_get(void)
{
    return srd_lib_version_revision_get();
}

int atk_lib_version_age_get(void)
{
    return srd_lib_version_age_get();
}

const char *atk_version_string_get(void)
{
    return srd_lib_version_string_get();
}

atk_GSList *atk_buildinfo_libs_get(void)
{
    return (atk_GSList *)srd_buildinfo_libs_get();
}

char *atk_buildinfo_host_get(void)
{
    return srd_buildinfo_host_get();
}

/*******************************************************/





/*******************************************************/
char *atk_decoder_version_string_get(void)
{
    return ATK_DECODER_VERSION_STRING;
}

int atk_decoder_version_major_get(void)
{
    return ATK_DECODER_VERSION_MAJOR;
}

int atk_decoder_version_minor_get(void)
{
    return ATK_DECODER_VERSION_MINOR;
}


/*******************************************************/



#include <stdexcept>

#ifdef _WIN32
// Do nothing on Windows
#else
#include <dlfcn.h>
#endif

// This header is generated at build time. See CMakeLists.txt.
#include <vector>

#include "com_ladybugdb_Native.h"
#if __has_include("lbug.h")
#include "lbug.h"
#elif __has_include("c_api/lbug.h")
#include "c_api/lbug.h"
#else
#error "Public lbug header not found"
#endif
#include "c_api/helpers.h"
#include <format>
#include <jni.h>
#include <sstream>

using Exception = std::exception;
using NotImplementedException = std::runtime_error;

namespace {
constexpr auto JAVA_DECIMAL_PRECISION_LIMIT = 38;
} // namespace

#ifdef __ANDROID__
static jint JNI_VERSION = JNI_VERSION_1_6;
#else
static jint JNI_VERSION = JNI_VERSION_1_8;
#endif

// map
static jclass J_C_Map;
static jmethodID J_C_Map_M_entrySet;
// set
static jclass J_C_Set;
static jmethodID J_C_Set_M_iterator;
// iterator
static jclass J_C_Iterator;
static jmethodID J_C_Iterator_M_hasNext;
static jmethodID J_C_Iterator_M_next;
// Map$Entry
static jclass J_C_Map$Entry;
static jmethodID J_C_Map$Entry_M_getKey;
static jmethodID J_C_Map$Entry_M_getValue;
// Exception
static jclass J_C_Exception;
// QueryResult
static jclass J_C_QueryResult;
static jfieldID J_C_QueryResult_F_qr_ref;
static jfieldID J_C_QueryResult_F_isOwnedByCPP;
// PreparedStatement
static jclass J_C_PreparedStatement;
static jfieldID J_C_PreparedStatement_F_ps_ref;
// DataType
static jclass J_C_DataType;
static jfieldID J_C_DataType_F_dt_ref;
// QuerySummary
static jclass J_C_QuerySummary;
static jmethodID J_C_QuerySummary_M_ctor;
// FlatTuple
static jclass J_C_FlatTuple;
static jfieldID J_C_FlatTuple_F_ft_ref;
static jfieldID J_C_FlatTuple_F_isOwnedByCPP;
// Value
static jclass J_C_Value;
static jfieldID J_C_Value_F_v_ref;
static jfieldID J_C_Value_F_isOwnedByCPP;
// DataTypeID
static jclass J_C_DataTypeID;
static jfieldID J_C_DataTypeID_F_value;
// Boolean
static jclass J_C_Boolean;
static jmethodID J_C_Boolean_M_init;
static jmethodID J_C_Boolean_M_booleanValue;
// Long
static jclass J_C_Long;
static jmethodID J_C_Long_M_init;
static jmethodID J_C_Long_M_longValue;
// Integer
static jclass J_C_Integer;
static jmethodID J_C_Integer_M_init;
static jmethodID J_C_Integer_M_intValue;
// InternalID
static jclass J_C_InternalID;
static jmethodID J_C_InternalID_M_init;
static jfieldID J_C_InternalID_F_tableId;
static jfieldID J_C_InternalID_F_offset;
// Double
static jclass J_C_Double;
static jmethodID J_C_Double_M_init;
static jmethodID J_C_Double_M_doubleValue;
// BigDecimal
static jclass J_C_BigDecimal;
static jmethodID J_C_BigDecimal_M_init;
static jmethodID J_C_BigDecimal_M_toString;
static jmethodID J_C_BigDecimal_M_stripTrailingZeros;
static jmethodID J_C_BigDecimal_M_precision;
static jmethodID J_C_BigDecimal_M_scale;
// LocalDate
static jclass J_C_LocalDate;
static jmethodID J_C_LocalDate_M_ofEpochDay;
static jmethodID J_C_LocalDate_M_toEpochDay;
static jmethodID J_C_LocalDate_M_getEpochSecond;
static jmethodID J_C_LocalDate_M_getNano;

// Instant
static jclass J_C_Instant;
static jmethodID J_C_Instant_M_ofEpochSecond;
// Short
static jclass J_C_Short;
static jmethodID J_C_Short_M_init;
static jmethodID J_C_Short_M_shortValue;
// Byte
static jclass J_C_Byte;
static jmethodID J_C_Byte_M_init;
static jmethodID J_C_Byte_M_byteValue;
// BigInteger
static jclass J_C_BigInteger;
static jmethodID J_C_BigInteger_M_init;
static jmethodID J_C_BigInteger_M_longValue;
static jmethodID J_C_BigInteger_M_shiftRight;
// Float
static jclass J_C_Float;
static jmethodID J_C_Float_M_init;
static jmethodID J_C_Float_M_floatValue;
// Duration
static jclass J_C_Duration;
static jmethodID J_C_Duration_M_ofMillis;
static jmethodID J_C_Duration_M_toMillis;
// UUID
static jclass J_C_UUID;
static jmethodID J_C_UUID_M_init;
static jmethodID J_C_UUID_M_fromString;
static jmethodID J_C_UUID_M_getMostSignificantBits;
static jmethodID J_C_UUID_M_getLeastSignificantBits;
static jmethodID J_C_UUID_M_toString;
// Connection
static jclass J_C_Connection;
static jfieldID J_C_Connection_F_conn_ref;
// Database
static jclass J_C_Database;
static jfieldID J_C_Database_db_ref;
// String
static jclass J_C_String;
static jmethodID J_C_String_M_ctor;
static jmethodID J_C_String_M_getBytes;

static void throwJNIException(JNIEnv* env, const char* message) {
    jclass exClass = env->FindClass("java/lang/RuntimeException");
    if (exClass == nullptr) {
        return;
    }
    env->ThrowNew(exClass, message);
}

template<typename... Args>
static jobject callObjectMethodChecked(JNIEnv* env, jobject object, jmethodID method,
    Args... args) {
    auto result = env->CallObjectMethod(object, method, args...);
    if (env->ExceptionCheck()) {
        throw NotImplementedException("Java exception raised during JNI object call");
    }
    return result;
}

template<typename... Args>
static jobject callStaticObjectMethodChecked(JNIEnv* env, jclass objectClass, jmethodID method,
    Args... args) {
    auto result = env->CallStaticObjectMethod(objectClass, method, args...);
    if (env->ExceptionCheck()) {
        throw NotImplementedException("Java exception raised during JNI static object call");
    }
    return result;
}

template<typename... Args>
static jboolean callBooleanMethodChecked(JNIEnv* env, jobject object, jmethodID method,
    Args... args) {
    auto result = env->CallBooleanMethod(object, method, args...);
    if (env->ExceptionCheck()) {
        throw NotImplementedException("Java exception raised during JNI boolean call");
    }
    return result;
}

template<typename... Args>
static jbyte callByteMethodChecked(JNIEnv* env, jobject object, jmethodID method, Args... args) {
    auto result = env->CallByteMethod(object, method, args...);
    if (env->ExceptionCheck()) {
        throw NotImplementedException("Java exception raised during JNI byte call");
    }
    return result;
}

template<typename... Args>
static jshort callShortMethodChecked(JNIEnv* env, jobject object, jmethodID method, Args... args) {
    auto result = env->CallShortMethod(object, method, args...);
    if (env->ExceptionCheck()) {
        throw NotImplementedException("Java exception raised during JNI short call");
    }
    return result;
}

template<typename... Args>
static jint callIntMethodChecked(JNIEnv* env, jobject object, jmethodID method, Args... args) {
    auto result = env->CallIntMethod(object, method, args...);
    if (env->ExceptionCheck()) {
        throw NotImplementedException("Java exception raised during JNI int call");
    }
    return result;
}

template<typename... Args>
static jlong callLongMethodChecked(JNIEnv* env, jobject object, jmethodID method, Args... args) {
    auto result = env->CallLongMethod(object, method, args...);
    if (env->ExceptionCheck()) {
        throw NotImplementedException("Java exception raised during JNI long call");
    }
    return result;
}

template<typename... Args>
static jfloat callFloatMethodChecked(JNIEnv* env, jobject object, jmethodID method, Args... args) {
    auto result = env->CallFloatMethod(object, method, args...);
    if (env->ExceptionCheck()) {
        throw NotImplementedException("Java exception raised during JNI float call");
    }
    return result;
}

template<typename... Args>
static jdouble callDoubleMethodChecked(JNIEnv* env, jobject object, jmethodID method,
    Args... args) {
    auto result = env->CallDoubleMethod(object, method, args...);
    if (env->ExceptionCheck()) {
        throw NotImplementedException("Java exception raised during JNI double call");
    }
    return result;
}

static std::string jstringToUtf8String(JNIEnv* env, jstring value) {
    if (value == nullptr) {
        return "";
    }
    auto* charset = env->NewStringUTF("UTF-8");
    if (charset == nullptr) {
        throw NotImplementedException("Failed to create UTF-8 charset string");
    }
    jbyteArray byteArr =
        (jbyteArray)callObjectMethodChecked(env, value, J_C_String_M_getBytes, charset);
    env->DeleteLocalRef(charset);
    size_t length = env->GetArrayLength(byteArr);
    jbyte* bytes = env->GetByteArrayElements(byteArr, nullptr);
    std::string result((char*)bytes, length);
    env->ReleaseByteArrayElements(byteArr, bytes, JNI_ABORT);
    env->DeleteLocalRef(byteArr);
    return result;
}

static jstring utf8StringToJstring(JNIEnv* env, std::string str) {
    jbyteArray byteArr = env->NewByteArray(str.size());
    env->SetByteArrayRegion(byteArr, 0, str.size(), reinterpret_cast<const jbyte*>(str.data()));
    jstring ret =
        (jstring)env->NewObject(J_C_String, J_C_String_M_ctor, byteArr, env->NewStringUTF("UTF-8"));
    return ret;
}

jobject createJavaObject(JNIEnv* env, void* memAddress, jclass javaClass, jfieldID refID) {
    try {
        auto address = reinterpret_cast<uint64_t>(memAddress);
        auto ref = static_cast<jlong>(address);

        jobject newObject = env->AllocObject(javaClass);

        env->SetLongField(newObject, refID, ref);
        return newObject;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

lbug_database* getDatabase(JNIEnv* env, jobject thisDB) {
    try {
        jlong fieldValue = env->GetLongField(thisDB, J_C_Database_db_ref);

        uint64_t address = static_cast<uint64_t>(fieldValue);
        auto* db = reinterpret_cast<lbug_database*>(address);
        return db;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return nullptr;
}

lbug_connection* getConnection(JNIEnv* env, jobject thisConn) {

    try {
        jlong fieldValue = env->GetLongField(thisConn, J_C_Connection_F_conn_ref);

        uint64_t address = static_cast<uint64_t>(fieldValue);
        auto* conn = reinterpret_cast<lbug_connection*>(address);
        return conn;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return nullptr;
}

lbug_prepared_statement* getPreparedStatement(JNIEnv* env, jobject thisPS) {
    try {
        jlong fieldValue = env->GetLongField(thisPS, J_C_PreparedStatement_F_ps_ref);

        uint64_t address = static_cast<uint64_t>(fieldValue);
        auto* ps = reinterpret_cast<lbug_prepared_statement*>(address);
        return ps;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return nullptr;
}

lbug_query_result* getQueryResult(JNIEnv* env, jobject thisQR) {
    try {
        jlong fieldValue = env->GetLongField(thisQR, J_C_QueryResult_F_qr_ref);

        uint64_t address = static_cast<uint64_t>(fieldValue);
        auto* qr = reinterpret_cast<lbug_query_result*>(address);
        return qr;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return nullptr;
}

lbug_flat_tuple* getFlatTuple(JNIEnv* env, jobject thisFT) {
    try {
        jlong fieldValue = env->GetLongField(thisFT, J_C_FlatTuple_F_ft_ref);

        uint64_t address = static_cast<uint64_t>(fieldValue);
        return reinterpret_cast<lbug_flat_tuple*>(address);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return nullptr;
}

lbug_logical_type* getDataType(JNIEnv* env, jobject thisDT) {
    try {
        jlong fieldValue = env->GetLongField(thisDT, J_C_DataType_F_dt_ref);

        uint64_t address = static_cast<uint64_t>(fieldValue);
        return reinterpret_cast<lbug_logical_type*>(address);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return nullptr;
}

lbug_value* getValue(JNIEnv* env, jobject thisValue) {
    try {
        jlong fieldValue = env->GetLongField(thisValue, J_C_Value_F_v_ref);

        uint64_t address = static_cast<uint64_t>(fieldValue);
        return reinterpret_cast<lbug_value*>(address);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return nullptr;
}

lbug_internal_id_t getInternalID(JNIEnv* env, jobject id) {
    try {
        auto table_id = static_cast<uint64_t>(env->GetLongField(id, J_C_InternalID_F_tableId));
        auto offset = static_cast<uint64_t>(env->GetLongField(id, J_C_InternalID_F_offset));
        return {.table_id = table_id, .offset = offset};
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return {};
}

void throwIfError(lbug_state state, const char* message) {
    if (state != LbugSuccess) {
        throw NotImplementedException(message);
    }
}

std::string takeOwnedCString(char* str) {
    if (str == nullptr) {
        return "";
    }
    std::string result(str);
    lbug_destroy_string(str);
    return result;
}

jstring takeOwnedCStringAsJString(JNIEnv* env, char* str) {
    if (str == nullptr) {
        return nullptr;
    }
    auto result = utf8StringToJstring(env, str);
    lbug_destroy_string(str);
    return result;
}

std::string dataTypeToString(lbug_data_type_id dataType) {
    switch (dataType) {
    case LBUG_ANY: return "ANY";
    case LBUG_NODE: return "NODE";
    case LBUG_REL: return "REL";
    case LBUG_RECURSIVE_REL: return "RECURSIVE_REL";
    case LBUG_SERIAL: return "SERIAL";
    case LBUG_BOOL: return "BOOL";
    case LBUG_INT64: return "INT64";
    case LBUG_INT32: return "INT32";
    case LBUG_INT16: return "INT16";
    case LBUG_INT8: return "INT8";
    case LBUG_UINT64: return "UINT64";
    case LBUG_UINT32: return "UINT32";
    case LBUG_UINT16: return "UINT16";
    case LBUG_UINT8: return "UINT8";
    case LBUG_INT128: return "INT128";
    case LBUG_DOUBLE: return "DOUBLE";
    case LBUG_FLOAT: return "FLOAT";
    case LBUG_DATE: return "DATE";
    case LBUG_TIMESTAMP: return "TIMESTAMP";
    case LBUG_TIMESTAMP_SEC: return "TIMESTAMP_SEC";
    case LBUG_TIMESTAMP_MS: return "TIMESTAMP_MS";
    case LBUG_TIMESTAMP_NS: return "TIMESTAMP_NS";
    case LBUG_TIMESTAMP_TZ: return "TIMESTAMP_TZ";
    case LBUG_INTERVAL: return "INTERVAL";
    case LBUG_DECIMAL: return "DECIMAL";
    case LBUG_INTERNAL_ID: return "INTERNAL_ID";
    case LBUG_STRING: return "STRING";
    case LBUG_BLOB: return "BLOB";
    case LBUG_LIST: return "LIST";
    case LBUG_ARRAY: return "ARRAY";
    case LBUG_STRUCT: return "STRUCT";
    case LBUG_MAP: return "MAP";
    case LBUG_UNION: return "UNION";
    case LBUG_UUID: return "UUID";
    default: return "ANY";
    }
}

void bindJavaParamsToPreparedStatement(JNIEnv* env, lbug_prepared_statement* preparedStatement,
    jobject javaMap) {
    jobject set = callObjectMethodChecked(env, javaMap, J_C_Map_M_entrySet);
    jobject iter = callObjectMethodChecked(env, set, J_C_Set_M_iterator);
    while (callBooleanMethodChecked(env, iter, J_C_Iterator_M_hasNext)) {
        jobject entry = callObjectMethodChecked(env, iter, J_C_Iterator_M_next);
        jstring key = (jstring)callObjectMethodChecked(env, entry, J_C_Map$Entry_M_getKey);
        jobject value = callObjectMethodChecked(env, entry, J_C_Map$Entry_M_getValue);
        std::string keyStr = jstringToUtf8String(env, key);
        auto* clonedValue = lbug_value_clone(getValue(env, value));
        auto state =
            lbug_prepared_statement_bind_value(preparedStatement, keyStr.c_str(), clonedValue);
        lbug_value_destroy(clonedValue);
        throwIfError(state, "Failed to bind prepared statement parameter");
        env->DeleteLocalRef(entry);
        env->DeleteLocalRef(key);
        env->DeleteLocalRef(value);
    }
}

/**
 * All Database native functions
 */
//     protected static native void lbugNativeReloadLibrary(String libPath);
JNIEXPORT void JNICALL Java_com_ladybugdb_Native_lbugNativeReloadLibrary(JNIEnv* env, jclass,
    jstring libPath) {
    try {
#ifdef _WIN32
// Do nothing on Windows
#else
        const char* path = env->GetStringUTFChars(libPath, JNI_FALSE);
        void* handle = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);
        env->ReleaseStringUTFChars(libPath, path);
        if (handle == nullptr) {
            auto error = dlerror(); // NOLINT(concurrency-mt-unsafe): load can only be executed in
                                    // single thread.
            env->ThrowNew(J_C_Exception, error);
        }
#endif
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

JNIEXPORT jlong JNICALL Java_com_ladybugdb_Native_lbugDatabaseInit(JNIEnv* env, jclass,
    jstring databasePath, jlong bufferPoolSize, jboolean enableCompression, jboolean readOnly,
    jlong maxDbSize, jboolean autoCheckpoint, jlong checkpointThreshold,
    jboolean throwOnWalReplayFailure, jboolean enableChecksums) {
    try {
        const char* path = env->GetStringUTFChars(databasePath, JNI_FALSE);
        auto systemConfig = lbug_default_system_config();
        if (bufferPoolSize != 0) {
            systemConfig.buffer_pool_size = static_cast<uint64_t>(bufferPoolSize);
        }
        systemConfig.enable_compression = enableCompression;
        systemConfig.read_only = readOnly;
        if (maxDbSize != 0) {
            auto unsignedMaxDbSize = static_cast<uint64_t>(maxDbSize);
            if ((unsignedMaxDbSize & (unsignedMaxDbSize - 1)) != 0) {
                env->ReleaseStringUTFChars(databasePath, path);
                env->ThrowNew(J_C_Exception,
                    "Buffer manager exception: The given max db size should be a power of 2.");
                return 0;
            }
            systemConfig.max_db_size = static_cast<uint64_t>(maxDbSize);
        }
        systemConfig.auto_checkpoint = autoCheckpoint;
        if (checkpointThreshold >= 0) {
            systemConfig.checkpoint_threshold = static_cast<uint64_t>(checkpointThreshold);
        }
        systemConfig.throw_on_wal_replay_failure = throwOnWalReplayFailure;
        systemConfig.enable_checksums = enableChecksums;
        try {
            auto* db = new lbug_database();
            auto state = lbug_database_init(path, systemConfig, db);
            env->ReleaseStringUTFChars(databasePath, path);
            if (state != LbugSuccess) {
                delete db;
                if (auto* errorMessage = takeLastCAPIErrorMessage()) {
                    env->ThrowNew(J_C_Exception, errorMessage);
                    free(errorMessage);
                } else {
                    env->ThrowNew(J_C_Exception, "Failed to initialize database");
                }
                return 0;
            }
            return static_cast<jlong>(reinterpret_cast<uint64_t>(db));
        } catch (const Exception& e) {
            env->ReleaseStringUTFChars(databasePath, path);
            env->ThrowNew(J_C_Exception, e.what());
        }
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return 0;
}

JNIEXPORT void JNICALL Java_com_ladybugdb_Native_lbugDatabaseDestroy(JNIEnv* env, jclass,
    jobject thisDB) {
    try {
        auto* db = getDatabase(env, thisDB);
        lbug_database_destroy(db);
        delete db;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

/**
 * All Connection native functions
 */

JNIEXPORT jlong JNICALL Java_com_ladybugdb_Native_lbugConnectionInit(JNIEnv* env, jclass,
    jobject db) {

    try {
        auto* conn_db = getDatabase(env, db);
        auto* conn = new lbug_connection();
        throwIfError(lbug_connection_init(conn_db, conn), "Failed to initialize connection");
        return static_cast<jlong>(reinterpret_cast<uint64_t>(conn));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    }
    return 0;
}

JNIEXPORT void JNICALL Java_com_ladybugdb_Native_lbugConnectionDestroy(JNIEnv* env, jclass,
    jobject thisConn) {
    try {
        auto* conn = getConnection(env, thisConn);
        lbug_connection_destroy(conn);
        delete conn;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

JNIEXPORT void JNICALL Java_com_ladybugdb_Native_lbugConnectionSetMaxNumThreadForExec(JNIEnv* env,
    jclass, jobject thisConn, jlong numThreads) {
    try {
        auto* conn = getConnection(env, thisConn);
        auto threads = static_cast<uint64_t>(numThreads);
        throwIfError(lbug_connection_set_max_num_thread_for_exec(conn, threads),
            "Failed to set max threads for execution");
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

JNIEXPORT jlong JNICALL Java_com_ladybugdb_Native_lbugConnectionGetMaxNumThreadForExec(JNIEnv* env,
    jclass, jobject thisConn) {
    try {
        auto* conn = getConnection(env, thisConn);
        uint64_t threads = 0;
        throwIfError(lbug_connection_get_max_num_thread_for_exec(conn, &threads),
            "Failed to get max threads for execution");
        return static_cast<jlong>(threads);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return 0;
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugConnectionQuery(JNIEnv* env, jclass,
    jobject thisConn, jstring query) {
    try {
        auto* conn = getConnection(env, thisConn);
        std::string cppQuery = jstringToUtf8String(env, query);
        auto* queryResult = new lbug_query_result();
        if (lbug_connection_query(conn, cppQuery.c_str(), queryResult) != LbugSuccess) {
            delete queryResult;
            if (auto* errorMessage = takeLastCAPIErrorMessage()) {
                throwJNIException(env, errorMessage);
                free(errorMessage);
                return jobject();
            }
            throwJNIException(env, "Failed to execute query");
            return jobject();
        }
        return createJavaObject(env, queryResult, J_C_QueryResult, J_C_QueryResult_F_qr_ref);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugConnectionPrepare(JNIEnv* env, jclass,
    jobject thisConn, jstring query) {
    try {
        auto* conn = getConnection(env, thisConn);
        std::string cppQuery = jstringToUtf8String(env, query);
        auto* preparedStatement = new lbug_prepared_statement();
        if (lbug_connection_prepare(conn, cppQuery.c_str(), preparedStatement) != LbugSuccess) {
            delete preparedStatement;
            if (auto* errorMessage = takeLastCAPIErrorMessage()) {
                throwJNIException(env, errorMessage);
                free(errorMessage);
                return jobject();
            }
            throwJNIException(env, "Failed to prepare statement");
            return jobject();
        }
        jobject ret = createJavaObject(env, preparedStatement, J_C_PreparedStatement,
            J_C_PreparedStatement_F_ps_ref);
        return ret;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugConnectionExecute(JNIEnv* env, jclass,
    jobject thisConn, jobject preStm, jobject paramMap) {
    try {
        auto* conn = getConnection(env, thisConn);
        auto* ps = getPreparedStatement(env, preStm);
        bindJavaParamsToPreparedStatement(env, ps, paramMap);
        auto* queryResult = new lbug_query_result();
        if (lbug_connection_execute(conn, ps, queryResult) != LbugSuccess) {
            delete queryResult;
            if (auto* errorMessage = takeLastCAPIErrorMessage()) {
                throwJNIException(env, errorMessage);
                free(errorMessage);
                return jobject();
            }
            throwJNIException(env, "Failed to execute prepared statement");
            return jobject();
        }
        jobject ret = createJavaObject(env, queryResult, J_C_QueryResult, J_C_QueryResult_F_qr_ref);
        return ret;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT void JNICALL Java_com_ladybugdb_Native_lbugConnectionInterrupt(JNIEnv* env, jclass,
    jobject thisConn) {
    try {
        auto* conn = getConnection(env, thisConn);
        lbug_connection_interrupt(conn);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

JNIEXPORT void JNICALL Java_com_ladybugdb_Native_lbugConnectionSetQueryTimeout(JNIEnv* env, jclass,
    jobject thisConn, jlong timeoutInMs) {
    try {
        auto* conn = getConnection(env, thisConn);
        auto timeout = static_cast<uint64_t>(timeoutInMs);
        throwIfError(lbug_connection_set_query_timeout(conn, timeout),
            "Failed to set query timeout");
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

/**
 * All PreparedStatement native functions
 */

JNIEXPORT void JNICALL Java_com_ladybugdb_Native_lbugPreparedStatementDestroy(JNIEnv* env, jclass,
    jobject thisPS) {
    try {
        auto* ps = getPreparedStatement(env, thisPS);
        lbug_prepared_statement_destroy(ps);
        delete ps;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

JNIEXPORT jboolean JNICALL Java_com_ladybugdb_Native_lbugPreparedStatementIsSuccess(JNIEnv* env,
    jclass, jobject thisPS) {
    try {
        auto* ps = getPreparedStatement(env, thisPS);
        return static_cast<jboolean>(lbug_prepared_statement_is_success(ps));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jboolean();
}

JNIEXPORT jstring JNICALL Java_com_ladybugdb_Native_lbugPreparedStatementGetErrorMessage(
    JNIEnv* env, jclass, jobject thisPS) {
    try {
        auto* ps = getPreparedStatement(env, thisPS);
        return takeOwnedCStringAsJString(env, lbug_prepared_statement_get_error_message(ps));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jstring();
}

/**
 * All QueryResult native functions
 */

JNIEXPORT void JNICALL Java_com_ladybugdb_Native_lbugQueryResultDestroy(JNIEnv* env, jclass,
    jobject thisQR) {
    try {
        auto* qr = getQueryResult(env, thisQR);
        lbug_query_result_destroy(qr);
        delete qr;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

JNIEXPORT jboolean JNICALL Java_com_ladybugdb_Native_lbugQueryResultIsSuccess(JNIEnv* env, jclass,
    jobject thisQR) {
    try {
        auto* qr = getQueryResult(env, thisQR);
        return static_cast<jboolean>(lbug_query_result_is_success(qr));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jboolean();
}

JNIEXPORT jstring JNICALL Java_com_ladybugdb_Native_lbugQueryResultGetErrorMessage(JNIEnv* env,
    jclass, jobject thisQR) {
    try {
        auto* qr = getQueryResult(env, thisQR);
        return takeOwnedCStringAsJString(env, lbug_query_result_get_error_message(qr));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jstring();
}

JNIEXPORT jlong JNICALL Java_com_ladybugdb_Native_lbugQueryResultGetNumColumns(JNIEnv* env, jclass,
    jobject thisQR) {
    try {
        auto* qr = getQueryResult(env, thisQR);
        return static_cast<jlong>(lbug_query_result_get_num_columns(qr));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jlong();
}

JNIEXPORT jstring JNICALL Java_com_ladybugdb_Native_lbugQueryResultGetColumnName(JNIEnv* env,
    jclass, jobject thisQR, jlong index) {
    try {
        auto* qr = getQueryResult(env, thisQR);
        char* columnName = nullptr;
        if (lbug_query_result_get_column_name(qr, static_cast<uint64_t>(index), &columnName) !=
            LbugSuccess) {
            return nullptr;
        }
        return takeOwnedCStringAsJString(env, columnName);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jstring();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugQueryResultGetColumnDataType(JNIEnv* env,
    jclass, jobject thisQR, jlong index) {
    try {
        auto* qr = getQueryResult(env, thisQR);
        auto* dataType = new lbug_logical_type();
        if (lbug_query_result_get_column_data_type(qr, static_cast<uint64_t>(index), dataType) !=
            LbugSuccess) {
            delete dataType;
            return nullptr;
        }
        return createJavaObject(env, dataType, J_C_DataType, J_C_DataType_F_dt_ref);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jlong JNICALL Java_com_ladybugdb_Native_lbugQueryResultGetNumTuples(JNIEnv* env, jclass,
    jobject thisQR) {
    try {
        auto* qr = getQueryResult(env, thisQR);
        return static_cast<jlong>(lbug_query_result_get_num_tuples(qr));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jlong();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugQueryResultGetQuerySummary(JNIEnv* env,
    jclass, jobject thisQR) {
    try {
        auto* qr = getQueryResult(env, thisQR);
        lbug_query_summary querySummary;
        throwIfError(lbug_query_result_get_query_summary(qr, &querySummary),
            "Failed to fetch query summary");
        jdouble cmpTime = static_cast<jdouble>(lbug_query_summary_get_compiling_time(&querySummary));
        jdouble exeTime = static_cast<jdouble>(lbug_query_summary_get_execution_time(&querySummary));
        jobject newQSObject =
            env->NewObject(J_C_QuerySummary, J_C_QuerySummary_M_ctor, cmpTime, exeTime);
        lbug_query_summary_destroy(&querySummary);
        return newQSObject;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jboolean JNICALL Java_com_ladybugdb_Native_lbugQueryResultHasNext(JNIEnv* env, jclass,
    jobject thisQR) {
    try {
        auto* qr = getQueryResult(env, thisQR);
        return static_cast<jboolean>(lbug_query_result_has_next(qr));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jboolean();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugQueryResultGetNext(JNIEnv* env, jclass,
    jobject thisQR) {
    try {
        auto* qr = getQueryResult(env, thisQR);
        auto* flatTuple = new lbug_flat_tuple();
        if (lbug_query_result_get_next(qr, flatTuple) != LbugSuccess) {
            delete flatTuple;
            if (auto* errorMessage = takeLastCAPIErrorMessage()) {
                throwJNIException(env, errorMessage);
                free(errorMessage);
                return jobject();
            }
            throwJNIException(env, "Failed to fetch next tuple");
            return jobject();
        }
        jobject ret = createJavaObject(env, flatTuple, J_C_FlatTuple, J_C_FlatTuple_F_ft_ref);
        env->SetBooleanField(ret, J_C_FlatTuple_F_isOwnedByCPP, static_cast<jboolean>(true));
        return ret;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jboolean JNICALL Java_com_ladybugdb_Native_lbugQueryResultHasNextQueryResult(JNIEnv* env,
    jclass, jobject thisQR) {
    try {
        auto* qr = getQueryResult(env, thisQR);
        return static_cast<jboolean>(lbug_query_result_has_next_query_result(qr));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jboolean();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugQueryResultGetNextQueryResult(JNIEnv* env,
    jclass, jobject thisQR) {
    try {
        auto* qr = getQueryResult(env, thisQR);
        auto* queryResult = new lbug_query_result();
        if (lbug_query_result_get_next_query_result(qr, queryResult) != LbugSuccess) {
            delete queryResult;
            return nullptr;
        }
        jobject ret = createJavaObject(env, queryResult, J_C_QueryResult, J_C_QueryResult_F_qr_ref);
        env->SetBooleanField(ret, J_C_QueryResult_F_isOwnedByCPP, static_cast<jboolean>(true));
        return ret;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jstring JNICALL Java_com_ladybugdb_Native_lbugQueryResultToString(JNIEnv* env, jclass,
    jobject thisQR) {
    try {
        auto* qr = getQueryResult(env, thisQR);
        return takeOwnedCStringAsJString(env, lbug_query_result_to_string(qr));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jstring();
}

JNIEXPORT void JNICALL Java_com_ladybugdb_Native_lbugQueryResultResetIterator(JNIEnv* env, jclass,
    jobject thisQR) {
    try {
        auto* qr = getQueryResult(env, thisQR);
        lbug_query_result_reset_iterator(qr);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

/**
 * All FlatTuple native functions
 */

JNIEXPORT void JNICALL Java_com_ladybugdb_Native_lbugFlatTupleDestroy(JNIEnv* env, jclass,
    jobject thisFT) {
    try {
        auto* flatTuple = getFlatTuple(env, thisFT);
        lbug_flat_tuple_destroy(flatTuple);
        delete flatTuple;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugFlatTupleGetValue(JNIEnv* env, jclass,
    jobject thisFT, jlong index) {
    try {
        auto* ft = getFlatTuple(env, thisFT);
        auto* value = new lbug_value();
        if (lbug_flat_tuple_get_value(ft, static_cast<uint64_t>(index), value) != LbugSuccess) {
            delete value;
            return nullptr;
        }
        jobject v = createJavaObject(env, value, J_C_Value, J_C_Value_F_v_ref);
        env->SetBooleanField(v, J_C_Value_F_isOwnedByCPP, static_cast<jboolean>(true));

        return v;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jstring JNICALL Java_com_ladybugdb_Native_lbugFlatTupleToString(JNIEnv* env, jclass,
    jobject thisFT) {
    try {
        auto* ft = getFlatTuple(env, thisFT);
        return takeOwnedCStringAsJString(env, lbug_flat_tuple_to_string(ft));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jstring();
}

/**
 * All DataType native functions
 */

JNIEXPORT jlong JNICALL Java_com_ladybugdb_Native_lbugDataTypeCreate(JNIEnv* env, jclass,
    jobject id, jobject childType, jlong numElementsInArray) {
    try {
        jint fieldValue = env->GetIntField(id, J_C_DataTypeID_F_value);
        auto* dataType = new lbug_logical_type();
        auto* childTypePtr = childType == nullptr ? nullptr : getDataType(env, childType);
        lbug_data_type_create(static_cast<lbug_data_type_id>(fieldValue), childTypePtr,
            static_cast<uint64_t>(numElementsInArray), dataType);
        return static_cast<jlong>(reinterpret_cast<uint64_t>(dataType));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jlong();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugDataTypeClone(JNIEnv* env, jclass,
    jobject thisDT) {
    try {
        auto* oldDT = getDataType(env, thisDT);
        auto* newDT = new lbug_logical_type();
        lbug_data_type_clone(oldDT, newDT);
        jobject dt = createJavaObject(env, newDT, J_C_DataType, J_C_DataType_F_dt_ref);
        return dt;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT void JNICALL Java_com_ladybugdb_Native_lbugDataTypeDestroy(JNIEnv* env, jclass,
    jobject thisDT) {
    try {
        auto* dt = getDataType(env, thisDT);
        lbug_data_type_destroy(dt);
        delete dt;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

JNIEXPORT jboolean JNICALL Java_com_ladybugdb_Native_lbugDataTypeEquals(JNIEnv* env, jclass,
    jobject dt1, jobject dt2) {
    try {
        auto* cppdt1 = getDataType(env, dt1);
        auto* cppdt2 = getDataType(env, dt2);
        return static_cast<jboolean>(lbug_data_type_equals(cppdt1, cppdt2));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jboolean();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugDataTypeGetId(JNIEnv* env, jclass,
    jobject thisDT) {
    try {
        auto* dt = getDataType(env, thisDT);
        std::string idStr = dataTypeToString(lbug_data_type_get_id(dt));
        jfieldID idField =
            env->GetStaticFieldID(J_C_DataTypeID, idStr.c_str(), "Lcom/ladybugdb/DataTypeID;");
        jobject id = env->GetStaticObjectField(J_C_DataTypeID, idField);
        return id;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugDataTypeGetChildType(JNIEnv* env, jclass,
    jobject thisDT) {
    try {
        auto* parentType = getDataType(env, thisDT);
        auto* childType = new lbug_logical_type();
        if (lbug_data_type_get_child_type(parentType, childType) != LbugSuccess) {
            delete childType;
            return nullptr;
        }
        return createJavaObject(env, childType, J_C_DataType, J_C_DataType_F_dt_ref);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jlong JNICALL Java_com_ladybugdb_Native_lbugDataTypeGetNumElementsInArray(JNIEnv* env,
    jclass, jobject thisDT) {
    try {
        auto* dt = getDataType(env, thisDT);
        uint64_t result = 0;
        if (lbug_data_type_get_num_elements_in_array(dt, &result) != LbugSuccess) {
            return 0;
        }
        return static_cast<jlong>(result);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jlong();
}

/**
 * All Value native functions
 */

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugValueCreateNull(JNIEnv* env, jclass) {
    try {
        auto* v = lbug_value_create_null();
        jobject ret = createJavaObject(env, v, J_C_Value, J_C_Value_F_v_ref);
        return ret;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugValueCreateNullWithDataType(JNIEnv* env,
    jclass, jobject dataType) {
    try {
        auto* dt = getDataType(env, dataType);
        auto* v = lbug_value_create_null_with_data_type(dt);
        jobject ret = createJavaObject(env, v, J_C_Value, J_C_Value_F_v_ref);
        return ret;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jboolean JNICALL Java_com_ladybugdb_Native_lbugValueIsNull(JNIEnv* env, jclass,
    jobject thisV) {
    try {
        auto* v = getValue(env, thisV);
        return static_cast<jboolean>(lbug_value_is_null(v));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jboolean();
}

JNIEXPORT void JNICALL Java_com_ladybugdb_Native_lbugValueSetNull(JNIEnv* env, jclass,
    jobject thisV, jboolean isNull) {
    try {
        auto* v = getValue(env, thisV);
        lbug_value_set_null(v, static_cast<bool>(isNull));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugValueCreateDefault(JNIEnv* env, jclass,
    jobject dataType) {
    try {
        auto* dt = getDataType(env, dataType);
        auto* v = lbug_value_create_default(dt);
        jobject ret = createJavaObject(env, v, J_C_Value, J_C_Value_F_v_ref);
        return ret;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jlong JNICALL Java_com_ladybugdb_Native_lbugValueCreateValue(JNIEnv* env, jclass,
    jobject val) {
    try {
        lbug_value* v = nullptr;
        if (env->IsInstanceOf(val, J_C_Boolean)) {
            jboolean value = callBooleanMethodChecked(env, val, J_C_Boolean_M_booleanValue);
            v = lbug_value_create_bool(static_cast<bool>(value));
        } else if (env->IsInstanceOf(val, J_C_Byte)) {
            jbyte value = callByteMethodChecked(env, val, J_C_Byte_M_byteValue);
            v = lbug_value_create_int8(static_cast<int8_t>(value));
        } else if (env->IsInstanceOf(val, J_C_Short)) {
            jshort value = callShortMethodChecked(env, val, J_C_Short_M_shortValue);
            v = lbug_value_create_int16(static_cast<int16_t>(value));
        } else if (env->IsInstanceOf(val, J_C_Integer)) {
            jint value = callIntMethodChecked(env, val, J_C_Integer_M_intValue);
            v = lbug_value_create_int32(static_cast<int32_t>(value));
        } else if (env->IsInstanceOf(val, J_C_Long)) {
            jlong value = callLongMethodChecked(env, val, J_C_Long_M_longValue);
            v = lbug_value_create_int64(static_cast<int64_t>(value));
        } else if (env->IsInstanceOf(val, J_C_BigInteger)) {
            int64_t lower =
                static_cast<int64_t>(callLongMethodChecked(env, val, J_C_BigInteger_M_longValue));
            jobject shifted = callObjectMethodChecked(env, val, J_C_BigInteger_M_shiftRight, 64);
            int64_t upper =
                static_cast<int64_t>(callLongMethodChecked(env, shifted,
                    J_C_BigInteger_M_longValue));
            v = lbug_value_create_int128({.low = static_cast<uint64_t>(lower), .high = upper});
        } else if (env->IsInstanceOf(val, J_C_Float)) {
            jfloat value = callFloatMethodChecked(env, val, J_C_Float_M_floatValue);
            v = lbug_value_create_float(static_cast<float>(value));
        } else if (env->IsInstanceOf(val, J_C_Double)) {
            jdouble value = callDoubleMethodChecked(env, val, J_C_Double_M_doubleValue);
            v = lbug_value_create_double(static_cast<double>(value));
        } else if (env->IsInstanceOf(val, J_C_BigDecimal)) {
            jobject normalized = callObjectMethodChecked(env, val,
                J_C_BigDecimal_M_stripTrailingZeros);
            jstring value = static_cast<jstring>(callObjectMethodChecked(env, normalized,
                J_C_BigDecimal_M_toString));
            std::string str = jstringToUtf8String(env, value);
            auto precision =
                static_cast<int32_t>(callIntMethodChecked(env, normalized,
                    J_C_BigDecimal_M_precision));
            auto scale = static_cast<int32_t>(callIntMethodChecked(env, normalized,
                J_C_BigDecimal_M_scale));
            if (precision > JAVA_DECIMAL_PRECISION_LIMIT) {
                throw NotImplementedException(
                    std::format("Decimal precision cannot be greater than {}"
                                "Note: positive exponents contribute to precision",
                        JAVA_DECIMAL_PRECISION_LIMIT));
            }
            v = lbug_value_create_decimal(str.c_str(), static_cast<uint32_t>(precision),
                static_cast<uint32_t>(scale));
        } else if (env->IsInstanceOf(val, J_C_String)) {
            jstring value = static_cast<jstring>(val);
            std::string str = jstringToUtf8String(env, value);
            v = lbug_value_create_string(str.c_str());
        } else if (env->IsInstanceOf(val, J_C_InternalID)) {
            v = lbug_value_create_internal_id(getInternalID(env, val));
        } else if (env->IsInstanceOf(val, J_C_UUID)) {
            jstring uuid =
                static_cast<jstring>(callObjectMethodChecked(env, val, J_C_UUID_M_toString));
            std::string uuidString = jstringToUtf8String(env, uuid);
            v = lbug_value_create_uuid(uuidString.c_str());
        } else if (env->IsInstanceOf(val, J_C_LocalDate)) {
            int64_t days =
                static_cast<int64_t>(callLongMethodChecked(env, val, J_C_LocalDate_M_toEpochDay));
            v = lbug_value_create_date({.days = static_cast<int32_t>(days)});
        } else if (env->IsInstanceOf(val, J_C_Instant)) {
            // TODO: Need to review this for overflow
            int64_t seconds =
                static_cast<int64_t>(callLongMethodChecked(env, val,
                    J_C_LocalDate_M_getEpochSecond));
            int64_t nano = static_cast<int64_t>(callLongMethodChecked(env, val,
                J_C_LocalDate_M_getNano));

            int64_t micro = (seconds * 1000000L) + (nano / 1000L);
            v = lbug_value_create_timestamp({.value = micro});
        } else if (env->IsInstanceOf(val, J_C_Duration)) {
            auto milis = callLongMethodChecked(env, val, J_C_Duration_M_toMillis);
            v = lbug_value_create_interval({.months = 0, .days = 0, .micros = milis * 1000L});
        } else {
            throwJNIException(env, "Type of value is not supported in value_create_value");
            return -1;
        }
        return static_cast<jlong>(reinterpret_cast<uint64_t>(v));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jlong();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugValueClone(JNIEnv* env, jclass,
    jobject thisValue) {
    try {
        auto* v = getValue(env, thisValue);
        auto* copy = lbug_value_clone(v);
        return createJavaObject(env, copy, J_C_Value, J_C_Value_F_v_ref);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT void JNICALL Java_com_ladybugdb_Native_lbugValueCopy(JNIEnv* env, jclass,
    jobject thisValue, jobject otherValue) {
    try {
        auto* thisV = getValue(env, thisValue);
        auto* otherV = getValue(env, otherValue);
        lbug_value_copy(thisV, otherV);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

JNIEXPORT void JNICALL Java_com_ladybugdb_Native_lbugValueDestroy(JNIEnv* env, jclass,
    jobject thisValue) {
    try {
        auto* v = getValue(env, thisValue);
        lbug_value_destroy(v);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugCreateList___3Lcom_ladybugdb_Value_2(
    JNIEnv* env, jclass, jobjectArray listValues) {
    try {
        jsize len = env->GetArrayLength(listValues);
        if (len == 0) {
            return nullptr;
        }
        std::vector<lbug_value*> children;
        for (jsize i = 0; i < len; ++i) {
            children.push_back(getValue(env, env->GetObjectArrayElement(listValues, i)));
        }
        lbug_value* listValue = nullptr;
        throwIfError(lbug_value_create_list(static_cast<uint64_t>(children.size()), children.data(),
            &listValue), "Failed to create list value");
        return createJavaObject(env, listValue, J_C_Value, J_C_Value_F_v_ref);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugCreateList__Lcom_ladybugdb_DataType_2J(
    JNIEnv* env, jclass, jobject dataType, jlong numElements) {
    try {
        auto* logicalType = getDataType(env, dataType);
        std::vector<lbug_value*> children;
        for (jlong i = 0; i < numElements; ++i) {
            children.push_back(lbug_value_create_default(logicalType));
        }
        lbug_value* listValue = nullptr;
        throwIfError(lbug_value_create_list(static_cast<uint64_t>(children.size()), children.data(),
            &listValue), "Failed to create list value");
        for (auto* child : children) {
            lbug_value_destroy(child);
        }
        return createJavaObject(env, listValue, J_C_Value, J_C_Value_F_v_ref);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jlong JNICALL Java_com_ladybugdb_Native_lbugValueGetListSize(JNIEnv* env, jclass,
    jobject thisValue) {
    try {
        auto* v = getValue(env, thisValue);
        uint64_t size = 0;
        throwIfError(lbug_value_get_list_size(v, &size), "Failed to get list size");
        return static_cast<jlong>(size);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jlong();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugValueGetListElement(JNIEnv* env, jclass,
    jobject thisValue, jlong index) {
    try {
        auto* v = getValue(env, thisValue);
        auto* elementValue = new lbug_value();
        if (lbug_value_get_list_element(v, static_cast<uint64_t>(index), elementValue) !=
            LbugSuccess) {
            delete elementValue;
            return nullptr;
        }
        jobject element = createJavaObject(env, elementValue, J_C_Value, J_C_Value_F_v_ref);
        env->SetBooleanField(element, J_C_Value_F_isOwnedByCPP, static_cast<jboolean>(true));
        return element;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugValueGetDataType(JNIEnv* env, jclass,
    jobject thisValue) {
    try {
        auto* v = getValue(env, thisValue);
        auto* dt = new lbug_logical_type();
        lbug_value_get_data_type(v, dt);
        return createJavaObject(env, dt, J_C_DataType, J_C_DataType_F_dt_ref);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugValueGetValue(JNIEnv* env, jclass,
    jobject thisValue) {
    try {
        auto* v = getValue(env, thisValue);
        lbug_logical_type dataType;
        lbug_value_get_data_type(v, &dataType);
        auto logicalTypeId = lbug_data_type_get_id(&dataType);

        switch (logicalTypeId) {
        case LBUG_BOOL: {
            bool value = false;
            throwIfError(lbug_value_get_bool(v, &value), "Failed to read BOOL value");
            jboolean val = static_cast<jboolean>(value);
            jobject ret = env->NewObject(J_C_Boolean, J_C_Boolean_M_init, val);
            return ret;
        }
        case LBUG_INT64:
        case LBUG_SERIAL: {
            int64_t value = 0;
            throwIfError(lbug_value_get_int64(v, &value), "Failed to read INT64 value");
            jlong val = static_cast<jlong>(value);
            jobject ret = env->NewObject(J_C_Long, J_C_Long_M_init, val);
            return ret;
        }
        case LBUG_INT32: {
            int32_t value = 0;
            throwIfError(lbug_value_get_int32(v, &value), "Failed to read INT32 value");
            jint val = static_cast<jint>(value);
            jobject ret = env->NewObject(J_C_Integer, J_C_Integer_M_init, val);
            return ret;
        }
        case LBUG_INT16: {
            int16_t value = 0;
            throwIfError(lbug_value_get_int16(v, &value), "Failed to read INT16 value");
            jshort val = static_cast<jshort>(value);
            jobject ret = env->NewObject(J_C_Short, J_C_Short_M_init, val);
            return ret;
        }
        case LBUG_INT8: {
            int8_t value = 0;
            throwIfError(lbug_value_get_int8(v, &value), "Failed to read INT8 value");
            jbyte val = static_cast<jbyte>(value);
            jobject ret = env->NewObject(J_C_Byte, J_C_Byte_M_init, val);
            return ret;
        }
        case LBUG_UINT64: {
            uint64_t value = 0;
            throwIfError(lbug_value_get_uint64(v, &value), "Failed to read UINT64 value");
            std::ostringstream valueStream;
            valueStream << value;
            std::string valueStr = valueStream.str();
            jstring val = env->NewStringUTF(valueStr.c_str());
            jobject ret = env->NewObject(J_C_BigInteger, J_C_BigInteger_M_init, val);
            return ret;
        }
        case LBUG_UINT32: {
            uint32_t value = 0;
            throwIfError(lbug_value_get_uint32(v, &value), "Failed to read UINT32 value");
            jlong val = static_cast<jlong>(value);
            jobject ret = env->NewObject(J_C_Long, J_C_Long_M_init, val);
            return ret;
        }
        case LBUG_UINT16: {
            uint16_t value = 0;
            throwIfError(lbug_value_get_uint16(v, &value), "Failed to read UINT16 value");
            jint val = static_cast<jint>(value);
            jobject ret = env->NewObject(J_C_Integer, J_C_Integer_M_init, val);
            return ret;
        }
        case LBUG_UINT8: {
            uint8_t value = 0;
            throwIfError(lbug_value_get_uint8(v, &value), "Failed to read UINT8 value");
            jshort val = static_cast<jshort>(value);
            jobject ret = env->NewObject(J_C_Short, J_C_Short_M_init, val);
            return ret;
        }
        case LBUG_INT128: {
            lbug_int128_t value{};
            throwIfError(lbug_value_get_int128(v, &value), "Failed to read INT128 value");
            char* str = nullptr;
            throwIfError(lbug_int128_t_to_string(value, &str), "Failed to format INT128 value");
            jstring val = env->NewStringUTF(str);
            lbug_destroy_string(str);
            jobject ret = env->NewObject(J_C_BigInteger, J_C_BigInteger_M_init, val);
            return ret;
        }
        case LBUG_DOUBLE: {
            double value = 0;
            throwIfError(lbug_value_get_double(v, &value), "Failed to read DOUBLE value");
            jdouble val = static_cast<jdouble>(value);
            jobject ret = env->NewObject(J_C_Double, J_C_Double_M_init, val);
            return ret;
        }
        case LBUG_DECIMAL: {
            char* str = nullptr;
            throwIfError(lbug_value_get_decimal_as_string(v, &str), "Failed to read DECIMAL value");
            jstring val = env->NewStringUTF(str);
            lbug_destroy_string(str);
            jobject ret = env->NewObject(J_C_BigDecimal, J_C_BigDecimal_M_init, val);
            return ret;
        }
        case LBUG_FLOAT: {
            float value = 0;
            throwIfError(lbug_value_get_float(v, &value), "Failed to read FLOAT value");
            jfloat val = static_cast<jfloat>(value);
            jobject ret = env->NewObject(J_C_Float, J_C_Float_M_init, val);
            return ret;
        }
        case LBUG_DATE: {
            lbug_date_t date{};
            throwIfError(lbug_value_get_date(v, &date), "Failed to read DATE value");
            return callStaticObjectMethodChecked(
                env,
                J_C_LocalDate, J_C_LocalDate_M_ofEpochDay, static_cast<jlong>(date.days));
        }
        case LBUG_TIMESTAMP_TZ: {
            lbug_timestamp_tz_t ts{};
            throwIfError(lbug_value_get_timestamp_tz(v, &ts), "Failed to read TIMESTAMP_TZ value");
            int64_t seconds = ts.value / 1000000L;
            int64_t nano = ts.value % 1000000L * 1000L;
            return callStaticObjectMethodChecked(
                env,
                J_C_Instant, J_C_Instant_M_ofEpochSecond, seconds, nano);
        }
        case LBUG_TIMESTAMP: {
            lbug_timestamp_t ts{};
            throwIfError(lbug_value_get_timestamp(v, &ts), "Failed to read TIMESTAMP value");
            int64_t seconds = ts.value / 1000000L;
            int64_t nano = ts.value % 1000000L * 1000L;
            return callStaticObjectMethodChecked(
                env,
                J_C_Instant, J_C_Instant_M_ofEpochSecond, seconds, nano);
        }
        case LBUG_TIMESTAMP_NS: {
            lbug_timestamp_ns_t ts{};
            throwIfError(lbug_value_get_timestamp_ns(v, &ts), "Failed to read TIMESTAMP_NS value");
            int64_t seconds = ts.value / 1000000000L;
            int64_t nano = ts.value % 1000000000L;
            return callStaticObjectMethodChecked(
                env,
                J_C_Instant, J_C_Instant_M_ofEpochSecond, seconds, nano);
        }
        case LBUG_TIMESTAMP_MS: {
            lbug_timestamp_ms_t ts{};
            throwIfError(lbug_value_get_timestamp_ms(v, &ts), "Failed to read TIMESTAMP_MS value");
            int64_t seconds = ts.value / 1000L;
            int64_t nano = ts.value % 1000L * 1000000L;
            return callStaticObjectMethodChecked(
                env,
                J_C_Instant, J_C_Instant_M_ofEpochSecond, seconds, nano);
        }
        case LBUG_TIMESTAMP_SEC: {
            lbug_timestamp_sec_t ts{};
            throwIfError(lbug_value_get_timestamp_sec(v, &ts),
                "Failed to read TIMESTAMP_SEC value");
            return callStaticObjectMethodChecked(
                env,
                J_C_Instant, J_C_Instant_M_ofEpochSecond, ts.value, 0);
        }
        case LBUG_INTERVAL: {
            lbug_interval_t interval{};
            throwIfError(lbug_value_get_interval(v, &interval), "Failed to read INTERVAL value");
            double seconds = 0;
            lbug_interval_to_difftime(interval, &seconds);
            auto millis = static_cast<jlong>(seconds * 1000);
            return callStaticObjectMethodChecked(env, J_C_Duration, J_C_Duration_M_ofMillis,
                millis);
        }
        case LBUG_INTERNAL_ID: {
            lbug_internal_id_t iid{};
            throwIfError(lbug_value_get_internal_id(v, &iid), "Failed to read INTERNAL_ID value");
            return env->NewObject(
                J_C_InternalID, J_C_InternalID_M_init, iid.table_id, iid.offset);
        }
        case LBUG_UUID: {
            char* str = nullptr;
            throwIfError(lbug_value_get_uuid(v, &str), "Failed to read UUID value");
            jstring uuid = env->NewStringUTF(str);
            lbug_destroy_string(str);
            return callStaticObjectMethodChecked(env, J_C_UUID, J_C_UUID_M_fromString, uuid);
        }
        case LBUG_STRING: {
            char* str = nullptr;
            throwIfError(lbug_value_get_string(v, &str), "Failed to read STRING value");
            return takeOwnedCStringAsJString(env, str);
        }
        case LBUG_BLOB: {
            uint8_t* blob = nullptr;
            uint64_t length = 0;
            throwIfError(lbug_value_get_blob(v, &blob, &length), "Failed to read BLOB value");
            auto ret = env->NewByteArray(length);
            env->SetByteArrayRegion(ret, 0, length, reinterpret_cast<jbyte*>(blob));
            lbug_destroy_blob(blob);
            return ret;
        }
        default:
            throwJNIException(env, "Type of value is not supported in value_get_value");
            return nullptr;
        }
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return nullptr;
}

JNIEXPORT jstring JNICALL Java_com_ladybugdb_Native_lbugValueToString(JNIEnv* env, jclass,
    jobject thisValue) {
    try {
        auto* v = getValue(env, thisValue);
        return takeOwnedCStringAsJString(env, lbug_value_to_string(v));
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jstring();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugNodeValGetId(JNIEnv* env, jclass,
    jobject thisNV) {
    try {
        auto* nv = getValue(env, thisNV);
        lbug_value idValue;
        if (lbug_node_val_get_id_val(nv, &idValue) != LbugSuccess) {
            return nullptr;
        }
        lbug_internal_id_t id{};
        if (lbug_value_get_internal_id(&idValue, &id) != LbugSuccess) {
            return nullptr;
        }
        return env->NewObject(J_C_InternalID, J_C_InternalID_M_init, id.table_id, id.offset);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jstring JNICALL Java_com_ladybugdb_Native_lbugNodeValGetLabelName(JNIEnv* env, jclass,
    jobject thisNV) {
    try {
        auto* nv = getValue(env, thisNV);
        lbug_value labelValue;
        if (lbug_node_val_get_label_val(nv, &labelValue) != LbugSuccess) {
            return nullptr;
        }
        char* label = nullptr;
        if (lbug_value_get_string(&labelValue, &label) != LbugSuccess) {
            return nullptr;
        }
        return takeOwnedCStringAsJString(env, label);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jstring();
}

JNIEXPORT jlong JNICALL Java_com_ladybugdb_Native_lbugNodeValGetPropertySize(JNIEnv* env, jclass,
    jobject thisNV) {
    try {
        auto* nv = getValue(env, thisNV);
        uint64_t size = 0;
        throwIfError(
            lbug_node_val_get_property_size(nv, &size), "Failed to get node property size");
        return static_cast<jlong>(size);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jlong();
}

JNIEXPORT jstring JNICALL Java_com_ladybugdb_Native_lbugNodeValGetPropertyNameAt(JNIEnv* env,
    jclass, jobject thisNV, jlong index) {
    try {
        auto* nv = getValue(env, thisNV);
        char* propertyName = nullptr;
        if (lbug_node_val_get_property_name_at(nv, static_cast<uint64_t>(index), &propertyName) !=
            LbugSuccess) {
            return nullptr;
        }
        return takeOwnedCStringAsJString(env, propertyName);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jstring();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugNodeValGetPropertyValueAt(JNIEnv* env,
    jclass, jobject thisNV, jlong index) {
    try {
        auto* nv = getValue(env, thisNV);
        auto* propertyValue = new lbug_value();
        if (lbug_node_val_get_property_value_at(
                nv, static_cast<uint64_t>(index), propertyValue) != LbugSuccess) {
            delete propertyValue;
            return nullptr;
        }
        jobject ret = createJavaObject(env, propertyValue, J_C_Value, J_C_Value_F_v_ref);
        env->SetBooleanField(ret, J_C_Value_F_isOwnedByCPP, static_cast<jboolean>(true));
        return ret;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jstring JNICALL Java_com_ladybugdb_Native_lbugNodeValToString(JNIEnv* env, jclass,
    jobject thisNV) {
    try {
        auto* nv = getValue(env, thisNV);
        char* result = nullptr;
        throwIfError(lbug_node_val_to_string(nv, &result), "Failed to convert node to string");
        return takeOwnedCStringAsJString(env, result);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jstring();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugRelValGetId(JNIEnv* env, jclass,
    jobject thisRV) {
    try {
        auto* rv = getValue(env, thisRV);
        lbug_value idValue;
        if (lbug_rel_val_get_id_val(rv, &idValue) != LbugSuccess) {
            return nullptr;
        }
        lbug_internal_id_t id{};
        if (lbug_value_get_internal_id(&idValue, &id) != LbugSuccess) {
            return nullptr;
        }
        return env->NewObject(J_C_InternalID, J_C_InternalID_M_init, id.table_id, id.offset);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugRelValGetSrcId(JNIEnv* env, jclass,
    jobject thisRV) {
    try {
        auto* rv = getValue(env, thisRV);
        lbug_value idValue;
        if (lbug_rel_val_get_src_id_val(rv, &idValue) != LbugSuccess) {
            return nullptr;
        }
        lbug_internal_id_t id{};
        if (lbug_value_get_internal_id(&idValue, &id) != LbugSuccess) {
            return nullptr;
        }
        return env->NewObject(J_C_InternalID, J_C_InternalID_M_init, id.table_id, id.offset);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugRelValGetDstId(JNIEnv* env, jclass,
    jobject thisRV) {
    try {
        auto* rv = getValue(env, thisRV);
        lbug_value idValue;
        if (lbug_rel_val_get_dst_id_val(rv, &idValue) != LbugSuccess) {
            return nullptr;
        }
        lbug_internal_id_t id{};
        if (lbug_value_get_internal_id(&idValue, &id) != LbugSuccess) {
            return nullptr;
        }
        return env->NewObject(J_C_InternalID, J_C_InternalID_M_init, id.table_id, id.offset);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jstring JNICALL Java_com_ladybugdb_Native_lbugRelValGetLabelName(JNIEnv* env, jclass,
    jobject thisRV) {
    try {
        auto* rv = getValue(env, thisRV);
        lbug_value labelValue;
        if (lbug_rel_val_get_label_val(rv, &labelValue) != LbugSuccess) {
            return nullptr;
        }
        char* label = nullptr;
        if (lbug_value_get_string(&labelValue, &label) != LbugSuccess) {
            return nullptr;
        }
        return takeOwnedCStringAsJString(env, label);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jstring();
}

JNIEXPORT jlong JNICALL Java_com_ladybugdb_Native_lbugRelValGetPropertySize(JNIEnv* env, jclass,
    jobject thisRV) {
    try {
        auto* rv = getValue(env, thisRV);
        uint64_t size = 0;
        throwIfError(
            lbug_rel_val_get_property_size(rv, &size), "Failed to get rel property size");
        return static_cast<jlong>(size);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jlong();
}

JNIEXPORT jstring JNICALL Java_com_ladybugdb_Native_lbugRelValGetPropertyNameAt(JNIEnv* env, jclass,
    jobject thisRV, jlong index) {
    try {
        auto* rv = getValue(env, thisRV);
        char* name = nullptr;
        if (lbug_rel_val_get_property_name_at(rv, static_cast<uint64_t>(index), &name) !=
            LbugSuccess) {
            return nullptr;
        }
        return takeOwnedCStringAsJString(env, name);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jstring();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugRelValGetPropertyValueAt(JNIEnv* env,
    jclass, jobject thisRV, jlong index) {
    try {
        auto* rv = getValue(env, thisRV);
        auto* value = new lbug_value();
        if (lbug_rel_val_get_property_value_at(rv, static_cast<uint64_t>(index), value) !=
            LbugSuccess) {
            delete value;
            return nullptr;
        }
        jobject ret = createJavaObject(env, value, J_C_Value, J_C_Value_F_v_ref);
        env->SetBooleanField(ret, J_C_Value_F_isOwnedByCPP, static_cast<jboolean>(true));
        return ret;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jstring JNICALL Java_com_ladybugdb_Native_lbugRelValToString(JNIEnv* env, jclass,
    jobject thisRV) {
    try {
        auto* rv = getValue(env, thisRV);
        char* result = nullptr;
        throwIfError(lbug_rel_val_to_string(rv, &result), "Failed to convert rel to string");
        return takeOwnedCStringAsJString(env, result);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jstring();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugCreateMap(JNIEnv* env, jclass,
    jobjectArray keys, jobjectArray values) {
    try {
        jsize len = env->GetArrayLength(keys);
        if (env->GetArrayLength(values) != len || len <= 0) {
            return nullptr;
        }
        std::vector<lbug_value*> keyValues;
        std::vector<lbug_value*> mappedValues;
        for (jsize i = 0; i < len; ++i) {
            keyValues.push_back(getValue(env, env->GetObjectArrayElement(keys, i)));
            mappedValues.push_back(getValue(env, env->GetObjectArrayElement(values, i)));
        }
        lbug_value* mapValue = nullptr;
        if (lbug_value_create_map(static_cast<uint64_t>(len), keyValues.data(), mappedValues.data(),
                &mapValue) != LbugSuccess) {
            return nullptr;
        }
        return createJavaObject(env, mapValue, J_C_Value, J_C_Value_F_v_ref);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jobject JNICALL Java_com_ladybugdb_Native_lbugCreateStruct(JNIEnv* env, jclass,
    jobjectArray fieldNames, jobjectArray fieldValues) {
    try {
        jsize len = env->GetArrayLength(fieldNames);
        if (env->GetArrayLength(fieldValues) != len || len <= 0) {
            return nullptr;
        }
        std::vector<std::string> fieldNamesCpp;
        std::vector<const char*> fieldNamePtrs;
        std::vector<lbug_value*> valuesCpp;
        for (jsize i = 0; i < len; ++i) {
            fieldNamesCpp.push_back(jstringToUtf8String(env,
                reinterpret_cast<jstring>(env->GetObjectArrayElement(fieldNames, i))));
            valuesCpp.push_back(getValue(env, env->GetObjectArrayElement(fieldValues, i)));
        }
        for (const auto& name : fieldNamesCpp) {
            fieldNamePtrs.push_back(name.c_str());
        }
        lbug_value* structValue = nullptr;
        if (lbug_value_create_struct(static_cast<uint64_t>(len), fieldNamePtrs.data(),
                valuesCpp.data(), &structValue) != LbugSuccess) {
            return nullptr;
        }
        return createJavaObject(env, structValue, J_C_Value, J_C_Value_F_v_ref);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jobject();
}

JNIEXPORT jstring JNICALL Java_com_ladybugdb_Native_lbugValueGetStructFieldName(JNIEnv* env, jclass,
    jobject thisSV, jlong index) {
    try {
        auto* sv = getValue(env, thisSV);
        char* name = nullptr;
        if (index < 0 ||
            lbug_value_get_struct_field_name(sv, static_cast<uint64_t>(index), &name) !=
                LbugSuccess) {
            return nullptr;
        }
        return takeOwnedCStringAsJString(env, name);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jstring();
}

JNIEXPORT jlong JNICALL Java_com_ladybugdb_Native_lbugValueGetStructIndex(JNIEnv* env, jclass,
    jobject thisSV, jstring fieldName) {
    try {
        auto* sv = getValue(env, thisSV);
        std::string field_name_str = jstringToUtf8String(env, fieldName);
        uint64_t index = 0;
        if (lbug_value_get_struct_field_index(sv, field_name_str.c_str(), &index) != LbugSuccess) {
            return -1;
        }
        return static_cast<jlong>(index);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jlong();
}

JNIEXPORT jstring JNICALL Java_com_ladybugdb_Native_lbugGetVersion(JNIEnv* env, jclass) {
    try {
        return takeOwnedCStringAsJString(env, lbug_get_version());

    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jstring();
}

JNIEXPORT jlong JNICALL Java_com_ladybugdb_Native_lbugGetStorageVersion(JNIEnv* env, jclass) {
    try {
        return static_cast<jlong>(lbug_get_storage_version());

    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jlong();
}

void createGlobalClassRef(JNIEnv* env, jclass& globalClassRef, const char* className) {
    try {
        jclass tempLocalClassRef = env->FindClass(className);
        globalClassRef = (jclass)env->NewGlobalRef(tempLocalClassRef);
        env->DeleteLocalRef(tempLocalClassRef);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

void initGlobalClassRef(JNIEnv* env) {
    try {
        createGlobalClassRef(env, J_C_Map, "java/util/Map");

        createGlobalClassRef(env, J_C_Set, "java/util/Set");

        createGlobalClassRef(env, J_C_Iterator, "java/util/Iterator");

        createGlobalClassRef(env, J_C_Map$Entry, "java/util/Map$Entry");

        createGlobalClassRef(env, J_C_Exception, "java/lang/Exception");

        createGlobalClassRef(env, J_C_QueryResult, "com/ladybugdb/QueryResult");

        createGlobalClassRef(env, J_C_PreparedStatement, "com/ladybugdb/PreparedStatement");

        createGlobalClassRef(env, J_C_DataType, "com/ladybugdb/DataType");

        createGlobalClassRef(env, J_C_QuerySummary, "com/ladybugdb/QuerySummary");

        createGlobalClassRef(env, J_C_FlatTuple, "com/ladybugdb/FlatTuple");

        createGlobalClassRef(env, J_C_Value, "com/ladybugdb/Value");

        createGlobalClassRef(env, J_C_DataTypeID, "com/ladybugdb/DataTypeID");

        createGlobalClassRef(env, J_C_Boolean, "java/lang/Boolean");

        createGlobalClassRef(env, J_C_Long, "java/lang/Long");

        createGlobalClassRef(env, J_C_Integer, "java/lang/Integer");

        createGlobalClassRef(env, J_C_InternalID, "com/ladybugdb/InternalID");

        createGlobalClassRef(env, J_C_Double, "java/lang/Double");

        createGlobalClassRef(env, J_C_BigDecimal, "java/math/BigDecimal");

        createGlobalClassRef(env, J_C_LocalDate, "java/time/LocalDate");

        createGlobalClassRef(env, J_C_Instant, "java/time/Instant");

        createGlobalClassRef(env, J_C_Short, "java/lang/Short");

        createGlobalClassRef(env, J_C_Byte, "java/lang/Byte");

        createGlobalClassRef(env, J_C_BigInteger, "java/math/BigInteger");

        createGlobalClassRef(env, J_C_Float, "java/lang/Float");

        createGlobalClassRef(env, J_C_Duration, "java/time/Duration");

        createGlobalClassRef(env, J_C_UUID, "java/util/UUID");

        createGlobalClassRef(env, J_C_Connection, "com/ladybugdb/Connection");

        createGlobalClassRef(env, J_C_Database, "com/ladybugdb/Database");

        createGlobalClassRef(env, J_C_String, "java/lang/String");
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

void initGlobalMethodRef(JNIEnv* env) {
    try {
        J_C_Map_M_entrySet = env->GetMethodID(J_C_Map, "entrySet", "()Ljava/util/Set;");

        J_C_Set_M_iterator = env->GetMethodID(J_C_Set, "iterator", "()Ljava/util/Iterator;");

        J_C_Iterator_M_hasNext = env->GetMethodID(J_C_Iterator, "hasNext", "()Z");
        J_C_Iterator_M_next = env->GetMethodID(J_C_Iterator, "next", "()Ljava/lang/Object;");

        J_C_Map$Entry_M_getKey = env->GetMethodID(J_C_Map$Entry, "getKey", "()Ljava/lang/Object;");
        J_C_Map$Entry_M_getValue =
            env->GetMethodID(J_C_Map$Entry, "getValue", "()Ljava/lang/Object;");

        J_C_QuerySummary_M_ctor = env->GetMethodID(J_C_QuerySummary, "<init>", "(DD)V");

        J_C_Boolean_M_init = env->GetMethodID(J_C_Boolean, "<init>", "(Z)V");

        J_C_Long_M_init = env->GetMethodID(J_C_Long, "<init>", "(J)V");

        J_C_Integer_M_init = env->GetMethodID(J_C_Integer, "<init>", "(I)V");

        J_C_InternalID_M_init = env->GetMethodID(J_C_InternalID, "<init>", "(JJ)V");

        J_C_Double_M_init = env->GetMethodID(J_C_Double, "<init>", "(D)V");

        J_C_BigDecimal_M_init = env->GetMethodID(J_C_BigDecimal, "<init>", "(Ljava/lang/String;)V");

        J_C_LocalDate_M_ofEpochDay =
            env->GetStaticMethodID(J_C_LocalDate, "ofEpochDay", "(J)Ljava/time/LocalDate;");
        J_C_LocalDate_M_toEpochDay = env->GetMethodID(J_C_LocalDate, "toEpochDay", "()J");
        J_C_LocalDate_M_getEpochSecond = env->GetMethodID(J_C_Instant, "getEpochSecond", "()J");
        J_C_LocalDate_M_getNano = env->GetMethodID(J_C_Instant, "getNano", "()I");

        J_C_Instant_M_ofEpochSecond =
            env->GetStaticMethodID(J_C_Instant, "ofEpochSecond", "(JJ)Ljava/time/Instant;");

        J_C_Short_M_init = env->GetMethodID(J_C_Short, "<init>", "(S)V");

        J_C_Float_M_init = env->GetMethodID(J_C_Float, "<init>", "(F)V");

        J_C_Duration_M_ofMillis =
            env->GetStaticMethodID(J_C_Duration, "ofMillis", "(J)Ljava/time/Duration;");
        J_C_Duration_M_toMillis = env->GetMethodID(J_C_Duration, "toMillis", "()J");

        J_C_Byte_M_init = env->GetMethodID(J_C_Byte, "<init>", "(B)V");

        J_C_BigInteger_M_init = env->GetMethodID(J_C_BigInteger, "<init>", "(Ljava/lang/String;)V");

        J_C_BigInteger_M_longValue = env->GetMethodID(J_C_BigInteger, "longValue", "()J");

        J_C_BigInteger_M_shiftRight =
            env->GetMethodID(J_C_BigInteger, "shiftRight", "(I)Ljava/math/BigInteger;");

        J_C_UUID_M_init = env->GetMethodID(J_C_UUID, "<init>", "(JJ)V");
        J_C_UUID_M_fromString =
            env->GetStaticMethodID(J_C_UUID, "fromString", "(Ljava/lang/String;)Ljava/util/UUID;");

        J_C_UUID_M_getMostSignificantBits =
            env->GetMethodID(J_C_UUID, "getMostSignificantBits", "()J");

        J_C_UUID_M_getLeastSignificantBits =
            env->GetMethodID(J_C_UUID, "getLeastSignificantBits", "()J");

        J_C_UUID_M_toString = env->GetMethodID(J_C_UUID, "toString", "()Ljava/lang/String;");

        J_C_Boolean_M_booleanValue = env->GetMethodID(J_C_Boolean, "booleanValue", "()Z");

        J_C_Byte_M_byteValue = env->GetMethodID(J_C_Byte, "byteValue", "()B");

        J_C_Short_M_shortValue = env->GetMethodID(J_C_Short, "shortValue", "()S");

        J_C_Integer_M_intValue = env->GetMethodID(J_C_Integer, "intValue", "()I");

        J_C_Long_M_longValue = env->GetMethodID(J_C_Long, "longValue", "()J");

        J_C_Float_M_floatValue = env->GetMethodID(J_C_Float, "floatValue", "()F");

        J_C_Double_M_doubleValue = env->GetMethodID(J_C_Double, "doubleValue", "()D");

        J_C_BigDecimal_M_toString =
            env->GetMethodID(J_C_BigDecimal, "toString", "()Ljava/lang/String;");

        J_C_BigDecimal_M_stripTrailingZeros =
            env->GetMethodID(J_C_BigDecimal, "stripTrailingZeros", "()Ljava/math/BigDecimal;");

        J_C_BigDecimal_M_precision = env->GetMethodID(J_C_BigDecimal, "precision", "()I");

        J_C_BigDecimal_M_scale = env->GetMethodID(J_C_BigDecimal, "scale", "()I");

        J_C_String_M_ctor = env->GetMethodID(J_C_String, "<init>", "([BLjava/lang/String;)V");

        J_C_String_M_getBytes = env->GetMethodID(J_C_String, "getBytes", "(Ljava/lang/String;)[B");
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

void initGlobalFieldRef(JNIEnv* env) {
    try {
        J_C_QueryResult_F_qr_ref = env->GetFieldID(J_C_QueryResult, "qr_ref", "J");
        J_C_QueryResult_F_isOwnedByCPP = env->GetFieldID(J_C_QueryResult, "isOwnedByCPP", "Z");

        J_C_PreparedStatement_F_ps_ref = env->GetFieldID(J_C_PreparedStatement, "ps_ref", "J");

        J_C_DataType_F_dt_ref = env->GetFieldID(J_C_DataType, "dt_ref", "J");

        J_C_FlatTuple_F_ft_ref = env->GetFieldID(J_C_FlatTuple, "ft_ref", "J");
        J_C_FlatTuple_F_isOwnedByCPP = env->GetFieldID(J_C_FlatTuple, "isOwnedByCPP", "Z");

        J_C_Value_F_v_ref = env->GetFieldID(J_C_Value, "v_ref", "J");
        J_C_Value_F_isOwnedByCPP = env->GetFieldID(J_C_Value, "isOwnedByCPP", "Z");

        J_C_DataTypeID_F_value = env->GetFieldID(J_C_DataTypeID, "value", "I");

        J_C_InternalID_F_tableId = env->GetFieldID(J_C_InternalID, "tableId", "J");

        J_C_InternalID_F_offset = env->GetFieldID(J_C_InternalID, "offset", "J");

        J_C_Connection_F_conn_ref = env->GetFieldID(J_C_Connection, "conn_ref", "J");

        J_C_Database_db_ref = env->GetFieldID(J_C_Database, "db_ref", "J");
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/) {
    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION) != JNI_OK) {
        return JNI_ERR;
    }
    try {
        initGlobalClassRef(env);
        initGlobalMethodRef(env);
        initGlobalFieldRef(env);
        return JNI_VERSION;
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
    return jint();
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* /*reserved*/) {
    JNIEnv* env = nullptr;
    vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION);

    try {
        env->DeleteGlobalRef(J_C_Map);
        env->DeleteGlobalRef(J_C_Set);
        env->DeleteGlobalRef(J_C_Iterator);
        env->DeleteGlobalRef(J_C_Map$Entry);
        env->DeleteGlobalRef(J_C_Exception);
        env->DeleteGlobalRef(J_C_QueryResult);
        env->DeleteGlobalRef(J_C_PreparedStatement);
        env->DeleteGlobalRef(J_C_DataType);
        env->DeleteGlobalRef(J_C_QuerySummary);
        env->DeleteGlobalRef(J_C_FlatTuple);
        env->DeleteGlobalRef(J_C_Value);
        env->DeleteGlobalRef(J_C_DataTypeID);
        env->DeleteGlobalRef(J_C_Boolean);
        env->DeleteGlobalRef(J_C_Long);
        env->DeleteGlobalRef(J_C_Integer);
        env->DeleteGlobalRef(J_C_InternalID);
        env->DeleteGlobalRef(J_C_Double);
        env->DeleteGlobalRef(J_C_BigDecimal);
        env->DeleteGlobalRef(J_C_LocalDate);
        env->DeleteGlobalRef(J_C_Instant);
        env->DeleteGlobalRef(J_C_Short);
        env->DeleteGlobalRef(J_C_Byte);
        env->DeleteGlobalRef(J_C_BigInteger);
        env->DeleteGlobalRef(J_C_Float);
        env->DeleteGlobalRef(J_C_Duration);
        env->DeleteGlobalRef(J_C_UUID);
        env->DeleteGlobalRef(J_C_Connection);
        env->DeleteGlobalRef(J_C_Database);
        env->DeleteGlobalRef(J_C_String);
    } catch (const Exception& e) {
        throwJNIException(env, e.what());
    } catch (...) {
        throwJNIException(env, "Unknown Error");
    }
}

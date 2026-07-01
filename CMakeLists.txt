set(CMAKE_FIND_PACKAGE_RESOLVE_SYMLINKS TRUE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project (lbug_java_api)

find_package(Java REQUIRED)
find_package(JNI REQUIRED)
if (JNI_FOUND)
    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
else()
    message (FATAL_ERROR "Java/JNI not found")
endif()
include(UseJava)

file(GLOB JAVA_SRC_FILES src/main/java/com/lbugdb/*.java)

# The Java sources import Apache Arrow classes (org.apache.arrow.c.*,
# org.apache.arrow.memory.*, org.apache.arrow.vector.*). When the Java
# sources are compiled through Gradle, these are pulled in from Maven
# Central via the dependencies block in build.gradle. The CMake build path
# (used by `make java` and the per-platform native lib CI job) does not go
# through Gradle, so we need to resolve the same JARs here. Versions are kept
# in sync with build.gradle.
set(LBUG_MAVEN_ARTIFACTS
    "org.apache.arrow:arrow-c-data:18.2.0"
    "org.apache.arrow:arrow-vector:18.2.0"
    "org.apache.arrow:arrow-memory-core:18.2.0"
    "org.apache.arrow:arrow-format:18.2.0"
    "com.google.flatbuffers:flatbuffers-java:24.3.25"
)

# Download a single Maven artifact JAR into a local cache directory.
# Args:
#   out_var     - output variable to receive the path to the downloaded JAR
#   group_id    - Maven group ID (e.g., org.apache.arrow)
#   artifact_id - Maven artifact ID (e.g., arrow-c-data)
#   version     - Maven version (e.g., 18.2.0)
function(lbug_download_maven_jar out_var group_id artifact_id version)
    string(REPLACE "." "/" group_path "${group_id}")
    set(jar_name "${artifact_id}-${version}.jar")
    set(url "https://repo1.maven.org/maven2/${group_path}/${artifact_id}/${version}/${jar_name}")
    set(local_path "${CMAKE_CURRENT_BINARY_DIR}/maven_cache/${jar_name}")
    if(NOT EXISTS "${local_path}")
        message(STATUS "Downloading ${group_id}:${artifact_id}:${version}")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/maven_cache")
        file(DOWNLOAD "${url}" "${local_path}"
            STATUS download_status
            LOG download_log
            TLS_VERIFY ON)
        list(GET download_status 0 status_code)
        list(GET download_status 1 error_msg)
        if(NOT status_code EQUAL 0)
            message(FATAL_ERROR
                "Failed to download Maven artifact ${group_id}:${artifact_id}:${version} "
                "from ${url}: ${error_msg}")
        endif()
    endif()
    set(${out_var} "${local_path}" PARENT_SCOPE)
endfunction()

set(LBUG_MAVEN_JARS "")
foreach(artifact_spec IN LISTS LBUG_MAVEN_ARTIFACTS)
    string(REPLACE ":" ";" spec_parts "${artifact_spec}")
    list(GET spec_parts 0 spec_group)
    list(GET spec_parts 1 spec_artifact)
    list(GET spec_parts 2 spec_version)
    lbug_download_maven_jar(jar_path "${spec_group}" "${spec_artifact}" "${spec_version}")
    list(APPEND LBUG_MAVEN_JARS "${jar_path}")
endforeach()

set(CMAKE_JAVA_COMPILE_FLAGS -source 1.8 -target 1.8 -encoding utf-8)
add_jar(lbug_java ${JAVA_SRC_FILES}
    INCLUDE_JARS ${LBUG_MAVEN_JARS}
    OUTPUT_DIR "${PROJECT_SOURCE_DIR}/build"
    GENERATE_NATIVE_HEADERS lbug_native_header)
get_target_property(_jarFile lbug_java JAR_FILE)
get_target_property(_classDir lbug_java CLASSDIR)

set (_stubDir "${CMAKE_CURRENT_BINARY_DIR}")

add_library(lbug_java_native SHARED src/jni/lbug_java.cpp)
target_include_directories(lbug_java_native PRIVATE ${JNI_INCLUDE_DIRS} ${_classDir} ${_stubDir})

if(LBUG_API_USE_PRECOMPILED_LIB)
    get_filename_component(LBUG_API_PRECOMPILED_LIB_DIR "${LBUG_API_PRECOMPILED_LIB_PATH}" DIRECTORY)
    get_filename_component(LBUG_API_PRECOMPILED_PREFIX "${LBUG_API_PRECOMPILED_LIB_DIR}" DIRECTORY)
    set(LBUG_API_PRECOMPILED_INCLUDE_DIR "${LBUG_API_PRECOMPILED_PREFIX}/include")
    target_include_directories(lbug_java_native PRIVATE
        ${LBUG_API_PRECOMPILED_LIB_DIR}
        ${LBUG_API_PRECOMPILED_INCLUDE_DIR}
    )
    if(WIN32)
        target_compile_definitions(lbug_java_native PRIVATE LBUG_STATIC_DEFINE)
    endif()
    target_link_libraries(lbug_java_native PRIVATE
        lbug_native_header
        "${LBUG_API_PRECOMPILED_LIB_PATH}"
        lbug_link_deps
    )
else()
    target_link_libraries(lbug_java_native PRIVATE lbug_native_header lbug)
endif()

message(STATUS "OS_NAME=${OS_NAME}")
message(STATUS "OS_ARCH=${OS_ARCH}")
string(JOIN "_" LIB_SUFFIX ".so" ${OS_NAME} ${OS_ARCH})

set_target_properties(lbug_java_native PROPERTIES SUFFIX ${LIB_SUFFIX})
set_target_properties(lbug_java_native PROPERTIES PREFIX "lib")
set_target_properties(lbug_java_native
        PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build"
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build"
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build")

add_custom_command(OUTPUT dummy_jar_target
    DEPENDS lbug_java_native lbug_java
    COMMAND ${Java_JAR_EXECUTABLE} uf ${_jarFile} -C
            $<TARGET_FILE_DIR:lbug_java_native> $<TARGET_FILE_NAME:lbug_java_native>)

add_custom_target(lbug_java_api ALL DEPENDS dummy_jar_target)

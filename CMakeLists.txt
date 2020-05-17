PROJECT(php7)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ENABLE_LANGUAGE(ASM)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE SRC_LIST FOLLOW_SYMLINKS 
    Zend/*.c
    main/*.c
    ext/pcre/pcre2lib/pcre2_auto_possess.c 
    ext/pcre/pcre2lib/pcre2_chartables.c 
    ext/pcre/pcre2lib/pcre2_compile.c 
    ext/pcre/pcre2lib/pcre2_config.c 
    ext/pcre/pcre2lib/pcre2_context.c 
    ext/pcre/pcre2lib/pcre2_dfa_match.c 
    ext/pcre/pcre2lib/pcre2_error.c 
    ext/pcre/pcre2lib/pcre2_jit_compile.c 
    ext/pcre/pcre2lib/pcre2_maketables.c 
    ext/pcre/pcre2lib/pcre2_match.c 
    ext/pcre/pcre2lib/pcre2_match_data.c 
    ext/pcre/pcre2lib/pcre2_newline.c
    ext/pcre/pcre2lib/pcre2_ord2utf.c 
    ext/pcre/pcre2lib/pcre2_pattern_info.c
    ext/pcre/pcre2lib/pcre2_serialize.c
    ext/pcre/pcre2lib/pcre2_string_utils.c
    ext/pcre/pcre2lib/pcre2_study.c 
    ext/pcre/pcre2lib/pcre2_substitute.c  
    ext/pcre/pcre2lib/pcre2_substring.c
    ext/pcre/pcre2lib/pcre2_tables.c 
    ext/pcre/pcre2lib/pcre2_ucd.c 
    ext/pcre/pcre2lib/pcre2_valid_utf.c
    ext/pcre/pcre2lib/pcre2_xclass.c
    ext/pcre/pcre2lib/pcre2_find_bracket.c 
    ext/pcre/pcre2lib/pcre2_convert.c 
    ext/pcre/pcre2lib/pcre2_extuni.c 
    ext/pcre/pcre2lib/pcre2_script_run.c
    ext/pcre/php_pcre.c
    ext/hash/*.c
    ext/date/*.c
    ext/reflection/*.c
    ext/spl/*.c
    ext/standard/*.c
    ext/mysqlnd/*.c
    ext/json/*.c
    ext/session/*.c
    ext/pdo/*.c
    ext/bcmath/*.c
    ext/openssl/*.c
    ext/tokenizer/*.c
    ext/phar/*.c
    ext/curl/*.c 
    ext/bz2/*.c
    ext/pcntl/*.c 
    ext/posix/*.c
    ext/mbstring/*.c
    ext/pdo_mysql/*.c 
    ext/pdo_sqlite/*.c
    ext/mysqli/*.c
    ext/libxml/*.c
    ext/dom/*.c
    ext/simplexml/*.c
    ext/xml/*.c
    ext/sqlite3/*.c
    ext/zlib/*.c
    ext/calendar/*.c
    ext/ctype/*.c
    ext/exif/*.c
    ext/fileinfo/*.c
    ext/filter/*.c
    ext/gd/*.c
    ext/iconv/*.c
    ext/sockets/*.c
    ext/xmlreader/*.c   
    ext/xmlwriter/*.c
    ext/readline/*.c
    ext/sysvmsg/*.c
    ext/sysvshm/*.c
    ext/redis/*.c
    ext/swoole/*.c
    ext/swoole/*.cc
    ext/swoole/thirdparty/boost/asm/combined.S
    ext/phpx/src/*.cc
)
file(GLOB_RECURSE HEAD_FILES FOLLOW_SYMLINKS Zend/*.h)

list(FILTER SRC_LIST EXCLUDE REGEX "ext/swoole/thirdparty/hiredis/examples/*")
list(FILTER SRC_LIST EXCLUDE REGEX "ext/swoole/thirdparty/hiredis/test.c")
list(FILTER SRC_LIST EXCLUDE REGEX "ext/swoole/examples/*")
list(FILTER SRC_LIST EXCLUDE REGEX "ext/swoole/core-tests/*")
list(FILTER SRC_LIST EXCLUDE REGEX "ext/swoole/thirdparty/php/streams/plain_wrapper.c")

SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# message(STATUS "source=${SRC_LIST}")
# message(STATUS "header=${HEAD_FILES}")

add_definitions(-DKeccakP200_excluded 
    -DKeccakP400_excluded 
    -DKeccakP800_excluded
    -DONIG_ESCAPE_UCHAR_COLLISION=1
    -DUChar=OnigUChar
    -DHAVE_BUNDLED_PCRE 
    -DPCRE2_CODE_UNIT_WIDTH=8 
    -DHAVE_CONFIG_H)

INCLUDE_DIRECTORIES(BEFORE ./Zend ./ ./TSRM ./main ./ext
    /usr/include/libxml2
    /usr/include/freetype2
    ext/swoole/include
    ext/swoole
    ext/phpx/include
    ext/bcmath/libbcmath/src
    ext/fileinfo/libmagic
    ext/hash/sha3/generic64lc
    ext/mbstring/libmbfl
    ext/mbstring/libmbfl/mbfl
    ext/gd/libgd
    ext/gd
)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)


#static library
add_library(static STATIC ${SRC_LIST})
set_target_properties(static PROPERTIES OUTPUT_NAME "php" VERSION "8.0.0-dev")
target_link_libraries(static pthread m png jpeg z rt resolv freetype pcre webp dl ssl crypto crypt xml2 bz2 curl sqlite3 onig brotlienc brotlidec readline)

# php-cli
file(GLOB_RECURSE cli_src_list FOLLOW_SYMLINKS 
    sapi/cli/*.c)
add_executable(php-cli ${cli_src_list})
add_dependencies(php-cli static)
target_link_libraries(php-cli static)

# php-fpm
INCLUDE_DIRECTORIES(BEFORE sapi/fpm)
file(GLOB_RECURSE fpm_src_list FOLLOW_SYMLINKS 
    sapi/fpm/*.c)

list(FILTER fpm_src_list EXCLUDE REGEX "sapi/fpm/fpm/fpm_trace_mach.c")
list(FILTER fpm_src_list EXCLUDE REGEX "sapi/fpm/fpm/fpm_systemd.c")
list(FILTER fpm_src_list EXCLUDE REGEX "sapi/fpm/fpm/fpm_trace_pread.c")

message(STATUS "source=${fpm_src_list}")

add_executable(php-fpm ${fpm_src_list})
add_dependencies(php-fpm static)
target_link_libraries(php-fpm static)

LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})

# embed
add_library(php SHARED ${SRC_LIST})
file(GLOB_RECURSE lib_src_list FOLLOW_SYMLINKS
    sapi/embed/*.c)
add_dependencies(php static)

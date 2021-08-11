cmake_minimum_required(VERSION 3.13)
project(zua C)

set(CMAKE_C_STANDARD 99)

include_directories(.)

#find_package(BISON)
#BISON_TARGET(zuaParser parser.y ${CMAKE_CURRENT_BINARY_DIR}/zua_parser.c
#        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/zua_parser.h)
#
#find_package(FLEX)
#FLEX_TARGET(zuaLexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/zua_lexer.c)

add_executable(zua
        main.c
        zua_scanner.c
        zua_type.c
        zua_parser.c
)

#%define api.prefix {php_json_yy}
#%define api.pure full
#%param  { json_parser *parser  }
%require "3.0"

%code requires {
    #include "zua_type.h"
    #include "zua_parser_defs.h"
}

%code top {
    /* bison --defines -l zua_parser.y -o zua_parser.c */
    #include <stdio.h>
    #include "zua_scanner.h"

    #define YYDEBUG 1
    #define YYERROR_VERBOSE 1
}

%code {
    static int zua_yylex(ZUA_YYSTYPE *, zua_json_parser *parser);
    static void zua_yyerror(zua_json_parser *parser, const char *str);
}

%union {
    zval value;
}

%define api.prefix {zua_yy}
%define api.pure full
%param {zua_json_parser *parser}
%destructor { zval_free_nogc(&$$); } <value>

%token<value> ZUA_JSON_T_NUL
%token<value> ZUA_JSON_T_NAN
%token<value> ZUA_JSON_T_INFINITY
%token<value> ZUA_JSON_T_NEGATIVE_INFINITY
%token<value> ZUA_JSON_T_TRUE
%token<value> ZUA_JSON_T_FALSE
%token<value> ZUA_JSON_T_INT
%token<value> ZUA_JSON_T_DOUBLE
%token<value> ZUA_JSON_T_STRING
%token<value> ZUA_JSON_T_ETRING
%token ZUA_JSON_T_EOI
%token ZUA_JSON_T_ERROR
%token ZUA_JSON_T_COMMENT_NOT_CLOSED


%type <value> start object array key value
%type <value> members member elements element

%%

start:
    value ZUA_JSON_T_EOI
        {
            ZUA_COPY_VALUE(&$$, &$1);
            ZUA_COPY_VALUE(parser->return_value, &$1);
            YYACCEPT;
        }
;


object:
    '{'
        {
        }
    members object_end
        {
            ZUA_COPY_VALUE(&$$, &$3);
        }
;


object_end:
        '}'
    |   ']'
            {
                parser->scanner.errcode = ZUA_JSON_BRACKET_MISMATCH;
                YYERROR;
            }
;

members:
        %empty
        {
            object_init(&$$);
        }
    |   member member_end
;


member:
        key ':' value
            {
                object_init(&$$);
                zua_hash_str_add_or_update(&$$, ZSTRL(Z_STR($1)), &$3);
                zua_string_free(Z_STR($1));
            }
    |   member ',' key ':' value
            {
                zua_hash_str_add_or_update(&$1, ZSTRL(Z_STR($3)), &$5);
                ZUA_COPY_VALUE(&$$, &$1);
                zua_string_free(Z_STR($3));
            }
;

member_end:
        %empty
    |   ','
;


array:
    '['
        {
        }
    elements array_end
        {
            ZUA_COPY_VALUE(&$$, &$3);
        }
;

array_end:
        ']'
    |   '}'
            {
                parser->scanner.errcode = ZUA_JSON_BRACKET_MISMATCH;
                YYERROR;
            }
;

elements:
        %empty
            {
                array_init(&$$);
            }
    |   element element_end
;


element:
        value
            {
                array_init(&$$);
                zua_hash_index_add(&$$, &$1);
            }
    |   element ',' value
            {
                zua_hash_index_add(&$1, &$3);
                ZUA_COPY_VALUE(&$$, &$1);
            }
;

element_end:
        %empty
    |   ','
;

key:
        ZUA_JSON_T_STRING
    |   ZUA_JSON_T_ETRING
;


value:
        object
    |   array
    |   ZUA_JSON_T_NUL
    |   ZUA_JSON_T_NAN
    |   ZUA_JSON_T_INFINITY
    |   ZUA_JSON_T_NEGATIVE_INFINITY
    |   ZUA_JSON_T_TRUE
    |   ZUA_JSON_T_FALSE
    |   ZUA_JSON_T_INT
    |   ZUA_JSON_T_DOUBLE
    |   ZUA_JSON_T_STRING
;

%%

void zua_json_parser_init( zua_json_parser *parser, zval *return_value, const char *str, size_t str_len, int options, int max_depth ) {
    memset(parser, 0, sizeof(zua_json_parser));
    zua_json_scanner_init(&parser->scanner, str, str_len, options);
    parser->depth = 1;
    parser->max_depth = max_depth;
    parser->return_value = return_value;
}

static void zua_yyerror( zua_json_parser *parser, const char *str )
{
    parser->scanner.errcode = ZUA_JSON_SYNTAX_ERROR;
}

static int zua_yylex(ZUA_YYSTYPE *val, zua_json_parser *parser)
{
    int token = zua_json_scan(&parser->scanner);
    val->value = parser->scanner.value;
    return token;
}



/*
 * re2c -t zua_scanner_defs.h -bci -o zua_scanner.c --no-generation-date zua_scanner.re
*/
#include <stdio.h>
#include "zua_scanner.h"
#include "zua_scanner_defs.h"
#include "zua_parser.h"
#include "zua_type.h"
#include <ctype.h>
#include <inttypes.h>

#define YYCTYPE     zua_json_ctype
#define YYCURSOR    s->cursor
#define YYLIMIT     s->limit
#define YYMARKER    s->marker
#define YYCTXMARKER s->ctxmarker

#define YYGETCONDITION()        s->state
#define YYSETCONDITION(yystate) s->state = yystate

#define YYFILL(n)

#define ZUA_JSON_CONDITION_SET(condition) YYSETCONDITION(yyc##condition)
#define ZUA_JSON_CONDITION_GOTO(condition) goto yyc_##condition
#define ZUA_JSON_CONDITION_SET_AND_GOTO(condition) \
    ZUA_JSON_CONDITION_SET(condition);            \
    ZUA_JSON_CONDITION_GOTO(condition)

void zua_json_scanner_init(zua_json_scanner *s, const char *str, uint32_t str_len, int options) {
    s->cursor = (zua_json_ctype *)str;
    s->limit  = (zua_json_ctype *)str + str_len;
    s->options = options;
    ZUA_JSON_CONDITION_SET(JS);
}


int zua_json_scan(zua_json_scanner *s) {
    ZVAL_NULL(&s->value);
    
std:
    s->token = s->cursor;
    
/*!re2c
re2c:indent:top = 1;
re2c:yyfill:enable = 0;

DIGIT   = [0-9];
DIGITNZ = [1-9];
UINT    = "0" | (DIGITNZ DIGIT*);
INT     = ("-"|"+")? UINT ;
FLOAT   = (INT) ? "." DIGIT* ;
EXP     = ( INT | FLOAT ) [eE] [+-]? DIGIT+ ;
NL      = "\r"? "\n";
HEX     = "0"[xX][a-fA-F]+;
WS      = [ \t\r]+;
EOI     = "\000";
UTF8T   = [\x80-\xBF] ;
UTF8_1  = [\x00-\x7F] ;
UTF8_2  = [\xC2-\xDF] UTF8T ;
UTF8_3A = "\xE0" [\xA0-\xBF] UTF8T ;
UTF8_3B = [\xE1-\xEC] UTF8T{2} ;
UTF8_3C = "\xED" [\x80-\x9F] UTF8T ;
UTF8_3D = [\xEE-\xEF] UTF8T{2} ;
UTF8_3  = UTF8_3A | UTF8_3B | UTF8_3C | UTF8_3D ;
UTF8_4A = "\xF0"[\x90-\xBF] UTF8T{2} ;
UTF8_4B = [\xF1-\xF3] UTF8T{3} ;
UTF8_4C = "\xF4" [\x80-\x8F] UTF8T{2} ;
UTF8_4  = UTF8_4A | UTF8_4B | UTF8_4C ;
UTF8    = UTF8_1 | UTF8_2 | UTF8_3 | UTF8_4;
ANY     = [^];
ESCPREF = "\\" ;
ESCSYM  = ( "\"" | "\\" | "/" | [bfnrt] ) ;
ESC     = ESCPREF ESCSYM ;
UTFSYM  = "u" ;
UTFPREF = ESCPREF UTFSYM ;
LABEL	= [a-zA-Z_\x80-\xff][a-zA-Z0-9_\x80-\xff]*;


<JS>"{"                  { return '{'; }
<JS>"}"                  { return '}'; }
<JS>"["                  { return '['; }
<JS>"]"                  { return ']'; }
<JS>":"                  { return ':'; }
<JS>","                  { return ','; }
<JS>"null"               {
    ZVAL_NULL(&s->value);
    return ZUA_JSON_T_NUL;
}
<JS>"true"               {
    ZVAL_TRUE(&s->value);
    return ZUA_JSON_T_TRUE;
}
<JS>"false"              {
    ZVAL_FALSE(&s->value);
    return ZUA_JSON_T_FALSE;
}
<JS>"NaN"               {
    ZVAL_NAN(&s->value);
    return ZUA_JSON_T_NAN;
}
<JS>"Infinity"          {
    ZVAL_INFINITY(&s->value);
    return ZUA_JSON_T_INFINITY;
}
<JS>"-Infinity"          {
    ZVAL_NINFINITY(&s->value);
    return ZUA_JSON_T_NEGATIVE_INFINITY;
}
<JS>INT                  {
    ZVAL_LONG(&s->value, strtol((char *)s->token, NULL, 10));
    return ZUA_JSON_T_INT;
}
<JS>HEX                  {
    ZVAL_LONG(&s->value, strtol((char *)s->token, NULL, 16));
    return ZUA_JSON_T_INT;
}
<JS>FLOAT|EXP            {
    ZVAL_DOUBLE(&s->value, strtod((char *) s->token, NULL));
    return ZUA_JSON_T_DOUBLE;
}
<JS>NL|WS                { goto std; }
<JS>EOI                  {
    if (s->limit < s->cursor) {
        return ZUA_JSON_T_EOI;
    } else {
        s->errcode = 1;
        return ZUA_JSON_T_ERROR;
    }
}
<JS>LABEL                {
    ZVAL_STRINGL(&s->value, s->token, s->cursor - s->token);
    ZUA_JSON_CONDITION_SET(JS);
    return ZUA_JSON_T_ETRING;
}
<JS>"@'"                {
    s->str_start = s->cursor;
    s->str_esc = 0;
    s->utf8_invalid = 0;
    s->utf8_invalid_count = 0;
    ZUA_JSON_CONDITION_SET_AND_GOTO(STR_P2);
}
<STR_P2>[^']*            { ZUA_JSON_CONDITION_GOTO(STR_P2); }
<STR_P2>[']              {
    size_t len = s->cursor - s->str_start - s->str_esc - 1 + s->utf8_invalid_count;
    ZVAL_STRINGL(&s->value, s->str_start, len);
    ZUA_JSON_CONDITION_SET(JS);
    return ZUA_JSON_T_STRING;
}
<JS>"@\""                {
    s->str_start = s->cursor;
    s->str_esc = 0;
    s->utf8_invalid = 0;
    s->utf8_invalid_count = 0;
    ZUA_JSON_CONDITION_SET_AND_GOTO(STR_P1);
}
<JS>["']                 {
    char c = s->token[0];
    s->str_start = s->cursor;
    uint32_t i = 0, j = 0;
    zua_string *str = NULL;
    for (; ; i++) {
        if (YYCURSOR < YYLIMIT) {
            if (*YYCURSOR == c) break;
            if (*YYCURSOR++ == '\\') {
                if (str == NULL) {
                    str = zua_string_create(s->str_start, i - j);
                } else {
                    str = zua_string_append(str, s->str_start+j, i - j);
                }
                while (isspace(*YYCURSOR)) {
                    YYCURSOR++;
                    i++;
                }
                j = i + 1;
            }
        } else {
            zua_string_free(str);
            return ZUA_JSON_T_ERROR;
        }
    }
    str = zua_string_append(str, s->str_start+j, i - j);
    s->cursor++;
    Z_STR_P(&s->value) = str;
    Z_TYPE_P(&s->value) = IS_STRING;
    return ZUA_JSON_T_STRING;
}
<JS>"//"                 {
    ZUA_JSON_CONDITION_SET_AND_GOTO(COMMENTS);
}
<JS>"/*"                 {
    while(YYCURSOR < YYLIMIT) {
        if (*YYCURSOR++ == '*' && *YYCURSOR == '/') {
            break;
        }
    }
    if (YYCURSOR < YYLIMIT) {
        YYCURSOR++;
    } else {
        return ZUA_JSON_T_COMMENT_NOT_CLOSED;
    }
    goto std;
}

<COMMENTS>[^\n]          {
    if (YYCURSOR >= YYLIMIT) {
        return ZUA_JSON_T_ERROR;
    }
    ZUA_JSON_CONDITION_SET_AND_GOTO(COMMENTS);
}
<COMMENTS>[\n]           { ZUA_JSON_CONDITION_SET_AND_GOTO(JS); }

<STR_P1>[^"]*            { ZUA_JSON_CONDITION_GOTO(STR_P1); }
<STR_P1>["]              {
    size_t len = s->cursor - s->str_start - s->str_esc - 1 + s->utf8_invalid_count;
    ZVAL_STRINGL(&s->value, s->str_start, len);
    ZUA_JSON_CONDITION_SET(JS);
    return ZUA_JSON_T_STRING;
}
<*>ANY {
    return ZUA_JSON_T_ERROR;
}
*/
}
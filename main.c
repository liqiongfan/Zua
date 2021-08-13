#include <stdio.h>
#include "zua_parser_defs.h"

int main(int argc, char *argv[]) {

    zval *response = json_decode(ZUA_STR("{ code: 200, message: 'SUCCESS', language: ['Golang', 'C/C++'] }"));
    if (response->u2.errcode != 0) {
        printf("解析错误~");
        zval_free(response);
        return 0;
    }
    zval *code = zua_get_value(response, ZUA_STR("code"));
    code != NULL && printf("code:%ld\n", Z_LVAL_P(code));

    zval *message = zua_get_value(response, ZUA_STR("message"));
    message != NULL && printf("message: %s\n", ZSTR_VAL(Z_STR_P(message)));

    zval *lang = zua_get_value(response, ZUA_STR("language"));
    if (lang != NULL) {
        zval *favor_language = zua_get_value_by_index(lang, 0);
        printf("Favor Language: %s\n", ZSTR_VAL(Z_STR_P(favor_language)));
    }

    zval_free(response);

    return 0;
}
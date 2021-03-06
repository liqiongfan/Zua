#include <stdio.h>
#include "zua_parser_defs.h"

int main(int argc, char *argv[]) {

    zua_string *file_json = zua_file_gets("../examples/file.json5");

    zval *response = json_decode(ZSTRL(file_json));
    printf("%s\n", ZSTR_VAL(file_json));

    if (response->u2.errcode != 0) {
        printf("解析错误~");
        zval_free(response);
        return 0;
    }
    zval *unquoted = zua_get_value(response, ZUA_STR("unquoted"));
    printf("unquoted:%s\n", ZSTR_VAL(Z_STR_P(unquoted)));

    zval *lineBreaks = zua_get_value(response, ZUA_STR("lineBreaks"));
    printf("lineBreaks: %s\n", ZSTR_VAL(Z_STR_P(lineBreaks)));
    
    zval *positiveSign = zua_get_value_by_path(response, ZUA_STR("test.cc.4"));
    printf("positiveSign: %s\n", ZSTR_VAL(Z_STR_P(positiveSign)));

    zval_free(response);
    zua_string_free(file_json);

    return 0;
}
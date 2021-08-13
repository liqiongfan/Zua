#include <stdio.h>
#include "zua_parser_defs.h"

int main(int argc, char *argv[]) {

    zval *response = json_decode(ZUA_STR("{\n"
                                         "  // comments\n"
                                         "  unquoted: 'and you can quote me on that',\n"
                                         "  singleQuotes: 'I can use \"double quotes\" here',\n"
                                         "  lineBreaks: \"Look, Mom! \\\n"
                                         "No \\\\n's!\",\n"
                                         "  positiveSign: +1,\n"
                                         "  trailingComma: 'in objects', andIn: ['arrays',],\n"
                                         "  \"backwardsCompatible\": \"with JSON\",\n"
                                         "}"));
    if (response->u2.errcode != 0) {
        printf("解析错误~");
        zval_free(response);
        return 0;
    }
    zval *unquoted = zua_get_value(response, ZUA_STR("unquoted"));
    printf("unquoted:%ld\n", Z_LVAL_P(unquoted));

    zval *lineBreaks = zua_get_value(response, ZUA_STR("lineBreaks"));
    printf("lineBreaks: %s\n", ZSTR_VAL(Z_STR_P(lineBreaks)));

    zval_free(response);

    return 0;
}
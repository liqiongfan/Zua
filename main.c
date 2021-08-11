#include <stdio.h>
#include "zua_parser_defs.h"

int main(int argc, char *argv[]) {

    char str[] = "{\n"
                 "  // comments\n"
                 "  unquoted: 'and you can quote me on that',\n"
                 "  singleQuotes: 'I can use \"double quotes\" here',\n"
                 "  lineBreaks: \"Look, Mom! \\\n"
                 "No \\\\n's!\",\n"
                 "  hexadecimal: 0xdecaf,\n"
                 "  leadingDecimalPoint: .8675309, andTrailing: 8675309.,\n"
                 "  positiveSign: +1,\n"
                 "  trailingComma: 'in objects', andIn: ['arrays',],\n"
                 "  \"backwardsCompatible\": \"with JSON\",\n"
                 "}";

    zval *r = json_decode(ZUA_STR(str));

    if (r->u2.errcode == ZUA_JSON_SYNTAX_ERROR) {
        printf("语法错误");
        return 0;
    }
    if (r->u2.errcode == ZUA_JSON_BRACKET_MISMATCH) {
        printf("JSON 括号不匹配~");
        return 0;
    }

    zua_string *s = json_encode_pretty(r);

    if (s != NULL) {
        printf("%s\n", ZSTR_VAL(s));
    }

    zval_free(r);
    zua_string_free(s);

    return 0;
}
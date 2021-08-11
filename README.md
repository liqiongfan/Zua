## Zua - another json tool written in pure C.

Zua is the json encoding/decoding tool written in pure C. which depends on `re2c` & `bison`, The lexer parsing command:

```shell script
re2c -t zua_scanner_defs.h -bci -o zua_scanner.c --no-generation-date zua_scanner.re
```

The bison grammar parsing file:

```shell script
bison --defines -l zua_parser.y -o zua_parser.c
```

Currently developing the `JSON5`, now support the following rules:

**Objects**
---

- [x] Object keys may be an ECMAScript 5.1 IdentifierName.
- [x] Objects may have a single trailing comma.

**Arrays**
---

- [x] Arrays may have a single trailing comma.

**Strings**
---

- [x] Strings may be single quoted.
- [x] Strings may span multiple lines by escaping new line characters.
- [x] Strings may include character escapes.

**Numbers**
---

- [x] Numbers may be hexadecimal.
- [x] Numbers may have a leading or trailing decimal point.
- [x] Numbers may be IEEE 754 positive infinity, negative infinity, and NaN.
- [x] Numbers may begin with an explicit plus sign.

**Comments**
---

- [x] Single and multi-line comments are allowed.

**White Space**
---

- [x] Additional white space characters are allowed.

### Features

Zua is the world's most popular `json` encoding/decoding library in the C/C++ world.
+ Support Comments, such as `//` or `/* Hello This is the comments */`
+ Support long int
+ Support boolean true/false
+ Support null
+ Support json pretty
+ Support Hexadecimal
+ Support NaN、 Infinity、 -Infinity
+ Support JSON5(100%).

### APIs

+ zval *json_decode(const char *str, unsigned int str_len)

    解码 `JSON` 字符串为 `zval` 结构体, 便于查询使用

---

+ zua_string *json_encode(zval *val)

    编码 `zval` 为 `JSON` 字符串

---
+ zua_string *json_encode_pretty(zval *val)

    编码 `zval` 为 `JSON` 字符串，美化JSON输出

---

+ zval *zua_get_value(zval *v, const char *key, unsigned int key_len)

    从对象中获取数据, key为键值, key_len为键的长度

---

+ zval *zua_get_value_by_index(zval *v, uint32_t index)

    从数组中获取数据, index为数组的索引位置

---
+ zval *zval_init();

    初始化一个zval结构体
---
+ object_init(zval *v)

    将 `zval` 初始化为 `object`

---
+ array_init(zval *v)

    将 `zval` 初始化为 `array`

---
+ zval_free(zval *v)

    释放 `zval` 结构体占用的内存

---
+ zua_hash_str_add_or_update(zval *h, const char *key, uint32_t key_len, zval *value)

    添加 `key` `value` 到 `zval` `h` 中

---
+ void zua_hash_index_add(zval *h, zval *value)

    添加 `value` 到 `zval` `h` 中

### Notice

底层采用 `HashMap` 存储数据, 所以数据的顺序不保证跟输入的顺序一致


### Examples

解码示例

```C
zval *r = json_decode(ZUA_STR("{\"code\": 200, \"message\": \"SUCCESS\", \"data\":[1, 2, 3]}"));

if (r->u2.errcode == ZUA_JSON_SYNTAX_ERROR) {
    printf("语法错误");
    return 0;
}
if (r->u2.errcode == ZUA_JSON_BRACKET_MISMATCH) {
    printf("JSON 括号不匹配~");
    return 0;
}
zval *code = zua_get_value(r, ZUA_STR("code"));
printf("code: %ld\n", Z_LVAL_P(code));

zval_free(r);
```

---
编码示例

```c
zval *obj = zval_init();
object_init(obj);

zval name;
ZVAL_STRING(&name, "v1.0.1");
zua_hash_str_add_or_update(obj, ZUA_STR("version"), &name);

zval code;
ZVAL_LONG(&code, 200);
zua_hash_str_add_or_update(obj, ZUA_STR("code"), &code);

zua_string *str = json_encode(obj);

printf("%s\n", ZSTR_VAL(str));

zua_string_free(str);
zval_free(obj);
```
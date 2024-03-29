## Zua 一个易用的 `JSON` & `JSON5` 解析引擎

Zua 采用纯C语言开发, 可以在任意支持标准C的机器上运行，占用内存少，解码格式多的特点，既可以解析 标准`JSON` 也可以解析`JSON5`格式
采用 `HashMap` 结构保存数据，查询性能能够达到 O(1), 无内存泄漏风险.


## C++版本

C++ Language press this to obtain: [jsoncpp](https://github.com/liqiongfan/jsoncpp)
<https://github.com/liqiongfan/jsoncpp>

建议C++语言使用上面的C++版本


## 项目地址

`https://github.com/liqiongfan/Zua`

### 设计理念

整个解析引擎使用 `zval` 结构体(struct _zua_struct) 来表达各个节点值，为了达到 O(1) 的查询性能，解析的数据存储到 `HashMap` 。


### JSON5 特性

##### Objects(对象)

- [x] 对象的键名可以不使用引号
- [x] 对象的键名可以使用单引号
- [x] 对象元素的末尾可以以一个逗号结束


##### Arrays(数组)

- [x] 数组元素的末尾可以以一个逗号结束


##### Strings(字符串)

- [x] 字符串可以使用单引号
- [x] 字符串可以跨越多行, 每行以转义符 '\\' 结尾
- [x] 字符串可以包含转义字符


##### Numbers(数字)

- [x] 数字可以是十六进制
- [x] 浮点数字可以只是单纯的 `.` 或者 以 `.` 开头或者结尾
- [x] 数字可以是 IEEE754标准的无穷大(`Infinity`)或者负无穷大(`-Infinity`)以及 `NaN`
- [x] 正数可以使用`+`明确指定


##### Comments(注释)

- [x] 可以使用单行注释(`//`)或者多行注释(`/* */`)


##### White Space(空白符)

- [x] 允许额外的空白字符


### APIs

+ ZUA_API zua_string *zua_file_gets(const char *file_name)：

    读取文件全部内容为`zua_string`,返回数据需要 `zua_string_free`

+ zval *json_decode(const char *str, unsigned int str_len):

    解码`JSON`字符串

+ zua_string *json_encode(zval *v):

    编码为`JSON`字符串

+ zua_string *json_encode_pretty(zval *val):

    编码为格式化的`JSON`字符串

+ zval *zua_get_value(zval *v, const char *key, unsigned int key_len):

    从对象中查询指定`key`的值

+ zval *zua_get_value_by_index(zval *v, uint32_t index):

    从数组中查询指定索引坐标的值，偏移值从 `0` 开始计数
    
+ zval *zua_get_value_by_path(zval *r, const char *str, uint32_t str_len)：
    
    使用点号的字符串查询 `JSON` 格式获取数据
    
+ bool zua_in_array(zval *r, zval *value):

    查询 value 是否在 r 中，注意如果 value 是 `array` 或者 `object` 则直接返回 FALSE

+ zval *zval_init():

    得到一个分配好内存的`zval`

+ object_init(zval *v):

    初始化为对象

+ array_init(zval *v):

    初始化为数组

+ zua_hash_str_add_or_update(zval *h, const char *key, uint32_t key_len, zval *value):

    往对象里面新增`key` & `value`对

+ void zua_hash_index_add(zval *h, zval *value):

    往数组里面新增值

+ zval_free(zval *v):

    释放内存


### 示例

+ 解码

```C
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
```


+ 编码

```C
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
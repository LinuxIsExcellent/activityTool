#define TCP_ALIVE_TIMEOUT 600
#define TCP_BUFFER_SIZE 1024
#define MIN(a, b) a <= b ? a : b
#define MAX(a, b) a >= b ? a : b
#define ISDIGIT(str) (str.find_first_not_of("-.0123456789") == string::npos)
#define AUTO_GEN_FILE_DESC "-- 此文件由程序在北京时间 %s 自动生成"

/*
 函数说明：对字符串中所有指定的子串进行替换
 参数：
string resource_str            //源字符串
string sub_str                //被替换子串
string new_str                //替换子串
返回值: string
 */
string subreplace(string resource_str, string sub_str, string new_str);


/*
 函数说明：检测是否包含某一个字串
 参数：
string resource_str            //源字符串
string sub_str                //被替换子串
返回值: bool
 */
bool string_contains(string resource_str, string sub_str);

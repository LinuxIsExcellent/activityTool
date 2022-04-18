#define TCP_ALIVE_TIMEOUT 600
#define TCP_BUFFER_SIZE 1024
#define MIN(a, b) a <= b ? a : b
#define MAX(a, b) a >= b ? a : b
#define ISDIGIT(str) (str.find_first_not_of("-.0123456789") == string::npos)
#define AUTO_GEN_FILE_DESC "-- 此文件由程序在服务器时间 %s 自动生成"

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

/* 函数说明：将double转换成一个字符串
 参数：
 double price 		//转换的数字
返回值:string 		//结果串
*/

std::string doubleToString(double price);

// 全局5s定时器
void Global5STimer();

// lua文件重新加载检测定时器
void LuaConfigReloadTimer();

// 全局配置重新加载检测定时器
void GlobalConfigReloadTimer();


/* 函数说明：根据特定的字符切割字符串
 参数：
 string strtem 		//字符串
 char a 			//特定的字符
返回值:vector<string> 	//结果串
*/
vector<string> split(string strtem,char a);
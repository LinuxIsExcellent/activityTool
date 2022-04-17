#pragma once

// 键值对
typedef struct oneValuePair
{
    string sField;
    string sValue;
}VALUEPAIR;

// 一行的数据
typedef struct oneRowData
{
    int64_t id;      //对应的唯一id
    int64_t nFeildCount;    //有多少个字段(二维表的所有行都是一致的，只是为了好读取)
    std::vector<VALUEPAIR> dataList;    //一行内所有的列数据
}ROWDATA;

// 一个表的数据
typedef struct tableData
{
    string sTableName;
    int64_t nRow;    //多少行
    int64_t nColumn;    //多少列
    std::vector<ROWDATA> dataList;    //每行的数据
}TABLEDATA;


// 字段的信息
typedef struct fieldInfo
{
    string sFieldName;      //字段名字
    string sFieldAnnonation;      //字段的备注
    string sFieldLink;      //字段的关联
}FIELDINFO;

typedef struct fieldSquence
{
    std::vector<FIELDINFO> vSFieldSquences;       //对应的字段的值的信息
}FIELDSQUENCE;

// 一维表的一个数据
typedef struct LuaKeyValue
{
    string sKey;        //键
    int64_t keyType;        //键的类型
    string sValue;        //值
    int64_t fieldType;        //值的lua类型
}LUAKEYVALUE;

// 缩进符
const std::string TAB = "\t";

// 监听的进程的文件信息
typedef struct listenProcessInfo
{
    std::string pidFile;
    std::string processName;
}LISTENPROCESSINFO;

// 数据的一个键值对
typedef struct LuaArrayValue
{
    int64_t nKey;        //键
    string sValue;        //值
    int64_t fieldType;        //值的lua类型
}LUAARRAYVALUE;
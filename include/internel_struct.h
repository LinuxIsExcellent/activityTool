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
    int16_t id;      //对应的唯一id
    int16_t nFeildCount;    //有多少个字段(二维表的所有行都是一致的，只是为了好读取)
    std::vector<VALUEPAIR> dataList;    //一行内所有的列数据
}ROWDATA;

// 一个表的数据
typedef struct tableData
{
    string sTableName;
    int16_t nRow;    //多少行
    int16_t nColumn;    //多少列
    std::vector<ROWDATA> dataList;    //每行的数据
}TABLEDATA;


typedef struct fieldSquence
{
    std::vector<uint16_t> vNLevels;         //深度队列
    std::vector<std::string> vSFieldSquences;       //对应的字段顺序
}FIELDSQUENCE;

// 一维表的一个数据
typedef struct LuaKeyValue
{
    string sKey;        //键
    string sValue;        //值
    int16_t fieldType;        //值的lua类型
}LUAKEYVALUE;

#include "jsonwrapper.h"

#include <cassert>

#include <QJsonArray>
#include <QJsonDocument>

#include "common/printf.h"

JSONWrapper::JSONWrapper()
    : rootJson()
{
    //no operation
}

JSONWrapper::~JSONWrapper()
{
    //no operation
}

int JSONWrapper::Parse(const std::string &jsonStr)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(QByteArray(jsonStr.c_str()), &jsonError);
    if(jsonDoc.isNull() || jsonDoc.isEmpty()){
        PRINTF(LOG_ERR, "%s: empty json doc(%s)\n", __func__, jsonStr.c_str());
        return -1;
    }
    if( !jsonDoc.isObject() ){
        PRINTF(LOG_ERR, "%s: not object of doc(%s)\n", __func__, jsonStr.c_str());
        return -1;
    }
    if(jsonError.error != QJsonParseError::NoError){
        PRINTF(LOG_ERR, "%s: error(%s) from %s\n", __func__, jsonError.errorString().toStdString().c_str(), jsonStr.c_str());
        return -1;
    }

    rootJson = jsonDoc.object();
    return 0;
}

int JSONWrapper::loadString(const std::string &keyName, std::string &str)
{
    if( !CheckKeyName(keyName) ){
        return -1;
    }

    QJsonValue value = rootJson.value(keyName.c_str());
    if( !value.isString() ){
        PRINTF(LOG_ERR, "%s: %s is not string type\n", __func__, keyName.c_str());
        return -1;
    }

    str = value.toString().toStdString();
    return 0;
}

int JSONWrapper::loadInt(const std::string &keyName, int &number)
{
    //利用double过渡
    double num;
    int ret = loadNumber(keyName, num);

    number = num;
    return ret;
}

int JSONWrapper::loadDouble(const std::string &keyName, double &number)
{
    return loadNumber(keyName, number);
}

int JSONWrapper::loadIntArray(const std::string &keyName, int *number, const int len)
{
    //利用double过渡
    double num[len];
    int ret = loadArray(keyName, num, len);

    for(int i=0; i<len; ++i){
        number[i] = num[i];
    }
    return ret;
}

int JSONWrapper::loadDoubleArray(const std::string &keyName, double *number, const int len)
{
    return loadArray(keyName, number, len);
}

bool JSONWrapper::CheckKeyName(const std::string &keyName)
{
    if(rootJson.contains(keyName.c_str())){
        return true;
    }else{
        PRINTF(LOG_ERR, "%s: no keyName=%s\n", __func__, keyName.c_str());
        return false;
    }
}

int JSONWrapper::loadNumber(const std::string &keyName, double &number)
{
    if( !CheckKeyName(keyName) ){
        return -1;
    }

    QJsonValue value = rootJson.value(keyName.c_str());
    if( !value.isDouble() ){
        return -1;
    }

    number = value.toDouble();
    return 0;
}

int JSONWrapper::loadArray(const std::string &keyName, double *number, const int len)
{
    if( !CheckKeyName(keyName) ){
        return -1;
    }

    QJsonValue value = rootJson.value(keyName.c_str());
    if( !value.isArray() ){
        PRINTF(LOG_ERR, "%s: %s is not an array\n", __func__, keyName.c_str());
        return -1;
    }

    QJsonArray jsonArray = value.toArray();
    assert(jsonArray.size() == len);

    for(int i=0; i<len; ++i){
        QJsonValue value = jsonArray.at(i);
        if( !value.isDouble() ){
            PRINTF(LOG_ERR, "%s: %s is not double type\n", __func__, keyName.c_str());
            return -1;
        }
        number[i] = value.toDouble();
    }

    return 0;
}

std::string JSONWrapper::toString()
{
    QJsonDocument jsonDoc( rootJson );
    return QString(jsonDoc.toJson(QJsonDocument::Compact)).toStdString();
}

JSONWrapper &JSONWrapper::addString(const std::string &keyName, const std::string &str)
{
    return addJsonValue(keyName, QString(str.c_str()));
}

JSONWrapper &JSONWrapper::addInt(const std::string &keyName, const int &number)
{
    return addJsonValue(keyName, number);
}

JSONWrapper &JSONWrapper::addDouble(const std::string &keyName, const double &number)
{
    return addJsonValue(keyName, number);
}

JSONWrapper &JSONWrapper::addIntArray(const std::string &keyName, const int *number, const int len)
{
    QJsonArray jsonArray;
    for(int i=0; i<len; ++i){
        jsonArray.append( number[i] );
    }
    return addJsonArray(keyName, jsonArray);
}

JSONWrapper &JSONWrapper::addDoubleArray(const std::string &keyName, const double *number, const int len)
{
    QJsonArray jsonArray;
    for(int i=0; i<len; ++i){
        jsonArray.append( number[i] );
    }
    return addJsonArray(keyName, jsonArray);
}

JSONWrapper &JSONWrapper::addJsonValue(const std::string &keyName, const QJsonValue &value)
{
    //利用QJsonValue的构造函数(QJsonValue有接收int, double, QString为参数的构造函数)
    rootJson.insert(keyName.c_str(), value);
    return *this;
}

JSONWrapper &JSONWrapper::addJsonArray(const std::string &keyName, const QJsonArray &array)
{
    //利用QJsonValue的构造函数(接收QJsonArray为参数)
    rootJson.insert(keyName.c_str(), QJsonValue(array));
    return *this;
}

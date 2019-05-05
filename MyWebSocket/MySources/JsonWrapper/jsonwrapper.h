#ifndef JSONWRAPPER_H
#define JSONWRAPPER_H

#include <string>
#include <QJsonObject>

class JSONWrapper
{
public:
    explicit JSONWrapper();
    virtual ~JSONWrapper();

    /************ read data from JSON ************/
public:
    int Parse(const std::string& jsonStr);
    int loadString(const std::string& keyName, std::string& str);
    int loadInt(const std::string& keyName, int& number);
    int loadDouble(const std::string& keyName, double& number);
    int loadIntArray(const std::string& keyName, int* number, const int len);
    int loadDoubleArray(const std::string& keyName, double* number, const int len);

    /* 模版重载loadInt() 解决对于unsigned int/bool/char/short的load时出现的错误
     * eg. unsigned int作为参数调用loadInt(string&, int&), 编译器将进行隐式转换unsigend int--->int
     *     该int是临时变量, loadInt()中的引用修改是该临时的int 对于unsigned int没有效果
     *     所以编译器将拒绝引用右值 */
    template<class T>
    int loadInt(const std::string& keyName, T& number){
        int num;
        int ret = loadInt(keyName, num);
        number = num;
        return ret;
    }
private:
    bool CheckKeyName(const std::string& keyName);
    int loadNumber(const std::string& keyName, double& number);
    int loadArray(const std::string& keyName, double* number, const int len);

    /************ write data into JSON ************/
public:
    std::string toString();
    JSONWrapper& addString(const std::string& keyName, const std::string& str);
    JSONWrapper& addInt(const std::string& keyName, const int& number);
    JSONWrapper& addDouble(const std::string& keyName, const double& number);
    JSONWrapper& addIntArray(const std::string& keyName, const int* number, const int len);
    JSONWrapper& addDoubleArray(const std::string& keyName, const double* number, const int len);
private:
    JSONWrapper& addJsonValue(const std::string& keyName, const QJsonValue& value);
    JSONWrapper& addJsonArray(const std::string& keyName, const QJsonArray& array);

private:
    QJsonObject rootJson;//根节点
};

#endif // JSONWRAPPER_H

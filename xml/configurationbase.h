#ifndef CONFIGURATIONBASE_H
#define CONFIGURATIONBASE_H

/* LoadNumberArray中使用assert 注意机器人轴数变化 小心数组越界 */

#include <string>

#include <QDomElement>
#include <QDomDocument>

#define SAVE_STRING_ELEMENT(str)        SaveString(doc, root, #str, str);
#define SAVE_NUMBER_ELEMENT(value)      SaveNumber(doc, root, #value, value);
#define SAVE_NUMBER_ARRAY(pValue,len)   SaveNumberArray(doc, root, #pValue, pValue, len);//pValue is a pointer

#define LOAD_NUMBER_ELEMENT(value)      LoadNumber(element, #value, value);
#define LOAD_STRING_ELEMENT(str)        LoadString(element, #str, str);
#define LOAD_NUMBER_ARRAY(pValue,len)   LoadNumberArray(element, #pValue, pValue, len);//pValue is a pointer

class ConfigurationBase
{
public:
    ConfigurationBase();
    virtual ~ConfigurationBase();

    int ReadFromFile();
    int SaveToFile();

    void SetConfFilePath(const std::string& filePath);

    virtual void LoadDefaultConfiguration() = 0;
protected:
    virtual void DoReadElement(QDomElement& element) = 0;
    virtual void DoSaveElement(QDomDocument& doc, QDomElement& root) = 0;

protected:
    bool FindElement(QDomElement& element, const char* name);
    void LoadString(QDomElement& element, const char* name, std::string& str);
    void LoadNumber(QDomElement& element, const char* name, int& value);
    void LoadNumber(QDomElement& element, const char* name, bool& value);
    void LoadNumber(QDomElement& element, const char* name, double& value);
    void LoadNumber(QDomElement& element, const char* name, unsigned int& value);
    void LoadNumberArray(QDomElement& element, const char* name, int* pValue, const int len);
    void LoadNumberArray(QDomElement& element, const char* name, double* pValue, const int len);

    void SaveString(QDomDocument& doc, QDomElement& root, const char* name, const std::string& str);
    template<typename T> void SaveNumber(QDomDocument& doc, QDomElement& root, const char* name, const T& value){
        QDomElement element = doc.createElement(name);
        element.setAttribute(name, value);
        root.appendChild(element);
    }
    template<typename T> void SaveNumberArray(QDomDocument& doc, QDomElement& root, const char* name, const T* pValue, const int len){
        QDomElement element = doc.createElement(name);
        for(int i=0; i<len; ++i){
            QString childName = arrayPrefix + QString::number(i);
            QDomElement childElement = doc.createElement(childName);
            childElement.setAttribute(childName, pValue[i]);
            element.appendChild(childElement);
        }
        root.appendChild(element);
    }

private:
    const QString arrayPrefix;
    const int arrayPrefixLen;

    std::string confFilePath;
};

#endif // CONFIGURATIONBASE_H

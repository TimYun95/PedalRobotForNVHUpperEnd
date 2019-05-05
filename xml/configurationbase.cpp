#include "configurationbase.h"

#include <cassert>

#include <QFile>

#include "common/printf.h"

ConfigurationBase::ConfigurationBase()
    : arrayPrefix("item"),
      arrayPrefixLen(arrayPrefix.size())
{

}

ConfigurationBase::~ConfigurationBase()
{

}

int ConfigurationBase::ReadFromFile()
{
    QFile file(confFilePath.c_str());
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        PRINTF(LOG_ERR, "%s: cannot open %s, reading default configuration...\n", __func__, confFilePath.c_str());
        LoadDefaultConfiguration();
        return SaveToFile();
    }

    QDomDocument doc;
    if(!doc.setContent(&file)){
        PRINTF(LOG_WARNING, "%s: QDomDocument cannot set content to %s.\n", __func__, confFilePath.c_str());
        file.close();
        return -1;
    }

    QDomElement element = doc.documentElement();
    for(QDomNode n = element.firstChild(); n.isNull()==false; n = n.nextSibling()){
        QDomElement e = n.toElement();
        if(e.isNull() == false){
            DoReadElement(e);
        }
    }
    file.close();
    return 0;
}

int ConfigurationBase::SaveToFile()
{
    QDomDocument doc;
    QDomNode instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.insertBefore(instruction, doc.firstChild());

    QDomElement root = doc.createElement("Settings");
    DoSaveElement(doc, root);
    doc.appendChild(root);

    QFile file(confFilePath.c_str());
    if (!file.open(QIODevice::WriteOnly)){
        PRINTF(LOG_INFO, "%s: cannot open %s to write configutation.\n", __func__, confFilePath.c_str());
        return -1;
    }
    file.write(doc.toString().toLocal8Bit().data());
    file.close();

    return 0;
}

void ConfigurationBase::SetConfFilePath(const std::string &filePath)
{
    confFilePath = filePath;
}

bool ConfigurationBase::FindElement(QDomElement &element, const char *name)
{
    return element.tagName() == QString(name);
}

void ConfigurationBase::LoadString(QDomElement &element, const char *name, std::string &str)
{
    if(FindElement(element, name)){
        str = element.attribute(name).toStdString();
    }
}

void ConfigurationBase::LoadNumber(QDomElement &element, const char *name, int &value)
{
    if(FindElement(element, name)){
        value = element.attribute(name).toInt();
    }
}

void ConfigurationBase::LoadNumber(QDomElement &element, const char *name, bool &value)
{
    if(FindElement(element, name)){
        value = element.attribute(name).toInt();
    }
}

void ConfigurationBase::LoadNumber(QDomElement &element, const char *name, double &value)
{
    if(FindElement(element, name)){
        value = element.attribute(name).toDouble();
    }
}

void ConfigurationBase::LoadNumber(QDomElement &element, const char *name, unsigned int &value)
{
    if(FindElement(element, name)){
        value = element.attribute(name).toUInt();
    }
}

void ConfigurationBase::LoadNumberArray(QDomElement &element, const char *name, int *pValue, const int len)
{
    if(FindElement(element, name)){
        for(QDomNode childNode = element.firstChild(); childNode.isNull()==false; childNode = childNode.nextSibling()){
            QDomElement childElement = childNode.toElement();
            if(childElement.isNull() == false){
                int index = childElement.tagName().mid(arrayPrefixLen).toInt();//索引
                assert(index < len);//配置文件item个数过多
                pValue[index] = childElement.attribute(childElement.tagName()).toInt();
            }
        }
    }
}

void ConfigurationBase::LoadNumberArray(QDomElement &element, const char *name, double *pValue, const int len)
{
    if(FindElement(element, name)){
        for(QDomNode childNode = element.firstChild(); childNode.isNull()==false; childNode = childNode.nextSibling()){
            QDomElement childElement = childNode.toElement();
            if(childElement.isNull() == false){
                int index = childElement.tagName().mid(arrayPrefixLen).toInt();//索引
                assert(index < len);//配置文件item个数过多
                pValue[index] = childElement.attribute(childElement.tagName()).toDouble();
            }
        }
    }
}

void ConfigurationBase::SaveString(QDomDocument& doc, QDomElement& root, const char* name, const std::string& str)
{
    QDomElement element = doc.createElement(name);
    element.setAttribute(name, str.c_str());
    root.appendChild(element);
}

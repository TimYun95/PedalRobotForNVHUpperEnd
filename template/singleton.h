#ifndef SINGLETON_H
#define SINGLETON_H

#include <cassert>

template<typename T>
class Singleton
{
protected:
    Singleton(){}
    virtual ~Singleton(){}

public:
    static void CreateInstance(){
        if(m_instance == nullptr){
            m_instance = new T();
        }
    }

    static T* Instance(){
        assert(m_instance != nullptr);
        return m_instance;
    }

    static void DestroyInstance(){
        if(m_instance != nullptr){
            delete m_instance;
            m_instance = nullptr;
        }
    }

private:
    static T* m_instance;
};

template<typename T> T* Singleton<T>::m_instance = nullptr;

#endif // SINGLETON_H

#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <memory>

template <typename T>
class Singleton
{
public:
    static T* Instance()
    {
        static std::unique_ptr<T> instance;
        if (instance == nullptr)
        {
            instance.reset(new T);
        }
        return instance.get();
    }

protected:
    Singleton() {}
    virtual ~Singleton() {}
};

#endif

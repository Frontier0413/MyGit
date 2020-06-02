#include <memory>
#include <map>
#include <iostream>
#include <vector>
#include <mutex>
using namespace std;

class myClass
{
public:
    union myVec
    {
        int val;
        vector<myClass> vec;
    };

    bool flag = true;
    myVec data;
};


void turnToVec(const vector<myClass>& m, vector<int>& result)
{
    for (int i = 0; i < m.size(); ++i)
    {
        if (m[i].flag) result.push_back(m[i].data.val);
        else turnToVec(m[i].data.vec, result);
    }
}

class RWLock
{
public:
    void Rlock()
    {
        mutexForUpdate.lock();
        ++reading;
        if (!reading) mutexForW.lock();
        mutexForUpdate.unlock();
    }

    void Wlock()
    {
        if (reading == 0)
        {
            mutexForW.lock();
        }
    }

    void Runlock()
    {
        mutexForUpdate.lock();
        --reading;
        if (!reading) mutexForW.unlock();
        mutexForUpdate.unlock();
    }

    void Wunlock()
    {
        mutexForW.unlock();
    }

private:
    int reading = 0;
    mutex mutexForUpdate;
    mutex mutexForW;
};
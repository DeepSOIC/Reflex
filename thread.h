#ifndef THREAD_H
#define THREAD_H

#include <Arduino.h>

class Thread;
class Host;

class Thread
{
public: //members
    enum eFlags {
        EF_SUSPEND = 1 << 0,
    };

    byte flags;

    Host* host;

public:
    virtual void loop() = 0;

    byte getFlag(eFlags flag) {
        return flags & flag == flag;
    }
    virtual byte event(byte event, byte param1, byte param2){return 0;}

    virtual void attach(Host* host, byte index){
        this->host = host;
    }
};

class Host: public Thread
{
public:
    static const byte MAX_THREADS = 8;
    Thread* threads[MAX_THREADS] = 0;
    byte n_threads = 0;

    virtual void loop() {
        for(byte i = 0; i < n_threads; ++i){
            if (!(threads[i]->getFlag(EF_SUSPEND)))
                threads[i]->loop();
        }
    }

    virtual byte event(byte event, byte param1, byte param2){
        byte ret;
        for(byte i = 0; i < n_threads; ++i){
            if (!(threads[i]->getFlag(EF_SUSPEND))){
                ret = threads[i]->event(event, param1, param2);
                if (ret)
                    return ret;
            }
        }
        return 0;
    }

    byte add(Thread* t){
        if(n_threads == MAX_THREADS)
            return 0;
        threads[n_threads] = t;
        t->attach(this, n_threads);
        ++n_threads;
        return 1;
    }

    byte fireEvent(byte event, byte param1, byte param2){
        if (this->host)
            return host->fireEvent(event, param1, param2);
        else
            return host->event(event, param1, param2);
    }
};

#endif // THREAD


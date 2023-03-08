#ifndef COSWAVES_H_INCLUDED
#define COSWAVES_H_INCLUDED

#include <sstream>
#include <string>

class wave {
    private:
        unsigned int cycleNumber;
        unsigned int period;
        int amplitude;
        unsigned int midpoint;
        bool direction;//true for up, false for down


    public:
        unsigned int startValue;
        unsigned int stopValue;
        void InitWave(unsigned int cycleNumber, unsigned int& l, unsigned int lastEndValue);
        unsigned int startClick;
        unsigned int stopClick;
        int GetPeriod();
        int GetAmp();
        int GetMid();
        bool GetDir();

        wave();
};


#endif // SINEWAVES_H_INCLUDED

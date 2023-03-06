#ifndef COSWAVES_H_INCLUDED
#define COSWAVES_H_INCLUDED

#include <sstream>
#include <string>

using namespace std;

class wave
{
private:

    unsigned int cyclenumber;
    unsigned int period;
    int amplitude;
    unsigned int midpoint;
    bool direction;//true for up, false for down


public:

    unsigned int startvalue;
    unsigned int stopvalue;
    void InitWave(unsigned int i, unsigned int&l, unsigned int lastendvalue);
    unsigned int startclick;
    unsigned int stopclick;
    int GetPeriod();
    int GetAmp();
    int GetMid();
    bool GetDir();

    wave();
};


#endif // SINEWAVES_H_INCLUDED

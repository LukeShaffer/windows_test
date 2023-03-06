#include "CosWaves.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

#define PI 3.14159265


wave::wave() {
    srand(time(0));
    period = 1;
    amplitude = 1;
    cycleNumber = 0;
    midpoint = 1;
    direction = true;
    startClick = 0;
    stopClick = 1;

}


void wave::InitWave(unsigned int cycleNumber, unsigned int& l, unsigned int lastEndValue) {
    /*
        Create the cosine wave structure to model my clicks.

        I need to define how long I want this wave to be (number of Clicks) as
        well as the maximum and minimum durations of each click in the wave.

        Params:
        cycleNumber - the number cycle that this wave makes up in the larger
            collection.
        l - The number of total waves in the total collection (length),
            this is a side effect and will be modified!
        lastEndValue - The value of the last click in the wave up to this point
            used to calculate new starting positions.
        
    */

    // first wave
    if (l == 0) {
        startClick = 0;
        startValue = (rand() % 100) + 420;
    }
    else if (l != 0) {
        startClick = l + 1;
        startValue = lastEndValue;
    }

    //old was rand() %20 +5
    amplitude = (rand() % 10) + 15;

    //even numbered waves start at the top with a midpoint below them
    if (cycleNumber % 2 == 0) {
        direction = true;

        stopValue = startValue - (2 * amplitude);
        midpoint = startValue - amplitude;
        while(midpoint < 300) {
            amplitude = (rand() % 10) + 15;
            stopValue = startValue - (2 * amplitude);
            midpoint = startValue - amplitude;
            // catch if the start value is past the minimum bound
            if(startValue <= 325) startValue = 326;    //stops infinite loop
        }
    }

    // odd numbered waves start at the bottom with a midpoint above them.
    else if (cycleNumber%2 ==1) {
        direction = false;

        stopValue = startValue + (2 * amplitude);
        midpoint = startValue + amplitude;
        while (midpoint > 500) {
            amplitude = (rand() % 10) + 15;
            stopValue = startValue + (2 * amplitude);
            midpoint = startValue + amplitude;

            // catch the maximum bound
            if(startValue >= 475) startValue = 474;    //stops infinite loop
        }

    }

    period = (rand() % 75 + 20);
    if (period % 2 != 0) {
        period-=1;
    }

    l += (period / 2);
    stopClick = startClick+(period/2);
}

int wave::GetAmp()
{
    return amplitude;
}

int wave::GetMid()
{
    return midpoint;
}

bool wave::GetDir()
{
    return direction;
}

int wave::GetPeriod()
{
    return period;
}

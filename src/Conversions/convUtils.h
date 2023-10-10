#ifndef CONVERSIONUTILS_H
#define CONVERSIONUTILS_H

static inline double mAmpsToAmps(double mA)
{
    return mA/1000;
}

static inline double ampsTomAmps(double A)
{
    return A*1000;
}

#endif
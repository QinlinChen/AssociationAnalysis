#include "Stopwatch.h"

std::ostream &operator<<(std::ostream &os, const Stopwatch &timer)
{
    os << timer.elapsedTime() << " seconds";
    return os;
}

#include <vector>

typedef struct EmailAlert 
{
    char emailSent;
    char emailNotSent;
};
 
typedef struct LEDAlert 
{
    char ledGlows;
    char ledOff;
};
namespace Statistics {
    auto ComputeStatistics(const std::vector<int*>& alerters );
}

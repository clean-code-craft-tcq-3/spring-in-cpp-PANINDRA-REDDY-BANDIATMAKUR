#include <vector>

struct EmailAlert 
{
    char emailSent;
    char emailNotSent;
};
 
struct LEDAlert 
{
    char ledGlows;
    char ledOff;
};
namespace Statistics {
    auto ComputeStatistics(const std::vector<int*>& alerters );
}

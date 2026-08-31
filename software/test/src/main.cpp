#include "common/ads1115_driver/Tests.h"
#include "flight_controller/Tests.h"

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
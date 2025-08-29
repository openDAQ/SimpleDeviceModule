#include <gtest/gtest.h>
#include <coreobjects/util.h>

int main(int argc, char** args)
{
    testing::InitGoogleTest(&argc, args);

    auto res = RUN_ALL_TESTS();

    return  res;
}

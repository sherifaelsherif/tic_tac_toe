#include "gtest/gtest.h"
#include "../math_utils.h"

TEST(MathUtilsTest, AddTest) {
    EXPECT_EQ(add(2, 3), 5);
    EXPECT_EQ(add(-1, 1), 0);
    EXPECT_EQ(add(0, 0), 0);
}

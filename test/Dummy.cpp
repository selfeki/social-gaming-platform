
#include "arepa/game_spec/ExpressionPtr.h"
#include "gtest/gtest.h"

using namespace gameSpecification;

namespace {

class sampleTests : public testing::Test { };

// sample test to check if module libraries link successfully

TEST_F(sampleTests, test1) {
  ExpressionPtr expPtr({"test"});
  EXPECT_TRUE(true);
}

}  // namespace
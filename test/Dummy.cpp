
#include "arepa/game_spec/ExpressionPtr.h"
#include "arepa/game_spec/Expression.h"
#include "gtest/gtest.h"

using namespace gameSpecification;

namespace {

class sampleTests : public testing::Test { };

// sample test to check if module libraries link successfully

TEST_F(sampleTests, test1) {
  Expression exp1 = ExpMap({ { { "c", 1 } } });
  Expression exp2 = ExpMap({ { { "b", exp1 } } });
  Expression exp3 = ExpMap({ { { "a", exp2 } } });
  ExpMap mapCopy = castExpUnsafe<ExpMap>(exp3);
  ExpMap& mapRef = castExpUnsafe<ExpMap>(exp3);

  ExpressionPtr expPtr { { "a", "b", "c" } };

  auto ptrCopy = expPtr.getPtr(mapCopy);
  auto ptrRef = expPtr.getPtr(mapRef);
    
  if (!ptrCopy) {
      std::cout << "null";
  } else {
      expPtr.store(42, mapCopy);
      // exp3 = { a: { b: { c: 1, }, }, }
      boost::apply_visitor(printExpVisitor(), exp3);
  }
  std::cout << std::endl;

  if (!ptrRef) {
      std::cout << "null";
  } else {
      expPtr.store(42, mapRef);
      // exp3 = { a: { b: { c: 42, }, }, }
      boost::apply_visitor(printExpVisitor(), exp3);
  }

  EXPECT_TRUE(true);
}

}  // namespace
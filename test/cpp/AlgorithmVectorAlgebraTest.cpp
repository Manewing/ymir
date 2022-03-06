#include <gtest/gtest.h>
#include <ymir/Algorithm/VectorAlgebra.hpp>

namespace {

TEST(AlgorithmVectorAlgebra, ScalarAdd) {
  std::vector<float> Values = {0, 1, 2, 3, 4};
  auto WV = ymir::Algorithm::algebra(Values);
  WV += 5;
  std::vector<float> RefValues = {5, 6, 7, 8, 9};
  EXPECT_EQ(Values, RefValues);
}

TEST(AlgorithmVectorAlgebra, ScalarIteratorAdd) {
  std::vector<float> Values = {0, 1, 2, 3, 4};
  ymir::Algorithm::internal::VectorScalarIterator<float, std::plus<float>>
      ItBegin(Values.begin(), 5);
  ymir::Algorithm::internal::VectorScalarIterator<float, std::plus<float>>
      ItEnd(Values.end());

  EXPECT_EQ(*ItBegin, 5.0f);
  EXPECT_EQ(ItBegin, ItBegin);
  EXPECT_NE(ItBegin, ItEnd);
  std::copy(ItBegin, ItEnd, Values.begin());

  std::vector<float> RefValues = {5, 6, 7, 8, 9};
  EXPECT_EQ(Values, RefValues);
}

TEST(AlgorithmVectorAlgebra, AlgebraView) {
  std::vector<float> Values = {0, 1, 2, 3, 4};
  std::vector<float> NewValues = {0, 0, 0, 0, 0};
  auto ValuesW = ymir::Algorithm::algebra(Values);
  auto NewValuesW = ymir::Algorithm::algebra(NewValues);
  NewValuesW = ValuesW / 2.0f;
  std::vector<float> RefValues = {0, 0.5, 1.0, 1.5, 2.0};
  EXPECT_EQ(NewValues, RefValues);
}

TEST(AlgorithmVectorAlgebra, AlgebraWrapperAssignmentOperators) {
  std::vector<int> Values = {0, 1, 2, 3, 4};
  auto ValuesW = ymir::Algorithm::algebra(Values);

  {
    ValuesW *= 2;
    std::vector<int> RefValues = {0, 2, 4, 6, 8};
    EXPECT_EQ(Values, RefValues);
  }

  {
    ValuesW /= 2;
    std::vector<int> RefValues = {0, 1, 2, 3, 4};
    EXPECT_EQ(Values, RefValues);
  }

  {
    ValuesW += 2;
    std::vector<int> RefValues = {2, 3, 4, 5, 6};
    EXPECT_EQ(Values, RefValues);
  }

  {
    ValuesW -= 2;
    std::vector<int> RefValues = {0, 1, 2, 3, 4};
    EXPECT_EQ(Values, RefValues);
  }
}

TEST(AlgorithmVectorAlgebra, AlgebraWrapperBinaryOperators) {
  const std::vector<int> ValuesA = {0, 1, 2, 3, 4};
  const std::vector<int> ValuesB = {5, 6, 7, 8, 9};
  std::vector<int> ValuesC = {0, 0, 0, 0, 0};
  auto ValuesAW = ymir::Algorithm::algebra(ValuesA);
  auto ValuesBW = ymir::Algorithm::algebra(ValuesB);
  auto ValuesCW = ymir::Algorithm::algebra(ValuesC);

  // AlgebraWrapper binary operators: (AW, AW)
  {
    ValuesCW = ValuesAW + ValuesBW;
    std::vector<int> RefValues = {5, 7, 9, 11, 13};
    EXPECT_EQ(ValuesC, RefValues);
  }

  {
    ValuesCW = ValuesAW - ValuesBW;
    std::vector<int> RefValues = {-5, -5, -5, -5, -5};
    EXPECT_EQ(ValuesC, RefValues);
  }

  {
    ValuesCW = ValuesAW * ValuesBW;
    std::vector<int> RefValues = {0, 6, 14, 24, 36};
    EXPECT_EQ(ValuesC, RefValues);
  }

  {
    ValuesCW = ValuesAW / ValuesBW;
    std::vector<int> RefValues = {0, 0, 0, 0, 0};
    EXPECT_EQ(ValuesC, RefValues);
  }

  // AlgebraWrapper binary operators: (AW, Value), (Value, AW)
  {
    ValuesCW = (ValuesAW + 5) + (1 + ValuesBW);
    std::vector<int> RefValues = {11, 13, 15, 17, 19};
    EXPECT_EQ(ValuesC, RefValues);
  }

  {
    ValuesCW = (ValuesAW * 2) + (-1 * ValuesBW);
    std::vector<int> RefValues = {-5, -4, -3, -2, -1};
    EXPECT_EQ(ValuesC, RefValues);
  }

  {
    ValuesCW = (ValuesAW - 5) + (5 - ValuesBW);
    std::vector<int> RefValues = {-5, -5, -5, -5, -5};
    EXPECT_EQ(ValuesC, RefValues);
  }

  {
    ValuesCW = (ValuesAW / 2) + (10 / ValuesBW);
    std::vector<int> RefValues = {2, 1, 2, 2, 3};
    EXPECT_EQ(ValuesC, RefValues);
  }
}

TEST(AlgorithmVectorAlgebra, AlgebraViewBinaryOperators) {
  const std::vector<int> ValuesA = {0, 1, 2, 3, 4};
  const std::vector<int> ValuesB = {5, 6, 7, 8, 9};
  std::vector<int> ValuesC = {0, 0, 0, 0, 0};
  auto ValuesAW = ymir::Algorithm::algebra(ValuesA);
  auto ValuesBW = ymir::Algorithm::algebra(ValuesB);
  auto ValuesCW = ymir::Algorithm::algebra(ValuesC);

  // AlgebraView binary operators (AV, AV), (AV, Value), (Value, AV)
  {
    ValuesCW = ((ValuesAW * 1) + 5) + (5 + (ValuesBW * 1));
    std::vector<int> RefValues = {15, 17, 19, 21, 23};
    EXPECT_EQ(ValuesC, RefValues);
  }

  {
    ValuesCW = ((ValuesAW * 1) - 5) + (5 - (ValuesBW * 1));
    std::vector<int> RefValues = {-5, -5, -5, -5, -5};
    EXPECT_EQ(ValuesC, RefValues);
  }

  {
    ValuesCW = ((ValuesAW * 1) * 5) + (5 * (ValuesBW * 1));
    std::vector<int> RefValues = {25, 35, 45, 55, 65};
    EXPECT_EQ(ValuesC, RefValues);
  }

  {
    ValuesCW = ((ValuesAW * 1) / 5) + (5 / (ValuesBW * 1));
    std::vector<int> RefValues = {1, 0, 0, 0, 0};
    EXPECT_EQ(ValuesC, RefValues);
  }
}

TEST(AlgorithmVectorAlgebra, ComplexEquation) {
  const std::vector<int> ValuesA = {0, 1, 2, 3, 4};
  const std::vector<int> ValuesB = {5, 6, 7, 8, 9};
  std::vector<int> ValuesC = {0, 0, 0, 0, 0};
  auto [VA, VB, VC] = ymir::Algorithm::algebra(ValuesA, ValuesB, ValuesC);
  VC = VA / 2 + VB * 3 + 5;
  std::vector<int> RefValues = {20, 23, 27, 30, 34};
  EXPECT_EQ(ValuesC, RefValues);
}

} // namespace
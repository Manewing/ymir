#include "TestHelpers.hpp"
#include <gtest/gtest.h>
#include <ymir/Types.hpp>

using namespace ymir;

namespace {

TEST(Size2dTest, Comparison) {
  Size2d<int> A{0, 0}, B{1, 0};
  EXPECT_EQ(A, A);
  EXPECT_NE(A, B);

  Size2d<float> C{0, 0}, D{0.2, 0};
  EXPECT_EQ(C, C);
  EXPECT_NE(C, D);
}

TEST(Size2dTest, Operators) {
  EXPECT_EQ(Size2d<int>(1, 4) + Size2d<int>(0, 5), Size2d<int>(1, 9));
  {
    Size2d<int> A(42, 3);
    A += Size2d<int>(1, 3);
    EXPECT_EQ(A, Size2d<int>(43, 6));
  }
}

TEST(Point2dTest, Comparison) {
  Point2d<int> A{3, 3}, B{4, 4};
  EXPECT_EQ(A, A);
  EXPECT_NE(A, B);

  Point2d<float> C{-1.5, 0}, D{0.2, 0};
  EXPECT_EQ(C, C);
  EXPECT_NE(C, D);
}

TEST(Point2dTest, Methods) {
  {
    Point2d<int> A{-4, 1};
    Point2d<int> AbsA{4, 1};
    EXPECT_EQ(A.abs(), AbsA);

    Point2d<int> PositiveA{0, 1};
    EXPECT_EQ(A.positive(), PositiveA);
  }

  {
    Point2d<float> A{-4, 1};
    Point2d<float> AbsA{4, 1};
    EXPECT_EQ(A.abs(), AbsA);

    Point2d<float> PositiveA{0, 1};
    EXPECT_EQ(A.positive(), PositiveA);
  }
}

TEST(Point2dTest, Operators) {
  {
    Point2d<int> A{1, 8}, B{1, -4};
    Size2d<int> S{3, 3};

    EXPECT_EQ(A + B, (Point2d<int>{2, 4}));
    EXPECT_EQ(A - B, (Point2d<int>{0, 12}));
    EXPECT_EQ(A + S, (Point2d<int>{4, 11}));
    EXPECT_EQ(S + A, (Point2d<int>{4, 11}));
    EXPECT_EQ(A - S, (Point2d<int>{-2, 5}));
  }

  {
    Point2d<float> A{1, 8}, B{1, -4};
    Size2d<float> S{3, 3};

    EXPECT_EQ(A + B, (Point2d<float>{2, 4}));
    EXPECT_EQ(A - B, (Point2d<float>{0, 12}));
    EXPECT_EQ(A + S, (Point2d<float>{4, 11}));
    EXPECT_EQ(S + A, (Point2d<float>{4, 11}));
    EXPECT_EQ(A - S, (Point2d<float>{-2, 5}));
  }
}

TEST(Rect2dTest, Comparison) {
  {
    Rect2d<int> A{{0, 0}, {10, 10}}, B{{5, 5}, {10, 10}}, C{{0, 0}, {1, 1}};

    EXPECT_EQ(A, A);
    EXPECT_NE(A, B);
    EXPECT_NE(A, C);
    EXPECT_NE(B, C);
  }
  {
    Rect2d<float> A{{0, 0}, {10, 10}}, B{{5, 5}, {8, 8}}, C{{0, 0}, {0.1, 1}};

    EXPECT_EQ(A, A);
    EXPECT_NE(A, B);
    EXPECT_NE(A, C);
    EXPECT_NE(B, C);
  }
}

TEST(Rect2dTest, Methods) {
  // Test get
  {
    EXPECT_EQ(Rect2d<int>::get({6, 7}, {1, 4}), Rect2d<int>({1, 4}, {5, 3}));
    EXPECT_EQ(Rect2d<int>::get({1, 4}, {6, 7}), Rect2d<int>({1, 4}, {5, 3}));
    EXPECT_EQ(Rect2d<int>::get({10, 5}, {6, 7}), Rect2d<int>({6, 5}, {4, 2}));
    EXPECT_EQ(Rect2d<int>::get({4, 4}, {4, 2}), Rect2d<int>({4, 2}, {0, 2}));
    EXPECT_EQ(Rect2d<int>::get({0, 0}, {0, 0}), Rect2d<int>({0, 0}, {0, 0}));
    EXPECT_EQ(Rect2d<int>::get({1, 1}, {1, 1}), Rect2d<int>({1, 1}, {0, 0}));
    EXPECT_EQ(Rect2d<float>::get({1, 3}, {6, 3}),
              Rect2d<float>({1, 3}, {5, 0}));
    EXPECT_EQ(Rect2d<float>::get({28, 16}, {11, 16}),
              Rect2d<float>({11, 16}, {17, 0}));
  }

  // Test empty()
  {
    Rect2d<int> A{{0, 0}, {10, 0}};
    Rect2d<float> B{{5, 5}, {0, 8}};
    Rect2d<int> C{{0, 0}, {1, 1}};
    Rect2d<int> D;
    Rect2d<float> E{{1, -5}, {0.1, 0.13}};
    EXPECT_TRUE(A.empty());
    EXPECT_TRUE(B.empty());
    EXPECT_FALSE(C.empty());
    EXPECT_TRUE(D.empty());
    EXPECT_FALSE(E.empty());
  }

  // Test contains point
  {
    Rect2d<int> A{{0, 0}, {10, 10}};
    EXPECT_TRUE(A.contains(Point2d<int>{5, 5}));
    EXPECT_FALSE(A.contains(Point2d<int>{10, 10}));

    Rect2d<float> B{{0, 0}, {10, 10}};
    EXPECT_TRUE(B.contains(Point2d<float>{5, 5}));
    EXPECT_TRUE(B.contains(Point2d<float>{9.9, 9.9}));
    EXPECT_FALSE(B.contains(Point2d<float>{10, 10}));

    Rect2d<int> C{{-9, -9}, {1, 1}};
    EXPECT_TRUE(C.contains(Point2d<int>{-9, -9}));
    EXPECT_FALSE(C.contains(Point2d<int>{-8, -8}));

    Rect2d<int> D{{-5, 0}, {4, 1}};
    EXPECT_TRUE(D.contains({-2, 0}));
    EXPECT_FALSE(D.contains({-2, 1}));

    Rect2d<float> E{{-5, 0}, {4, 0}};
    EXPECT_FALSE(E.contains(Point2d<float>{-5, 0}));
    EXPECT_FALSE(E.contains(Point2d<float>{-2, 1}));
  }

  // Test contains rect
  {
    Rect2d<int> A{{0, 0}, {10, 10}};

    EXPECT_TRUE(A.contains(A)) << "Rect contains itself";

    EXPECT_TRUE(A.contains(Rect2d<int>{{5, 5}, {5, 5}}))
        << "Rect contains rect that align with border";

    EXPECT_TRUE(A.contains(Rect2d<int>{{5, 5}, {3, 3}}));
    EXPECT_FALSE(A.contains(Rect2d<int>{{5, 5}, {5, 6}}));

    Rect2d<float> B{{0, 0}, {10, 10}};
    EXPECT_TRUE(B.contains(B)) << "Rect contains itself";

    EXPECT_TRUE(B.contains(Rect2d<float>{{5, 5}, {5, 5}}))
        << "Rect contains rect that align with border";

    EXPECT_TRUE(B.contains({{5, 4.7}, {3.3, 3.3}}));
    EXPECT_FALSE(B.contains(Rect2d<float>{{5, 5}, {5.1, 5.0}}));
  }
}

TEST(Rect2dTest, Operators) {
  // intersection op: &
  {
    Rect2d<int> A{{4, 4}, {3, 3}};
    Rect2d<int> B{{-1, -1}, {50, 50}};
    Rect2d<int> C{{-2, 3}, {6, 4}};

    EXPECT_EQ(A & A, A) << "Rect intersection with iteself should yield itself";
    EXPECT_EQ(A & B, A) << "Should be completely contained";
    EXPECT_EQ(A & C, Rect2d<int>({0, 0}, {0, 0}));
    EXPECT_EQ(B & C, Rect2d<int>({-1, 3}, {5, 4}));

    Rect2d<float> E{{4, 4}, {3, 3}};
    Rect2d<float> F{{-1.2, -1.3}, {50, 50}};
    Rect2d<float> G{{-2.1, 3.3}, {6, 4}};

    EXPECT_EQ(E & E, E) << "Rect intersection with iteself should yield itself";
    EXPECT_EQ(E & F, E) << "Should be completely contained";
    EXPECT_EQ(E & G, Rect2d<float>({0, 0}, {0, 0}));
    EXPECT_FALSE((F & G).empty());
  }
}

TEST(Dir2dTest, Operators) {
  EXPECT_EQ(Dir2d::NONE | Dir2d::UP, 0x2);
  EXPECT_EQ(Dir2d() | Dir2d::UP, 0x2);
  EXPECT_EQ(Dir2d::RIGHT | Dir2d::UP, 0x6);
  EXPECT_EQ(Dir2d::RIGHT & Dir2d::UP, Dir2d::NONE);

  Dir2d A = Dir2d::UP;
  A |= Dir2d(Dir2d::LEFT);
  EXPECT_EQ(A, 0x0A);
}

TEST(Dir2dTest, Methods) {
  Dir2d Dir;
  EXPECT_EQ(Dir, Dir2d::NONE) << "Should always initialize to NONE";
  EXPECT_EQ(Dir2d(Dir2d::NONE).opposing(), Dir2d::NONE);
  EXPECT_EQ(Dir2d(Dir2d::RIGHT).opposing(), Dir2d::LEFT);
  EXPECT_EQ(Dir2d(Dir2d::DOWN).opposing(), Dir2d::UP);

  EXPECT_EQ(Dir2d(Dir2d::DOWN).advance<int>(), Point2d<int>(0, 1));
}

TEST(Dir2dTest, Output) {
  {
    Dir2d Dir = Dir2d::NONE;
    EXPECT_EQ(Dir.str(), "NONE");
    EXPECT_EQ(dump(Dir), "Dir2d(NONE)");
  }
  {
    Dir2d Dir = Dir2d::VERTICAL;
    EXPECT_EQ(Dir.str(), "VERTICAL");
    EXPECT_EQ(dump(Dir), "Dir2d(VERTICAL)");
  }
  {
    Dir2d Dir = Dir2d::LEFT;
    EXPECT_EQ(Dir.str(), "LEFT");
    EXPECT_EQ(dump(Dir), "Dir2d(LEFT)");
  }
  {
    Dir2d Dir = Dir2d::RIGHT | Dir2d::UP | Dir2d::DOWN;
    EXPECT_EQ(Dir.str(), "UP | DOWN | RIGHT");
    EXPECT_EQ(dump(Dir), "Dir2d(UP | DOWN | RIGHT)");
  }
}

} // namespace
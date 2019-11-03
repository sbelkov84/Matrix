#include "matrix.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <string>
//------------------------------------------------------------------------

TEST(Matrix, On10x10)
{
  std::vector<std::string> Printed;
  TMatrix<int, 0> Matrix;
  
  int Value = 1;
  for (std::size_t i = 0; i < 10; ++i)
  {
    for (std::size_t j = 0; j < 10; ++j)
    {
      Matrix[i][j] = Value;
      ++Value;
    }
  }

  Value = 1;
  for (std::size_t i = 0; i < 10; ++i)
  {
    for (std::size_t j = 0; j < 10; ++j)
    {
      EXPECT_EQ(Matrix[i][j], Value);
      ++Value;
    }
  }  
}
//------------------------------------------------------------------------

TEST(Matrix, OnRangedFor)
{
  std::string Printed;
  testing::internal::CaptureStdout();

  TMatrix<int, 0> Matrix;
  Matrix[100][100] = 314;
  
  for (auto& c : Matrix)
  {
    int x;
    int y;
    int z;
    std::tie(x, y, z) = c;
    std::cout << x << y << z << std::endl;
  }
  
  Printed = testing::internal::GetCapturedStdout();
  EXPECT_EQ(Printed, (std::string) "100100314");   
}
//------------------------------------------------------------------------

TEST(Matrix, OnSize)
{
  TMatrix<int, -1> Matrix;
  Matrix[1][1] = 11;
  Matrix[0][1] = -1;

  EXPECT_EQ(Matrix.size(), 1); 
}
//------------------------------------------------------------------------

TEST(Matrix, OnDeleteItem)
{
  TMatrix<int, -1, 3> Matrix;
  Matrix[1][1][1] = 11;
  Matrix[0][1][2] = 12;
  EXPECT_EQ(Matrix.size(), 2); 

  Matrix[0][1][2] = -1;

  EXPECT_EQ(Matrix.size(), 1); 
}
//------------------------------------------------------------------------

TEST(Matrix, On_3_Dimension)
{
  TMatrix<int, -1, 3> Matrix;
  Matrix[0][1][2] = 123;
  auto A = Matrix[0][1][2];

  EXPECT_EQ(A, 123); 

}
//------------------------------------------------------------------------

TEST(Matrix, OnDimensionExceeded)
{
  std::string Printed;
  testing::internal::CaptureStdout();

  TMatrix<int, -1, 3> Matrix;
  Matrix[0][1][2][3] = 111;
  Printed = testing::internal::GetCapturedStdout();
  EXPECT_EQ(Printed, (std::string) "Matrix dimension was exceeded"); 
}
//------------------------------------------------------------------------

TEST(Matrix, OnClassicAssignOperator)
{
  TMatrix<int, -1> Matrix;
  ((Matrix[100][100] = 314) = 0) = 217;

  EXPECT_EQ(Matrix[100][100], 217); 
}
//------------------------------------------------------------------------

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  int Res = RUN_ALL_TESTS();
  return Res;
}
//------------------------------------------------------------------------

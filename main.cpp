//------------------------------------------------------------------------
#include "matrix.h"
//------------------------------------------------------------------------
#include <iostream>
//------------------------------------------------------------------------

int main()
{
  TMatrix<int, 0> Matrix;
  std::size_t Size = 9;

  for (std::size_t i = 0; i <= Size; ++i)
  {
    Matrix[i][i] = i;
    Matrix[i][Size - i] = Size - i;
  }

  for (std::size_t i  = 1; i < Size; ++i)
  {
    for (std::size_t j = 1; j < Size; ++j)
    {
      std::cout << Matrix[i][j];
      if ((j + 1) < Size)
        std::cout << " ";
    }
    std::cout << std::endl;
  }

  std::cout << Matrix.size() << std::endl;

  return 0;
}

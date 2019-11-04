#ifndef MATRIX_H
#define MATRIX_H
//------------------------------------------------------------------------
#include <iostream>
#include <map>
#include <tuple>
//------------------------------------------------------------------------

/*! \brief Makes tuple from array of indicies of matrix element and value of element */
/*! auto ArrToTupleImpl(const TArr& Arr, std::index_sequence<I...>, U& Value) */
template <typename TArr, std::size_t... I, typename U>
auto ArrToTupleImpl(const TArr& Arr, std::index_sequence<I...>, U& Value)
{
  return std::make_tuple(Arr[I]..., Value);
}
//------------------------------------------------------------------------

/*! \brief Entry function to prepare std::array<T, N> of matrix element indicies for ArrToTupleImpl-function */
/*! auto ArrToTuple(const std::array<T, N>& Arr, U& Value) */
template <typename T, std::size_t N, typename Indicies = std::make_index_sequence<N>, typename U>
auto ArrToTuple(const std::array<T, N>& Arr, U& Value)
{
  return ArrToTupleImpl(Arr, Indicies{}, Value);
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

template <typename T, T Default, std::size_t Dimens = 2>
struct TMatrix;
//------------------------------------------------------------------------

/*! \brief Iterator-class impementation for TMatrix */
/*! class TMatrixIterator */
template <typename T, T Default, std::size_t Dim>
class TMatrixIterator
{
private:

  /*! \brief Pointer to current matrix-element or map-of-elements(/maps) */
  /*! TMatrix<T, Default, Dim>* Current */
  TMatrix<T, Default, Dim>* Current;

public:

  /*! \brief Just a constructor */ 
  /*! TMatrixIterator(TMatrix<T, Default, Dim>* Current) */
  explicit TMatrixIterator(TMatrix<T, Default, Dim>* Current) : Current(Current) {}
  
  /*! \brief In this operator we make from Iterator on matrix-element std::tuple with all indicies and value of this element */
  /*! const auto operator *() */
  const auto operator *()
  {
    /*! \brief Array of element's indicies */
    /*! std::array<int, Dim> IndArr; */
    std::array<int, Dim> IndArr;

    /*! \brief Local index to fill IndArr from last to first elements */
    /*! int Index */
    int Index = Dim - 1;

    /*! \brief Local pointer to current matrix-element or map-of-elements(/maps) */
    /*! TMatrix<T, Default, Dim>* TmpCurrent */
    TMatrix<T, Default, Dim>* TmpCurrent = Current;
    while (TmpCurrent->CurDim < Dim)
    {
      IndArr[Index] = TmpCurrent->Index;
      TmpCurrent = TmpCurrent->Parent;
      --Index;
    }

    /*! \brief Local variable to store Value of matrix element */
    /*! T Value */
    T Value = Current->Value;

    /*! \brief Result of function's working */
    /*! auto Res */
    auto Res = ArrToTuple(IndArr, Value);
    
    return Res;
  }

  /*! \brief In this operator we go to the next matrix element. If element's Value equals to Default, we would go and check next matrix element */
  /*! TMatrixIterator<T, Default, Dim>& operator ++() */
  TMatrixIterator<T, Default, Dim>& operator ++()
  {
    /*! \brief Result of function's working. Initialized with nullptr */
    /*! TMatrixIterator<T, Default, Dim> Result */
    TMatrixIterator<T, Default, Dim> Result(nullptr);
    /*! \brief Pointer to matrix-map, which includes necessary element or sub-map(s) with this element */
    /*! TMatrix<T, Default, Dim>* ResMatr */
    TMatrix<T, Default, Dim>* ResMatr;

    ResMatr = Current->FindNextNeighbor();
    while (ResMatr == nullptr && Current->Parent != nullptr)
    {
      Current = Current->Parent;
      ResMatr = Current->FindNextNeighbor();
    }
    //---
    if (ResMatr != nullptr)
      Result = ResMatr->begin();
    //---
    *this = Result;
    //---
    return *this;
  }
  //---

  /*! \brief In this operator we check matrix element iterator on inequality */
  /*! bool operator !=(const TMatrixIterator<T, Default, Dim>& Rhs) */
  bool operator !=(const TMatrixIterator<T, Default, Dim>& Rhs)
  {
    return this->Current != Rhs.Current;
  }
};
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

/*! \brief TMatrix-struct. Main struct. Includes std::maps of sub-TMatrix'es. Depth of inclusion depends on dimension (variable Dimens) */
/*! struct TMatrix */
template <typename T, T Default, std::size_t Dimens>
struct TMatrix
{
  /*! \brief Constructor. Is used to create sub-TMatrix'es */
  /*! TMatrix<T, Default, Dimens>(std::size_t Dim = Dimens, TMatrix<T, Default, Dimens>* Parent = nullptr) */
  TMatrix<T, Default, Dimens>(std::size_t Dim = Dimens, TMatrix<T, Default, Dimens>* Parent = nullptr)
  {
    this->Value = Default;
    this->CurDim = Dim;
    this->IsEmpty = true;
    this->Parent = Parent;
  }
  //---

  /*! \brief Map of sub-TMatrix'es */
  /*! std::map<int, TMatrix<T, Default, Dimens>> Storage */
  std::map<int, TMatrix<T, Default, Dimens>> Storage;

  /*! \brief Value of matrix element. Is needed only for lowest level of sub-TMatrix'es */
  /*! T Value */
  T Value;
  /*! \brief Indicates if element is empty. Is added, mostly, for future */
  /*! bool IsEmpty */
  bool IsEmpty;
  /*! \brief Indicates depth-level of sub-TMatrix. Is used, mostly, in []-operator */
  /*! std::size_t CurDim */
  std::size_t CurDim;
  /*! \brief Pointer to parent TMatrix */
  /*! TMatrix<T, Default, Dimens>* Parent */
  TMatrix<T, Default, Dimens>* Parent;
  /*! \brief In fact double of It->first for Parent->Storage. But is necessary, i.e. to go upward from lower sub-TMatrix */
  /*! int Index */
  int Index;

  /*! \brief This operator returns sub-TMatrix corresponding to Index */
  /*! TMatrix<T, Default, Dimens>& operator [](int Ind) */
  TMatrix<T, Default, Dimens>& operator [](int Ind)
  {
    try
    {
      if (CurDim < 1)
        throw ("Matrix dimension was exceeded");
    }
    catch (const char* ExcptStr)
    {
      std::cout << ExcptStr;
    }

    /*! \brief Finding corresponding iterator in Storage's std::map */
    /*! auto It */
    auto It = Storage.find(Ind);

    if (It == Storage.end())
    {
      Storage[Ind] = TMatrix<T, Default, Dimens>(CurDim - 1, this);
      Storage[Ind].Index = Ind;
    }

    return Storage[Ind];
  }
  //---

  /*! \brief This operator assigns value to matrix element. 
   * If Rhs == Default: deletes element, recursive checks Parent->Storage(s), and if those became empty, deletes them */
  /*! TMatrix<T, Default, Dimens>& operator =(const T& Rhs) */
  TMatrix<T, Default, Dimens>& operator =(const T& Rhs)
  {
    /*! \brief Result of operator working */
    /*! TMatrix<T, Default, Dimens>* Result */
    TMatrix<T, Default, Dimens>* Result = this;

    if (Rhs == Default)
    {
      Result = Parent;
      /*! \brief Index of matrix element. If recusion starts, will become index of parent sub-TMatrix'es */
      /*! int TmpIndex */
      int TmpIndex = Index;
      while (Result != nullptr)
      {
        /*! \brief Iterator of element/sub-TMatrix in Parent->Storage */
        /*! auto It */
        auto It = Result->Storage.find(TmpIndex);
        if (It != Result->Storage.end())
          Result->Storage.erase(It);
        //---
        if (Result->Storage.empty())
        {
          TmpIndex = Result->Index;
          Result = Result->Parent;
        }
        else
        {
          break;
        }
      }
      return *Result;
    }

    this->Value = Rhs;
    this->IsEmpty = false;
    //---
    return *Result;
  }
  //---

  /*! \brief Comparison operator. Compare element's value with Rhs */
  bool operator ==(T& Rhs)
  {
    return this->Value == Rhs;
  }
  //---

  using iterator = TMatrixIterator<T, Default, Dimens>;
  /*! \brief Begin-iterator. Recursive goes to element and constructs TMatrixIterator with pointer on first lower sub-TMatrix. 
   * If can't find necessary sub-TMatrix, constructs TMatrixIterator with nullptr, which equals to iterator end() (see below) */
  /*! TMatrixIterator<T, Default, Dimens> begin() */
  iterator begin()
  {
    if (CurDim > 0)
    {
      /*! \brief Just local std::map::iterator that is used for recusion */
      /*! auto It */
      auto It = Storage.begin();
      if (It == Storage.end())
        return iterator(nullptr);
      else
        return It->second.begin();
    }
    else if (CurDim == 0)
    {
      iterator Result = iterator(this);
      if (this->Value == Default)
        ++Result;
      return Result;
    }
    else
    {
      return iterator(nullptr);
    }
  }

  /*! \brief End-iterator. Constructs TMatrixIterator with nullptr */
  /*! TMatrixIterator<T, Default, Dimens> end() */
  iterator end()
  {
    return iterator(nullptr);
  }  
  //---

  /*! \brief This function finds next neighbor of element(not equals Default)/sub-TMatrix(not empty) in Parent->Storage. 
   * Checks upper sub-TMatrix'es if current element(not equals Default)/sub-TMatrix(not empty) only one in Parent->Storage 
   * and in the end returns pointer to next neighbor of element(not equals Default)/sub-TMatrix(not empty) or nullptr */
  /*! TMatrix<T, Default, Dimens>* FindNextNeighbor() */
  TMatrix<T, Default, Dimens>* FindNextNeighbor()
  {
    /*! \brief Result */
    /*! TMatrix<T, Default, Dimens>* Result */
    TMatrix<T, Default, Dimens>* Result = nullptr;
    if (Parent != nullptr)
    {
      /*! \brief std::map::iterator to current element/sub-TMatrix in Parent->Storage */
      /*! auto It */
      auto It = Parent->Storage.find(Index);
      ++It;

      if (It->second.CurDim == 0)
      {
        while (It != Parent->Storage.end() && It->second.Value == Default)
          ++It;
      }

      if (It != Parent->Storage.end())
        Result = &It->second;
    }
    //---
    return Result;
  }
  //---
  
  /*! \brief Returns quantity of non-default elements in whole matrix */
  /*! std::size_t size() */
  std::size_t size()
  {
    /*! \brief Result */
    /*! std::size_t Result */
    std::size_t Result = 0;
    for (auto It = this->begin(); It != this->end(); ++It)
    {
      ++Result;
    }
    //---
    return Result;
  }
};
//------------------------------------------------------------------------

/*! \brief Operator to print out TMatrix element */
/*! std::ostream& operator <<(std::ostream& OStream, TMatrix<T, Default, Dimens>& Matrix) */
template <typename T, T Default, std::size_t Dimens>
std::ostream& operator <<(std::ostream& OStream, TMatrix<T, Default, Dimens>& Matrix)
{
  OStream << Matrix.Value;
  //---
  return OStream;
}
//------------------------------------------------------------------------

#endif // MATRIX_H

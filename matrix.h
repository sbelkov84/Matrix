#ifndef MATRIX_H
#define MATRIX_H
//------------------------------------------------------------------------
#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <tuple>
//------------------------------------------------------------------------

template <typename TArr, std::size_t... I, typename U>
auto ArrToTupleImpl(const TArr& Arr, std::index_sequence<I...>, U& Value)
{
  return std::make_tuple(Arr[I]..., Value);
}
//------------------------------------------------------------------------

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

template <typename T, T Default, std::size_t Dim>
class TMatrixIterator
{
private:

  TMatrix<T, Default, Dim>* Current;

public:

  explicit TMatrixIterator(TMatrix<T, Default, Dim>* Current) : Current(Current) {}

  const auto operator *()
  {
    std::array<int, Dim> IndArr;
    int Index = Dim - 1;

    TMatrix<T, Default, Dim>* TmpCurrent = Current;
    while (TmpCurrent->CurDim < Dim)
    {
      IndArr[Index] = TmpCurrent->Index;
      TmpCurrent = TmpCurrent->Parent;
      --Index;
    }

    T Value = Current->Value;

    auto Res = ArrToTuple(IndArr, Value);
    //Res.asdf;

    return Res;
  }

  TMatrixIterator<T, Default, Dim>& operator ++()
  {
    TMatrixIterator<T, Default, Dim> Result(nullptr);
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

  bool operator !=(const TMatrixIterator<T, Default, Dim>& Rhs)
  {
    return this->Current != Rhs.Current;
  }
};
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

template <typename T, T Default, std::size_t Dimens>
struct TMatrix
{
  TMatrix<T, Default, Dimens>(std::size_t Dim = Dimens, TMatrix<T, Default, Dimens>* Parent = nullptr)
  {
    this->Value = Default;
    this->CurDim = Dim;
    this->IsEmpty = true;
    this->Parent = Parent;
  }
  //---

  std::map<int, TMatrix<T, Default, Dimens>> Storage;

  T Value;
  bool IsEmpty;
  std::size_t CurDim;
  TMatrix<T, Default, Dimens>* Parent;
  int Index;

  TMatrix<T, Default, Dimens>& operator [](int Index)
  {
    try
    {
      if (CurDim < 1)
        throw ("Matrix dimension was exceeded");
    }
    catch (const char* ExcptStr)
    {
      std::cout << ExcptStr << std::endl;
    }

    auto It = Storage.find(Index);

    if (It == Storage.end())
    {
      Storage.insert(std::pair<int, TMatrix<T, Default, Dimens>> {Index, TMatrix<T, Default, Dimens>(CurDim - 1, this)});
      Storage[Index].Index = Index;
    }

    return Storage[Index];
  }
  //---

  TMatrix<T, Default, Dimens>& operator =(T&& Rhs)
  {
    if (Rhs == Default)
    {
      auto It = Parent->Storage.find(Index);
      if (It != Parent->Storage.end())
        Parent->Storage.erase(It);
    }

    this->Value = Rhs;
    this->IsEmpty = false;
    //---
    return *this;
  }
  //---

  bool operator ==(T& Rhs)
  {
    return this->Value == Rhs;
  }
  //---

  using iterator = TMatrixIterator<T, Default, Dimens>;
  iterator begin()
  {
    if (CurDim > 0)
    {
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

  iterator end()
  {
    return iterator(nullptr);
  }  
  //---

  TMatrix<T, Default, Dimens>* FindNextNeighbor()
  {
    TMatrix<T, Default, Dimens>* Result = nullptr;
    if (Parent != nullptr)
    {
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

  std::size_t size()
  {
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

template <typename T, T Default, std::size_t Dimens>
std::ostream& operator <<(std::ostream& OStream, TMatrix<T, Default, Dimens>& Matrix)
{
  OStream << Matrix.Value;
  //---
  return OStream;
}
//------------------------------------------------------------------------

#endif // MATRIX_H

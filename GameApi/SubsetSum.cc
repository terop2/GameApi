#include <iostream>
#include <vector>
#include <map>

template<class I, class T>
class Function
{
public:
  virtual T Index(I i) const=0;
  virtual bool IsValid(I i) const=0;
};

class SubSetSumFuncRoot : public Function<int, bool>
{
public:
  SubSetSumFuncRoot() { }
  bool Index(int i) const
  {
    if (i==0) return true;
    return false;
  }
  virtual bool IsValid(int i) const
  {
    return false;
  }
};


class CompressFunction : public Function<int, bool>
{
public:
  CompressFunction(Function<int,bool> &b, int min_range, int max_range, int num) : b(b), min_range(min_range), max_range(max_range), num(num) { }
  void PreCalc()
  {
    // (max_range-min_range)*O(lg n) steps
    for(int i=min_range;i<max_range;i++)
      {
	bool b1 = b.Index(i); // O(lg n) steps
	bool b2 = b.Index(i+num); // O(lg n) steps
	func[i] = b1||b2; // O(lg n) steps
	isvalid[i] = (b1||b2) && (b2 || b.IsValid(i+num) || b.IsValid(i)); // 3*O(lg n) steps 
      }
  }
  bool Index(int i) const
  {
    bool bb = func[i]; // O(lg n) steps
    return bb;
  }
  bool IsValid(int i) const
  {
    return isvalid[i]; // O(lg n) steps
  }
private:
  Function<int,bool> &b;
  int min_range;
  int max_range;
  mutable std::map<int, bool> func;
  mutable std::map<int, bool> isvalid;
  int num;
};

int main()
{
  SubSetSumFuncRoot r;
  int r1 = 10+5; // O(N*P)
  int r2 = 8+2;  // O(N*P)
  CompressFunction  s1(r, -r2,r1, 10); // (r1+r2)*O(lg n) steps
  s1.PreCalc();
  CompressFunction  s2(s1,-r2,r1, 5); 
  s2.PreCalc();
  CompressFunction s3(s2,-r2,r1, -7); 
  s3.PreCalc();
  CompressFunction s4(s3,-r2,r1, -3); 
  s4.PreCalc();

  // CompressFunction PreCalc executed N times.

  // TOTAL time O(N*2^N)*O(lg n)*N

  std::cout << "Valid: " << s4.IsValid(0) << std::endl; // lg n steps
}

#include <vector>
#include <iostream>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <math.h>
//
// Newton's what comes next - differential equation solver
//
// f'(x) + x^2 + 1 = 0, f(0)=999
// => f'(x) = -x^2 - 1
// => fptr = -x^2 -1, degree = 1 (because f'(x))
// => initial values = [999]

// f''(x) + x^2 + 1 = 0, f'(0)=888, f(0)=666
// => f''(x) = -x^2 - 1
// => fptr = -x^2 - 1, degree=2, (because f''(x))
// => initial values= [666,888]


//
//    s0 s1 s2 s3
//  i0 r1
//  i1
//
// pos=1


struct X
{
  float x;
  X(float x) : x(x) { }
  std::string print_x() const {
    std::stringstream ss;
    ss << x;
    return ss.str();
  }
  float to_float() const { return x; }
};

struct I
{
  int index;
  I(int index) : index(index) { }
  friend X convert_i_to_x(I i, X range_start, X range_end, I sample_count)
  {
    float ii = float(i.index);
    float count = float(sample_count.index);
    float ii_scaled = range_start.x + ii/count*(range_end.x-range_start.x);
    X res(ii_scaled);
    return res;
  }
};


struct FX
{
  float f_val;
  FX() : f_val(0.0) { } // std::vector<FX>::resize uses this
  FX(float fx) : f_val(fx) { }
  std::string print_fx() const
  {
    std::stringstream ss;
    ss << f_val;
    return ss.str();
  }
  float to_float() const { return f_val; }
  friend FX operator+(FX a, FX b) { FX res(a.f_val + b.f_val); return res; }
};
struct D
{
  int ii;
  D(int i) : ii(i) { }
};

struct P {
private:
  float mult;
public:
  P(float mult) : mult(mult) { }
  void operator+=(float s) { mult+=s; }
  bool check_if_zero() const { return fabs(mult)<0.00000001; }
  std::string print_mult() const { std::stringstream ss; ss << mult; return ss.str(); }
};

void fill_slot(std::vector<FX> &res, int pos, const std::vector<FX> &slots)
{
  FX f0 = res[pos-1];
  FX s0 = slots[pos-1];
  FX r1 = f0 + s0;
  res[pos] = r1;
}

std::vector<FX> next_level(std::vector<FX> slots, std::vector<FX> initial_values, int degree)
{
  std::vector<FX> res;
  res.push_back(initial_values[degree]);
  res.resize(slots.size()+1);
  
  for(int i=1;i<slots.size()+1;i++)
    {
      fill_slot(res,i,slots);
    }
  return res;
}

std::vector<P> lagranges_interpolation(std::vector<X> x, std::vector<FX> y)
{
  int n = x.size();
  assert(n==y.size());
  P def(0.0);
  std::vector<P> result(n, def);
  
  for (int i = 0; i < n; i++)
    {
      std::vector<float> basis(1, 1.0);
      double denominator = 1.0;
      
      for (int j = 0; j < n; j++)
	{
	  if (i == j) continue;
	  
	  // Multiply basis by (x - x[j])
	  std::vector<float> next(basis.size() + 1, 0.0);
	  
	  for (int k = 0; k < basis.size(); k++)
	    {
	      next[k]   -= basis[k] * x[j].to_float();
	      next[k+1] += basis[k];
	    }
	  
	  basis = next;
	  
	  denominator *= (x[i].to_float() - x[j].to_float());
	}
      
      // Add y[i] * basis / denominator
      for (int k = 0; k < n; k++)
	{
	  result[k] += y[i].to_float() * basis[k] / denominator;
	}
    }
  return result;
}

std::string x_display(int i)
{
  if (i==0) return "";
  if (i==1) return "x";
  std::stringstream ss;
  ss << i;
  return std::string("x^")+ss.str();  
}

std::string i_display(D ii, X range_start)
{
  std::string s = "f";
  for(int i=0;i<ii.ii;i++) s+='\'';
  //std::stringstream ss;
  X ii_scaled = range_start;
  s+="(" + ii_scaled.print_x() + ")=";
  return s;
}


void solve_diff_eq(FX (*fptr)(X x), std::string eq, int degree,
		   std::vector<FX> initial_values,
		   X range_start, X range_end,
		   int sample_count)
{
  std::cout << std::endl;

  std::cout << "Diff Equation to be solved:" << std::endl;
  if (degree==1)
    std::cout << "df/dx + " << eq << " =0 "<< std::endl;
  else
    std::cout << "df^" << degree << "/dx^" << degree << " + " << eq << " =0 "<< std::endl;

  std::cout << std::endl;
  std::cout << "Initial values:" << std::endl;
  int s3 = initial_values.size();
  for(int i=s3-1;i>=0;i--)
    {
      std::cout << i_display(i,range_start) << initial_values[i].f_val << std::endl;
    }
  
  

  
  sample_count--;
  std::vector<FX> val; // i->FX
  std::vector<X> orig_x; // i->X
  for(int i=0;i<sample_count+1;i++)
    {
      I ii0 = { i };
      I sample_count0 = { sample_count };
      X ii_scaled = convert_i_to_x(ii0,range_start,range_end,sample_count0);
      //float ii = float(i);
      //float ii_scaled = range_start + ii/float(sample_count)*(range_end-range_start);
      orig_x.push_back(ii_scaled);
      val.push_back(fptr(ii_scaled)); // 0, 1, 2, 3 ,4, ..., 9
    }
  std::vector<FX> curr; // i->FX
  for(int i=degree-1;i>=0;i--)
    {
      curr = next_level(val,initial_values,i);
      val = curr;
    }
  int s = std::min(curr.size(),orig_x.size());

  std::cout.setf(std::ios::fixed, std::ios::floatfield);
  std::cout.setf(std::ios::showpoint);

  std::cout << std::endl;
  std::cout << "Samples:" << std::endl;
  for(int i=0;i<s;i++)
    {
      std::cout << "f(" << orig_x[i].print_x() << ") = " << curr[i].print_fx() << std::endl;
    }

  curr.resize(orig_x.size());
  
  std::vector<P> res = lagranges_interpolation(orig_x,curr);
  std::cout << "Polynomial:" << std::endl;
  int s1 = res.size();
  for(int i=0;i<s1;i++)
    {
      if (res[i].check_if_zero()) { } else {
	if (i!=0) std::cout << "+";
	std::cout << res[i].print_mult();
	std::cout << x_display(i);
      }
    }
  std::cout << std::endl;
  std::cout << std::endl;
}

FX diff1_subst(X x) { return FX(-x.x*x.x-1); }
FX diff2_subst(X x) { return FX(-x.x*x.x-1); }

void diff_test_case1()
{
  std::cout << "Test case #1:" << std::endl;
  solve_diff_eq(&diff1_subst, "x*x+1", 1, std::vector<FX>{0.0f}, X(0.0), X(30.0), 10);
}
void diff_test_case2()
{
  std::cout << "Test case #2:" << std::endl;
  solve_diff_eq(&diff2_subst, "x*x+1", 2,std::vector<FX>{0.0f,0.0f}, X(0.0), X(30.0), 10);  
}

int main()
{
  diff_test_case1();
  diff_test_case2();
}

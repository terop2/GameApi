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
void fill_slot(std::vector<float> &res, int pos, const std::vector<float> &slots)
{
  float f0 = res[pos-1];
  float s0 = slots[pos-1];
  float r1 = f0 + s0;
  res[pos] = r1;
}

std::vector<float> next_level(std::vector<float> slots, std::vector<float> initial_values, int degree)
{
  std::vector<float> res;
  res.push_back(initial_values[degree]);
  res.resize(slots.size()+1);
  
  for(int i=1;i<slots.size()+1;i++)
    {
      fill_slot(res,i,slots);
    }
  return res;
}

std::vector<float> lagranges_interpolation(std::vector<float> x, std::vector<float> y)
{
  int n = x.size();
  assert(n==y.size());
  std::vector<float> result(n, 0.0);
  
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
	      next[k]   -= basis[k] * x[j];
	      next[k+1] += basis[k];
	    }
	  
	  basis = next;
	  
	  denominator *= (x[i] - x[j]);
	}
      
      // Add y[i] * basis / denominator
      for (int k = 0; k < n; k++)
	{
	  result[k] += y[i] * basis[k] / denominator;
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

std::string i_display(int ii, float range_start)
{
  std::string s = "f";
  for(int i=0;i<ii;i++) s+='\'';
  std::stringstream ss;
  float ii_scaled = range_start;
  ss << ii_scaled;
  s+="(" + ss.str() + ")=";
  return s;
}


void solve_diff_eq(float (*fptr)(float x), int degree,
		   std::vector<float> initial_values,
		   float range_start, float range_end,
		   int sample_count)
{
  std::cout << std::endl;
  std::cout << "Initial values:" << std::endl;
  int s3 = initial_values.size();
  for(int i=s3-1;i>=0;i--)
    {
      std::cout << i_display(i,range_start) << initial_values[i] << std::endl;
    }
  
  

  
  sample_count--;
  std::vector<float> val;
  std::vector<float> orig_x;
  for(int i=0;i<sample_count+1;i++)
    {
      float ii = float(i);
      float ii_scaled = range_start + ii/float(sample_count)*(range_end-range_start);
      orig_x.push_back(ii_scaled);
      val.push_back(fptr(ii_scaled)); // 0, 1, 2, 3 ,4, ..., 9
    }
  std::vector<float> curr;
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
      std::cout << "f(" << orig_x[i] << ") = " << curr[i] << std::endl;
    }

  curr.resize(orig_x.size());
  
  std::vector<float> res = lagranges_interpolation(orig_x,curr);
  std::cout << "Polynomial:" << std::endl;
  int s1 = res.size();
  for(int i=0;i<s1;i++)
    {
      if (fabs(res[i])<0.00000001) { } else {
	if (i!=0) std::cout << "+";
	std::cout << res[i];
	std::cout << x_display(i);
      }
    }
  std::cout << std::endl;
}

float diff1_subst(float x) { return -x*x-1; }
float diff2_subst(float x) { return -x*x-1; }

void diff_test_case1()
{
  solve_diff_eq(&diff1_subst, 1, std::vector<float>{0.0f}, -30.0, 30.0, 10);
}
void diff_test_case2()
{
  solve_diff_eq(&diff2_subst, 2, std::vector<float>{0.0f,0.0f}, -30.0, 30.0, 10);  
}

int main()
{
  diff_test_case1();
  diff_test_case2();
}

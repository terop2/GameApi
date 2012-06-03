
void test_expr()
{
  ConstantValue val(100);
  X_Value<int> x;
  X_Value<int> y;
  T_Int_to_Float_conversion conv_x(x); 
  T_Int_to_Float_conversion conv_y(y); 
  T_Int_to_Float_conversion conv_r(val);
  T_power_2_float x_power(conv_x);
  T_power_2_float y_power(conv_y);
  T_power_2_float r_power(conv_r);
  T_Float_to_Int_conversion fx(x_power);
  T_Float_to_Int_conversion fy(y_power);
  T_Float_to_Int_conversion fr(r_power);
  PlusValue<int> plus(fx, fy);

  X_X_Value_Int x_x;
  X_choose_first<int,int> left_side(x_x);
  X_choose_second<int,int> right_side(x_x);
  
  

}

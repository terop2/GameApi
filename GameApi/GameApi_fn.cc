
#include "GameApi_h.hh"

struct FunctionPriv
{
};


EXPORT GameApi::FunctionApi::FunctionApi(Env &e) : e(e)
{
  priv = (void*)new FunctionPriv;
}
EXPORT GameApi::FunctionApi::~FunctionApi()
{
  delete (FunctionPriv*)priv;
}
float GameApi::FunctionApi::get_value(GameApi::F f, float val)
{
  FunctionImpl *impl = find_function(e,f);
  if (!impl) return 0.0;
  if (!impl->func) return 0.0;
  return impl->func->Index(val);
}

GameApi::F GameApi::FunctionApi::constant(float y)
{
  return add_function(e, new ConstantFloatFunction(y));
}
GameApi::F GameApi::FunctionApi::sin()
{
  return add_function(e, new NativeFunction<float,float>(&std::sin));
}
GameApi::F GameApi::FunctionApi::cos()
{
  return add_function(e, new NativeFunction<float,float>(&std::cos));
}

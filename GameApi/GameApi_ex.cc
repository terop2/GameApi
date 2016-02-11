
#include "GameApi_h.hh"

class VariableExpr : public ExprNode
{
public:
  VariableExpr(std::string name) : name(name) { }
  virtual float float_execute(std::vector<GameApi::FloatExprEnv> &env)
  {
    int s = env.size();
    for(int i=0;i<s;i++)
      {
	GameApi::FloatExprEnv &e = env[i];
	if (e.name == name) { return e.value; }
      }
    std::cout << "Expr float value not found: " << name << std::endl;
    return 0.0f;
  }
  virtual int int_execute(std::vector<GameApi::IntExprEnv> &env)
  {
    int s = env.size();
    for(int i=0;i<s;i++)
      {
	GameApi::IntExprEnv &e = env[i];
	if (e.name == name) { return e.value; }
      }
    std::cout << "Expr int value not found: " << name << std::endl;
    return 0;
  }  
private:
  std::string name;
};
GameApi::EX GameApi::ExprApi::variable(std::string name)
{
  return add_expr(e, new VariableExpr(name));
}

class PlusExpr : public ExprNode
{
public:
  PlusExpr(ExprNode *n1, ExprNode *n2) : n1(n1), n2(n2) { }
  virtual float float_execute(std::vector<GameApi::FloatExprEnv> &env)
  {
    return n1->float_execute(env) + n2->float_execute(env);
  }
  virtual int int_execute(std::vector<GameApi::IntExprEnv> &env)
  {
    return n1->int_execute(env) + n2->int_execute(env);
  }
private:
  ExprNode *n1, *n2;
};
GameApi::EX GameApi::ExprApi::plus(EX e1, EX e2)
{
  ExprNode *n1 = find_expr(e, e1);
  ExprNode *n2 = find_expr(e, e2);
  return add_expr(e, new PlusExpr(n1,n2));
}


class MulExpr : public ExprNode
{
public:
  MulExpr(ExprNode *n1, ExprNode *n2) : n1(n1), n2(n2) { }
  virtual float float_execute(std::vector<GameApi::FloatExprEnv> &env)
  {
    return n1->float_execute(env) * n2->float_execute(env);
  }
  virtual int int_execute(std::vector<GameApi::IntExprEnv> &env)
  {
    return n1->int_execute(env) * n2->int_execute(env);
  }
private:
  ExprNode *n1, *n2;
};
GameApi::EX GameApi::ExprApi::mul(EX e1, EX e2)
{
  ExprNode *n1 = find_expr(e, e1);
  ExprNode *n2 = find_expr(e, e2);
  return add_expr(e, new MulExpr(n1,n2));
}


class MinusExpr : public ExprNode
{
public:
  MinusExpr(ExprNode *n1, ExprNode *n2) : n1(n1), n2(n2) { }
  virtual float float_execute(std::vector<GameApi::FloatExprEnv> &env)
  {
    return n1->float_execute(env) - n2->float_execute(env);
  }
  virtual int int_execute(std::vector<GameApi::IntExprEnv> &env)
  {
    return n1->int_execute(env) - n2->int_execute(env);
  }
private:
  ExprNode *n1, *n2;
};
GameApi::EX GameApi::ExprApi::minus(EX e1, EX e2)
{
  ExprNode *n1 = find_expr(e, e1);
  ExprNode *n2 = find_expr(e, e2);
  return add_expr(e, new MinusExpr(n1,n2));
}


class DivExpr : public ExprNode
{
public:
  DivExpr(ExprNode *n1, ExprNode *n2) : n1(n1), n2(n2) { }
  virtual float float_execute(std::vector<GameApi::FloatExprEnv> &env)
  {
    return n1->float_execute(env) / n2->float_execute(env);
  }
  virtual int int_execute(std::vector<GameApi::IntExprEnv> &env)
  {
    return n1->int_execute(env) / n2->int_execute(env);
  }
private:
  ExprNode *n1, *n2;
};
GameApi::EX GameApi::ExprApi::div(EX e1, EX e2)
{
  ExprNode *n1 = find_expr(e, e1);
  ExprNode *n2 = find_expr(e, e2);
  return add_expr(e, new DivExpr(n1,n2));
}

class SinExpr : public ExprNode
{
public:
  SinExpr(ExprNode *n) : n(n) { }
  virtual float float_execute(std::vector<GameApi::FloatExprEnv> &env)
  {
    float val = n->float_execute(env);
    return sin(val);
  }
  virtual int int_execute(std::vector<GameApi::IntExprEnv> &env)
  {
    std::cout << "ERROR! sin with ints" << std::endl;
    return 0;
  }

private:
  ExprNode *n;
};

class CosExpr : public ExprNode
{
public:
  CosExpr(ExprNode *n) : n(n) { }
  virtual float float_execute(std::vector<GameApi::FloatExprEnv> &env)
  {
    float val = n->float_execute(env);
    return cos(val);
  }
  virtual int int_execute(std::vector<GameApi::IntExprEnv> &env)
  {
    std::cout << "ERROR! cos with ints" << std::endl;
    return 0;
  }

private:
  ExprNode *n;
};
GameApi::EX GameApi::ExprApi::sin(GameApi::EX ee)
{
  ExprNode *n = find_expr(e, ee);
  return add_expr(e, new SinExpr(n));
}
GameApi::EX GameApi::ExprApi::cos(GameApi::EX ee)
{
  ExprNode *n = find_expr(e, ee);
  return add_expr(e, new CosExpr(n));
}

class ConstantExpr : public ExprNode
{
public:
  ConstantExpr(int ival, float fval) : ival(ival),fval(fval) { }
  virtual float float_execute(std::vector<GameApi::FloatExprEnv> &env)
  {
    return fval;
  }
  virtual int int_execute(std::vector<GameApi::IntExprEnv> &env)
  {
    return ival;
  }
private:
  int ival;
  int fval;
};
GameApi::EX GameApi::ExprApi::float_constant(float val)
{
  return add_expr(e, new ConstantExpr(int(val), val));
}
GameApi::EX GameApi::ExprApi::int_constant(int val)
{
  return add_expr(e, new ConstantExpr(val, float(val)));
}

float GameApi::ExprApi::expr_eval_float(EX expr, std::vector<FloatExprEnv> env)
{
  std::vector<GameApi::FloatExprEnv> vec;
  int s = env.size();
  for(int i=0;i<s;i++)
    {
      GameApi::FloatExprEnv e;
      e.name = env[i].name;
      e.value = env[i].value;
      vec.push_back(e);
    }
  ExprNode *n = find_expr(e, expr);
  return n->float_execute(vec);
}
int GameApi::ExprApi::expr_eval_int(EX expr, std::vector<IntExprEnv> env)
{
  std::vector<GameApi::IntExprEnv> vec;
  int s = env.size();
  for(int i=0;i<s;i++)
    {
      GameApi::IntExprEnv e;
      e.name = env[i].name;
      e.value = env[i].value;
      vec.push_back(e);
    }
  ExprNode *n = find_expr(e, expr);
  return n->int_execute(vec);
}
GameApi::EX expr_parse(GameApi::ExprApi &api, std::string expr, bool &success);

int find_char(std::string expr, char c)
{
  int s = expr.size();
  for(int i=0;i<s;i++)
    {
      if (expr[i]==c) return i;
    }
  return -1;
}

GameApi::EX mul_parse(GameApi::ExprApi &api, std::string expr, bool &success)
{
  int val = find_char(expr, '*');
  if (val==-1) { success=false; GameApi::EX e; e.id = -1; return e; }
  std::string expr1 = expr.substr(0,val);
  std::string expr2 = expr.substr(val+1, expr.size()-val-1);
  GameApi::EX e1 = expr_parse(api, expr1, success);
  if (!success) { success=false; GameApi::EX e; e.id = -1; return e; }
  GameApi::EX e2 = expr_parse(api, expr2, success);
  if (!success) { success=false; GameApi::EX e; e.id = -1; return e; }
  success = true;
  return api.mul(e1,e2);
}
GameApi::EX div_parse(GameApi::ExprApi &api, std::string expr, bool &success)
{
  int val = find_char(expr, '/');
  if (val==-1) { success=false; GameApi::EX e; e.id = -1; return e; }
  std::string expr1 = expr.substr(0,val);
  std::string expr2 = expr.substr(val+1, expr.size()-val-1);
  GameApi::EX e1 = expr_parse(api, expr1, success);
  if (!success) { success=false; GameApi::EX e; e.id = -1; return e; }
  GameApi::EX e2 = expr_parse(api, expr2, success);
  if (!success) { success=false; GameApi::EX e; e.id = -1; return e; }
  success = true;
  return api.div(e1,e2);
}
GameApi::EX plus_parse(GameApi::ExprApi &api, std::string expr, bool &success)
{
  int val = find_char(expr, '+');
  if (val==-1) { success=false; GameApi::EX e; e.id = -1; return e; }
  std::string expr1 = expr.substr(0,val);
  std::string expr2 = expr.substr(val+1, expr.size()-val-1);
  GameApi::EX e1 = expr_parse(api, expr1, success);
  if (!success) { success=false; GameApi::EX e; e.id = -1; return e; }
  GameApi::EX e2 = expr_parse(api, expr2, success);
  if (!success) { success=false; GameApi::EX e; e.id = -1; return e; }
  success = true;
  return api.plus(e1,e2);
}
GameApi::EX minus_parse(GameApi::ExprApi &api, std::string expr, bool &success)
{
  int val = find_char(expr, '-');
  if (val==-1) { success=false; GameApi::EX e; e.id = -1; return e; }
  std::string expr1 = expr.substr(0,val);
  std::string expr2 = expr.substr(val+1, expr.size()-val-1);
  GameApi::EX e1 = expr_parse(api, expr1, success);
  if (!success) { success=false; GameApi::EX e; e.id = -1; return e; }
  GameApi::EX e2 = expr_parse(api, expr2, success);
  if (!success) { success=false; GameApi::EX e; e.id = -1; return e; }
  success = true;
  return api.minus(e1,e2);
}
GameApi::EX sin_parse(GameApi::ExprApi &api, std::string expr, bool &success)
{
  if (expr.size()>4 && expr[0]=='s' && expr[1]=='i' && expr[2]=='n' && expr[3]=='(')
    {
      std::string rem = expr.substr(4,expr.size()-4);
      int pos = find_char(rem,')');
      if (pos==-1) { success=false; GameApi::EX e; e.id = -1; return e; }
      std::string expr1 = rem.substr(0, pos);
      int ss = expr.size()-1;
      if (pos==ss)
	{
	  GameApi::EX e = expr_parse(api, expr1, success);
	  if (!success) { success=false; GameApi::EX e; e.id = -1; return e; }
	  success = true;
	  return api.sin(e);
	}
      else
	{
	  success=false; GameApi::EX e; e.id = -1; return e;
	}
    }
  success=false; GameApi::EX e; e.id = -1; return e;
}

GameApi::EX cos_parse(GameApi::ExprApi &api, std::string expr, bool &success)
{
  if (expr.size()>4 && expr[0]=='c' && expr[1]=='o' && expr[2]=='s' && expr[3]=='(')
    {
      std::string rem = expr.substr(4,expr.size()-4);
      int pos = find_char(rem,')');
      if (pos==-1) { success=false; GameApi::EX e; e.id = -1; return e; }
      std::string expr1 = rem.substr(0, pos);
      int ss = expr.size()-1;
      if (pos==ss)
	{
	  GameApi::EX e = expr_parse(api, expr1, success);
	  if (!success) { success=false; GameApi::EX e; e.id = -1; return e; }
	  success = true;
	  return api.cos(e);
	}
      else
	{
	  success=false; GameApi::EX e; e.id = -1; return e;
	}
    }
  success=false; GameApi::EX e; e.id = -1; return e;
}

GameApi::EX constant_parse_float(GameApi::ExprApi &api, std::string expr, bool &success)
{
  int val = find_char(expr, '.');
  if (val==-1) {
    success=false; GameApi::EX e; e.id = -1; return e;
  }
  int s = expr.size();
  for(int i=0;i<s;i++)
    {
      char c = expr[i];
      if (c>='0' && c<='9') continue;
      if (c=='.') continue;
      success=false; GameApi::EX e; e.id = -1; return e;      
    }
  success = true;
  std::stringstream ss(expr);
  float val2;
  ss >> val2;
  GameApi::EX e = api.float_constant(val2);
   return e;
}


GameApi::EX constant_parse_int(GameApi::ExprApi &api, std::string expr, bool &success)
{
  int s = expr.size();
  for(int i=0;i<s;i++)
    {
      char c = expr[i];
      if (c>='0' && c<='9') continue;
      success=false; GameApi::EX e; e.id = -1; return e;      
    }
  success = true;
  std::stringstream ss(expr);
  int val2;
  ss >> val2;
  GameApi::EX e = api.int_constant(val2);
  return e;
}


GameApi::EX expr_parse_float(GameApi::ExprApi &api, std::string expr, bool &success)
{
  GameApi::EX expr3 = plus_parse(api, expr, success);
  if (success) return expr3;
  GameApi::EX expr32 = minus_parse(api, expr, success);
  if (success) return expr32;
  GameApi::EX expr1 = mul_parse(api, expr, success);
  if (success) return expr1;
  GameApi::EX expr2 = div_parse(api, expr, success);
  if (success) return expr2;
  GameApi::EX expr4 = sin_parse(api, expr, success);
  if (success) return expr4;
  GameApi::EX expr5 = cos_parse(api, expr, success);
  if (success) return expr5;
  GameApi::EX expr6 = constant_parse_float(api, expr, success);
  if (success) return expr6;
  std::cout << "EXPR PARSE ERROR (float)!" << expr << std::endl;
  return api.float_constant(0.0);
}

GameApi::EX expr_parse_int(GameApi::ExprApi &api, std::string expr, bool &success)
{
  GameApi::EX expr3 = plus_parse(api, expr, success);
  if (success) return expr3;
  GameApi::EX expr33 = minus_parse(api, expr, success);
  if (success) return expr33;
  GameApi::EX expr1 = mul_parse(api, expr, success);
  if (success) return expr1;
  GameApi::EX expr2 = div_parse(api, expr, success);
  if (success) return expr2;
  GameApi::EX expr4 = sin_parse(api, expr, success);
  if (success) return expr4;
  GameApi::EX expr5 = cos_parse(api, expr, success);
  if (success) return expr5;
  GameApi::EX expr6 = constant_parse_int(api, expr, success);
  if (success) return expr6;
  std::cout << "EXPR PARSE ERROR(int)!" << expr << std::endl;
  return api.int_constant(0);
}

GameApi::EX expr_parse(GameApi::ExprApi &api, std::string expr, bool &success)
{
  GameApi::EX expr3 = expr_parse_float(api, expr, success);
  if (success) return expr3;
  GameApi::EX expr4 = expr_parse_int(api,expr, success);
  if (success) return expr4;
  success = false;
  return api.float_constant(0.0);
}

GameApi::EX GameApi::ExprApi::expr_float(std::string expr, bool &success)
{
  return expr_parse(*this, expr, success);
}
GameApi::EX GameApi::ExprApi::expr_int(std::string expr, bool &success)
{
  return expr_parse(*this, expr, success);
}


#include <string>
#include <vector>
#include <utility>
#include <map>
#include <iostream>
#include <sstream>

#include "..\GameApi.hh"
#include "..\VectorTools.hh"


class Item
{
  virtual std::string Name() const=0;
  virtual int ParamCount() const=0;
  virtual std::string ParamName(int p) const=0;
  virtual std::string ParamType(int p) const=0;
  virtual std::string ParamDefault(int p) const=0;
  virtual std::string ReturnType() const=0;
  virtual int Execute(std::vector<std::string> params)=0;
  virtual std::pair<std::string, std::string> CodeGen(std::vector<std::string> params, std::vector<std::string> param_names)=0;
};

std::string api_name();
std::vector<Item*> functions();
std::string type_symbol();
int num_displays();
void display(int i, int disp);
int add(void *w);
void *find(int i);

struct AK
{
  int id;
};
struct PML
{
  int id;
};

class AccelNode
{
public:
  std::vector<void*> vec;
};

class PreCalcMainLoop
{
public:
  virtual void Execute()=0;
};


class AccelStructure
{
public:
  virtual void clear()=0;
  virtual AccelNode *find_point(Point p) const=0;
  virtual AccelNode *find_next(AccelNode *current, Point start, Point target) const=0;
  virtual std::vector<AccelNode*> find_span(Point p1, Point p2) const=0;
  virtual std::vector<AccelNode*> find_tri(Point p1, Point p2, Point p3) const=0;
  virtual std::vector<AccelNode*> find_cube(float start_x, float end_x, float start_y, float end_y, float start_z, float end_z) const=0;
  virtual std::vector<AccelNode*> find_quad(Point p1, Point p2, Point p3, Point p4) const=0;
  virtual void push_back(Point p, void *data)=0;
  virtual void push_back(AccelNode *n, void *data)=0;
};

AK empty(int sx, int sy, int sz,
	 float start_x, float end_x,
	 float start_y, float end_y,
	 float start_z, float end_z
	 );
PML bind_p(AK orig, GameApi::P p);
PML clear(AK orig);
PML or_elem(PML p1, PML p2);
PML or_array(std::vector<PML> vec);




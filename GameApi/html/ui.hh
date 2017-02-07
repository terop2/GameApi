//#include "data.hh"

#include <string>
#include <vector>
#include <utility>
#include <map>
#include <iostream>
#include <sstream>

// DLL ENTRY POINTS
class Widget;
class Item
{
public:
  virtual std::string Name() const=0;
  virtual int ParamCount() const=0;
  virtual std::string ParamName(int p) const=0;
  virtual std::string ParamType(int p) const=0;
  virtual std::string ParamDefault(int p) const=0;
  virtual std::string ReturnType() const=0;
  virtual std::string Symbols() const=0;
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
// END OF DLL ENTRY POINTS

struct W { 
  int id;
};

W empty();
W list_y(std::vector<W> v, int gap_y);
W list_x(std::vector<W> v, int gap_x);
W grid_y(std::vector<W> v, int sx, int gap_x, int gap_y);
W layer(std::vector<W> v);
// TODO RESPONSIVE DESIGN
W percentage_xx(W w1, W w2, float val, float val2);
W percentage_margin(W w, float val, float val2, float val3);
W height_y(W w1, int height);
W max_width(W w1, int width);
W flex_image_keep_aspect_x(W w1, float percentage_width);
W flex_image_keep_aspect_y(W w1, float percentage_height);
W flex_grid(std::vector<W> w, int max_width);
// END TODO
W hide(W widget);
W show_button(W button_content, W hidden_element);
W hide_button(W button_content, W hidden_element);
W image(std::string url, int size_x, int size_y);
W image_placeholder(int size_x, int size_y);
W gradient(int id, int size_x, int size_y);
W child_size(W w);
W margin(W item, int left, int top, int right, int bottom);
W right_align(W item, int sx);
W center_align(W item, int sx);
W center_y(W item, int sy);
W form(W w, bool is_get, std::string script_url);
W submit_button(std::string label, int size_x, int size_y);
W list_item1(W image, std::string label);
W list_item2(W image, std::string label);
W editor(std::string name, std::string value, std::string placeholder, int size_x, int size_y, int font_size, unsigned int font_colour, int align);
W label(std::string l, int size_x, int size_y, int font_size, unsigned int font_colour, int align);
W link(std::string url, std::string l, int size_x, int size_y, int font_size, unsigned int font_colour, int align);
W margin(W orig, int l, int t, int r, int b);
W password_editor(std::string name, std::string value, std::string placeholder, int size_x, int size_y, int font_size, unsigned int font_colour, int align);
W passwd_dialog(int size_x, int size_y);
W passwd_setup_dialog(int size_x, int size_y);

std::string gen_html_page(W w);


// app specific stuff
//W client_list_item(Record r);
//W product_list_item(Product p);

std::string unique_id_apiitem();
const int E = -16384;

class Widget
{
public:
  Widget() { }
  virtual void make_unique(std::string path) {
    if (ids.find(path)==ids.end())
      {
      ids[path]=unique_id_apiitem();
      }
  }
  virtual void SetPos(std::string path, int pos_x2, int pos_y2) {
    //std::cout << "Widget:SetPos: " << pos_x2 << " " << pos_y2 << std::endl;
    pos_x[path] = pos_x2; pos_y[path]=pos_y2; 
  }
  virtual void SetSize(std::string path, int size_x2, int size_y2) { 
    //std::cout << "Widget:SetSize: " << size_x2 << " " << size_y2 << std::endl;
    size_x[path] = size_x2; size_y[path] = size_y2; 
  }
  virtual int preferred_width(std::string path) { return 0; }
  virtual int preferred_height(std::string path) { return 0; }
  virtual std::string Page(std::string path)
  {
    std::cout << "Widget Page:" << path << std::endl;

    std::string s = std::string("<div id=\"") + ids[path] + "\">\n";
    s+="</div>\n";
    return s;
  }
  void AddScript(std::string path, std::string s)
  {
    script[path]+=s;
  }
  virtual std::string Script(std::string path)
  {
    return script[path];
  }
  virtual std::string Css(std::string path)
  {
    std::string s = std::string("#") + ids[path] + "{\n";
    std::stringstream ss;
    ss << size_x[path];
    s+=std::string("width:") + ss.str() + "px;\n";
    std::stringstream ss2;
    ss2 << size_y[path];
    s+=std::string("height:") + ss2.str() + "px;\n";
    s+=std::string("position:absolute;\n");
    std::stringstream ss3;
    ss3 << pos_x[path];
    std::stringstream ss4;
    ss4 << pos_y[path];
    s+=std::string("left:") + ss3.str() + "px;\n";
    s+=std::string("top:") + ss4.str() + "px;\n";
    s+=CssProp(path);
    s+="}\n";
    return s;
  }
  void SetCssProp(std::string path, std::string s) { cssprop[path] = s; }
  virtual std::string CssProp(std::string path)
  {
    return cssprop[path];
  }
protected:
  std::map<std::string, std::string> script;
  std::map<std::string, std::string> cssprop;
  std::string id;
  std::map<std::string, int> pos_x;
  std::map<std::string, int> pos_y;
  std::map<std::string, int> size_x;
  std::map<std::string, int> size_y;
  std::map<std::string, std::string> ids;
  std::string last_path;
};

class WidgetForward : public Widget
{
public:
  WidgetForward(int pref_size_x, int pref_size_y) {
    p_x = pref_size_x;
    p_y = pref_size_y;
  }
  virtual void make_unique(std::string path) {
    Widget::make_unique(path);
    int s = vec.size();
    for(int ii=0;ii<s;ii++)
      {
	Widget *w = vec[ii];
	std::string next = next_path(path,ii);
	w->make_unique(next);
      }
  }
  std::string next_path(std::string path, int i)
  {
    std::string s = path;
   
    s+='a' + i;
    return s;
  }
  virtual int preferred_width(std::string path) { return p_x; }
  virtual int preferred_height(std::string path) { return p_y; }

  void push_back(Widget *w, int x, int y, int sx, int sy, int r, int b)
  {
    vec.push_back(w);
    child_x.push_back(x);
    child_y.push_back(y);
    child_sx.push_back(sx);
    child_sy.push_back(sy);
    child_r.push_back(r);
    child_b.push_back(b);
    empty_x.push_back(0);
    empty_y.push_back(0);
  }
  virtual void SetPos(std::string path, int pos_x2, int pos_y2)
  {
    Widget::SetPos(path, pos_x2, pos_y2);
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	Widget *w = vec[i];
	int x = child_x[i];
	int y = child_y[i];
	if (x==E) { x=size_x[path] - child_r[i]-child_sx[i]; }
	if (y==E) { y=size_y[path] - child_b[i]-child_sy[i]; }
	//std::cout << "Child: " << pos_x << " " << pos_y << " " << x << " " << y << std::endl;
	std::string next = next_path(path, i);
	w->SetPos(next, x,
		  y);
      }
  }
  virtual void SetSize(std::string path, int size_x2, int size_y2)
  {
    Widget::SetSize(path, size_x2, size_y2);
    int s = vec.size();
    for(int i=0;i<s;i++)
      {
	Widget *w = vec[i];
	if (child_x[i]==E) { empty_x[i]=size_x[path] - child_sx[i]-child_r[i]; }
	if (child_y[i]==E) { empty_y[i]=size_y[path] - child_sy[i]-child_b[i]; }
	if (child_sx[i]==E) { empty_x[i]=size_x[path]-child_x[i]-child_r[i]; }
	if (child_sy[i]==E) { empty_y[i]=size_y[path]-child_y[i]-child_b[i]; }
	if (child_r[i]==E) { empty_x[i]=size_x[path]-child_x[i]-child_sx[i]; }
	if (child_b[i]==E) { empty_y[i]=size_y[path]-child_y[i]-child_sx[i]; }
	
	int x,y,sx,sy;
	x = child_x[i];
	y = child_y[i];
	sx = child_sx[i];
	sy = child_sy[i];

	if (x==E) { x = empty_x[i]; }
	if (y==E) { y = empty_y[i]; }
	if (sx==E) { sx = empty_x[i]; }
	if (sy==E) { sy = empty_y[i]; }

	std::string next = next_path(path, i);
	w->SetPos(next, x, y);
	w->SetSize(next, sx,sy);
      }
  }
  virtual std::string Page(std::string path) { 
    std::cout << "WidgetForward Page:" << path << std::endl;
    std::string s = std::string("<div id=\"") + ids[path] + "\">\n";
    int ss = vec.size();
    for(int i=0;i<ss;i++)
      {
	Widget *w = vec[i];
	std::string next = next_path(path, i);
	s+=w->Page(next);
      }
    s+="</div>\n";
    return s;
  }
  virtual std::string Script(std::string path)
  {
    std::string s = Widget::Script(path);
    int ss = vec.size();
    for(int i=0;i<ss;i++)
      {
	Widget *w = vec[i];
	std::string next = next_path(path, i);
	s+=w->Script(next);
      }
    return s;
  }
  virtual std::string Css(std::string path)
  {
    std::string s = Widget::Css(path);
    int ss = vec.size();
    for(int i=0;i<ss;i++)
      {
	Widget *w = vec[i];
	std::string next = next_path(path, i);
	s+=w->Css(next);
      }
    return s;
  }
protected:
  int p_x, p_y;
  std::vector<int> child_x;
  std::vector<int> child_y;
  std::vector<int> child_sx;
  std::vector<int> child_sy;
  std::vector<int> child_r;
  std::vector<int> child_b;
  std::vector<int> empty_x;
  std::vector<int> empty_y;
  std::vector<Widget*> vec;
};

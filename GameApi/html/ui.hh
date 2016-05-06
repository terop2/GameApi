//#include "data.hh"

#include <string>
#include <vector>
#include <utility>

// DLL ENTRY POINTS
class Item
{
public:
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
int num_displays();
void display(int i, int disp);
// END OF DLL ENTRY POINTS

struct W { 
  int id;
};

W empty();
W list_y(std::vector<W> v, int gap_y);
W list_x(std::vector<W> v, int gap_x);
W grid_y(std::vector<W> v, int sx, int gap_x, int gap_y);
W layer(std::vector<W> v);
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

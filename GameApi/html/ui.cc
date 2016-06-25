
#include "ui.hh"
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include <functional>

std::string hexify(std::string s)
{
  std::string res;
  int ss = s.size();
  for(int i=0;i<ss;i++)
    {
      unsigned char c = s[i];
      unsigned char c2 = c & 0xf;
      unsigned char c3 = c & 0xf0;
      c3>>=4;
      const char *chrs = "0123456789ABCDEF";
      res+=chrs[c3];
      res+=chrs[c2];
    }
  return res;
}
std::string unhexify(std::string s)
{
  std::string res;
  int ss = s.size();
  for(int i=0;i<ss;i+=2)
    {
      int val = -1;
      int val2 = -1;
      char c1 = s[i];
      char c2 = s[i+1];
      const char *chrs = "0123456789ABCDEF";
      for(int i1=0;i1<16;i1++)
	{
	  if (c1==chrs[i1]) { val=i1; }
	  if (c2==chrs[i1]) { val2=i1; }
	}
      res+=char((val<<4) + val2);
    }
  return res;
}




std::string Pixels(int val)
{
  std::stringstream ss;
  ss << val << "px";
  return ss.str();
}
std::string Colour(unsigned int colour)
{
  std::stringstream ss;
  int r = colour & 0xff;
  int g = (colour & 0xff00) >> 8;
  int b = (colour & 0xff0000) >> 16;
  int a = (colour & 0xff000000) >> 24;
  float aa = float(a)/255.0;
  ss << "rgba(" << r << "," << g << "," << b << ", " << aa << ")" << std::dec;
  return ss.str();
}

std::string unique_id_apiitem()
{
  static int id = 0;
  id++;
  std::stringstream ss;
  ss << id;
  return std::string("I") + ss.str();
}



// GLOBAL VARIABLE
std::vector<Widget*> envimpl;

W add_w(Widget *w)
{
  envimpl.push_back(w);
  W ww;
  ww.id = envimpl.size()-1;
  return ww;
}
Widget *find_w(W w)
{
  Widget *i = envimpl[w.id];
  return i;
}



template<class T>
class FromStreamClass2
{
public:
  T from_stream(std::string s)
  {
    T t;
    std::stringstream is(unhexify(s));
    is >> t;
    return t;
  }
};

template<>
class FromStreamClass2<std::string>
{
public:
  std::string from_stream(std::string s)
  {
    return unhexify(s);
  }
};


template<>
class FromStreamClass2<unsigned int>
{
public:
  unsigned int from_stream(std::string s)
  {
  unsigned int bm;
  //char c;
  //is >> c;
  //is >> c;
  std::stringstream is(unhexify(s));
  is >> std::hex >> bm >> std::dec;
  return bm;
  }
};


template<class T>
class FromStreamClass2<std::vector<T>>
{
public:
  std::vector<T> from_stream(std::string sk)
  {
    std::string s = unhexify(sk);
    std::cout << "Vector" << std::endl;
    std::vector<T> vec;
    if (s.size()<2)
      {
	std::cout << "from_stream length problem" << std::endl;
	return vec;
      }
    char c = s[0];
    if (c!='[') { std::cout << "from_stream parse error on std::vector" << std::endl; return vec; }

    FromStreamClass2<T> cls;
    int ss = s.size();
    std::string next;
    int prev = 1;
    for(int i=1;i<ss;i++)
      {
	if (s[i]==',' || s[i]==']')
	  {
	    std::string substr = s.substr(prev, i-prev);
	    T t = cls.from_stream(hexify(substr));
	    vec.push_back(t);
	    prev=i+1;
	    if (s[i]==']') { break; }
	  }
      }
    return vec;
  } 
};



#define MACRO(lab) \
template<> \
class FromStreamClass2<lab> \
{ \
public:\
  lab from_stream(std::string s)\
  {\
  lab bm;\
  std::stringstream is(unhexify(s));		\
  is >> bm.id;\
  return bm;\
  }\
};
MACRO(W);

template<typename T> T from_stream22(std::stringstream &is)
{
  FromStreamClass2<T> cls;
  std::string s;
  is >> s;
  return cls.from_stream(s);
}

template<class RT, class... P>
int funccall(RT (fptr)(P...), std::vector<std::string> s, std::vector<std::string> param_name)
{
#if 0
  // EXECUTEENV NOT USED
  int s3 = s.size();
  for(int i=0;i<s3;i++)
    {
      std::string pn = param_name[i];
      int s4 = e.names.size();
      for(int j=0;j<s4;j++)
	{
	  std::string n = e.names[j];
	  std::string v = e.values[j];
	  if (pn == n)
	    {
	      s[i] = v;
	    }
	}
      
    }
#endif

  std::stringstream ss;
  int s2 = s.size();
#ifndef EMSCRIPTEN
  for(int i=s2-1;i>=0;i--)
    {
      ss << hexify(s[i]) << " ";
    }
#else
  for(int i=0;i<s2;i++)
    {
      ss << hexify(s[i]) << " ";
    }
#endif
  std::cout << "FuncCall: " << ss.str() << std::endl;

  std::stringstream ss2(ss.str());
  
  //T *ptr = &(ev.*api);
  RT val = (*fptr)(from_stream22<P>(ss2)...);
  std::cout << "FuncCall returning: " << val.id << std::endl;
  return val.id;
}


template<class RT, class... P>
class ApiItem : public Item
{
public:
  ApiItem(RT (*fptr)(P...),
	  std::string name,
	  std::vector<std::string> param_name,
	  std::vector<std::string> param_type,
	  std::vector<std::string> param_default,
	  std::string ret_type,
	  std::string func_name) : fptr(fptr), name(name), param_name(param_name),
				   param_type(param_type), param_default(param_default), return_type(ret_type), func_name(func_name) { }
  std::string Name() const { return name; }
  int ParamCount() const { return param_name.size(); }
  std::string ParamName(int p) const { return param_name[p]; }
  std::string ParamType(int p) const { return param_type[p]; }
  std::string ParamDefault(int p) const { return param_default[p]; }
  std::string ReturnType() const { return return_type; }
  int Execute(std::vector<std::string> params)
  {
    return funccall(fptr, params, param_name);
  }
  std::pair<std::string,std::string> CodeGen(std::vector<std::string> params, std::vector<std::string> param_names)
  {
    std::string s;
    int ss = params.size();
   for(int i=0;i<ss;i++)
      {
	s+= params[i];
      }

    s+= return_type;
    s+= " ";
    std::string id = unique_id_apiitem();
    s+= id;
    s+= "=";
    s+= func_name;
    s+= "(";
    int sk = param_names.size();
    for(int i=0;i<sk;i++)
      {
	s+= param_names[i];
	if (i!=int(param_names.size())-1) s+=",";
      }
    s+=");\n";
    return std::make_pair(id, s);

  }
private:
  RT (*fptr)(P...);
  std::string name;
  std::vector<std::string> param_name;
  std::vector<std::string> param_type;
  std::vector<std::string> param_default;
  std::string return_type;
  std::string func_name;
};

template<class RT, class... P>
Item* ApiItemF(RT (*fptr)(P...),
	       std::string name,
	       std::vector<std::string> param_name,
	       std::vector<std::string> param_type,
	       std::vector<std::string> param_default,
	       std::string return_type,
	       std::string func_name)
{
  return new ApiItem<RT,P...>(fptr, name, param_name, param_type, param_default, return_type, func_name);
}

// DLL ENTRY POINTS
std::string api_name() { return "WebApi"; }
int num_displays() { return 1; }
std::string type_symbol()
{
  return "W";
}
void display(int i, int disp)
{
  switch(disp)
    {
    case 0:
      W wid = { i };
      std::string page = gen_html_page(wid);
      std::ofstream f("tst.html");
      f << page;
      f.close();
      system("start tst.html");
      break;
    }
}
int add(void *w)
{
  Widget *ww = (Widget*)w;
  W ww2 = add_w(ww);
  return ww2.id;
}
void *find(int i)
{
  W w;
  w.id = i;
  Widget *w2 = find_w(w);
  return (void*)w2;
}
std::vector<Item*> functions()
{
  std::vector<Item*> vec;
  vec.push_back(ApiItemF(&empty, 
			 "wb_empty", 
			 { }, // param names 
			 { }, // param types
			 { }, // param default
			 "W", // return type
			 "empty"));
  vec.push_back(ApiItemF(&list_y,
			 "wb_list_y",
			 { "vec", "gap_y" },
			 { "[W]", "int" },
			 { "", "0" },
			 "W",
			 "list_y"));
  vec.push_back(ApiItemF(&list_x,
			 "wb_list_x",
			 { "vec", "gap_x" },
			 { "[W]", "int" },
			 { "", "0" },
			 "W",
			 "list_x"));
  vec.push_back(ApiItemF(&grid_y,
			 "wb_grid_y",
			 { "vec", "sx", "gap_x", "gap_y" },
			 { "[W]", "int", "int", "int" },
			 { "", "3", "0", "0" },
			 "W",
			 "grid_y"));
  vec.push_back(ApiItemF(&layer,
			 "wb_layer",
			 { "vec" },
			 { "[W]" },
			 { "" },
			 "W",
			 "layer"));
  vec.push_back(ApiItemF(&image,
			 "wb_image",
			 { "url", "size_x", "size_y" },
			 { "std::string", "int", "int" },
			 { "http://", "100", "100" },
			 "W",
			 "image"));
  vec.push_back(ApiItemF(&label,
			 "wb_label",
			 { "label", "size_x", "size_y", "font_size", "font_colour", "align" },
			 { "std::string", "int", "int", "int", "unsigned int", "int" },
			 { "Short Label", "200", "30", "30", "ff000000", "0" },
			 "W",
			 "label"));
  vec.push_back(ApiItemF(&link,
			 "wb_link",
			 { "url", "label", "size_x", "size_y", "font_size", "font_colour", "align" },
			 { "std::string", "std::string", "int", "int", "int", "unsigned int", "int" },
			 { "http://", "Link", "100", "30", "30", "ff000000", "0" },
			 "W",
			 "link"));

  vec.push_back(ApiItemF(&image_placeholder,
			 "wb_placeholder",
			 { "size_x", "size_y" },
			 { "int", "int" },
			 { "200", "200" },
			 "W",
			 "image_placeholder"));
  vec.push_back(ApiItemF(&gradient,
			 "wb_gradient",
			 { "id", "size_x", "size_y" },
			 { "int", "int", "int" },
			 { "0", "100", "100" },
			 "W",
			 "gradient"));
  vec.push_back(ApiItemF(&margin,
			 "wb_margin",
			 { "item", "left", "top", "right", "bottom" },
			 { "W", "int", "int", "int", "int" },
			 { "", "2", "2", "2", "2" },
			 "W",
			 "margin"));
  vec.push_back(ApiItemF(&right_align,
			 "wb_ralign",
			 { "item", "sx" },
			 { "W", "int" },
			 { "", "100" },
			 "W",
			 "right_align"));
  vec.push_back(ApiItemF(&center_align,
			 "wb_calign",
			 { "item", "sx" },
			 { "W", "int" },
			 { "", "100" },
			 "W",
			 "center_align"));
  vec.push_back(ApiItemF(&center_y,
			 "wb_center_y",
			 { "item", "sy" },
			 { "W", "int" },
			 { "", "100" },
			 "W",
			 "center_y"));
  vec.push_back(ApiItemF(&hide,
			 "wb_hide",
			 { "widget" },
			 { "W" },
			 { "" },
			 "W",
			 "hide"));
#if 0
  vec.push_back(ApiItemF(&show_button,
			 "wb_show_button",
			 { "content", "hidden" },
			 { "W", "W" },
			 { "", "" },
			 "W",
			 "show_button"));
  vec.push_back(ApiItemF(&hide_button,
			 "wb_hide_button",
			 { "content", "hidden" },
			 { "W", "W" },
			 { "", "" },
			 "W",
			 "hide_button"));
#endif
  vec.push_back(ApiItemF(&form,
			 "wb_form",
			 { "widget", "is_get", "script_url" },
			 { "W", "bool", "std::string" },
			 { "", "true", "http://" },
			 "W",
			 "form"));
  vec.push_back(ApiItemF(&submit_button,
			 "wb_submit",
			 { "label", "size_x", "size_y" },
			 { "std::string", "int", "int" },
			 { "Submit", "300", "100" },
			 "W",
			 "submit_button"));
  vec.push_back(ApiItemF(&editor,
			 "wb_editor",
			 { "name", "value", "placeholder", "size_x", "size_y", "font_size", "font_colour", "align" },
			 { "std::string", "std::string", "std::string", "int", "int", "int", "unsigned int", "int" },
			 { "addr", "a", "Edit this", "300", "30", "30", "ff000000", "0" },
			 "W",
			 "editor"));
  vec.push_back(ApiItemF(&password_editor,
			 "wb_passwd_editor",
			 { "name", "value", "placeholder", "size_x", "size_y", "font_size", "font_colour", "align" },
			 { "std::strin", "std::string", "std::string", "int", "int", "int", "unsigned int", "int" },
			 { "passwd", "", "Enter Password", "100", "30", "30", "ff000000", "0" },
			 "W",
			 "password_editor"));
  
  return vec;
}
// END OF DLL ENTRY POINTS

class EmptyWidget : public Widget
{
};

class LabelWidget : public Widget
{
public:
  LabelWidget(std::string str, int size_x, int size_y, int font_size, unsigned int font_colour, int align) : str(str), size_x(size_x), size_y(size_y), font_size(font_size), font_colour(font_colour), align(align) { }
  int preferred_width(std::string path) { return size_x; }
  int preferred_height(std::string path) { return std::max(size_y * num_lines(), size_y); }
  int num_lines() const {
    int lines = 1;
    int s = str.size();
    for(int i=0;i<s;i++)
      {
	if (str[i]=='\n') lines++;
      }
    return lines;
  }
  std::string Page(std::string path)
  {
    std::cout << "Label Page:" << path << std::endl;

    std::string s = std::string("<div id=\"") + ids[path] + "\">";
    int ss = str.size();
    int prev = 0;
    for(int i=0;i<ss;i++)
      {
	if (str[i]=='\n')
	  {
	    std::string line = str.substr(prev, i-prev);
	    s+=line;
	    s+="<br>";
	    prev=i+1;
	  }
      }
    std::string rest = str.substr(prev, ss-prev);
    s+=rest;
    s+="</div>\n";
    return s;
  }
  virtual std::string Css(std::string path)
  {
    return Widget::Css(path);
  }
  virtual std::string CssProp(std::string path)
  {
    std::string s = cssprop[path];
    s+="font-size: " + Pixels(font_size) + ";\n";
    s+="line-height: " + Pixels(size_y) + ";\n";
    s+="color: " + Colour(font_colour) + ";\n";
    switch(align) {
    case 0:
    default:
      s+="text-align: left;\n";
      break;
    case 1:
      s+="text-align: center;\n";
      break;
    case 2:
      s+="text-align: right;\n";
      break;
    };
    return s;
  }
protected:
  std::string str;
  int size_x, size_y;
  int font_size;
  unsigned int font_colour;
  int align;
};

class LinkWidget : public LabelWidget
{
public:
  LinkWidget(std::string url, std::string str, int size_x, int size_y, int font_size, unsigned int font_colour, int align) : LabelWidget(str, size_x, size_y, font_size, font_colour, align), url(url) { }
  std::string Page(std::string path)
  {
    std::cout << "Link Page:" << path << std::endl;

    std::string s = std::string("<div id=\"") + ids[path] + "\">";
    s+="<a href=\"" + url + "\">";

    int ss = str.size();
    int prev = 0;
    for(int i=0;i<ss;i++)
      {
	if (str[i]=='\n')
	  {
	    std::string line = str.substr(prev, i-prev);
	    s+=line;
	    s+="<br>";
	    prev=i+1;
	  }
      }
    std::string rest = str.substr(prev, ss-prev);

    s+= rest;
    s+="</a>";
    s+="</div>\n";
    return s;
  }
  std::string Css(std::string path)
  {
    std::string s = LabelWidget::Css(path);
    s+="div#" + ids[path] + " a {\n";
    s+="color: " + Colour(font_colour) + ";\n";
    s+="}\n";
    return s;
  }
private:
  std::string url;
};

W empty()
{
  return add_w(new EmptyWidget);
}
W list_y(std::vector<W> v, int gap_y)
{
  int s = v.size();
  int p_x=0;
  int p_y=0;
  for(int i=0;i<s;i++)
    {
      Widget *w = find_w(v[i]);
      p_x = std::max(p_x,w->preferred_width(""));
      p_y += w->preferred_height("")+gap_y;
    }
  p_y-=gap_y;

  WidgetForward *wf = new WidgetForward(p_x,p_y);
  int t = 0;
  for(int i=0;i<s;i++)
    {
      Widget *w = find_w(v[i]);
      int h = w->preferred_height("");
      int ww = w->preferred_width("");
      wf->push_back(w, 0,t, ww, h, E, E);
      t+=h+gap_y;
    }
  return add_w(wf);
}


W list_x(std::vector<W> v, int gap_x)
{
  int s = v.size();

  int p_x=0;
  int p_y=0;
  for(int i=0;i<s;i++)
    {
      Widget *w = find_w(v[i]);
      p_x += w->preferred_width("") + gap_x;
      p_y = std::max(p_y,w->preferred_height(""));
    }

  WidgetForward *wf = new WidgetForward(p_x,p_y);
  int x = 0;
  for(int i=0;i<s;i++)
    {
      Widget *w = find_w(v[i]);
      int ww = w->preferred_width("");
      int hh = w->preferred_height("");
      wf->push_back(w, x,0, ww, hh, E, E);
      x+=ww+gap_x;
    }
  return add_w(wf);
}

W label(std::string str, int size_x, int size_y, int font_size, unsigned int font_colour, int align)
{
  return add_w(new LabelWidget(str,size_x, size_y, font_size, font_colour, align));
}

W link(std::string url, std::string str, int size_x, int size_y, int font_size, unsigned int font_colour, int align)
{
  return add_w(new LinkWidget(url, str,size_x, size_y, font_size, font_colour, align));
}


W layer(std::vector<W> v)
{
  int s = v.size();

  int p_x=0;
  int p_y=0;
  for(int i=0;i<s;i++)
    {
      Widget *w = find_w(v[i]);
      p_x = std::max(p_x,w->preferred_width(""));
      p_y = std::max(p_y,w->preferred_height(""));
    }

  WidgetForward *wf = new WidgetForward(p_x,p_y);
  for(int i=0;i<s;i++)
    {
      Widget *w = find_w(v[i]);
      wf->push_back(w, 0,0, E, E, 0, 0);
      std::stringstream ss;
      ss << i;
      w->SetCssProp("", std::string("z-index: ") + ss.str() + ";\n"); 
    }
  return add_w(wf);
}
std::string gen_html_page(W w)
{
  Widget *ww = find_w(w);
  ww->make_unique("");
  int width = ww->preferred_width("");
  int height = ww->preferred_height("");
  ww->SetPos("", 0,0);
  ww->SetSize("", width, height);
  std::string s = "<!DOCTYPE html>\n";
  s+="<html>\n";
  s+="<head>\n";
  s+="<style>\n";
  s+=ww->Css("");
  s+="</style>\n";
  s+="</head>\n";
  s+="<body>\n";
  s+=ww->Page("");
  s+="<script>\n";
  s+=ww->Script("");
  s+="</script>\n";
  s+="</body>\n";
  s+="</html>\n";
  return s;
}

class ImageWidget : public Widget
{
public:
  ImageWidget(std::string url, int size_x, int size_y) : url(url), size_x(size_x), size_y(size_y) { }
  int preferred_width(std::string path) { return size_x; }
  int preferred_height(std::string path) { return size_y; }
  std::string Page(std::string path) {
    std::cout << "Image Page:" << path << std::endl;

    std::string s = std::string("<div id=\"") + ids[path] + "\">\n";
    std::stringstream sx;
    sx << size_x;
    std::stringstream sy;
    sy << size_y;

    s+="<img src=\"" + url + "\" width=" + sx.str() +" height=" + sy.str() + "></img>\n";
    s+="</div>\n";
    return s;
  }
  std::string Css(std::string path)
  {
    return Widget::Css(path);
  }
private:
  std::string url;
  int size_x;
  int size_y;
};

W image(std::string url, int size_x, int size_y)
{
  return add_w(new ImageWidget(url, size_x, size_y));
}
W gradient(int i, int size_x, int size_y)
{
  std::string url;
  switch(i) {
  case 0:
  default:
    url="http://sivut.koti.soon.fi/~terop/brown_gradient.png";
    break;
  };
  return image(url, size_x, size_y);
}
int grid_x_pos(int val, int sx)
{
  int ty = val / sx;
  int tx = val - ty*sx;
  return tx;
}
int grid_y_pos(int val, int sx)
{
  int ty = val / sx;
  //int tx = val - ty*sx;
  return ty;
}
W grid_y(std::vector<W> v, int sx, int gap_x, int gap_y)
{
  int s = v.size();
  std::vector<int> p_x;
  p_x.resize(sx);
  std::vector<int> p_y;
  int pp = grid_y_pos(v.size(), sx) + 1;
  p_y.resize(pp);

  for(int i=0;i<s;i++)
    {
      Widget *w = find_w(v[i]);
      int px = grid_x_pos(i, sx);
      int py = grid_y_pos(i, sx);
      p_x[px]= std::max(p_x[px], w->preferred_width("")+gap_x);
      p_y[py]= std::max(p_y[py], w->preferred_height("")+gap_y);
    }
  p_x[p_x.size()-1]-=gap_x;
  p_y[p_y.size()-1]-=gap_y;
  int pp_x = 0;
  int s2 = p_x.size();
  for(int i=0;i<s2;i++)
    {
      pp_x += p_x[i];
    }
  int s3 = p_y.size();
  int pp_y = 0;
  for(int i=0;i<s3;i++)
    {
      pp_y += p_y[i];
    }
  WidgetForward *wf = new WidgetForward(pp_x, pp_y);
  int ty = 0;
  for(int i=0;i<s3;i++)
    {
      int tx = 0;
      for(int j=0;j<s2;j++)
	{
	  int index = i*sx+j;
	  Widget *w;
	  if (index < v.size())
	    w = find_w(v[index]);
	  else
	    w = find_w(empty());
	  int ww = p_x[j];
	  int hh = p_y[i];
	  wf->push_back(w, tx, ty, ww, hh, E,E);
	  tx+=ww;
	}
      ty+=p_y[i];
    }
  return add_w(wf);
}

class ScriptButton : public WidgetForward
{
public:
  ScriptButton(Widget *w, std::function<std::string (std::string)> f, std::function<std::string (std::string)> funcname, std::string shared_id) : WidgetForward(w->preferred_width(""), w->preferred_height("")), f(f), funcname(funcname), w(w), shared_id(shared_id)
  {
    int ww = w->preferred_width("");
    int hh = w->preferred_height("");
    push_back(w, 0,0, ww, hh, E,E);

  } 
  std::string Page(std::string path)
  {
    std::cout << "Script Page:" << path << std::endl;

    std::string s = std::string("<div id=\"") + ids[path] + "\" onClick=\"" + funcname(ids[path]) + "()\">\n";
    std::string next = next_path(path, 0);
    s+=w->Page(next);
    s+="</div>\n";
    return s;
  }
  std::string Script(std::string path)
  {
    std::string s = WidgetForward::Script(path);
    return s+f(ids[path]);
  }
private:
  std::string shared_id;
  std::function<std::string (std::string)> f;
  std::function<std::string (std::string)> funcname;
  Widget *w;
};
W margin(W widget, int left, int top, int right, int bottom)
{
  Widget *w = find_w(widget);
  WidgetForward *w2 = new WidgetForward(w->preferred_width("")+left+right, w->preferred_height("")+top+bottom);
  w2->push_back(w, left,top,E,E, right,bottom);
  return add_w(w2);

}
W right_align(W item, int sx)
{
  Widget *w = find_w(item);
  int size = w->preferred_width("");
  return margin(item, sx-size,0,0,0);
}
W center_align(W item, int sx)
{
  Widget *w = find_w(item);
  int size = w->preferred_width("");
  return margin(item, (sx-size)/2,0,(sx-size)/2,0);
}
W center_y(W item, int sy)
{
  Widget *w = find_w(item);
  int size = w->preferred_height("");
  return margin(item, 0, (sy-size)/2, 0, (sy-size)/2);
}

W hide(W widget)
{
  Widget *w = find_w(widget);
  WidgetForward *w2 = new WidgetForward(w->preferred_width(""), w->preferred_height(""));
  w2->push_back(w, 0,0,E,E, 0,0);
  w2->SetCssProp("", "display: none;\n");
  return add_w(w2);
}
#if 0

W show_button(W button_content, W hidden_element)
{
  Widget *w = find_w(button_content);
  Widget *h = find_w(hidden_element);
  std::string id = unique_id_apiitem();
  std::string h_id = h->Id();
  std::string shared_id = unique_id_apiitem();
  auto f = [shared_id](std::string id) {
  std::string script =
    "function myFunc" + id + "() {\n"
  "   document.getElementById(\"" + shared_id + "\").style.display = \"block\";\n"
  "}\n";
  return script;
  };

  auto f2 = [](std::string id) {
    std::string funcname = "myFunc" + id;
    return funcname;
  };
  ScriptButton *but = new ScriptButton(w,f,f2,shared_id);

  return add_w(but);
}

W hide_button(W button_content, W hidden_element)
{
  Widget *w = find_w(button_content);
  Widget *h = find_w(hidden_element);
  std::string id = unique_id_apiitem();
  std::string h_id = h->Id();
  auto f = [h](std::string id) {
    std::string script =
    "function myFunc" + id + "() {\n"
    "   document.getElementById(\"" + h->Id() + "\").style.display = \"none\";\n"
    "}\n";
    return script;
  };
  auto f2 = [](std::string id) {
    std::string funcname = "myFunc" + id;
    return funcname;
  };
  
  return add_w(new ScriptButton(w, f, f2));
}
#endif
W image_placeholder(int size_x, int size_y)
{
  W img = image("http://sivut.koti.soon.fi/~terop/rectangle.png", size_x, size_y);
  std::stringstream ss;
  ss << size_x << "x" << size_y;
  int font_size = 30;
  int width = 100;
  if (size_x < 70 || size_y < 30) { font_size = 15; width=50; }
  W lab = label(ss.str(), width, font_size, font_size, 0xff000000,1);
  W lab2 = center_y(lab, size_y);
  W lab3 = center_align(lab2, size_x);
  W lay = layer(std::vector<W>{ img, lab3 });
  W lay2 = child_size(lay);
  return lay2;
}
class ChildSizeWidget : public WidgetForward
{
public:
  ChildSizeWidget(Widget *w) : WidgetForward(w->preferred_width(""), w->preferred_height("")) 
  {
    int ww = w->preferred_width("");
    int hh = w->preferred_height("");
    push_back(w, 0,0,ww,hh,E,E);
  }
};
W child_size(W w)
{
  Widget *ww = find_w(w);
  return add_w(new ChildSizeWidget(ww));
}
class FormWidget : public WidgetForward
{
public:
  FormWidget(Widget* w, std::string url, bool is_get) : WidgetForward(w->preferred_width(""), w->preferred_height("")), w(w), url(url), is_get(is_get) 
  { 
    int ww = w->preferred_width("");
    int hh = w->preferred_height("");
    push_back(w, 0,0, ww, hh, E, E);
  }
  std::string Page(std::string path)
  {
    std::cout << "Form Page:" << path << std::endl;

    std::string s = std::string("<form action=\"") + url + "\" method=\"";
    if (is_get)
      {
	s+="get";
      }
    else
      {
	s+="post";
      }
    s+="\">\n";
    s += std::string("<div id=\"") + ids[path] + "\">\n";
    
    std::string next = next_path(path,0);
    s+=w->Page(next);

    s+="</div>\n";
    s+="</form>\n";
    return s;
  }
private:
  Widget* w;
  std::string url;
  bool is_get;
};
W form(W vec, bool is_get, std::string script_url)
{
  Widget *w = find_w(vec);
  return add_w(new FormWidget(w, script_url, is_get));
}
class SubmitWidget : public Widget
{
public:
  SubmitWidget(std::string label, int size_x, int size_y) : label(label), size_x(size_x), size_y(size_y) { }
  int preferred_width(std::string path) { return size_x; }
  int preferred_height(std::string path) { return size_y; }
  std::string Page(std::string path) {
    std::cout << "Submit Page:" << path << std::endl;

    std::string s = "";
    s+="<input id=\"" + ids[path] + "\" type=\"submit\" value=\"" + label + "\">";
    return s;
  }
private:
  std::string label;
  int size_x, size_y;
};
W submit_button(std::string label, int size_x, int size_y)
{
  return add_w(new SubmitWidget(label, size_x, size_y));
}

class EditorWidget : public Widget
{
public:
  EditorWidget(std::string name, std::string value, std::string placeholder, int size_x, int size_y, int font_size, unsigned int font_colour, int align) : name(name), value(value), placeholder(placeholder), size_x(size_x), size_y(size_y), font_size(font_size), font_colour(font_colour), align(align) { }
  int preferred_width(std::string path) { return size_x; }
  int preferred_height(std::string path) { return size_y; }
  std::string Page(std::string path) {
    std::cout << "Editor Page:" << path << std::endl;

    std::string s = "";
    s+="<input id=\"" + ids[path] + "\" type=\"text\" placeholder=\"" + placeholder + "\" value=\"" + value + "\" name=\"" + name + "\">";
    return s;
  }
  virtual std::string CssProp(std::string path)
  {
    std::string s = cssprop[path];
    s+="font-size: " + Pixels(font_size) + ";\n";
    s+="color: " + Colour(font_colour) + ";\n";
    switch(align) {
    case 0:
    default:
      s+="text-align: left;\n";
      break;
    case 1:
      s+="text-align: center;\n";
      break;
    case 2:
      s+="text-align: right;\n";
      break;
    };
    s+="border-width: 0px 0px 0px 0px;\n";
    s+="padding: 0px 0px 0px 0px;\n";
    return s;
  }

private:
  std::string name;
  std::string value;
  std::string placeholder;
  int size_x, size_y;
  int font_size;
  unsigned int font_colour;
  int align;
};

W editor(std::string name, std::string value, std::string placeholder, int size_x, int size_y, int font_size, unsigned int font_colour, int align)
{
  return add_w(new EditorWidget(name, value, placeholder, size_x, size_y,font_size, font_colour, align));
}

class PasswordEditorWidget : public Widget
{
public:
  PasswordEditorWidget(std::string name, std::string value, std::string placeholder, int size_x, int size_y, int font_size, unsigned int font_colour, int align) : name(name), value(value), placeholder(placeholder), size_x(size_x), size_y(size_y), font_size(font_size), font_colour(font_colour), align(align) { }
  int preferred_width(std::string path) { return size_x; }
  int preferred_height(std::string path) { return size_y; }
  std::string Page(std::string path) {
    std::cout << "Passwd Page:" << path << std::endl;

    std::string s = "";
    s+="<input id=\"" + ids[path] + "\" type=\"password\" placeholder=\"" + placeholder + "\" value=\"" + value + "\" name=\"" + name + "\">";
    return s;
  }
  virtual std::string CssProp(std::string path)
  {
    std::string s = cssprop[path];
    s+="font-size: " + Pixels(font_size) + ";\n";
    s+="color: " + Colour(font_colour) + ";\n";
    switch(align) {
    case 0:
    default:
      s+="text-align: left;\n";
      break;
    case 1:
      s+="text-align: center;\n";
      break;
    case 2:
      s+="text-align: right;\n";
      break;
    };
    s+="border-width: 0px 0px 0px 0px;\n";
    s+="padding: 0px 0px 0px 0px;\n";
    return s;
  }

private:
  std::string name;
  std::string value;
  std::string placeholder;
  int size_x, size_y;
  int font_size;
  unsigned int font_colour;
  int align;
};


W password_editor(std::string name, std::string value, std::string placeholder, int size_x, int size_y, int font_size, unsigned int font_colour, int align)
{
  return add_w(new PasswordEditorWidget(name, value, placeholder, size_x, size_y,font_size, font_colour, align));
}

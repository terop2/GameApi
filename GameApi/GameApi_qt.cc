
#include "GameApi_h.hh"

std::string strip_spaces(std::string s)
{
  int start=0;
  while(s[start]==' '||s[start]=='\n') start++;
  int end=s.size()-1;
  while(s[end]==' '||s[end]=='\n') end--;
  return s.substr(start,end-start+1);

}

void async_load_url(GameApi::Env &e, std::string url, std::string homepage)
{
  e.async_load_url(url,homepage);
}
std::vector<unsigned char> *get_loaded_async_url(GameApi::Env &e, std::string url)
{
  return e.get_loaded_async_url(url);
}


bool is_import(std::string line) { 
  std::stringstream ss(line);
  std::string word;
  ss >> word;
  if (word=="import") return true;
  return false; 
}
bool is_component(std::string line) {
  if (line.size()>0) {
    char ch = line[line.size()-1];
    //std::cout << "CH:" << line << ";;" << ch << std::endl;
    if (ch=='{') return true;
    return false;
  }
  return false;
}
bool is_component_end(std::string line) {
  if (line.size()>0) {
    char ch = line[line.size()-1];
    //std::cout << "CH2:" << line << ";;" << ch << std::endl;
    if (ch=='}') return true;
    return false;
  }
  return false;
}
int find_ch(std::string s, char ch)
{
  int ss = s.size();
  for(int i=0;i<ss;i++) {
    if (s[i]==ch) return i;
  }
  return -1;
}

std::pair<std::string,std::string> split_attr(std::string attr, bool &success)
{
  int ch = find_ch(attr, ':');
  if (ch==-1) {success=false; return std::make_pair("",""); }
  std::string first = attr.substr(0,ch);
  std::string second = attr.substr(ch+1,attr.size()-ch-1);
  success = true;
  return std::make_pair(first,second);
}
bool is_attribute(std::string line)
{
  if (is_component(line)) return false;
  bool success = false;
  std::pair<std::string,std::string> p = split_attr(line, success);
  return success;
}
bool is_property(std::string line) { return false; }
bool is_pragma(std::string line) { return false; }
bool is_comment_start(std::string line)
{
  int s = line.size();
  int pos = -1;
  for(int i=0;i<s-1;i++)
    {
      if (line[i]=='/' && line[i+1]=='*') pos=i;
    }
  if (pos==-1) return false;
  int pos2 = -1;
  for(int i=0;i<s-1;i++)
    {
      if (line[i]=='*' && line[i+1]=='/') pos2=i;
    }
  if (pos2!=-1) return false;
  return true;
}

bool is_comment_end(std::string line)
{
  int s = line.size();
  int pos = -1;
  for(int i=0;i<s-1;i++)
    {
      if (line[i]=='*' && line[i+1]=='/') pos=i;
    }
  if (pos==-1) return false;
  return true;

}

class QMLRoot : public QMLData
{
  virtual void Prepare() { }
  virtual QMLData* Parent() const { return 0; }
  virtual std::string Type() const { return "root"; }
  virtual int NumImports() const { return 0; }
  virtual std::string Import(int import_i) const { return ""; }
  virtual int NumAttribs() const { return 0; }
  virtual std::string AttrName(int attr_i) const { return ""; }
  virtual std::string AttrValue(int attr_i) const { return ""; }
  virtual int NumChildren() const { return m_child_map.size(); }
  virtual std::string Name(int child_i) const { return "root"; }
  virtual std::string Type(int child_i) const { return "root"; }
  virtual QMLData *Children(int child_i) const { return m_child_map[child_i]; }
public:
  std::vector<QMLData*> m_child_map;
};

class QMLLoader : public QMLData
{
public:
  QMLLoader() : parent(new QMLRoot) { ((QMLRoot*)parent)->m_child_map.push_back(this); }
  QMLLoader(QMLData *parent) : parent(parent) { ((QMLLoader*)parent)->m_child_map.push_back(this); }
  void set_data(std::vector<unsigned char> vec) { data = vec; }
  virtual void Prepare()
  {
    m_imports=std::vector<std::string>();
    m_import_children = std::vector<QMLData*>();
    m_component_name = std::vector<std::string>();
    m_component_type = std::vector<std::string>();
    m_component_children = std::vector<QMLData*>();
    m_attributes = std::vector<std::pair<std::string,std::string> >();
    m_current_component=std::vector<std::string>();

    std::string s(data.begin(), data.end());
    std::stringstream ss(s);
    std::string line;
    int level=0;
    bool comment=false;
    while(std::getline(ss,line)) {
      //std::cout << "Line: " << line << std::endl;
      if (is_comment_end(line)) { comment=false; }
      if (is_comment_start(line)) { comment=true; }
      if (comment==true) continue;
      if (is_component(line)) { 
	//std::cout << "Level++" << std::endl; 
	level++; }
      if (is_component_end(line)) {
	//std::cout << "Level--" << std::endl;
	level--;
	if (level==0) {
	  //std::cout << "End Component" << std::endl;
	  std::string last = m_current_component.back();
	  m_current_component.pop_back();
	  std::vector<unsigned char> vec(last.begin(),last.end());
	  m_component_children.push_back(new QMLLoader(this));
	  QMLData *dt = m_component_children[m_component_children.size()-1];
	  QMLLoader *load = (QMLLoader*)dt;
	  load->set_data(vec);
	  //load->Prepare();
	}

      }

      if (m_current_component.size()>0)
	{
	  //std::cout << "AppendLine" << std::endl;
	  m_current_component[m_current_component.size()-1]+=line + "\n";
	  continue;
	}
      if (is_import(line)) { 
	//std::cout << "IMPORT" << std::endl;
	m_imports.push_back(line); }
      if (is_component(line)) { 
	//std::cout << "COMPONENT" << std::endl;
	std::string line2 = line.substr(0,line.size()-1-1);
	bool success = false;
	std::pair<std::string,std::string> p = split_attr(line2, success);
	if (success) {
	  m_component_name.push_back(p.first);
	  m_component_type.push_back(p.second);
	} else {
	  m_component_name.push_back("");
	  m_component_type.push_back(line2);
	}
	m_current_component.push_back("");
      }
      if (is_attribute(line)) {
	//std::cout << "ATTRIBUTE" << std::endl;
	bool success = false;
	std::pair<std::string,std::string> p = split_attr(line, success);
	//assert(success);
	m_attributes.push_back(std::make_pair(p.first,p.second));
      }
      if (is_property(line)) { /* TODO */ }
      if (is_pragma(line)) { /* TODO */ }
    }
  }
  virtual QMLData *Parent() const { return parent; }
  virtual std::string Type() const { 
    QMLData *p = Parent();
    QMLLoader *p2a = dynamic_cast<QMLLoader*>(p);
    QMLRoot *p2b = dynamic_cast<QMLRoot*>(p);
    if (p2a) {
      int s = p2a->m_child_map.size();
      for(int i=0;i<s;i++)
	{
	  QMLData *d = p2a->m_child_map[i];
	  if (d==this) { return p->Type(i); }
	}
    } else if (p2b) {
      int s = p2b->m_child_map.size();
      for(int i=0;i<s;i++)
	{
	  QMLData *d = p2b->m_child_map[i];
	  if (d==this) { return p->Type(i); }
	}
    } else { std::cout << "Unknown type in QMLLoader::Type()" << std::endl; }
    return "@";
  }
  virtual int NumImports() const { return m_imports.size(); }
  virtual std::string Import(int import_i) const
  {
    return m_imports[import_i];
  }
  virtual QMLData *ImportChildren(int import_i) const
  {
    return m_import_children[import_i];
  }
  virtual int NumAttribs() const
  {
    return m_attributes.size();
  }
  virtual std::string AttrName(int attr_i) const
  {
    return m_attributes[attr_i].first;
  }
  virtual std::string AttrValue(int attr_i) const
  {
    return m_attributes[attr_i].second;
  }
  virtual int NumChildren() const
  {
    return m_component_children.size();
  }
  virtual std::string Name(int child_i) const { return strip_spaces(m_component_name[child_i]); }
  virtual std::string Type(int child_i) const { return strip_spaces(m_component_type[child_i]); }
  virtual QMLData *Children(int child_i) const
  {
    return m_component_children[child_i];
  }

public:
  std::vector<QMLData*> m_child_map;
private:
  QMLData *parent;

  std::vector<unsigned char> data;
  std::vector<std::string> m_imports;
  std::vector<QMLData*> m_import_children;
  std::vector<std::string> m_component_name;
  std::vector<std::string> m_component_type;
  std::vector<QMLData*> m_component_children;
  std::vector<std::pair<std::string,std::string> > m_attributes;
  std::vector<std::string> m_current_component;
};

class QMLModule : public QMLData
{
public:
  QMLModule(GameApi::Env &e, std::string url, std::string homepage) : e(e), url(url), homepage(homepage) {}
  void Prepare() {
    //std::cout << "qtPrepare" << std::endl;
#ifndef EMSCRIPTEN
    async_load_url(e,url,homepage);
#endif
    std::vector<unsigned char> *vec = get_loaded_async_url(e,url);
    if (!vec) { std::cout << "async not ready!" << std::endl; return; }
    // replace \n\r -> \n
    std::string ss(vec->begin(),vec->end());
    ss.erase(std::remove(ss.begin(),ss.end(),'\r'), ss.end());
    std::string line;
    std::string res;
    std::stringstream sss(ss);
    while(std::getline(sss,line)) { res+=line+"\n"; }
    std::vector<unsigned char> vec2(res.begin(),res.end());
    // replace end
    load.set_data(vec2);
    load.Prepare();
  }
  QMLData *Parent() const { return load.Parent(); }
  std::string Type() const {return load.Type(); }

  virtual int NumImports() const { return load.NumImports(); }
  virtual std::string Import(int import_i) const
  {
    return load.Import(import_i);
  }
  virtual QMLData *ImportChildren(int import_i) const
  {
    return load.ImportChildren(import_i);
  }
  virtual int NumAttribs() const
  {
    return load.NumAttribs();
  }
  virtual std::string AttrName(int attr_i) const
  {
    return load.AttrName(attr_i);
  }
  virtual std::string AttrValue(int attr_i) const
  {
    return load.AttrValue(attr_i);
  }
  virtual int NumChildren() const
  {
    return load.NumChildren();
  }
  virtual std::string Name(int child_i) const { return load.Name(child_i); }
  virtual std::string Type(int child_i) const { return load.Type(child_i); }
  virtual QMLData *Children(int child_i) const
  {
    return load.Children(child_i);
  }

private:
  GameApi::Env &e;
  std::string url;
  std::string homepage;
  QMLLoader load;
};

class PrintQMLData : public FrameBufferLoop
{
public:
  PrintQMLData(QMLData *mod) : mod(mod) {}
  void Prepare() {
    mod->Prepare();
    int s = mod->NumImports();
    for(int i=0;i<s;i++) {
      std::cout << mod->Import(i) << std::endl;
    }
    int s3 = mod->NumAttribs();
    for(int i=0;i<s3;i++) {
      std::cout << mod->AttrName(i) << "::" << mod->AttrValue(i) << std::endl;
    }
    int s4 = mod->NumChildren();
    for(int i=0;i<s4;i++)
      {
	std::cout << "Children: " << i << std::endl;
	std::cout << mod->Name(i) << ":" << mod->Type(i) << std::endl;
	QMLData *dt = mod->Children(i);
	PrintQMLData *dt2 = new PrintQMLData(dt);
	dt2->Prepare();
	std::cout << "EndChildren: " << i << std::endl;
      }
  }
  void handle_event(FrameLoopEvent &e) { }
  void frame(DrawLoopEnv &e) {}
private:
  QMLData *mod;
};
extern std::string gameapi_homepageurl;
GameApi::FML GameApi::LowFrameBufferApi::qml_print(std::string url)
{
  QMLModule *mod = new QMLModule(e,url,gameapi_homepageurl);
  PrintQMLData *dt = new PrintQMLData(mod);
  return add_framemainloop(e, dt);
}

QMLLoop *create_qml_element(QMLData *dt);

void recursive_prepare(QMLData *mod)
{
  mod->Prepare();
  int s = mod->NumChildren();
  for(int i=0;i<s;i++)
    recursive_prepare(mod->Children(i));
}


class QMLCreateNode : public FrameBufferLoop
{
public:
  QMLCreateNode(QMLData *mod) : mod(mod) { }
  void Prepare() {
    // these are QMLLoader's prepares
    recursive_prepare(mod);

    // these are QMLLoop's prepares (does it recursively)
    loop = create_qml_element(mod);
    loop->Prepare();
  }
  void frame(DrawLoopEnv &e)
  {
    loop->frame(e);
  }
  void handle_event(FrameLoopEvent &e)
  {
    loop->handle_event(e);
  }

private:
  QMLData *mod;
  QMLLoop *loop;
};

GameApi::FML GameApi::LowFrameBufferApi::qml_create_node(std::string url)
{
  QMLModule *mod = new QMLModule(e,url,gameapi_homepageurl);
  return add_framemainloop(e,new QMLCreateNode(mod));
}

struct PVec {
  std::vector<std::string> p1;
  std::vector<std::string> p2;
  std::string get_val(std::string key) const
  {
    int s = p1.size();
    for(int i=0;i<s;i++) { if (p1[i]==key) return p2[i]; }
    return "@";
  }
  int get_int_val(std::string key) const {
    std::string s = get_val(key);
    std::stringstream ss;
    int val;
    if (!ss >> val) val=-1;
    return val;
  }
  bool get_bool_val(std::string key) const {
    std::string s = get_val(key);
    std::stringstream ss;
    bool val;
    if (!ss >> val) val=false;
    return val;    
  }
  std::string get_id() const { return get_val("id"); }
  int get_width() const { return get_int_val("width"); }
  int get_height() const { return get_int_val("height"); }
  bool is_visible() const { return get_bool_val("visible"); }
  std::string get_source() const { return get_val("source"); }
  std::string get_image_source() const { return get_val("imageSource"); }
  int get_edge() const { 
    std::string s = get_val("edge");
    if (s=="Qt.TopEdge") { return 0; }
    if (s=="Qt.BottomEdge") { return 1; }
    return -1;
  }
};

class UnknownElement : public QMLLoop
{
public:
  UnknownElement(std::string context, std::string type, PVec vec) : context(context), type(type), vec(vec) { }
  virtual void Prepare()
  {
    std::cout << "UnknownElement: " << context << "." << type << std::endl;
  }
  virtual void handle_event(FrameLoopEvent &e) { }
  virtual void frame(DrawLoopEnv &e) { }
  virtual int NumChildrent() const { return 0; }
  virtual QMLLoop *Children(int i) const { return 0; }
private:
  std::string context, type;
  PVec vec;
};
class UnknownWithChildren : public QMLLoop
{
public:
  UnknownWithChildren(std::string context, std::string type, PVec vec, std::vector<QMLLoop*> children) : context(context), type(type), vec(vec), children(children) { }
  virtual void Prepare()
  {
    std::cout << "Unknown: " << context << "." << type << std::endl;
    PrepareChildren();
  }
  virtual void handle_event(FrameLoopEvent &e)
  {
  }
  virtual void frame(DrawLoopEnv &e)
  {
  }

  virtual int NumChildren() const { return children.size(); }
  virtual QMLLoop *Children(int i) const
  {
    return children[i];
  }
  void PrepareChildren()
  {
    int s = children.size();
    for(int i=0;i<s;i++)
      children[i]->Prepare();
  }
private:
  std::string context;
  std::string type;
  PVec vec;
  std::vector<QMLLoop*> children;
};

QMLLoop *create_qml_element(QMLData *dt)
{
  //std::cout << "create_qml_element" << std::endl;
  std::string type = dt->Type();
  
  // collect attributes
  PVec p_vec;
  QMLData *c = dt;

  int sk = c->NumAttribs();
  for(int i=0;i<sk;i++)
    {
      std::string attr_name = c->AttrName(i);
      std::string attr_value = c->AttrValue(i);
      p_vec.p1.push_back(attr_name);
      p_vec.p2.push_back(attr_value);
    }
  // end collect attribs

  std::vector<QMLLoop*> vec;
  int sl = c->NumChildren();
  //std::cout << "create_qml_element start children" << sl << std::endl;
  for(int i=0;i<sl;i++)
    {
      //std::cout << "Children: " << i << std::endl;
      //std::cout << c->Name(i) << ":" << c->Type(i) << std::endl;
      vec.push_back(create_qml_element(c->Children(i)));
      //std::cout << "EndChildren" << i << std::endl;
    }
  //std::cout << "create_qml_element end children" << sl << std::endl;
  

  int s = type.size();
  int pos = -1;
  for(int i=0;i<s;i++) {
    if (type[i]=='.') pos=i;
  }
  std::string context;
  std::string type2=type;
  if (pos!=-1) {
    context = type.substr(0,pos);
    type2 = type.substr(pos+1,type.size()-1-1);
  }
  if (type2=="ApplicationWindow") {
    return new UnknownWithChildren(context,type2,p_vec,vec);
  }
  if (type2=="Image") {
    return new UnknownWithChildren(context,type2,p_vec,vec);
  }
  if (type2=="NaviButton") {
    return new UnknownWithChildren(context,type2,p_vec,vec);
  }
  if (type2=="StackView") {
    return new UnknownWithChildren(context,type2,p_vec,vec);
  }
  return new UnknownWithChildren(context,type2,p_vec,vec);
}

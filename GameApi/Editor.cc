
#if 0
#define NO_SDL_GLEXT
#include "Editor.hh"
#include <gtkmm.h>
#include <gtkmm/socket.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>


class ModelColumn : public Gtk::TreeModel::ColumnRecord
{
public:
  ModelColumn()
  {
    add(m_col_text);
    add(m_editable_text);
  }
  Gtk::TreeModelColumn<Glib::ustring> m_col_text;
  Gtk::TreeModelColumn<Glib::ustring> m_editable_text;
};


class MyTreeStore : public Gtk::TreeStore
{
public:
  MyTreeStore(ModelColumn &m) : Gtk::TreeStore(m) { }
  void set_value_impl( const iterator & row,
		       int column,
		       const Glib::ValueBase &value)
  {
    Gtk::TreeStore::set_value_impl(row, column, value);
    
    std::cout << "set_value_impl" << std::endl;
  }
};

void UpdateChilds(ModelColumn &m, Glib::RefPtr<Gtk::TreeStore> treeStore, Gtk::TreeModel::iterator i, ExpressionTree<ExpressionNode> *tree, int n2)
{
  int count = tree->t.func->NumParams();
  
  std::cout << "UpdateChilds: " << count << std::endl;

  tree->children.resize( count, 0 );
  
  Gtk::TreeModel::Row r = *i;
  const Gtk::TreeNodeChildren &child = r.children();
  Gtk::TreeModel::iterator iter = child.begin();
  for(int i=0;i<count;i++)
    {
      if (!iter)
	{
	  Gtk::TreeModel::iterator ii = treeStore->append(r.children());
	  Gtk::TreeModel::Row r = *ii;
	  r[m.m_col_text] = (std::string("New Element :: ") + tree->t.func->ParamType(i)).c_str();
	}
      else
	{
	  iter++;
	}
    }
  while(iter != child.end())
    {
      iter = treeStore->erase(iter);
    }
  
}

void AppendData(ModelColumn &m, Glib::RefPtr<Gtk::TreeStore> treeStore, Gtk::TreeModel::iterator i, ExpressionTree<ExpressionNode> *tree, CreationFunction *parentfunc, int param_num)
{
  if (!tree)
    {
      Gtk::TreeModel::Row row = *i;
      //GtkTreeIter *g = row.gobj();
      //g_object_set_data(G_OBJECT(g), "exprtree", tree);
      //g_object_set_data(G_OBJECT(g), "param", (void*)param_num);
      Gtk::TreeModel::iterator iter_child;
      int child_size = row.children().size();
      if (param_num < child_size)
	{
	  iter_child = row.children()[param_num];
	}
      else
	{
	  iter_child = treeStore->append(row.children());
	}
      Gtk::TreeModel::Row child_row = *iter_child;
      child_row[m.m_col_text] = (std::string("Unknown :: ") + parentfunc->ParamType(param_num)).c_str();
      return;
    }
  Gtk::TreeModel::Row row = *i;
  Gtk::TreeModel::iterator ii = i;
  ii++;
  //GtkTreeIter *g = row.gobj();
  //g_object_set_data(G_OBJECT(g), "exprtree", tree);
  //g_object_set_data(G_OBJECT(g), "param", (void*)param_num);
  Gtk::TreeModel::iterator iter_child;
  int child_size2 = row.children().size();
  if (param_num < child_size2)
    {
      iter_child = row.children()[param_num];
    }
  else
    {
      iter_child = treeStore->append(row.children());
    }
  
  Gtk::TreeModel::Row child_row = *iter_child;

  child_row[m.m_col_text] = (tree->t.func->Name() + " :: "+ parentfunc->ReturnType()).c_str();
  int size = tree->children.size();
  for(int i=0;i<size;i++)
    {
      AppendData(m, treeStore, iter_child, tree->children[i], tree->t.func, i);
    }
  
}
class MainWindow;
void CreatePopupMenu(MainWindow *window, GdkEventButton *event, const std::vector<CreationFunction*> &vec, Gtk::TreeModel::iterator iter);

class MyTreeView : public Gtk::TreeView
{
public:
  MyTreeView(MainWindow &w) : w(w) { }
  bool on_button_press_event(GdkEventButton *event);
private:
  MainWindow &w;
};

std::pair<ExpressionTree<ExpressionNode> *, int> FindTreeElement(ExpressionTree<ExpressionNode> *root, Gtk::TreeModel::iterator i, Glib::RefPtr<Gtk::TreeStore> model)
{
  Gtk::TreeModel::Path p = model->get_path(i);
  int size = p.size()-2;
  for(int i=1;i<size;i++)
    {
      int &n = p[i+1];
      root = root->children[n];
    }
  int &n2 = p[p.size()-1];
  return std::make_pair(root, n2);
}

Gtk::Toolbar *CreateToolbar(MainWindow *window);
class TB;
class SDLScreen;
class MainWindow : public Gtk::Window
{
public:
  MainWindow(ExpressionTree<ExpressionNode> *&tree, Render *rend);
  void cb(const Glib::ustring &, const Glib::ustring &newstring)
  {
    std::cout << "Our cb" << std::endl;
    Glib::RefPtr<Gtk::TreeSelection> treeSelection = treeview.get_selection();
    Gtk::TreeModel::iterator iter = treeSelection->get_selected();
    Gtk::TreeModel::Row row = *iter;
    
    //cell->property_text() = newstring;
    
    std::pair<ExpressionTree<ExpressionNode> *, int> p = FindTreeElement(tree, iter, treeStore);
    if (!p.first) return;
    ExpressionTree<ExpressionNode> *node = p.first->children[p.second];
    if (node && node->t.func->Editable())
      {
	row[m.m_editable_text] = newstring;
	node->t.func->Edit(newstring);
      }

  }
public:
  Glib::RefPtr<Gtk::TreeStore> treeStore;
  MyTreeView treeview;
  ModelColumn m;
  ExpressionTree<ExpressionNode> *&tree;
  Gtk::CellRendererText *cell;
  Gtk::VBox vbox;
  Gtk::HBox hbox;
  Gtk::VBox vbox2;
  TB *tb;
  SDLScreen *sdlscreen;
};

void RunEffect(MainWindow *window);


class SDLScreen
{
public:
  SDLScreen(int scr_x, int scr_y) : m_time(0.0), f(NULL), surf(NULL), scr_x(scr_x), scr_y(scr_y) { }
  ~SDLScreen() { set_frame_anim(NULL); }
  void bind()
  {
    std::stringstream ss;
    ss << sock.get_id();
    std::string s = std::string("SDL_WINDOWID=")+ss.str();
    //std::cout << s << std::endl;
    char *c = new char[1024];
    const char *cc = s.c_str();
    std::copy(cc, cc+s.length()+1, c);
    SDL_putenv(c);
  }
  void init_sdl()
  {
    surf = InitSDL(scr_x, scr_y, false);
  }
  void sizerequest()
  {
    sock.set_size_request(scr_x,scr_y);
  }
  void cleanup_sdl()
  {
    SDL_Quit();
  }
  void set_frame_anim(FrameAnim *ff)
  {
    if (f)
      {
	CleanupFrameAnim(*f, surf);
      }
    f = ff;
    if (f)
      {
	InitFrameAnim(*f, surf);
      }
  }
  void set_time(float time)
  {
    m_time = time;
  }
  void frame()
  {
    DisplayFrame(*f, surf, m_time);
  }
  float m_time;
  Gtk::Socket sock;
  FrameAnim *f;
  SDL_Surface *surf;
  int scr_x, scr_y;
};

void RunEffect(MainWindow *window, Render *r)
{
  std::cout << "run button pressed" << std::endl;
  FrameAnim *anim = CreateFrameAnim(window->tree, r);
  window->sdlscreen->set_frame_anim(anim);
  window->sdlscreen->frame();
}


class TB
{
public:
  Glib::RefPtr<Gtk::UIManager> uiman;
  Glib::RefPtr<Gtk::ActionGroup> actiongroup;

  Gtk::Toolbar *CreateToolbar(MainWindow *window, Render *r)
  {
    actiongroup = Gtk::ActionGroup::create();
    actiongroup->add(Gtk::Action::create("run", "run"),
		     sigc::bind(sigc::ptr_fun2(RunEffect),window, r)
		     );
    uiman = Gtk::UIManager::create();
    uiman->insert_action_group(actiongroup);
    std::string s = 
      "<ui>"
      "  <toolbar action=\"toolbar1\">"
      "     <placeholder name=\"toolitems\">"
      "       <toolitem name=\"run\" action=\"run\"/>"
      "     </placeholder>"
      "  </toolbar>"
      "</ui>";
    try
      {
	uiman->add_ui_from_string(s);
      }
    catch(const Glib::Error &ex)
      {
	std::cout << "Building toolbar failed: " << ex.what() << std::endl;
    }
    Gtk::Toolbar *tb = dynamic_cast<Gtk::Toolbar*>(uiman->get_widget("/toolbar1"));
    Gtk::ToolButton *ti = dynamic_cast<Gtk::ToolButton*>(uiman->get_widget("/toolbar1/toolitems/run"));
    ti->set_label("run");
    tb->set_toolbar_style(Gtk::TOOLBAR_TEXT);
    return tb;
  }
};



MainWindow::MainWindow(ExpressionTree<ExpressionNode> *&tree, Render *rend) : treeview(*this), tree(tree)
  {
    resize(600,600);
    treeStore = Glib::RefPtr<Gtk::TreeStore>(new MyTreeStore(m)); //Gtk::TreeStore::create(m);
    Gtk::TreeModel::iterator iter = treeStore->append();
    Gtk::TreeModel::Row row = *iter;
    row[m.m_col_text] = "Root";
    row[m.m_editable_text] = "RootValue";
    
    if (tree)
      {
	AppendData(m, treeStore, iter, tree, tree->t.func, 0);
      }
    treeview.append_column("text", m.m_col_text);
    cell = Gtk::manage( new Gtk::CellRendererText );
    treeview.append_column("value", *cell /*m.m_editable_text*/);
    cell->property_editable() = true;
    cell->signal_edited().connect(sigc::mem_fun(*this, &MainWindow::cb));
    Gtk::TreeViewColumn *col = treeview.get_column(1);
    if (col)
      {
	col->add_attribute(cell->property_text(), m.m_editable_text);
	//col->add_attribute(*cell, "text", m.m_editable_text);
      }
    treeview.set_model(treeStore);
    tb = new TB;
    Gtk::Toolbar *toolbar = tb->CreateToolbar(this, rend);
    vbox.pack_start(*toolbar, Gtk::PACK_SHRINK);
    vbox.pack_start(hbox);
    hbox.pack_start(treeview);
    sdlscreen = new SDLScreen(320,200);
    hbox.pack_start(vbox2, Gtk::PACK_SHRINK);
    vbox2.pack_start(sdlscreen->sock, Gtk::PACK_SHRINK);
    
    add(vbox);
    set_events(Gdk::BUTTON_PRESS_MASK);
    show_all_children();

    sdlscreen->bind();
    sdlscreen->init_sdl();
    sdlscreen->sizerequest();
  }


bool MyTreeView::on_button_press_event(GdkEventButton *event)
{
  Gtk::TreeView::on_button_press_event(event);
  std::cout << "button press1" << std::endl;
  if (event->type == GDK_BUTTON_PRESS && event->button==3)
    {
      std::cout << "button press" << std::endl;
      Glib::RefPtr<Gtk::TreeSelection> treeSelection = w.treeview.get_selection();
      Gtk::TreeModel::iterator iter = treeSelection->get_selected();
      std::pair<ExpressionTree<ExpressionNode> *, int> p = FindTreeElement(w.tree, iter, w.treeStore);
      //std::string type = "P13CurveMultiply";
      std::string type = p.first->t.func->ParamType(p.second);
      std::cout << "Trying type: " << type << std::endl;
      std::vector<CreationFunction*> vec = filter_creation_array(*creation_array(), type);
      CreatePopupMenu(&w, event, vec, iter);
    }
  return true;
}


void MenuAction(int i, CreationFunction *ptr, Gtk::TreeModel::iterator iter, ExpressionTree<ExpressionNode> *&root, Glib::RefPtr<Gtk::TreeStore> model, MainWindow *window)
{
  std::cout << "MenuAction: " << ptr->Name() << std::endl;
  Gtk::TreeModel::Row r = *iter;
  Gtk::TreeModel::Path p = model->get_path(iter);
  int size = p.size()-2;
  ExpressionTree<ExpressionNode> *root2 = root;
  for(int i=1;i<size;i++)
    {
      int &n = p[i+1];
      std::cout << "path: " << n << " " << root2->t.func->Name() << std::endl;
      root2 = root2->children[n];
     //if (!root2) return;
    }
  int &n2 = p[p.size()-1];
  std::cout << root2->t.func->Name() << " " << n2 << std::endl;
  root2->children[n2] = create_expr_tree_node(ptr);
  if (root2->t.func->Appendable())
    {
      root2->t.func->Append(NULL);
    }
  ExpressionTree<ExpressionNode> *root3 = root2;
  root2 = root2->children[n2];
  AppendData(window->m, window->treeStore, iter->parent(), root2, root2->t.func, n2);
  UpdateChilds(window->m, window->treeStore, iter->parent(), root3, n2);

  //window->treeStore = Gtk::TreeStore::create(window->m);
  //window->treeview.set_model(window->treeStore);
  //Gtk::TreeModel::iterator iter2 = window->treeStore->append();

  //AppendData(window->m, window->treeStore, iter2, root, root->t.func, 0); 
}
void CreatePopupMenu(MainWindow *window, GdkEventButton *event, const std::vector<CreationFunction*> &vec, Gtk::TreeModel::iterator iter)
{
  Glib::RefPtr<Gtk::UIManager> uiman;
  Glib::RefPtr<Gtk::ActionGroup> actiongroup;
  Gtk::Menu *menupopup;

  actiongroup = Gtk::ActionGroup::create();
  
  int size = vec.size();
  for(int i=0;i<size;i++)
    {
      actiongroup->add(Gtk::Action::create(vec[i]->Name(), vec[i]->Name()),
		       sigc::bind(sigc::ptr_fun(MenuAction),i,vec[i],iter, window->tree, window->treeStore, window)
		       );
    }
  uiman = Gtk::UIManager::create();
  uiman->insert_action_group(actiongroup);
  //window->add_accel_group(uiman->get_accel_group());
  
  std::string s = std::string("<ui>"
			      " <popup name='Nodes'>");
  int size2 = vec.size();
  for(int i=0;i<size2;i++)
    {
      s+=std::string("<menuitem action='")+vec[i]->Name()+"'/>";
    }
  s+=std::string("</popup></ui>");
  Glib::ustring ui_info = s.c_str();
  try
    {
      uiman->add_ui_from_string(ui_info);
    }
  catch(const Glib::Error& ex)
    {
      std::cout << "Building menus failed: " << ex.what() << std::endl;
    }
  menupopup = dynamic_cast<Gtk::Menu*>(uiman->get_widget("/Nodes"));
  menupopup->popup(event->button, event->time);
}


void ExecuteGtkmm(int argc, char **argv, ExpressionTree<ExpressionNode> *&tree, Render *r)
{
  Gtk::Main kit(argc, argv);
  MainWindow window(tree, r);
  //Gtk::Window window;
  //ModelColumn m;

  //Gtk::TreeView treeview;

  Gtk::Main::run(window);
}



ExpressionTree<ExpressionNode> *create_expr_tree_node(CreationFunction *func)
{
  ExpressionTree<ExpressionNode> *node = new ExpressionTree<ExpressionNode>;
  node->t.func = func;
  int num = func->NumParams();
  for(int i=0;i<num;i++)
    {
      node->children.push_back(NULL);
    }
  return node;
}

std::string stringify(float x) 
{
  std::stringstream s;
  s << x;
  return s.str();
}

template<class T>
void valuefy(std::string s, T &f)
{
  std::stringstream ss(s);
  ss >> f;
}

std::vector<CreationFunction*> filter_creation_array(Array<int, CreationFunction*> &arr, std::string ret_type)
{
  std::vector<CreationFunction*> vec;
  int size = arr.Size();
  for(int i=0;i<size;i++)
    {
      CreationFunction *func = arr.Index(i);
      //std::cout << "Compare: " << func->ReturnType() << " vs " << ret_type << std::endl;
      if (func->ReturnType() == ret_type)
	{
	  std::cout << "ExactMatch " << ret_type << std::endl;
	  vec.push_back(func);
	}
      else if (func->ReturnType()[0]=='P' && func->ReturnType().substr(1,func->ReturnType().size()-1)==ret_type)
	{
	  std::cout << "PointerMatch " << func->ReturnType() << " " << ret_type << std::endl;
	  vec.push_back(func);
	}
    }
  return vec;
}

CurveMultiply* curve_multiply_func(MatrixCurve &curve, CurveIn3d &curve2)
{
  return new CurveMultiply(curve,curve2);
}
FrameAnim *default_frame_anim(const FaceCollection &faces, const float &time, Render &r)
{
  return new DefaultFrameAnim(faces, time, &r);
}

FaceCollection *cube_elem()
{
  return new CubeElem;
}
FaceCollection *cylinder_elem(const int &numfaces)
{
  return new CylinderElem(numfaces);
}
FaceCollection *sphere_elem(const int &numfaces, const int &numfaces2)
{
  return new SphereElem(numfaces, numfaces2);
}
BoxableFaceCollection *sphere_elem2(const int &numfaces, const int &numfaces2)
{
  return new SphereElem(numfaces, numfaces2);
}
FaceCollection *color_elem(const BoxableFaceCollection &next, const unsigned int &color)
{
  return new ColorElem(next, color);
}
FaceCollection *matrix_elem(const Matrix &m, const BoxableFaceCollection &next)
{
  return new MatrixElem(next, m);
}

BoxableFaceCollection *cube_elem2()
{
  return new CubeElem;
}
BoxableFaceCollection *cylinder_elem2(const int &numfaces)
{
  return new CylinderElem(numfaces);
}
BoxableFaceCollection *color_elem2(const BoxableFaceCollection &next, const unsigned int &color)
{
  return new ColorElem(next, color);
}
BoxableFaceCollection *matrix_elem2(const Matrix &m, const BoxableFaceCollection &next)
{
  return new MatrixElem(next, m);
}
Matrix *x_rotation(const float &rot)
{
  return new Matrix(Matrix::XRotation(rot));
}
Matrix *y_rotation(const float &rot)
{
  return new Matrix(Matrix::YRotation(rot));
}
Matrix *z_rotation(const float &rot)
{
  return new Matrix(Matrix::ZRotation(rot));
}
Matrix *translate(const float &x, const float &y, const float &z)
{
  return new Matrix(Matrix::Translate(x,y,z));
}
Matrix *scale(const float &x, const float &y, const float &z)
{
  return new Matrix(Matrix::Scale(x,y,z));
}
Matrix *matrix_multiplication(const Array<int,Matrix> &m)
{
  Matrix mm = Matrix::Identity();
  for(int i=0;i<m.Size();i++)
    {
      mm = mm * m.Index(i); 
    }
  return new Matrix(mm);
}

FaceCollection *boxed_elem(BoxCollection &coll, BoxableFaceCollection &item)
{
  return new BoxedElem(coll, item);
}
BoxableFaceCollection *boxed_elem2(BoxCollection &coll, BoxableFaceCollection &item)
{
  return new BoxedElem(coll, item);
}
BoxCollection *boxcollectionconvert(const Array<int, Matrix> &c2)
{
  return new BoxCollectionConvert(c2);
}
FrameAnim *sequential_frame_anim(const Array<int, FrameAnim*> &arr, Render &r)
{
  return new SequentialFrameAnim(arr, &r);
}
float *waveform_ref(const Waveform &wave, const float &val)
{
  return new float(wave.Index(val));
}
Waveform *part_waveform(Waveform &base, Array<int, WaveformPart*> &arr)
{
  return new PartWaveform(base, arr);
}
WaveformPart *waveform_part(const float &start_x, const float &end_x, const float &delta_y)
{
  return new LinWaveformPart(start_x, end_x, delta_y);
}
Waveform *zerowaveform(const float &length_x, const float &min_y, const float &max_y)
{
  return new ZeroWaveform(length_x, min_y, max_y);
}

CreationFunctionArray *creation_array()
{
  DynCreationFunctionArray *arr = new DynCreationFunctionArray;
  arr->push_back_ptr_array<FrameAnim*>("frameanimarray[]");
  arr->push_value<float>("floatval");
  arr->push_value<int>("intval");
  arr->push_back_cref_cref(waveform_ref, "waveform(wave,val)");
  arr->push_back_cref_cref(sphere_elem, "sphere(num,num)");
  arr->push_back_cref_cref(sphere_elem2, "sphere(num,num)");
  arr->push_back_cref_cref(waveform_ref, "waveform(wave,val)");
  arr->push_back_ref_ref(part_waveform, "part_waveform(wave,array)");
  arr->push_back_cref_cref_cref(waveform_part, "waveform_part(start_x,end_x,delta_y)");
  arr->push_back_cref_cref_cref(zerowaveform, "zerowaveform(length_x, min_y, max_y)");
  arr->push_back_ptr_array<WaveformPart*>("waveformpartarray[]");
  arr->push_back_ref_ref(curve_multiply_func, "curve_multiply(curve,curve2)");
  arr->push_back_cref_cref_ref(default_frame_anim, "default_frame_anim(faces,time,render)");
  arr->push_back_cref_ref(sequential_frame_anim, "sequential_frame_anim[FrameAnim]");
  arr->push_back(cube_elem, "cube_elem");
  arr->push_back_cref(cylinder_elem, "cylinder_elem(numfaces)");
  arr->push_back_cref_cref(color_elem, "color_elem(faces,color)");
  arr->push_back_cref_cref(matrix_elem, "matrix_elem(matrix,faces)");
  arr->push_back_ref_ref(boxed_elem, "boxed_elem(boxcoll,faces)");

  arr->push_back(cube_elem2, "cube_elem()");
  arr->push_back_cref(cylinder_elem2, "cylinder_elem(numfaces)");
  arr->push_back_cref_cref(color_elem2, "color_elem(faces,color)");
  arr->push_back_ref_ref(boxed_elem2, "boxed_elem(boxcoll,faces)");
  arr->push_back_cref_cref(matrix_elem2, "matrix_elem(matrix,faces)");
  arr->push_back_cref(x_rotation, "x_rotation(angle)");
  arr->push_back_cref(y_rotation, "y_rotation(angle)");
  arr->push_back_cref(z_rotation, "z_rotation(angle)");
  arr->push_back_cref_cref_cref(translate, "translate(x,y,z)");
  arr->push_back_cref_cref_cref(scale, "scale(sx,sy,sz)");
  arr->push_back_cref(boxcollectionconvert, "boxcollectionconvert(array)");
  arr->push_back_array<Matrix>("matrixarray[]");
  arr->push_back_cref(matrix_multiplication, "multmatrix(matrixarray)");
  return arr;
}

void *ExecuteEffect2(ExpressionTree<ExpressionNode> *root)
{
  if (!root) { std::cout << "Root NULL" << std::endl; return NULL; }
  std::cout << "Execute2: " << root->t.func->Name() << std::endl;
  int size = root->children.size();
  for(int i=0;i<size;i++)
    {
      void *value = ExecuteEffect2(root->children[i]);
      if (!value) { std::cout << "Value NULL" << std::endl; }
      else
	{
	  root->t.func->SetParam(i, value);
	}
    }
  root->t.func->Execute();
  std::cout << "Execute2ret: " << root->t.func->Name() << std::endl;
  void *retval = root->t.func->GetReturn();
  return retval;
}

FrameAnim *CreateFrameAnim(ExpressionTree<ExpressionNode> *tree, Render *r)
{
  void *value = ExecuteEffect2(tree);
  std::cout << "ExecuteEffect:" << tree->t.func->Name() << std::endl;
  const Array<int, FrameAnim*> *arr = (const Array<int,FrameAnim*>*)value;
  Array<int, FrameAnim*> *arr2 = const_cast<Array<int,FrameAnim*>*>(arr);
  ArrayFrameAnim *faarray = new ArrayFrameAnim(*arr2,r);
  return faarray;
}

void ExecuteEffect(ExpressionTree<ExpressionNode> *tree, SDL_Surface *surf, Render *r)
{
  FrameAnim *fa = CreateFrameAnim(tree, r);
  Execute(*fa, surf);
  delete fa;
}
#endif

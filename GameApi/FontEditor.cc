#define NO_SDL_GLEXT
#include "FontEditor.hh"
#include "Font.hh"
#include <gtkmm.h>
#include <gdkmm.h>
#include <gdkmm/pixbuf.h> 
class FontEditorMainWindow : public Gtk::Window
{
public:
  bool DragBegin(GdkEventButton *event)
  {
    int posx, posy;
    image.get_pointer(posx,posy);
    //posx-=eventbox.get_allocation().get_x();
    //posy-=eventbox.get_allocation().get_y();
    //double posx = event->x;
    //double posy = event->y;
    std::cout << "MousePos: " << posx << " " << posy << std::endl;

    int index = font->FindCharacter(ch);
    const FontCharacter *character = font->FontGlyph(index);
    GlyphToPointCollection points(*character, 0);
    Point2d mousepos = { float(posx), float(posy) };
    Point2d mousepossmall = Zoom::ZoomToSmall(mousepos, 10);
    int id = FindNearestPoint(mousepossmall, points);
    
    drag_id = id;
    std::cout << "DragBegin" << std::endl;
    return true;
  }
  bool DragEnd(GdkEventButton *event)
  {
    if (drag_id==-1) return true;
    int posx, posy;
    image.get_pointer(posx,posy);
    //posx-=eventbox.get_allocation().get_x();
    //posy-=image.get_allocation().get_y();
    //double posx = event->x - image.;
    //double posy = event->y;
    Point2d mousepos = { float(posx), float(posy) };
    Point2d mousepossmall = Zoom::ZoomToSmall(mousepos, 10);
    int index = font->FindCharacter(ch);
    const FontCharacter *character = font->FontGlyph(index);
    const FontCharacterModifiable *mods = dynamic_cast<const FontCharacterModifiable*>(character);
    FontCharacterModifiable *mods2 = const_cast<FontCharacterModifiable*>(mods);
    mods2->MovePoint(0, drag_id, mousepossmall);

    RefreshCharacter(font, ch);
    std::cout << "DragEnd" << std::endl;
    return true;
  }
  FontEditorMainWindow(Font *font) 
  {
    resize(300,320);

    eventbox.signal_button_press_event().connect(sigc::mem_fun(*this, &FontEditorMainWindow::DragBegin));
    eventbox.signal_button_release_event().connect(sigc::mem_fun(*this, &FontEditorMainWindow::DragEnd));
    button1.set_label("Insert");
    button2.set_label("Move");
    button3.set_label("Split");
    button4.set_label("Destroy");
    hbox.pack_start(button1, false, true, 0);
    hbox.pack_start(button2, false, true, 0);
    hbox.pack_start(button3, false, true, 0);
    hbox.pack_start(button4, false, true, 0);
    vbox.pack_start(hbox, false, true, 0);
    vbox.pack_start(eventbox, false, false, 0);
    eventbox.add(image);
    add(vbox);
    show_all_children();
  }
  void Refresh()
  {
    Glib::RefPtr<Gdk::Pixbuf> pixbuf 
      = Gdk::Pixbuf::create_from_data((unsigned char*)buf.buffer, (Gdk::Colorspace)Gdk::COLORSPACE_RGB, true, 8, buf.width, buf.height, buf.ydelta*4);
    image.set(pixbuf);
  }
  void RefreshCharacter(Font *f, int ch2)
  {
    font = f;
    ch = ch2;
    SingletonArray<int> array(ch);
    BufferRef ref = RenderFont(font, array, 10);
    
    int index = f->FindCharacter(ch);
    const FontCharacter *character = f->FontGlyph(index);
    GlyphToPointCollection points(*character, 0);
    ZoomPoints zoompoints(points, 10);
    DrawDots(ref, zoompoints);
    SetBuffer(ref);
    Refresh();
  }
  void SetBuffer(BufferRef ref) { buf = ref; }

  Gtk::ToggleButton button1, button2, button3, button4;

  Gtk::VBox vbox;
  Gtk::HBox hbox;
  Gtk::EventBox eventbox;
  Gtk::Image image;
  Font *font;
  int ch;
  BufferRef buf;
  int drag_id;
};


void ExecuteFontEditor(int argc, char **argv, Font *font)
{
  Gtk::Main kit(argc, argv);
  FontEditorMainWindow window(font);
  //Color c1(255,255,255);
  //Color c2(0,0,0);
  //ChessBoardBitmap bm(c1,c2,256/8,256/8,8,8);
  //BufferFromBitmap buf(bm);
  //buf.Gen();
  //window.SetBuffer(buf.Buffer());
  //window.Refresh();
  window.RefreshCharacter(font, 'a');
  Gtk::Main::run(window);
}

void DrawDot(BufferRef ref, Point2d p)
{
  int sx = 6;
  int sy = 6;
  for(int x=0;x<sx;x++)
    {
      for(int y=0;y<sy;y++)
	{
	  int xx = p.x + x - 3;
	  int yy = p.y + y - 3;
	  if (xx >= 0 && xx < (int)ref.width)
	    {
	      if (yy >= 0 && yy < (int)ref.height)
		ref.buffer[xx+yy*ref.ydelta] = 0xff0000;
	    }

	}
    }
}

void DrawDots(BufferRef ref, PointCollection2d &p)
{
  int s = p.Size();
  for(int i=0;i<s;i++)
    {
      Point2d pp = p.Index(i);
      DrawDot(ref, pp);
    }
}

int FindNearestPoint(Point2d p, PointCollection2d &points)
{
  int s = points.Size();
  float mindist = 99999.0;
  int index = -1;
  for(int i=0;i<s;i++)
    {
      Point2d pp = points.Index(i);
      float dist = (pp.x-p.x)*(pp.x-p.x)+(pp.y-p.y)*(pp.y-p.y);
      if (dist < mindist && dist <30) { mindist = dist; index = i; }
    }
  return index;
}

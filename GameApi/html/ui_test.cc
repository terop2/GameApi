
#include "ui.hh"
#include <fstream>

int main()
{
  W e1 = label("test\ntest\ntest\ntestabcdefghij\ntest\ntest", 100,100, 20, 0x80000000,0);
  W e2 = link("http://sivut.koti.soon.fi/~terop/test.html", "test\ntest", 100,100, 20, 0xff000000,0);
  W e31 = e1;
  W e32 = e2;
  W e33 = gradient(0,100, 100);
  W e34 = gradient(0,100, 100);
  W e35 = gradient(0,100, 100);
  W e36 = gradient(0,100, 100);
  std::vector<W> v = { e31,e32,e33,e34,e35,e36 };
  W lst = grid_y(v, 4, 2,2);
  W lst_hide = hide(lst);
  W but = image_placeholder(200,200);
  W but2 = image_placeholder(50,50);
  W lst1 = show_button(but, lst_hide);
  W lst1a = hide_button(but2, lst_hide);
  W lst3 = list_y(std::vector<W>{lst1,lst1a, lst_hide}, 2);


  W edit = editor("Address", "Text", "Placeholder", 300,40,40, 0x80000000,0);
  W edit2 = submit_button("Submit", 300,60);
  W lst4 = list_y(std::vector<W>{ edit, edit2 }, 2);
  W frm = form(lst4, true, "foo.php");
  
  std::string lst2 = gen_html_page(frm);
  std::ofstream ss("tst.html");
  ss << lst2 << std::endl;
  ss.close();
}

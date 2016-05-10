
#include "ui.hh"
#include <fstream>

int main()
{

  W lab1 = label("test\ntest\ntest\ntest", 100, 30, 40, 0xff000000, 1);
  W lab2 = label("test\ntest\ntest\ntest\ntest\ntest", 100, 30, 40, 0xff000000, 1);
  W c1a = center_y(lab1, 200);
  W c1b = center_y(lab1, 200);
  W c1c = center_y(lab1, 200);
  W c1d = center_y(lab1, 200);
  W c2e = center_y(lab2, 250);
  W d1a = gradient(0, 100,200);
  W d1b = gradient(0, 100,200);
  W d1c = gradient(0, 100,200);
  W d1d = gradient(0, 100,200);
  W d2e = gradient(0, 100,250);
  W e1a = layer(std::vector<W>{d1a, c1a});
  W e1b = layer(std::vector<W>{d1b, c1b});
  W e1c = layer(std::vector<W>{d1c, c1c});
  W e1d = layer(std::vector<W>{d1d, c1d});
  W e2e = layer(std::vector<W>{d2e, c2e});

  std::vector<W> vec = { e1a, e1b, e1c, e1d, e2e };
  W gr = grid_y(vec, 2, 2, 2);
  
  

  std::string lst2 = gen_html_page(gr);
  std::ofstream ss("tst.html");
  ss << lst2 << std::endl;
  ss.close();
}

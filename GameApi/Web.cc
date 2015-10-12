
#define NO_SDL_GLEXT
#include "Web.hh"
#include "Graph.hh"
#include "Effect.hh"
#include "Equalizer.hh"
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <sys/types.h>
#include <sys/stat.h>

void SaveFrameAnim(FrameAnim &f, SDL_Surface *screen, std::string &output, float delta_time_between_frames, float total_time)
{
  InitFrameAnim(f,screen);
  for(float t = 0.0;t<total_time;t+=delta_time_between_frames)
    {
      DisplayFrame(f,screen,t);
      //SDL_Surface *surf1 = SDL_DisplayFormat(screen);
      //SDL_Surface *surf2 = SDL_DisplayFormatAlpha(surf1);
      BufferRef ref = BufferRef::NewBuffer(1024,768);
      glReadPixels(0,0,1024,768,GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, ref.buffer);
      BitmapFromBuffer bm(ref);
      PpmFile file("a.txt", bm, false);
      output += file.Contents();
    }
}

void SaveFrameAnimToFile(FrameAnim &f, SDL_Surface *screen, std::string filename, float delta_time_between_frames, float total_time)
{
  std::cout << "Saving to " << filename << std::endl;
  std::string s;
  SaveFrameAnim(f,screen,s,delta_time_between_frames, total_time);

  std::ofstream file(filename.c_str());
  file << s;
  
}


void CreateFiles(Directory &d)
{ 
#if 0
  mkdir((std::string("./")+d.DirectoryName()).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  CreateFiles(d,"./"+d.DirectoryName()+"/");
#endif
}

void CreateFiles(Directory &d, std::string path)
{
#if 0
  int dirs = d.NumDirs();
  for(int i=0;i<dirs;i++)
    {
      Directory *dd = d.Dir(i);
      mkdir((path+dd->DirectoryName()).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      CreateFiles(*dd, path+dd->DirectoryName()+"/");
    }
  int files = d.NumFiles();
  for(int i=0;i<files;i++)
    {
      std::string filename = d.FileName(i);
      std::string contents = d.FileContents(i);
      std::ofstream f((path+filename).c_str());
      f << contents;
      f.close();
    }
#endif
}


void CreateHtml()
{
  Color c;
  c.r = 0;
  c.g = 255;
  c.b = 0;
  Color c2;
  c2.r = 255;
  c2.g = 255;
  c2.b = 255;
  Color c2a;
  c2a.r = 255;
  c2a.g = 0;
  c2a.b = 255;
  Color c3;
  c3.r = 255;
  c3.g = 128;
  c3.b = 0;
  //ConstantBitmap<Color> cb(c,5,5);
  //ConstantBitmap<Color> cb2(c2,5,5);
  //Point2d center;
  //center.x = 10;
  //center.y = 10;
  //BitmapCircle bc(center, 10.0,21,21);
  ColorChooseFunction cf(c,c2);

  Point2d /*p1, */ p2,p3; //,p4;
  //p1.x = 3;
  //p1.y = 5;
  p2.x = 5;
  p2.y = 20;
  p3.x = 20;
  p3.y = 0;
  //p4.x = 15;
  //p4.y = 20;
  //TriangleContinuousBitmap tri(21,21,p1,p2,p3);
  /*
  QuadPointCollection2d tr(p1,p2,p3,p4);
  ContinuousLines2d tr2(tr, true);
  PolygonFill tri(21,21,tr2);
  MapContinuousBitmap<bool,Color> trimap(tri,cf);
  */
  GradientBitmap trimap(21,21, p3,p2, c,c2);
  //ColorTriangle2 trimap(21,21,p1,p2,p3,c,c2a,c3,c2);
  ///ColorTriangle trimap(21,21,p1,p2,p3,c,c2);
  SampleBitmap m_bc(trimap, 21,21);
  //BitmapFromContinuousBitmap<bool> bc(tri,21,21);
  //MapBitmap<bool,Color> m_bc(bc, cf);
  PosBitmap pos(21,21);
  PngBitmapFunction pos_f("testpng", m_bc, 15,15);
  MapBitmap<Pos, HtmlImage*> m_pos(pos, pos_f);
  ConvertBitmap<HtmlImage*, HtmlPart*> conv(m_pos);
  ConvertBitmap<HtmlImage*, File*> conv2(m_pos);
  HtmlTable bd(conv);
  EmptyDir empty("output");


  //PosBitmap bmp2(10,10);
  //TableFunction tf;
  //MapBitmap<Pos,HtmlPart*> bmp(bmp2, tf);
  //HtmlTable bd(bmp);
  DocBody body("Title", bd);
  HtmlFile htmlfile("test.html", body);
  BitmapDirectory dir(empty, conv2);
  FileInDir f1(dir, htmlfile);
  CreateFiles(f1);
}
HtmlImage *PngBitmapFunction::Index(Pos c) const
  {
    delete constant;
    constant = new ConstantBitmap<Color>(bm.Map(c.x,c.y), sx,sy);
    delete img;
    img = new HtmlImage(filename+"_"+Num(c.x)+"_"+Num(c.y)+".png", *constant, sx, sy);
    return img;
  }


#include "Effect.hh"
#include "GraphI.hh"
#include <sstream>
#include <string>
#include <fstream>

class Directory
{
public:
  virtual std::string DirectoryName() const=0;
  virtual int NumFiles() const=0;
  virtual std::string FileName(int val) const=0;
  virtual std::string FileContents(int val) const=0;
  virtual int NumDirs() const=0;
  virtual Directory* Dir(int val) const=0;
};

void CreateFiles(Directory &d);
void CreateFiles(Directory &d, std::string path);

class File
{
public:
  virtual std::string FileName() const=0;
  virtual std::string Contents() const=0;
};

class EmptyDir : public Directory
{
public:
  EmptyDir(std::string dirname) 
  : dirname(dirname) { }
  std::string DirectoryName() const { return dirname; }
  int NumFiles() const { return 0; }
  std::string FileName(int val) const { return ""; }
  std::string FileContents(int val) const { return ""; }
  int NumDirs() const { return 0; }
  Directory* Dir(int val) const { return 0; }
private:
  std::string dirname;
};

class FileInDir : public Directory
{
public:
  FileInDir(Directory &d, File &f) : d(d), f(f) { }
  std::string DirectoryName() const { return d.DirectoryName(); }
  int NumFiles() const { return d.NumFiles()+1; }
  std::string FileName(int val) const { if (val==0) return f.FileName(); else return d.FileName(val-1); }
  std::string FileContents(int val) const
  {
    if (val==0) return f.Contents(); else return d.FileContents(val-1);
  }
  int NumDirs() const { return d.NumDirs(); }
  Directory* Dir(int val) const { return d.Dir(val); }

private:
  Directory &d;
  File &f;
};

class DirInDir : public Directory
{
public:
  DirInDir(Directory &d, Directory &d_inside) : d(d), d_inside(d_inside) { }
  std::string DirectoryName() const { return d.DirectoryName(); }
  int NumFiles() const { return d.NumFiles(); }
  std::string FileName(int val) const { return d.FileName(val); }
  std::string FileContents(int val) const { return d.FileContents(val); }
  int NumDirs() const { return d.NumDirs()+1; }
  Directory* Dir(int val) const
  {
    if (val==0) return &d_inside;
    else return d.Dir(val-1);
  }

private:
  Directory &d;
  Directory &d_inside;
};


class HtmlPart
{
public:
  virtual std::string Part() const=0;
  virtual ~HtmlPart() { }
};

class CharacterPart : public HtmlPart
{
public:
  CharacterPart(char c) : c(c) { }
  std::string Part() const { return std::string(&c,&c+1); }
private:
  char c;
};

class ArrayPart : public HtmlPart
{
public:
  template<class T>
  ArrayPart(T beg, T end) : vec(beg, end) { }
  std::string Part() const
  {
    int size = vec.size();
    std::string res;
    for(int i=0;i<size;i++)
      {
	HtmlPart *p = vec[i];
	res += p->Part();
      }
    return res;
  }
private:
  std::vector<HtmlPart*> vec;
};

class LinkPart : public HtmlPart
{
public:
  LinkPart(std::string url, HtmlPart &text) : url(url), text(text) { }
  std::string Part() const
  {
    std::string res;
    res += "<a href=\"";
    res += url;
    res += "\">";
    res += text.Part();
    res += "</a>";
    return res;
  }
private:
  std::string url;
  HtmlPart &text;
};

class TextPart : public HtmlPart
{
public:
  TextPart(std::string contents) : contents(contents) { }
  std::string Part() const { return contents; }
private:
  std::string contents;
};

class ImagePart : public HtmlPart
{
public:
  ImagePart(std::string filename, int width, int height) : filename(filename),  width(width), height(height) { }

  std::string Part() const
  {
    std::string res;
    res += "<img border=\"0\" src=\"";
    res += filename;
    res += "\" width=\"";
    std::stringstream s;
    s << width;
    res += s.str();
    std::stringstream s2;
    s2 << height;
    res += "\" height=\"";
    res += s2.str();
    res += "\">";
    return res;
  }
private:
  std::string filename;
  int width, height;
};

class ColorChooseFunction : public Function<bool, Color>
{
public:
  ColorChooseFunction(Color true_color, Color false_color) : t(true_color), f(false_color) { }
  Color Index(bool b) const {
    if (b) return t;
    else return f; 
  }
private:
  Color t,f;
};


class TableFunction : public Function<Pos, HtmlPart*>
{
public:
  TableFunction() : text(0) { }
  HtmlPart *Index(Pos p) const
  {
    std::stringstream s;
    s << "(" << p.x << "," << p.y << ")";
    delete text;
    text = new TextPart(s.str());
    return text;
  }
private:
  mutable TextPart *text;
};

class HtmlTable : public HtmlPart
{
public:
  HtmlTable(Bitmap<HtmlPart*> &bm) : bm(bm) { }
  std::string Part() const
  {
    std::string s;
    s+="<table border=\"0\" cellspacing=\"0\">";
    for(int y=0;y<bm.SizeY();y++)
      {
	s+="<tr>";
	for(int x=0;x<bm.SizeX();x++)
	  {
	    s+="<td>";
	    s+=bm.Map(x,y)->Part();
	    s+="</td>";
	  }
	s+="</tr>";
      }
    s+="</table>";
    return s;
  }
private:
  Bitmap<HtmlPart*> &bm;
};

class DocBody : public HtmlPart
{
public:
  DocBody(std::string title, HtmlPart &body) : title(title), body(body) { }
  std::string Part() const
  {
    std::string res;
    res += "<html><head><title>";
    res += title;
    res += "</title></head>";
    res += "<body TEXT=\"#000000\" BGCOLOR=\"#FFFFFF\" ACOLOR=\"#000000\">";
    res += body.Part();
    res += "</body></html>";
    return res;
  }
private:
  std::string title;
  HtmlPart &body;
};



class HtmlFile : public File
{
public:
  HtmlFile(std::string filename, HtmlPart &contents) : filename(filename), contents(contents) { }
  virtual std::string FileName() const { return filename; }
  virtual std::string Contents() const { return contents.Part(); }
private:
  std::string filename;
  HtmlPart &contents;
};


class PpmFile : public File
{
public:
  PpmFile(std::string filename, Bitmap<Color> &contents) : filename(filename), contents(contents) { }
  virtual std::string FileName() const { return filename; }
  virtual std::string Contents() const 
  {
    std::string res;
    res += "P3\n";
    res += Num(contents.SizeX());
    res += " ";
    res += Num(contents.SizeY());
    res += " ";
    res += "255\n";
    for(int y=0;y<contents.SizeY();y++)
      {
      for(int x=0;x<contents.SizeX();x++)
	{
	  Color c = contents.Map(x,y);
	  res += std::string(" ") + Num(c.r) + " " + Num(c.g) + " " + Num(c.b) + " "; 
	}
      res += "\n";
      }
    return res;
  }
  
  std::string Num(int i) const { std::stringstream s; s << i; return s.str(); }

private:
  std::string filename;
  Bitmap<Color> &contents;
};

class PgmFile : public File
{
public:
  PgmFile(std::string filename, Bitmap<Color> &contents) : filename(filename), contents(contents) { }
  virtual std::string FileName() const { return filename; }
  virtual std::string Contents() const 
  {
    std::string res;
    res += "P5\n";
    res += Num(contents.SizeX());
    res += " ";
    res += Num(contents.SizeY());
    res += " ";
    res += "255\n";
    for(int y=0;y<contents.SizeY();y++)
      {
      for(int x=0;x<contents.SizeX();x++)
	{
	  Color c = contents.Map(x,y);
	  res += std::string(" ") + Num(c.alpha) + " "; 
	}
      res += "\n";
      }
    return res;
  }
  
  std::string Num(int i) const { std::stringstream s; s << i; return s.str(); }

private:
  std::string filename;
  Bitmap<Color> &contents;
};


class PngFile : public File
{
public:
  PngFile(std::string filename, 
	  Bitmap<Color> &contents, 
	  std::string tempfilename = "./output/tmp.tmp",
	  std::string tempfilename2 = "./output/tmp2.tmp") 
  : filename(filename), 
    contents(contents), 
    tmpfilename(tempfilename),
    tmpfilename2(tempfilename2)
  { }

  virtual std::string FileName() const { return filename; }
  virtual std::string Contents() const 
  { 
#if 0
    std::ofstream f(tmpfilename.c_str());
    PpmFile p(tmpfilename, contents);
    f << p.Contents();
    f.close();

    std::ofstream f2(tmpfilename2.c_str());
    PgmFile p2(tmpfilename2, contents);
    f2 << p2.Contents();
    f2.close();

    FILE *ff = popen((std::string("pnmtopng ")+"-alpha " + tmpfilename2 + " " +tmpfilename).c_str(), "r");
    std::string s;
    while(!feof(ff))
      {
	char c;
	size_t t = fread(&c, 1, 1, ff);
	(void)t;
	s+=c;
      }
    fclose(ff);
    return s; 
#endif
    return "";
  }

private:
  std::string filename;
  Bitmap<Color> &contents;
  std::string tmpfilename;
  std::string tmpfilename2;
};

class HtmlImage : public HtmlPart, public File
{
public:
  HtmlImage(std::string png_filename, Bitmap<Color> &c, int width, int height) : png(png_filename, c), img(png_filename, width, height) { }
  std::string Part() const { return img.Part(); }
  std::string FileName() const { return png.FileName(); }
  std::string Contents() const { return png.Contents(); }
private:
  PngFile png;
  ImagePart img;
};

class PngBitmapFunction : public Function<Pos, HtmlImage*>
{
public:
  PngBitmapFunction(std::string file_basename, Bitmap<Color> &bm, int sx, int sy) : filename(file_basename), bm(bm),sx(sx), sy(sy), img(0), constant(0) { }
  HtmlImage *Index(Pos c) const;
  std::string Num(int i) const { std::stringstream s; s << i; return s.str(); }
private:
  std::string filename;
  Bitmap<Color> &bm;
  int sx,sy;
  mutable HtmlImage *img;
  mutable Bitmap<Color> *constant;
};


class BitmapDirectory : public Directory
{
public:
  BitmapDirectory(Directory &d, Bitmap<File*> &bm) : d(d), bm(bm) { }

  virtual std::string DirectoryName() const { return d.DirectoryName(); }
  virtual int NumFiles() const { return d.NumFiles()+bm.SizeX()*bm.SizeY(); }
  virtual std::string FileName(int val) const 
  {
    if (val < d.NumFiles())
      {
	return d.FileName(val);
      }
    else
      {
	int val_a = val-d.NumFiles();
	int val_y = val_a / bm.SizeX();
	int val_x = val_a - bm.SizeX()*val_y;

	File *f = bm.Map(val_x,val_y);
	return f->FileName();
      }
  }
  virtual std::string FileContents(int val) const
  {
    if (val < d.NumFiles())
      {
	return d.FileContents(val);
      }
    else
      {
	int val_a = val-d.NumFiles();
	int val_y = val_a / bm.SizeX();
	int val_x = val_a - bm.SizeX()*val_y;

	File *f = bm.Map(val_x,val_y);
	return f->Contents();
      }
  }
  virtual int NumDirs() const { return d.NumDirs(); }
  virtual Directory* Dir(int val) const { return d.Dir(val); }
private:
  Directory &d;
  Bitmap<File*> &bm;
};

void SaveFrameAnim(FrameAnim &f, SDL_Surface *screen, std::string &output, float delta_time_between_frames, float total_time);
void SaveFrameAnimToFile(FrameAnim &f, SDL_Surface *screen, std::string filename, float delta_time_between_frames, float total_time);

void CreateHtml();


//
// Javascript
//


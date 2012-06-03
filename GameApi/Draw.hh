
class Draw
{
public:
  virtual void Quad(Point p1, Point p2, Point p3, Point p4)=0;
  virtual void Tri(Point p1, Point p2, Point p3)=0;
  virtual void TexCoordsQuad(Point2d p1, Point2d p2, Point2d p3, Point2d p4)=0;
  virtual void TexCoordsTri(Point2d p1, Point2d p2, Point2d p3)=0;
  virtual int AddTexture(const Bitmap<Color> &c)=0;
  virtual void RemoveTexture(int i)=0;
  virtual void TexturePoly(int i)=0;
  virtual void ColorPoly(Color c)=0;
  virtual int StartObj()=0;
  virtual void EndObj(int i)=0;
  virtual void Object(int i)=0;

};

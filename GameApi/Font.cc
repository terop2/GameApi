
#include "Font.hh"
#include "Effect.hh"
#include "Buffer.hh"
#include "Equalizer.hh"
#include "Graph.hh"
#include "FontEditor.hh"

BufferRef RenderFont(Font *font, Array<int, int> &text, int zoomlevel)
{
  // calculate size of required bitmap
  int xsize = 0;
  int s = text.Size();
  for(int i=0;i<s;i++)
    {
      int ch = text.Index(i);
      int cha = font->FindCharacter(ch);
      xsize += font->FontGlyph(cha)->Advance();
    }
  int ysize = font->Height();
  // allocate bitmap
  BufferRef ref = BufferRef::NewBuffer(xsize*zoomlevel, ysize*zoomlevel);

  std::vector<ContinuousBitmap<bool>*> cleanup;
  std::vector<GlyphToPointCollection *> cleanup2;
  std::vector<ContinuousLines2d*> cleanup3;
  for(int i=0;i<s;i++)
    {
      int cha = text.Index(i);
      int ch = font->FindCharacter(cha);
      std::cout << "Character" << i << " " << ch << " " << cha << std::endl;
      const FontCharacter *glyph = font->FontGlyph(ch);
      int lsize = glyph->NumLoops();
      std::cout << "lsize: " << glyph << " " << lsize << std::endl;
      Point2d tl = glyph->PosTL();
      Point2d br = glyph->PosBR();
      float sx = br.x-tl.x;
      float sy = br.y-tl.y;
      std::cout << "sxsy:" << sx << " " << sy << std::endl;
      ConstantContinuousBitmap<bool> bmp(sx,sy, false);
      ContinuousBitmap<bool> *bm = &bmp;
      for(int k=0;k<lsize;k++)
	{
	  GlyphToPointCollection *points = new GlyphToPointCollection(*glyph, k);
	  cleanup2.push_back(points);
	  ContinuousLines2d *lines = new ContinuousLines2d(*points, true);
	  cleanup3.push_back(lines);
	  
	  if (glyph->Inverse(k))
	    {
	      std::cout << "Inverse" << std::endl;
	      // bm = ContinuousBitmap<Color>
	      PolygonFill *poly = new PolygonFill(sx,sy, *lines);
	      AndNotContinuousBitmap *anot = new AndNotContinuousBitmap(*bm, *poly);
	      //cleanup.push_back(poly);
	      //cleanup.push_back(anot);
	      bm = anot;
	    }
	  else
	    {
	      std::cout << "Not Inverse" << std::endl;
	      PolygonFill *poly = new PolygonFill(sx,sy, *lines);
	      ContinuousBitmap<bool> *array[] = { bm, poly };
	      OrContinuousBitmap *aor = new OrContinuousBitmap(array, array+2);
	      //cleanup.push_back(poly);
	      //cleanup.push_back(aor);
	      bm = aor;
	    }	  
	}
      Color c2(255,255,255);
      Color c(0,0,0,255);
      ChooseColorFunction func(c,c2);
      MapContinuousBitmap<bool,Color> mymap(*bm, func);
      SampleBitmap sample(mymap, std::ceil(sx),std::ceil(sy));
      Zoom zoom(sample, zoomlevel);
      BlitBitmap(ref, tl.x, tl.y, zoom);
    }
  // return bitmap
  return ref;

}

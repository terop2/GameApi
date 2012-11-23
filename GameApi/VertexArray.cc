
#include "VertexArray.hh"

//#define GL_GLEXT_PROTOTYPES
#define NO_SDL_GLEXT
#include "Effect.hh"
#include "Effect2.hh"
#include <GL/glew.h>
#include <SDL/SDL_opengl.h>

void VertexArraySet::push_poly(int id, int num, Point *points)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }
  if (num == 3)
    {
      p->tri_polys.push_back(points[0]);
      p->tri_polys.push_back(points[1]);
      p->tri_polys.push_back(points[2]);
    }
  else if (num==4)
    {
      p->quad_polys.push_back(points[0]);
      p->quad_polys.push_back(points[1]);
      p->quad_polys.push_back(points[2]);
      p->quad_polys.push_back(points[3]);
    }
  else
    {
      p->poly_polys.push_back(std::vector<Point>());
      int s = p->poly_polys.size()-1;
      for(int i=0;i<num;i++)
	{
	  p->poly_polys[s].push_back(points[i]);
	}
    }
}


void VertexArraySet::push_normal(int id, int num, Vector *points)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }
  if (num == 3)
    {
      p->tri_normals.push_back(points[0]);
      p->tri_normals.push_back(points[1]);
      p->tri_normals.push_back(points[2]);
    }
  else if (num==4)
    {
      p->quad_normals.push_back(points[0]);
      p->quad_normals.push_back(points[1]);
      p->quad_normals.push_back(points[2]);
      p->quad_normals.push_back(points[3]);
    }
  else
    {
      p->poly_normals.push_back(std::vector<Vector>());
      int s = p->poly_normals.size()-1;
      for(int i=0;i<num;i++)
	{
	  p->poly_normals[s].push_back(points[i]);
	}
    }
}

void VertexArraySet::push_attrib(int id, int attrib_id, int num, float *points)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }
  if (num == 3)
    {
      p->tri_attribs[attrib_id].push_back(points[0]);
      p->tri_attribs[attrib_id].push_back(points[1]);
      p->tri_attribs[attrib_id].push_back(points[2]);
    }
  else if (num==4)
    {
      std::cout << "Attrib1: " << points[0] << std::endl;
      std::cout << "Attrib2: " << points[1] << std::endl;
      std::cout << "Attrib3: " << points[2] << std::endl;
      std::cout << "Attrib4: " << points[3] << std::endl;
      p->quad_attribs[attrib_id].push_back(points[0]);
      p->quad_attribs[attrib_id].push_back(points[1]);
      p->quad_attribs[attrib_id].push_back(points[2]);
      p->quad_attribs[attrib_id].push_back(points[3]);
    }
  else
    {
      p->poly_attribs[attrib_id].push_back(std::vector<float>());
      int s = p->poly_attribs[attrib_id].size()-1;
      for(int i=0;i<num;i++)
	{
	  p->poly_attribs[attrib_id][s].push_back(points[i]);
	}
    }
}

void VertexArraySet::push_attribi(int id, int attrib_id, int num, int *points)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }
  if (num == 3)
    {
      p->tri_attribsi[attrib_id].push_back(points[0]);
      p->tri_attribsi[attrib_id].push_back(points[1]);
      p->tri_attribsi[attrib_id].push_back(points[2]);
    }
  else if (num==4)
    {
      p->quad_attribsi[attrib_id].push_back(points[0]);
      p->quad_attribsi[attrib_id].push_back(points[1]);
      p->quad_attribsi[attrib_id].push_back(points[2]);
      p->quad_attribsi[attrib_id].push_back(points[3]);
    }
  else
    {
      p->poly_attribsi[attrib_id].push_back(std::vector<int>());
      int s = p->poly_attribsi[attrib_id].size()-1;
      for(int i=0;i<num;i++)
	{
	  p->poly_attribsi[attrib_id][s].push_back(points[i]);
	}
    }
}

void VertexArraySet::push_color(int id, int num, unsigned int *points)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }
  if (num == 3)
    {
      p->tri_color.push_back(points[0]);
      p->tri_color.push_back(points[1]);
      p->tri_color.push_back(points[2]);
    }
  else if (num==4)
    {
      p->quad_color.push_back(points[0]);
      p->quad_color.push_back(points[1]);
      p->quad_color.push_back(points[2]);
      p->quad_color.push_back(points[3]);
    }
  else
    {
      p->poly_color.push_back(std::vector<unsigned int>());
      int s = p->poly_color.size()-1;
      for(int i=0;i<num;i++)
	{
	  p->poly_color[s].push_back(points[i]);
	}
    }
}


void VertexArraySet::push_texcoord(int id, int num, Point2d *points)
{
  Polys *p = m_set[id];
  if (!p)
    {
      m_set[id] = new Polys;
      p = m_set[id];
    }
  if (num == 3)
    {
      p->tri_texcoord.push_back(points[0]);
      p->tri_texcoord.push_back(points[1]);
      p->tri_texcoord.push_back(points[2]);
    }
  else if (num==4)
    {
      //std::cout << "Vertex: " << points[0] << " " << points[1] << " " << points[2] << " " << points[3] << std::endl;
      p->quad_texcoord.push_back(points[0]);
      p->quad_texcoord.push_back(points[1]);
      p->quad_texcoord.push_back(points[2]);
      p->quad_texcoord.push_back(points[3]);
    }
  else
    {
      p->poly_texcoord.push_back(std::vector<Point2d>());
      int s = p->poly_texcoord.size()-1;
      for(int i=0;i<num;i++)
	{
	  p->poly_texcoord[s].push_back(points[i]);
	}
    }
}
void RenderVertexArray::render(int id)
{
    // triangles
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)s.tri_polys(id));
    glNormalPointer(GL_FLOAT, 0, (GLvoid*)s.tri_normal_polys(id));
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, (GLvoid*)s.tri_color_polys(id));
    glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)s.tri_texcoord_polys(id));
    glDrawArrays(GL_TRIANGLES, 0, s.tri_count(id));
    // quads
    glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)s.quad_polys(id));
    glNormalPointer(GL_FLOAT, 0, (GLvoid*)s.quad_normal_polys(id));
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, (GLvoid*)s.quad_color_polys(id));
    glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)s.quad_texcoord_polys(id));
    glDrawArrays(GL_QUADS, 0, s.quad_count(id));

    // polygons
    int ss = s.poly_count(id);
    for(int i=0;i<ss;i++)
      {
	glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)s.poly_polys(id,i));
	glNormalPointer(GL_FLOAT, 0, (GLvoid*)s.poly_normal_polys(id,i));
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, (GLvoid*)s.poly_color_polys(id,i));
	glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)s.poly_texcoord_polys(id,i));
	glDrawArrays(GL_POLYGON, 0, s.poly2_count(id,i));
      }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }

void RenderVertexArray2::render(int id, int attr1, int attr2, int attr3, int attr4,
				int aattr1, int aattr2, int aattr3, int aattr4)
{
    // triangles
    //glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_NORMAL_ARRAY);
    //glEnableClientState(GL_COLOR_ARRAY);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableVertexAttribArray(aattr1);
    glEnableVertexAttribArray(aattr2);
    glEnableVertexAttribArray(aattr3);
    glEnableVertexAttribArray(aattr4);

    glEnableVertexAttribArray(attr1);
    glEnableVertexAttribArray(attr2);
    glEnableVertexAttribArray(attr3);
    glEnableVertexAttribArray(attr4);

    //glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)s1.tri_polys(id));
    //glNormalPointer(GL_FLOAT, 0, (GLvoid*)s1.tri_normal_polys(id));
    //glColorPointer(4, GL_UNSIGNED_BYTE, 0, (GLvoid*)s1.tri_color_polys(id));
    //glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)s1.tri_texcoord_polys(id));

    glVertexAttribPointer(aattr1, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s1.tri_polys(id));
    glVertexAttribPointer(aattr2, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s1.tri_normal_polys(id));
    glVertexAttribPointer(aattr3, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (GLvoid*)s1.tri_color_polys(id));
    glVertexAttribPointer(aattr4, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)s1.tri_texcoord_polys(id));

    glVertexAttribPointer(attr1, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s2.tri_polys(id));
    glVertexAttribPointer(attr2, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s2.tri_normal_polys(id));
    glVertexAttribPointer(attr3, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (GLvoid*)s2.tri_color_polys(id));
    glVertexAttribPointer(attr4, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)s2.tri_texcoord_polys(id));

    glDrawArrays(GL_TRIANGLES, 0, s1.tri_count(id));
    // quads
    //glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)s1.quad_polys(id));
    //glNormalPointer(GL_FLOAT, 0, (GLvoid*)s1.quad_normal_polys(id));
    //glColorPointer(4, GL_UNSIGNED_BYTE, 0, (GLvoid*)s1.quad_color_polys(id));
    //glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)s1.quad_texcoord_polys(id));

    glVertexAttribPointer(aattr1, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s1.quad_polys(id));
    glVertexAttribPointer(aattr2, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s1.quad_normal_polys(id));
    glVertexAttribPointer(aattr3, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (GLvoid*)s1.quad_color_polys(id));
    glVertexAttribPointer(aattr4, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)s1.quad_texcoord_polys(id));

    glVertexAttribPointer(attr1, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s2.quad_polys(id));
    glVertexAttribPointer(attr2, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s2.quad_normal_polys(id));
    glVertexAttribPointer(attr3, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (GLvoid*)s2.quad_color_polys(id));
    glVertexAttribPointer(attr4, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)s2.quad_texcoord_polys(id));

    glDrawArrays(GL_QUADS, 0, s1.quad_count(id));

    // polygons
    int ss = s1.poly_count(id);
    for(int i=0;i<ss;i++)
      {
	//glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)s1.poly_polys(id,i));
	//glNormalPointer(GL_FLOAT, 0, (GLvoid*)s1.poly_normal_polys(id,i));
	//glColorPointer(4, GL_UNSIGNED_BYTE, 0, (GLvoid*)s1.poly_color_polys(id,i));
	//glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)s1.poly_texcoord_polys(id,i));

	glVertexAttribPointer(aattr1, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s1.poly_polys(id,i));
	glVertexAttribPointer(aattr2, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s1.poly_normal_polys(id,i));
	glVertexAttribPointer(aattr3, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (GLvoid*)s1.poly_color_polys(id,i));
	glVertexAttribPointer(aattr4, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)s1.poly_texcoord_polys(id,i));

	glVertexAttribPointer(attr1, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s2.poly_polys(id,i));
	glVertexAttribPointer(attr2, 3, GL_FLOAT,  GL_FALSE, 0, (GLvoid*)s2.poly_normal_polys(id,i));
	glVertexAttribPointer(attr3, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (GLvoid*)s2.poly_color_polys(id,i));
	glVertexAttribPointer(attr4, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)s2.poly_texcoord_polys(id,i));

	glDrawArrays(GL_POLYGON, 0, s1.poly2_count(id,i));
      }

    //glDisableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_NORMAL_ARRAY);
    //glDisableClientState(GL_COLOR_ARRAY);
    //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableVertexAttribArray(attr1);
    glDisableVertexAttribArray(attr2);
    glDisableVertexAttribArray(attr3);
    glDisableVertexAttribArray(attr4);

    glDisableVertexAttribArray(aattr1);
    glDisableVertexAttribArray(aattr2);
    glDisableVertexAttribArray(aattr3);
    glDisableVertexAttribArray(aattr4);
  }

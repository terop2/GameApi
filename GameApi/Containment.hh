// 
// Copyright (C) 2009 Tero Pulkkinen
// Released under LGPL License.
//
// This file is part of Polygon.
//
// Polygon is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Polygon is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with Polygon.  If not, see <http://www.gnu.org/licenses/>.
//

#include "Tree.hh"
#include "Effect.hh"

class Container;

class Elem
{
public:
  virtual Container *Parent() const=0;
  
};

class Container
{
public:
  virtual Array<int, Elem*> *Children() const=0;
};

class Portal
{
public:
  virtual PortalPeer *First() const=0;
  virtual PortalPeer *Second() const=0;
};

class PortalPeer
{
public:
  virtual Matrix Position() const=0;
  virtual int PortalNum() const=0;
};

class Room
{
public:
  virtual ObjectTree *Drawables() const=0;
  virtual Array<int, PortalPeer*> *Portals() const=0;
};
class World : public Array<int, Room*>
{
public:
  virtual Array<int, Portal*> *Portals() const=0;
};
class Renderer
{
public:
  virtual void Render() const=0;
};

class WorldRenderer : public Renderer
{
public:
  WorldRenderer(World &world) : world(world) { }
  void PrepareWorld()
  {
    for(int i=0;i<world.Size();i++)
      {
	Room *r = world.Index(i);
	ObjectTree *drawable = r->Drawables();
	treestates[i] = new TreeState;
	PrepareTree(*drawable, *treestates[i]);
      }
  }
  void RenderOne(int roomnum, Matrix pos, float time) const
  {
    Room *currentroom = world.Index(roomnum);
    ObjectTree *drawable = currentroom->Drawables();
    RenderTree(*drawable, time, treestates[roomnum]);
  }
private:
  World &world;
  std::map<roomnum, TreeState*> treestates;
};

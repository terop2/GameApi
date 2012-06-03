
Color ColorCombine(Color c1, Color c2, float val) { return Color::Interpolate(c1,c2,val); }
Vector Reflect(Point ray_pos, Vector ray_dir, Point hit_pos, Vector normal)
{
  Vector v;
  return v;
}

ObjectProperties ToObj(SphereProperties &sp)
{
  ObjectProperties obj;
  obj.volume = sp.volume;
  obj.surfacepoints = sp.surfacepoints;
  obj.normals = sp.normals;
  return obj;
}


void PrepareStream(Stream3d &s, ArrayRender &r)
{
  Stream3dMesh r_mesh(s);
  Stream3dMeshNormals r_normal(s);
  Stream3dMeshTexCoords r_texcoord(s);
  Stream3dMeshColors r_color(s);

  r.Alloc(r_mesh.NumFaces(0), r_mesh.NumFaces(0)*r_mesh.NumPoints(), r_mesh.NumFrames(), r_normal.NumFrames(), r_color.NumFrames(), r_texcoord.NumFrames());
  
  r.InsertAll(r_mesh, r_normal, r_texcoord, r_color);
}
void RenderMovement(Movement &m, float time, ArrayRender &r)
{
}

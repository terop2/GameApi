
#include "GameApi_h.hh"

EXPORT GameApi::TXID GameApi::FrameBufferApi::tex_id(FBO buffer)
{
  FBOPriv *priv = find_fbo(e, buffer);
  GameApi::TXID id;
  id.id = priv->texture;
  return id;
}
EXPORT GameApi::TXID GameApi::FrameBufferApi::depth_id(FBO buffer)
{
  FBOPriv *priv = find_fbo(e, buffer);
  GameApi::TXID id;
  id.id = priv->depthbuffer;
  return id;
}
EXPORT GameApi::FBO GameApi::FrameBufferApi::create_fbo(int sx, int sy)
{
  GLuint fbo_name;
  glGenFramebuffers(1, &fbo_name);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_name);

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sx,sy, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  GLuint depth_texture;
  glGenRenderbuffers(1, &depth_texture);
  glBindRenderbuffer(GL_RENDERBUFFER, depth_texture);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, sx, sy);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_texture);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  return add_fbo(e, fbo_name, texture, depth_texture, sx,sy);
}
EXPORT void GameApi::FrameBufferApi::config_fbo(FBO buffer)
{
  FBOPriv *priv = find_fbo(e, buffer);
  
  glBindFramebuffer(GL_FRAMEBUFFER, priv->fbo_name);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, priv->texture, 0);
  //GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
  //glDrawBuffers(1, DrawBuffers);
  //glReadBuffer( GL_COLOR_ATTACHMENT0 );
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
EXPORT void GameApi::FrameBufferApi::bind_fbo(FBO buffer)
{
  FBOPriv *priv = find_fbo(e, buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, priv->fbo_name);
  glBindRenderbuffer(GL_RENDERBUFFER, priv->depthbuffer);
  glViewport(0,0,priv->sx,priv->sy);
}
EXPORT void GameApi::FrameBufferApi::bind_screen(int sx, int sy)
{
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glViewport(0,0,sx,sy);
}
EXPORT bool GameApi::FrameBufferApi::fbo_status(FBO buffer)
{
  FBOPriv *priv = find_fbo(e, buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, priv->fbo_name);
  int val = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  std::cout << "CheckFrameBuffer: " << val << "== " << GL_FRAMEBUFFER_COMPLETE << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return val == GL_FRAMEBUFFER_COMPLETE;
}
EXPORT GameApi::BM GameApi::FrameBufferApi::fbo_to_bitmap(EveryApi &ev, FBO buffer)
{
  EnvImpl *env = ::EnvImpl::Environment(&e);
  int screen_width = ev.mainloop_api.get_screen_width();
  int screen_height = ev.mainloop_api.get_screen_height();
  FBOPriv *priv = find_fbo(e, buffer);
  
  int width = priv->sx;
  int height = priv->sy;

  glBindTexture( GL_TEXTURE_2D, priv->texture);

  BufferRef ref = BufferRef::NewBuffer(width, height);
  glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, ref.buffer);


  Bitmap<Color> *bm = new BitmapFromBuffer(ref);
  //env->deletes.push_back(std::shared_ptr<void>(bm));
  //Bitmap<Color> *bm2 = new FlipColours(*bm);
  return add_color_bitmap2(e, bm);

}

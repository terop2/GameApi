
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

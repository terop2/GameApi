
  Env e;
  EveryApi ev(e);
#ifdef WINDOWS
  std::string homepageurl = "http://tpgames.org/";
  std::string seamless_url="";
  int w_width = 800;
  int w_height = 600;
  ev.mainloop_api.init_window(w_width,w_height);
  ev.mainloop_api.set_screen_size(w_width, w_height);
  ev.mainloop_api.set_homepage_url(homepageurl);
  ev.mainloop_api.set_seamless_url(seamless_url);
  ev.shader_api.load_default();
#endif
#ifdef LINUX
  std::string homepageurl = "http://tpgames.org/";
  std::string seamless_url="";
  int w_width = 800;
  int w_height = 600;
  ev.mainloop_api.init_window(w_width,w_height);
  ev.mainloop_api.set_screen_size(w_width, w_height);
  ev.mainloop_api.set_homepage_url(homepageurl);
  ev.mainloop_api.set_seamless_url(seamless_url);
  ev.shader_api.load_default();
#endif

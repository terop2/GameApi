
#ifndef TASKS_HH
#define TASKS_HH
#include "GameApi.hh"

IMPORT void tasks_init();
IMPORT void tasks_add(int id, void *(*fptr)(void*), void *data);
IMPORT void tasks_join(int id);
IMPORT void tasks_async_join(int id, void (*fptr)(void*), void *data);
IMPORT void tasks_async_join_m(int id, void (*fptr)(void*), void *data);
IMPORT void tasks_join_property(bool (*fptr)(void*), void *data); // bool=false => exit
IMPORT bool tasks_m_check_async_ongoing(int id);

int add_splitter_logo_callback(void (*fptr)(void*), void *data);
void remove_splitter_logo_callback(int id);

int add_mlguiwidget_logo_callback(void (*fptr)(void*), void *data);
void remove_mlguiwidget_logo_callback(int id);
void call_all_mlguiwidget_cbs();

class CollectInterfaceImpl;
class CollectInterface;
struct CollectData
{
  CollectInterfaceImpl *vis;
  int vis_counter;
  int vis_counter_before;
  bool has_vis;
};
CollectData collect(CollectInterface *i);
void collect_repeat(CollectData &d);


#endif

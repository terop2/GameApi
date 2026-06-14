
#ifndef TASKS_HH
#define TASKS_HH
#include "GameApi.hh"

IMPORT void tasks_init();
IMPORT void tasks_add(int id, void *(*fptr)(void*), void *data);
IMPORT void tasks_join(int id);
IMPORT void tasks_async_join(int id, void (*fptr)(void*), void *data);
IMPORT void tasks_async_join_m(int id, void (*fptr)(void*), void *data);


int add_splitter_logo_callback(void (*fptr)(void*), void *data);
void remove_splitter_logo_callback(int id);

int add_mlguiwidget_logo_callback(void (*fptr)(void*), void *data);
void remove_mlguiwidget_logo_callback(int id);
void call_all_mlguiwidget_cbs();

#endif

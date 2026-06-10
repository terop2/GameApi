
#ifndef TASKS_HH
#define TASKS_HH
#include "GameApi.hh"

IMPORT void tasks_init();
IMPORT void tasks_add(int id, void *(*fptr)(void*), void *data);
IMPORT void tasks_join(int id);
IMPORT void tasks_async_join(int id, void (*fptr)(void*), void *data);
#endif

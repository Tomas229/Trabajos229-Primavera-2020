#include <nSystem.h>
#include <fifoqueues.h>

//Me pasó todos los test

typedef struct
{
  nMonitor m, block;
  FifoQueue q0, q1; //Haciendo fila
  int ocup;
} Ctrl;

typedef struct
{
  int cat; /* 0 || 1 */
  nCondition w;
  int ready;
} Request;

Ctrl *c;

void init()
{
  c = (Ctrl *)nMalloc(sizeof(*c));
  c->block = nMakeMonitor();
  c->m = nMakeMonitor();
  c->q0 = MakeFifoQueue();
  c->q1 = MakeFifoQueue();
  c->ocup = -1;
}

void wakeup(int cat);

void pedir(int cat)
{
  nEnter(c->m);
  if (c->ocup != -1) //Si está ocupado espera
  {
    Request r;
    r.cat = cat;
    r.w = nMakeCondition(c->m);
    r.ready = FALSE;
    if (cat == 1)
    {
      PutObj(c->q1, &r); /* al final de queue */
    }
    else
    {
      PutObj(c->q0, &r); /* al final de queue */
    }

    while (!r.ready)
    {
      nWaitCondition(r.w);
    }
    nDestroyCondition(r.w);
  }
  else
  {
    c->ocup = cat;
  }
  nExit(c->m);
  nEnter(c->block);
}

void devolver()
{
  nEnter(c->m);
  if (c->ocup == 1 && !EmptyFifoQueue(c->q0))
  {
    c->ocup = 0;
    wakeup(0);
  }
  else if (c->ocup == 1 && !EmptyFifoQueue(c->q1))
  {
    wakeup(1);
  }
  else if (c->ocup == 0 && !EmptyFifoQueue(c->q1))
  {
    c->ocup = 1;
    wakeup(1);
  }
  else if (c->ocup == 0 && !EmptyFifoQueue(c->q0))
  {
    wakeup(0);
  }
  else
  {
    c->ocup = -1;
  }
  nExit(c->m);
  nExit(c->block);
}

void wakeup(int cat)
{
  if (cat == 1)
  {
    Request *pr = (Request *)GetObj(c->q1);
    if (pr == NULL)
      return;
    if (c->ocup == cat && pr != NULL)
    {
      pr->ready = TRUE;
      nSignalCondition(pr->w);
    }
    else if (pr != NULL)
    {
      PushObj(c->q1, pr); /* se devuelve al comienzo de q */
    }
  }
  else if (cat == 0)
  {
    Request *pr = (Request *)GetObj(c->q0);
    if (pr == NULL)
      return;
    if (c->ocup == cat && pr != NULL)
    {
      pr->ready = TRUE;
      nSignalCondition(pr->w);
    }
    else if (pr != NULL)
    {
      PushObj(c->q0, pr); /* se devuelve al comienzo de q */
    }
  }
}
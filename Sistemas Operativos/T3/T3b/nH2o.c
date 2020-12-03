#include "nSystem.h"
#include "nSysimp.h"
#include "fifoqueues.h"

FifoQueue qOxy;   //Cola para que esperen Oxy
FifoQueue qHydro; //Cola para que esperen Hydros

void cleanqOxy() //Saca los oxy que están ready debido a los timeout
{
  int n = LengthFifoQueue(qOxy);
  for (int i = 0; i < n; i++)
  {
    nTask o = GetObj(qOxy);
    if (o->status != READY)
    {
      PutObj(qOxy, o);
    }
  }
}

nH2o nCombineOxy(void *oxy, int timeout)
{
  START_CRITICAL();
  nTask this_task = current_task;
  this_task->oxyTask = oxy;
  this_task->h2oFinal = NULL;

  if (LengthFifoQueue(qHydro) > 1) //Hay almenos dos procesos con hydro esperando
  {
    nTask h1 = GetObj(qHydro);
    nTask h2 = GetObj(qHydro);

    nH2o h2o = nMalloc(sizeof(*h2o)); //Creación nH2o
    h2o->oxy = oxy;
    h2o->hydro1 = h1->hydroTask;
    h2o->hydro2 = h2->hydroTask;

    this_task->h2oFinal = h2o; //Actualizando a las nTask con sus nH2o
    h1->h2oFinal = h2o;
    h2->h2oFinal = h2o;

    h1->status = READY; //Cambiando a estado READY
    h2->status = READY;
    PushTask(ready_queue, h1);
    PushTask(ready_queue, h2);
  }
  else //EL oxy debe esperar y ver si utilizar timeout
  {
    if (timeout > 0)
    {
      this_task->status = WAIT_OXY_TIMEOUT;
      ProgramTask(timeout);
    }
    else
    {
      this_task->status = WAIT_OXY;
    }
    PutObj(qOxy, this_task); //Oxy entra a la cola de oxys
    ResumeNextReadyTask();
  }

  nH2o nH2oResult = this_task->h2oFinal;
  END_CRITICAL();
  return nH2oResult;
}

nH2o nCombineHydro(void *hydro)
{
  START_CRITICAL();
  nTask this_task = current_task;
  this_task->hydroTask = hydro;
  cleanqOxy();
  if (LengthFifoQueue(qHydro) > 0 && LengthFifoQueue(qOxy) > 0) //Hay almenos dos procesos con Oxy e hydro esperando
  {
    nTask o = GetObj(qOxy);
    nTask h = GetObj(qHydro);

    nH2o h2o = nMalloc(sizeof(*h2o)); //Creación nH2o
    h2o->oxy = o->oxyTask;
    h2o->hydro1 = h->hydroTask;
    h2o->hydro2 = hydro;

    this_task->h2oFinal = h2o; //Actualizando a las nTask con sus nH2o
    o->h2oFinal = h2o;
    h->h2oFinal = h2o;
    if (o->status == WAIT_OXY_TIMEOUT)
    {
      CancelTask(o);
    }
    o->status = READY; //Cambiando a estado READY
    h->status = READY;

    PushTask(ready_queue, o);
    PushTask(ready_queue, h);
  }
  else
  {
    this_task->status = WAIT_HYDRO;
    PutObj(qHydro, this_task);
    ResumeNextReadyTask();
  }

  nH2o nH2oResult = this_task->h2oFinal;
  END_CRITICAL();
  return nH2oResult;
}

void H2oInit()
{
  qOxy = MakeFifoQueue();
  qHydro = MakeFifoQueue();
}
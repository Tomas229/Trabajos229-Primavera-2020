#include "nSystem.h"
#include "nSysimp.h"
#include "fifoqueues.h"

FifoQueue qOxy;   //Cola para que esperen Oxy
FifoQueue qHydro; //Cola para que esperen Hydros

nH2o nCombineOxy(void *oxy, int timeout)
{
  START_CRITICAL();
  nTask this_task = current_task;
  this_task->oxyTask = oxy;
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
  else
  {
    this_task->status = WAIT_OXY;
    PutObj(qOxy, this_task);
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
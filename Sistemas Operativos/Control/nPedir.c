#include "nSystem.h"
#include "nSysimp.h"

//Me pasó todos los test

Queue q0;
Queue q1;
int ocup = -1;

void pedirInit()
{
  q0 = MakeQueue();
  q1 = MakeQueue();
}

void nPedir(int cat)
{
  START_CRITICAL();
  if (ocup == -1)
  {
    ocup = cat;
  }
  else
  {
    current_task->status = WAIT_PEDIR;
    if (cat == 1)
    {
      PutTask(q1, current_task);
    }
    else
    {
      PutTask(q0, current_task);
    }
    ResumeNextReadyTask();
  }
}

void nDevolver()
{
  if (ocup == 1 && !EmptyQueue(q0))
  {

    nTask t = GetTask(q0);
    t->status = READY;
    PushTask(ready_queue, t);
    ocup = 0;
  }
  else if (ocup == 1 && !EmptyQueue(q1))
  {
    nTask t = GetTask(q1);
    t->status = READY;
    PushTask(ready_queue, t);
  }
  else if (ocup == 0 && !EmptyQueue(q1))
  {
    nTask t = GetTask(q1);
    t->status = READY;
    PushTask(ready_queue, t);
    ocup = 1;
  }
  else if (ocup == 0 && !EmptyQueue(q0))
  {
    nTask t = GetTask(q0);
    t->status = READY;
    PushTask(ready_queue, t);
  }
  else
  {
    ocup = -1;
  }
  END_CRITICAL();
}

#include <nSystem.h>
#include <fifoqueues.h>
#include "spinlock.h"
#include "pedir.h"

SpinLock m;
FifoQueue q0, q1;

int ocup = -1; // -1: libre
// 0 o 1: ocupado por thread de cat 0 o 1

void iniciar()
{
    initSpinLock(&m, OPEN);
    q0 = MakeFifoQueue();
    q1 = MakeFifoQueue();
}

void terminar()
{
    destroySpinLock(&m);
}

void pedir(int cat)
{
    spinLock(&m);
    if (ocup == -1)
    {
        ocup = cat;
        spinUnlock(&m);
    }
    else
    {
        SpinLock s;
        initSpinLock(&s, CLOSED);
        if (cat)
        {
            PutObj(q1, &s);
        }
        else
        {
            PutObj(q0, &s);
        }
        spinUnlock(&m);
        spinLock(&s);
        destroySpinLock(&s);
    }
}

void devolver()
{
    spinLock(&m);
    if (ocup == 1 && !EmptyFifoQueue(q0))
    {
        ocup = 0;
        spinUnlock(GetObj(q0));
    }
    else if (ocup == 1 && !EmptyFifoQueue(q1))
    {
        spinUnlock(GetObj(q1));
    }
    else if (ocup == 0 && !EmptyFifoQueue(q1))
    {
        ocup = 1;
        spinUnlock(GetObj(q1));
    }
    else if (ocup == 0 && !EmptyFifoQueue(q0))
    {
        spinUnlock(GetObj(q0));
    }
    else
    {
        ocup = -1;
    }
    spinUnlock(&m);
}
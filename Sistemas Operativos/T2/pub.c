#include "fifoqueues.h"
#include <nSystem.h>

#define VARON 0
#define DAMA 1

typedef struct
{
    nMonitor m;
    FifoQueue qVarones, qDamas; //Haciendo fila
    int damas, varones;         //Ocupando el baño
} Ctrl;

typedef struct
{
    int sexo; /* DAMA || VARON */
    nCondition w;
    int ready;
} Request;

Ctrl *c;

void ini_pub(void)
{
    c = (Ctrl *)nMalloc(sizeof(*c));
    c->m = nMakeMonitor();
    c->qVarones = MakeFifoQueue();
    c->qDamas = MakeFifoQueue();
    c->damas = 0;
    c->varones = 0;
}

void wakeup(int sexo)
{
    if (sexo == VARON)
    {
        Request *pr = (Request *)GetObj(c->qVarones);
        if (c->damas == 0 && pr != NULL)
        {
            pr->ready = TRUE;
            nSignalCondition(pr->w);
            c->varones++;
        }
        else if (pr != NULL)
        {
            PushObj(c->qVarones, pr); /* se devuelve al comienzo de q */
        }
    }
    else if (sexo == DAMA)
    {
        Request *pr = (Request *)GetObj(c->qDamas);
        if (c->varones == 0 && pr != NULL)
        {
            pr->ready = TRUE;
            nSignalCondition(pr->w);
            c->damas++;
        }
        else if (pr != NULL)
        {
            PushObj(c->qDamas, pr); /* se devuelve al comienzo de q */
        }
    }
}

void entrar(int sexo)
{
    nEnter(c->m);
    int contar = TRUE;
    if (sexo == VARON)
    {
        if (c->damas != 0 || !EmptyFifoQueue(c->qDamas)) //Si hay damas adentro o en espera, el varón espera.
        {
            Request r;
            r.sexo = sexo;
            r.w = nMakeCondition(c->m);
            r.ready = FALSE;
            PutObj(c->qVarones, &r); /* al final de queue */
            while (!r.ready)
            {
                nWaitCondition(r.w);
            }
            nDestroyCondition(r.w);
            contar = FALSE;
        }
        if (contar)
        {
            c->varones++;
        }
        wakeup(VARON); //Le da paso al siguiente varón en la fila
    }
    else if (sexo == DAMA)
    {
        if (c->varones != 0 || !EmptyFifoQueue(c->qVarones)) //Si hay varones adentro o en espera, la dama espera.
        {
            Request r;
            r.sexo = sexo;
            r.w = nMakeCondition(c->m);
            r.ready = FALSE;
            PutObj(c->qDamas, &r); /* al final de queue */
            while (!r.ready)
            {
                nWaitCondition(r.w);
            }
            nDestroyCondition(r.w);
            contar = FALSE;
        }
        if (contar)
        {
            c->damas++;
        }
        wakeup(DAMA); //Le da paso a la siguiente dama en la fila
    }
    nExit(c->m);
}

void salir(int sexo)
{
    nEnter(c->m);
    if (sexo == VARON)
    {
        c->varones--;
        if (c->varones == 0) //Si es el último varón en el baño se deja que entre la primera dama en la fila
        {
            wakeup(DAMA);
        }
    }
    else if (sexo == DAMA)
    {
        c->damas--;
        if (c->damas == 0) //Si es la último dama en el baño se deja que entre el primera varón en la fila
        {
            wakeup(VARON);
        }
    }
    nExit(c->m);
}
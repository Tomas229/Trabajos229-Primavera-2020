#include <nQueue.h>

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
    c = (Ctrl)nMalloc(sizeof(*c));
    c->m = nMakeMonitor();
    c->qVarones = MakeFifoQueue();
    c->qDamas = MakeFifoQueue();
    c->damas = 0;
    c->varones = 0;
}

void entrar(int sexo)
{
    nEnter(c->m);

    if (sexo == VARON)
    {
        if (damas != 0 || !EmptyFifoQueue(c->qDamas)) //Si hay damas adentro o en espera, el varón espera.
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
        }
        c->varones++; //revisar
        Request *pr = (Request *)GetObj(c->qVarones);
        if (damas == 0 && pr != NULL)
        {
            pr->ready == TRUE;
            nSignalCondition(pr->w);
        }
        else if (pr != NULL)
        {
            PushObj(c->q, pr); /* se devuelve al comienzo de q */
        }
    }
    else if (sexo == DAMA)
    {
        if (varones != 0 || !EmptyFifoQueue(c->qVarones)) //Si hay varones adentro o en espera, la dama espera.
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
        }
        c->damas++; //revisar
        Request *pr = (Request *)GetObj(c->qDamas);
        if (varones == 0 && pr != NULL)
        {
            pr->ready == TRUE;
            nSignalCondition(pr->w);
        }
        else if (pr != NULL)
        {
            PushObj(c->q, pr); /* se devuelve al comienzo de q */
        }
    }

    nExit(c->m);
}

void salir(int sexo)
{
    if (sexo == VARON)
    {
        nEnter(c->m);
        c->varones--;
        if (c->varones == 0) //Si es el último varón en el baño se deja que entre la primera dama en la fila
        {
            Request *pr = (Request *)GetObj(c->qDamas);
            if (varones == 0 && pr != NULL)
            {
                pr->ready == TRUE;
                nSignalCondition(pr->w);
            }
            else if (pr != NULL)
            {
                PushObj(c->q, pr); /* se devuelve al comienzo de q */
            }
        }
        nExit(c->m);
    }
    else if (sexo == DAMA)
    {
        nEnter(c->m);
        c->damas--;
        if (c->damas == 0) //Si es la último dama en el baño se deja que entre el primera varón en la fila
        {
            Request *pr = (Request *)GetObj(c->qVarones);
            if (damas == 0 && pr != NULL)
            {
                pr->ready == TRUE;
                nSignalCondition(pr->w);
            }
            else if (pr != NULL)
            {
                PushObj(c->q, pr); /* se devuelve al comienzo de q */
            }
        }
        nExit(c->m);
    }
}

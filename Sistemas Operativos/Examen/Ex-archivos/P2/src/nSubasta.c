#include <nSystem.h>
#include "nSysimp.h"

// Encabezados para la cola de prioridades

typedef struct priqueue *PriQueue;

PriQueue MakePriQueue(int size);
void *PriGet(PriQueue pq);
void PriPut(PriQueue pq, void* t, int pri);
int PriSize(PriQueue pq);
int PriBest(PriQueue pq);
int EmptyPriQueue(PriQueue pq);
int PriLength(PriQueue pq);

// Defina aca el tipo struct subasta
// El typedef struct subasta *nSubasta esta en nSystem.h

struct subasta {
  ... complete ...
};


// Programe aca las funciones nuevaSubasta, ofrecer y adjudicar, mas
// otras funciones que necesite.

#define Resume ResumeNextReadyTask

nSubasta nNuevaSubasta(int n) {
  ... complete ...
  ... cree la cola de prioridades con MakePriQueue(100) ...
  return ... ;
}

int nOfrecer(nSubasta s, int oferta) {
  ... complete ...
  return ...;
}

int nAdjudicar(nSubasta s, int *punid) {
  ... complete ...
  return ...;
}

// Implementacion de la cola de prioridades: dada

#define MAXQSZ 100

struct priqueue {
  void **vec;
  int *ofertas;
  int size, maxsize;
};

PriQueue MakePriQueue(int maxsize) {
  PriQueue pq= nMalloc(sizeof(*pq));
  pq->maxsize= maxsize;
  pq->vec= nMalloc(sizeof(void*)*(maxsize+1));
  pq->ofertas= nMalloc(sizeof(int)*(maxsize+1));
  pq->size= 0;
  return pq;
}

void *PriGet(PriQueue pq) {
  void *t;
  int k;
  if (pq->size==0)
    return NULL;
  t= pq->vec[0];
  pq->size--;
  for (k= 0; k<pq->size; k++) {
    pq->vec[k]= pq->vec[k+1];
    pq->ofertas[k]= pq->ofertas[k+1];
  }
  return t;
}

void PriPut(PriQueue pq, void *t, int oferta) {
  if (pq->size==pq->maxsize)
    nFatalError("PriPut", "Desborde de la cola de prioridad\n");
  int k;
  for (k= pq->size-1; k>=0; k--) {
    if (oferta > pq->ofertas[k])
      break;
    else {
      pq->vec[k+1]= pq->vec[k];
      pq->ofertas[k+1]= pq->ofertas[k];
    }
  }
  pq->vec[k+1]= t;
  pq->ofertas[k+1]= oferta;
  pq->size++;
}

int PriSize(PriQueue pq) {
  return pq->size;
}

int PriBest(PriQueue pq) {
  return pq->size==0 ? 0 : pq->ofertas[0];
}

int EmptyPriQueue(PriQueue pq) {
  return pq->size==0;
}

int PriLength(PriQueue pq) {
  return pq->size;
}

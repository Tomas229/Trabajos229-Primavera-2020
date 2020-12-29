#include <nSystem.h>

#include "subasta.h"

//typedef struct
//{ PriQueue c; // cola de prioridades
//  int n;      // número de unidades
//  SpinLock m; // Exclusión mutua subasta
//} * Subasta;

struct subasta
{
  PriQueue c; // cola de prioridades
  int n;      // número de unidades
  SpinLock m; // Exclusión mutua subasta
};

// Agregue la estructura Req
typedef struct
{
  int oferta; // dinero ofrecido
  int resol;  // estado de la oferta: -1 pendiente,
              // 0 rechazada, 1 aprobada.
  SpinLock m; //Para esperar a estar listo
} Req;

Subasta nuevaSubasta(int n)
{
  Subasta s = malloc(sizeof(*s));
  s->n = n;
  s->c = MakePriQueue(n + 1);
  initSpinLock(&(s->m), OPEN);
  return s;
}
int ofrecer(Subasta s, int oferta)
{
  Req r = {oferta, -1};
  initSpinLock(&(r.m), CLOSED);

  spinLock(&(s->m)); //Sección crítica

  PriPut(s->c, &r, oferta);
  if (PriSize(s->c) > s->n)
  {
    Req *pr = PriGet(s->c);
    pr->resol = 0;        // se rechaza
    spinUnlock(&(pr->m)); //liberar al pr
  }

  spinUnlock(&(s->m)); //Fin sección crítica

  while (r.resol == -1)
    spinLock(&(r.m));
  return r.resol;
}
int adjudicar(Subasta s, int *punid)
{
  int recaud = 0;
  *punid = 0;

  spinLock(&(s->m)); //Sección crítica
  while (!EmptyPriQueue(s->c))
  {
    Req *pr = PriGet(s->c);
    pr->resol = 1; // se adjudica
    recaud += pr->oferta;
    spinUnlock(&(pr->m)); //liberar al pr
    (*punid)++;
  }
  s->n -= *punid;
  spinUnlock(&(s->m)); //Fin sección crítica
  return recaud;
}

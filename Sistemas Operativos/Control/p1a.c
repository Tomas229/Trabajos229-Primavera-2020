#include <nSystem.h>

//PAsa todos los test

nSem s[2]; // = nMakeSem(0) x 2
nSem m;
int cnt[2] = {0, 0};
// cnt[cat] es el n° de threads de
// categoría cat en espera.
int ocup = -1; // -1: libre
// 0 o 1: ocupado por thread de cat 0 o 1
void pedir(int cat)
{
  nWaitSem(m);
  if (ocup == -1)
  {
    ocup = cat;
    nSignalSem(m);
  }
  else
  {
    cnt[cat]++;
    nSignalSem(m);
    nWaitSem(s[cat]);
  }
}
void devolver()
{
  nWaitSem(m);
  if (cnt[!ocup] > 0)
  {
    cnt[!ocup]--;
    int old = ocup;
    ocup = !ocup; // ¡Atención!
    nSignalSem(s[!old]);
  }
  else if (cnt[ocup] > 0)
  {
    cnt[ocup]--;
    nSignalSem(s[ocup]);
    // ¡ocup se mantiene!
  }
  else
    ocup = -1;
  nSignalSem(m);
}
void init()
{
  s[0] = nMakeSem(0);
  s[1] = nMakeSem(0);
  m = nMakeSem(1);
}

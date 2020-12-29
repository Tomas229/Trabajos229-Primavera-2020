#include <nSystem.h>

#include "equipo.h"

char **equipo; // = nMalloc(5*sizeof(char*)); equipo en formación
int k = 0;     // cuantos jugadores han llegado
nSem m;        // = nMakeSem(1); para garantizar la exclusión mutua
nSem *tickets; // = nMakeSem(0); para hacer esperar hasta 4 jugadores

void init_equipo(void)
{
    equipo = nMalloc(5 * sizeof(char *));
    m = nMakeSem(1);
    tickets = nMalloc(4 * sizeof(nSem));
}

char **hay_equipo(char *nom)
{
    nWaitSem(m); // inicio secc. crítica
    int num = k;
    equipo[k++] = nom;
    char **miequipo = equipo;
    if (k != 5)
    { // aún no hay equipo
        nSem n = nMakeSem(0);
        tickets[num] = n; // Para poder esperar
        nSignalSem(m);    // fin secc. crít.
        nWaitSem(n);      //Esperamos
        nDestroySem(n);   //Destruimos el semáforo
    }
    else
    { // hay equipo: despertar a los 4 jugadores en espera
        for (int i = 0; i < 4; i++)
            nSignalSem(tickets[i]);
        // preparar nuevo equipo
        equipo = nMalloc(5 * sizeof(char *));
        k = 0;
        nSignalSem(m); // fin secc. crít.
    }
    return miequipo;
}
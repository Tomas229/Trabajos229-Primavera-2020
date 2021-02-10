# Descargue el nSystem desde:
# https://users.dcc.uchile.cl/~lmateu/CC4302/download/nsystem64-beta2.tgz
# Ud. debe descomprimir este archivo con:
# $ tar zxvf nsystem64-beta2.tgz
# Compile el nSystem con:
# $ cd nsystem64-beta2/src
# $ make

# Importante:
# Para compilar exitosamente su tarea necesita definir la variable de
# ambiente NSYSTEM con:
# $ export NSYSTEM=.../nsystem64-beta2
# (debe reemplazar ... por la ruta para llegar a nsystem64-beta2)

# Programe su tarea en el archivo equipo.c y luego pruebela con:
# $ cd <el directorio en donde se encuentra este archivo>
# $ make test-equipo
# $ ./test-equipo
#
# Las siguientes son otras formas de ejecutar su tarea para detectar
# errores de manejo de memoria o dataraces.  Pero *debe* usar pSystem64.
# No intente ejecutar valgrind, drd o sanitize con nSystem64.  Obtendria
# demasiados reportes falso positivos.
#
# make run-valgrind-ddd
# Lanza un proceso para que pueda ser depurado con ddd con el comando:
# target remote | vgdb --pid ...pid...
# Hay reemplazar ...pid... por el numero que reporta valgrind con ===numero===
# Si valgrind detecta un error de manejo de la memoria la ejecucion se detiene
# justo antes que ocurra y se puede usar ddd para revisar los valores de las
# variables del proceso.
#
# make run-drd-ddd
# Similar a run-valgrind-ddd pero chequea dataraces en vez de errores
# de manejo de memoria.  La ejecucion es extremadamente lenta.
# Es bueno alterna run-valgrind-ddd y run-drd-ddd porque a veces un
# error de manejo de memoria gatilla un datarace y a veces un datarace
# gatilla un error de manejo de memoria
#
# make run-sanitize
# Al igual que run-drd-ddd sirve para detectar dataraces pero la ejecucion
# es mucho mas rapida.  La desventaja es que no se puede usar con ddd y
# ademas *hay que recompilar* pSystem64 con make sanitize.


LIBNSYS= $(NSYSTEM)/lib/libnSys.a

# Con opciones de debugging
CFLAGS= -Wall -pedantic -std=c99 -g -I$(NSYSTEM)/include -I$(NSYSTEM)/src
LDFLAGS= -g
LDLIBS= -pthread $(LIBNSYS)

all: clean test-equipo

debug:
	make

optimize:
	make CFLAGS="-O -Wall -pedantic -std=c99 -I$(NSYSTEM)/include -I$(NSYSTEM)/src" LDFLAGS=""

test-equipo: test-equipo.o equipo.o $(LIBNSYS)

test-equipo.o test2-equipo.o equipo.o: equipo.h $(NSYSTEM)/include/nSystem.h $(NSYSTEM)/src/fifoqueues.h

run-valgrind-ddd:
	make CFLAGS="-g -DVALGRIND=1 -Wall -pedantic -std=c99 -I$(NSYSTEM)/include -I$(NSYSTEM)/src"
	bash -c 'valgrind --leak-check=full --vgdb=yes --vgdb-error=0 ./test-equipo |& tee run.log'

run-drd-ddd:
	make CFLAGS="-g -DVALGRIND=1 -Wall -pedantic -std=c99 -I$(NSYSTEM)/include -I$(NSYSTEM)/src"
	bash -c 'valgrind --tool=drd --vgdb=yes --vgdb-error=0 ./test-equipo |& tee run.log'

run-sanitize:
	make CFLAGS="-g -fsanitize=thread -DVALGRIND=1 -Wall -pedantic -std=c99 -I$(NSYSTEM)/include -I$(NSYSTEM)/src" LDFLAGS=-fsanitize=thread"
	bash -c './test-log |& tee run.log'

clean:
	rm -f *.o test-equipo

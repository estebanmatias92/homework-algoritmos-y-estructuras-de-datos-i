# Algoritmos y Estructuras de Datos I - Unidad Cuatrimestre - Actividad Integrador - Studio 54

## Descripción

Este programa es una aplicación de gestión de invitados desarrollada para Studio 54 con motivo de su fiesta de aniversario en la ciudad de Nueva York. La aplicación permite administrar una lista de invitados, incluyendo sus datos personales, y controlar la confirmación o cancelación de su asistencia a la fiesta. La interfaz se presenta en la consola y los datos de los invitados se almacenan de forma persistente en un archivo.

## Características

* **Gestión de Invitados**: Permite agregar, borrar y modificar los datos personales de los invitados.
* **Control de Asistencia**: Registra si un invitado ha confirmado o cancelado su asistencia a la fiesta.
* **Listado de Asistentes**: Muestra un listado de todos los invitados en la pantalla.
* **Actualización de Estado por Ticket**: Permite confirmar o cancelar la asistencia de un invitado buscando por su número de ticket.
* **Persistencia de Datos**: Los datos de los invitados se guardan en un archivo (`guests.txt`).
* **Funciones de Utilidad**: Incluye funciones para limpiar la consola y pausar la ejecución para una mejor interacción con el usuario.

## Estructura de directorios

```shell
 .
├──  .env
├──  .gitignore
├──  compose.yaml
├──  core-service
│   ├──  .dockerignore
│   ├──  compose.yaml
│   ├──  Dockerfile
│   └── 󱧼 src
│       └──  main.cpp
├──  Makefile
└──  README.md
```

## Uso

### Para ejecución directa

Compilar con:

```shell
# From ./core-service
mkdir -p build
g++ ./src/main.cpp -o ./build/main
```

Ejecutar con:

```shell
# From ./core-service
./build/main
```

### Para despliegue con Docker

Ejecutar el siguiente comando desde directorio raiz:

```shell
# With Make
make up-prod

# With compose
docker compose run --build --rm app
```

### Para desarrollo

Ejecutar:

```shell
# With Make
make run-dev

# With compose from ./core-service
docker compose run --rm dev
```

## Requisitos

* Compilador C++ (g++ recomendado)

* Docker (opcional, solo para entornos containerizados)

* Sistema operativo con terminal
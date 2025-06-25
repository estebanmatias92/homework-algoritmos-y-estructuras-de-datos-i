# Algoritmos y Estructuras de Datos I - Actividad Integradora - Cloro-Gab Clientes y Productos

## Descripción

Esta aplicación en C++ está diseñada para la empresa "Cloro-Gab" y permite registrar y gestionar los pedidos de productos de sus clientes. En esta etapa, el programa se enfoca en los módulos de "Clientes" y "Productos", permitiendo la administración de la información de los clientes y los productos específicos que cada uno adquiere. La interfaz opera a través de la consola y los datos se almacenan de forma persistente en un archivo de texto plano.

## Características

- **Gestión de Clientes**: Permite dar de alta nuevos clientes, eliminarlos por su código y modificar su información personal (nombre, teléfono, email).
- **Gestión de Productos por Cliente**: Cada cliente puede tener asociados múltiples productos. La aplicación soporta el CRUD (Crear, Leer, Actualizar, Eliminar) de productos para un cliente específico.
- **Estructura de Datos Anidada**: Los datos están organizados en una jerarquía que anida productos dentro de clientes, reflejando una relación "cliente-producto" en memoria mediante structs y arreglos.
- **Listado Completo de Clientes**: Muestra un listado general de todos los clientes registrados, incluyendo un resumen de sus productos asociados.
- **Búsqueda y Visualización Detallada (Listar Uno)**: Permite buscar un cliente por su código único y mostrar todos sus detalles, incluyendo la lista completa de sus productos adquiridos.
- **Persistencia de Datos**: Toda la información de clientes y sus productos se guarda y carga desde un archivo de texto (clients_and_products.txt), asegurando que los datos no se pierdan entre sesiones.
- **Modularidad y Utilidad**: El código está modularizado en funciones para una mejor organización y reusabilidad. Incluye funciones de utilidad para mejorar la interacción con el usuario, como limpiar la consola y pausar la ejecución.

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
docker compose up --build
```

### Para desarrollo

Ejecutar:

```shell
# With Make
make run-dev

# With compose from ./core-service
docker compose --profile interactive run --rm dev_interactive
```

## Requisitos

- Compilador C++ (g++ recomendado)

- Docker (opcional, solo para entornos containerizados)

- Sistema operativo con terminal

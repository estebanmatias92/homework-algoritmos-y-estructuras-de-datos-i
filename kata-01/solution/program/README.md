# Algoritmos y Estructuras de Datos I - Unidad 04 - Kata 01

## Descripción

A completar...

## Características

- A completar...
- A completar...
- A completar...
- A completar...

## Estructura de directorios

```shell
 .
├──  .gitignore
├──  core-service
│   ├──  .dockerignore
│   ├── 󱧼 src
│   │    └──  main.cpp
│   ├──  compose.yaml
│   └──  Dockerfile
├──  compose.yaml
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

Ejecutar el siguiente comando desde directorio raiz

```shell
docker compose run --build --rm app
```

### Para desarrollo

Ejecutar:

```shell
# From ./core-service
docker compose run --rm dev
```

## Requisitos

- Compilador C++ (g++ recomendado)
- Docker (opcional, solo para entornos containerizados)
- Sistema operativo con terminal

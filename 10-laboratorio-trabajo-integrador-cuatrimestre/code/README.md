# Prácticas Profesionalizantes I - Actividad 07

## Descripción

Programa que muestra un menú principal con operaciones básicas y variadas. Permite realizar cálculos matemáticos y conversiones numéricas mediante submenús interactivos.

## Características

- Menú principal con opciones para operaciones básicas y variadas
- Submenú de operaciones básicas (suma, resta, multiplicación, división)
- Submenú de operaciones variadas (conversión numérica, sumatoria, factorial, inversión de número)
- Funciones de limpieza de consola y pausa multiplataforma

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

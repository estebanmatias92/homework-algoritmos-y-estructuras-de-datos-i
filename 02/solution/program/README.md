# Algoritmos y Estructuras de Datos I - Actividad 02

## Descripción

Programa básico en C++ que muestra un mensaje de saludo y espera la interacción del usuario.

## Características

- Muestra el mensaje "Hola ISFT 151!"
- Espera que el usuario presione Enter antes de finalizar
- Implementado usando funciones estándar de E/S

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

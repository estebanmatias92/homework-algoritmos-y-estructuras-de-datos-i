---
title: Algoritmos y Estructuras de Datos I - Unidad 01 - Actividad 01
filename: lapenta-carlos-matias-unidad-01-actividad-01
type: assignment
date: 2025-03-28
due_date: 2025-04-04
student: Lapenta Carlos Matías
course:
  name: Algoritmos y Estructuras de Datos I
  code: ""
professor: José Luis Oemig
unit: 1
assignment: 1
topic: Introducción Programación
difficulty: very-easy
priority: high
status_completed: true
status_submitted: true
status_graded: false
grading:
  max_points: 100
  earned_points: 0
  weight: 0
submission_type: Word_document
submission_file_format: .docx
submission_platform: Google_Classroom
links_instructions: https://drive.google.com/file/d/1P_PEGZIvEWyV3vZisp7nJWPkOYZP3_jA/view
links_subsmission: https://classroom.google.com/c/Njg4NzQ0NDQwNjQy/a/Njk4NjAyNDg2MzQ5/details
links_local_copy: "[[TP - Unidad 1 Parte I.pdf]]"
ai_chat_links:
  - https://chat.deepseek.com/a/chat/s/81e50d2a-6c0b-42ad-87a7-2dbcdf8eec73
  - https://notebooklm.google.com/notebook/6d0c8014-c334-4517-aa58-004bc5e8ed37
tags:
  - algorithms
  - data-structures
  - first-year
  - assignment
  - unit-01
---
# Algoritmos y Estructuras de Datos I - Unidad 01 - Actividad 01
<!--
- **Materia**: `= this.course.name`
- **Unidad**: `= this.unit`
- **Actividad**: `= this.assignment`
- **Tema**: `= this.topic`
- **Profesor**: `= this.professor`
- **Estudiante**: `= this.student`
- **Fecha de entrega**: `= this.due_date`
- **Completado**: `= this.status_completed`
- **Instrucciones**: `= this.links_instructions`
- **Copia Local**: `= this.links_local_file`
- **Enlace de Entrega**: `= this.links_submission`
-->
- **Estudiante**: Lapenta Carlos Matías

## 📌 Descripción de la Actividad

- [Plataforma de Entrega](https://classroom.google.com/c/Njg4NzQ0NDQwNjQy/a/Njk4NjAyNDg2MzQ5/details) | [Instrucciones](https://drive.google.com/file/d/1P_PEGZIvEWyV3vZisp7nJWPkOYZP3_jA/view)
- Objetivos principales:
    - Responder el siguiente cuestionario en función de la bibliografía Obligatoria.
- Formato de entrega requerido: **.docx**

## 📝 Desarrollo

### 1. Definir los conceptos de Informática y computadora.

La **informática** es el conjunto de teorías y métodos sobre el _procesamiento automático de la información_. Este procesamiento puede darse _a través de sistemas digitales_ como computadoras. 

Una **computadora** es un objeto que calcula, un _dispositivo electrónico programable_ capaz de _almacenar, recuperar y procesar datos_. Puede introducir, almacenar, recuperar, procesar y enviar datos. Compara valores y _realiza operaciones aritméticas_.

### 2. Definir Hardware y Software

**Hardware** se refiere a los _componentes físicos de una computadora_. Estos componentes son dispositivos electrónicos que se pueden clasificar en:

- **Unidad de Memoria**
- **Unidad Aritmética/Lógica** (ALU)
- **Unidad de Control** (CU)
- **Dispositivos de Entrada**
- **Dispositivos de Salida**
- **Dispositivos de Almacenamiento Auxiliares**

La UAL y la UC comprenden la "_Unidad Central de Procesamiento_" o _CPU_ (Central Processing Unit), mientras que a los dispositivos de entrada/salida y almacenamiento auxiliar se los conoce como "_Periféricos_".

El **Software** comprende el _conjunto de programas que permiten operar el hardware_. Los programas son series de instrucciones ordenadas y finitas que _se ejecutan en la computadora_ e interactúan con el hardware.

### 3. Describir los roles de los Programadores (extender lo visto en clase)

Los programadores son los encargados de traducir algoritmos a secuencias de instrucciones para que la computadora pueda ejecutarlas. Desarrollan soluciones mediante lenguajes que el hardware entiende y puede procesar.

Pueden trabajar solos o en equipo y tienen la responsabilidad de entregar soluciones de calidad sin errores, independientemente del dominio en el que se encuentre trabajando y las herramientas que use.

### 4. Relacionar los conceptos de la Diapositiva 9

La **algoritmia** es la _resolución de problemas paso a paso_. Son métodos lógicos y eficientes, en el diseño e implementación de instrucciones secuenciales y ordenadas, que realizan un trabajo.

La **programación** es la _traducción de esas instrucciones_ o pasos en _lenguajes que las computadoras puedan entender y ejecutar_. Un algoritmo digital es un programa que puede ser interpretado por la computadora y realiza una tarea.

### 5. Describir el Modelo de Von Neumann

El **Modelo de Von Neumann** plantea 4 componentes principales para un sistema computacional:

- **Dispositivos I/O** (Entrada/Salida): Permite la comunicación con el exterior (Periféricos), _envía y recibe información del CPU_
- **Unidad Central de Procesamiento** (CPU): Procesa los datos recibidos, decide cómo procesarlos, dónde almacenarlos (RAM o secundario) y cómo mostrarlos siguiendo instrucciones del software.
  - **Unidad Aritmético/Lógica** (ALU): Realiza los _cálculos y operaciones lógicas_
  - **Unidad de Control** (CU): Coordina las operaciones siguiendo instrucciones del programa
- **Memoria** (RAM): Almacena temporalmente _datos_ e _instrucciones_ (programas) en el mismo espacio
- **Buses**: Canales que permiten la _comunicación entre componentes_

La inmediata adición de **almacenamiento secundario** o almacenamiento permanente permitió al sistema de una computadora _guardar los programas y datos_ almacenados, sin que se borraran al apagarse.

### 6. Qué es un Lenguaje de Programación, qué permite

Un **lenguaje de programación** es un _conjunto de reglas, símbolos y palabras especiales_ que se utilizan para darle instrucciones a una computadora. Permiten al programador expresar algoritmos en un idioma que el dispositivo electrónico programable entiende y puede ejecutar.

### 7. Describir los lenguajes de Alto, Medio y Bajo Nivel, dar ejemplos

El **nivel de los lenguajes** está dado por la _abstracción_ o distancia que posean del lenguaje binario que la CPU utiliza para realizar las operaciones aritméticas.

_A mayor semejanza con el lenguaje natural, mayor abstracción_ y mayor es el nivel del lenguaje. Se puede decir que _cuanto más "cerca del metal"_ se encuentra el lenguaje, _menor abstracción_ y menor nivel tiene.

**Niveles**:

- **Alto**: Son lenguajes que _se asemejan al idioma inglés_ y a otros lenguajes naturales (C++, Java, Fortran), no permiten el control de los recursos (hardware) de manera directa.
- **Medio**: Son considerados los lenguajes que, si bien implementan sintaxis de lenguaje natural, _todavía permiten control sobre el hardware_ de manera directa (C y Pascal serían considerados de nivel medio)
- **Bajo**: Son lenguajes que no se asemejan al lenguaje natural, tienen poco nivel de abstracción y permiten el _control total del hardware de manera directa_. (Código Máquina y Ensamblador)

### 8. Qué es un Compilador, qué función cumple

Un **compilador** es un **programa que traduce** **código fuente** a **código objeto** o **código máquina** (**ejecutable**). Realiza análisis morfológico, sintáctico y semántico antes de producir el código objeto.

1. **Análisis morfológico**: Divide el programa en unidades sintácticas (símbolos, palabras clave, literales, todo componente de una expresión es separado en "tokens")
2. **Análisis sintáctico** (parser): Revisa la estructura gramatical. (Analiza las expresiones de las instrucciones)
3. **Análisis semántico**: Comprueba la corrección del significado de las instrucciones dentro de una rutina.

El código objetivo generado se puede guardar y ejecutar repetidamente sin recompilar.

### 9. Qué es un Intérprete, cómo funciona

Un **intérprete** es un _traductor que analiza y ejecuta directamente_ un programa escrito en un código de alto nivel, procesa y ejecuta instrucción por instrucción en _tiempo de ejecución_.

Tiene un funcionamiento similar al compilador, con los respectivos _análisis morfológicos, sintácticos y semánticos_, pero no genera código objeto, en su lugar, _analiza y ejecuta secuencialmente_ cada instrucción.

### 10. Relacionar Java Virtual Machine, .Net Framework con JIT (ampliar lo visto)

**Java Virtual Machine** (JVM) y **Common Language Runtime** (CLR, .NET virtual machine) son _máquinas virtuales, entornos de ejecución_. _Traducen "bytecode" o código intermedio_ a lenguaje máquina _y ejecutan instrucciones_ en la computadora.

El _código intermedio_ (**bytecode** para Java y **CIL** para .NET) es _generado previamente por compiladores_ de los respectivos entornos de desarrollo (JDK o .NET SDK).

**Just-In-Time compilation** es una estrategia de _compilación dinámica en tiempo de ejecución_. Partes del bytecode se compilan a lenguaje máquina en tiempo de ejecución, para aprovechar la eficiencia de la ejecución de código nativo.

### 11. Relacionar Problema, Algoritmo y Programa (armar un mapa conceptual o similar, trabajarlo en grupo)

![Mapa Conceptual Unidad 01 Actividad 01](mapa-conceptual-unidad-01-actividad-01.png)

### 12. Describir el concepto que afirma "SOMMERVILLE" en su libro Ingeniería de Software

Según Sommerville, un **modelo del proceso de software** es una _representación abstracta o simplificada del proceso de desarrollo de software_.

Cada representación ofrece una visión parcial del proceso de desarrollo. Un _modelo de actividad del proceso_ puede mostrar actividades y su secuencia, pero no detallar roles o personas involucradas.

Los modelos no son descripciones definitivas, son abstracciones que explican diferentes enfoques del proceso de desarrollo. Son considerados "_marcos de trabajo_" (frameworks).

### 13. Describir el Modelo Iterativo e Incremental

En el **modelo iterativo e incremental**, el software se _desarrolla a través de múltiples ciclos_ (iteraciones), en _cada iteración_ se produce un incremento funcional del software, _añadiendo nuevas funcionalidades_ y agregando valor con cada ciclo.

### 14. Relacionar UML, Ingeniería de Software y modelos

El **Lenguaje de Modelo Unificado** (UML) es un _lenguaje gráfico estándar_ que _permite la creación de modelos_. Estos **modelos** son _representaciones abstractas, que permiten comprender, diseñar, comunicar y documentar_ un sistema de software en su ciclo de desarrollo.

La **Ingeniería de Software** se ocupa de todos los aspectos de la producción de software (especificación, diseño, desarrollo, validación, etc.), y _utiliza modelos (a menudo con UML) para gestionar la complejidad del software_.

### 📚 Material de Referencia

- [[Unidad 1 Parte 1.pptx]]

### 📂 Archivos Adjuntos

- [[lapenta-carlos-matias-unidad-01-actividad-01.docx]]
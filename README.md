
# 🚀 Minishell: Un Intérprete de Comandos a Medida

Este proyecto es nuestra propia versión de un intérprete de comandos de UNIX, inspirado en `bash`. Nacido en el entorno de **42**, este Minishell no solo ejecuta comandos, sino que cuenta una historia de adaptación, colaboración y aprendizaje sobre los entresijos de los sistemas operativos.

-----

## 📋 Índice

  * [La Historia de Nuestro Minishell](https://github.com/florindaperez/minishell_bonus?tab=readme-ov-file#-la-historia-de-nuestro-minishell)
  * [Arquitectura del Proyecto](https://github.com/florindaperez/minishell_bonus?tab=readme-ov-file#%EF%B8%8F-arquitectura-del-proyecto)
  * [Diagrama de Flujo de Ejecución](https://github.com/florindaperez/minishell_bonus/blob/main/README.md#-diagrama-de-flujo-de-ejecuci%C3%B3n)
  * [Características Implementadas](https://github.com/florindaperez/minishell_bonus?tab=readme-ov-file#-caracter%C3%ADsticas-implementadas)
  * [Cómo Empezar](https://github.com/florindaperez/minishell_bonus?tab=readme-ov-file#%EF%B8%8F-c%C3%B3mo-empezar)
  * [Uso y Ejemplos](https://github.com/florindaperez/minishell_bonus?tab=readme-ov-file#-uso-y-ejemplos)
  * [Comandos Built-in](https://github.com/florindaperez/minishell_bonus?tab=readme-ov-file#%EF%B8%8F-comandos-built-in)
  * [Autores](https://github.com/florindaperez/minishell_bonus?tab=readme-ov-file#%EF%B8%8F-autores)
  * [Agradecimientos](https://github.com/florindaperez/minishell_bonus?tab=readme-ov-file#-agradecimientos)

-----

## 📖 La Historia de Nuestro Minishell

Este proyecto comenzó con un enfoque clásico: un **parser** y un **ejecutor** que trabajaban en conjunto. Sin embargo, a final  de camino, el equipo se deshizo. Con un nuevo compañero, vino una nueva visión y una base de código diferente que tuvimos que integrar conjuntamente.

El reto fue adaptar dos partes de dos proyectos a una arquitectura de tres fases:

1.  **Parseo**: El módulo original de mi compañero.
2.  **Traducción**: Una capa intermedia que preparaba los datos para la ejecución.
3.  **Ejecución**: El motor final que corría los comandos.

Esta adaptación fue un desafío de integración muy interesante y el resultado es un Minishell con una estructura modular y robusta, reflejada en la distribución de nuestros archivos.

-----

## 🏗️ Arquitectura del Proyecto

Nuestro Minishell procesa los comandos en tres etapas principales, cada una gestionada por un módulo específico:

1.  **`Parser` (Analizador sintáctico)**

      * **Responsabilidad**: Lee la línea de entrada del usuario y la descompone en *tokens*.
      * **Funcionamiento**: Gestiona las comillas, espacios y caracteres especiales (`|`, `<`, `>`). Su salida es una **lista enlazada** de tokens, una estructura flexible ideal para el análisis sintáctico.

2.  **`Translator` (Traductor)**

      * **Responsabilidad**: Convierte la lista de tokens del parser en una estructura optimizada para el ejecutor.
      * **Funcionamiento**: "Traduce" la sintaxis del shell a una estructura de datos ejecutable. Es una etapa clave donde, por ejemplo, las listas de tokens se transforman en **arrays de strings** (`char **`), que es el formato que esperan las llamadas al sistema.

3.  **`Executor` (Ejecutor)**

      * **Responsabilidad**: Toma la estructura de datos del traductor y la ejecuta.
      * **Funcionamiento**: Crea los procesos hijos, configura las redirecciones de E/S, maneja las tuberías (`pipe`) y finalmente ejecuta los comandos, ya sean *built-in* o del sistema.

#### Detalles Técnicos de Implementación

  * **Transformación de Datos:** El **parser** trabaja con **listas enlazadas** por su flexibilidad, pero el **ejecutor** necesita **arrays** de strings para interactuar con las llamadas al sistema (`execve`). El **traductor** es el puente que realiza esta conversión esencial.
  * **Gestión de Estado con Punteros:** Para mantener la persistencia del estado de minishell (como las variables de entorno o el directorio actual), el programa utiliza una estructura principal. Se pasan **punteros** a esta estructura a los diferentes módulos. Así, cuando un comando *built-in* como `export` o `cd` modifica el entorno, el cambio es global y permanente para la sesión del minishell.

<p align="right"><a href="#-índice">⬆️ Volver al Índice</a></p>

-----

## 📊 Diagrama de Flujo de Ejecución

Nuestro shell procesa cada comando siguiendo un ciclo claro y modular, como ilustra el diagrama.

<img width="810" height="910" alt="image" src="https://github.com/user-attachments/assets/9211b1fe-6b68-4861-b1a1-75f4a911669f" />


El proceso se puede resumir en 3 grandes fases:

1.  **Análisis de la Entrada**

      * **Flujo:** `Readline` → `Tokenizer` → `Expander` → `Parser`
      * Se lee la entrada, se descompone en tokens, se expanden las variables (`$VAR`) y se valida la sintaxis.
      * **Módulos clave:** `/src/tokenizer/`, `/src/expander/`, `/src/parser/`.

2.  **Ejecución del Comando**

      * **Flujo:** `Translator` → `Executor`
      * Los tokens se "traducen" a una estructura ejecutable y se procesan, gestionando tuberías, redirecciones y comandos.
      * **Módulos clave:** `/src/translate/`, `/src/executor/`.

3.  **Limpieza**

      * **Flujo:** `Cleaner`
      * Al final del ciclo, se libera toda la memoria dinámica para evitar fugas.

  <p align="right"><a href="#-índice">⬆️ Volver al Índice</a></p>

-----

## ✨ Características Implementadas

  * **Prompt Interactivo**: Muestra un prompt dinámico y está listo para recibir comandos.
  * **Historial de Comandos**: Navega por los comandos previos usando las flechas (gracias a `readline`).
  * **Parsing Robusto**: Interpreta correctamente comillas, escapes y caracteres especiales.
  * **Ejecución de Comandos del Sistema**: Busca y ejecuta comandos usando la variable de entorno `$PATH`.
  * **Pipes (`|`)**: Encadena múltiples comandos, conectando la salida de uno con la entrada del siguiente.
  * **Redirecciones**:
      * `>`: Redirección de salida.
      * `>>`: Redirección de salida en modo apendizaje (*append*).
      * `<`: Redirección de entrada.
      * `<<`: Heredoc
  * **Expansión de Variables de Entorno**: Reconoce y expande variables como `$USER` o `$HOME`.
  * **Gestión de Señales**:
      * `Ctrl-C`: Muestra un nuevo prompt sin cerrar el shell.
      * `Ctrl-D`: Cierra el shell de forma limpia.
      * `Ctrl-\`: No interrumpe el shell.

<p align="right"><a href="#-índice">⬆️ Volver al Índice</a></p>

-----

## ⚙️ Cómo Empezar

Para compilar y ejecutar nuestro Minishell, solo necesitas `make` y un compilador de C.

1.  **Clona el repositorio:**

    ```sh
    git clone https://github.com/florindaperez/minishell.git
    ```

2.  **Navega al directorio del proyecto:**

    ```sh
    cd minishell
    ```

3.  **Compila el proyecto:**

    ```sh
    make
    ```

    Esto generará el ejecutable `minishell`.

<p align="right"><a href="#-índice">⬆️ Volver al Índice</a></p>


-----

## 🕹️ Uso y Ejemplos

Inicia el shell con:

```sh
./minishell
```

Aquí tienes algunos ejemplos de lo que puedes hacer:

  * **Listar archivos y contar las líneas de un `grep`:**
    ```sh
    ls -l | grep ".c" | wc -l
    ```
  * **Guardar la salida en un archivo:**
    ```sh
    cat minishell.c > contenido.txt
    ```

<p align="right"><a href="#-índice">⬆️ Volver al Índice</a></p>


-----

## 🛠️ Comandos Built-in

Hemos implementado los siguientes comandos directamente en el shell:

| Comando    | Descripción                                                   |
| :--------- | :------------------------------------------------------------ |
| **`echo`** | Muestra texto en la salida estándar (con soporte para `-n`).  |
| **`cd`** | Cambia el directorio de trabajo actual.                       |
| **`pwd`** | Muestra la ruta del directorio actual.                        |
| **`export`**| Crea o modifica variables de entorno.                         |
| **`unset`**| Elimina variables de entorno.                                 |
| **`env`** | Muestra todas las variables de entorno.                       |
| **`exit`** | Sale del minishell, finalizando el proceso.                   |

-----

## ✍️ Autores

Este proyecto fue un esfuerzo de colaboración entre:

  * **[Joan C.](https://github.com/jocuni-p)** - Desarrollo inicial de la parte de parseo .
  * **[Florinda P.](https://github.com/florindaperez)** - Desarrollo de la ejecución y adaptación e integración de todos los módulos.

-----

## 🎉 Agradecimientos

  * A la increíble comunidad de **42 Barcelona** por el apoyo constante.
  * A nuestros compañeros y revisores, cuyo *feedback* fue clave para mejorar el proyecto.

<p align="right"><a href="#-índice">⬆️ Volver al Índice</a></p>


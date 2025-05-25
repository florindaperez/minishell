#!/bin/bash

# --- Configuración ---
MINISHELL_EXECUTABLE_REL="./minishell" # Ruta relativa original
BASH_EXECUTABLE="/bin/bash"      # O /bin/sh si prefieres sh como referencia

# Convertir MINISHELL_EXECUTABLE_REL a una ruta absoluta
if [[ "$MINISHELL_EXECUTABLE_REL" != /* ]]; then
    # Asegurarse de que pwd no falle si estamos en un directorio eliminado
    current_dir=$(pwd 2>/dev/null)
    if [ -z "$current_dir" ]; then
        echo -e "\033[0;31mERROR: No se pudo obtener el directorio actual (pwd falló). Saliendo.\033[0m"
        exit 1
    fi
    MINISHELL_EXECUTABLE="$current_dir/$MINISHELL_EXECUTABLE_REL"
else
    MINISHELL_EXECUTABLE="$MINISHELL_EXECUTABLE_REL"
fi

# Verificar si el ejecutable de minishell existe y es ejecutable
if [ ! -f "$MINISHELL_EXECUTABLE" ]; then
    echo -e "\033[0;31mERROR: El ejecutable de minishell NO EXISTE en: $MINISHELL_EXECUTABLE\033[0m"
    echo -e "\033[0;31mPor favor, ajusta la variable MINISHELL_EXECUTABLE_REL o compila tu minishell.\033[0m"
    exit 1
elif [ ! -x "$MINISHELL_EXECUTABLE" ]; then
    echo -e "\033[0;31mERROR: El ejecutable de minishell NO ES EJECUTABLE en: $MINISHELL_EXECUTABLE\033[0m"
    echo -e "\033[0;31mVerifica los permisos.\033[0m"
    exit 1
fi


# Contadores de pruebas
total_tests=0
passed_tests=0

# Colores
GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[0;33m"
BLUE="\033[0;34m"
NC="\033[0m" # No Color

# --- Función de Ayuda para Ejecutar Pruebas ---
run_test() {
    local description="$1"
    local command_str="$2"
    local setup_cmd="$3"
    local cleanup_cmd="$4"
    local minishell_out minishell_raw_out minishell_exit
    local ref_out ref_raw_out ref_exit
    local original_dir
    local script_home # Directorio donde se encuentra el script de prueba

    script_home=$(dirname "$(realpath "$0")")
    ((total_tests++))
    echo -e "${BLUE}-----------------------------------------------------${NC}"
    echo -e "${BLUE}PRUEBA:${NC} $description"
    echo -e "${BLUE}Comando:${NC} $command_str"

    original_dir=$(pwd 2>/dev/null)

    # Ejecutar comando de preparación si existe
    if [ -n "$setup_cmd" ]; then
        (eval "$setup_cmd") > /dev/null 2>&1 # Ejecutar en subshell para no afectar el CWD del script principal aquí
        local setup_exit_code=$?
        if [ $setup_exit_code -ne 0 ]; then
            echo -e "${RED}ADVERTENCIA: El SETUP del test (comando: '$setup_cmd') falló o tuvo salida de error (código: $setup_exit_code). Continuando, pero el test podría no ser válido.${NC}"
        fi
    fi
    
    # Guardar CWD actual antes de ejecutar los comandos que podrían cambiarlo
    # Esto es para los comandos que se ejecutan DENTRO de minishell/bash
    # Si el setup_cmd cambia el directorio del script, necesitamos que minishell se ejecute desde ese contexto
    # pero que el comando interno (ej. cd dir1) sea relativo a ese contexto.

    local effective_run_dir="$original_dir"
    if [ -n "$setup_cmd" ]; then
      # Si hay setup_cmd, asumimos que establece el directorio correcto para la ejecución del comando
      # y que el comando_str está diseñado para ejecutarse desde ese directorio.
      # El script en sí no cambia de directorio aquí, minishell y bash se lanzarán desde original_dir
      # pero los comandos internos como 'cd dir1' en el command_str deben funcionar relativos al
      # directorio que el setup_cmd hubiera creado o al que hubiera navegado.
      # Para pruebas de 'cd', el setup_cmd prepara el terreno, y el command_str hace el 'cd'
      # que queremos probar DENTRO del shell.
      # Si el setup_cmd es "cd /alguna/ruta", el script principal sigue en original_dir.
      # La llamada a minishell/bash es desde original_dir, pero el comando interno "cd dir1"
      # se interpretará por minishell/bash en su propio CWD, que al inicio es original_dir.
      # Esto significa que para "cd dir1 && pwd", "dir1" debe existir en original_dir o ser una ruta absoluta.
      #
      # Corrección: Para que los comandos como "cd dir1" funcionen correctamente después de un setup_cmd
      # que crea "dir1" en una ubicación específica, el minishell/bash DEBE ejecutarse desde esa ubicación.
      if [[ "$setup_cmd" == cd* ]]; then
          # Extraer la ruta del comando cd
          eval "target_setup_dir=${setup_cmd#cd }"
          # Expandir la ruta (manejar ~, variables, etc.)
          target_setup_dir=$(eval echo "$target_setup_dir")
          if [ -d "$target_setup_dir" ]; then
              effective_run_dir="$target_setup_dir"
          fi
      fi
    fi

    # Ejecutar en tu minishell
    minishell_raw_out=$(cd "$effective_run_dir" && printf "%s\1" "$("$MINISHELL_EXECUTABLE" -c "$command_str" 2>&1)")
    minishell_exit=$?

    # Ejecutar en el shell de referencia (bash)
    ref_raw_out=$(cd "$effective_run_dir" && printf "%s\1" "$($BASH_EXECUTABLE -c "$command_str" 2>&1)")
    ref_exit=$?

    # Ejecutar comando de limpieza si existe
    if [ -n "$cleanup_cmd" ]; then
        (eval "$cleanup_cmd") > /dev/null 2>&1 # Ejecutar en subshell
    fi
    
    # Volver al directorio original del script, no al que estaba antes de este test específico.
    if [ -n "$script_home" ] && [ -d "$script_home" ]; then
        cd "$script_home"
    fi

    minishell_out_clean=${minishell_raw_out%?} 
    ref_out_clean=${ref_raw_out%?}

    if [ "$minishell_out_clean" == "$ref_out_clean" ] && [ "$minishell_exit" == "$ref_exit" ]; then
        echo -e "${GREEN}PASSED${NC}"
        ((passed_tests++))
    else
        echo -e "${RED}FAILED${NC}"
        echo -e "${YELLOW}Salida de tu Minishell (código: $minishell_exit):${NC}"
        echo -n "'$minishell_out_clean'"
        echo "|END_MS|" # Marcador para ver newlines/espacios finales
        echo -e "${YELLOW}Salida Esperada (bash) (código: $ref_exit):${NC}"
        echo -n "'$ref_out_clean'"
        echo "|END_BASH|" # Marcador
    fi
}

# --- Pruebas para ECHO ---
echo -e "\n${YELLOW}========== INICIANDO PRUEBAS PARA ECHO ==========${NC}"

run_test "Echo simple" "echo hola"
run_test "Echo con múltiples palabras" "echo hola mundo cruel"
run_test "Echo con opción -n" "echo -n hola"
run_test "Echo con opción -n y múltiples palabras" "echo -n hola mundo"
run_test "Echo con múltiples opciones -n (bash: \"-n hola\")" "echo -n -n hola" 
run_test "Echo con opción -nnn" "echo -nnn hola mundo"
run_test "Echo sin argumentos" "echo"
run_test "Echo solo con -n" "echo -n"
run_test "Echo con -n en medio (no como opción)" "echo hola -n mundo"
run_test "Echo con guion solo" "echo -"
run_test "Echo con doble guion (argumento literal)" "echo --" 
run_test "Echo con tilde (expansión de HOME por el shell)" "echo ~"
run_test "Echo con var de entorno (comillas dobles, expansión por el shell)" 'echo "$USER"'
run_test "Echo con var de entorno (comillas simples, literal)" "echo '\$USER'" # Corregido para que $ sea literal
run_test "Echo con múltiples espacios preservados por comillas" 'echo "hola       mundo"'
run_test "Echo con cadena vacía entre comillas" 'echo ""'
run_test "Echo con -n y cadena vacía entre comillas" 'echo -n ""'
run_test "Echo con solo espacios entre comillas" 'echo "   "'
run_test "Echo con -n y solo espacios entre comillas" 'echo -n "   "'
run_test "Echo con caracteres especiales (bash escapa \\ ante \$)" 'echo "hola*mundo!@#\\$%^&*()_+"'
run_test "Echo -n con caracteres especiales (bash escapa \\ ante \$)" 'echo -n "hola*mundo!@#\\$%^&*()_+"'
run_test "Echo -n sin espacio después (Bash lo trata como literal)" "echo -nhola" 
run_test "Echo -n -nn -n hola (Bash: \"-nn -n hola\")" "echo -n -nn -n hola"

# --- Pruebas para PWD ---
echo -e "\n${YELLOW}========== INICIANDO PRUEBAS PARA PWD ==========${NC}"

# Guardar el directorio de inicio del script para volver a él
script_initial_dir=$(pwd 2>/dev/null)
if [ -z "$script_initial_dir" ]; then
    echo -e "${RED}ERROR CRÍTICO: No se pudo obtener el directorio actual al inicio de las pruebas de PWD.${NC}"
    exit 1
fi

# Crear directorios temporales para pruebas de cd/pwd
temp_dir_base=$(mktemp -d "$script_initial_dir/_test_minishell_pwd_XXXXXX")
if [ -z "$temp_dir_base" ] || [ ! -d "$temp_dir_base" ]; then
    echo -e "${RED}ERROR CRÍTICO: No se pudo crear el directorio temporal base para las pruebas de pwd.${NC}"
    exit 1
fi

temp_dir1="$temp_dir_base/dir1"
temp_dir2="$temp_dir_base/dir1/subdir2"

mkdir -p "$temp_dir2"
if [ ! -d "$temp_dir2" ]; then
    echo -e "${RED}ERROR CRÍTICO: No se pudieron crear subdirectorios temporales en $temp_dir_base.${NC}"
    rm -rf "$temp_dir_base"
    exit 1
fi

run_test "Pwd en directorio actual (directorio del script)" "pwd" "" "cd \"$script_initial_dir\""

# Para estas pruebas, el comando se ejecuta *desde* el directorio especificado por el setup_cmd
run_test "Pwd después de cd a un subdirectorio" "pwd" "cd \"$temp_dir1\"" "cd \"$script_initial_dir\""
run_test "Pwd después de cd a un sub-subdirectorio" "pwd" "cd \"$temp_dir2\"" "cd \"$script_initial_dir\""
run_test "Pwd después de cd .. desde sub-subdirectorio" "cd .. && pwd" "cd \"$temp_dir2\"" "cd \"$script_initial_dir\"" # Prueba el 'cd ..' interno
run_test "Pwd después de cd a ruta absoluta (/tmp)" "pwd" "cd /tmp" "cd \"$script_initial_dir\""

# Prueba para cd -
# Bash necesita un OLDPWD seteado. Minishell también.
# 1. Estar en script_initial_dir.
# 2. (Setup) cd a temp_dir1. (OLDPWD para el script ahora es script_initial_dir)
# 3. El comando para minishell/bash será: "cd /tmp && cd -"
#    - minishell/bash (desde temp_dir1) hace 'cd /tmp'. Su OLDPWD interno se vuelve temp_dir1.
#    - luego hace 'cd -'. Debería volver a temp_dir1.
#    - luego 'pwd'. Debería mostrar la ruta de temp_dir1.
run_test "Pwd después de cd -" \
  "cd /tmp && cd - && pwd" \
  "cd \"$temp_dir1\"" \
  "cd \"$script_initial_dir\""


# Prueba para verificar si PWD se actualiza en el entorno de minishell
echo -e "${BLUE}-----------------------------------------------------${NC}"
echo -e "${BLUE}PRUEBA:${NC} Pwd y echo \$PWD coinciden después de cd (desde dir temporal)"
echo -e "${BLUE}Comando minishell:${NC} cd dir1 && pwd && echo \$PWD"
((total_tests++))

# El comando para minishell/bash se ejecutará desde $temp_dir_base
# 'cd dir1' es relativo a $temp_dir_base
minishell_multi_out=$(cd "$temp_dir_base" && "$MINISHELL_EXECUTABLE" -c "cd dir1 && pwd && echo \$PWD" 2>&1)
minishell_multi_exit=$?

ref_multi_out=$(cd "$temp_dir_base" && $BASH_EXECUTABLE -c "cd dir1 && pwd && echo \$PWD" 2>&1)
ref_multi_exit=$?

cd "$script_initial_dir" # Volver al directorio original del script

pwd_output_ms=$(echo "$minishell_multi_out" | head -n 1)
echo_pwd_output_ms=$(echo "$minishell_multi_out" | tail -n 1)
pwd_output_bash=$(echo "$ref_multi_out" | head -n 1)

if [ "$pwd_output_ms" == "$echo_pwd_output_ms" ] && \
   [ "$pwd_output_ms" == "$pwd_output_bash" ] && \
   [ "$minishell_multi_exit" == "$ref_multi_exit" ]; then
    echo -e "${GREEN}PASSED${NC}"
    ((passed_tests++))
else
    echo -e "${RED}FAILED${NC}"
    echo -e "${YELLOW}Salida 'pwd' (Minishell):${NC} '$pwd_output_ms'"
    echo -e "${YELLOW}Salida 'echo \$PWD' (Minishell):${NC} '$echo_pwd_output_ms'"
    echo -e "${YELLOW}Salida Esperada para pwd (Bash):${NC} '$pwd_output_bash'"
    echo -e "${YELLOW}Código Salida (Minishell):${NC} $minishell_multi_exit, (Bash): $ref_multi_exit"
fi

# Limpieza de directorios temporales
rm -rf "$temp_dir_base"


# --- Resumen Final ---
echo -e "\n${YELLOW}========== RESUMEN DE PRUEBAS ==========${NC}"
echo -e "Pruebas Totales: $total_tests"
echo -e "${GREEN}Pasadas: $passed_tests${NC}"
if [ $((total_tests - passed_tests)) -gt 0 ]; then
    echo -e "${RED}Fallidas: $((total_tests - passed_tests))${NC}"
    exit 1
else
    echo -e "${GREEN}Todas las pruebas pasaron.${NC}"
    exit 0
fi
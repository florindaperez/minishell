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
# Argumentos:
# $1: Descripción de la prueba
# $2: Comando(s) a ejecutar en los shells
# $3: (Opcional) Código de salida esperado específico para minishell (si difiere de bash o para forzar)
run_test() {
    local description="$1"
    local command_str="$2"
    local expected_ms_exit_override="$3" 
    local minishell_out minishell_raw_out minishell_exit
    local ref_out ref_raw_out ref_exit

    ((total_tests++))
    echo -e "${BLUE}-----------------------------------------------------${NC}"
    echo -e "${BLUE}PRUEBA:${NC} $description"
    echo -e "${BLUE}Comando(s):${NC} $command_str"
    
    minishell_raw_out=$(printf "%s\1" "$("$MINISHELL_EXECUTABLE" -c "$command_str" 2>&1)")
    minishell_exit=$?

    ref_raw_out=$(printf "%s\1" "$($BASH_EXECUTABLE -c "$command_str" 2>&1)")
    ref_exit=$?

    minishell_out_clean=${minishell_raw_out%?} 
    ref_out_clean=${ref_raw_out%?}

    local current_test_passed=true
    local reason_for_failure="" 
    local note_for_passing=""   

    local target_ms_exit="$ref_exit"
    if [ -n "$expected_ms_exit_override" ]; then
        target_ms_exit="$expected_ms_exit_override"
    fi

    local is_env_or_export_no_args=false
    if [[ "$description" == "export sin argumentos (formato declare -x)" || "$description" == "env simple" ]]; then
        is_env_or_export_no_args=true
    fi

    # Comprobar código de salida primero
    if [ "$minishell_exit" -ne "$target_ms_exit" ]; then
        current_test_passed=false
        reason_for_failure+="Código de salida no coincide (MS: $minishell_exit, Esperado para MS: $target_ms_exit, Bash dio: $ref_exit). "
    fi

    # Comprobar salida de texto
    if [ "$minishell_out_clean" != "$ref_out_clean" ]; then
        if $is_env_or_export_no_args; then
            # Para env/export sin args, si el código de salida fue bueno, es una NOTA.
            # Si el código de salida ya falló, se añade como otra razón del fallo.
            if $current_test_passed; then # Solo es una nota si el código de salida estaba bien
                 note_for_passing+="NOTA: Salida de texto diferente (posiblemente orden/variables dinámicas como '_', 'SHLVL'). Verificar manualmente contenido esencial. "
            else
                 reason_for_failure+="Salida de texto también diferente. "
            fi
        else
            # Para otras pruebas, la diferencia de texto es un fallo.
            current_test_passed=false
            reason_for_failure+="Salida de texto no coincide. "
        fi
    fi

    if $current_test_passed; then
        echo -e "${GREEN}PASSED${NC}"
        if [[ -n "$note_for_passing" ]]; then
             echo -e "  ${YELLOW}${note_for_passing}${NC}"
        fi
        ((passed_tests++))
    else
        echo -e "${RED}FAILED:${NC} ${reason_for_failure}"
        # Mostrar siempre los detalles si la prueba falló por alguna razón
        echo -e "  ${YELLOW}Salida MS (código: $minishell_exit):${NC}"
        echo -n "'$minishell_out_clean'"
        echo "|END_MS|"
        echo -e "  ${YELLOW}Salida BASH (código de ref: $ref_exit):${NC}"
        echo -n "'$ref_out_clean'"
        echo "|END_BASH|"
        if [ "$target_ms_exit" -ne "$ref_exit" ] && [ "$minishell_exit" -ne "$target_ms_exit" ]; then
             echo -e "  ${YELLOW}Detalle Código Salida Esperado para MS:${NC} $target_ms_exit"
        fi
    fi
}

# --- Pruebas para ENV ---
echo -e "\n${YELLOW}========== PRUEBAS ENV ==========${NC}"
run_test "env simple" "env"
run_test "env con argumento (error y $?=127)" "env arg_test" 127
run_test "env con múltiples argumentos (error y $?=127)" "env arg1 arg2" 127

# --- Pruebas para EXPORT ---
echo -e "\n${YELLOW}========== PRUEBAS EXPORT ==========${NC}"
run_test "export sin argumentos (formato declare -x)" "export"
run_test "export VAR (sin valor, luego env)" "export TEST_NOVAL; env"
run_test "export VAR= (valor vacío, luego env)" "export TEST_EMPTYVAL=; env"
run_test "export VAR=valor (simple, luego env)" "export TEST_SIMPLE=hello; env"
run_test "export VAR=\"valor con espacios\" (luego env)" "export TEST_SPACES=\"hello world\"; env"
run_test "export VAR_EXISTENTE=nuevo (actualizar, luego env)" "export TEST_UPDATE=old; export TEST_UPDATE=new; env"
run_test "export _VAR=valido" "export _UNDERSCORE=valid; env"
run_test "export VAR123=valido" "export NUM123VAR=valid; env"
run_test "export VAR1=val1 VAR2=val2" "export MULTI_A=1 MULTI_B=2; env"

echo -e "${BLUE}--- Pruebas de export con identificadores inválidos ---${NC}"
run_test "export 1VAR=invalido (solo export, $?=1)" "export 1BADVAR=test" 1
run_test "export VAR-MAL=invalido (solo export, $?=1)" "export BAD-VAR=test" 1
run_test "export \"\"=invalido (solo export, $?=1)" "export \"\"=test" 1 
run_test "export =valor (solo export, $?=1)" "export =novalue" 1 
run_test "export VAR=val INVALID-KEY VAR2=val2 (mezcla, luego env)" \
         "export EXP_MIX_VALID=ok INVALID-KEY EXP_MIX_VALID2=ok2; env" 0 

# --- Pruebas para UNSET ---
echo -e "\n${YELLOW}========== PRUEBAS UNSET ==========${NC}"
run_test "unset sin argumentos ($?=0)" "unset" 0
run_test "unset VAR_EXISTENTE" "export TO_UNSET=exists; unset TO_UNSET; env"
run_test "unset VAR_INEXISTENTE ($?=0)" "unset NON_EXISTENT_VAR; env" 0
run_test "unset VAR1 VAR2" "export UNSET_MULTI1=a UNSET_MULTI2=b; unset UNSET_MULTI1 UNSET_MULTI2; env"
run_test "unset VAR_EXISTENTE VAR_INEXISTENTE" "export UNSET_MIX_EXIST=yes; unset UNSET_MIX_EXIST NON_EXISTENT_AGAIN; env"

echo -e "${BLUE}--- Pruebas de unset con identificadores inválidos ---${NC}"
run_test "unset 1INVALIDO (solo unset, $?=1)" "unset 1BAD_UNSET_VAR" 1
run_test "unset VAR-MAL (solo unset, $?=1)" "unset VAR-BAD" 1
run_test "unset VAR= (solo unset, $?=1)" "unset VAR=" 1 
run_test "unset VALIDA INVALIDA-UNSET (mezcla, luego env)" \
         "export UNSET_VALID_ARG=ok; unset UNSET_VALID_ARG INVALIDA-UNSET; env" \
         0 

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
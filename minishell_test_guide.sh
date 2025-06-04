#!/bin/bash

# --- Configuración ---
MINISHELL_EXE="./minishell" # Ajusta esto si tu ejecutable está en otro lugar o tiene otro nombre
TEST_DIR="minishell_test_workspace"

# Colores
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# --- Funciones Auxiliares ---
print_section_header() {
    echo -e "\n${BLUE}=======================================================================${NC}"
    echo -e "${BLUE}>>> SECCIÓN: $1${NC}"
    echo -e "${BLUE}=======================================================================${NC}"
}

print_test_header() {
    echo -e "\n${YELLOW}--- PRUEBA: $1 ---${NC}"
}

ask_manual_check() {
    echo -e "${GREEN}Por favor, ejecuta los comandos indicados en tu minishell y en bash.${NC}"
    echo -e "${GREEN}Compara stdout, stderr y el código de retorno (\`echo \$?\`).${NC}"
    read -p "Presiona Enter para continuar a la siguiente prueba..."
}

# --- Preparación del Entorno de Prueba ---
setup_test_environment() {
    echo "Configurando entorno de prueba en ./${TEST_DIR}..."
    rm -rf "${TEST_DIR}"
    mkdir -p "${TEST_DIR}"
    cd "${TEST_DIR}" || exit 1

    echo "Contenido de infile para pruebas de redirección." > infile.txt
    echo "Línea original en appendfile." > appendfile.txt
    mkdir -p testdir/subdir
    touch empty_file.txt
    echo "Ejecutable de prueba" > script_executable.sh
    chmod +x script_executable.sh
    echo "Archivo no ejecutable" > script_no_execute.sh
    chmod -x script_no_execute.sh # Quitar permisos de ejecución
    mkdir perm_denied_dir
    chmod 000 perm_denied_dir # Directorio sin permisos

    echo "Entorno listo."
    cd .. || exit 1
}

# --- Limpieza del Entorno de Prueba ---
cleanup_test_environment() {
    echo -e "\nLimpiando entorno de prueba..."
    rm -rf "${TEST_DIR}"
    echo "Limpieza completa."
}

# Registrar la limpieza para que se ejecute al salir
trap cleanup_test_environment EXIT

# --- Inicio de las Pruebas ---
setup_test_environment

echo -e "${BLUE}INICIO DE LA GUÍA DE PRUEBAS DE MINISHELL (Parte Obligatoria)${NC}"
echo "Recuerda ejecutar cada bloque de comandos en tu ${MINISHELL_EXE} y en bash para comparar."
echo "Verifica stdout, stderr y el código de retorno con 'echo \$?'."

# --- A. Generalidades y Prompt ---
print_section_header "A. Generalidades y Prompt"

print_test_header "A.1 Mostrar un prompt"
echo "  Acción: Inicia tu minishell."
echo "  Esperado: Debe aparecer un prompt (ej. minishell>)."
ask_manual_check

print_test_header "A.2 No crashear y manejo básico de línea vacía"
echo "  Comandos de Ejemplo (ejecutar en minishell y bash):"
echo "    (presionar Enter sin escribir nada)"
echo "    (escribir solo espacios y presionar Enter)"
echo "  Esperado: Nuevo prompt, sin errores, \$? = 0."
ask_manual_check

# --- B. Parsing y Ejecución Simple de Comandos ---
print_section_header "B. Parsing y Ejecución Simple"

print_test_header "B.1 Ejecución simple con ruta absoluta"
echo "  Comandos de Ejemplo:"
echo "    /bin/ls"
echo "    /bin/pwd"
echo "    /bin/echo hello world"
echo "  Esperado: Ejecución normal, \$? el del comando (0 si éxito)."
ask_manual_check

print_test_header "B.2 Ejecución simple con búsqueda en PATH"
echo "  Comandos de Ejemplo:"
echo "    ls"
echo "    pwd"
echo "    echo hello world"
echo "  Esperado: Igual que con ruta absoluta si el comando se encuentra."
ask_manual_check

print_test_header "B.3 Comando no encontrado"
echo "  Comandos de Ejemplo:"
echo "    comandoquenoexiste123abc"
echo "    ./comandoquenoexisteeneldirectorioactual"
echo "  Esperado: Mensaje de error en stderr (ej. 'command not found'). \$? = 127."
ask_manual_check

print_test_header "B.4 Rutas relativas"
echo "  Comandos de Ejemplo (desde el directorio ${TEST_DIR}):"
echo "    ./script_executable.sh"
echo "    script_no_execute.sh # Sin ./ (si no está en PATH)"
echo "  Esperado para ./script_executable.sh: 'Ejecutable de prueba', \$? = 0."
echo "  Esperado para script_no_execute.sh: Error 'command not found', \$? = 127."
ask_manual_check

print_test_header "B.5 Manejo de comillas simples ('')"
echo "  Comandos de Ejemplo:"
echo "    echo 'hello world'"
echo "    echo 'hello   world'"
echo "    echo '\$USER'"
echo "    echo '\"\$USER\"'"
echo "    echo '< > | ;'"
echo "  Esperado: Salida literal de lo que está entre comillas."
ask_manual_check

print_test_header "B.6 Manejo de comillas dobles (\")"
echo "  Comandos de Ejemplo:"
echo "    export TESTVAR=\"mundo\""
echo "    echo \"hello \$USER\""
echo "    echo \"hello \$TESTVAR\""
echo "    (ls / > /dev/null ; echo \"exit status: \$?\") # Para $? predecible"
echo "    echo \"comillas 'simples' dentro\""
echo "    echo \"< > | ;\""
echo "    echo \"un\\\$escaped\""
echo "  Esperado: Expansión de variables, otros caracteres literales. Para \"un\\\$escaped\", la salida debe ser 'un\$escaped'."
ask_manual_check

print_test_header "B.7 Comillas no cerradas"
echo "  Comandos de Ejemplo:"
echo "    echo 'hola"
echo "    echo \"mundo"
echo "  Esperado: Error de sintaxis (ej. 'unexpected EOF'). Compara con 'bash -c \"echo 'hola\"'."
ask_manual_check

print_test_header "B.8 Expansión de variables de entorno (\$VAR)"
echo "  Comandos de Ejemplo:"
echo "    export MYVAR=\"Soy Minishell\""
echo "    echo \$MYVAR"
echo "    echo \"\$MYVAR\""
echo "    echo '\$MYVAR'"
echo "    echo \"Texto \$MYVAR texto\""
echo "    echo \$INEXISTENT_VAR"
echo "    echo \"Texto \$INEXISTENT_VAR texto\""
echo "  Esperado: Sustitución correcta. Variable inexistente se expande a vacío. Presta atención a los espacios."
ask_manual_check

print_test_header "B.9 Expansión de \$?"
echo "  Comandos de Ejemplo:"
echo "    ls /unexisting_path_for_sure ; echo \$?"
echo "    echo \$? # Después del echo anterior"
echo "    \"\" # Comando comillas vacías"
echo "    echo \$? # Después de \"\" "
echo "  Esperado: Código de retorno del último comando. Para \"\", bash da 'command not found', \$?=127."
ask_manual_check

# --- C. Builtins ---
print_section_header "C. Builtins"

print_test_header "C.1 Builtin: echo"
echo "  Comandos de Ejemplo:"
echo "    echo Hello World"
echo "    echo -n Hello World"
echo "    echo -n-n -n Hello"
echo "    echo -n Hello -n World"
echo "    echo Hello -n World"
echo "    echo"
echo "    echo -n"
echo "  Esperado: Comportamiento idéntico a bash echo."
ask_manual_check

print_test_header "C.2 Builtin: cd"
echo "  Comandos de Ejemplo (ejecutar desde la raíz del proyecto donde está ${TEST_DIR}):"
echo "    pwd"
echo "    cd ${TEST_DIR}/testdir"
echo "    pwd"
echo "    cd subdir"
echo "    pwd"
echo "    cd .."
echo "    pwd"
echo "    cd # Sin argumentos (a \$HOME)"
echo "    pwd"
echo "    cd /tmp"
echo "    pwd"
echo "    cd \$HOME/${TEST_DIR}/testdir # Con variable (asegúrate que \$HOME/${TEST_DIR} exista o adapta)"
echo "    pwd"
echo "    cd -"
echo "    pwd"
echo "    cd /path/inexistente"
echo "    echo \$?"
echo "    cd ${TEST_DIR}/empty_file.txt # Intentar cd a un archivo"
echo "    echo \$?"
echo "    cd arg1 arg2"
echo "    echo \$?"
echo "  Esperado: Cambios de directorio, errores a stderr, PWD/OLDPWD actualizados. \$?=0 éxito, \$?=1 fallo."
ask_manual_check

print_test_header "C.3 Builtin: pwd"
echo "  Comandos de Ejemplo (ejecutar desde la raíz del proyecto donde está ${TEST_DIR}):"
echo "    pwd"
echo "    cd ${TEST_DIR}/testdir; pwd; cd ../.. "
echo "  Esperado: Ruta absoluta del directorio actual. \$?=0."
ask_manual_check

print_test_header "C.4 Builtin: export"
echo "  Comandos de Ejemplo:"
echo "    export"
echo "    export MYVAR1=valor1"
echo "    env | grep MYVAR1"
echo "    export MYVAR2=\"valor con espacios\""
echo "    env | grep MYVAR2"
echo "    export MYVAR1=nuevovalor"
echo "    env | grep MYVAR1"
echo "    export VAR_SIN_VALOR"
echo "    env | grep VAR_SIN_VALOR"
echo "    export VAR_CON_IGUAL="
echo "    env | grep VAR_CON_IGUAL"
echo "    export 1VAR=error"
echo "    echo \$?"
echo "    export VAR-CON-GUION=error"
echo "    echo \$?"
echo "    export MI_VAR=\"hola\"\" \"\"mundo\""
echo "    env | grep MI_VAR"
echo "  Esperado: 'export' sin args muestra variables (formato 'declare -x KEY=\"value\"'). Asignaciones actualizan env. Nombres inválidos dan error, \$?=1."
ask_manual_check

print_test_header "C.5 Builtin: unset"
echo "  Comandos de Ejemplo:"
echo "    export TESTUNSET1=abc TESTUNSET2=def"
echo "    env | grep TESTUNSET"
echo "    unset TESTUNSET1"
echo "    env | grep TESTUNSET1 # No debe aparecer"
echo "    unset TESTUNSET2 TESTUNSET_INEXISTENTE"
echo "    env | grep TESTUNSET2 # No debe aparecer"
echo "    unset 1INVALID"
echo "    echo \$?"
echo "    unset VAR-INVALID"
echo "    echo \$?"
echo "  Esperado: Elimina variables. Nombres inválidos dan error, \$?=1. Unset de inexistente no es error."
ask_manual_check

print_test_header "C.6 Builtin: env"
echo "  Comandos de Ejemplo:"
echo "    env"
echo "    export NUEVA_PARA_ENV=testenv"
echo "    env"
echo "    env arg_error"
echo "    echo \$?"
echo "  Esperado: Muestra variables (NOMBRE=valor). 'env arg_error' en minishell 42 suele ser un error, \$?=127 o similar."
ask_manual_check

print_test_header "C.7 Builtin: exit"
echo "  ¡PRECAUCIÓN! Estos comandos terminarán tu shell. Pruébalos individualmente."
echo "  Comandos de Ejemplo:"
echo "    exit                # (después de un comando exitoso, \$? debería ser 0)"
echo "    ls /nonexistent; exit # (sale con \$? de ls)"
echo "    exit 42"
echo "    exit 256            # (bash sale con 0)"
echo "    exit -10            # (bash sale con 246)"
echo "    exit hola           # (bash: error numérico, \$?=2)"
echo "    exit 10 20          # (bash: too many arguments, \$?=1)"
echo "  Esperado: El shell termina. 'exit' se imprime en stderr si es interactivo. Códigos de salida como en bash."
ask_manual_check

# --- D. Redirecciones ---
print_section_header "D. Redirecciones (archivos creados/usados en ${TEST_DIR})"

print_test_header "D.1 Redirección de entrada (<)"
echo "  Comandos de Ejemplo (en ${TEST_DIR}):"
echo "    cat < infile.txt"
echo "    wc -l < infile.txt"
echo "    < infile.txt cat"
echo "    <infile.txt cat"
echo "    < non_existent_file.txt cat"
echo "    echo \$?"
echo "  Esperado: Comandos leen de infile.txt. Error si no existe, \$?=1."
ask_manual_check

print_test_header "D.2 Redirección de salida (>)"
echo "  Comandos de Ejemplo (en ${TEST_DIR}):"
echo "    ls > outfile.txt"
echo "    cat outfile.txt"
echo "    echo \"nuevo contenido\" > outfile.txt"
echo "    cat outfile.txt"
echo "    > outfile_vacio.txt"
echo "    cat outfile_vacio.txt"
echo "    echo hola > perm_denied_dir/outfile.txt # Error de permiso"
echo "    echo \$?"
echo "  Esperado: Archivos creados/sobrescritos. Errores a stderr, \$?=1."
ask_manual_check

print_test_header "D.3 Redirección de salida con append (>>)"
echo "  Comandos de Ejemplo (en ${TEST_DIR}):"
echo "    echo \"linea A\" >> appendfile.txt"
echo "    cat appendfile.txt"
echo "    echo \"linea B\" >> appendfile.txt"
echo "    cat appendfile.txt"
echo "    echo hola >> perm_denied_dir/appendfile.txt # Error de permiso"
echo "    echo \$?"
echo "  Esperado: Salida añadida al archivo. Errores a stderr, \$?=1."
ask_manual_check

print_test_header "D.4 Here Document (<<)"
echo "  Comandos de Ejemplo:"
echo "    cat << EOF"
echo "    linea 1 de heredoc"
echo "    linea 2 con \$USER (se expande)"
echo "    EOF"
echo ""
echo "    cat << \"EOF\""
echo "    linea 1 de heredoc literal"
echo "    linea 2 con \$USER (no se expande)"
echo "    \"EOF\""
echo ""
echo "    wc -c <<EOF"
echo "    test"
echo "    EOF"
echo "  Esperado: Entrada leída hasta delimitador. Expansión si delimitador no está entrecomillado."
ask_manual_check

print_test_header "D.5 Combinación y Orden de Redirecciones"
echo "  Comandos de Ejemplo (en ${TEST_DIR}):"
echo "    echo \"in_redir\" > infile_redir.txt"
echo "    < infile_redir.txt cat > outfile_redir.txt"
echo "    cat outfile_redir.txt"
echo ""
echo "    echo \"out1\" > out1.txt"
echo "    echo \"out2\" > out2.txt"
echo "    echo hola > out1.txt > out2.txt # out2.txt debe tener 'hola', out1.txt es intermedio"
echo "    echo \"Contenido de out1.txt:\" ; cat out1.txt"
echo "    echo \"Contenido de out2.txt:\" ; cat out2.txt"
echo ""
echo "    echo \"in1\" > in1.txt; echo \"in2\" > in2.txt"
echo "    < in1.txt < in2.txt cat # cat debe usar in2.txt"
echo "  Esperado: La última redirección para un FD específico prevalece."
ask_manual_check

print_test_header "D.6 Errores de redirección (ambigüedad, permisos)"
echo "  Comandos de Ejemplo (en ${TEST_DIR}):"
echo "    export AMBIGUO_EMPTY=\"\""
echo "    export AMBIGUO_SPACES=\"uno dos\""
echo "    echo test > \$AMBIGUO_EMPTY"
echo "    echo \$?"
echo "    echo test > \$AMBIGUO_SPACES"
echo "    echo \$?"
echo "    cat < \$INEXISTENTE_PARA_REDIR"
echo "    echo \$?"
echo "    echo \"hola\" > / # Es un directorio (o error de permiso)"
echo "    echo \$?"
echo "    echo \"test\" > perm_denied_dir/file"
echo "    echo \$?"
echo "  Esperado: Mensajes de error apropiados (ej. 'ambiguous redirect'), \$?=1."
ask_manual_check

# --- E. Pipes (|) ---
print_section_header "E. Pipes (|)"

print_test_header "E.1 Pipe simple"
echo "  Comandos de Ejemplo (infile.txt debe existir en ${TEST_DIR}):"
echo "    ls -l | wc -l"
echo "    echo \"hola mundo prueba\" | grep mundo"
echo "    cat ${TEST_DIR}/infile.txt | sort | uniq"
echo "  Esperado: Salida de cmd1 a entrada de cmd2. \$? es el del último comando."
ask_manual_check

print_test_header "E.2 Múltiples pipes"
echo "  Comandos de Ejemplo:"
echo "    cat /etc/passwd | grep /bin/bash | cut -d: -f1 | sort"
echo "  Esperado: Salida correcta a través de la pipeline. \$? es el del último comando."
ask_manual_check

print_test_header "E.3 Pipes con builtins"
echo "  Comandos de Ejemplo:"
echo "    env | sort | grep USER"
echo "    echo -e \"linea1\\nlinea2\\nlinea1\" | pwd | cat # pwd no usa entrada pero debe funcionar"
echo "    export PPVAR=pipetest; echo \$PPVAR | unset PPVAR; env | grep PPVAR # PPVAR no debe estar"
echo "  Esperado: Builtins funcionan en pipelines. Cambios de env en subshells no persisten."
ask_manual_check

print_test_header "E.4 Errores en pipes"
echo "  Comandos de Ejemplo:"
echo "    ls | comando_que_no_existe_pipe | wc -c"
echo "    echo \$? # Debería ser 127 (de comando_que_no_existe_pipe)"
echo "    comando_que_no_existe_pipe2 | wc -c"
echo "    echo \$? # Debería ser 127"
echo "  Esperado: Error reportado. \$? es el del comando fallido."
ask_manual_check

# --- F. Señales ---
print_section_header "F. Señales (PRUEBAS MANUALES INTERACTIVAS)"

print_test_header "F.1 Ctrl-C (SIGINT)"
echo "  Prueba 1: En el prompt vacío."
echo "    Acción: Inicia ${MINISHELL_EXE}, presiona Ctrl-C."
echo "    Esperado: Nuevo prompt, shell no termina, \$? = 130."
echo "  Prueba 2: Durante un comando largo (ej. 'sleep 10' o 'cat' sin input)."
echo "    Acción: Ejecuta 'sleep 10', presiona Ctrl-C mientras corre."
echo "    Esperado: Comando termina, nuevo prompt, shell no termina, \$? = 130."
echo "  Prueba 3: Durante un heredoc."
echo "    Acción: Ejecuta 'cat << EOF', escribe algo, presiona Ctrl-C."
echo "    Esperado: Heredoc se interrumpe, nuevo prompt, shell no termina, \$? = 130."
ask_manual_check

print_test_header "F.2 Ctrl-D (EOF)"
echo "  Prueba 1: En prompt con línea vacía."
echo "    Acción: Inicia ${MINISHELL_EXE}, presiona Ctrl-D."
echo "    Esperado: Shell termina (a menudo imprime 'exit')."
echo "  Prueba 2: En prompt con texto en la línea."
echo "    Acción: Inicia ${MINISHELL_EXE}, escribe 'echo hola', presiona Ctrl-D."
echo "    Esperado: No hace nada o procesa la línea. Shell no termina."
ask_manual_check

print_test_header "F.3 Ctrl-\ (SIGQUIT)"
echo "  Prueba 1: En el prompt vacío."
echo "    Acción: Inicia ${MINISHELL_EXE}, presiona Ctrl-\."
echo "    Esperado: No hace nada. Shell no termina."
echo "  Prueba 2: Durante un comando largo (ej. 'sleep 10' o 'cat')."
echo "    Acción: Ejecuta 'sleep 10', presiona Ctrl-\ mientras corre."
echo "    Esperado: Comando termina (bash imprime 'Quit (core dumped)'), shell no termina, \$? = 131."
ask_manual_check

# --- G. Errores y Casos Límite ---
print_section_header "G. Errores y Casos Límite"

print_test_header "G.1 Comandos vacíos o solo espacios"
echo "  Comandos de Ejemplo:"
echo "    (Presionar Enter)"
echo "      (Solo espacios y Enter)"
echo "  Esperado: Nuevo prompt, \$? = 0."
ask_manual_check

print_test_header "G.2 Errores de sintaxis (Parser)"
echo "  Comandos de Ejemplo:"
echo "    | ls"
echo "    ls | | wc"
echo "    <"
echo "    >"
echo "    echo hola >"
echo "    cat < < ${TEST_DIR}/infile.txt"
echo "    ;;"
echo "  Esperado: Mensaje de error de sintaxis, \$? = 2 (o 258 como bash moderno)."
ask_manual_check

print_test_header "G.3 Permisos de ejecución"
echo "  Comandos de Ejemplo (en ${TEST_DIR}):"
echo "    ./script_no_execute.sh"
echo "    echo \$?"
echo "    ./perm_denied_dir # Intentar ejecutar un directorio"
echo "    echo \$?"
echo "  Esperado: Error 'Permission denied' o 'Is a directory', \$? = 126."
ask_manual_check

print_test_header "G.4 Field splitting (argumentos expandidos a múltiples palabras)"
echo "  Debes crear un script 'print_args.sh' en ${TEST_DIR} con:"
echo "    #!/bin/bash"
echo "    for arg in \"\$@\"; do echo \"Arg: [\$arg]\"; done"
echo "  Luego hazlo ejecutable: chmod +x ${TEST_DIR}/print_args.sh"
echo "  Comandos de Ejemplo:"
echo "    export MULTI_WORDS=\"uno dos tres\""
echo "    ./${TEST_DIR}/print_args.sh \$MULTI_WORDS"
echo "    ./${TEST_DIR}/print_args.sh \"\$MULTI_WORDS\""
echo "  Esperado con \$MULTI_WORDS: Arg: [uno], Arg: [dos], Arg: [tres]"
echo "  Esperado con \"\$MULTI_WORDS\": Arg: [uno dos tres]"
ask_manual_check

# La limpieza se hará automáticamente al salir con 'trap'

echo -e "\n${BLUE}FIN DE LA GUÍA DE PRUEBAS DE MINISHELL${NC}"
echo "Recuerda que el comportamiento de los builtins que modifican el entorno (cd, export, unset, exit)"
echo "y las señales deben ser verificados cuidadosamente de forma interactiva."
echo "Usa 'valgrind --leak-check=full ${MINISHELL_EXE}' para buscar fugas de memoria."

exit 0

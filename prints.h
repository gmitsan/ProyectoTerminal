#include <iostream>
#include <windows.h>
using namespace std;

// Colores
const string RESET = "\x1b[0m";      
const string NEGRITA = "\x1b[1m";    
const string SUBRAYADO = "\x1b[4m";  

const string NEGRO = "\x1b[30m";
const string ROJO = "\x1b[31m";
const string VERDE = "\x1b[32m";
const string AMARILLO = "\x1b[33m";
const string AZUL = "\x1b[34m";
const string MAGENTA = "\x1b[35m";
const string CIAN = "\x1b[36m";
const string BLANCO = "\x1b[37m";
const string AZUL_OSCURO = "\x1b[38;5;20m";
const string AZUL_MEDIO = "\x1b[38;5;33m";
const string AZUL_CLARO = "\x1b[38;5;45m";
const string LILA = "\x1b[38;5;99m";
const string MORADO = "\x1b[38;5;129m";
const string VIOLETA = "\x1b[38;5;135m";
const string GRIS_CLARO = "\x1b[38;5;250m";

const string FONDO_NEGRO = "\x1b[40m";
const string FONDO_ROJO = "\x1b[41m";
const string FONDO_VERDE = "\x1b[42m";
const string FONDO_AMARILLO = "\x1b[43m";
const string FONDO_AZUL = "\x1b[44m";
const string FONDO_MAGENTA = "\x1b[45m";
const string FONDO_CIAN = "\x1b[46m";
const string FONDO_BLANCO = "\x1b[47m";

void limpiarPantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void Entrada() {
    SetConsoleOutputCP(65001);
    cout << "\n";
    cout << AZUL_MEDIO << NEGRITA;
    cout << "  ╔══════════════════════════════════════════════════════════╗\n";
    cout << "  ║" << VIOLETA << "                                                     " << AZUL_MEDIO << "     ║\n";
    cout << "  ║" << MORADO << "      U N I X   T E R M I N A L   S I M U L A T O R  " << AZUL_MEDIO << "     ║\n";
    cout << "  ║" << VIOLETA << "                                                     " << AZUL_MEDIO << "     ║\n";
    cout << "  ╠══════════════════════════════════════════════════════════╣\n";
    cout << "  ║" << LILA << "                     DESARROLLADORAS                      " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "     Gabriela Cantos • Steizy Fornica • Amelie Moreno     " << AZUL_MEDIO << "║\n";
    cout << "  ╠══════════════════════════════════════════════════════════╣\n";
    cout << "  ║" << VIOLETA << "                   COMANDOS DISPONIBLES                   " << AZUL_MEDIO << "║\n";
    cout << "  ║                                                          ║\n";
    cout << "  ║" << AZUL_CLARO << "    cd ." << GRIS_CLARO << "               - Quedarse en directorio actual    " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    cd /" << GRIS_CLARO << "               - Ir al directorio raíz            " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    cd .." << GRIS_CLARO << "              - Subir al directorio padre        " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    cd ~" << GRIS_CLARO << "               - Ir a carpeta personal            " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    ls" << GRIS_CLARO << "                 - Listar contenidos                " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    mkdir <nombre>" << GRIS_CLARO << "     - Crear nuevo directorio           " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    rm <nombre>" << GRIS_CLARO << "        - Eliminar archivo/directorio      " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    pwd" << GRIS_CLARO << "                - Mostrar directorio actual        " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    touch <nombre>" << GRIS_CLARO << "     - Crear archivo tetxo plano        " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    mv <actual><nuevo>" << GRIS_CLARO << " - Renombrar archivo o directorio   " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    nano <archivo>" << GRIS_CLARO << "     - Editar contenido de un archivo   " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    exit" << GRIS_CLARO << "               - Salir de la terminal             " << AZUL_MEDIO << "║\n";
    cout << "  ║                                                          ║\n";
    cout << "  ║" << MORADO << "     Escribe 'help' en cualquier momento para ayuda       " << AZUL_MEDIO << "║\n";
    cout << "  ╚══════════════════════════════════════════════════════════╝\n";
    cout << RESET << "\n";
}

void Comandos() {
    SetConsoleOutputCP(65001);
    cout << "\n";
    cout << AZUL_MEDIO << NEGRITA;
    cout << "  ╔════════════════════════════════════════════════════════════╗\n";
    cout << "  ║" << MORADO << "           L I S T A     D E     C O M A N D O S       " << AZUL_MEDIO << "     ║\n";
    cout << "  ╠════════════════════════════════════════════════════════════╣\n";
    cout << "  ║                                                            ║\n";
    cout << "  ║" << AZUL_CLARO << "    cd <ruta>" << GRIS_CLARO << "            - Cambiar directorio               " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    cd ." << GRIS_CLARO << "                 - Quedarse en directorio actual    " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    cd /" << GRIS_CLARO << "                 - Ir al directorio raíz            " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    cd .." << GRIS_CLARO << "                - Subir al directorio padre        " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    cd ~" << GRIS_CLARO << "                 - Ir a carpeta personal            " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    ls" << GRIS_CLARO << "                   - Listar contenidos                " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    mkdir <nombre>" << GRIS_CLARO << "       - Crear nuevo directorio           " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    rm <nombre>" << GRIS_CLARO << "          - Eliminar archivo/directorio      " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    pwd" << GRIS_CLARO << "                  - Mostrar directorio actual        " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    touch <nombre>" << GRIS_CLARO << "       - Crear archivo tetxo plano        " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    mv <actual> <nuevo>" << GRIS_CLARO << "  - Renombrar archivo o directorio   " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    nano <archivo>" << GRIS_CLARO << "       - Editar contenido de un archivo   " << AZUL_MEDIO << "║\n";
    cout << "  ║" << AZUL_CLARO << "    exit" << GRIS_CLARO << "                 - Salir de la terminal             " << AZUL_MEDIO << "║\n";
    cout << "  ║" << MORADO << "                                                            " << AZUL_MEDIO << "║\n";
    cout << "  ╚════════════════════════════════════════════════════════════╝\n";
    cout << RESET << "\n\n";

}

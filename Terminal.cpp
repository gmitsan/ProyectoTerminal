using namespace std; 
#include <iostream>  
#include <fstream>   // Biblioteca para manejo de archivos
#include <string>    // Biblioteca para manejo de strings
#include <windows.h>

// Estructura de los nosodos de archivo/directorio
struct NodoArchivo {
    string nombre;               // Nombre del archivo/directorio
    bool esDirectorio;           // True si es directorio, false si es archivo
    NodoArchivo* padre;          // Puntero al nodo padre en la jerarquía
    NodoArchivo* primerHijo;     // Puntero al primer hijo (para directorios)
    NodoArchivo* siguienteHermano; // Puntero al siguiente nodo hermano
    string rutaAbsoluta;         // Ruta completa desde la raíz

    // Constructor del nodo
    NodoArchivo(const string& nombre, bool esDir, NodoArchivo* padre = NULL) :
        nombre(nombre),          // Inicializa el nombre
        esDirectorio(esDir),     // Inicializa el tipo (archivo/directorio)
        padre(padre),            // Inicializa el puntero al padre
        primerHijo(NULL),     // Inicializa primer hijo como nulo
        siguienteHermano(NULL) // Inicializa siguiente hermano como nulo
    {
        // Construye la ruta completa
        if (padre == NULL) {  // Si no tiene padre (es la raíz)
            rutaAbsoluta = nombre;  // La ruta es solo su nombre
            if (esDirectorio) {   // Si es directorio
                rutaAbsoluta += "/";  // Agrega barra al final
            }
        } else {  // Si tiene padre
            rutaAbsoluta = padre->rutaAbsoluta + nombre; // Concatena con la ruta del padre
            if (esDirectorio) {   // Si es directorio
                rutaAbsoluta += "/";  // Agrega barra al final
            }
        }
    }
};

// Puntero global al nodo raíz de la estructura
NodoArchivo* raiz = nullptr;  

// Puntero global para el directorio actual
NodoArchivo* directorioActual = nullptr;

// ----- FUNCIONES DE IMPRESION -----

// Función recursiva para mostrar TODA la estructura de archivos/directorios
void mostrarEstructura(NodoArchivo* nodo, int nivel = 0, bool esUltimo = false) {
    if (nodo == NULL) return;

    // Indentación jerárquica
    for (int i = 1; i < nivel; i++) {
        cout << "│   ";
    }
    
    // Mostrar conexión de rama (usando if en lugar de ternario)
    if (nivel > 0) {
        if (esUltimo) {
            cout << "└── ";
        } else {
            cout << "├── ";
        }
    }

    // Mostrar nombre del nodo
    cout << nodo->nombre;
    if (nodo->esDirectorio) {
        cout << "/";
    }
    cout << endl;

    // Contar hijos para determinar el último
    NodoArchivo* hijo = nodo->primerHijo;
    int totalHijos = 0;
    NodoArchivo* temp = hijo;
    while (temp != NULL) {
        totalHijos++;
        temp = temp->siguienteHermano;
    }
    int contador = 0;

    // Mostrar recursivamente cada hijo
    while (hijo != NULL) {
        contador++;
        bool esUltimoHijo = (contador == totalHijos);
        mostrarEstructura(hijo, nivel + 1, esUltimoHijo);
        hijo = hijo->siguienteHermano;
    }
}

void mostrarSistemaArchivos() {
    cout << "\n";  
    mostrarEstructura(raiz);
}

// Cuenta los elementos en una ruta (separados por '/')
int contarElementosRuta(const string& ruta) {
    int cantidad = 0;         // Contador de elementos
    bool enElemento = false;  // Bandera para saber si estamos dentro de un elemento
    
    // Recorre cada caracter de la ruta
    // : singnifica "pertenece a", lo que hace es iterar sobre cada elemento de un string, array, etc. sin necesidad de índices
    for (char caracter : ruta) {
        if (caracter != '/') {  // Si no es una barra
            if (!enElemento) {   // Si no estábamos en un elemento
                cantidad++;      // Incrementa el contador
                enElemento = true; // Marca que estamos en un elemento
            }
        } else {               // Si es una barra
            enElemento = false; // Marca que salimos de un elemento
        }
    }
    
    return cantidad;  // Retorna el total de elementos
}

// Divide la ruta en un array de strings con cada elemento
string* dividirRuta(const string& ruta, int& cantidadElementos) {
    cantidadElementos = contarElementosRuta(ruta);  // Obtiene cantidad de elementos
    if (cantidadElementos == 0) return nullptr;     // Si no hay elementos, retorna nulo

    string* elementos = new string[cantidadElementos];  // Crea array dinámico
    int indice = 0;      // Índice para llenar el array
    int inicio = -1;     // Indica la posición donde empieza un nuevo elemento

    // Recorre la ruta incluyendo posición extra para el último elemento
    // Se declara i como size_t ya que ruta.length() devuelve un size_t (long del string) -> evitamos comparaciones entre tipos distintos 
    for (size_t i = 0; i <= ruta.length(); i++) {
        // Si llegamos al final o encontramos una barra
        if (i == ruta.length() || ruta[i] == '/') {
            if (inicio != -1) {  // Si teníamos un elemento iniciado
                // Extrae el substring y lo guarda en el array
                elementos[indice++] = ruta.substr(inicio, i - inicio);
                inicio = -1;  // Resetea la posición de inicio
            }
        } else if (inicio == -1) {  // Si encontramos inicio de nuevo elemento
            inicio = i;  // Guarda posición de inicio
        }
    }

    return elementos;  // Retorna el array de elementos
}

// Determina si un nombre corresponde a un archivo (contiene punto)
bool esArchivo(const string& nombre) {
    // Busca un punto en el nombre
    for (char c : nombre) {
        if (c == '.') return true;  // Si encuentra punto, es archivo
    }
    return false;  // Si no, es directorio
}

// Busca un hijo con nombre específico en un directorio
NodoArchivo* buscarHijo(NodoArchivo* directorio, const string& nombre) {
    NodoArchivo* hijo = directorio->primerHijo;  // Comienza con el primer hijo
    
    // Recorre todos los hermanos
    while (hijo != NULL) {
        if (hijo->nombre == nombre) {  // Si encuentra coincidencia
            return hijo;  // Retorna el nodo encontrado
        }
        hijo = hijo->siguienteHermano;  // Avanza al siguiente hermano
    }
    
    return NULL;  // Si no lo encuentra, retorna nulo
}

// Agrega un nuevo hijo a un directorio
void agregarHijo(NodoArchivo* directorio, NodoArchivo* nuevoHijo) {
    nuevoHijo->padre = directorio;  // Establece el padre del nuevo hijo
    
    if (directorio->primerHijo == NULL) {  // Si no tiene hijos
        directorio->primerHijo = nuevoHijo;   // Lo establece como primer hijo
    } else {  // Si ya tiene hijos
        NodoArchivo* hermano = directorio->primerHijo;  // Comienza con el primer hijo
        // Busca el último hermano
        while (hermano->siguienteHermano != NULL) {
            hermano = hermano->siguienteHermano;
        }
        hermano->siguienteHermano = nuevoHijo;  // Agrega el nuevo al final
    }
}

// Puntero al directorio personal del usuario (en este caso, "/Home/User")
NodoArchivo* directorioPersonal = NULL;

// Inicializa el sistema de archivos creando la raíz
void inicializarSistema() {
    if (raiz == NULL) {
        raiz = new NodoArchivo("/", true);
        
        // Crear estructura básica de directorios
        NodoArchivo* home = new NodoArchivo("Home", true, raiz);
        agregarHijo(raiz, home);
        
        directorioPersonal = new NodoArchivo("User", true, home);
        agregarHijo(home, directorioPersonal);
        
        // Asegurar que directorioActual tenga un valor válido
        // Primera opción: Usar directorioPersonal como actual // Segunda opción: Si no existe, usar raiz (/) como respaldo
        if (directorioPersonal != NULL) {
            directorioActual = directorioPersonal;
        } else {
            directorioActual = raiz;
        }
    }
}

// Inserta una ruta completa en el sistema de archivos
void insertarRuta(const string& ruta) {
    if (ruta.empty()) return;  // Si la ruta está vacía, no hace nada

    int cantidadElementos = 0;

    // Divide la ruta en componentes individuales
    string* elementos = dividirRuta(ruta, cantidadElementos);

    if (elementos == NULL) return;  // Si no hay elementos válidos, sale

    NodoArchivo* actual = raiz;  // Comienza desde el directorio raíz

    // Procesa cada elemento de la ruta
    for (int i = 0; i < cantidadElementos; i++) {
        // Busca si el elemento ya existe como hijo
        NodoArchivo* hijo = buscarHijo(actual, elementos[i]);

        if (hijo == NULL) {  // Si no existe
            bool esDir = true;  // Asume que es directorio, pero luego hay comprobación...
            
            // Aquí se verifica si tiene punto, y es el último, es archivo
            if (i == cantidadElementos - 1 && esArchivo(elementos[i])) {
                esDir = false;
            }

            // Crea el nuevo nodo
            hijo = new NodoArchivo(elementos[i], esDir, actual);
            // Lo agrega como hijo
            agregarHijo(actual, hijo);
        }

        actual = hijo;  // Avanza al siguiente nivel de la jerarquía
    }
}

// Función para cargar la estructura de directorios desde un archivo de texto
bool cargarDesdeArchivo(const string& nombreArchivo) {
    // Intenta abrir el archivo en modo lectura
    ifstream archivo(nombreArchivo);
    
    // Verifica si el archivo se abrió correctamente
    if (!archivo) {
        // Muestra mensaje de error si no pudo abrirse
        cout << "ERROR: No se encontró el archivo: " << nombreArchivo << endl;
        return false;  // Retorna false indicando fallo
    }

    // Mensaje informativo de inicio de proceso
    cout << "Leyendo archivo: " << nombreArchivo << "..." << endl;
    
    // Contadores para estadísticas
    int totalLineas = 0;      // Total de líneas en el archivo
    int lineasProcesadas = 0; // Líneas con rutas válidas

    string lineaActual;  // Variable para almacenar cada línea leída
    
    // Leer el archivo línea por línea
    while (getline(archivo, lineaActual)) {
        totalLineas++;  // Incrementa contador de líneas leídas

        // Busca el primer caracter que no sea espacio, tab o salto de línea
        size_t primerChar = lineaActual.find_first_not_of(" \t\r\n");
        
        // Si no encuentra caracteres válidos, salta esta línea
        // npos -> No Position, representa una posición inválida o no encontrada
        if (primerChar == string::npos) {
            continue;  // Pasa a la siguiente línea
        }

        // Busca el último caracter que no sea espacio, tab o salto de línea
        size_t ultimoChar = lineaActual.find_last_not_of(" \t\r\n");
        
        // Extrae la subcadena sin espacios al inicio ni al final
        string rutaLimpia = lineaActual.substr(primerChar, ultimoChar - primerChar + 1);

        // Inserta la ruta limpia en la estructura de directorios
        insertarRuta(rutaLimpia);
        lineasProcesadas++;  // Incrementa contador de líneas procesadas
    }

    // Cierra el archivo después de leerlo completamente
    archivo.close();

    // Muestra resumen estadístico del proceso
    cout << "Proceso completado:" << endl;
    cout << "- Lineas leídas: " << totalLineas << endl;
    cout << "- Rutas procesadas: " << lineasProcesadas << endl;
    cout << "- Lineas vacías ignoradas: " << (totalLineas - lineasProcesadas) << endl;

    // Retorna true indicando que el proceso terminó exitosamente
    return true;
}

// ----- FUNCIONES PARA CD -----
// Función para cambiar el directorio actual del sistema de archivos
// true si el cambio fue exitoso, false si hubo error
bool cambiarDirectorio(const string& ruta) {
    if (directorioActual == NULL) {
        directorioActual = raiz;
    }

    // Caso 1: Ruta vacía → no cambia el directorio 
    // Caso 2: Ruta es . → directorio actual 
    if (ruta.empty() || ruta == ".") {
        return true;
    }

    NodoArchivo* nuevoDir = NULL;

    // Caso 3: Ruta es / → Cambia al directorio raíz
    if (ruta == "/") {
        nuevoDir = raiz;
    } 
    // Caso 4: Ruta es ~ → Busca el directorio personal (/Home/User).
    else if (ruta == "~") {
        // Buscar directorio personal si no está inicializado
        if (directorioPersonal == NULL) {
            NodoArchivo* home = buscarHijo(raiz, "Home");
            if (home) {
                directorioPersonal = buscarHijo(home, "User");
            }
        }
        // Si aún no se encontró, mostrar mensaje de error y no cambiar el directorio
        if (directorioPersonal == NULL) {
            cout << "No se encontró el directorio personal (~)" << endl;
            return false;
        }
        nuevoDir = directorioPersonal;
    } 
    // Caso 5: Ruta normal (absoluta o relativa)
    else {
        bool rutaAbsoluta = (ruta[0] == '/');
        NodoArchivo* base;

        // Se establece el punto de partida para la navegación
        if (rutaAbsoluta) {
            base = raiz;
        } else {
            base = directorioActual;
        }

        // Dividir la ruta en componentes
        int numPartes = 0;
        string* partes = dividirRuta(ruta, numPartes);
        if (partes == NULL) return false; // La ruta no contiene elementos válidos

        // Navegar por la estructura de directorios
        nuevoDir = base; // Comenzar desde el directorio base
        for (int i = 0; i < numPartes; i++) {
            const string& p = partes[i]; // Componente actual de la ruta
            if (p == ".") continue; // Ignorar "." (directorio actual)
            if (p == "..") { // Subir al directorio padre si es posible
                if (nuevoDir->padre != NULL) nuevoDir = nuevoDir->padre;
            } else {
                // Buscar el subdirectorio en el directorio actual
                NodoArchivo* hijo = buscarHijo(nuevoDir, p);
                // Verificar que exista y sea un directorio
                if (hijo == NULL || !hijo->esDirectorio) {
                    return false; // No se encontró el directorio
                }
                nuevoDir = hijo; // Moverse al subdirectorio
            }
        }
        delete[] partes; 
    }

    // Actualizar el directorio actual si todo fue bien
    if (nuevoDir != NULL) {
        directorioActual = nuevoDir;
        return true;
    }
    return false; // Fallo por alguna razón no contemplada
}

// Muestra el prompt del sistema de archivos con el directorio actual
void mostrarPrompt() {
    if (directorioActual == NULL) {
        directorioActual = raiz;
    }

    // Directorio raíz
    if (directorioActual == raiz) {
        cout << "/$ ";
        return;
    }

    // Directorio personal del usuario
    if (directorioActual == directorioPersonal) {
        cout << "~/$ "; // Usa "~" como abreviatura del directorio personal
        return;
    }

    // Usar la ruta completa almacenada en el nodo
    string ruta = directorioActual->rutaAbsoluta;

    // Reemplazar "/Home/User" por "~" si está en el directorio personal
    size_t pos = ruta.find("/Home/User");
    if (pos != string::npos) {
        ruta.replace(pos, 13, "~"); // Reemplazar 13 caracteres por "~"
    }

    // 6. Mostrar el prompt final con el símbolo $
    cout << ruta << "$ ";
}

// ----- FUNCIONES PARA PWD -----
// Función que imprime el directorio actual y su contenido (Print Working Directory)
void pwd() {
    if (directorioActual == NULL) {
        cout << "/" << endl;
        return;
    }

    if (directorioActual == raiz) {
        cout << "/" << endl;
        return;
    }

    if (directorioActual == directorioPersonal) {
        cout << "~/" << endl;
        return;
    }

    // Obtener la ruta completa almacenada en el nodo
    string ruta = directorioActual->rutaAbsoluta;

    // Reemplazar "/Home/User" por "~" si corresponde
    size_t pos = ruta.find("/Home/User");
    if (pos != string::npos) {
        ruta.replace(pos, 13, "~");
    }

    // Imprimir la ruta resultante
    cout << ruta << endl;
}

// Función para listar los contenidos del directorio actual
void listarDirectorioActual() {
    if (directorioActual == NULL) {
        cout << "ERROR No hay directorio actual definido" << endl;
        return;
    }

    if (!directorioActual->esDirectorio) {
        cout << "ERROR El nodo actual no es un directorio" << endl;
        return;
    }

    NodoArchivo* hijo = directorioActual->primerHijo;
    int totalHijos = 0;

    // Contar el número de hijos
    NodoArchivo* temp = hijo;
    while (temp != NULL) {
        totalHijos++;
        temp = temp->siguienteHermano;
    }

    // Mostrar cada hijo CON SU FORMATO
    int contador = 0;
    while (hijo != NULL) {
        contador++;
        bool esUltimo = (contador == totalHijos);

        // Estructura de árbol
        if (esUltimo) {
            cout << "└── ";
        } else {
            cout << "├── ";
        }

        // Nombre del archivo/directorio
        cout << hijo->nombre;

        // Marcar directorios con "/"
        if (hijo->esDirectorio) {
            cout << "/";
        }

        cout << endl;
        hijo = hijo->siguienteHermano;
    }

    // Si no hay elementos, mostrar mensaje
    if (totalHijos == 0) {
        cout << "(vacío)" << endl;
    }
}

// ----- PROGRAMA PRINCIPAL -----
int main() {
    //SetConsoleOutputCP(CP_UTF8);  // Configurar salida UTF-8
    inicializarSistema();  // Inicializa el sistema
    const string archivoEntrada = "Ejemplo1.txt";  // Archivo a cargar
    directorioActual = raiz;  // Empezar en la raíz

    cout << "Cargando estructura desde '" << archivoEntrada << "'..." << endl;

    // Cargar estructura inicial
    if (!cargarDesdeArchivo("Ejemplo1.txt")) {
        cout << "ADVERTENCIA! No se pudo cargar el archivo inicial" << endl;
    }

    mostrarSistemaArchivos(); 
    
    string comando;
    
    while (true) {
        mostrarPrompt();  // Mostrar ruta actual antes de cada comando
        getline(cin, comando);
        
        // Eliminar espacios al inicio y final
        comando.erase(0, comando.find_first_not_of(" \t"));
        comando.erase(comando.find_last_not_of(" \t") + 1);
        
        if (comando.empty()) continue;  // Ignorar líneas vacías
        
        // Procesar comando cd
        if (comando.find("cd ") == 0) {
            string ruta = comando.substr(3);
            ruta.erase(0, ruta.find_first_not_of(" \t"));  // Limpiar espacios
            if (!cambiarDirectorio(ruta)) {
                cout << "cd: " << ruta << ": No existe el directorio" << endl;
            }
        }
        // Comando para mostrar estructura
        else if (comando == "ls") {
            listarDirectorioActual();
        }
        
        else if (comando == "pwd") {
            cout<<directorioActual->rutaAbsoluta<< endl;
            pwd();
        }
        // Comando para salir
        else if (comando == "exit") {
            break;
        }
        else {
            cout << "Comando no reconocido. Comandos disponibles:" << endl;
            cout << "  cd <ruta> - Cambiar directorio" << endl;
            cout << "  ls        - Mostrar estructura" << endl;
            cout << "  pwd       - Mostrar ruta actual" << endl;
            cout << "  exit      - Salir del programa" << endl;
        }
            }
    return 0;
}
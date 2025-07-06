#include <iostream>  
#include <fstream>   
#include <string>    
#include <vector>
#define WIN32_LEAN_AND_MEAN  
#include <windows.h>
#include "prints.h"
using namespace std;

// --------------------------
// ESTRUCTURAS DE DATOS (Solo datos, sin métodos)
// --------------------------

struct Archivo {
    string nombre;
    string extension;
    Archivo* siguiente;
    string rutaAbsoluta;
};

struct Directorio {
    string nombre;
    Directorio* padre;
    Directorio* primerSubdir;
    Archivo* primerArchivo;
    Directorio* siguiente;
    string rutaAbsoluta;
};

// --------------------------
// VARIABLES GLOBALES
// --------------------------

Directorio* raiz = nullptr;
Directorio* directorioActual = nullptr;
Directorio* directorioPersonal = nullptr;

// ----------- FUNCION DE IMPRESION
void mostrarArbolEstructura(Directorio* dir, int nivel = 0, bool esUltimo = false) {
    if (dir == nullptr) return;

    // Indentación jerárquica
    for (int i = 1; i < nivel; i++) {
        cout << "│  ";
    }
    
    // Mostrar conexión de rama
    if (nivel > 0) {
        if (esUltimo) {
            cout << "└── ";
        } else {
            cout << "├── ";
        }
    }

    // Mostrar nombre del directorio
    if (dir->nombre == "/") {
        cout << "/" << endl;
    } else {
        cout << dir->nombre << "/" << endl;
    }

    // Contar hijos (subdirectorios + archivos)
    int totalHijos = 0;
    
    // Contar subdirectorios
    Directorio* subdir = dir->primerSubdir;
    while (subdir != nullptr) {
        totalHijos++;
        subdir = subdir->siguiente;
    }
    
    // Contar archivos
    Archivo* arch = dir->primerArchivo;
    while (arch != nullptr) {
        totalHijos++;
        arch = arch->siguiente;
    }

    int contador = 0;

    // Recorrer subdirectorios en orden inverso
    Directorio* hijoDir = dir->primerSubdir;
    Directorio* dirs[100]; // Almacenar directorios temporalmente
    int dirCount = 0;
    
    while (hijoDir != nullptr) {
        dirs[dirCount++] = hijoDir;
        hijoDir = hijoDir->siguiente;
    }
    
    for (int i = dirCount - 1; i >= 0; i--) {
        contador++;
        bool esUltimoHijo = (contador == totalHijos);
        mostrarArbolEstructura(dirs[i], nivel + 1, esUltimoHijo);
    }

    // Recorrer archivos en orden inverso
    Archivo* hijoArch = dir->primerArchivo;
    Archivo* archivos[100]; // Almacenar archivos temporalmente
    int archCount = 0;
    
    while (hijoArch != nullptr) {
        archivos[archCount++] = hijoArch;
        hijoArch = hijoArch->siguiente;
    }
    
    for (int i = archCount - 1; i >= 0; i--) {
        contador++;
        bool esUltimoHijo = (contador == totalHijos);

        // Indentación para archivos
        for (int j = 1; j < nivel + 1; j++) {
            cout << "│  ";
        }
        
        if (esUltimoHijo) {
            cout << "└── ";
        } else {
            cout << "├── ";
        }

        cout << archivos[i]->nombre;
        if (!archivos[i]->extension.empty()) {
            cout << "." << archivos[i]->extension;
        }
        cout << endl;
    }
}
void mostrarSistemaArchivos() {
    if (directorioActual == nullptr) {
        cout << ROJO << "ERROR: Directorio actual no establecido" << RESET << endl;
        return;
    }
    mostrarArbolEstructura(directorioActual);
}
// --------------------------
// FUNCIONES PARA MANIPULAR ESTRUCTURAS
// --------------------------

// Crear un nuevo archivo
Archivo* crearArchivo(const string& nombreCompleto) {
    Archivo* nuevo = new Archivo;
    size_t punto = nombreCompleto.find_last_of('.');
    if (punto != string::npos) {
        nuevo->nombre = nombreCompleto.substr(0, punto);
        nuevo->extension = nombreCompleto.substr(punto + 1);
    } else {
        nuevo->nombre = nombreCompleto;
        nuevo->extension = "";
    }
    nuevo->siguiente = nullptr;
    nuevo->rutaAbsoluta = "";
    return nuevo;
}

// Crear un nuevo directorio
Directorio* crearDirectorio(const string& nom, Directorio* p = nullptr) {
    Directorio* nuevo = new Directorio;
    nuevo->nombre = nom;
    nuevo->padre = p;
    nuevo->primerSubdir = nullptr;
    nuevo->primerArchivo = nullptr;
    nuevo->siguiente = nullptr;
    
    if (p == nullptr) {
        nuevo->rutaAbsoluta = "/";
    } else {
        nuevo->rutaAbsoluta = p->rutaAbsoluta;
        if (nuevo->rutaAbsoluta.back() != '/') {
            nuevo->rutaAbsoluta += "/";
        }
        nuevo->rutaAbsoluta += nom + "/";
    }
    
    return nuevo;
}

// Agregar subdirectorio a un directorio
void agregarSubdirectorio(Directorio* dir, Directorio* nuevo) {
    nuevo->siguiente = dir->primerSubdir;
    dir->primerSubdir = nuevo;
}

// Agregar archivo a un directorio
void agregarArchivo(Directorio* dir, Archivo* nuevo) {
    nuevo->siguiente = dir->primerArchivo;
    dir->primerArchivo = nuevo;
}

// Buscar subdirectorio en un directorio
Directorio* buscarSubdirectorio(Directorio* dir, const string& nombre) {
    Directorio* actual = dir->primerSubdir;
    while (actual != nullptr) {
        if (actual->nombre == nombre) return actual;
        actual = actual->siguiente;
    }
    return nullptr;
}

// Buscar archivo en un directorio
Archivo* buscarArchivo(Directorio* dir, const string& nombre) {
    Archivo* actual = dir->primerArchivo;
    while (actual != nullptr) {
        if (actual->nombre == nombre) return actual;
        actual = actual->siguiente;
    }
    return nullptr;
}

// Eliminar un directorio recursivamente
void destruirDirectorio(Directorio* dir) {
    if (!dir) return;
    
    // Eliminar subdirectorios
    Directorio* subdir = dir->primerSubdir;
    while (subdir != nullptr) {
        Directorio* siguienteSubdir = subdir->siguiente;
        destruirDirectorio(subdir);
        subdir = siguienteSubdir;
    }
    
    // Eliminar archivos
    Archivo* archivo = dir->primerArchivo;
    while (archivo != nullptr) {
        Archivo* siguienteArchivo = archivo->siguiente;
        delete archivo;
        archivo = siguienteArchivo;
    }
    
    delete dir;
}

// --------------------------
// FUNCIONES AUXILIARES
// --------------------------

int contarElementosRuta(const string& ruta) {
    int cantidad = 0;
    bool enElemento = false;
    
    for (char c : ruta) {
        if (c != '/') {
            if (!enElemento) {
                cantidad++;
                enElemento = true;
            }
        } else {
            enElemento = false;
        }
    }
    
    return cantidad;
}

string* dividirRuta(const string& ruta, int& cantidadElementos) {
    cantidadElementos = contarElementosRuta(ruta);
    if (cantidadElementos == 0) return nullptr;

    string* elementos = new string[cantidadElementos];
    int indice = 0;
    int inicio = -1;

    for (size_t i = 0; i <= ruta.length(); i++) {
        if (i == ruta.length() || ruta[i] == '/') {
            if (inicio != -1) {
                elementos[indice++] = ruta.substr(inicio, i - inicio);
                inicio = -1;
            }
        } else if (inicio == -1) {
            inicio = i;
        }
    }

    return elementos;
}

bool esArchivo(const string& nombre) {
    return nombre.find('.') != string::npos;
}

// --------------------------
// FUNCIONES PRINCIPALES
// --------------------------

void inicializarSistema() {
    if (raiz == nullptr) {
        raiz = crearDirectorio("");
        raiz->nombre = "/";
        
        Directorio* home = crearDirectorio("Home", raiz);
        agregarSubdirectorio(raiz, home);
        
        directorioPersonal = crearDirectorio("User", home);
        agregarSubdirectorio(home, directorioPersonal);
        
        if (directorioPersonal != NULL) {
            directorioActual = directorioPersonal;
        } else {
            directorioActual = raiz;
        }
    }
}

void insertarRuta(const string& ruta) {
    if (ruta.empty()) return;

    int cantidadElementos = 0;
    string* elementos = dividirRuta(ruta, cantidadElementos);
    if (elementos == nullptr) return;

    Directorio* actual = raiz;

    for (int i = 0; i < cantidadElementos; i++) {
        if (esArchivo(elementos[i])) {
            // Es un archivo (solo si es el último elemento)
            if (i == cantidadElementos - 1) {
                if (buscarArchivo(actual, elementos[i])) continue;
                Archivo* nuevoArchivo = crearArchivo(elementos[i]);
                nuevoArchivo->rutaAbsoluta = actual->rutaAbsoluta + elementos[i];
                agregarArchivo(actual, nuevoArchivo);
            }
        } else {
            // Es un directorio
            Directorio* subdir = buscarSubdirectorio(actual, elementos[i]);
            if (subdir == nullptr) {
                subdir = crearDirectorio(elementos[i], actual);
                agregarSubdirectorio(actual, subdir);
            }
            actual = subdir;
        }
    }

    delete[] elementos;
}

bool cargarDesdeArchivo(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    
    if (!archivo) {
        cout << ROJO << "ERROR: No se encontró el archivo: " << nombreArchivo << RESET << endl;
        return false;
    }

    cout << "Leyendo archivo: " << nombreArchivo << "..." << endl;
    
    int totalLineas = 0;
    int lineasProcesadas = 0;
    string lineaActual;
    
    while (getline(archivo, lineaActual)) {
        totalLineas++;
        size_t primerChar = lineaActual.find_first_not_of(" \t\r\n");
        
        if (primerChar == string::npos) continue;

        size_t ultimoChar = lineaActual.find_last_not_of(" \t\r\n");
        string rutaLimpia = lineaActual.substr(primerChar, ultimoChar - primerChar + 1);

        insertarRuta(rutaLimpia);
        lineasProcesadas++;
    }

    archivo.close();

    /*cout << "\n¡Proceso completado!" << endl;
    cout << "  - Lineas leídas: " << totalLineas << endl;
    cout << "  - Rutas procesadas: " << lineasProcesadas << endl;
    cout << "  - Lineas vacías ignoradas: " << (totalLineas - lineasProcesadas) << endl;
    cout << "" << endl;*/

    return true;
}

// --------------------------
// FUNCIONES DE COMANDOS
// --------------------------

// ----- CD -----
// Función para cambiar el directorio actual del sistema de archivos
// true si el cambio fue exitoso, false si hubo error
bool cambiarDirectorio(const string& ruta) {
    if (directorioActual == nullptr) {
        directorioActual = raiz;
    }
    // Caso 1: Ruta vacía → no cambia el directorio 
    // Caso 2: Ruta es . → directorio actual 
    if (ruta.empty() || ruta == ".") {
        return true;
    }

    Directorio* nuevoDir = nullptr;
    // Caso 3: Ruta es / → Cambia al directorio raíz
    if (ruta == "/") {
        nuevoDir = raiz;
    } 
    // Caso 4: Ruta es ~ → Busca el directorio personal (/Home/User).
    else if (ruta == "~") {
        // Buscar directorio personal si no está inicializado
        if (directorioPersonal == NULL) {
            Directorio* home = buscarSubdirectorio(raiz, "Home");
            if (home) {
                directorioPersonal = buscarSubdirectorio(home, "User");
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
        Directorio* base;
        // Se establece el punto de partida para la navegación
        if (rutaAbsoluta) {
            base = raiz;
        } else {
            base = directorioActual;
        }
        // Dividir la ruta en componentes
        int numPartes = 0;
        string* partes = dividirRuta(ruta, numPartes);
        if (partes == nullptr) return false; // La ruta no contiene elementos válidos
        
        // Navegar por la estructura de directorios
        nuevoDir = base;

        for (int i = 0; i < numPartes; i++) {
            const string& p = partes[i]; // Componente actual de la ruta
            if (p == ".") continue; // Ignorar "." (directorio actual)
            if (p == "..") { // Subir al directorio padre si es posible
                if (nuevoDir->padre != nullptr) nuevoDir = nuevoDir->padre;
            } else {
                // Buscar el subdirectorio en el directorio actual
                Directorio* subdir = buscarSubdirectorio(nuevoDir, p);
                if (subdir == nullptr) {
                    return false;
                }
                nuevoDir = subdir;
            }
        }
    }
    // Actualizar el directorio actual si todo fue bien
    if (nuevoDir != nullptr) {
        directorioActual = nuevoDir;
        return true;
    }
    return false;
}
// Muestra el prompt del sistema de archivos con el directorio actual
void mostrarPrompt() {
    if (directorioActual == nullptr) {
        cout << "/$ ";
        return;
    }

    string ruta = directorioActual->rutaAbsoluta;
    
    // Casos especiales
    if (ruta == "/") {
        cout << "/$ ";
    } 
    else if (ruta == "/Home/User/") {
        cout << "~/$ ";
    }
    else if (ruta.find("/Home/User/") == 0) {
        cout << "~" << ruta.substr(10) << "$ ";
    }
    else {
        cout << ruta << "$ ";
    }
}
// ----- PWD -----
// Función que imprime la ruta del directorio actual (Print Working Directory)
void pwd() {
    if (directorioActual == nullptr) {
        cout << "/" << endl;
        return;
    }

    string ruta = directorioActual->rutaAbsoluta;
    
    if (ruta == "/") {
        cout << "/" << endl;
    }
    else if (ruta == "/Home/User/") {
        cout << "~/" << endl;
    }
    else if (ruta.find("/Home/User/") == 0) {
        cout << "~" << ruta.substr(10) << endl;
    }
    else {
        cout << ruta << endl;
    }
    cout<<directorioActual->rutaAbsoluta<<endl;
}

// ----- LS -----
// Función para listar los contenidos del directorio actual
void listarDirectorioActual() {
    if (directorioActual == nullptr) {
        cout << ROJO << "ERROR No hay directorio actual definido" << RESET << endl;
        return;
    }

    int totalElementos = 0;
    
    // Contar subdirectorios
    Directorio* subdir = directorioActual->primerSubdir;
    while (subdir != nullptr) {
        totalElementos++;
        subdir = subdir->siguiente;
    }
    
    // Contar archivos
    Archivo* archivo = directorioActual->primerArchivo;
    while (archivo != nullptr) {
        totalElementos++;
        archivo = archivo->siguiente;
    }

    // Mostrar subdirectorios
    subdir = directorioActual->primerSubdir;
    int contador = 0;
    while (subdir != nullptr) {
        contador++;
        bool esUltimo = (contador == totalElementos);
        // Estructura de árbol
        if (esUltimo) {
            cout << "└── ";
        } else {
            cout << "├── ";
        }

        cout << subdir->nombre << "/" << endl;
        subdir = subdir->siguiente;
    }

    // Mostrar archivos
    archivo = directorioActual->primerArchivo;
    while (archivo != nullptr) {
        contador++;
        bool esUltimo = (contador == totalElementos);

        if (esUltimo) {
            cout << "└── ";
        } else {
            cout << "├── ";
        }

        cout << archivo->nombre;
        if (!archivo->extension.empty()) {
            cout << "." << archivo->extension;
        }
        cout << endl;
        archivo = archivo->siguiente;
    }

    if (totalElementos == 0) {
        cout << "(vacío)" << endl;
    }
}

// ----- MKDIR -----
// Función para crear un nuevo directorio en el directorio actual
bool crearDirectorioCmd(const string& nombre) {
    if (directorioActual == nullptr) {
        cout << ROJO << "ERROR No se puede crear en la ubicación actual" << RESET << endl;
        return false;
    }
    
    if (nombre.empty()) {
        cout << ROJO << "ERROR Nombre no puede estar vacío" << RESET <<endl;
        return false;
    }
    // Rechazar si parece archivo (contiene punto)
    if (nombre.find('.') != string::npos) {
        cout << ROJO << "ERROR No se permiten puntos en nombres de directorio" << RESET <<endl;
        return false;
    }

    if (buscarSubdirectorio(directorioActual, nombre) != nullptr || 
        buscarArchivo(directorioActual, nombre) != nullptr) {
        cout << ROJO << "ERROR Ya existe '" << nombre << "' en este directorio" << RESET <<endl;
        return false;
    }

    // Crear el nuevo directorio
    Directorio* nuevoDir = crearDirectorio(nombre, directorioActual);
    agregarSubdirectorio(directorioActual, nuevoDir);
    
    cout << VERDE << "Directorio '" << nombre << "' creado exitosamente" << RESET << endl;
    return true;
}
// ----- RM -----
// Función auxiliar para eliminación recursiva
bool eliminarArchivoODirectorio(const string& nombre) {
    if (directorioActual == nullptr) {
        cout << ROJO << "ERROR: No se puede eliminar en la ubicación actual" << RESET <<endl;
        return false;
    }

    if (nombre.empty()) {
        cout << ROJO << "ERROR: Nombre no puede estar vacío" << RESET << endl;
        return false;
    }

    // Buscar primero en subdirectorios
    Directorio* anteriorDir = nullptr;
    Directorio* dirActual = directorioActual->primerSubdir;
    while (dirActual != nullptr && dirActual->nombre != nombre) {
        anteriorDir = dirActual;
        dirActual = dirActual->siguiente;
    }

    if (dirActual != nullptr) {
        // Eliminar directorio y todo su contenido
        if (anteriorDir == nullptr) {
            directorioActual->primerSubdir = dirActual->siguiente;
        } else {
            anteriorDir->siguiente = dirActual->siguiente;
        }
        
        destruirDirectorio(dirActual);
        cout << VERDE << "Directorio '" << nombre << "' eliminado exitosamente" << RESET << endl;
        return true;
    }

    // Si no era directorio, buscar en archivos
    Archivo* anteriorArch = nullptr;
    Archivo* archActual = directorioActual->primerArchivo;
    while (archActual != nullptr && archActual->nombre != nombre) {
        anteriorArch = archActual;
        archActual = archActual->siguiente;
    }

    if (archActual != nullptr) {
        if (anteriorArch == nullptr) {
            directorioActual->primerArchivo = archActual->siguiente;
        } else {
            anteriorArch->siguiente = archActual->siguiente;
        }
        
        delete archActual;
        cout << VERDE << "Archivo '" << nombre << "' eliminado exitosamente" << RESET << endl;
        return true;
    }

    cout << ROJO << "ERROR: No existe '" << nombre << "' en este directorio" << RESET <<endl;
    return false;
}

// ----- TOUCH -----
// Función para crear un nuevo archivo de texto en el directorio actual
bool crearArchivoCmd(const string& nombreCompleto) {
    if (directorioActual == nullptr) {
        cout << ROJO << "ERROR: No se puede crear archivos en la ubicación actual" << RESET << endl;
        return false;
    }
    // Validación del nombre
    if (nombreCompleto.empty()) {
        cout << ROJO <<"ERROR: El nombre del archivo no puede estar vacío" << RESET <<endl;
        return false;
    }

    // Validar extensión .txt
    size_t punto = nombreCompleto.find_last_of('.');
    if (punto == string::npos || nombreCompleto.substr(punto) != ".txt") {
        cout << ROJO << "ERROR: Solo se permiten archivos con extensión .txt" << RESET << endl;
        return false;
    }

    // Extraer nombre base (sin extensión)
    string nombreBase = nombreCompleto.substr(0, punto);

    // Verificar si ya existe como directorio o archivo
    if (buscarSubdirectorio(directorioActual, nombreBase) != nullptr || 
        buscarArchivo(directorioActual, nombreBase) != nullptr) {
        cout << ROJO << "ERROR: Ya existe '" << nombreCompleto << "' en este directorio" << RESET <<endl;
        return false;
    }
    // Crear el nuevo archivo 
    Archivo* nuevoArchivo = crearArchivo(nombreCompleto);
    nuevoArchivo->rutaAbsoluta = directorioActual->rutaAbsoluta + nombreCompleto;
    agregarArchivo(directorioActual, nuevoArchivo);
    // Crear el archivo físico primero
    ofstream archivoFisico(nombreCompleto);
    archivoFisico.close();
    
    cout << VERDE << "Archivo '" << nombreCompleto << "' creado exitosamente" << RESET << endl;
    return true;
}
// ----- MV -----
// Función para renombrar un archivo o directorio
bool renombrarElemento(const string& nombreActual, const string& nuevoNombre) {
    if (directorioActual == nullptr) {
        cout << ROJO <<"ERROR: No se puede renombrar en la ubicación actual" << RESET <<endl;
        return false;
    }

    if (nombreActual.empty() || nuevoNombre.empty()) {
        cout << ROJO << "ERROR: Nombre actual y nuevo nombre no pueden estar vacíos" << RESET << endl;
        return false;
    }

    // Verificar si el nuevo nombre ya existe
    if (buscarSubdirectorio(directorioActual, nuevoNombre) != nullptr || 
        buscarArchivo(directorioActual, nuevoNombre) != nullptr) {
        cout << ROJO << "ERROR: Ya existe un elemento con el nombre '" << nuevoNombre << "' en este directorio" << RESET <<endl;
        return false;
    }

    // Buscar y renombrar directorio
    Directorio* anteriorDir = nullptr;
    Directorio* dirActual = directorioActual->primerSubdir;
    while (dirActual != nullptr) {
        if (dirActual->nombre == nombreActual) {
            // Guardar el nombre original para actualizar rutas
            string nombreOriginal = dirActual->nombre;
            
            // Actualizar el nombre
            dirActual->nombre = nuevoNombre;
            
            // Actualizar ruta absoluta de este directorio
            if (dirActual->padre != nullptr) {
                dirActual->rutaAbsoluta = dirActual->padre->rutaAbsoluta + nuevoNombre + "/";
            } else {
                dirActual->rutaAbsoluta = "/";
            }
            
            // Actualizar rutas de todos los contenidos recursivamente
            Directorio* stack[100];
            int stackSize = 0;
            stack[stackSize++] = dirActual;
            
            while (stackSize > 0) {
                Directorio* actual = stack[--stackSize];
                
                // Actualizar subdirectorios
                Directorio* subdir = actual->primerSubdir;
                while (subdir != nullptr) {
                    subdir->rutaAbsoluta = actual->rutaAbsoluta + subdir->nombre + "/";
                    stack[stackSize++] = subdir;
                    subdir = subdir->siguiente;
                }
                
                // Actualizar archivos
                Archivo* archivo = actual->primerArchivo;
                while (archivo != nullptr) {
                    archivo->rutaAbsoluta = actual->rutaAbsoluta + archivo->nombre;
                    if (!archivo->extension.empty()) {
                        archivo->rutaAbsoluta += "." + archivo->extension;
                    }
                    archivo = archivo->siguiente;
                }
            }
            
            cout << VERDE << "Directorio '" << nombreOriginal << "' renombrado a '" << nuevoNombre << "' exitosamente" << RESET << endl;
            return true;
        }
        anteriorDir = dirActual;
        dirActual = dirActual->siguiente;
    }

    // Buscar y renombrar archivo
    Archivo* anteriorArch = nullptr;
    Archivo* archActual = directorioActual->primerArchivo;
    while (archActual != nullptr) {
        //cout<< "Nombre actual: " << archActual->nombre<<" y"<<archActual->extension << " Ingresado: "<<nombreActual<<endl;
        if (archActual->nombre+"."+archActual->extension == nombreActual) {
            // Guardar nombre y extensión originales
            string nombreOriginal = archActual->nombre;
            string extensionOriginal = archActual->extension;
            
            // Verificar si el nuevo nombre incluye extensión
            size_t punto = nuevoNombre.find_last_of('.');
            if (punto != string::npos) {
                archActual->nombre = nuevoNombre.substr(0, punto);
                archActual->extension = nuevoNombre.substr(punto + 1);
            } else {
                archActual->nombre = nuevoNombre;
                // Mantener la extensión original si no se especifica nueva
            }
            
            // Actualizar ruta absoluta
            archActual->rutaAbsoluta = directorioActual->rutaAbsoluta + archActual->nombre;
            if (!archActual->extension.empty()) {
                archActual->rutaAbsoluta += "." + archActual->extension;
            }
            
            cout << VERDE << "Archivo '" << nombreOriginal;
            if (!extensionOriginal.empty()) {
                cout << "." << extensionOriginal;
            }
            cout << "' renombrado a '" << archActual->nombre;
            if (!archActual->extension.empty()) {
                cout << "." << archActual->extension;
            }
            cout << "' exitosamente" << RESET << endl;
            
            return true;
        }
        anteriorArch = archActual;
        archActual = archActual->siguiente;
    }

    cout << ROJO <<"ERROR: No existe '" << nombreActual << "' en este directorio" << RESET <<endl;
    return false;
}
// ----- NANO -----
// Función para editar archivos usando nano
bool nanoArchivo(const string& nombreCompleto) {
    if (directorioActual == nullptr) {
        cout << ROJO << "ERROR: No se puede editar en la ubicación actual" << RESET << endl;
        return false;
    }

    if (nombreCompleto.empty()) {
        cout << ROJO << "ERROR: Nombre de archivo no puede estar vacío" << RESET << endl;
        return false;
    }
    // Validar extensión .txt
    size_t punto = nombreCompleto.find_last_of('.');
    if (punto == string::npos) {
        cout << ROJO << "ERROR: El archivo debe tener extensión .txt" << RESET << endl;
        return false;
    }
    
    string extension = nombreCompleto.substr(punto);
    if (extension != ".txt") {
        cout << ROJO << "ERROR: Solo se permiten archivos con extensión .txt" << RESET <<endl;
        return false;
    }

    string nombreBase = (punto != string::npos) ? nombreCompleto.substr(0, punto) : nombreCompleto;
 
    // Buscar el archivo en el directorio actual
    Archivo* arch = buscarArchivo(directorioActual, nombreBase);
    if (!arch) {
        cout << "nano: " << nombreCompleto << ": No existe el archivo" << endl;
        return false;
    }

    // Construir ruta completa del archivo
    //string rutaCompleta = directorioActual->rutaAbsoluta + nombreCompleto;
    
    // Leer contenido actual del archivo
    ifstream archivoEntrada(nombreCompleto);
    string contenido;
    string linea;
    
    if (archivoEntrada.is_open()) {
        //cout<<"ENTRA ARCHIVO ENTRADA"<<endl;
        while (getline(archivoEntrada, linea)) {
            cout<<"LINEA: "<<linea<<endl;
            contenido += linea + "\n";
        }
        archivoEntrada.close();
    }
    
    // Mostrar contenido actual
    cout << "Editando: " << nombreCompleto << endl;
    cout << "Contenido actual:" << endl;
    cout << "----------------------------------------" << endl;
    if (!contenido.empty()) {
        cout << contenido;
    } else {
        cout << "(archivo vacío)";
    }
    cout << endl << "----------------------------------------" << endl;
    
    // Permitir al usuario ingresar nuevo contenido
    cout << "Ingrese el nuevo contenido (escriba ':wq' en una línea nueva para guardar y salir):" << endl;
    
    string nuevoContenido;
    string inputLine;
    
    // Leer hasta que el usuario ingrese ":wq"
    while (true) {
        getline(cin, inputLine);
        
        if (inputLine == ":wq") {
            break;
        }
        
        nuevoContenido += inputLine + "\n";
    }
    
    // Guardar el nuevo contenido en el archivo
    ofstream archivoSalida(nombreCompleto, ios::app);  // Modo append
    if (archivoSalida.is_open()) {
        archivoSalida << nuevoContenido;
        archivoSalida.close();
        cout << VERDE << "Se editó " << nombreCompleto << " exitosamente" << RESET << endl;
    } else {
        cout << ROJO << "ERROR: No se pudo abrir el archivo para escritura" << RESET << endl;
        return false;
    }
    
    return true;
}

void guardarDirectorioEnArchivo(Directorio* dir, ofstream& archivoSalida) {
    if (dir == nullptr) return;

    // Guardar este directorio (excepto la raíz)
    if (dir != raiz) {
        string rutaDir = dir->rutaAbsoluta;
        // Eliminar la barra final si existe
        if (!rutaDir.empty() && rutaDir.back() == '/') {
            rutaDir.pop_back();
        }
        archivoSalida << rutaDir << endl;
    }

    // Guardar todos los archivos en este directorio
    Archivo* arch = dir->primerArchivo;
    while (arch != nullptr) {
        archivoSalida << arch->rutaAbsoluta << endl;
        arch = arch->siguiente;
    }

    // Guardar recursivamente los subdirectorios
    Directorio* subdir = dir->primerSubdir;
    while (subdir != nullptr) {
        guardarDirectorioEnArchivo(subdir, archivoSalida);
        subdir = subdir->siguiente;
    }
}

void guardarSistemaArchivos(const string& nombreArchivo) {
    ofstream archivoSalida(nombreArchivo);
    if (!archivoSalida) {
        cout << ROJO << "ERROR: No se pudo abrir el archivo para guardar: " << nombreArchivo << RESET << endl;
        return;
    }

    guardarDirectorioEnArchivo(raiz, archivoSalida);
    archivoSalida.close();
    
    cout << "Se guardó el estado del sistema de archivos en '" << nombreArchivo << "'" << endl;
}
// ----- PROGRAMA PRINCIPAL -----

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    Entrada();
    system("pause");
    limpiarPantalla();

    inicializarSistema(); 

    const string archivoEntrada = "Ejemplo1.txt";
    directorioActual = raiz;

    cout << "Cargando estructura desde '" << archivoEntrada << "'..." << endl;

    if (!cargarDesdeArchivo(archivoEntrada)) {
        cout << "ADVERTENCIA: No se pudo cargar el archivo inicial" << endl;
        cout << "             Recuerde agregarlo al output\n" << endl;
        cout << "Sus directorios inicializaran vacios\n" << endl;
    }
    //mostrarSistemaArchivos();  // Mostrar estructura inicial
    string comando;
    
    while (true) {
        mostrarPrompt();
        getline(cin, comando);
        
        comando.erase(0, comando.find_first_not_of(" \t"));
        comando.erase(comando.find_last_not_of(" \t") + 1);
        
        if (comando.empty()) continue;
        // Procesar comando mkdir
        if (comando.find("mkdir ") == 0) {
            string nombreDir = comando.substr(6);
            nombreDir.erase(0, nombreDir.find_first_not_of(" \t"));
            if (!nombreDir.empty()) {
                crearDirectorioCmd(nombreDir);
            } else {
                cout << ROJO << "ERROR Nombre de directorio no válido" << RESET << endl;
            }
        }
        // Procesar comando cd
        else if (comando.find("cd ") == 0) {
            string ruta = comando.substr(3);
            ruta.erase(0, ruta.find_first_not_of(" \t"));  
            if (!cambiarDirectorio(ruta)) {
                cout << "cd: " << ruta << ": No existe el directorio" << endl;
            }
        }
        // Procesar comando rm
        else if (comando.find("rm ") == 0) {
            string nombre = comando.substr(3);
            nombre.erase(0, nombre.find_first_not_of(" \t"));
            if (!nombre.empty()) {
                eliminarArchivoODirectorio(nombre);
            } else {
                cout << ROJO << "ERROR Nombre no válido" << RESET <<endl;
            }
        }
        // Procesar comando ls
        else if (comando == "ls") {
            listarDirectorioActual();
        }
        // Procesar comando pwd
        else if (comando == "pwd") {
            pwd();
        }
        // Procesar comando touch
        else if (comando.find("touch ") == 0) {
            string nombreArchivo = comando.substr(6);
            nombreArchivo.erase(0, nombreArchivo.find_first_not_of(" \t"));
            if (!nombreArchivo.empty()) {
                crearArchivoCmd(nombreArchivo);
            } else {
                cout << ROJO << "ERROR: Nombre de archivo no válido" << RESET << endl;
            }
        }// Comando NANO
        else if (comando.find("nano ") == 0) {
            string nombreArchivo = comando.substr(5);
            nombreArchivo.erase(0, nombreArchivo.find_first_not_of(" \t"));
            if (!nombreArchivo.empty()) {
                nanoArchivo(nombreArchivo);
            } else {
                cout << ROJO << "ERROR: Nombre de archivo no válido" << RESET <<endl;
            }
        }
        else if (comando.find("mv ") == 0) {
            // Extraer argumentos
            string argumentos = comando.substr(3);
            size_t espacio = argumentos.find(' ');
            if (espacio == string::npos) {
                cout << ROJO << "ERROR: Formato incorrecto. Uso: mv <actual> <nuevo>" << RESET <<endl;
            } else {
                string nombreActual = argumentos.substr(0, espacio);
                string nuevoNombre = argumentos.substr(espacio + 1);
                
                // Eliminar espacios en blanco
                nombreActual.erase(0, nombreActual.find_first_not_of(" \t"));
                nombreActual.erase(nombreActual.find_last_not_of(" \t") + 1);
                nuevoNombre.erase(0, nuevoNombre.find_first_not_of(" \t"));
                nuevoNombre.erase(nuevoNombre.find_last_not_of(" \t") + 1);
                
                if (!nombreActual.empty() && !nuevoNombre.empty()) {
                    renombrarElemento(nombreActual, nuevoNombre);
                } else {
                    cout << ROJO << "ERROR: Nombre actual y nuevo nombre no pueden estar vacíos" << RESET <<endl;
                }
            }
        }
        else if (comando == "help") {
            Comandos();
        }
        else if (comando == "exit") {
            guardarSistemaArchivos(archivoEntrada);
            break;
        }
        else {
            cout << ROJO << " ERROR: El comando que ha ingresado es incorrecto" << RESET << endl;
            Comandos();
        }
    }
    
    // Limpiar memoria al salir
    destruirDirectorio(raiz);
    raiz = nullptr;
    directorioActual = nullptr;
    directorioPersonal = nullptr;
            
    return 0;
}
#include <iostream>  
#include <fstream>   
#include <string>    
#define WIN32_LEAN_AND_MEAN  
#include <windows.h>
#include "prints.h"
using namespace std;

// --------------------------
// ESTRUCTURAS DE DATOS
// --------------------------

// Estructura para archivos
struct Archivo {
    string nombre;
    string extension;
    Archivo* siguiente; // Siguiente archivo en el directorio
    string rutaAbsoluta;
    
    Archivo(const string& nombreCompleto, Archivo* sig = nullptr) : 
        siguiente(sig) {
        size_t punto = nombreCompleto.find_last_of('.');
        if (punto != string::npos) {
            nombre = nombreCompleto.substr(0, punto);
            extension = nombreCompleto.substr(punto + 1);
        } else {
            nombre = nombreCompleto;
            extension = "";
        }
    }
};

// Estructura para directorios
struct Directorio {
    string nombre;
    Directorio* padre;
    Directorio* primerSubdir;
    Archivo* primerArchivo;
    Directorio* siguiente;
    string rutaAbsoluta;
    
    Directorio(const string& nom, Directorio* p = nullptr) : 
        nombre(nom), padre(p), primerSubdir(nullptr), 
        primerArchivo(nullptr), siguiente(nullptr) {
        if (padre == nullptr) {
            rutaAbsoluta = "/";
        } else {
            rutaAbsoluta = padre->rutaAbsoluta;
            if (rutaAbsoluta.back() != '/') rutaAbsoluta += "/";
            rutaAbsoluta += nombre + "/";
        }
    }
    
    ~Directorio() {
        // Eliminar todos los subdirectorios
        Directorio* subdir = primerSubdir;
        while (subdir != nullptr) {
            Directorio* siguienteSubdir = subdir->siguiente;
            delete subdir;
            subdir = siguienteSubdir;
        }
        
        // Eliminar todos los archivos
        Archivo* archivo = primerArchivo;
        while (archivo != nullptr) {
            Archivo* siguienteArchivo = archivo->siguiente;
            delete archivo;
            archivo = siguienteArchivo;
        }
    }
    
    void agregarSubdirectorio(Directorio* nuevo) {
        nuevo->siguiente = primerSubdir;
        primerSubdir = nuevo;
    }
    
    void agregarArchivo(Archivo* nuevo) {
        nuevo->siguiente = primerArchivo;
        primerArchivo = nuevo;
    }
    
    Directorio* buscarSubdirectorio(const string& nombre) {
        Directorio* actual = primerSubdir;
        while (actual != nullptr) {
            if (actual->nombre == nombre) return actual;
            actual = actual->siguiente;
        }
        return nullptr;
    }
    
    Archivo* buscarArchivo(const string& nombre) {
        Archivo* actual = primerArchivo;
        while (actual != nullptr) {
            if (actual->nombre == nombre) return actual;
            actual = actual->siguiente;
        }
        return nullptr;
    }
};

// --------------------------
// VARIABLES GLOBALES
// --------------------------

Directorio* raiz = nullptr;
Directorio* directorioActual = nullptr;
Directorio* directorioPersonal = nullptr;

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
        raiz = new Directorio("");
        raiz->nombre = "/";
        
        Directorio* home = new Directorio("Home", raiz);
        raiz->agregarSubdirectorio(home);
        
        directorioPersonal = new Directorio("User", home);
        home->agregarSubdirectorio(directorioPersonal);
        
        directorioActual = directorioPersonal;
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
                if (actual->buscarArchivo(elementos[i])) continue;
                Archivo* nuevoArchivo = new Archivo(elementos[i]);
                nuevoArchivo->rutaAbsoluta = actual->rutaAbsoluta + elementos[i];
                actual->agregarArchivo(nuevoArchivo);
            }
        } else {
            // Es un directorio
            Directorio* subdir = actual->buscarSubdirectorio(elementos[i]);
            if (subdir == nullptr) {
                subdir = new Directorio(elementos[i], actual);
                actual->agregarSubdirectorio(subdir);
            }
            actual = subdir;
        }
    }

    delete[] elementos;
}

bool cargarDesdeArchivo(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    
    if (!archivo) {
        cout << "ERROR: No se encontró el archivo: " << nombreArchivo << endl;
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

    cout << "\n¡Proceso completado!" << endl;
    cout << "  - Lineas leídas: " << totalLineas << endl;
    cout << "  - Rutas procesadas: " << lineasProcesadas << endl;
    cout << "  - Lineas vacías ignoradas: " << (totalLineas - lineasProcesadas) << endl;
    cout << "" << endl;

    return true;
}

// --------------------------
// FUNCIONES DE COMANDOS
// --------------------------

bool cambiarDirectorio(const string& ruta) {
    if (directorioActual == nullptr) {
        directorioActual = raiz;
    }

    if (ruta.empty() || ruta == ".") {
        return true;
    }

    Directorio* nuevoDir = nullptr;

    if (ruta == "/") {
        nuevoDir = raiz;
    } 
    else if (ruta == "~") {
        nuevoDir = directorioPersonal;
    } 
    else {
        bool rutaAbsoluta = (ruta[0] == '/');
        Directorio* base = rutaAbsoluta ? raiz : directorioActual;

        int numPartes = 0;
        string* partes = dividirRuta(ruta, numPartes);
        if (partes == nullptr) return false;

        nuevoDir = base;

        for (int i = 0; i < numPartes; i++) {
            const string& p = partes[i];
            if (p == ".") continue;
            if (p == "..") {
                if (nuevoDir->padre != nullptr) nuevoDir = nuevoDir->padre;
            } else {
                Directorio* subdir = nuevoDir->buscarSubdirectorio(p);
                if (subdir == nullptr) {
                    delete[] partes;
                    return false;
                }
                nuevoDir = subdir;
            }
        }
        delete[] partes;
    }

    if (nuevoDir != nullptr) {
        directorioActual = nuevoDir;
        return true;
    }
    return false;
}

void mostrarPrompt() {
    if (directorioActual == nullptr) {
        cout << "/$ ";
        return;
    }

    string ruta = directorioActual->rutaAbsoluta;

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
}

void listarDirectorioActual() {
    if (directorioActual == nullptr) {
        cout << "ERROR No hay directorio actual definido" << endl;
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

bool crearDirectorio(const string& nombre) {
    if (directorioActual == nullptr) {
        cout << "ERROR No se puede crear en la ubicación actual" << endl;
        return false;
    }
    
    if (nombre.empty()) {
        cout << "ERROR Nombre no puede estar vacío" << endl;
        return false;
    }

    if (nombre.find('.') != string::npos) {
        cout << "ERROR No se permiten puntos en nombres de directorio" << endl;
        return false;
    }

    if (directorioActual->buscarSubdirectorio(nombre) != nullptr || 
        directorioActual->buscarArchivo(nombre) != nullptr) {
        cout << "ERROR Ya existe '" << nombre << "' en este directorio" << endl;
        return false;
    }

    Directorio* nuevoDir = new Directorio(nombre, directorioActual);
    directorioActual->agregarSubdirectorio(nuevoDir);
    
    cout << "Directorio '" << nombre << "' creado exitosamente" << endl;
    return true;
}

bool eliminarArchivoODirectorio(const string& nombre) {
    if (directorioActual == nullptr) {
        cout << "ERROR: No se puede eliminar en la ubicación actual" << endl;
        return false;
    }

    if (nombre.empty()) {
        cout << "ERROR: Nombre no puede estar vacío" << endl;
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
        
        delete dirActual;
        cout << "Directorio '" << nombre << "' eliminado exitosamente" << endl;
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
        cout << "Archivo '" << nombre << "' eliminado exitosamente" << endl;
        return true;
    }

    cout << "ERROR: No existe '" << nombre << "' en este directorio" << endl;
    return false;
}

bool crearArchivo(const string& nombreCompleto) {
    if (directorioActual == nullptr) {
        cout << "ERROR: No se puede crear archivos en la ubicación actual" << endl;
        return false;
    }

    if (nombreCompleto.empty()) {
        cout << "ERROR: El nombre del archivo no puede estar vacío" << endl;
        return false;
    }

    if (nombreCompleto.find('.') == string::npos) {
        cout << "ERROR: El nombre debe contener una extensión (ej: archivo.txt)" << endl;
        return false;
    }

    // Extraer nombre base (sin extensión)
    size_t punto = nombreCompleto.find_last_of('.');
    string nombreBase = nombreCompleto.substr(0, punto);

    // Verificar si ya existe como directorio o archivo
    if (directorioActual->buscarSubdirectorio(nombreBase) != nullptr || 
        directorioActual->buscarArchivo(nombreBase) != nullptr) {
        cout << "ERROR: Ya existe '" << nombreCompleto << "' en este directorio" << endl;
        return false;
    }

    Archivo* nuevoArchivo = new Archivo(nombreCompleto);
    nuevoArchivo->rutaAbsoluta = directorioActual->rutaAbsoluta + nombreCompleto;
    directorioActual->agregarArchivo(nuevoArchivo);
    
    cout << "Archivo '" << nombreCompleto << "' creado exitosamente" << endl;
    return true;
}

// --------------------------
// FUNCIÓN PRINCIPAL
// -------------------------

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

    if (!cargarDesdeArchivo("Ejemplo1.txt")) {
        cout << "ADVERTENCIA: No se pudo cargar el archivo inicial" << endl;
        cout << "             Recuerde agregarlo al output\n" << endl;
        return 1;
    }
    
    string comando;
    //estructura
    while (true) {
        mostrarPrompt();
        getline(cin, comando);
        
        comando.erase(0, comando.find_first_not_of(" \t"));
        comando.erase(comando.find_last_not_of(" \t") + 1);
        
        if (comando.empty()) continue;
        
        if (comando.find("mkdir ") == 0) {
            string nombreDir = comando.substr(6);
            nombreDir.erase(0, nombreDir.find_first_not_of(" \t"));
            if (!nombreDir.empty()) {
                crearDirectorio(nombreDir);
            } else {
                cout << "ERROR Nombre de directorio no válido" << endl;
            }
        }
        else if (comando.find("cd ") == 0) {
            string ruta = comando.substr(3);
            ruta.erase(0, ruta.find_first_not_of(" \t"));  
            if (!cambiarDirectorio(ruta)) {
                cout << "cd: " << ruta << ": No existe el directorio" << endl;
            }
        }
        else if (comando.find("rm ") == 0) {
            string nombre = comando.substr(3);
            nombre.erase(0, nombre.find_first_not_of(" \t"));
            if (!nombre.empty()) {
                eliminarArchivoODirectorio(nombre);
            } else {
                cout << "ERROR Nombre no válido" << endl;
            }
        }
        else if (comando == "ls") {
            listarDirectorioActual();
        }
        else if (comando == "pwd") {
            pwd();
        }
        else if (comando.find("touch ") == 0) {
            string nombreArchivo = comando.substr(6);
            nombreArchivo.erase(0, nombreArchivo.find_first_not_of(" \t"));
            if (!nombreArchivo.empty()) {
                crearArchivo(nombreArchivo);
            } else {
                cout << "ERROR: Nombre de archivo no válido" << endl;
            }
        }
        else if (comando == "help") {
            Comandos();
        }
        else if (comando == "exit") {
            break;
        }
        else {
            
            cout << ROJO << " ERROR: El comando que ha ingresado es incorrecto" << RESET << endl;
            cout << "---------------------------------------------------" << endl;
            cout << "  Comandos disponibles:" << endl;
            cout << "  cd <ruta>       - Cambiar directorio" << endl;
            cout << "  cd .            - Quedarse en directorio actual " << endl;
            cout << "  cd /            - Ir al directorio raíz  " << endl;
            cout << "  cd ..           - Subir al directorio padre" << endl;
            cout << "  cd ~            - Ir a carpeta personal" << endl;
            cout << "  ls              - Listar contenidos" << endl;
            cout << "  mkdir <nombre>  - Crear nuevo directorio" << endl;
            cout << "  rm <nombre>     - Eliminar archivo/directorio" << endl;
            cout << "  pwd             - Mostrar directorio actual" << endl;
            cout << "  touch <nombre>  - Crear archivo tetxo plano" << endl;
            cout << "  exit            - Salir de la terminal" << endl;
            cout << "---------------------------------------------------" << endl;
        }
    }
    
    // Limpiar memoria al salir
    delete raiz;
            
    return 0;
}
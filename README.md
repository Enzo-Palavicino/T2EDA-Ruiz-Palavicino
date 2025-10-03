# Tarea 2 EDA – Instrucciones de Compilación y Ejecución
#### Integrantes
- Gonzalo Ruiz 
- Enzo Palavicino

## Requisitos previos
- Sistema operativo con soporte para compilación en C++ (Linux, macOS o WSL en Windows).  
- Compilador `g++`.  

## Directorio de trabajo
Es importante trabajar siempre dentro de la carpeta:

```bash
...T2EDA-Ruiz-Palavicino/cpp
```
De no ser asi, muevase con el comando `cd` en el terminal a la carpeta `cpp` ya que todos los comandos de  compilacion y ejecucion se haran aqui.

## Compliacion
El proyecto ya cuenta con un Makefile asi que para compilar en el terminal:
```bash
make
```
Con esto se genera la carpeta `obj/` para los archivos objeto intermedios y tambien genera el ejecutable principal poscodes.

## Ejecucion 
El archivo poscodes generado anteriormente necesita los archivos de datos, siendo estos `codes_500k.txt`, `codes_1M.txt`, `codes_10M.txt`, y por eso estos archivos de datos se tienen que encontrar denntro de la carpeta:
```bash
...T2EDA-Ruiz-Palavicino/cpp/build
```
Una vez que esten los datos en la carpeta, se corre el siguiente comando en el terminal:
```bash
./poscodes --data-dir build --runs 5
```
Este comando corre 5 los experimentos con 5 repeticiones para sus respectivos algoritmos, mostrando en el terminal, el Dataset de donde vienen, el algoritmo utilizado, su promedio de tiempo en segundos, su desviacion estandar respectiva en segundos y despues de mostrar todo eso, abajo de esa tabla muestra los tiempos de cada repeticion con su algoritmo y datos.

> Importante saber que en el comando de arriba se uso `--runs 5`, esto se hace porque en la tarea nos piden 5 repeticiones y se hace asi para correr las 5 repeticiones con un solo comando.

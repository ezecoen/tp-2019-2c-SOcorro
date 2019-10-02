# tp-2019-2c-SOcorro

## VALGRIND:

    valgrind --show-leak-kinds=all --leak-check=full -v ./(ejecutable)
  
  Para obtener todo el valgrind en un archivo de texto:
  
    valgrind --show-leak-kinds=all --leak-check=full -v --log-file="(nombreDelArchivo.txt)" ./(ejecutable)
  
  Ejemplo:
  
    valgrind --show-leak-kinds=all --leak-check=full -v --log-file="muse.txt" ./muse muse.config

  Helgrind:
  
    valgrind --tool="helgrind" --log-file="musehelgrind.txt" -v ./muse muse.config


## Para recordar el usuario en el repo:

    git config credential.helper store

## Para que funcione la shared library:
Muse:
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/tp-2019-2c-SOcorro/libreriaComun/Debug/
    
Prueba:
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/tp-2019-2c-SOcorro/memoria/libmuse/Debug/

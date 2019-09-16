# tp-2019-2c-SOcorro

## VALGRIND:

    valgrind --show-leak-kinds=all --leak-check=full -v ./(ejecutable)
  
  Para obtener todo el valgrind en un archivo de texto:
  
    valgrind --show-leak-kinds=all --leak-check=full -v --log-file="(nombreDelArchivo.txt)" ./(ejecutable)
  
  Ejemplo:
  
    valgrind --show-leak-kinds=all --leak-check=full -v --log-file="memoria.txt" ./memoria

  Helgrind:
  
    valgrind --tool="helgrind" --log-file="memoriahelgrind.txt" -v ./memoria memoria.config


## Para recordar el usuario en el repo:

    git config credential.helper store

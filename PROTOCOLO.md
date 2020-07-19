# Definición del protocolo

Para decir al servidor que función se realiza, se han utilizado combinaciones en binario para definir cada una de ellas:

```
#define LIST 100
#define GET 101 
#define SEND 110 
#define END 111
```

LIST se encarga de listar los ficheros que se encuentran en la carpeta de trabajo del servidor.
GET hace de nuevo LIST y te pide el fichero que quieres descargar.
SEND lista los ficheros de la carpeta del cliente donde se está ejecutando y te pide el fichero que quieres enviar al servidor.
END se encarga de cerrar el servidor y el cliente.

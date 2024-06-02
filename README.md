# Tarea1CDR
pasos(linux):

1. Una vez clonado el repositorio git a local

2. Acceda al directorio a travès del terminal
	ej:
	cd /home/pablo/tarea
	
3. Una vez aquì ejecute el comando "make" para generar los archivos bin de "cliente" y "servidor"

4. Posterior a que se hayan generado correctamente puede dar marcha al servidor:	
	ej:	./servidor_bin 5000  // 5000 = puerto a elecciòn , ejemplo de este caso
	
5. Luego, debe conectar al cliente de la siguiente manera
	ej: ./cliente_bin <ip> 5000
	
	
	
comentarios extra:

- cabe mencionar que los comandos del cliente y el servidor deben ejecutarse en terminales diferentes 
- este ip se puede conseguir por medio del comando : "hostname -I"

	

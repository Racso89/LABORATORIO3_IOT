Algoritmo ESP32
	Leer TEMPERATURA
	Leer PUERTA
	Leer RELE
	Leer X
	Si CONEXION Entonces
		Si TEMPERATURA>X Entonces
			Escribir RELEON
		SiNo
			Si TEMPERATURA<X Entonces
				Escribir RELEOFF
			SiNo
				Escribir PUBLICACIONGENERAL
				Escribir PUBTEM
				Escribir PUBHUM
				Si SERVO==ABRIR Entonces
					Escribir SERVO,90
					Escribir ABRE,PUERTA
				SiNo
					Si SERVO==CERRAR Entonces
						Escribir SERVO,0
						Escribir CIERRA,PUERTA
					SiNo
						Escribir PUBLICACIONGENERAL
						Leer X
						Si X>15 Entonces
							Escribir CONFIGURACION,TEMPERATURA
						SiNo
							Escribir PUBLICACIONGENERAL
						FinSi
					FinSi
				FinSi
			FinSi
		FinSi
	SiNo
		Escribir loop
		Escribir FUNCIONES
		Escribir CORREOPUERTA
		Escribir CORREOVENTILADOR
		Escribir CALLBACK
		Escribir WIFI
		Escribir RECONNECT
	FinSi
FinAlgoritmo

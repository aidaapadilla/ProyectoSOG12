#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <mysql.h>
#include <ctype.h>
#include <pthread.h>
//Conexion global BBDD
MYSQL *conn;

typedef struct {
	char nombre [20];
	int socket;
}Conectado;

typedef struct{
	Conectado conectados [100];
	int num;
} ListaConectados;

ListaConectados miLista;

//Consulta_1

void Consulta_1 (char respuesta[512], char nombre [20])
{
	// Consulta 1:
	// BUSCA LOS JUGADORES CON LOS QUE HA JUGADO LA PERSONA QUE INTRDUCES POR CONSOLA
	char consulta [512];
	int err;
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	
	
	printf("entro en 1\n");
	
	strcpy(consulta,"SELECT DISTINCT JUGADOR.ID FROM(PARTIDA, JUGADOR, CONNECTOR) WHERE PARTIDA.ID IN (SELECT PARTIDA.ID FROM(PARTIDA, JUGADOR, CONNECTOR) WHERE JUGADOR.ID= '");
	strcat(consulta,nombre);
	strcat(consulta,"' AND JUGADOR.ID= CONNECTOR.ID_J AND CONNECTOR.ID_P=PARTIDA.ID) AND PARTIDA.ID= CONNECTOR.ID_P AND CONNECTOR.ID_J=JUGADOR.ID AND JUGADOR.ID NOT IN ('");
	strcat(consulta,nombre);
	strcat(consulta,"')"); 
	
	printf("%s\n",consulta);
	// Establece connexión con la base de datos
	err=mysql_query (conn, consulta); 
	if(err!=0){
		printf("Error al consultar datos de la base \u0153u \u0153s\n", mysql_errno(conn),mysql_error(conn));
		exit(1);
	}
	//Retorna resultados de la consulta
	resultado = mysql_store_result (conn);
	row = mysql_fetch_row (resultado);
	if (row == NULL)
	{
		row[0]='0';
		sprintf (respuesta, "%s" , row[0]);
	}
	else
	{
		sprintf (respuesta, "%s", row[0]);
		printf("Entra al i=0\n");
		row = mysql_fetch_row (resultado);
		
		while (row!=NULL)
		{
			sprintf (respuesta, "%s,%s", respuesta, row[0]); //Encadenamos la respuesta ya que puede haber jugado con mas de una persona
			row = mysql_fetch_row (resultado);
		}
	}
	printf("%s\n",respuesta);
}

void Consulta_2 (char respuesta[512], char nombre [20])
{
	
	// Consulta 2
	
	char consulta[512];
	int err;
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	
	// BUSCA QUE JUGADOR QUE HA JUGADO EN LA PARTIDA INTRODUCIDA POR CONSOLA TIENE MAS PUNTOS ACUMULADOS
	printf("entro codigo2\n");
	
	strcpy(consulta,"SELECT DISTINCT JUGADOR.ID FROM(JUGADOR,CONNECTOR,PARTIDA) WHERE JUGADOR.TOTALPUNTS = (SELECT DISTINCT MAX(JUGADOR.TOTALPUNTS) FROM(JUGADOR, PARTIDA, CONNECTOR) WHERE CONNECTOR.ID_P = ");
	strcat(consulta,nombre);
	strcat(consulta," AND CONNECTOR.ID_J =JUGADOR.ID) AND JUGADOR.ID = CONNECTOR.ID_J AND CONNECTOR.ID_P = ");
	strcat(consulta,nombre);
	
	printf("%s\n",consulta);
	// Establece connexión con la base de datos
	err=mysql_query (conn, consulta); 
	if(err!=0){
		printf("Error al consultar datos de la base \u0153u \u0153s\n", mysql_errno(conn),mysql_error(conn));
		exit(1);
	}
	//Retorna resultados de la consulta
	resultado = mysql_store_result (conn);
	row = mysql_fetch_row (resultado);
	if (row == NULL)
	{
		printf("row null\n");
		row[0]='0';
		sprintf (respuesta, "%s" , row[0]);
	}
	else 
	{
		printf("todo ok\n");
		sprintf (respuesta, "%s", row[0]);
	}
	
}

void Consulta_3 (char respuesta[512], char nombre [20])
{
	// Consulta 3
	// BUSCA LA PARTIDA MÁS CORTA Y QUE SU TIEMPO ES INFERIOR A 1500s
	char consulta [512];
	int err;
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	
	printf("entro bien");
	strcpy (consulta,"SELECT PARTIDA.ID FROM(PARTIDA) WHERE PARTIDA.TEMPS =(SELECT MIN(PARTIDA.TEMPS) FROM PARTIDA) AND PARTIDA.TEMPS < 1500"); 
	
	// hacemos la consulta 
	err=mysql_query (conn, consulta); 
	if (err!=0) {
		printf ("Error al consultar datos de la base %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	//recogemos el resultado de la consulta 
	resultado = mysql_store_result (conn); 
	row = mysql_fetch_row (resultado);
	if (row == NULL){
		sprintf (respuesta,"No se han obtenido datos en la consulta");
		printf ("No se han obtenido datos en la consulta\n");
	}
	else{
		// El resultado debe ser una matriz con una sola fila
		// y una columna que contiene el nombre
		sprintf (respuesta,"ID de la partida mas corta: %s", row[0] );
		printf ("ID de la partida mas corta: %s\n", row[0] );
	}
	
}


//Devolver 0, 1 -1
int LogIn ( char nombre[20], char contrasenya [20], int sock_conn)
{	
	//Poner las variables locales
	int err;
	char consulta[512];
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	
	
	sprintf(consulta, "SELECT JUGADOR.CONTRASENYA FROM (JUGADOR) WHERE JUGADOR.ID ='%s'",nombre);
	
	//Realizamos la consulta
	err=mysql_query (conn, consulta); 
	if (err!=0) 
	{
		printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	//recogemos el resultado de la consulta 
	resultado = mysql_store_result (conn); 
	row = mysql_fetch_row (resultado);
	printf(row[0]);
	if (row == NULL)
	{
		return -1; //El usuario introducido no existe en la base de datos
		printf ("No se han obtenido datos en la consulta\n");
	}
	else
	{
		// El resultado debe ser una matriz con una sola fila
		// y una columna que contiene el nombre
		if (strcmp(row[0],contrasenya)==0)
		{	int resPon=0;
			resPon=Pon(&miLista, nombre, sock_conn); //No esta claro. Miguel
			if (resPon==-1)
			{	return -2;
				printf("Esta llena la lista");
		//sprintf(respuesta, "LLENO");
			}
			else
			{
			
			//sprintf(respuesta,"OK"); //El usuario es identificado en la base de datos
				return 0;
				printf("OK\n");
				printf("Se ha añadido bien");
			}
			//Añadir dame conectados en la CMD del server
			//printf(&miLista->conectados[sock_conn].nombre); //Miguel :C, 311-313 hauria de funcionar en if!=0 i l'hem hagut de ficar aqui perque sino no va :C Migueeeeeeeeeeeeeel :(((
			//printf ("Respuesta: %s\n", respuesta);
			/*						 Y lo enviamos*/
			//write (sock_conn,respuesta, strlen(respuesta));// socket de conexion, respuesta, longitud de la respuesta
				
		}
		
		
		else
		{
			return 1; //Existe el usuario introducido pero la contraseña proporcionada es incorrecta
			printf ("CONTRASENYA INCORRECTA\n");
		}
	}
	
}

int SignIn (char nombre[20], char contrasenya[20])
{
	
	int err;
	char consulta[512];
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	
	strcpy(consulta,"SELECT JUGADOR.CONTRASENYA FROM (JUGADOR) WHERE JUGADOR.ID = '");
	strcat(consulta,nombre);
	strcat(consulta,"'");
	printf("%s\n",consulta);
	
	//Realizamos la consulta
	err=mysql_query (conn, consulta); 
	if (err!=0) {
		printf ("Error al consultar datos de la base %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	//recogemos el resultado de la consulta 
	resultado = mysql_store_result (conn); 
	row = mysql_fetch_row (resultado);
	
	if (row == NULL){ //Si el usuario no existe procederá a registrarlo
		char consulta2 [1000];
		strcpy(consulta2,"INSERT INTO JUGADOR VALUES ('");
		strcat(consulta2,nombre);
		strcat(consulta2,"','");
		strcat(consulta2,contrasenya);
		strcat(consulta2,"',0)");
		printf("%s\n",consulta2);
		err=mysql_query (conn, consulta2); 
		if (err!=0) {
			return -1;
			printf ("Error al introducir los datos de la base %u %s\n",
					mysql_errno(conn), mysql_error(conn));
			exit (1);
		}
		//recogemos el resultado de la consulta 
		else{
			return 0; //Hemos podido añadir correctamente el usuario en la base de datos
			printf ("OK\n");
		}
	}
	else{
		return 1; //El usuario introducido ya existe en la base de datos, por lo tanto no lo añadimos
		printf ("YA EXISTE EL USUARIO\n");
	}
}


int Pon(ListaConectados *lista, char nombre [20], int socket){
	//Añade nuevos conectados. Retorna 0 si ok y -1 si la lista está llena (no puede añadir)
	if (lista->num==100)
		return -1;
	else{
		strcpy(lista->conectados[lista->num].nombre,nombre);
		lista->conectados[lista->num].socket=socket;
		lista->num++;
		return 0;
	}
}
int DamePosicion (ListaConectados *lista, char nombre [20]){
	//Devuelve la posicion en la lista o -1 si no está en la lista
	printf("Entro al DamePosicion\n");
	int i=0;
	int encontrado=0;
	printf("Lista num: %d\n", lista->num);
	while ((i<lista->num) && !encontrado)
	{	printf("Entra al While\n");
		if(strcmp(lista->conectados[i].nombre, nombre)==0)
		{encontrado=1;
			printf("Lista: %s",lista->conectados[i].nombre);}
		if (!encontrado)
			i=i+1;
		
	}
	if (encontrado==1)
		return i;
	else
		return -1;
	
}

int Elimina (ListaConectados *lista, char nombre[20])
	//Retorna 0 si elimina y -1 si ese usuario no está en la lista
{	printf("Acorralo. Entro en Elimina\n");
	int pos= DamePosicion (lista, nombre);
	if (pos==-1)
		return -1;
	else{
		int i;
		for (i=pos; i<lista->num-1; i++)
		{	lista->conectados[i] = lista->conectados [i+1];
		//strcpy(lista->conectados[i].nombre=lista->conectados[i+1].nombre);
		//lista->conectados[i].socket=lista->conectados[i+1].socket
		}
		lista->num--;
		return 0;
	}
}

void DameConectados (ListaConectados *lista, char listaconectados [300]){
	//Pone en conectados los nombres de todos los conectados separados
	//por /. Primero pone el número de conectados. Ejemplo: "3/Juan/Maria/Pedro"
	printf("Entro en la funcion DameConectados \n");
	sprintf (listaconectados, "%d", lista->num);
	int i;
	for (i=0; i<lista->num;i++)
		sprintf (listaconectados, "%s/%s", listaconectados, lista->conectados[i].nombre);
	
}





//Funcion abrir base de datos y que el main la llame


int contador;
//Estructura necesaria para acceso excluyente
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;

void *AtenderCliente (void *socket)
{	
	int err;
	// Estructura especial para almacenar resultados de consultas 
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	char consulta [80];
	//Creamos una conexion al servidor MYSQL 
	conn = mysql_init(NULL);
	if (conn==NULL) {
		printf ("Error al crear la conexion: %u %s\n", mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	//inicializar la conexion
	conn = mysql_real_connect (conn, "localhost","root", "mysql", "db",0, NULL, 0);
	if (conn==NULL) {
		printf ("Error al inicializar la conexion: %u %s\n", mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	//PREGUNTAR ESTRUCTURA MIGUEL
	int sock_conn;
	int *s;
	s= (int *) socket;
	sock_conn= *s;
	
	//int socket_conn = * (int *) socket;
	
	char peticion[512];
	char respuesta[512];
	/*	char consulta [512];*/
	char nombre [20];
	/*	int err;*/
	int ret;
	/*	MYSQL_ROW row;*/
	/*	MYSQL_RES *resultado;*/
	/*	MYSQL *conn;*/
	
	int terminar =0;
	// Entramos en un bucle para atender todas las peticiones de este cliente
	//hasta que se desconecte
	
	// Entramos en un bucle para atender todas las peticiones de este cliente
	//hasta que se desconecte
	while (terminar ==0)
	{
		// Ahora recibimos la petici?n
		ret=read(sock_conn,peticion, sizeof(peticion));
		printf ("Recibido\n");
		
		// Tenemos que a?adirle la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		peticion[ret]='\0';
		
		printf ("Peticion: %s\n",peticion);
		
		// vamos a ver que quieren
		char *p = strtok( peticion, "/");
		int codigo =  atoi (p);
		printf("numero de codigo %d\n",codigo);
		// Ya tenemos el c?digo de la petici?n
		char nombre[20];
		if ( codigo!=3 && codigo!=0 && codigo!=6 && codigo!=8)
		{
			printf("entro codigo dif0\n");
			p = strtok( NULL, "/"); //segundo trozo
			strcpy (nombre, p);
			// Ya tenemos el nombre
			printf ("Codigo: %d, Nombre: %s\n", codigo, nombre);
		}
		if (codigo==0) //Desconexión
		{
			terminar=1;
		}
		else if (codigo==8)
		{
			sprintf(respuesta,"%d",contador);
			printf("He entrado en codigo==8\n");
		}
		else if (codigo ==7) //LogOut
		{
			printf("Me estoy desconectando (==0)\n");
			int resElimina=Elimina(&miLista, nombre);
			printf("resElimina %d", resElimina);
			if (resElimina==0)
				printf("Se ha eliminado correctamente\n");
			terminar=1;
		
			
		}
		
		
		else if (codigo ==1)
		{
			// Consulta 1:
			// BUSCA LOS JUGADORES CON LOS QUE HA JUGADO LA PERSONA QUE INTRDUCES POR CONSOLA
			Consulta_1 (respuesta, nombre);
		}
		
		else if (codigo ==2) 
		{
			// Consulta 2
			// BUSCA QUE JUGADOR QUE HA JUGADO EN LA PARTIDA INTRODUCIDA POR CONSOLA TIENE MAS PUNTOS ACUMULADOS
			Consulta_2(respuesta, nombre);
			
		}
		else if (codigo ==3)
		{
			// Consulta 3
			// BUSCA LA PARTIDA MÁS CORTA Y QUE SU TIEMPO ES INFERIOR A 1500s
			Consulta_3(respuesta,nombre);
		}
		
		else if (codigo==4) 
			// LOG IN
			// NOS PERMITE COMPROBAR SI EL USUARIO Y LA CONTRASEÑA INTRODUCIDOS EXISTEN EN LA BASE DE DATOS, EN CASO CONTRARIO NOS AVISA
		{
			p = strtok( NULL, "/"); //Cogemos contraseña
			char contrasenya [20];
			strcpy (contrasenya, p);
			printf("%s\n",nombre);
			printf("%s\n",contrasenya);
			
			int resLogIn=LogIn(nombre, contrasenya, sock_conn);
			if (resLogIn==-1)
				sprintf(respuesta, "NO EXISTE");
			else if (resLogIn==-2)
				sprintf(respuesta, "LLENO");
			else if (resLogIn==0)
			{	sprintf(respuesta, "OK");
				//write (sock_conn,respuesta, strlen(respuesta));
			}
			else if (resLogIn==1)
				sprintf(respuesta, "CONTRASEÑA INCORRECTA");
				
		}
		
		else if (codigo==5) //SIGN IN
		{
			p = strtok( NULL, "/"); //Cogemos contraseña
			char contrasenya [20];
			strcpy (contrasenya, p);
			
			int resSignIn=0;
			resSignIn= SignIn(nombre, contrasenya);
			
			if (resSignIn==-1)
				sprintf(respuesta,"ERROR");
			
			else if (resSignIn==0)
				sprintf (respuesta,"OK");
			
			else if (resSignIn==1)
				sprintf (respuesta,"YA EXISTE");
		}
		
		else if (codigo==6) //No entra aqui
		{	printf("Ya estoy en codigo=6\n");
			char misConectados[300];
			
			
			respuesta[0]='\0';
			printf("Antes de hacer la llamada\n");
			DameConectados(&miLista,misConectados);
			printf("Estos son: %s\n",misConectados);
			sprintf(respuesta,misConectados);
			
			printf("Resultado: %s\n", misConectados);
			
/*			char *p = strtok (misConectados, "/");*/
			
/*			int n =atoi(p);*/
/*			int i;*/
/*			for (i=0; i<n; i++){*/
/*				char nombre [20];*/
/*				p=strtok (NULL, "/");*/
/*				strcpy (nombre,p);*/
				//printf ("Conectado: %s\n", nombre);
				
				
/*			}*/
		}
		
		if (codigo != 0) 
		{
			//Envia el mensaje a mi consola en C#
			printf("Respuesta: %s\n", respuesta);
			// Y lo enviamos
			write(sock_conn,respuesta, strlen(respuesta));// socket de conexion, respuesta, longitud de la respuesta
		}
		//Solo en consultas
		if ((codigo==1) || (codigo==2) || (codigo==3) ||(codigo==6) )
		{
			pthread_mutex_lock( &mutex ) ; //Thread dice: No me interrumpas ahora
			contador=contador+1;
			pthread_mutex_unlock( &mutex); // Thread dice: Ya puedes interrumpirme
		}
	}
	close(sock_conn);
}
int main (int argc, char *argv[]) 
{
	
	//CONECTAR BASE DE DADES
	/*	MYSQL *conn;*/
	/*	int err;*/
	// Estructura especial para almacenar resultados de consultas 
	/*	MYSQL_RES *resultado;*/
	/*	MYSQL_ROW row;*/
	/*	char consulta [80];*/
	//Creamos una conexion al servidor MYSQL 
	/*	conn = mysql_init(NULL);*/
	/*	if (conn==NULL) {*/
	/*		printf ("Error al crear la conexion: %u %s\n", mysql_errno(conn), mysql_error(conn));*/
	/*		exit (1);*/
	/*	}*/
	//inicializar la conexion
	/*	conn = mysql_real_connect (conn, "localhost","root", "mysql", "db",0, NULL, 0);*/
	/*	if (conn==NULL) {*/
	/*		printf ("Error al inicializar la conexion: %u %s\n", mysql_errno(conn), mysql_error(conn));*/
	/*		exit (1);*/
	/*	}*/
	
	//ListaConectados miLista;
	miLista.num=0; 
	// CONECTAR SERVIDOR-CLIENT
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char peticion[512]; //peticion buff
	char respuesta[512]; //respuesta buff2
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) //crear socket de escucha
		printf("Error creant socket");
	// Fem el bind al port
	contador=0;
	
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY); //indicamos a cualquier
	// escucharemos en el port 9200
	serv_adr.sin_port = htons(9276); 
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0) //asociamos al socket las esecificaciones anteriores
		printf ("Error al bind\n");
	//La cola de peticiones pendientes no podr? ser superior a 4
	if (listen(sock_listen, 2) < 0) //lo ponemos en pasivo. EL 2 es el numero de objetos en cola
		printf("Error en el Listen");
	
	int i;
	int sockets[100];
	pthread_t thread;
	i=0;
	// Bucle infinito
	for (;;){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		//sock_conn es el socket que usaremos para este cliente
		
		sockets[i] =sock_conn;
		//sock_conn es el socket que usaremos para este cliente
		
		// Crear thead y decirle lo que tiene que hacer
		
		pthread_create (&thread, NULL, AtenderCliente,&sockets[i]);
		i=i+1;
		
	}
	// Se acabo el servicio para este cliente
	//close(sock_conn); 
	//mysql_close(conn); //Cerramos la base de datos
	//exit(0);
}




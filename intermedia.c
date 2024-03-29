#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>


int calculaAleatorios(int min, int max);
void manejadoraSomelier(int sig); //Manejadora del somelier
void manejadoraMozo(int sig); //Manejadora del mozo
void manejadoraPinche(int sig);//Manejadora de los pinches
void manejadoraJefeDeSala(int sig);//Manejadora del jefe de sala



int main(int argc, char *argv[]){

	struct sigaction somelier1;
	struct sigaction mPinches;
	struct sigaction mJefeDeSala;

	srand (time(NULL));

	printf("%d: Arrancando...\n",getpid()); //Este es el codigo del Chef
	
	int posicion = atoi(argv[1]);

	pid_t chef, somelier, jefeDeSala, pinches[posicion];

	somelier = fork();

	if(somelier == -1){

		perror("Error en la llamada al fork()");

	}else if(somelier == 0){ //codigo del somelier

		
		somelier1.sa_handler = manejadoraSomelier;
		
		sigaction(SIGUSR1, &somelier1, NULL);
		sigaction(SIGUSR2, &somelier1, NULL);

		for(;;) pause();
	}

	jefeDeSala = fork();

	if(jefeDeSala == -1){

		perror("Error en la llamada al fork()");

	}else if(jefeDeSala == 0){ //codigo del jefe de sala

		mJefeDeSala.sa_handler=manejadoraJefeDeSala;

		sigaction(SIGUSR1,&mJefeDeSala,NULL);

		for(;;) pause();

	}

	

	for(int i=0; i<posicion;i++){//creacion de tantos pinches como se diga

		pinches[i]=fork();

		if(pinches[i]==-1){

			perror("Error en la llamada al fork()");

		}else if(pinches[i]==0){//codigo del pinche
	
			mPinches.sa_handler = manejadoraPinche;

			sigaction(SIGUSR1, &mPinches, NULL);

			for(;;) pause();
			
		}	
	}

	

	srand(getpid());


	//codigo del chef

	sleep(3);

		
	if(calculaAleatorios(0, 1) == 0){

		printf("Chef: Faltan ingredientes\n-----Avisando al somelier-----\n");

		kill(somelier, SIGUSR1);

	}else{

		printf("Chef: Falta vino\n-----Avisando al somelier-----\n");

		kill(somelier, SIGUSR2);
			

	}
	
	int encontrado;
	int variable;

	wait(&encontrado);
		
	variable =WEXITSTATUS(encontrado);

	if(variable==1){

		printf("Chef: Falta vino no puedo abrir el restaurante\n-----Cerrando el restaurante-----\n");

		
		kill(jefeDeSala,SIGTERM);
		for(int i=0;i<posicion;i++){
			kill(pinches[i],SIGTERM);
		}
		
	}else if(variable==2){

		printf("Chef: Me faltan ingredientes\n-----Avisando a los pinches-----\n");
		
		for(int i=0; i<posicion; i++){
			kill(pinches[i],SIGUSR1);
		}
	}else{

		printf("Chef: No falta nada\n-----Avisando a los pinches-----\n");

		for(int i=0; i<posicion; i++){

			kill(pinches[i],SIGUSR1);
		}
	}

	int devuelto;
	int suma=0;
	
	for(int i=0; i<posicion;i++){
			
		wait(&devuelto);
		suma = suma + WEXITSTATUS(devuelto);
		int variable4= WEXITSTATUS(devuelto);

		if(variable4==0){
	
			printf("Soy el pinche %d, y el plato lo he preparado mal\n",i);

		}else{

			printf("Soy el pinche %d, y el plato lo he preparado bien\n",i);

		}
			
	}

	if(suma==0){
		
		printf("Pinches:No hemos cocinado ni un plato bien\n-----Avisando al Chef-----\n");

		printf("Chef: No tengo platos cocinados\n-----Cerrando el programa-----\n");

		kill(somelier,SIGTERM);
			
		for(int i=0; i<posicion;i++){
			kill(pinches[i],SIGTERM);
		}
		kill(jefeDeSala,SIGTERM);

		}else{

			printf("Pinches: Hemos cocinado bien al menos un plato\n-----Avisando al Chef-----\n");

			printf("Chef: Tengo al menos un plato bien cocinado\n---Avisando al jefe de sala de que monte las mesas---\n");

			kill(jefeDeSala,SIGUSR1);
	
		}

		int mesa;
		int variable3;

		wait(&mesa);

		variable3=WEXITSTATUS(mesa);

		if(variable3==1){

			printf("Chef: PUEDE ABRIRSE EL RESTAURANTE\n-----Cerrando el programa-----\n");

			kill(somelier,SIGTERM);
			kill(jefeDeSala,SIGTERM);

			for(int i=0; i<posicion;i++){
			kill(pinches[i],SIGTERM);

			}
		}
	
		
}

void manejadoraSomelier(int signal){
	
	int encontrado;
	int variable1;

	pid_t mozo;

	mozo = fork();

	struct sigaction sMozo;
	sMozo.sa_handler=manejadoraMozo;

	if(mozo==-1){

		perror("error en la llamada al fork()");

	}else if(mozo == 0){ // codigo del mozo

		sigaction(SIGPIPE, &sMozo, NULL);
		
		for(;;) pause();
		
	}else{ //codigo del somelier


		if(signal==SIGUSR1){

			printf("Somelier: Faltan ingredientes\n-----Avisando al mozo-----\n");

		}else if(signal==SIGUSR2){

			printf("Somelier: Falta vino\n-----Avisando al mozo-----\n");			

		}

		sleep(2);		

		kill(mozo,SIGPIPE);

		

		
		
		waitpid(mozo,&encontrado,0); 		
		variable1=WEXITSTATUS(encontrado);

		if(variable1 == 0){ //Miro a ver lo que han encontrado los mozos
	
			if(signal == SIGUSR1){

				printf("Somelier: Los mozos no han encontrado los ingredientes\n-----Avisando al Chef-----\n");

				exit(2);

			}else if(signal == SIGUSR2){

				printf("Somelier: Los mozos no han encontrado el vino\n-----Avisando al Chef-----\n");
	
				exit(1);
			}

		}else{

			printf("Somelier: Los mozos han encontrado lo necesario\n-----Avisando al Chef-----\n");

			exit(3);
		}
		
		

	}

}

void manejadoraMozo(int signal){
	
	srand(getpid());

	if(calculaAleatorios(0,1)==0){

		printf("Mozo: No he encontrado lo que me pediste\n-----Avisando al somelier-----\n");

		exit(0);	
	}else{

		printf("Mozo: He encontrado lo que me pediste\n-----Avisando al somelier-----\n");

		exit(1);

	}

}

void manejadoraPinche(int signal){

	srand(getpid());

	sleep(calculaAleatorios(2,5));

	int devuelto = calculaAleatorios(0,1);

	exit(devuelto);

}

void manejadoraJefeDeSala(int signal){

	printf("Jefe de sala: Montando las mesas\n");

	sleep(3);

	printf("Jefe de sala: Mesas montadas\n-----Avisando al Chef-----\n");

	exit(1);

}


int calculaAleatorios(int min, int max){
	return rand() % (max-min+1) + min;
}

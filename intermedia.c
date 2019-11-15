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



int main(int argc, char *argv[]){

	int encontrado;

	struct sigaction somelier1={0};
	struct sigaction mPinches={0};

	srand (time(NULL));

	printf("%d: Arrancando...\n",getpid()); //Este es el codigo del Chef
	
	int posicion = atoi(argv[1]);

	pid_t chef, somelier, jefeDeSala, pinches[posicion];

	chef = getpid();

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

	}

	

	for(int i=0; i<posicion;i++){//creacion de tantos pinches como se diga

		pinches[i]=fork();

		int suma=0;

		if(pinches[i]==-1){

			perror("Error en la llamada al fork()");

		}else if(pinches[i]==0){//codigo del pinche
			
			mPinches.sa_handler = manejadoraPinche;

			sigaction(SIGUSR1, &mPinches, NULL);

			for(;;) pause();
			
		}	

		

	}

	

	srand(getpid());


	if(chef==getpid()){//codigo del chef

		sleep(3);

		
		if(calculaAleatorios(0, 1) == 0){

			kill(somelier, SIGUSR1);

		}else{

			kill(somelier, SIGUSR2);

		}
	
		int encontrado;
		int variable;

		waitpid(somelier,&encontrado,0);
		if(WIFEXITED(encontrado))
		variable =WEXITSTATUS(encontrado);

		printf("%d\n",variable);

		if(variable==1){

			printf("Chef: Falta vino no puedo abrir el restaurante\n");

			
			//aqui tengo que matar a todos los hijos
	
		}else if(variable==2){

			printf("Chef: Me faltan ingredientes\n");
		
			for(int i=0; i<posicion; i++){
				kill(pinches[i],SIGUSR1);
			}
		}else{

			printf("Chef: No falta nada\n");

			for(int i=0; i<posicion; i++){

				kill(pinches[i],SIGUSR1);
			}
		}

		
	}
		
}

void manejadoraSomelier(int signal){
	
	int encontrado;

	pid_t mozo;

	mozo = fork();

	struct sigaction sMozo = {0};
	sMozo.sa_handler=manejadoraMozo;

	if(mozo==-1){

		perror("error en la llamada al fork()");

	}else if(mozo == 0){ // codigo del mozo

		

		sigaction(SIGPIPE, &sMozo, NULL);		
	
		
	}else{ //codigo del somelier

		if(signal==SIGUSR1){

			printf("Faltan ingredientes\n");

			sleep(1);

			kill(mozo,SIGPIPE);
			
			

		}else if(signal==SIGUSR2){

			printf("Falta vino\n");

				sleep(1);

			kill(mozo,SIGPIPE);

		}

	int encontrado;
	int variable;
		wait(&encontrado); 
		
		variable=WEXITSTATUS(encontrado);

		printf("%d\n",variable);

		if(encontrado == 0){
	
			if(signal == SIGUSR1){

				printf("Somelier: Faltan ingredientes\n");

				exit(2);

			}else if(signal == SIGUSR2){

				printf("Somelier: Falta vino\n");
	
				exit(1);
			}

		}else{

			printf("No falta nada\n");

			exit(3);
		}

	}

}

void manejadoraMozo(int signal){

	srand(getpid());

	if(calculaAleatorios(0,1)==0){

		printf("No he encontrado lo que me pediste\n");

		exit(0);	
	}else{

		printf("He encontrado lo que me pediste\n");

		exit(1);

	}

}

void manejadoraPinche(int signal){

	int suma = calculaAleatorios(0,1);

	printf("%d\n",suma);

}


int calculaAleatorios(int min, int max){
	return rand() % (max-min+1) + min;
}

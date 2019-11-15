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



int main(int argc, char *argv[]){

	int encontrado;

	struct sigaction somelier1;

	srand (time(NULL));

	printf("%d: Arrancando...\n",getpid()); //Este es el codigo del Chef
	
	

	pid_t chef, somelier, jefeDeSala, pinche;

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

	srand(getpid());


	if(chef==getpid()){//codigo del chef

		sleep(3);

		
		if(calculaAleatorios(0, 1) == 0){

			kill(somelier, SIGUSR1);

		}else{

			kill(somelier, SIGUSR2);

		}

		
	}
		
}

void manejadoraSomelier(int signal){
	
	int encontrado;

	pid_t mozo;

	mozo = fork();

	struct sigaction sMozo;
	sMozo.sa_handler=manejadoraMozo;

	if(mozo==-1){

		perror("error en la llamada al fork()");

	}else if(mozo == 0){ // codigo del mozo

		int variable;

		sigaction(SIGPIPE, &sMozo, NULL);


		int encontrado;
		wait(&encontrado); 

		variable==WEXITSTATUS(encontrado);

		printf("%d\n",variable);

		if(encontrado == 0){

	

		}else if(encontrado == 1){

		
		}else{

			printf("No faltan ningun ingrediente\n");

		}
			
		
	}else //codigo del somelier

		if(signal==SIGUSR1){

			printf("Faltan ingredientes\n");

			sleep(1);

			kill(mozo,SIGPIPE);
			
			

		}else if(signal==SIGUSR2){

			printf("Falta vino\n");

				sleep(1);

			kill(mozo,SIGPIPE);

	}

}

void manejadoraMozo(int signal){

	if(calculaAleatorios(0,1)==0){

		printf("No he encontrado lo que me pediste\n");

		exit(0);	
	}else{

		printf("He encontrado lo que me pediste\n");

		exit(1);

	}

}

int calculaAleatorios(int min, int max){
	return rand() % (max-min+1) + min;
}

#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>




#define SEM_KEY 17122010	// a key to a set of semaphores (any number)
#define HOW_MANY 1		    // how many cycles to perform

int phi_id, proc_id, status, semaphor_id; //philosopher ii, process id, used for the last loop (signal end synchronization)




void start_philosopher(int phi_number) {
	printf("%i joins the Table \n", phi_number);

	int number = 0;
	int which = HOW_MANY;
	int hungry = 0;
	while (which) {
		if (hungry) {
			which--;
			number++;
			takeForks(phi_number, phi_number - 1);
			printf("%i START EATING %i MEAL \n", phi_number, number);
			sleep(1);
			leaveForks(phi_number, phi_number - 1);
			hungry = 0;
		}
		else {
			printf("%i THINKS AND BECOME HUNGRY \n", phi_number);
			sleep(2);
			hungry = 1;
		}
	}
}

void takeForks(int left_fork_id, int right_fork_id) {
	// take two forks right and left one
	//however in case of left fork zero, remember that right one is fork number 4
	if (left_fork_id == 0)
		right_fork_id = 4;
	printf("%i TRYS TO TAKE FORKS ==> %i L || %i R\n", left_fork_id, left_fork_id, right_fork_id);


	struct sembuf semaphor_fork[2] = { {right_fork_id, -1, 0}, {left_fork_id, -1, 0} }; 
	//struct sembuf { {semNumber, semOperation, operationFlags} }, semOperation > 0 | < 0 -increase/decrease sem by value, 
	//semOperation = 0 -wait for 0, sem_flg: 0 - blocking operation, IPC_NOWAIT - nonblocking operation
	
    	semop(semaphor_id, semaphor_fork, 2); 
	
	//semop() performs operations on selected semaphores in the set indicated by semid. 
	//Each of the nsops elements in the array pointed to by sops specifies an operation 
	//to be performed on a single semaphore. The elements of this structure are of type 
	//struct sembuf, containing the following members: 
}

void leaveForks (int left_fork_id, int right_fork_id) {
	// take two forks right and left one
	//however in case of left fork zero, remember that right one is fork number 4
	if (left_fork_id == 0)
		right_fork_id = 4;
	printf("%i LEAVE FORKS ==> %i L || %i R\n", left_fork_id, left_fork_id, right_fork_id);


	struct sembuf semaphor_fork[2] = { {right_fork_id, 1, 0}, {left_fork_id, 1, 0} }; //Same as above
    semop(semaphor_id, semaphor_fork, 2); //Same as above, but unblocking operation

}



int main() {

	int i = 0;

	semaphor_id = semget(SEM_KEY, 5, 0644 | IPC_CREAT); 
	//It may be used either
        //to obtain the identifier of a previously created semaphore set (when
        //semflg is zero and key does not have the value IPC_PRIVATE), or to
        //create a new set.
	
   	 //Grants access or creates permission for semaphores.
	if (semaphor_id == -1) {
		printf("\nERROR\n");
		exit(1);
	}

	while (i <= 4)
		semctl(semaphor_id, i++, SETVAL, 1); 
	
	   //semctl() performs the control operation specified by cmd on the
           //System V semaphore set identified by semid, or on the semnum-th
           //semaphore of that set.  

	   //	 SETVAL    Set the value of semval to arg.val for the semnum-th sema‐
           //      phore of the set, updating also the sem_ctime member of the
           //      semid_ds structure associated with the set.  Undo entries
           //      are cleared for altered semaphores in all processes.  If
           //      the changes to semaphore values would permit blocked
           //      semop(2) calls in other processes to proceed, then those
           //      processes are woken up.  The calling process must have
           //      alter permission on the semaphore set.
	
	
	for (int i = 0; i<5; i++)
	{
		proc_id = fork();
		//check for errors in childs creations
		if (proc_id < 0) {
			kill(-2, SIGTERM);
			printf("FORK ERROR \n");
			exit(1);
		}
		//in case of proper child creation
		else if (proc_id == 0) {
			phi_id = i;
			start_philosopher(i);
			printf("XXX %i has done! XXX \n", phi_id);
			return 0;
		}
	}

	//wait till all processes are synchronised
	while (1) {
		proc_id = wait(&status);
		if (proc_id< 0)
			break;
	}

	i = 0;
	// Deallocation of semaphores:
	if (semctl(semaphor_id, 0, IPC_RMID, 1)<0)
		printf("ERROR deallocationg semaphores.\n");
	
	
	 // semctl() performs the control operation specified by cmd on the
         // System V semaphore set identified by semid, or on the semnum-th
         // semaphore of that set.  (The semaphores in a set are numbered
         // starting at 0.)

	 //IPC_RMID  Immediately remove the semaphore set, awakening all pro‐
         //        cesses blocked in semop(2) calls on the set (with an error
         //        return and errno set to EIDRM).  The effective user ID of
         //        the calling process must match the creator or owner of the
         //        semaphore set, or the caller must be privileged.  The argu‐
         //        ment semnum is ignored.
	
	
	return 0;
}













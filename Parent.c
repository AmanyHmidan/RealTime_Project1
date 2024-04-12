
#include <GL/glut.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define NUM_CHILDREN 12
#define MAX_ENERGY 100
#define MIN_ENERGY 90
#define BALL_SPEED 50

#define FIFO_NAME "/tmp/my_fifo"

#define FIFO_NAME_LEADER_A "/tmp/my_fifo_leader_a"
#define FIFO_NAME_LEADER_B "/tmp/my_fifo_leader_b"

pid_t child_pids[NUM_CHILDREN];

// Global variables to keep track of ball positions
GLfloat ballAPositionX = -0.6; // Initial position for ball of Team A leader
GLfloat ballAPositionY = 0.8;
GLfloat ballBPositionX = 0.6; // Initial position for ball of Team B leader
GLfloat ballBPositionY = 0.8;

//------------------------------------------------------
void startGame();
void terminate_players(pid_t player_pids[], int num_players);

int main(int argc, char **argv) {


  // Ensure the correct number of arguments are provided
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    char *filename = argv[1]; // Get the filename from command-line arguments

    
    

  printf("Hello from Parent!\n");

  int i;

  // Seed the random number generator only once in the parent process
  srand(time(NULL));

  if (access(FIFO_NAME, F_OK) == 0) {
    // Remove existing FIFO if it already exists
    if (unlink(FIFO_NAME) == -1) {
      perror("Error removing existing FIFO");
      exit(-1);
    }
  }

  // Create the FIFO if it doesn't already exist
  /*if (mkfifo(FIFO_NAME, 0666) == -1) {
    perror("mkfifo");
    exit(EXIT_FAILURE);
  }*/

  // Create the FIFO for Team Leader A
  if (mkfifo(FIFO_NAME_LEADER_A, 0666) == -1) {
    if (errno != EEXIST) {
      perror("mkfifo leader A");
      exit(EXIT_FAILURE);
    }
  }

  // Create the FIFO for Team Leader B
  if (mkfifo(FIFO_NAME_LEADER_B, 0666) == -1) {
    if (errno != EEXIST) {
      perror("mkfifo leader B");
      exit(EXIT_FAILURE);
    }
  }

  // Fork the 2 team leaders and 10 players
  for (i = 0; i < 12; i++) {
    pid_t pid = fork();

    if (pid < 0) {
      // Fork failed
      perror("fork");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
      // Child process

      char index_str[10]; // Buffer for index string
      snprintf(index_str, sizeof(index_str), "%d",
               i); // Convert index to string

      execlp("./player", "player", index_str, (char *)NULL);

      // If execlp returns, there was an error
      perror("execlp");
      exit(EXIT_FAILURE);
    } else {
      // Parent process
      child_pids[i] = pid;
    }
  }

 // sleep(1);
  //usleep(400000);
  startGame();

  //sleep(1);

    // drawer area start
    pid_t drawer;

    if((drawer = fork()) == -1){
        perror("The drawer fork error\n");
        exit(-1);
    }

    // To let the drawer leave the main code and go to drawer.c
    if(!drawer){
        execlp("./last", "last", filename, (char *) NULL);

        // If the program not have enough memory then will raise error
        perror("exec Failure\n");
        exit(-1);
    }


  // Parent process waits for all child processes to complete
  for (i = 0; i < NUM_CHILDREN; i++) {
       waitpid(child_pids[i], NULL, 0);
   }

  /*waitpid(child_pids[0], NULL, 0);
  waitpid(child_pids[6], NULL, 0);*/

  printf("children processes have completed.\n");

  //terminate_players(child_pids, 12);*/

  return 0;
}

void startGame() {
  // Open the FIFO for writing
  /*int fd = open(FIFO_NAME, O_RDWR); // O_WRONLY
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }*/
  // Open the FIFO for writing to Team Leader A
  int fdA = open(FIFO_NAME_LEADER_A, O_RDWR);
  if (fdA == -1) {
    perror("open leader A FIFO for writing");
    exit(EXIT_FAILURE);
  }

  // Open the FIFO for writing to Team Leader B
  int fdB = open(FIFO_NAME_LEADER_B, O_RDWR);
  if (fdB == -1) {
    perror("open leader B FIFO for writing");
    exit(EXIT_FAILURE);
  }

  // Send message to team leaders indicating that the game has started
  const char *messageA = "Game started!\nPass one ball to team leader A\n";
  const char *messageB = "Game started!\nPass one ball to team leader B\n";
  ssize_t bytes_written; // Declare bytes_written here

  // Write to Team Leader A's FIFO
  bytes_written = write(fdA, messageA, strlen(messageA));
  if (bytes_written == -1) {
    perror("write to leader A");
    exit(EXIT_FAILURE);
  }

  // Write to Team Leader B's FIFO
  bytes_written = write(fdB, messageB, strlen(messageB));
  if (bytes_written == -1) {
    perror("write to leader B");
    exit(EXIT_FAILURE);
  }

   sleep(1);
  // Send a signal to team leader A
  pid_t leaderA_pid = child_pids[0];
  printf("Sending SIGUSR1 to Team Leader A (PID: %d)\n", (int)leaderA_pid);
  if (kill(leaderA_pid, SIGUSR1) == -1) {
    perror("kill");
    exit(EXIT_FAILURE);
  }
  // close(fdA);


  // Send a signal to team leader B
  pid_t leaderB_pid = child_pids[6];
  printf("Sending SIGUSR1 to Team Leader B (PID: %d)\n", (int)leaderB_pid);
  if (kill(leaderB_pid, SIGUSR1) == -1) {
    perror("kill");
    exit(EXIT_FAILURE);
  }
  // close(fdB);
}

// Define the function to terminate the player processes
void terminate_players(pid_t player_pids[], int num_players) {
  for (int i = 0; i < num_players; i++) {
    // Send a SIGTERM signal to each player process
    if (kill(player_pids[i], SIGTERM) == -1) {
      perror("kill");
    }
  }
}

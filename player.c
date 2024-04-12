#include <GL/glut.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
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
#define max_ballNumber 20

#define FIFO_PREFIX "/tmp/fifo_player_"

#define FIFO_NAME_LEADER_A "/tmp/my_fifo_leader_a"
#define FIFO_NAME_LEADER_B "/tmp/my_fifo_leader_b"

int fdA;
int fdB;
int round_value = 0;

int playerIndex;    // current player index that is sent with exec
char fifo_name[30]; // Name of private FIFO

// Define enum for teams
typedef enum { TEAM_A, TEAM_B } Team;

// Define struct for player
typedef struct {
  int id;     // Player ID
  Team team;  // Team of the player
  int energy; // Energy level of the player
  pid_t pid;  // Process ID of the player
              // char fifo_name[50]; // Name of private FIFO
  int ballIndex;

  int possessionCount[30]; // Count of possessions for each ball
  int ballCounter;
} Player;

// Define struct for ball
typedef struct {
  Team possession; // Team that currently possesses the ball
  int playerID;    // id of the player currently possessing the ball
  bool dropped;    // Whether the ball has been dropped

} Ball;

// Global variables
Player player;
// Player team_leaders[2];
Ball *balls = NULL;          // Array to store balls dynamically
int initial_ball_count = 10; // Initial number of balls at the start of the game

// Function to generate a random energy level
int generateRandomEnergy() {
  return rand() % (MAX_ENERGY - MIN_ENERGY + 1) +
         MIN_ENERGY; // Generate a random number between MIN_ENERGY and
                     // MAX_ENERGY
}

void printPlayers(int i);
void signal_handler(int sig);
void passing_balls();
void passTo_otherTeam(int playerID, int ballNum);
void writeTo_nextLeader(int playerID, int otherleaderID, int ballNum);
int generateRandomPause(int energy);
double calculateDropProbability(int energy);
void check_hasNoBall(Team myTeam);

int main(int argc, char **argv) {
  // Dynamically allocate memory for balls array
  balls = (Ball *)malloc(sizeof(Ball) * initial_ball_count);

  // Register signal handler for SIGUSR1
  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGUSR1, &sa, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <index>\n", argv[0]);
    return EXIT_FAILURE;
  }

  playerIndex = atoi(argv[1]); // Convert the argument to an integer

  if (playerIndex < 0 || playerIndex > 11) {
    fprintf(stderr, "Invalid index: %d\n", playerIndex);
    return EXIT_FAILURE;
  }

  srand(time(NULL) ^
        (getpid() << 16)); // Seed with a combination of current time and PID

  // Generate a random energy level for each player and team leader
  int energy = generateRandomEnergy();

  if (playerIndex <= 5) {
    player.team = TEAM_A;
    player.id = playerIndex;
    player.energy = energy;
    player.pid = getpid(); // Use getpid() to get the PID of the child process

    player.ballIndex = -1;

    player.ballCounter = 0;

  } else if (playerIndex >= 6) {
    player.team = TEAM_B;
    player.id = playerIndex;
    player.energy = energy;
    player.pid = getpid(); // Use getpid() to get the PID of the child process

    player.ballIndex = -1;

    player.ballCounter = 0;
  }

  printPlayers(playerIndex);

  // Create FIFO for each player

  snprintf(fifo_name, sizeof(fifo_name), "/tmp/fifo%d", playerIndex);

  // Remove existing file if it exists
  if (access(fifo_name, F_OK) != -1) {
    unlink(fifo_name);
  }

  if (mkfifo(fifo_name, 0666) == -1) {
    perror("mkfifo");
    exit(EXIT_FAILURE);
  }

  printf("Created FIFO: %s\n", fifo_name);

  // Introduce a short delay after creating the FIFO
  usleep(100000); // Delay for 100 milliseconds (100,000 microseconds)

  if (playerIndex == 0 || playerIndex == 6) {
    pause();
  }

  passing_balls();

  return 0;
}

void printPlayers(int i) {

  if (i == 0) {
    printf("TeamLeader %d, teamA, with PID %d, has energy level: %d\n",
           player.id, (int)player.pid, player.energy);
  } else if (i == 6) {
    printf("TeamLeader %d, teamB, with PID %d, has energy level: %d\n",
           player.id, (int)player.pid, player.energy);
  } else if (i < 6) {
    printf("Player %d, teamA, with PID %d, has energy level: %d\n", player.id,
           (int)player.pid, player.energy);
  } else {
    printf("Player %d, teamB, with PID %d, has energy level: %d\n", player.id,
           (int)player.pid, player.energy);
  }
}

// Signal handler function
void signal_handler(int sig) {
  printf("Signal received from parent to %d\n", getpid());

  // Buffer to read the message from the FIFO
  char messageA[256];
  char messageB[256];
  ssize_t bytes_read;

  // TEAM leader A:
  if (playerIndex == 0) {
    // Open the FIFO for reading
    int fdA = open(FIFO_NAME_LEADER_A, O_RDWR);
    if (fdA == -1) {
      perror("open");
      exit(EXIT_FAILURE);
    }

    // Read the message from the FIFO
    bytes_read = read(fdA, messageA, sizeof(messageA));
    if (bytes_read < 0) {
      perror("read");
      exit(EXIT_FAILURE);
    } else if (bytes_read == 0) {
      fprintf(stderr, "No data available\n");
    } else {
      // Null-terminate the received message
      messageA[bytes_read] = '\0';

      // Print the received message
      printf("Player PID %d received messageA: %s\n", getpid(), messageA);
      balls[0].possession = TEAM_A;
      balls[0].playerID = 0;
      balls[0].dropped = 0;

      player.ballIndex = 0;

      player.possessionCount[0]++;
      player.ballCounter += 1;
      // printf("ballCounter %d\n", player.ballCounter);
    }

    // Close the FIFO
    close(fdA);
    unlink("/path/to/FIFO_NAME_LEADER_A"); // Delete the FIFO
  }

  // TEAM leader B:
  if (playerIndex == 6) {

    // Open the FIFO for reading
    int fdB = open(FIFO_NAME_LEADER_B, O_RDWR);
    if (fdB == -1) {
      perror("open");
      exit(EXIT_FAILURE);
    }

    // Read the message from the FIFO
    bytes_read = read(fdB, messageB, sizeof(messageB));
    if (bytes_read < 0) {
      perror("read");
      exit(EXIT_FAILURE);
    } else if (bytes_read == 0) {
      fprintf(stderr, "No data available\n");
    } else {
      // Null-terminate the received message
      messageB[bytes_read] = '\0';

      // Print the received message
      printf("Player PID %d received messageB: %s\n", getpid(), messageB);
      balls[1].possession = TEAM_B;
      balls[1].playerID = 6;
      balls[1].dropped = 0;

      player.ballIndex = 1;

      player.possessionCount[1]++;
      player.ballCounter += 1;
    }

    // Close the FIFO
    close(fdB);
    unlink("/path/to/FIFO_NAME_LEADER_B"); // Delete the FIFO
  }

  // Exit the process
  // exit(EXIT_SUCCESS);
}

void passing_balls() {

  int playerID = player.id;
  Team myTeam = player.team;
  int nextPlayerID;
  int flag_toLeader = 0; // when the ball is sent to the leader from last
                         // player, flag turns 1 and ball sent to other leader

  // printf("current playerid: %d\n", playerID);

  // Determine the next player based on current player's team and ID
  if (myTeam == TEAM_A) {
    // For Team A, the sequence is 0->1->2->3->4->5->0
    nextPlayerID = (playerID >= 0 && playerID < 5) ? playerID + 1 : 0;
  } else {
    // For Team B, the sequence is 6->7->8->9->10->11->6
    nextPlayerID = (playerID >= 6 && playerID < 11) ? playerID + 1 : 6;
  }

  while (round_value < 6) {
  
    if(round_value >2)
       check_hasNoBall(myTeam);

    int ballNum;
   

    round_value += 1;

    int random_pause = generateRandomPause(player.energy);
    double drop_prob = calculateDropProbability(player.energy);
    double drop_num = 0.5;

    /*if (drop_num < drop_prob) {
             random_pause = (int)(random_pause * 1.5); // ball droped
             usleep(random_pause);
          } else {
             usleep(random_pause); // Ball is not dropped
          }*/

    if (player.ballCounter != 0) {
    
   

      for (int k = 0; k < 3; k++) {
        if (player.possessionCount[k] != 0)
          ballNum = k;
        // printf("possessionCount for ball%d is %d\n", k,
        // player.possessionCount[k]);
      }

      // printf("ball count %d playerID %d,flag_toLeader %d \n", ballCount,
      // playerID,flag_toLeader);

      // usleep(random_pause);

      // Check if the player is a leader and if two passes have been made within
      // the same team
      if (playerID == 0 || playerID == 6) {

        if (flag_toLeader) {

          printf("entered pass\n");

          //player.possessionCount[ballNum] - 1;

          //player.ballCounter -= 1;

          flag_toLeader = 0;

          passTo_otherTeam(playerID, ballNum);

          // printf("after pass: playerID: %d, ballNum: %d , ballCount: %d\n",
          //  playerID, player.ballIndex, ballCount);

          // printf("ball count in 342 %d playerID %d\n",ballCount, playerID );
          //usleep(100000);
          continue;
        }
      }

      // Open FIFO for communication with the next player
      char NextFifo_name[50];
      snprintf(NextFifo_name, sizeof(fifo_name), "/tmp/fifo%d",
               nextPlayerID); // Get next child's FIFO name

      // Ensure the next player's FIFO exists
      if (access(NextFifo_name, F_OK) == -1) {
        fprintf(stderr, "Next player's FIFO %s does not exist.\n",
                NextFifo_name);
        exit(EXIT_FAILURE);
      }

      int fd = open(NextFifo_name, O_WRONLY);
      if (fd == -1) {
        perror("open for write");
        exit(EXIT_FAILURE);
      }

      // Send the ball number as a message to the next player
      char buffer[20];

      if (playerID == 5 || playerID == 11) {
        snprintf(buffer, sizeof(buffer), "last player, %d", ballNum);
      } else {
        // Convert integer to string
        snprintf(buffer, sizeof(buffer), "%d", ballNum);
      }

      ssize_t bytes_written = // Write the string to the FIFO
          write(fd, buffer,
                strlen(buffer) + 1); // +1 to include the null terminator

      // printf("Sending ball%d from player%d to player%d\n", ballNum, playerID,
      // nextPlayerID);

      player.ballIndex = -1;
      player.ballCounter -= 1;
      player.possessionCount[ballNum]--;

      if (bytes_written == -1) {
        perror("write");
        exit(EXIT_FAILURE);
      }

      // Close the FIFO
      // close(fd);
    }

    // Open the FIFO for reading (blocks until a writer is available)
    /*while (access(fifo_name, F_OK) == -1) {
      // Loop body
    }*/
    int fd = open(fifo_name, O_RDONLY) ;
    if (fd == -1) {
      perror("open for read");
      exit(EXIT_FAILURE);
    }
    
    //usleep(100000);

    // Read data from the FIFO
    char buffer[256];
    ssize_t bytes_read;
    bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    } else if (bytes_read == 0) {
      printf("No data available for player %d to read\n", playerID);
      continue;
    } else {
      // Null-terminate the received message
      // buffer[bytes_read] = '\0';

      int received_number;
      if (sscanf(buffer, "last player, %d", &received_number) == 1) {
        // Successfully extracted the ball number
        printf("Received message from the last player. Ball number: %d\n",
               received_number);
        flag_toLeader = 1;
      } else {
        // Convert string back to integer
        received_number = atoi(buffer);
      }

      printf("Received message: ball%d to player%d\n", received_number,
             playerID);

      player.ballIndex = received_number;
      player.ballCounter += 1;
      player.possessionCount[received_number]++;

      // printf("received_number = %d\n", received_number);
      balls[received_number].playerID = playerID;

      // Send a signal to the opengl program to resume ball movement
      // kill(main_pid, SIGUSR1); // Send SIGUSR1 signal to the opengl  program
    }

    // Close the FIFO
    if (close(fd) == -1) {
      perror("close");
      exit(EXIT_FAILURE);
    }
  }
}

void passTo_otherTeam(int playerID, int ballNum) {

  Team otherTeam;
  Team myTeam = player.team;
  int otherleaderID;
  // ballCount -= 1;
  // printf("ball count in pass %d playerID %d\n", ballCount, playerID);

  if (myTeam == TEAM_A) {
    otherTeam = TEAM_B;
  } else if (myTeam == TEAM_B) {
    otherTeam = TEAM_A;
  }

  if (playerID == 0) {
    otherleaderID = 6;
  } else if (playerID == 6) {
    otherleaderID = 0;
  }

  balls[ballNum].possession = otherTeam;
  balls[ballNum].playerID = otherleaderID;
  player.ballIndex =
      -1; // because it wants to give the ball to the other team leader

  // pass ballto other leader by sending ballNum as a message to his fifo
  writeTo_nextLeader(playerID, otherleaderID, ballNum);
}

void writeTo_nextLeader(int playerID, int otherleaderID, int ballNum ) {

  // Open FIFO for communication with the next player
  char NextFifo_name[50];
  snprintf(NextFifo_name, sizeof(fifo_name), "/tmp/fifo%d",
           otherleaderID); // Get next child's FIFO name

  // Ensure the next player's FIFO exists
  if (access(NextFifo_name, F_OK) == -1) {
    fprintf(stderr, "Next player's FIFO %s does not exist.\n", NextFifo_name);
    exit(EXIT_FAILURE);
  }

  // printf("aaa\n");
  int fd = open(NextFifo_name, O_WRONLY);

  if (fd == -1) {
    perror("open for write");
    exit(EXIT_FAILURE);
  }

  // Send the ball number as a message to the next player
  char buffer[20];
  // Convert integer to string
  snprintf(buffer, sizeof(buffer), "%d", ballNum);

  ssize_t bytes_written = // Write the string to the FIFO
      write(fd, buffer,
            strlen(buffer) + 1); // +1 to include the null terminator

  player.possessionCount[ballNum] -=1;

  player.ballCounter -= 1;

  printf("Sending ball%d from leader%d to leader%d\n", ballNum, playerID,
         otherleaderID);

  if (bytes_written == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }

  // Close the FIFO
  close(fd);
}

// Function to calculate the drop probability based on energy level using an
// exponential decay function
double calculateDropProbability(int energy) {
  double k = 0.05; // Decay constant

  // Calculate drop probability using the exponential decay equation
  double drop_probability = 100 * exp(-k * energy);

  // printf("drop prob%f\n", drop_probability);
  return drop_probability; // Convert to integer for simplicity in this context
}

// Function to generate a random pause duration based on player's energy
int generateRandomPause(int energy) {
  // Define the maximum and minimum pause durations
  const int MAX_PAUSE_DURATION =
      1000000; // Maximum pause duration (in microseconds)
  const int MIN_PAUSE_DURATION =
      500000; // Minimum pause duration (in microseconds)

  // Calculate the pause duration based on energy
  // Higher energy -> Shorter pause, Lower energy -> Longer pause
  int pause_duration =
      MIN_PAUSE_DURATION +
      energy * (MAX_PAUSE_DURATION - MIN_PAUSE_DURATION) / MAX_ENERGY;

  // Return the calculated pause duration
  return pause_duration;
}

void check_hasNoBall(Team myTeam){

   int flaghasBall = 0;
   
   for(int i=0; i<initial_ball_count; i++){
   
       if(balls[i].possession == myTeam){ 
          printf("ball%d possession = %d\n", i, (int)myTeam);
          flaghasBall = 1;
          break;
       }
   
   }
   
   if(flaghasBall == 0 ){
   //printf("hiii\n");
   // our team has no ball, send a new one 
   
     if( player.id == 0 || player.id == 6 ){
      balls[2].possession = myTeam;
      balls[2].playerID = player.id;
      balls[2].dropped = 0;
      player.ballIndex = 2;
      player.possessionCount[2]++;
      player.ballCounter += 1;
     
     }
   
      
   
   }



}

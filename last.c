#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <GL/glut.h>
#include <stdbool.h>
#include <string.h>
#define NUM_CHILDREN 12
//#define MAX_ENERGY 100
//#define MIN_ENERGY 80
//#define BALL_SPEED 50
#define MAX_BALLS 30
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h> 
int CountA=1;
int CountB=1;
bool Aputball = false;
bool Bputball = false;
int MAX_ENERGY ;
int MIN_ENERGY ;
int BALL_SPEED ;
int timer1 ; // 0,5 minute timer (in seconds)
int timer5 ; // 2,5 minute timer (in seconds)
char teamAScoresStr[10]; // Assuming scores are within 10 digits
char teamBScoresStr[10];
char teamAScoresStr2[10]; // Assuming scores are within 10 digits
char teamBScoresStr2[10];
char teamAScoresStr3[10]; // Assuming scores are within 10 digits
char teamBScoresStr3[10];

char teamAScoresStr4[10]; // Assuming scores are within 10 digits
char teamBScoresStr4[10];
char teamAScoresStr5[10]; // Assuming scores are within 10 digits
char teamBScoresStr5[10];


int teamAScores[] = {0, 0, 0, 0, 0}; // Assuming 5 rounds
int teamBScores[] = {0, 0, 0, 0, 0}; // Assuming 5 roundss

// Global variables to keep track of ball positions
GLfloat ballAPositionX = -0.3; // Initial position for ball of Team A leader
GLfloat ballAPositionY = 0.8;
GLfloat ballBPositionX = 0.6;  // Initial position for ball of Team B leader
GLfloat ballBPositionY = 0.8;

// Global variables to new ball
GLfloat NballAPositionX = -0.3; // Initial position for ball of Team A leader
GLfloat NballAPositionY = 0.8;
GLfloat NballBPositionX = 0.6;  // Initial position for ball of Team B leader
GLfloat NballBPositionY = 0.8;

         
GLfloat infoBoxStartX = 20.0f;
GLfloat infoBoxStartY = 260.0f; // Make sure this is within the visible range
GLfloat infoBoxWidth = 140.0f; // Just an example, adjust as needed
GLfloat infoBoxHeight = 60.0f; // Just an example, adjust as needed
bool flagfirstina = true;
bool flagfirstinb = true;
int moveCount = 0; // A counter for the number of moves made
int totalMoves = 4;
bool isWaitingForBallMove = false; // To control waiting state
int waitDuration = 3000; // 2000 milliseconds or 2 seconds wait
bool shouldMove = true; // Global flag to control the moving process
int BmoveCount = 0; // A counter for the number of moves made
int BtotalMoves = 4;
bool BisWaitingForBallMove = false; // To control waiting state
int BwaitDuration = 3500; // 2000 milliseconds or 2 seconds wait
bool BshouldMove = true; // Global flag to control the moving process
int test =1;
int BballAdjustCount = 0;
int AballAdjustCount = 0;
int roundCount = 0; // for count the rounds
int enrgywaitA = 0; // Global declaration
int enrgywaitB = 0; // Assuming you also have a global variable for B
GLint value [13] ; 
int pipefd[2];
int sumA = 0;
int sumB = 0;
int newballX = 0.3;
int newballY = 0.8;	
 char timer1Text[20];
 char timer5Text[20];
  int b =0;
//------------------------------------------------------


// Define enum for teams
typedef enum {
    TEAM_A,
    TEAM_B
} Team;

// Define struct for player
typedef struct {
    int id;          // Player ID
    Team team;       // Team of the player
    int energy;      // Energy level of the player
    pid_t pid;       // Process ID of the player
    int pipe[2];     // Pipe for communication with next player
  int wait;
} Player;

// Define struct for ball
typedef struct {
    Team possession;  // Team that currently possesses the ball
    bool dropped;     // Whether the ball has been dropped
    float positionX;
    float positionY;
} Ball;

// Global variables
Player players[10]; // 5 players for each team
Player team_leaders[2];
Ball balls[2];

// Function to generate a random energy level
int generateRandomEnergy() {
    return rand() % (MAX_ENERGY - MIN_ENERGY + 1) + MIN_ENERGY; // Generate a random number between MIN_ENERGY and MAX_ENERGY
}

int waitRandomEnergy() {
    return (rand() % ((MAX_ENERGY - MIN_ENERGY + 1) + MIN_ENERGY)*140); // Generate a random number between MIN_ENERGY and MAX_ENERGY
}

//------------------------------------------------


// Function prototypes
void init();
void display();
void drawTeam(GLfloat color[], GLfloat startX, GLfloat startY, int numMembers); 
void drawBall(GLfloat color[], GLfloat startX, GLfloat startY);
void updateBallPosition(); // Added semicolon here
void drawText(const char* text, int length, int x, int y);
void drawTable();
void drawBoldText(const char* text, int x, int y);
void drawInformationBox();
void drawText2(float x, float y, const char *string);
void drawMultipleLinesOfText(const char* text, GLfloat x, GLfloat y);
void updateTimers(int value);
void ddrawText(float x, float y, char *string);
void initializeEnergyWaits(void);
void resumeBallMovement(int value);
void BmoveBallAgain(int value);
void moveBallAgain(int value);
void idle();
void performActionAfterDelay(int value);
void glutSleep(int millisecondsDelay);
void drawintText(const int* text, int x, int y);
void drawTable();
void enableBallMovement(int value);
void newdrawBall(GLfloat color[], GLfloat startX, GLfloat startY);
void check();
void changeballBtoA(int value);
void changeballAtoB(int value);
void checkb();
void BchangeballBtoA(int value);
void BchangeballAtoB(int value);
void updateBallPosition();

Ball balls[2] = {
    {TEAM_A, false}, // Initialize ball for Team A
    {TEAM_B, false}  // Initialize ball for Team B
};
//---------------------------------------------------------------------------------------------

void idle() {
    glutPostRedisplay(); // This will ensure display() is called regularly
}
int main(int argc, char** argv) {

// Ensure the correct number of arguments are provided
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    char *filename = argv[1]; // Get the filename from command-line arguments

    // Access the file directly using the filename
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }

        // Read the file content or perform other operations
        fscanf(file, "MAX_ENERGY = %d\n", &MAX_ENERGY);
        fscanf(file, "MIN_ENERGY = %d\n", &MIN_ENERGY);
        fscanf(file, "BALL_SPEED = %d\n", &BALL_SPEED);
        fscanf(file, "timer1 = %d\n", &timer1);
        fscanf(file, "timer5 = %d\n", &timer5 );
    fclose(file);
    
    pid_t child_pids[NUM_CHILDREN];
    int i;
    //initializeEnergyWaits();
    
    // Seed the random number generator only once in the parent process
    srand(time(NULL));

    // Initialize the ball possession and drop status
    balls[0].possession = TEAM_A;
    balls[0].dropped = false;
    balls[1].possession = TEAM_B;
    balls[1].dropped = false;
   

    // Fork the 2 team leaders and players
    for (i = 0; i < 12; i++) {
    
	int wait = waitRandomEnergy();
	value[i] = wait;
        pid_t pid = fork();

        if (pid < 0) {
            // Fork failed
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
         
	srand(time(NULL) ^ (getpid() << 16)); // Seed with a combination of current time and PID

	  // Generate a random energy level for each player and team leader
       
            int energy = generateRandomEnergy();
          

            if (i == 0 || i == 6) {
                team_leaders[i].team = (i == 0) ? TEAM_A : TEAM_B;
                team_leaders[i].id = i;
                team_leaders[i].pid = getpid(); // Use getpid() to get the PID of the child process
                team_leaders[i].energy = energy;
              team_leaders[i].wait = wait;
                       
              
            } else if (i < 6) {
                // Players 1 to 5 for Team A
                players[i].team = TEAM_A;
                players[i].id = i;
                players[i].pid = getpid(); // Use getpid() to get the PID of the child process
                players[i].energy = energy;
              players[i].wait = wait;
            } else {
                // Players 1 to 5 for Team B
                players[i].team = TEAM_B;
                players[i].id = i - 6;
                players[i].pid = getpid(); // Use getpid() to get the PID of the child process
                players[i].energy = energy;
              players[i].wait = wait;
            }
            
         
           if(i==0 ){
          // value = team_leaders[i].wait;
           printf("teamLeader %d, teamA, with PID %d, has energy level: %d wait = %d\n",team_leaders[i].id,  (int)team_leaders[i].pid, team_leaders[i].energy,team_leaders[i].wait);
                    } else if (i==6){
           printf("teamLeader %d, teamB, with PID %d, has energy level: %d wait = %d\n",team_leaders[i].id,  (int)team_leaders[i].pid, team_leaders[i].energy,team_leaders[i].wait);
           }
	   else if(i < 6) {
	     printf("Player %d, teamA, with PID %d, has energy level: %d wait = %d \n",players[i].id,  (int)players[i].pid, players[i].energy, players[i].wait);
	   }
	   else{
	       printf("Player %d, teamB, with PID %d, has energy level: %d wait = %d \n",players[i].id,  (int)players[i].pid, players[i].energy, players[i].wait);
	   }


            exit(EXIT_SUCCESS); // Child exits after printing its name, PID, and energy level
        } else {
            // Parent process
            child_pids[i] = pid;
            
        }
    }

    // Parent process waits for all child processes to complete
    for (i = 0; i < NUM_CHILDREN; i++) {
        waitpid(child_pids[i], NULL, 0);
    }
for (int j = 0; j < NUM_CHILDREN; j++) {
    printf("%d \n", value[j]);
}
    printf("All child processes have completed.\n");


    // Initialize OpenGL
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1500, 1500);
    glutCreateWindow("Team Visualization");
    init();
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    //glutCreateWindow("Timers");
    glClearColor(0.0, 0.0, 0.0, 1.0);
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    //glutDisplayFunc(display);
    glutTimerFunc(1000, updateTimers, 0); // Start timer update loop
    glutMainLoop();

    
  
    return 0;
}

//------------------------------ END MAIN ------------------------------------

void performActionAfterDelay(int value) {
    // Perform the actions you want after the delay here
    // ...
    
    glutPostRedisplay(); // Trigger redrawing the scene after the action
}

// Function to mimic sleep by delaying an action
void glutSleep(int millisecondsDelay) {
    glutTimerFunc(millisecondsDelay, performActionAfterDelay, 0);
}

//                              DRAW FUNCTION

void drawTeam(GLfloat color[], GLfloat startX, GLfloat startY, int numMembers) {
    glColor3fv(color); // Set color for team members

    // Calculate the gap between each member in a column
    GLfloat gapX = 0.1;
    GLfloat gapY = 0.3;

    for (int i = 0; i < numMembers; i++) {
        glPushMatrix();
        // Calculate the position for each team member
        GLfloat x = startX + (i / 6) * gapX; // Change the X position after every 6 members
        GLfloat y = startY - (i % 6) * gapY; // Use modulo to reset position after every 6 members

        glTranslatef(x, y, 0.0);
        glutSolidSphere(0.07, 20, 20); // Draw team member as a sphere (head)
        glPopMatrix();
    }
}


// Function to draw a colored ball at specified coordinates
void drawBall(GLfloat color[], GLfloat startX, GLfloat startY) {
    glPushMatrix();
    glTranslatef(startX, startY, 0.0);
    glColor3fv(color);
    glutSolidSphere(0.03, 20, 20); // Draw a bigger sphere for the team leader
    glPopMatrix();
}




void init() {
    // Set clear color to black
    glClearColor(0.0, 0.0, 0.0, 1.0);
    // Set the matrix mode to projection to set up the camera
    glMatrixMode(GL_PROJECTION);
    // Load the identity matrix
    glLoadIdentity();
    // Set up an orthogonal projection matrix
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

// Function to draw text
void drawText(const char* text, int length, int x, int y) {
    glMatrixMode(GL_PROJECTION); 
    double matrix[16]; // Use a C-style array instead of dynamic memory allocation
    glGetDoublev(GL_PROJECTION_MATRIX, matrix); 
    glLoadIdentity(); 
    glOrtho(0, 800, 0, 600, -5, 5); 
    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity(); 
    glPushMatrix(); 
    glLoadIdentity(); 
    glRasterPos2i(x, y); 
    for(int i = 0; i < length; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)text[i]); 
    }
    glPopMatrix(); 
    glMatrixMode(GL_PROJECTION); 
    glLoadMatrixd(matrix); 
    glMatrixMode(GL_MODELVIEW); 
}


void drawintText(const int* text, int x, int y) {
    // Bright pink color for better contrast
    glColor3f(1.0f, 0.0f, 0.8f);

    // Draw the text multiple times with slight offsets to create a bold effect
    glPushMatrix();
    glTranslatef(x, y, 0);
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            glRasterPos2f(i * 0.1f, j * 0.1f); // Smaller offset for a crisper bold effect
            for (const int* p = text; *p; p++) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '0' + *p); // Convert int to character
            }
        }
    }
    glPopMatrix();
}




// Function to draw the table
void drawTable() {
    // Set up 2D orthogonal projection to draw the table in a 2D space
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    // Define the orthogonal projection to match the window size
    gluOrtho2D(0, 800, 0, 600); // These values should be adjusted to your window's actual size
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f); // Set the color to white (R: 1.0, G: 1.0, B: 1.0)


    // Variables to control the size and position of the table
    GLfloat tableStartX = 10.0f;  // X position where the table starts
    GLfloat tableStartY = 500.0f; // Y position where the table starts (from the top down)
    GLfloat cellWidth = 70.0f;    // Width of each cell
    GLfloat cellHeight = 30.0f;   // Height of each cell
    int numRows = 7;              // Number of rows including the title
    int numCols = 3;              // Number of columns

    // Draw the cells of the table
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            GLfloat x = tableStartX + j * cellWidth; 
            GLfloat y = tableStartY - i * cellHeight; 
            glBegin(GL_POLYGON);
            glVertex2f(x, y);
            glVertex2f(x + cellWidth, y);
            glVertex2f(x + cellWidth, y - cellHeight);
            glVertex2f(x, y - cellHeight);
            glEnd();
        }
    }

    // Draw the text for the table headers and cells
        glColor3f(0.0f, 1.0f, 1.0f);
       drawBoldText("Beach Ball Game", 10, (int)tableStartY + 15);
       int offsetX = 50; // This value adjusts the horizontal position of the text to the right

	// Adjust the headers "Team A" and "Team B" to be more to the right
	drawBoldText("Team A", (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)tableStartY - 15);
	drawBoldText("Team B", (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)tableStartY - 15);




  // Draw the row titles using the bold text function
	const char* rounds[] = {"Round 1", "Round 2", "Round 3", "Round 4", "Round 5", "Result"};
	for (int i = 0; i < 6; i++) {
        drawBoldText(rounds[i], (int)tableStartX + 5, (int)(tableStartY - (i+1) * cellHeight) - 15);
	}
	// Determine the new Y position for the text above the existing content.
	GLfloat additionalOffset = 120.0f; // Additional space above the current text position. Adjust as needed.
	GLfloat nameYPosition = infoBoxStartY - (cellHeight * (numRows + 1)) + additionalOffset; // Move text up

	
	drawMultipleLinesOfText(" Project Team:\n Mariam Hamad - 1200837\n Leena Affouri   - 1200335 \n Amany Hmidan - 1200225 \n Shahd Ali          - 1200183", infoBoxStartX + 10, nameYPosition);
                        
                         
     // Modify the scores of Team A and Team B based on the round count
     if (roundCount >= 1 && roundCount <= 5) {
        // Update scores based on the game results
        if (CountA > CountB) {
            teamAScores[roundCount - 1] = 0; // Team A lost
            teamBScores[roundCount - 1] = 1; // Team B win
        } else if (CountA < CountB) {
            teamAScores[roundCount - 1] = 1; // Team A win
            teamBScores[roundCount - 1] = 0; // Team B lost
        } else {
            teamAScores[roundCount - 1] = 1; // Draw
            teamBScores[roundCount - 1] = 1; // Draw
        }
    }              
     
  

if (roundCount == 1)  {    

 sprintf(teamAScoresStr, "%d", teamAScores[0]);
    sprintf(teamBScoresStr, "%d", teamBScores[0]);

        glColor3f(0.0f, 0.0f, 1.0f); // Set the color to blue
        drawBoldText(teamAScoresStr, (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (1) * cellHeight)-15);
        drawBoldText(teamBScoresStr, (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (1) * cellHeight)-15);
     
     
}

        
         
         else if (roundCount == 2) {
         sprintf(teamAScoresStr, "%d", teamAScores[0]);
    sprintf(teamBScoresStr, "%d", teamBScores[0]);

        glColor3f(0.0f, 0.0f, 1.0f); // Set the color to blue
        drawBoldText(teamAScoresStr, (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (1) * cellHeight)-15);
        drawBoldText(teamBScoresStr, (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (1) * cellHeight)-15);
     
     sprintf(teamAScoresStr2, "%d", teamAScores[1]);
    sprintf(teamBScoresStr2, "%d", teamBScores[1]);


        glColor3f(0.0f, 0.0f, 1.0f); // Set the color to blue
        drawBoldText(teamAScoresStr2, (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (2) * cellHeight)-15);
        drawBoldText(teamBScoresStr2, (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (2) * cellHeight)-15);
         }
         else if (roundCount == 3) {
          sprintf(teamAScoresStr, "%d", teamAScores[0]);
          sprintf(teamBScoresStr, "%d", teamBScores[0]);

        glColor3f(0.0f, 0.0f, 1.0f); // Set the color to blue
        drawBoldText(teamAScoresStr, (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (1) * cellHeight)-15);
        drawBoldText(teamBScoresStr, (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (1) * cellHeight)-15);
     
     sprintf(teamAScoresStr2, "%d", teamAScores[1]);
    sprintf(teamBScoresStr2, "%d", teamBScores[1]);


        glColor3f(0.0f, 0.0f, 1.0f); // Set the color to blue
        drawBoldText(teamAScoresStr2, (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (2) * cellHeight)-15);
        drawBoldText(teamBScoresStr2, (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (2) * cellHeight)-15);
       
       sprintf(teamAScoresStr3, "%d", teamAScores[2]);
    sprintf(teamBScoresStr3, "%d", teamBScores[2]);
       
       glColor3f(0.0f, 0.0f, 1.0f); // Set the color to blue
        drawBoldText(teamAScoresStr3, (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (3) * cellHeight)-15);
        drawBoldText(teamBScoresStr3, (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (3) * cellHeight)-15);
         }
         else if (roundCount == 4) {
                 sprintf(teamAScoresStr, "%d", teamAScores[0]);
    sprintf(teamBScoresStr, "%d", teamBScores[0]);

        glColor3f(0.0f, 0.0f, 1.0f); // Set the color to blue
        drawBoldText(teamAScoresStr, (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (1) * cellHeight)-15);
        drawBoldText(teamBScoresStr, (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (1) * cellHeight)-15);
     
     sprintf(teamAScoresStr2, "%d", teamAScores[1]);
    sprintf(teamBScoresStr2, "%d", teamBScores[1]);


        glColor3f(0.0f, 0.0f, 1.0f); // Set the color to blue
        drawBoldText(teamAScoresStr2, (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (2) * cellHeight)-15);
        drawBoldText(teamBScoresStr2, (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (2) * cellHeight)-15);
       
       sprintf(teamAScoresStr3, "%d", teamAScores[2]);
    sprintf(teamBScoresStr3, "%d", teamBScores[2]);
       
       glColor3f(0.0f, 0.0f, 1.0f); // Set the color to blue
        drawBoldText(teamAScoresStr3, (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (3) * cellHeight)-15);
        drawBoldText(teamBScoresStr3, (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (3) * cellHeight)-15);
        
        sprintf(teamAScoresStr4, "%d", teamAScores[3]);
    sprintf(teamBScoresStr4, "%d", teamBScores[3]);
        
        
        glColor3f(0.0f, 0.0f, 1.0f); // Set the color to blue
        drawBoldText(teamAScoresStr4, (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (4) * cellHeight)-15);
        drawBoldText(teamBScoresStr4, (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (4) * cellHeight)-15);
         }
         else if (roundCount == 5) {
                         sprintf(teamAScoresStr, "%d", teamAScores[0]);
    sprintf(teamBScoresStr, "%d", teamBScores[0]);

        glColor3f(0.0f, 0.0f, 1.0f); // Set the color to blue
        drawBoldText(teamAScoresStr, (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (1) * cellHeight)-15);
        drawBoldText(teamBScoresStr, (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (1) * cellHeight)-15);
     
     sprintf(teamAScoresStr2, "%d", teamAScores[1]);
    sprintf(teamBScoresStr2, "%d", teamBScores[1]);


        glColor3f(0.0f, 0.0f, 1.0f); // Set the color to blue
        drawBoldText(teamAScoresStr2, (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (2) * cellHeight)-15);
        drawBoldText(teamBScoresStr2, (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (2) * cellHeight)-15);
       
       sprintf(teamAScoresStr3, "%d", teamAScores[2]);
    sprintf(teamBScoresStr3, "%d", teamBScores[2]);
       
       glColor3f(0.0f, 0.0f, 1.0f); // Set the color to blue
        drawBoldText(teamAScoresStr3, (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (3) * cellHeight)-15);
        drawBoldText(teamBScoresStr3, (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (3) * cellHeight)-15);
        
        sprintf(teamAScoresStr4, "%d", teamAScores[3]);
    sprintf(teamBScoresStr4, "%d", teamBScores[3]);
        
        
        glColor3f(0.0f, 0.0f, 1.0f); // Set the color to blue
        drawBoldText(teamAScoresStr4, (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (4) * cellHeight)-15);
        drawBoldText(teamBScoresStr4, (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (4) * cellHeight)-15);
        
        
         sprintf(teamAScoresStr5, "%d", teamAScores[4]);
    sprintf(teamBScoresStr5, "%d", teamBScores[4]);

        glColor3f(0.0f, 0.0f, 1.0f); // Set the color to blue
        drawBoldText(teamAScoresStr5, (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (5) * cellHeight)-15);
        drawBoldText(teamBScoresStr5, (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (5) * cellHeight)-15);
         }
// Print the results

	for (int j = 0; j < 5; j++) {
   	 sumA += teamAScores[j];
    	 sumB += teamBScores[j];

	if (sumA < sumB) {
 	   drawBoldText("Loser", (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (6) * cellHeight) - 15);
           drawBoldText("Winner", (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (6) * cellHeight) - 15);
	} else if (sumA > sumB) {
           drawBoldText("Winner", (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (6) * cellHeight) - 15);
          drawBoldText("Loser", (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (6) * cellHeight) - 15);
	} else {
         drawBoldText("Draw", (int)tableStartX + (int)(cellWidth / 2) - 15 + offsetX, (int)(tableStartY - (6) * cellHeight) - 15);
         drawBoldText("Draw", (int)tableStartX + 3 * cellWidth / 2 - 15 + offsetX, (int)(tableStartY - (6) * cellHeight) - 15);
	}
	}
               

    // Reset to the original projection matrix
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}




void drawBoldText(const char* text, int x, int y) {
    // Bright pink color for better contrast
    glColor3f(1.0f, 0.0f, 0.8f);

    // Draw the text multiple times with slight offsets to create a bold effect
    glPushMatrix();
    glTranslatef(x, y, 0);
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            glRasterPos2f(i * 0.1f, j * 0.1f); // Smaller offset for a crisper bold effect
            for (const char* p = text; *p; p++) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
            }
        }
    }
    glPopMatrix();
}




void drawMultipleLinesOfText(const char* text, GLfloat x, GLfloat startY) {
    int lineHeight = 18; // Adjust the line height as needed
    const char* lineStart = text;
    const char* lineEnd;
    GLfloat y = startY;

   glColor3f(1.0f, 1.0f, 1.0f); // Set the color to white (R: 1.0, G: 1.0, B: 1.0)

    while (*lineStart) {
        // Find the end of the line or end of the string
        lineEnd = strchr(lineStart, '\n');
        if (lineEnd == NULL) {
            lineEnd = lineStart + strlen(lineStart);
        }

        // Draw each character until the end of the line
        glRasterPos2f(x, y);
        for (const char* p = lineStart; p < lineEnd; p++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
        }

        // Move down to the next line
        y -= lineHeight;

        // If we've reached the end of the string, break out of the loop
        if (*lineEnd == '\0') {
            break;
        }

        // Move to the start of the next line, skipping the newline character
        lineStart = lineEnd + 1;
    }
}

void drawText2(float x, float y, const char *string) {
    glPushMatrix();
    glTranslatef(x, y, 0.0);
    glScalef(0.0006, 0.0006, 1); // Adjust the scaling factor for smaller text
    glColor3f(1.0f, 1.0f, 1.0f); // Set text color to white
    while (*string) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *string);
        string++;
    }
    glPopMatrix();
}



//----------------------------------------------------------------------
//



void enableBallMovement(int value) {
    shouldMove = true; // Allow the ball of Team A to move again
    BshouldMove = true; // Allow the ball of Team B to move again
     moveCount =0;
     BmoveCount=0;
    // Re-enable other flags if necessary
    glutPostRedisplay();
}

// Function to handle timer update
void updateTimers(int value) {

    // Decrement the timers
    if (timer1 > 0) {
        timer1--; // Decrement Timer 1 if it's greater than 0
    } 
    else {
    
        timer1 = 30; // Reset Timer 1 to 30 if it reaches 0
        // Reset flags to stop ball movement immediately
        roundCount++;
        // Reset positions
        ballAPositionX = -0.3;
        ballAPositionY = 0.8;
        ballBPositionX = 0.6;
        ballBPositionY = 0.8;
	shouldMove = false; // Stop the ball of Team A from moving
        BshouldMove = false; // Stop the ball of Team B from moving
        //flagfirstina = true;
        //flagfirstinb = true;
        Aputball = false;
        Bputball = false;
        AballAdjustCount =0;
        BballAdjustCount =0;
        CountA=1;
        CountB=1;
         printf("Round %d\n", roundCount); // Print the current round count
        fflush(stdout);
        // Schedule the ball to move after a delay
        glutTimerFunc(2000, enableBallMovement, 0); // Wait 2000 milliseconds (2 seconds) before moving the ball
        
        
    }
    if (timer5 > 0) {
        timer5--; // Decrement Timer 5 if it's greater than 0
    }else{
    printf("The Game has finished.\n The Final Result Will Appear On SCreen\n Press Enter to exit.\n");
         getchar(); 
         exit(EXIT_SUCCESS);
    
    }



    // Update the display
    glutPostRedisplay();

    // Recall this function after 1 second
    glutTimerFunc(1000, updateTimers, 0);
}






void ddrawText(float x, float y, char *string) {
    glColor3f(1.0f, 1.0f, 1.0f); // Set text color to white
    glRasterPos2f(x, y); // Set position for text
    // Loop through each character of the string and render it
    while (*string) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *string);
        string++;
    }
}
void newdrawBall(GLfloat color[], GLfloat startX, GLfloat startY) {
    glPushMatrix();
    glTranslatef(startX, startY, 0.0);
    glColor3fv(color);
    glutSolidSphere(0.02, 20, 20); // Draw a bigger sphere for the team leader
    glPopMatrix();
    
}


// Function to display the scene
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

// Draw Team A
GLfloat teamAColor[] = {0.8, 0.2, 0.6}; // Dark Pink
drawTeam(teamAColor, -0.3, 0.8, 6); // Draw 6 members for Team A
drawText2(-0.4, 0.9, "TEAM A"); // Draw "TEAM A" above Team A

// Draw Team B
GLfloat teamBColor[] = {0.2, 0.6, 0.8}; // Light Blue
drawTeam(teamBColor, 0.6, 0.8, 6); // Draw 6 members for Team B
drawText2(0.5, 0.9, "TEAM B"); // Draw "TEAM B" above Team B


    // Draw the balls
    GLfloat ballAColor[] = {1.0, 1.0, 0.0}; // Yellow ball for Team A
    GLfloat ballBColor[] = {0.0, 1.0, 1.0}; // Cyan ball for Team B
    drawBall(ballAColor, ballAPositionX, ballAPositionY); // Draw the ball for Team A
    drawBall(ballBColor, ballBPositionX, ballBPositionY); // Draw the ball for Team B

    // Update ball position for next frame
    // Update ball position for next frame
    //initializeEnergyWaits();
    enrgywaitA = value[0];
   
    updateBallPosition(0); // Pass an integer argument (e.g., 0) to updateBallPosition
    
    // Draw the table
    drawTable();
    //draw ball in team b 
// Assuming NballBPositionY and NballAPositionY are initially set

if (Bputball) {

    
     GLfloat ballColor[] = {0.0, 1.0, 0.0}; // Updated for black color as requested
         CountB++;
   
    newdrawBall(ballColor, NballBPositionX, NballBPositionY);

}

if (Aputball) {

      GLfloat ballColor[] = {0.0, 1.0, 0.0}; // Updated for black color as requested
      CountA++;
    newdrawBall(ballColor, NballAPositionX, NballAPositionY);

    
}


    //glutSwapBuffers();
    
     // Convert timer values to minutes and seconds
    int timer1Minutes = timer1 / 60;
    int timer1Seconds = timer1 % 60;
    int timer5Minutes = timer5 / 60;
    int timer5Seconds = timer5 % 60;


    // Display timer1
    
    sprintf(timer1Text, "Timer 1: %02d:%02d", timer1Minutes, timer1Seconds);
    ddrawText(-0.9, 0.9, timer1Text);

    // Display timer5
    
    sprintf(timer5Text, "Timer 5: %02d:%02d", timer5Minutes, timer5Seconds);
    ddrawText(-0.9, 0.8, timer5Text);
    glFlush();
}


void check(){

	if (moveCount == 0){
	 enrgywaitA = value[0];
 	//printf("leader a: %d\n", enrgywaitA);
 	
	} else if(moveCount < 151){
	enrgywaitA = value[1];
	//printf("player1: %d\n", enrgywaitA);
	}
	else if(moveCount == 151){
	enrgywaitA= value[2];
 	//printf("2: %d\n", enrgywaitA);
	}else if(moveCount == 302){

	enrgywaitA =  value[3];
	//printf("player3: %d\n", enrgywaitA);
	}else if(moveCount == 453){
	enrgywaitA=  value[4];
	//printf("player4: %d\n", enrgywaitA);
	}else if(moveCount == 603){
	enrgywaitA =  value[5];
	//printf("player5: %d\n", enrgywaitA);
	}
	
	}
	
	





void changeballAtoB(int value) {
    // Allow the ball of Team A to move again
    // Allow the ball of Team B to move again
            moveCount =0;
            ballAPositionY = 0.8; // Y-coordinate reset
            ballAPositionX = 0.6 ;
            shouldMove = true;
            flagfirstina = false;
             //BshouldMove = true;
           //  flagfirstinb = true;
    // Re-enable other flags if necessary
    CountA--;
     CountB++;
    glutPostRedisplay();
     glutTimerFunc(3000, moveBallAgain, 0);
     
    
}

void changeballBtoA(int value) {
    // Allow the ball of Team A to move again
    // Allow the ball of Team B to move again
            moveCount = 0;
            ballAPositionY = 0.8; // Y-coordinate reset
            ballAPositionX = -0.3 ;
            shouldMove = true;
            //f++;
            flagfirstina = true;
            // BshouldMove = true;
             //flagfirstinb = false;
             CountB--;
     	      CountA++;
    // Re-enable other flags if necessary
    	   glutPostRedisplay();
           glutTimerFunc(3000, moveBallAgain, 0);
     
    
}







void moveBallAgain(int value) {
    check(); // Assuming this function sets up necessary conditions or checks
     //printf("Move Count: %d\n", moveCount);
     
      if (Aputball && AballAdjustCount < 720) {
        NballAPositionY -= 0.002; // Adjust Y-axis position
        AballAdjustCount++; // Increment counter
        glutPostRedisplay();
        }else{
        NballAPositionX = -0.3; // Initial position for ball of Team A leader
        NballAPositionY = 0.8;
        AballAdjustCount =0;
        // glutTimerFunc(5000, enableBallMovement, 0); 
        // NballAPositionX = 0.6; // Initial position for ball of Team A leader
        //NballAPositionY = 0.8;
         
         
        }
    if (moveCount < 604 && shouldMove) {
        ballAPositionY -= 0.002; // Move the ball downwards
        //NballAPositionY -= 0.002;
        moveCount++; // Increment the move counter
          glutPostRedisplay(); 
         glutTimerFunc(enrgywaitA, moveBallAgain, 0);
       // Request display update
        
        // Continue moving
    } else {
        // Movement completed; decide next action based on `flagfirstina`
         shouldMove = false;
         //glutPostRedisplay();
        if (flagfirstina) {
            ballAPositionX = -0.3;
            ballAPositionY = 0.8;
            //shouldMove = false;
            Aputball = true;
            glutTimerFunc(1000, changeballAtoB, 0);
            
        } else {
            ballAPositionX = 0.6;
            ballAPositionY = 0.8;
           Bputball = true;
            glutTimerFunc(1000, changeballBtoA, 0);
            
            
        }
        // No need to reschedule moveBallAgain here since movement is complete
        //glutPostRedisplay(); // Ensure the final position is displayed
       // glutTimerFunc(enrgywaitA, moveBallAgain, 0);
    }
}
    
    


void checkb(){
	if (BmoveCount  == 0){
	enrgywaitB = value[6];
	//printf("leaderb: %d\n", enrgywaitB);

	} else if(BmoveCount  < 151){
	enrgywaitB = value[7];
	//printf("player7: %d\n", enrgywaitB);
	}
	else if(BmoveCount  == 151){
	enrgywaitB = value[8];
	//printf("player8: %d %d\n", enrgywaitB, BmoveCount);
	}else if(BmoveCount  == 302){
	enrgywaitB = value[9];

	//printf("player9: %d\n", enrgywaitB);
	}else if(BmoveCount  == 453){
	enrgywaitB = value[10];
	//printf("player10: %d\n", enrgywaitB);
	}else if(BmoveCount  == 603){
	enrgywaitB = value[11];
	//printf("player11: %d\n", enrgywaitB);
	}
	}



 void BchangeballAtoB(int value) {
    // Allow the ball of Team A to move again
    // Allow the ball of Team B to move again
            BmoveCount =0;
            ballBPositionY = 0.8; // Y-coordinate reset
            ballBPositionX = 0.6 ;
           
             BshouldMove = true;
             flagfirstinb = true;
    // Re-enable other flags if necessary
     CountA--;
    CountB++;
   
    glutTimerFunc(3000, BmoveBallAgain, 0);
    glutPostRedisplay();
}

void BchangeballBtoA(int value) {
    // Allow the ball of Team A to move again
    // Allow the ball of Team B to move again
           BmoveCount = 0;
            ballBPositionY = 0.8; // Y-coordinate reset
            ballBPositionX = -0.3 ;
             BshouldMove = true;
             flagfirstinb = false;
             CountB--;
     CountA++;
    // Re-enable other flags if necessary
   
    glutTimerFunc(3000, BmoveBallAgain, 0);
     glutPostRedisplay();
}
	

void BmoveBallAgain(int value) {
    	checkb();
    if (Bputball && BballAdjustCount < 720) {
        NballBPositionY -= 0.002; // Adjust Y-axis position
        BballAdjustCount++; // Increment counter
        glutPostRedisplay();
        }else{
         NballBPositionX = 0.6;  // Initial position for ball of Team B leader
         NballBPositionY = 0.8;
         BballAdjustCount=0;
          //glutPostRedisplay();
         // glutTimerFunc(2000, enableBallMovement, 0); 
        //  NballBPositionX = -0.3;  // Initial position for ball of Team B leader
         //NballBPositionY = 0.8;
        }	
    	
    if (BmoveCount < 604 && BshouldMove) {
        //printf("Move Count: %d\n", BmoveCount);
        ballBPositionY -= 0.002;
        glutPostRedisplay();
        BmoveCount++;
        
        glutTimerFunc(enrgywaitB, BmoveBallAgain, 0);
    } else {
        // Movement completed; decide next action based on `flagfirstina`
         BshouldMove = false;
         //glutPostRedisplay();
        if (flagfirstinb) {
            ballBPositionX = 0.6;
            ballBPositionY = 0.8;
           Bputball = true;
          
            glutTimerFunc(1000, BchangeballBtoA, 0);
            
        } else {
           
            
             ballBPositionX = -0.3;
            ballBPositionY = 0.8;
            
            Aputball = true;
           
            glutTimerFunc(1000, BchangeballAtoB, 0);
            
        }
    
   
}
}


void updateBallPosition() { // from leader to first player


    if (balls[0].possession == TEAM_A && !isWaitingForBallMove && shouldMove) {
       check ();
        if (ballAPositionY > 0.5) { // If still above the ground or target
            shouldMove = true;      // Ensure moving process is allowed
            moveCount = 1;          // Initialize move counter
     
          // printf("leader a: %d\n", enrgywaitA);
           
            glutTimerFunc(enrgywaitA, moveBallAgain, 0); // Start moving process
             ballAPositionY -= 0.002;
             //NballAPositionY -= 0.002;
            glutPostRedisplay();    // Request display update
            
        }
    }




if (balls[1].possession == TEAM_B && !BisWaitingForBallMove && BshouldMove) {
checkb();
 if (ballBPositionY > 0.5) { // If still above the ground or target
 
            BshouldMove = true;      // Ensure moving process is allowed
            BmoveCount = 1;          // Initialize move counter
             glutTimerFunc(enrgywaitB, BmoveBallAgain, 0); // Start moving process
            ballBPositionY -= 0.002; // Move the ball downwards
            glutPostRedisplay();    // Request display update
           
        }
}

    // Update the display
    glutPostRedisplay();

    // Recall this function after a certain time interval (e.g., 50 milliseconds)
    glutTimerFunc(1000, updateBallPosition, 0);

}








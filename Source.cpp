/*
*** FILE NAME   : othello_game.cpp
*** DESCRIPTION : This program is an example program using OpenGL
*** DATE        : July 2015
*** WRITTEN By  : Farheen Sultana
*/

/*

Maximum search depth taken as 5.


Eval_function

In this game, if a player plays with the strategy of taking as many pieces as possible it is not correct. As the pieces can be taken away easily.
So, there is a evaluation function based on the position of the square as follows(in the order of preference)

four corners -> much preferred
the four outer edges -> next preferred
A squares : 0 : 2, 0 : 5, 7 : 2, 7 : 5 positions
B Squares : 0 : 3, 0 : 4, 7 : 3, 7 : 4 positions
C squares : 0 : 1, 0 : 6, 7 : 1, 7 : 6 positions

Inner 16 squares:
2 : 2, 2 : 5, 5 : 2, 5 : 5 positions are the good positions in the inner 16 squares.
Player must try not to occupy the remaining squares in the inner 16 squares. Because a disc in inner square may be flipped easily.

Middle square:
the middle square: (between innner 16 postions and outer edges) row 1, 6 && col 2 to 5 and col 1, 6 && row 2 to 5 is next preferred.

the four X squares  1 : 1, 1 : 6, 6 : 1, 6 : 6 : MOst dangerous positions as these can be taken away easily

Weights are assigned to the squares and their preference is evaluated.

*/




#include <stdio.h>               // standard C libraries
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <GL/glut.h>             // GLUT library
#include "cs_graphics_setup.h"   // Header for CS4250/5250/6250 courses

//@@***********************************************************************************@@
// Constants
#define WINDOW_XS 712 // Window size
#define WINDOW_YS 512
#define WINDOW_NAME "Play Othello" // Window name

#define ANI_MSEC 50 // gap between frames

#define FONT_10 GLUT_BITMAP_TIMES_ROMAN_10
#define FONT_24 GLUT_BITMAP_TIMES_ROMAN_24
float num_color[3] = { 1.0, 0.0, 0.0 }; //red color


//@@***********************************************************************************@@
// Structures
typedef struct pt
{
	GLfloat x, y;
}MyPoint;

struct node
{
	int node_value;
	int board[8][8];
	struct node *child[20];
	struct node *parent_ref;
}*root = NULL, *head = NULL, *ne=NULL;
static struct node *cur_root = NULL;


//@@***********************************************************************************@@
// Global Variables
MyPoint bottomLeftPt;

int recLength = 50;
int recHeight = 80;
int cirRadius = 12;
float DegToRad = 3.141592 * 1.0 / 180.0;
int flag_player;
int colorDir = 0;
int flag2 = 1;
int cur_search_depth = 1;
int max_search_depth = 5;
static int color_disc_tracker = 1; //this is to check that in alternate iterations, legal moves of white and black are considered and tree is constructed.
//for first iteration i.e., for constructing first children we need to consider legal moves of computer , so color_disc_tracker =1


//@@***********************************************************************************@@
// Function prototypes
void display_func(void);
void keyboard_func(unsigned char c, int x, int y);
void animation_func(int val);
void display_board(int[8][8]);

void initiate_board();
void generateTree(struct node *);
int othello_eval_func(int, int);
int alphaBetaSearch(struct node *);
int isLegalMove(int,int,int, struct node *);
int doDiscsFlip(int, int, int,int,int, struct node*);
void mouse_func(int button, int state, int x, int y);

void makeMove(int row, int col, struct node *  cur, int color_disc);
int max_value(struct node * cur, int alpha, int beta);
int min_value(struct node * cur, int alpha, int beta);



//@@***********************************************************************************@@
int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	init_setup(WINDOW_XS, WINDOW_YS, WINDOW_NAME);

	// initial position of bottom left corner of rectangle
	bottomLeftPt.x = 50;
	bottomLeftPt.y = 50;

	initiate_board();
	glutDisplayFunc(display_func);
	generateTree(head);

	glutKeyboardFunc(keyboard_func);
	glutMouseFunc(mouse_func);
	glutTimerFunc(ANI_MSEC, animation_func, 0);
	glutMainLoop();

	return 1;
} // end of main()

//@@***********************************************************************************@@

void initiate_board()
{
	int i = 0, j = 0;
	head = (struct node*)malloc(sizeof(struct node));
	ne = (struct node*)malloc(sizeof(struct node));
	ne = head;
	
	//initialize board values 0->blank spaces, -1->black discs , 1 ->white discs
	for (i = 0; i <= 7; i++)
	{
		for (j = 0; j <= 7; j++)
		{
			head->board[i][j] = 0;
		}
	}
	head->board[3][3] = 1;
	head->board[4][4] = 1;
	head->board[4][3] = -1;
	head->board[3][4] = -1;
	
	head->parent_ref = NULL;
	printf("head ref is %d", head);
	printf("First Board\n");
	for (i = 0; i <= 7; i++)
	{
		for (j = 0; j <= 7; j++)
		{

			printf("%d \t",  head->board[i][j]);
		}
		printf("\n");
	}

	
} //initiate_board() ends


//@@***********************************************************************************@@
//generates tree till depth = 5

void generateTree(struct node * head)
{
	int i = 0; //8*8 board row iterator
	int j = 0; // 8*8 board column iterator
	int k = 0, m = 0;
	int n = 0;
	int flag_tree_track = 0; // tracker checks if child nodes existed so that depth can be incremented.


	struct node* parent = (struct node*)malloc(sizeof(struct node));
	struct node* temp = NULL, *temp1 = NULL;
	struct node *new_child = NULL;
	//struct node * prev;
	parent = head;

	root = parent;
	printf("\n parent= %d", parent);
	cur_root = root;

	//----------------------------constructing tree---------------------------------------//
	//expand tree only till the max_search_depth (here, 5)
	//construct child nodes for all legal moves.

	while (cur_search_depth <= max_search_depth)
	{
		//printf("depth =%d", cur_search_depth);
		for (i = 0; i < 8; i++) //row iteration 
		{
			for (j = 0; j < 8; j++) //column iteration
			{
				if (parent->board[i][j] == 0)
				{
					if (isLegalMove(i, j, color_disc_tracker, parent))
					{

						flag_tree_track = 1;

						new_child = (struct node*)malloc(sizeof(struct node));
						parent->child[k] = new_child;
						if (color_disc_tracker = 1) //1 means white, computer node, so MAX NODE. node_value= worst case for MAX node = -1000
							new_child->node_value = -1000;
						else
							new_child->node_value = +1000;
						if (cur_search_depth == 5)
							new_child->node_value = othello_eval_func(i, j);
						new_child->parent_ref = parent;
						k++;
					}
				}

			}
		}
		for (n = k; n < 20; n++)
			parent->child[n] = NULL;

		if (flag_tree_track == 1 && flag2 == 1) //if there is atleast one legalmove, atleast one child node and you are not iterating in the same level
		{
			cur_search_depth++;
			color_disc_tracker = -color_disc_tracker; //alternate colors of discs at every level

		}

		for (k = 0; parent->child[k] != NULL; k++) //construct children of each child of parent.
		{
			flag2 = 0;
			generateTree(parent->child[k]);
		}
		flag2 = 1;


	}

	cur_search_depth = 0;
	generateTree(head->parent_ref);
}

//@@***********************************************************************************@@

// This method checks if there is a legal move available
//it calls doDiscsFlip() function.
//Parameters:
//row and col of the disc, color of disc, current node address.

//Returns:
//if a legal move is available, islegalMove() returns 1 , 
//if there is no available legal move for the player, islegalMove() returns 0.

int isLegalMove(int row, int col, int color_disc, struct node* cur)
{
	int r1, c1;

	for (r1 = -1; r1 <= 1; r1++)
		for (c1 = -1; c1 <= 1; c1++)
			if ((r1 != 0 && c1 != 0) && doDiscsFlip(row, col,color_disc, r1, c1, cur))
				return 1;

	//dodiscsflip returned with a 0 value, no discs to flip, so invalid move
	return 0;

}

//@@***********************************************************************************@@

//-------------------------doDiscsFlip()method----------------------//
// This method checks if discs can be flipped in a particular direction
//
//Parameters:
// row , col of the current player's tile, color_disc -> color of disc, r1,c1-> direction of move, cur_board -> board at current state
//r1= r1-1 c1 = c1-1  -> upward left diagonal
//r1= r1-1, c1 = c1-0  -> upwards direction
//r1 =r1-1 , c1 = c1-1 -> upwards right diagonal

//r1 = r1-0, c1 = c1-1 -> left
//r1 = r1- 0, c1 = c1+1 -> right direction

//r1 = r1-1, c1= c1-1 -> downward left diagonal
//r1 = r1- 1, c1 = c1-0 -> downward direction
//r1= r1-1 , c1 = c1+1 -> downward right diagonal

//Returns:
//if disc can be flipped  doDiscsFlip() returns 1 , 
//if not then it returns 0;

int doDiscsFlip(int row, int col, int color_disc, int r1, int c1,struct node *cur)
{

	int r2 = row + r1;
	int c2 = col + c1;
	while (row >= 0 && row < 8 && col >= 0 && col < 8 && cur->board[row][col] == -color_disc)
	{
		r2 += r1;
		c2 += c1;
	}

	// if there is an array index out of bounds or if we dont have a disc of same color at the other end
	//return 0
	if (r2 < 0 || r2> 7 || c2 < 0 || c2 > 7 ||  cur->board[r2][c2] != color_disc)
		return 0;
	
	//Yes, discs can be flipped in this particular direction
	return 1;
}


//@@***********************************************************************************@@

/* --------------Othello_Eval_Func()-------------------------------------

In this game, if a player plays with the strategy of taking as many pieces as possible it is not correct. As the pieces can be taken away easily.
So, there is a evaluation function based on the position of the square as follows(in the order of preference)

four corners -> much preferred
the four outer edges -> next preferred
A squares : 0 : 2, 0 : 5, 7 : 2, 7 : 5 positions 
B Squares : 0 : 3, 0 : 4, 7 : 3, 7 : 4 positions
C squares : 0 : 1, 0 : 6, 7 : 1, 7 : 6 positions

Inner 16 squares:
2 : 2, 2 : 5, 5 : 2, 5 : 5 positions are the good positions in the inner 16 squares.
Player must try not to occupy the remaining squares in the inner 16 squares. Because a disc in inner square may be flipped easily.

Middle square:
the middle square: (between innner 16 postions and outer edges) row 1, 6 && col 2 to 5 and col 1, 6 && row 2 to 5 is next preferred.

the four X squares  1 : 1, 1 : 6, 6 : 1, 6 : 6 : MOst dangerous positions as these can be taken away easily.

*/

int othello_eval_func(int row,int col)
{
	int weight;
	
	//4 outer corner squares are most preferred, 4 outermost edges are more preferred

	if (((row >= 0 && row < 8) && (col == 0 || col == 7)) || ((col >= 0 && col < 8) && (row == 0 || col == 7))) // 4 corners
	{
		weight = 800;
	}

	if (  ( (row == 0|| row == 7) && (col == 3 ||  col == 4) ) ) //  the B squares, 0:3, 0:4 ,7:3, 7:4 positions 
		weight = 500;
	if ( ( (row == 0 || row == 7) && (col == 2 || col == 5) ) ) //  the A squares, 0:2, 0:5 , 7:2, 7:5 positions 
		weight = 400;
	if ( ( (row == 0 || row == 7) && (col == 1 || col == 6) ) ) //  the C squares, 0:1, 0:6 , 7:1, 7:6 positions 
		weight = 300;

	// the middle square between outermost edges and 16 squares 

	if ((row >= 2 && row <= 5) && (col == 1 || col == 6))
		weight = 100;

	if ((col >= 2 && col <= 5) && (row == 1 || row == 6))
		weight = 100;

	// inner 16 squares

	if ((row == 2 || row == 5) && (col == 2 || col == 5)) // 2:2 , 2:5, 5:2, 5 :5 positions
		weight = 200;


	if ((row == 2 || row == 5) && (col == 3 || col == 4))
		weight = 50;

	if ((row == 3 || row == 4) && (col == 2 || col == 5))
		weight = 50;

	//least preferred or dangerous position
	if ((row == 1 || row == 6) && (col == 1 || col == 6)) // the four x positions 1:1 , 1:6 , 6:1, 6:6  
	{
		weight = 10;
	}
	return weight;

}
//@@***********************************************************************************@@

int alphaBetaSearch(struct node *cur)
{
	int value= max_value(cur, -1000, +1000);
	return value;
}


//@@***********************************************************************************@@
int max_value(struct node * cur, int alpha, int beta)
{
	int k = 0;
	int node_value= -1000;
		
	while (cur->parent_ref != NULL  )
	{

		if (node_value < min_value(cur, alpha, beta))
			node_value = min_value(cur, alpha, beta);

		if ((node_value >= beta))
				return node_value;
		else
		{
				k++;
				cur = cur->child[k];
				if (node_value > alpha)
					alpha = node_value;
				
		}
		
	}
	return node_value;

}

//@@***********************************************************************************@@

int min_value(struct node * cur, int alpha, int beta)
{
	int k = 0;
	int node_value = +1000;

	while (cur->parent_ref != NULL)
	{

		if (node_value > max_value(cur, alpha, beta))
			node_value = max_value(cur, alpha, beta);

		if ((node_value <= alpha))
			return node_value;
		else
		{
			k++;
			cur = cur->child[k];
			if (node_value < beta)
				beta = node_value;

		}

	}
	return node_value;

}




//@@***********************************************************************************@@

void makeMove(int row, int col, struct node *  cur, int color_disc)
{ 
	
	int r1, r2, c1, c2;

	cur->board[row][col] = color_disc;

	for (r1 = -1; r1 <= 1; r1++)
	{
		for (c1 = -1; c1 <= 1; c1++)
		{
			if ( (r1 != 0 && c1 != 0) && doDiscsFlip(row, col, color_disc, r1, c1, cur) )
			{
				r2 = row + r1;
				c2 = col + c1;
				while (cur->board[r2][c2] == -color_disc)
				{
					cur->board[r2][c2] = color_disc;
					r2 = r2 + r1;
					c2 = c2 + c1;
				}
			}
		}
	}
}
//@@***********************************************************************************@@

void mouse_func(int button, int state, int pos_x, int pos_y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		float deg;
		float cirX, cirY;

		int color_disc = -1;
		int row, col;
		int i, j;
		int x = 0, y = 0;
		int disc_color = -1;
		int cirCenX, cirCenY;

		for (i = 0; i<8; i++)
		{
			for (j = 0; j<8; j++)
			{
				if (pos_x >= x && pos_x <= (x + 64) && pos_y >= y   && pos_y <= y + 64)
				{
					row = i;
					col = j;
					cirCenX = x + 24; //taking the coordinate values for drawing circle
					cirCenY = y + 30;

				}
				x = x + 64;
			}
			x = 0;
			y = y + 64;
		}



		if(isLegalMove(row,col,-1, head))
		{
		glColor3f(0.0, 0.0, 0.0);
		glBegin(GL_POLYGON);
		for (deg = 0.0; deg < 360; deg += 0.01)
		{
			cirX = cirRadius * cos(deg*DegToRad) + cirCenX;
			cirY = cirRadius * sin(deg*DegToRad) + cirCenY;
			glVertex2f(cirX, cirY);
		}
		glEnd();
		glutPostRedisplay();

		}
		else
		printf(" Illegal Move");
		

	}
}




//@@***********************************************************************************@@

void display_func(void)
{

	int i = 0, j = 0;
	int x1 = 64;
	int y1 = 0;

	int x2 = 64;
	int y2 = 512;

	glClearColor(0.75, 0.75, 0.0, 1.0);   // background color (yellow)
	glClear(GL_COLOR_BUFFER_BIT);       // clearing the buffer not to keep the color


	display_board(head->board); //display functions should be called from display_func().


	//draw the dividing line at 200 pixel from right
	glColor3f(1.0, 0.0, 0.0); // setting pen color (red)
	glBegin(GL_LINES);
	glVertex2i(512, 0);
	glVertex2i(512, 512);
	glEnd();

	//draw the 8*8 matrix for tiles
	//drawing vertical lines
	for (i = 0; i < 7; i++)
	{
		glColor3f(1.0, 0.0, 0.0); // setting pen color (red)
		glBegin(GL_LINES);
		glVertex2i(x1, y1);
		glVertex2i(x2, y2);
		x1 = x1 + 64;
		x2 = x2 + 64;
		glEnd();

	}

	x1 = 0;
	y1 = 64;

	x2 = 512;
	y2 = 64;

	//drawing horizontal lines
	for (i = 0; i < 7; i++)
	{
		glColor3f(1.0, 0.0, 0.0); // setting pen color (red)
		glBegin(GL_LINES);
		glVertex2i(x1, y1);
		glVertex2i(x2, y2);
		y1 = y1 + 64;
		y2 = y2 + 64;
		glEnd();

	}

	glFlush();

	glutSwapBuffers(); // double buffering
} // end of display_func()


//@@***********************************************************************************@@
void keyboard_func(unsigned char c, int x, int y)
{
	switch (c)
	{
	case 'Q':
	case 'q':
		printf("Good Bye !\n");
		exit(0); // terminates the program
	}  // end of switch
} // end of keyboard_func()


//@@***********************************************************************************@@
void display_board(int cur_board1[8][8])
{
	int i = 0, j = 0;
	//int x = 24;
	//int y = 475;

	float deg;
	float cirX, cirY;
	int cirCenX = 24, cirCenY = 475;
	float cirR = 0.0, cirG = 0.0, cirB = 0.0;

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{

			if (cur_board1[i][j] == 0)
			{
				cirCenX = cirCenX + 64;
				continue;
			}
			else if (cur_board1[i][j] == -1) //-1 means black so set color to black
				glColor3f(0.0, 0.0, 0.0);
			else if (cur_board1[i][j] == 1)
				glColor3f(1.0, 1.0, 1.0); // 1 means white so set color to white

			//draw a circle
			glBegin(GL_POLYGON);
			for (deg = 0.0; deg < 360; deg += 0.01)
			{
				cirX = cirRadius * cos(deg*DegToRad) + cirCenX;
				cirY = cirRadius * sin(deg*DegToRad) + cirCenY;
				glVertex2f(cirX, cirY);
			}
			glEnd();

			cirCenX = cirCenX + 64;
		}
		cirCenX = 24;
		cirCenY = cirCenY - 64;
	}
}


//@@***********************************************************************************@@

void animation_func(int val)
{
	float cirR = 0.0, cirG = 0.0, cirB = 0.0;

	if (colorDir == 0)
	{
		cirR -= 0.04;
		cirG -= 0.04;
		cirB -= 0.04;

		if (cirR < 0.0)
		{
			cirR = 0;
			cirG = 1;

			colorDir = 1;
		}
	}
	else
	{
		cirR += 0.04;
		cirG += 0.04;
		cirB += 0.04;

		if (cirR > 1.0)
		{
			cirR = 1;
			cirG = 0;

			colorDir = 0;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(ANI_MSEC, animation_func, 0);
}//end animation_func

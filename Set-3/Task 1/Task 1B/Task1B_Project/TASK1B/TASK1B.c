/*
 * TASK1B.c
 *
 * Created: 04/11/2017 22:58:28
 *  Author: Avinash Dubey
 */ 

#define F_CPU 14745600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"

#define Header_files

#include "buzzer.h"
#include "path_planning.h"





//global start variable to store the starting position of the firebird V and initializing it with the first starting position
int start = 45;

//global present direction to store the present direction at which the firebird is pointing
int present_direction = 1;

//to check whether the graph is intialized or not
_Bool is_initialized = 0;

/** Structure Name: Direction
 ** Purpose: Used to store the directions to be traversed by the firebird. It has three values -1,0,1 and additional value of 2 to stop.
 **          -1 - left, 0 - straight, 1 - right, 2 - destination has been reached.
 **/
struct Direction {
	int directions[X];
};

/** This is our own function to determine the firebirds movements on the flex according to the path calculated
 ** Function Name: moveInThePath 
 ** Input: path - it is the structure representing the shortest path from the start to the finish
 ** Output: Returns a Direction structure which consists of the directions to be taken by the firebird V in order to move
 ** in the path given as argument
 **/
#include "motion_control.h"

struct Direction moveInThePath(struct Path_Array path) {
	
	int next_direction;
	int path_it = 1;
	int next_node, present_node;
	present_node = path.path[path_it-1];
	next_node = path.path[path_it];
	struct Direction direction;

	while(next_node!=-1) {
		// calculating the next direction to be taken. 
		// The following schema maps the values of next_direction to directions (north, south, east, west) -
		// 0 - west | 1 - north | 2 - east | 3-south
		if(next_node - present_node == 1) {
			next_direction = 2;
		} else if(next_node - present_node == -1) {
			next_direction = 0;
		} else if(next_node - present_node == 7) {
			next_direction = 3;
		} else if(next_node - present_node == -7) {
			next_direction = 1;
		}

		// direction to be taken with respect to the present node in order to move to the next node.
		// The following schema maps the values of turn to left, right and straight - 
		// -1,3 - left | 1,-3 - right | 0 - straight
		int turn = next_direction - present_direction;

		if(turn == -1 || turn == 3) {
			direction.directions[path_it-1] = -1;
		} else if(turn == 1 || turn == -3) {
			direction.directions[path_it-1] = 1;
		} else if(turn == 0) {
			direction.directions[path_it-1] = 0;
		}

		present_node = next_node;
		next_node = path.path[++path_it];
		present_direction = next_direction;
	}

	direction.directions[path_it-1] = 2;

	return 	direction;
}


void ReachDestinationAvoidingNode(unsigned char Xd,unsigned char Yd,unsigned char Xn,unsigned char Yn)
{
	// lcd_cursor(2,1);
	// lcd_string("Entering RDAN");
	//TODO:: Please write your application code. You can make your own functions & header files
	
	//declaring the graph
	static struct Graph graph;

	//creating the graph if it not created and initialized yet
	if(!is_initialized) {
		graph = createGraph();
		is_initialized = 1;
	}

	lcd_cursor(2,1);
	lcd_string("Exit createGraph");

	//removing the obstacle from it
	graph = removeObstacle(graph,Xn,Yn);

	lcd_cursor(2,1);
	lcd_string("Exit removeObstacle");

	//mapping the coordinates of the destination node on the flex to the node number in the graph
	int finish = coordinateMapping(Xd,Yd);

	//finding the shortest path from start finish after removing the obstacle.
	struct Path_Array path_array = findPath(graph,start,finish);

	lcd_cursor(2,1);
	lcd_string("Exit findPath");

	//next start position will the destination node of the firebirds current path
	start = finish;

	//call the function to run the firebird V on the specified shortest path.
	//localization of the firebird V
	struct Direction directions = moveInThePath(path_array);
	lcd_cursor(2,1);
	lcd_string("Exit moveInThePath");

	//starting the motion of the bot
	start_motion(directions);

	//adding the obstacle
	graph = addObstacle(graph,Xn,Yn);
}


//Do not make changes in main function

int main(void)
{
	init_devices();
	lcd_set_4bit();
	lcd_init();

	// lcd_cursor(2,1);
	// lcd_string("Entering main");
       ReachDestinationAvoidingNode(5,'D',6,'D');
	   buzzer_on();
	   _delay_ms(500);
	   buzzer_off();
	   ReachDestinationAvoidingNode(2,'F',2,'D');
	   buzzer_on();
	   _delay_ms(500);
	   buzzer_off();
	   ReachDestinationAvoidingNode(2,'A',2,'C');
	   buzzer_on();
	   
}
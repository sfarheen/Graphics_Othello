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



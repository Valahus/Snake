


Project name: SNAKE


Author: BOSIICA Ionut-Mihai
no etudiant: u21411192



Description of project:
The project tries to be as close as possible to a SNAKE GAME in terminal.



I used mr. P. Vanier Project indications regarding the termios and mods/levels.
Tools i used beside my IDE were Valgrind, GDB.






To run the game WITH NO ARGUMENTS, open a terminal window and type:


make
./game 



If you want to run the game with different arguments you have 3 arguments from which you can chose(either one alone, either combined):
    --speed AMOUNT or -s AMOUNT ----- where AMOUNT is a number ranging from 1 to 1000 ( the speed of the snake ) 
    --level LVL or -l LVL ------ where LVL indicates the indice of the level ( since there are only 2 levels in each mod, the choice is from 00 and 01 ) 
    --mod MOD or -m MOD ----- where MOD indicates the current mod folder selected ( 0 for mod or 1 for mod 2 in our cases) 
    



    
/****************************************/
LAUNCHING THE GAME WITH ARGUMENTS:
example:

make clean 
make 
./game -s 769 -l 01 -m 1

    or

make clean
make
./game --speed 1000 --level 00 --mod 0




to play the game both ARROWS( UP, DOWN, LEFT, RIGHT ) or (W, A, S, D) can be used. 
"Q" key used in game to go back to level screen, mod screen, exit game 



The levels have no end. The level ends when either you press Q ( out of boredom ) or when you kill the snake. 
The levels I made are not that interesting, in "mod"- the first one has only collectibles which i used only to see if the snake grows
                                                    - the second has only obstacles
                                            in "mod2" - both leves are better, but you are free to test and make your own as i am descriebing down bellow:


/************************************/
HOW TO MAKE MODS/LEVELS:
                                            
To create your own levels, acces one of the existing mod folders, or create another one named mod3. It should contain:
    -deroulement text file in which you write a title ,the index of the levels ( 00, 01, etc ) and the name of the level coresponding to each index.(see existing deroulement file if needed )
    -niveux folder which contains the levels counted as in 00, 01, 02, etc. To write your own level, the file has to contain on the:
        -first line - N number of objects ( collectibles and/or walls ) 
        -the next N lines contain the character asigned to collectibles ( * ) or the one for walls ( # ) and indicate the position ( x, y ) of the character as in (# 10 20)
        PS: look inside mod folders to understand better if instructions are not clear.
        
        
        

        
I hope you will have fun making your levels and playing!

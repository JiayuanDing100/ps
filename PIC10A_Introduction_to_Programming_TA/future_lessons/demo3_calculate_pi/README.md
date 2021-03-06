# Demo 4 - calculation of pi

Modify the mini program given at the very end of [week 1 lecture note](https://ccle.ucla.edu/pluginfile.php/1778309/mod_resource/content/0/Topic_IntroToProgramming.pdf)
```cpp
// Randomly select an integer value from 1 to 10 inclusive
int randomFrom1To10 = rand() % 10 + 1;
// Variable to store whether the user needs to continue guessing
bool notCorrectYet = true; // start it being true
/* The user will be asked to guess until they are correct */
while( notCorrectYet ) { // while the user has not yet guessed correctly
cout <<"Guess the number from 1 to 10: ";
	int guess; // value the user will guess
	cin >>guess; // take the user’s guess
	if ( guess == randomFrom1To10 ) { // if the guess is correct
		notCorrectYet = false; // update status so loop can terminate
	}
}
cout <<"You guessed the number!"; // tell them they guessed it
```
and do Book Exercise R1.18. (calculate pi). 

## In class challenge
There's a limit on how big an typed int variable can be.

Suppose this value is LIMIT_OF_INT. We know the behavior of the limit is that it will be larger than or equal itself plus 1. (Since the computer can't represent LIMIT_OF_INT + 1, it will return something else)

Write a program that finds out this exact number.

```cpp
int LIMIT_OF_INT;
while (notCorrrectYet){
	
	// fill in this part

	if (LIMIT_OF_INT >= LIMIT_OF_INT + 1){
		notCorrectYet = false; // it's correct NOW
	}
}
```
WARNING: this program could take up to 30 seconds to run. You'll see why when you see the result.

## Side Note
For those who learned about ++i or i++ else where, you should read [this](http://www.cplusplus.com/forum/general/160856/) and consider always using ++i. 

# Demo 3 - exploring data types

Experiments with different data types (int, double) and their operations


## In class challenge

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
and do Book Exercise R1.18. (calculate pi). See Demo 4.

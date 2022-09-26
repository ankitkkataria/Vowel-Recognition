# Vowel-Recognition
ReadMe
Some important points 
All the files and folders in my zip file should be in the same directory in order for the code to work well. 
This zip file doesn't contain the cooledit software i've used to record these vowels.
As inputs i'm taking 10 files of each vowel (a,e,i,o,u) using it as training data so later i can recognize whatever vowel file you want to test.
Since i am not able to run visual studio on my machine you'll have to open my cpp file in any IDE of your choice.
For convenience,some important parameters have been pre-defined at the top

How to run this program :-
This program basically recognizes the vowels so feel free to record any vowel in cool edit keep the mode mone and 16k samples per second.
Then select and trim the vowel part keep some silence in the begining and end.
Then go into VowelsFolder and delete one of those hundered files but copy it's name first
Then you can make your file you just recorded the same name as the file you deleted 
It doesn't matter if you've recorded vowel 'u' but you deleted '214101008_a_15' and you rename it   '214101008_a_15' just remember when you run your code the program will automatically figure out that in the file  '214101008_a_15' it's not 'a' it's actually vowel 'u'.
Most of the functions and their uses are written in the code itself as comments.
Once training is complete the program recognizes our own vowel file from 11 to 20 and predicts from what it has learnt what vowel is currently in this file.
Thanks.

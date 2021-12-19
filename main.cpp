#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <iostream>
using namespace std;
int maximum (FILE *fp);  //This function is to find the max in a cooledit text file.
int minimum (FILE *fp);     //This function is to find the min in a cooledit text file.
double getDcshift(FILE *fp); // This function will take care of DC shift if there is any.
double getNormalizationFactor (FILE *fp); // This function will normalize the sound wave whenever neccessary 
void FillSamplesArray(double samples_arr[],FILE *fp,int y); // This  function will make frames of size 320 which we will then use for training and testing purposes.
int count_samples(FILE *fp); // Counts the no. of samples in our cooledit file
double R[13]; // This contains auto correlated energy values 
double Ai[13]; // This contains the solutions to durbin's algo
double Ci[13]; // modification of Ai
char vowel[] = {'a','e','i','o','u','\0'}; // vowel array 
int max_index =0; // this will contain index of the max sample (in terms of amplitude ) in our cool edit file 
int start_index = 0;
double weights[] = {1.0,3.0,7.0,13.0,19.0,22.0,25.0,33.0,42.0,50.0,56.0,61.0}; 
int StartIndex =0;
double finalVowelPredictArray[5] = {0.0,0.0,0.0,0.0,0.0}; // This will contain tokhura distance of our file that we are testing from the training data
void test (); // we call this to test after learning from training data
#define P 12
#define N 320

 void calculate_Cis(double arr[],double getCi[])  // This function calculates Ci values 
{

	//First we calculate Ri values 
	for(int i=0;i<=12;i++)
	{
		double sum=0.0;
		for(int j=0;j<320-i;j++)
		{
			 sum=sum+arr[j]*arr[j+i];
				
		  }
	     	R[i]=sum;
		   sum=0;
	}


//	Now we will find Ai values with lavinson's algo
	double E[13],K[13],A[13][13];
	
	//initializing initial values
	for(int i=0;i<=P;i++)
	{
		E[0]=0;
		K[i]=0;
		
	   for(int j=0;j<=P;j++)
		   A[i][j]=0;
	}
	
	E[0]=R[0];

	for(int i=1;i<=P;i++)
	{
	   for(int j=1;j<i;j++)
		   K[i]+=(A[i-1][j]*R[i-j]);
        
	   K[i]=((R[i]-K[i])/E[i-1]);
	   A[i][i]=K[i];
	    
	   for(int j=1;j<i;j++)
		   A[i][j]=(A[i-1][j]-(K[i]*A[i-1][i-j]));
      
	   E[i]=(1-K[i]*K[i])*E[i-1];
	}

	//puts values in Ai array
	for(int i=1;i<=P;i++)
		Ai[i-1]=A[12][i];

	
	
	//now we calculate Cis values
	for(int i=1;i<=P;i++)
		Ci[i]=0;
	Ci[0]=log(R[0]*R[0]);

	for(int i=1;i<=P;i++)
	{ 
		for(int j=1;j<i;j++)
			Ci[i]+=((double)j/i)*Ci[j]*Ai[i-j-1];

	   Ci[i]+=Ai[i-1];
	   
	}

    
	
	
	// Then we  will store these values in a getCi array 
	for(int i=1;i<=P;i++)
		getCi[i-1]=Ci[i];
	
}
// Now we will get average Ci values for our training data per vowel
void getAvgCi(double ALL_Ci[10][5][12],double AvgCi[5][12])
{
	// initialize AvgCi to zero
	for(int b=0;b<5;b++)
	{
	   for(int c=0;c<12;c++)
	    AvgCi[b][c]=0;
  	}
	
	// Putting values in our AvgCi values 
	for ( int i = 0; i < 5 ; i++)
	{
		for ( int q = 0; q < 10 ; q++)
		{
			for ( int x= 0; x < 12 ; x++)
				{	
				   AvgCi[i][x] = AvgCi[i][x] + ALL_Ci[q][i][x];
				}
		}
	}
	
	// Now  since we added 10 rows we will divide everything by  10
	for(int b=0;b<5;b++)
	{
	   for(int c=0;c<12;c++)
	    AvgCi[b][c] = AvgCi[b][c]/10;
  	}
	

}
// This funcition basically returns the max value's index back to us
int getMaxIndex (FILE *fp,int total_number_of_samples_in_file)
{
	char arr[100];
	int max_index;
	double max =0;
	rewind(fp);
	for(int i =0;i<=4;i++) // skipping 4 lines of our code 
	{
		fgets(arr,100,fp);
	}
	
	double TempArray [total_number_of_samples_in_file] ; // This will just create a simple 1-D array to store all the values in file
	
	for (int i =0; i <= total_number_of_samples_in_file ; i++)  // this finds absolute max
	{
		 fscanf(fp,"%lf\n",&(TempArray[i]));
		TempArray[i] = abs(TempArray[i]);
		
		
	}
	
		for (int i =0; i <= total_number_of_samples_in_file ; i++) // this then finds absolute max's index
		{
			if (max < TempArray[i])
			{
				max = TempArray[i];
				max_index = i;
			}
			
		}

   rewind(fp); // rewinding so our file pointer doens't point to the end all the time
	return (max_index+1);
	
}

int count_samples(FILE *fp) // It simply finds the no of sample in the file 
{      int samples;
        char arr[100];
	   fscanf(fp,"%s        %d\n",arr,&samples );
	   return samples;
	
}		

double getNormalizationFactor (FILE *fp) // this finds the normalization_factor
{
	double max;
	double min;
	double nf; 
	double temp;
	max = maximum(fp); 
    min = minimum(fp);
	min = abs(min);
	temp = (max+min)/2;  
	nf = 5000/temp;
	rewind(fp);
	return nf;
	
}	

double getDcshift (FILE*fp) 
{    
	double number_of_samples = 0;
	double samples_sum = 0;
	double current_sample;
	rewind(fp);
	if (fp == NULL)
	{
		printf("dcshift can not be done as there is nothing in this file ");
		return 0.0;
	}
	
	else 
      {
	 // this skips the first five lines as they aren't data
				char arr[100];
				for(int i =0;i<=4;i++)
						{
							fgets(arr,100,fp);
						}
			
			while(!feof(fp)) // this will find the no of samples.
					{
							fscanf(fp,"%lf\n",&current_sample);
							samples_sum+=current_sample;
							number_of_samples++;
							
					}
			
		 }
		double dcoffset = samples_sum/number_of_samples; // dc offset
		rewind(fp);
		return dcoffset;
	}
//here i will pass file name and array so that it can store values in array 320 samples
void FillSamplesArray(double samples_arr[],FILE *fp,int start_index,int iteration)
{
	// skipping five lines again
	char arr[100];
	double temp;
	rewind(fp);
	for(int i =0;i<=4;i++)
	{
		fgets(arr,100,fp);
	}
	
	int read_ahead_the_previous_iteration = start_index + iteration*320; // reading 320 samples then moving ahead when we have to fill the next array
	// this will help us skip
	for(int i =0 ; i < (read_ahead_the_previous_iteration-1) ; i++)
	{
		fscanf(fp,"%lf\n",&temp);
	}
	// this actually fills the array
	for (int i = 0 ; i < 320 ; i++)
	{   
		fscanf(fp,"%lf\n",&(samples_arr[i]));
	}
	
}
	
int maximum(FILE*fp) // finds max sample in file passed by file pointer 
{
	static int maxx;
	char arr[100];
	for(int i =0;i<=4;i++)
	{
		fgets(arr,100,fp);
	}
	int temp =0;
	while (!feof(fp))
	{
		fscanf(fp,"%d\n",&temp);
		if (maxx < temp)
		{
			maxx = temp;
		}
		
	}
	
	rewind(fp);
	return maxx;
}

int minimum(FILE*fp) // finds min sample in file passed by file pointer 
{
	static int minn;
	char arr[100];
	for(int i =0;i<=4;i++)
	{
		fgets(arr,100,fp);
	}
	
	int temp =0;
	while (!feof(fp))
	{
		fscanf(fp,"%d\n",&temp);
		if (minn > temp)
		{
			minn = temp;
		}
		
	}
	
	rewind(fp);
	return minn;
}

//function to predict the vowel
void compare_with_reference_files(double testArray[5][12])
{
  double referenceArray[5][12]; // so i can store the actual reference file in an array for comparing 
  char refFileName[80];
   for (int i = 0 ; i < 5 ; i++)
	   {           // This statement below will open the proper reference file
				   sprintf(refFileName,"%c_averageCi_referenceFIle.txt",vowel[i]);
				   FILE *vowel_testing_fp = fopen(refFileName,"r");
				   if(vowel_testing_fp==NULL)
				   {
					   printf (" The reference file %s that you're trying to access is not actually present in the directory ", refFileName);
					   return;
				   }
				   
				for(int j = 0 ; j < 5 ; j++)
				{
					for ( int k=0 ; k <12 ; k++)
						{
									fscanf(vowel_testing_fp,"%lf\n",&(referenceArray[j][k]));
						}
				}
			
					
				//Now for all 5 rows of our test file we will calculate it's tokhura's distance from vowel[i]'s reference file.
			    double tokhuraDistanceTemp [5] = {0.0,0.0,0.0,0.0,0.0};
				
			   	for(int rowNum = 0 ; rowNum < 5 ; rowNum++)
				{
					for ( int colNum=0 ; colNum <12 ; colNum++)
					{
						tokhuraDistanceTemp[rowNum] += weights[colNum] * ((testArray[rowNum][colNum]-referenceArray[rowNum][colNum]) * (testArray[rowNum][colNum]-referenceArray[rowNum][colNum])  );
					}
				} //put this in a array tokhuraDistanceTemp array;
				
			long double sum=0.0;
			for(int itr=0;itr<5;itr++) // this is to find avg
			{
				sum+= tokhuraDistanceTemp[itr];
			}
			   sum=sum/5;
			  finalVowelPredictArray[i]=sum; // this will store the tokhura distance of vowel[i} from your current file
	
	   }

	double minValue = finalVowelPredictArray[0]; //This finds min distance 
	int minIndex = 0;
	for ( int m = 1 ; m < 5 ; m++)
	{
		if (minValue > finalVowelPredictArray[m])
		{
			minValue = finalVowelPredictArray[m];
			minIndex = m;
		}
	}
	printf(" pridiction is %c  ",vowel[minIndex]);	 // predicting statement 
}




	
	int main ()
					{
						// In main function i'm just learning from the files 
                        double dcoffset = 0.0;
						double normalization_factor = 0.0;
						double samples_arr[320];
						char file_name[60];
						double ALL_Ci[10][5][12];
						double AvgCi[5][12];
						FILE *fp;
							for (int i = 0 ; i <5 ; i++) // This for loop is for me to read all 5 vowel's files 
							{ 
									for (int j =1 ; j <=10; j++) // This for loop is for me to read 10 files each vowel
									{
										sprintf(file_name,"VowelsFolder\\214101008_%c_%d.txt",vowel[i],j); // opening files 
										fp = fopen(file_name , "r");
										if(fp == NULL)
										{
											printf("The FIle named  %s  couldn't be found in your program's directory " , file_name);
										}
										
										dcoffset = getDcshift(fp); 
										normalization_factor = getNormalizationFactor(fp); 
										int total_number_of_samples_in_file = count_samples(fp); 
										max_index = getMaxIndex (fp,total_number_of_samples_in_file); 
										start_index=max_index; 

									   for (int k=0;k<5;k++) // This for loop for each of my vowels i will make five frames and each time i'll calculate Ci and put it in All_CI array from which we will later calculate avg_ci
										{   
												int iteration=k; 
												FillSamplesArray(samples_arr,fp,start_index,iteration);
												for (int m = 0; m < 320 ; m++)
												{
													samples_arr[m] = (samples_arr[m]-dcoffset)*normalization_factor;
												}
					
											 
											  calculate_Cis(samples_arr,Ci); // calculating ci for this frame
											   for(int x = 0; x <=12 ; x++) // Applying raised sin window on Ci values
											   {
												   Ci[x] = Ci[x] * (1+6*sin(((3.14)*x)/12));
											   }
											   
											
											for(int y = 0 ; y < 12 ; y++)  // storing in ALL_Cii
											   {
												ALL_Ci[j-1][k][y] = Ci[y];
												
											   }
											   
		                                        
											   
										}//Five frames completed 
										
										
									}// one vowel completed
										
							 getAvgCi(ALL_Ci,AvgCi); // Averaging 
							  char referenceFileName[50];
							  sprintf(referenceFileName,"%c_averageCi_referenceFIle.txt",vowel[i]);
							  FILE *referenceFilePointer = fopen(referenceFileName,"w"); 
							  
							  
									for(int b=0;b<5;b++) // Here we are writing an reference file which contains the avg_ci values for this particular vowel[i] by the end since this loop will run 5 times we will have 5 reference files and that's what we call learning 
									{
									for(int c=0;c<12;c++)
									{
									fprintf(referenceFilePointer,"%lf\n" , AvgCi[b][c]);
									}
								//   fprintf(referenceFilePointer,"\n");

									}
									printf("learning from %c vowel is done & reference file is created in the same directory \n",vowel[i]);
									fclose(referenceFilePointer);
							}//Five vowels completed

test (); // Calling test function to test if what i've learnt is right or wrong 
 
}    
	   
void test ()
	{       // here we are testing from our learnt data using tokhura's distance
		double testArray[5][12];
		double dcoffset = 0.0;
		double normalization_factor = 0.0;
		double samples_array_for_testing [320];
				 for(int w = 0; w < 5 ; w++) // This for loop is so i can go through all the vowels a,e,i,o,u for testing 
					{   char testFileName[70];;
						   for(int h= 11; h <=20 ; h++) // This for loop tests all the files from 11 to 20 for all 5 vowels just to test 
						   {
								sprintf(testFileName,"VowelsFolder\\214101008_%c_%d.txt",vowel[w],h); // opening files to now test from what we have learned
								  FILE *testFilePointer = fopen(testFileName,"r");
								  if(testFilePointer==NULL)
									  {
										  printf("The file with name %s doesn't exist in the directory ", testFileName);
										 break; 
									  }
								  // Same like we did while learning 
					              int total_number_of_samples_in_file = count_samples(testFilePointer);
								  max_index = getMaxIndex (testFilePointer,total_number_of_samples_in_file);
								  start_index = max_index;
								dcoffset = getDcshift(testFilePointer);
								normalization_factor = getNormalizationFactor(testFilePointer);
								  
								  
								  for(int k = 0 ; k <5 ; k++) // This is so i can make a 5*12 Ci array so i can compare it with all the vowels 
								  {
									  
									  int iteration=k;
												FillSamplesArray(samples_array_for_testing,testFilePointer,start_index,iteration);
												for (int m = 0; m < 320 ; m++)
												{
													samples_array_for_testing[m] = (samples_array_for_testing[m]-dcoffset)*normalization_factor;
												}

										   calculate_Cis(samples_array_for_testing,Ci);
											for(int x = 0; x <=12 ; x++) // Applying raised sin window on Ci values
											{
												   Ci[x] = Ci[x] * (1+6*sin(((3.14)*x)/12));
											}
											
											for(int y = 0;y<12;y++)
											{
												testArray[k][y] = Ci[y]; // sending it to compare with all the vowels 
											}
											
										
								}
								   printf("\n File that i'm currently testing is %s----------------->",testFileName);
								   compare_with_reference_files(testArray); // This is to finally compare tokhura distance between our test file and all the 5 vowels

						} 
							
					}

    }	   
		   
	   
	   
	   
	   
	   
	   
	   
	   
	   
	   
	   
	   
				
	
			
	



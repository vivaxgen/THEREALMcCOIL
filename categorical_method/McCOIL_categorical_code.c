//R CMD SHLIB McCOIL_categorical_code.c llfunction_het.c
#include <stdio.h>
#include <R.h>
#include <Rmath.h>
#include "loglikelihood_het.h"
#include <time.h>

void McCOIL_categorical(int *max, int *iterations, int *n0, int *k0, double *sampleS2, int *M0, double *P0, double *error1, double *error2, char **file_index, char **path) {

////////can be changed///////
	double varP=0.1;  
	
///////don't change////////
	int i=0, j=0, prime=0, x=0, y=0;
	double sumori=0, sumcan=0; 
	
	GetRNGstate();  //provide seed for random sampling
	
	////read in parameter values////
	int max_moi= *max;	
	int iter = *iterations;
	int n = *n0, k = *k0;
	double e1 = *error1, e2 = *error2;
	int M[(n+1)], Mcan[(n+1)], Maccept[(n+1)];
	double P[(k+1)], Pcan[(k+1)]; 
	int Paccept[(k+1)];
	double ll[(n+1)][(k+1)];
	double llcan[(n+1)][(k+1)];
	double S2[(n+1)][(k+1)];
	double q1=0.0, q2=0.0;
	for (i=1;i<=n;i++){
		M[i]= M0[i-1];
		Mcan[i]= M[i];
		Maccept[i]= 0;
		for (j=1;j<=k;j++){
			S2[i][j]= sampleS2[(i-1)*k+j-1]; 
			if (i==1) {
				P[j]= P0[j-1];
				Pcan[j]= P[j];
				Paccept[j]= 0;
			}
		}
	}
	time_t t1, t2;
	t1 = time(NULL); // time 1
	
	////output////
	char var_file[1000];
	sprintf(var_file, "%s/%s", path[0], file_index[0]); 
	FILE *V0 = fopen(var_file, "w");

	////MCMC////
	
	//calculate likelihood of initial values
	for (i=1;i<=n;i++){
		for (j=1;j<=k;j++){
			ll[i][j]= logLike_het(M[i], P[j], S2[i][j], e1, e2); //change all likelihood function, change likelihood function and h file
			llcan[i][j]= ll[i][j];
		}
	}
	for (i=1;i<=iter;i++){
		//update M
		for (j=1; j<=n; j++){
			prime = rbinom(1,0.5);
			if (prime==0) prime=-1; 
			Mcan[j]= M[j] + prime;
			if ((Mcan[j]<=max_moi) && (Mcan[j]>0)){
				sumcan=0;
				sumori=0;
				for (x=1;x<=k;x++){
					llcan[j][x]=logLike_het(Mcan[j], P[x], S2[j][x], e1, e2);
					sumcan+=llcan[j][x];
					sumori+=ll[j][x];
				}
				//accept
				if (log(runif(0.0,1.0)) < (sumcan-sumori)) {
					M[j]=Mcan[j];
					Maccept[j]++;
					for (x=1;x<=k;x++){
						ll[j][x]=llcan[j][x];
					}
				}
				//reject
				else {
					Mcan[j]=M[j];
					for (x=1;x<=k;x++){
						llcan[j][x]=ll[j][x];
					}
				}
			}
			else {
				Mcan[j]=M[j];
			}
		}
		//update PP
		for (j=1; j<=k; j++){
			Pcan[j]= rnorm(P[j],varP);
			if ((Pcan[j]<1) && (Pcan[j]>0)){
				sumcan=0;
				sumori=0;
				for (y=1;y<=n;y++){
					llcan[y][j]=logLike_het(M[y], Pcan[j], S2[y][j], e1, e2);
					sumcan+=llcan[y][j];
					sumori+=ll[y][j];
				}
				//accept
				if (log(runif(0.0,1.0)) < (sumcan-sumori)) {
					P[j]=Pcan[j];
					Paccept[j]++;
					for (y=1;y<=n;y++){
						ll[y][j]=llcan[y][j];
					}
				}
				//reject
				else {
					Pcan[j]=P[j];
					for (y=1;y<=n;y++){
						llcan[y][j]=ll[y][j];
					}
				}
			}
			else {
				Pcan[j]=P[j];
			}
		}
		//print this iteration
		fprintf(V0,"%d", i);
		for (x=1;x<=n;x++) fprintf(V0,"\t%d",  M[x]);
		for (x=1;x<=k;x++) fprintf(V0,"\t%.6f", P[x]);
		fprintf(V0,"\n");

	}
	
	fprintf(V0, "total_acceptance");
	for (x=1;x<=n;x++) fprintf(V0,"\t%d",  Maccept[x]);
	for (x=1;x<=k;x++) fprintf(V0,"\t%d", Paccept[x]);
	fprintf(V0,"\n");	
	
	t2 = time(NULL); // time 2
	Rprintf("Time = %.2f s\n", difftime(t2, t1));
	fclose(V0);
	PutRNGstate();
}


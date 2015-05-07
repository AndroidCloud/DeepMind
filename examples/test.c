/*! gcc -Wall -g -o test test.c libkdtree.a */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include "kdtree.h"
#include <math.h>
unsigned int get_msec(void)
{
	static struct timeval timeval, first_timeval;

	gettimeofday(&timeval, 0);

	if(first_timeval.tv_sec == 0) {
		first_timeval = timeval;
		return 0;
	}
	return (timeval.tv_sec - first_timeval.tv_sec) * 1000 + (timeval.tv_usec - first_timeval.tv_usec) / 1000;
}

#define PI 3.14159265
void convert(char str[] , int *a,double *b,double *c, double *d){
	const char *term = "\t\n\r "; /* word terminators*/
	char *state; /* Invocation state of strtok_r*/
	char *tok = strtok_r(str, term, &state); /* First invocation is different*/
	int count = 0;
	int age;
	double first,second;
	double x,y,z;
	while (tok) {
		if(count==0){
			age = atoi(tok);
			count+=1;
			/*printf("Age %d\n",age);*/
		}
		else if (count==1){
			first = atof(tok);
			/*printf("Long %f\n",first);*/
			count+=1;
		}
		else if (count==2){
			second = atof(tok);
			/*printf("Latitude %f\n",second);*/
		}
		tok = strtok_r(NULL, term, &state); /* subsequent invocations call with NULL*/
	}
	double val = PI / 180;
	y = 6371 * cos(first*val) * sin(second*val);
	x = 6371 * cos(first*val) * cos(second*val);
	z = 6371 * sin(first*val);
	/*printf("%f %f %f\n",x,y,z);*/
	double lat = asin(z/6371) * 180.0 / PI;
	double lon = atan2(y,x) *180/PI;
	/*printf("%f %f\n", lat,lon);*/
	*a = age;
	*b = x;
	*c = y;
	*d = z;

}
void coordinates_to_lat(double first, double second, double *x, double *y, double *z){
	double val = PI / 180;
	*y = 6371 * cos(first*val) * sin(second*val);
	*x = 6371 * cos(first*val) * cos(second*val);
	*z = 6371 * sin(first*val);
}


void lat_to_coordinates(double x, double y, double z, double *lat, double *longitude){
	*lat = asin(z/6371) * 180.0 / PI;
	*longitude = atan2(y,x) *180/PI;
}

int main(int argc, char **argv)
{
	int i,j, vcount = 10;
	int tot_count = 101;
	void *kd[tot_count], *set[tot_count];
	unsigned int msec, start;
/*
	if(argc > 1 && isdigit(argv[1][0])) {
		vcount = atoi(argv[1]);
	}
*/
	printf("inserting  random vectors... ");

	/* Creating Trees correspinding to each age group */
	for (i=0;i<tot_count;i++)
		kd[i]=kd_create(3);

	start = get_msec();

	/* Reading Input File */
	FILE *file;
	char *line = NULL;
	size_t len = 0;
	char read;
	file=fopen(argv[1], "r");

	if (file == NULL)
		return 1;

	int a;
	double b;
	double c;
	double d;

	/* Inseting the person's co-ordinates and age in the respective trees */
	while ((read = getline(&line, &len, file)) != -1) {
		convert(line, &a, &b, &c, &d);
		assert(kd_insert3(kd[a], b, c, d, 0) == 0);	 
	}

	if (line)
		free(line);

	msec = get_msec() - start;
	printf("%.3f sec\n", (float)msec / 1000.0);

	start = get_msec();
	double lat, longitude, latitude;
	/**
	  printf("range query returned %d items in %.5f sec\n", kd_res_size(set[0]), (float)msec / 1000.0);
	 **/
	int reqd_dist,index , ind;
	while(1){
		printf("Enter How many top places you want\n");
		scanf("%d",&reqd_dist);
		int counter = 0;
		int initial_radius = 100;
		printf("Enter the age of the person\n");
		scanf("%d",&index);
		printf("Enter the co-ordinates of a person \n");
		scanf("%f %f",&longitude,&latitude);
		double a_x, a_y,a_z;
		coordinates_to_lat(longitude, latitude, &a_x,&a_y,&a_z);
		int starting_index[101] ={0};
		/* Copmuter Science   */
		int loop_break = 0;
		int end_index =  index + sqrt(reqd_dist);
		if(end_index >= 101)
			end_index = 100;
		while(counter <= reqd_dist){
			for (i=index ; i<= end_index ; i++){
				set[i] = kd_nearest_range3(kd[i], a_x, a_y,a_z, initial_radius);
				/*printf("After %d\n",kd_res_size(set[i]));
				*/
			}

			/* TODO :: change sqrt(reqd_dist) to something else...... */	
			for(ind = index; ind <= end_index; ind++){
				double x = 1, y = 1, z = 1;
				a = 1;
				while(starting_index[ind]){
					kd_res_next(set[ind]);
					starting_index[ind]-=1;
				}
				while(a){
					kd_res_item3(set[ind], &x, &y, &z);
					/*printf("%f %f %f\n",x,y,z);*/
					lat_to_coordinates(x, y, z, &lat, &longitude);
					printf("%d %0.2f %0.2f\n",ind,lat, longitude);
					counter += 1;
					int val = kd_res_end(set[ind]);
					if(val == 1){
						a = 0;
						break;
					}
					else{
						a = kd_res_next(set[ind]);
						/*printf("%d\n",a);*/
					}
					if(counter >= reqd_dist){
						loop_break = 1;
						break;
					}
				}
				if(loop_break ==1)
					break;
			}

			for (i=index ; i<= end_index;i++){
				starting_index[i] = kd_res_size(set[i]);
			}

			if(loop_break ==1)
				break;
			initial_radius += initial_radius;
		}
	}
	msec = get_msec() - start;
	/*	for(i=1;i<tot_count;i++){
		kd_res_free(set[i]);
		kd_free(kd[i]);
		}
	 */
	/**
	  printf("range query returned %d items in %.5f sec\n", kd_res_size(set[i]), (float)msec / 1000.0);
	 **/
	/*
	   printf("range query returned %d items in %.5f sec\n", kd_res_size(set), (float)msec / 1000.0);
	   kd_res_free(set);
	   kd_free(kd);
	 */
	return 0;
}

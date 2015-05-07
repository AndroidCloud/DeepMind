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


int main(int argc, char **argv)
{
	int i,j, vcount = 10;
	void *kd2, *set2;
	int tot_count = 101;
	void *kd[tot_count], *set[tot_count];
	unsigned int msec, start;

	if(argc > 1 && isdigit(argv[1][0])) {
		vcount = atoi(argv[1]);
	}
	printf("inserting %d random vectors... ", vcount);
	fflush(stdout);


	kd2 = kd_create(3);

	for (i=0;i<tot_count;i++)
		kd[i]=kd_create(3);
	

	start = get_msec();

	FILE *file;
        char *line = NULL;
        size_t len = 0;
        char read;
        file=fopen(argv[2], "r");

        if (file == NULL)
                return 1;

	int a;
        double b;
        double c;
        double d;

        while ((read = getline(&line, &len, file)) != -1) {
                printf("%s", line);
         	convert(line, &a, &b, &c, &d);
		assert(kd_insert3(kd[a], b, c, d, 0) == 0);	 
        }

        if (line)
                free(line);



	/*for(i=0; i<vcount; i++) {
		float x, y, z;
		x = ((float)rand() / RAND_MAX) * 200.0 - 100.0;
		y = ((float)rand() / RAND_MAX) * 200.0 - 100.0;
		z = ((float)rand() / RAND_MAX) * 200.0 - 100.0;
		printf("%f %f %f\n",x,y,z);

		assert(kd_insert3(kd2, x, y, z, 0) == 0);

		for(j=0;j<tot_count;j++)
			assert(kd_insert3(kd[j], x, y, z, 0) == 0);
	}*/
	msec = get_msec() - start;
	printf("%.3f sec\n", (float)msec / 1000.0);

	start = get_msec();
	double posi[3];
	posi[0] = 0;
	posi[1] =0;
	posi[2] = 0;

//	set2 = kd_nearest_n(kd2, posi, 10);

/*
	set = kd_nearest_range3(kd, 0, 0, 0, 40);
*/	
	for (i=0;i<tot_count;i++)
		set[i] = kd_nearest_range3(kd[i], 0, 0, 0, 40);
		
	double x = 1,y=1,z=1;
	printf("range query returned %d items in %.5f sec\n", kd_res_size(set[0]), (float)msec / 1000.0);
	kd_res_item3(set[0], &x, &y, &z);
	printf("%f %f %f\n",x,y,z);

	/*a = kd_res_next(set[0]);
	printf("%d\n",a);
	kd_res_item3(set[0], &x, &y, &z);
	printf("%f %f %f\n",x,y,z);

	a = kd_res_next(set[0]);
	printf("%d\n",a);
	kd_res_item3(set[0], &x, &y, &z);
	printf("%f %f %f\n",x,y,z);


	a = kd_res_next(set[0]);
	printf("%d\n",a);
*/
/*
	 printf("%d\n",set->size);	
*/
	 /* if(set[0]->riter) {
                set->riter->item->pos[0];
                set->riter->item->pos[1];
                set->riter->item->pos[2];
		printf("%f\n",set->riter->item->pos[0]);
         }
*/

	msec = get_msec() - start;
	for(i=0;i<tot_count;i++){
		printf("range query returned %d items in %.5f sec\n", kd_res_size(set[i]), (float)msec / 1000.0);
		kd_res_free(set[i]);
		kd_free(kd[i]);
	}
/*
	printf("range query returned %d items in %.5f sec\n", kd_res_size(set), (float)msec / 1000.0);
	kd_res_free(set);
	kd_free(kd);
*/
	return 0;
}

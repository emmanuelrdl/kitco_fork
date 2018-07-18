/*
 ============================================================================
 Name        : pgm2kitco.c
 Author      : chris
 Version     :
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

int find_translation(uint8_t* dict, uint8_t dict_len, char *out)
{
	return 0;
}

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int main(int argc, char **argv)
{
	char* pgm;

	for(int i=0; i<argc; i++) {
		pgm = argv[i];
	}

	fprintf(stderr, "path: %s\n", pgm);

	FILE *file = fopen(pgm, "r");
	if (file) {

    	uint16_t buf=0;
    	uint16_t imgh = 0;
    	uint16_t imgw = 0;
    	uint8_t dataseek = 0;
    	uint8_t remaining0A = 4;

    	uint8_t greymin = 0xff;
    	uint8_t greymax = 0x0;
    	uint8_t toomanygreys = 0;
    	int greys[4] = {-1,-1,-1,-1};

    	uint8_t *data;
    	uint32_t data_i=0;

    	int c;
	    while ((c = getc(file)) != EOF)
	    {
	    	if (remaining0A == 2) // parsing image width/height
	    	{
	    		if (c == 0x20) {
	    			imgw = buf;
	    			buf = 0;
	    		}
	    		else if (c == 0x0a) {
	    			remaining0A--;
	    			imgh = buf;
	    			buf = 0;
	    			fprintf(stderr, "size: %dx%d\n", imgw, imgh);
	    			data = malloc(imgw*imgh*sizeof(uint8_t));
	    		}
	    		else {
	    			buf = buf*10 + (c-0x30);
	    		}
	    	}
	    	else if (remaining0A == 0) // pix data
	    	{
	    		data[data_i++] = c;
	    		greymin = MIN(greymin, c);
	    		greymax = MAX(greymax, c);
	    		if (!toomanygreys)
	    		{
		    		int i=0;
	    			while (greys[i]!=c && greys[i]!=-1 && ++i<4);
		    		if(i == 4) {
		    			toomanygreys=1;
		    			fprintf(stderr, "warning: too many greys !");
		    		}
		    		else
		    			greys[i]=(int)c;
	    		}
	    		continue;
	    	}
	    	else if (c == 0x0a)
	    		remaining0A--;

	    	dataseek++;
	    }
	    fclose(file);

	    fprintf(stderr, "value min: %x\nvalue max: %x\n", greymin, greymax);

	    printf("int image_width = %d;\n", imgw);
	    printf("int image_height = %d;\n", imgh);
	    printf("unsigned char image[] = {\n");

	    qsort(greys, 4, sizeof(int), cmpfunc);
	    greys[0] = greymin;
	    greys[3] = greymax;
	    fprintf(stderr, "greys %d %d %d %d\n", greys[0], greys[1], greys[2], greys[3]);

	    uint8_t sum=0;
	    uint32_t i=0;
    	uint8_t o = 0;
	    // MSB
	    for (i=0; i<data_i; i++)
	    {
	    	sum |= (data[i] > greys[1] ? 0 : (1<<i%8));

	    	if (i%8 == 7 || i+1 == data_i)
	    	{
	    		printf("0x%x,", sum);
	    		sum = 0;
	    	}
	    }
	    printf("\n");
	    // LSB
	    sum=0;
	    for (i=0; i<data_i; i++)
	    {
	    	o=0;
	    	while (data[i] > greys[o] && ++o < 3);
	    	sum |= (3-o & 1) << i%8;

	    	if (i%8 == 7 || i+1 == data_i)
	    	{
	    		printf("0x%x%c", sum, i+1 == data_i ? ' ' : ',');
	    		sum = 0;
	    	}
	    }

	    printf("};");
	    free(data);
	}
	else
	{
		fprintf(stderr, "%s not found !", pgm);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

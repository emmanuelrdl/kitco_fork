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

#define DEFAULT_NAME "image"

#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

int find_translation(uint8_t* dict, uint8_t dict_len, char *out)
{
	return 0;
}

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

void usage()
{
	fprintf(stderr, "usage:\n\tpgm2kitco <pgm file> [name]");
}

int main(int argc, char **argv)
{
	char* pgm;
	char* image_name = 0;

	if(argc == 1) {
		usage();
		exit(1);
	}
	if(argc > 1)
		pgm = argv[1];
	if(argc > 2)
		image_name = argv[2];

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
	    qsort(greys, 4, sizeof(int), cmpfunc);
	    greys[0] = greymin;
	    greys[3] = greymax;
	    fprintf(stderr, "greys %d %d %d %d\n", greys[0], greys[1], greys[2], greys[3]);

/////////////////////////////

	    printf("/* generated with pgm2kitco */\n\n");

	    printf("#include <avr/pgmspace.h>\n");
	    printf("#include <kitco_gfx.h>\n\n");

	    printf("#ifndef %s_H_\n", image_name != 0 ? image_name : "_pidgeot");
	    printf("#define %s_H_\n\n", image_name != 0 ? image_name : "_pidgeot");

	    printf("const unsigned char PROGMEM _%s[] = {\n", image_name != 0 ? image_name : "_pidgeot");

	    uint8_t buf8 = 0;
	    uint8_t counter = 0;
	    uint16_t index;

	    for(int x=0; x<imgw; x++)
	    {
	    	for(int y=0; y<imgh; y++)
	    	{
	    		index = x + y*imgw;
		    	buf8 |= (data[index] > greys[1] ? 0 : (1<<counter));

	    		if(++counter == 8)
	    		{
	    			printf("0x%x,", buf8);
	    			counter = 0;
	    			buf8 = 0;
	    		}
	    	}
	    }

		if(counter != 0)
			printf("0x%x,", buf8);

	    printf("\n");

		counter = 0;
    	uint8_t o = 0;
    	buf8 = 0;
	    for(int x=0; x<imgw; x++)
	    {
	    	for(int y=0; y<imgh; y++)
	    	{
	    		index = x + y*imgw;
		    	o=0;
		    	while (data[index] > greys[o] && ++o < 3);
		    	buf8 |= ((3-o) & 1) << counter;

	    		if(++counter == 8)
	    		{
	    			printf("0x%x%c", buf8, (x+1 == imgw && y+1 == imgh) ? '\n' : ',');
	    			counter = 0;
	    			buf8 = 0;
	    		}
	    	}
	    }

		if(counter != 0)
			printf("0x%x", buf8);

	    printf("};\n");
	    printf("const kitco_image_P PROGMEM %s = { %d, %d, 2, _%s };\n\n",
	    		image_name != 0 ? image_name : "_pidgeot",
	    		imgw,
				imgh,
				image_name != 0 ? image_name : "_pidgeot");

	    printf("#endif\n");
	    free(data);
	}
	else
	{
		fprintf(stderr, "%s not found !", pgm);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

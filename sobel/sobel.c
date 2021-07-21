// Zachary Anderson (za956053)
// CAP4453 @ SPR21
// Source code from https://www.cs.ucf.edu/courses/cap4453/progs/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

        int pic[256][256];
        int outpicx[256][256];
        int outpicy[256][256];
        int maskx[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
        int masky[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};
        double ival[256][256],maxival;

int main(argc,argv)
int argc;
char **argv;
{
	int i,j,p,q,mr,sum1,sum2;
	double threshold;
	FILE *fo1, *fo2, *fp1, *fopen();
	char *helper;

	argc--; argv++;
	helper = *argv;
	fp1 = fopen(helper,"rb");

	argc--; argv++;
	helper = *argv;
	fo1 = fopen(helper,"wb");

	argc--; argv++;
	helper = *argv;
	threshold = atof(helper);

	// removing header from input file
	// char rm[20];
	// for (i = 0; i <= 3; i++)
	// {
	// 	fscanf(fp1,"%s", rm);
	// }

	for (i = 0; i < 256; i++)
	{ 
		for (j = 0; j < 256; j++)
		{
			pic[i][j] = getc (fp1);
			pic[i][j] &= 0377;
		}
	}

	mr = 1;
	for (i = mr; i < 256 - mr; i++)
	{ 
		for (j = mr; j < 256 - mr; j++)
		{
			sum1 = 0;
			sum2 = 0;
			for (p = -mr; p <= mr; p++)
			{
				for (q = -mr; q <= mr; q++)
				{
					sum1 += pic[i + p][j + q] * maskx[p + mr][q + mr];
					sum2 += pic[i + p][j + q] * masky[p + mr][q + mr];
				}
			}
			outpicx[i][j] = sum1;
			outpicy[i][j] = sum2;
		}
	}

	maxival = 0;

	for (i = mr; i < 256 - mr; i++)
	{
		for (j = mr; j < 256 - mr; j++)
		{
			ival[i][j]=sqrt((double)((outpicx[i][j] * outpicx[i][j]) +
									(outpicy[i][j] * outpicy[i][j])));
			if (ival[i][j] > maxival) maxival = ival[i][j];
		}
	}

	// creation of hi and low thresholds 
	double hiThreshold = 255 / 6.9;
	double loThreshold = 255 / 2.2;

	// creation of vault[][], a 2D array used to store permanent pixel values for each input.
	double vault[256][256];

	// caching pixels as reference to use ival[][] as a printing array.
	for (i = 0; i < 256; i++)
	{
		for (j = 0; j < 256; j++)
		{
			vault[i][j] = ival[i][j];
		}
	}
	// printf("Vault transfer success.\n");

	// char chomp[15];
	
	// magnitude image
	FILE *file1 = fopen("output1.pgm", "wb");
	fprintf(file1, "P5\n256 256\n255\n");

	// printing to output1.pgm
	for (i = 0; i < 256; i++)
	{
		for (j = 0; j< 256; j++)
		{
			ival[i][j] = (vault[i][j] / maxival) * 255;
			fprintf(file1,"%c",(char)((int)(ival[i][j])));
		}
	}

	fclose(file1);
	
	// hi threshold image
	FILE *file2 = fopen("output2.pgm", "wb");
	fprintf(file2, "P5\n256 256\n255\n");
	
	// printing to output2.pgm
	for (i = 0; i < 256; i++)
	{
		for (j = 0; j< 256; j++)
		{
			ival[i][j] = (vault[i][j] / maxival) * 255;

			if (ival[i][j] > hiThreshold)
				ival[i][j] = 255;
			if (ival[i][j] < hiThreshold)
				ival[i][j] = 0;


			
			fprintf(file2,"%c",(char)((int)(ival[i][j])));
		}
	}

	fclose(file2);

	// lo threshold image
	FILE *file3 = fopen("output3.pgm", "wb");
	fprintf(file3, "P5\n256 256\n255\n");

	// printing to output3.pgm
	for (i = 0; i < 256; i++)
	{
		for (j = 0; j< 256; j++)
		{
			ival[i][j] = (vault[i][j] / maxival) * 255;

			if (ival[i][j] > loThreshold)
				ival[i][j] = 255;
			if (ival[i][j] < loThreshold)
				ival[i][j] = 0;
			
			fprintf(file3,"%c",(char)((int)(ival[i][j])));
		}
	}

	fclose(file3);

	printf("⭑ Edge detection has finished.\n⭑ Please refer to the output files for results.\n");
	return 0;
}

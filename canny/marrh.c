// Zachary Anderson (za956053)
// CAP4453 @ SPR21
// Source code from https://www.cs.ucf.edu/courses/cap4453/progs/
//  Marr-Hildreth.c  (or marrh.c)

#include <stdio.h>
#include <math.h>
#define  PICSIZE 256
#define  MAXMASK 100

         int    pic[PICSIZE][PICSIZE];
         double outpicx[PICSIZE][PICSIZE];
         double outpicy[PICSIZE][PICSIZE];
         double ival[PICSIZE][PICSIZE];
         double maskx[MAXMASK][MAXMASK];
		 double masky[MAXMASK][MAXMASK];
         double peaks[PICSIZE][PICSIZE];
		 double finishedEdges[PICSIZE][PICSIZE];

int main(argc,argv)
int argc;
char **argv;
{
	int     i,j,p,q,s,t,u,v,mr,centx,centy, flag;
	double  maskval,sum1,sum2,sig,maxival,direction, loThreshold, hiThreshold;
	FILE    *file1, *file2, *file3, *ifp, *fopen();

	// opening 3 files for output
	ifp = fopen("garb34.pgm", "rb");
	file1 = fopen("garb34mag.pgm", "wb");
	file2 = fopen("garb34peaks.pgm", "wb");
	file3 = fopen("garb34finished.pgm", "wb");

	// giving each output file a header
	fprintf(file1, "P5\n256 256\n255\n");
	fprintf(file2, "P5\n256 256\n255\n");
	fprintf(file3, "P5\n256 256\n255\n");

	// removing header from input file: garb34.pgm
	char rm[20];
	for (i = 0; i <= 3; i++)
	{
		fscanf(ifp,"%s", rm);
	}

	// sig initialized
	sig = 1;

	mr = (int)(sig * 3);
	centx = (MAXMASK / 2);
	centy = (MAXMASK / 2);

	// Canny: P1
	for (i = 0; i < 256; i++)
	{ 
		for (j = 0; j < 256; j++)
		{
			pic[i][j]  =  getc (ifp);
		}
	}

	// assigning values to maskx and maxy (derivative)
	for (p = -mr; p <= mr; p++)
	{
		for (q = -mr; q <= mr; q++)
		{
			maskval = ((-q) * (exp(-1 * (((p * p) + (q * q)) / (2 * (sig * sig))))));
			(maskx[p + centy][q + centx]) = maskval;
			maskval = ((-p) * (exp(-1 * (((p * p) + (q * q)) / (2 * (sig * sig))))));
			(masky[p + centy][q + centx]) = maskval;
		}
	}

	// using maskx and masky to calculate sum 1 & 2 and outpic x & y 2D arrays
	for (i = mr; i <= 255 - mr; i++)
	{
		for (j = mr; j <= 255 - mr; j++)
		{
			sum1 = 0, sum2 = 0;
			for (p = -mr; p <= mr; p++)
			{
				for (q = -mr; q <= mr; q++)
				{
					sum1 += pic[i + p][j + q] * maskx[p + centy][q + centx];
					sum2 += pic[i + p][j + q] * masky[p + centy][q + centx];
				}
			}
			outpicx[i][j] = sum1;
			outpicy[i][j] = sum2;
		}
	}

	// sqrt of squares code from sobel.c
	for (i = mr; i < 256 - mr; i++)
	{ for (j = mr; j < 256 - mr; j++)
		{
			ival[i][j]=sqrt((double)((outpicx[i][j] * outpicx[i][j]) +
									(outpicy[i][j] * outpicy[i][j])));
			if (ival[i][j] > maxival) maxival = ival[i][j];
		}
	}

	// printing to garb34mag.pgm
	for (i = 0; i < 256; i++)
	{
		for (j = 0; j < 256; j++)
		{
			ival[i][j] = (ival[i][j] / maxival) * 255;
			fprintf(file1,"%c",(char)((int)(ival[i][j])));
		}
	}

	// Canny: P2

	// selecting each pixel's gradient direction for testing
	for (i = mr; i < 256 - mr; i++)
	{
		for (j = mr; j < 256 - mr; j++)
		{
			if ((outpicx[i][j]) == 0.0)
				outpicx[i][j] = 0.00001;
			direction = outpicy[i][j] / outpicx[i][j];
			if ((direction <= 0.4142) && (direction > -0.4142))
			{
				if ((ival[i][j] > ival[i][j-1]) && (ival[i][j] > ival[i][j+1]))
					peaks[i][j] = 255;
			}
			else if ((direction <= 2.4142) && (direction > 0.4142))
			{
				if ((ival[i][j] > ival[i-1][j-1]) && (ival[i][j] > ival[i+1][j+1]))
					peaks[i][j] = 255;
			}
			else if ((direction <= -0.4142) && (direction > -2.4142))
			{
				if ((ival[i][j] > ival[i+1][j-1]) && (ival[i][j] > ival[i-1][j+1]))
					peaks[i][j] = 255;
			}
			else
			{
				if ((ival[i][j] > ival[i-1][j]) && (ival[i][j] > ival[i+1][j]))
					peaks[i][j] = 255;
			}
		}
	}

	// printing to garb34peaks.pgm
	for (i = 0; i < 256; i++)
	{ 
		for (j=0;j<256;j++)
		{
			if (peaks[i][j] == 255) peaks[i][j] = 255;
			else peaks[i][j] = 0;
			fprintf(file2,"%c",(char)((int)(peaks[i][j])));
		}
	}

	// Canny: P4
	// initializing percent to rubric's required value
	double percent = 0.05, crop, toparea;
	int curve[PICSIZE] = { 0 };

	for (i = 0; i < 256; i++)
		for (j = 0; j < 256; j++)
			curve[(int)ival[i][j]] += 1;
	
	// assigning values for crop & toparea
	crop = percent * 256 * 256;
	toparea = 0;

	// determining top area using reverse for loop
	for (i = 255; i > 0; i--)
	{
		toparea += curve[i];
		if (toparea > crop) break;
	}

	// assigning threshold values
	hiThreshold = i;
	loThreshold = i * 0.15; // changed from 0.35 (sharper edges)

	// Canny: P3
	for (i = 0; i < 256; i++)
		for (j = 0; j < 256; j++)
			if (peaks[i][j] == 255)
			{
				if (ival[i][j] > hiThreshold)
				{
					peaks[i][j] = 0;
					finishedEdges[i][j] = 255;
				}
				else if (ival[i][j] < loThreshold)
				{
					peaks[i][j] = 0;
					finishedEdges[i][j] = 0;
				}
			}
	flag = 1;
	while(flag == 1)
	{	
		flag = 0;
		for (i = 0; i < 256; i++)
			for (j = 0; j < 256; j++)
				if (peaks[i][j] == 255)
				{
					for (u = -1; u <= 1; u++)
						for (v = -1; v <= 1; v++)
							if (finishedEdges[i + u][j + v] == 255)
							{
								peaks[i][j] = 0;
								finishedEdges[i][j] = 255;
								flag = 1;
							}
				}
	}

	// printing to garb34finished.pgm
	for (i = 0; i < 256; i++)
	{
		for (j = 0; j < 256; j++)
		{
			fprintf(file3,"%c",(char)((int)(finishedEdges[i][j])));
		}
	}
	
	// closing file pointers and output files
	fclose(ifp);
	fclose(file1);
	fclose(file2);
	fclose(file3);

	printf("⭑ Edge detection has finished.\n⭑ Please refer to the output files for results.\n");

	return 0;
}
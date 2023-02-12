#include <stdio.h>
#include <error.h>
#include <stdlib.h>
#include <omp.h>
#include <fcntl.h>
#include <gd.h>

int main(int argc, char **argv){
	int w, h, r, g, b, tmp;
	int color, x, y, z;
	FILE *fp1, *fp2;	//infile outfile
	gdImagePtr img;	//img ptr
	w = h = r = g = b = 0;
	color = x = y = z = 0;
	if (argc != 3)
		error(1,0,"Usage : gdneg in.png out.png");
	if ((fp1 = fopen(argv[1], "r")) == NULL){
		printf("\n Error in %s\n",argv[1]);
		exit(1);
	}

	img = gdImageCreateFromPng(fp1);
	w = gdImageSX(img);
	h = gdImageSY(img);

	double t = omp_get_wtime();
	#pragma omp parallel for schedule(guided) private(x,y,color)
	for (x = 0; x <= w; x++){
		for (y = 0; y <= h; y++){
			// Get color val of pixel
			color = gdImageGetPixel(img,x,y);
			
			//Get r,g,b vals
			r = gdImageRed(img,color);
			g = gdImageGreen(img,color);
			b = gdImageBlue(img,color);

			// Allocate new color, where r = g = b			
			tmp = (r + g + b) / 3;
			r = g = b = tmp;
			color = gdImageColorAllocate(img,r,g,b);

			//Set equal intensity to pixel
			#pragma omp critical
			gdImageSetPixel(img,x,y,color);
		}
	}
	
	//Write img to outfile
	fp2 = fopen(argv[2],"w+");
	gdImagePng(img,fp2);
	
	t = omp_get_wtime() - t;
	gdImageDestroy(img);
	printf("\nDone\nTime taken => %lf\n",t);
	fclose(fp2); fclose(fp1); //close
	return 0;
}

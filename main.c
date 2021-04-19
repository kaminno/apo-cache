#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
#include <math.h>

int saturation(int, int, int, int, int);

int main(int argc, char* argv[]){
	
	char* filename = argc > 1 ? argv[1] : NULL;

	if (!filename){
		fprintf(stderr, "ERROR: missing first argument - file name\n");
		return 1;
	}

	unsigned int histogram[5] = {0, 0, 0, 0, 0};

	// load image
	FILE* f = fopen(filename, "rb");
	char ch[3];
	int width, height, maxValue;
	fscanf(f, "%s %d %d %d\n", ch, &width, &height, &maxValue);
	unsigned char* red = (unsigned char*)malloc(height * width);
	unsigned char* green = (unsigned char*)malloc(height * width);
	unsigned char* blue = (unsigned char*)malloc(height * width);
	for(int i = 0; i < width * height; i++){
		red[i] = fgetc(f);
		green[i] = fgetc(f);
		blue[i] = fgetc(f);
	}
	fclose(f);

	unsigned char* newRed = (unsigned char*)malloc(height * width);
	unsigned char* newGreen = (unsigned char*)malloc(height * width);
	unsigned char* newBlue = (unsigned char*)malloc(height * width);

	FILE* g = fopen("output.ppm", "wb");
	//FILE* g = fopen("new.ppm", "wb");
	fprintf(g, "%s\n%d\n%d\n%d\n", "P6", width, height, 255);

	for(unsigned int i = 0; i < width * height; i++){
		if(i < width || i > ((height - 1) * width) - 1 || i % width == 0 || i % width == width - 1){
			newRed[i] = red[i];
			newGreen[i] = green[i];
			newBlue[i] = blue[i];
		}
		else{
			newRed[i] = saturation(red[i], red[i-1], red[i+1], red[i-width], red[i+width]);
			newGreen[i] = saturation(green[i], green[i-1], green[i+1], green[i-width], green[i+width]);
			newBlue[i] = saturation(blue[i], blue[i-1], blue[i+1], blue[i-width], blue[i+width]);
		}
		fputc(newRed[i], g);
		fputc(newGreen[i], g);
		fputc(newBlue[i], g);

		int value = round(0.2126*newRed[i] + 0.7152*newGreen[i] + 0.0722*newBlue[i]);
		if(value <= 50){
			histogram[0] += 1;
		}
		else if(value <= 101){
			histogram[1] += 1;
		}
		else if(value <= 152){
			histogram[2] += 1;
		}
		else if(value <= 203){
			histogram[3] += 1;
		}
		else{
			histogram[4] += 1;
		}
	}

	fclose(g);

	// save histogram
	f = fopen("output.txt", "w");
	//f = fopen("histogram.txt", "w");
	fprintf(f, "%d %d %d %d %d", histogram[0], histogram[1], histogram[2], histogram[3], histogram[4]);
	fclose(f);

	free(red);
	free(green);
	free(blue);
	free(newRed);
	free(newGreen);
	free(newBlue);
	
	return 0;
}

int saturation(int centerPixel, int p1, int p2, int p3, int p4){
	int res = 5 * centerPixel - p1 - p2 - p3 - p4;
	if(res > 255){
		res = 255;
	}
	else if(res < 0){
		res = 0;
	}
	return res;
}

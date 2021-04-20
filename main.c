#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define READ_HIGHT 10

int saturation(int, int, int, int, int);
void readNextValues(FILE* f, unsigned char* red, unsigned char* green, unsigned char* blue, int width);

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

	for(int i = 0; i < width * READ_HIGHT; i++){
		red[i] = fgetc(f);
		green[i] = fgetc(f);
		blue[i] = fgetc(f);
	}

	// implement rewriting uchar array of values while reading from file
	unsigned char newRed;
	unsigned char newGreen;
	unsigned char newBlue;

	FILE* g = fopen("output.ppm", "wb");
	fprintf(g, "%s\n%d\n%d\n%d\n", "P6", width, height, 255);
	int j = width * READ_HIGHT;
	int c1 = ((height - 1) * width) - 1;
	int c2 = width - 1;
	int multi = width * height;
	int counter = 0;
	for(unsigned int i = 0; counter < multi; i++){
		if(i == j - width){
			readNextValues(f, red, green, blue, width);
			i = width;
		}
		if(counter < width || counter > c1 || i % width == 0 || i % width == c2){
			newRed = red[i];
			newGreen = green[i];
			newBlue = blue[i];
		}
		else{
			newRed = saturation(red[i], red[i-1], red[i+1], red[i-width], red[i+width]);
			newGreen = saturation(green[i], green[i-1], green[i+1], green[i-width], green[i+width]);
			newBlue = saturation(blue[i], blue[i-1], blue[i+1], blue[i-width], blue[i+width]);
		}
		fputc(newRed, g);
		fputc(newGreen, g);
		fputc(newBlue, g);
		counter++;
		int value = round(0.2126*newRed + 0.7152*newGreen + 0.0722*newBlue);
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

	// for(unsigned int i = 0; i < width * height; i++){
	// 	if(j < multi){
	// 		red[j] = fgetc(f);
	// 		green[j] = fgetc(f);
	// 		blue[j] = fgetc(f);
	// 		j++;
	// 	}
	// 	if(i < width || i > c1 || i % width == 0 || i % width == c2){
	// 		newRed = red[i];
	// 		newGreen = green[i];
	// 		newBlue = blue[i];
	// 	}
	// 	else{
	// 		newRed = saturation(red[i], red[i-1], red[i+1], red[i-width], red[i+width]);
	// 		newGreen = saturation(green[i], green[i-1], green[i+1], green[i-width], green[i+width]);
	// 		newBlue = saturation(blue[i], blue[i-1], blue[i+1], blue[i-width], blue[i+width]);
	// 	}
	// 	fputc(newRed, g);
	// 	fputc(newGreen, g);
	// 	fputc(newBlue, g);

	// 	int value = round(0.2126*newRed + 0.7152*newGreen + 0.0722*newBlue);
	// 	if(value <= 50){
	// 		histogram[0] += 1;
	// 	}
	// 	else if(value <= 101){
	// 		histogram[1] += 1;
	// 	}
	// 	else if(value <= 152){
	// 		histogram[2] += 1;
	// 	}
	// 	else if(value <= 203){
	// 		histogram[3] += 1;
	// 	}
	// 	else{
	// 		histogram[4] += 1;
	// 	}
	// }


	fclose(f);
	fclose(g);

	// save histogram
	f = fopen("output.txt", "w");
	fprintf(f, "%d %d %d %d %d", histogram[0], histogram[1], histogram[2], histogram[3], histogram[4]);
	fclose(f);

	free(red);
	free(green);
	free(blue);
	
	return 0;
}

void readNextValues(FILE* f, unsigned char* red, unsigned char* green, unsigned char* blue, int width){
	//int j = 0;
	int w = width * (READ_HIGHT - 2);
	for(int i = 0; i < 2 * width; i++){
		red[i] = red[w + i];
		green[i] = green[w + i];
		blue[i] = blue[w + i];
	}
	for(int i = 2 * width; (!feof(f)) && (i < width * READ_HIGHT); i++){
		red[i] = fgetc(f);
		green[i] = fgetc(f);
		blue[i] = fgetc(f);
	}
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

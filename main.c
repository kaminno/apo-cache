#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// #include <unistd.h>
// #include <stdio.h>
// #include <time.h>
// #define _POSIX_C_SOURCE 202001L

typedef struct{
	int width, height;
	unsigned char** red;
	unsigned char** green;
	unsigned char** blue;
} ImagePPM;

ImagePPM* loadPPM(char* filename);
void savePPM(char* filename, ImagePPM*);
ImagePPM* applyConvolution(ImagePPM*, int*);
int saturation(int, int, int, int, int);
void addToHistogram(int*, int, int, int);
void storeHisto(char*, int*);
void freeImage(ImagePPM*);

int main(int argc, char* argv[]){
	
	char* filename = argc > 1 ? argv[1] : NULL;

	if (!filename){
		fprintf(stderr, "ERROR: missing first argument - file name\n");
		return 1;
	}

	int histogram[5] = {0, 0, 0, 0, 0};
	ImagePPM* i = loadPPM(filename);
	ImagePPM* j = applyConvolution(i, histogram);

	savePPM("new.ppm", j);
	storeHisto("histo.txt", histogram);

	// savePPM("output.ppm", j);
	// storeHisto("output.txt", histogram);

	freeImage(i);
	freeImage(j);
	free(i);
	free(j);
	
	return 0;
}

void freeImage(ImagePPM* image){
	for(int i = 0; i < image->height; i++){
		free(image->red[i]);
		free(image->green[i]);
		free(image->blue[i]);
	}
	free(image->red);
	free(image->green);
	free(image->blue);
}

void storeHisto(char* filename, int* histo){
	FILE* f = fopen(filename, "w");
	fprintf(f, "%d %d %d %d %d", histo[0], histo[1], histo[2], histo[3], histo[4]);
	fclose(f);
}

void addToHistogram(int* histo, int r, int g, int b){
	int value = round(0.2126*r + 0.7152*g + 0.0722*b);
	if(value <= 50){
		histo[0] += 1;
	}
	else if(value <= 101){
		histo[1] += 1;
	}
	else if(value <= 152){
		histo[2] += 1;
	}
	else if(value <= 203){
		histo[3] += 1;
	}
	else if(value <= 255){
		histo[4] += 1;
	}
	else{
		fprintf(stderr, "Error: wrong histo value!\n");
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

ImagePPM* applyConvolution(ImagePPM* sourceImage, int* histo){
	ImagePPM* modyfiedImage = (ImagePPM*)malloc(sizeof(ImagePPM));

	modyfiedImage->red = (unsigned char**)malloc(sizeof(unsigned char*) * sourceImage->height);
	modyfiedImage->green = (unsigned char**)malloc(sizeof(unsigned char*) * sourceImage->height);
	modyfiedImage->blue = (unsigned char**)malloc(sizeof(unsigned char*) * sourceImage->height);

	// for(int i = 0; i < sourceImage->height; i++){
	// 	modyfiedImage->red[i] = (unsigned char*)malloc(sourceImage->width);
	// 	for(int j = 0; j < sourceImage->width; j++){
	// 		if(i == 0 || i == sourceImage->height - 1 || j == 0 || j == sourceImage->width - 1){
	// 			modyfiedImage->red[i][j] = sourceImage->red[i][j];
	// 		}
	// 		else{
	// 			modyfiedImage->red[i][j] = saturation(sourceImage->red[i][j], sourceImage->red[i-1][j], sourceImage->red[i+1][j],
	// 													sourceImage->red[i][j-1], sourceImage->red[i][j+1]);
	// 		}
	// 	}
	// }
	// for(int i = 0; i < sourceImage->height; i++){
	// 	modyfiedImage->green[i] = (unsigned char*)malloc(sourceImage->width);
	// 	for(int j = 0; j < sourceImage->width; j++){
	// 		if(i == 0 || i == sourceImage->height - 1 || j == 0 || j == sourceImage->width - 1){
	// 			modyfiedImage->green[i][j] = sourceImage->green[i][j];
	// 		}
	// 		else{
	// 			modyfiedImage->green[i][j] = saturation(sourceImage->green[i][j], sourceImage->green[i-1][j], sourceImage->green[i+1][j],
	// 													sourceImage->green[i][j-1], sourceImage->green[i][j+1]);
	// 		}
	// 	}
	// }
	// for(int i = 0; i < sourceImage->height; i++){
	// 	modyfiedImage->blue[i] = (unsigned char*)malloc(sourceImage->width);
	// 	for(int j = 0; j < sourceImage->width; j++){
	// 		if(i == 0 || i == sourceImage->height - 1 || j == 0 || j == sourceImage->width - 1){
	// 			modyfiedImage->blue[i][j] = sourceImage->blue[i][j];
	// 		}
	// 		else{
	// 			modyfiedImage->blue[i][j] = saturation(sourceImage->blue[i][j], sourceImage->blue[i-1][j], sourceImage->blue[i+1][j],
	// 													sourceImage->blue[i][j-1], sourceImage->blue[i][j+1]);
	// 		}
	// 	}
	// }

	for(int i = 0; i < sourceImage->height; i++){
		modyfiedImage->red[i] = (unsigned char*)malloc(sourceImage->width);
		modyfiedImage->green[i] = (unsigned char*)malloc(sourceImage->width);
		modyfiedImage->blue[i] = (unsigned char*)malloc(sourceImage->width);
		for(int j = 0; j < sourceImage->width; j++){
			if(i == 0 || i == sourceImage->height - 1 || j == 0 || j == sourceImage->width - 1){
				modyfiedImage->red[i][j] = sourceImage->red[i][j];
				modyfiedImage->green[i][j] = sourceImage->green[i][j];
				modyfiedImage->blue[i][j] = sourceImage->blue[i][j];
			}
			else{
				int r = 5 * sourceImage->red[i][j] - sourceImage->red[i-1][j] - sourceImage->red[i+1][j]
														- sourceImage->red[i][j-1] - sourceImage->red[i][j+1];
				if(r > 255){
					modyfiedImage->red[i][j] = 255;
				}
				else if(r < 0){
					modyfiedImage->red[i][j] = 0;
				}
				else{
					modyfiedImage->red[i][j] = r;
				}
				int g = 5 * sourceImage->green[i][j] - sourceImage->green[i-1][j] - sourceImage->green[i+1][j]
														- sourceImage->green[i][j-1] - sourceImage->green[i][j+1];
				if(g > 255){
					modyfiedImage->green[i][j] = 255;
				}
				else if(g < 0){
					modyfiedImage->green[i][j] = 0;
				}
				else{
					modyfiedImage->green[i][j] = g;
				}
				int b = 5 * sourceImage->blue[i][j] - sourceImage->blue[i-1][j] - sourceImage->blue[i+1][j]
														- sourceImage->blue[i][j-1] - sourceImage->blue[i][j+1];
				if(b > 255){
					modyfiedImage->blue[i][j] = 255;
				}
				else if(b < 0){
					modyfiedImage->blue[i][j] = 0;
				}
				else{
					modyfiedImage->blue[i][j] = b;
				}
				
				// modyfiedImage->red[i][j] = saturation(sourceImage->red[i][j], sourceImage->red[i-1][j], sourceImage->red[i+1][j],
				// 										sourceImage->red[i][j-1], sourceImage->red[i][j+1]);
				// modyfiedImage->green[i][j] = saturation(sourceImage->green[i][j], sourceImage->green[i-1][j], sourceImage->green[i+1][j],
				// 										sourceImage->green[i][j-1], sourceImage->green[i][j+1]);
				// modyfiedImage->blue[i][j] = saturation(sourceImage->blue[i][j], sourceImage->blue[i-1][j], sourceImage->blue[i+1][j],
				// 										sourceImage->blue[i][j-1], sourceImage->blue[i][j+1]);
			}
			//addToHistogram(histo, modyfiedImage->red[i][j], modyfiedImage->green[i][j], modyfiedImage->blue[i][j]);
			int value = round(0.2126*modyfiedImage->red[i][j] + 0.7152*modyfiedImage->green[i][j] + 0.0722*modyfiedImage->blue[i][j]);
			if(value <= 50){
				histo[0] += 1;
			}
			else if(value <= 101){
				histo[1] += 1;
			}
			else if(value <= 152){
				histo[2] += 1;
			}
			else if(value <= 203){
				histo[3] += 1;
			}
			else if(value <= 255){
				histo[4] += 1;
			}
		}
	}

	modyfiedImage->width = sourceImage->width;
	modyfiedImage->height = sourceImage->height;

	return modyfiedImage;
}

ImagePPM* loadPPM(char* filename){
	FILE* f = fopen(filename, "rb");
	ImagePPM* image = (ImagePPM*)malloc(sizeof(ImagePPM));
	char one[3];
	int maxVal;
	fscanf(f, "%s %d %d %d\n", one, &image->width, &image->height, &maxVal);
//	printf("Size: %d, %d\n", image->width, image->height);
	image->red = (unsigned char**)malloc(sizeof(unsigned char*) * image->height);
	image->green = (unsigned char**)malloc(sizeof(unsigned char*) * image->height);
	image->blue = (unsigned char**)malloc(sizeof(unsigned char*) * image->height);
	for(int i = 0; i < image->height; i++){
		image->red[i] = (unsigned char*)malloc(image->width);
		image->green[i] = (unsigned char*)malloc(image->width);
		image->blue[i] = (unsigned char*)malloc(image->width);
		for(int j = 0; j < image->width; j++){
			image->red[i][j] = getc(f);
			image->green[i][j] = getc(f);
			image->blue[i][j] = getc(f);
			//printf("%d %d %d\n", image->red[i][j], image->green[i][j], image->blue[i][j]);
		}
	}
	fclose(f);
	return image;
}

void savePPM(char* filename, ImagePPM* image){
	FILE* f = fopen(filename, "wb");
	fprintf(f, "%s\n%d %d\n%d\n", "P6", image->width, image->height, 255);
	for(int i = 0; i < image->height; i++){
		for(int j = 0; j < image->width; j++){
			putc(image->red[i][j], f);
			putc(image->green[i][j], f);
			putc(image->blue[i][j], f);
		}
	}
	fclose(f);
}

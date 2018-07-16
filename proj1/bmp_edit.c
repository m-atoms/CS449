/*******************************
 * Author: Michael Adams
 * Last Edit: 2/11/18
 ******************************/
#include <stdio.h>
#include <math.h>
#include <string.h>

#pragma pack(1)

#define DIB_SIZE 40
#define PX_SIZE 3

// BMPHeader struct def - 14 bytes
typedef struct BMPHeader {
	char format_id[2];
	int file_size;
	char res0[2];
	char res1[2];
	int offset;
}BMPHeader;

// DIBHeader struct def - 40 bytes
typedef struct DIBHeader {
	int header_size;
	int px_width;
	int px_height;
	short color_planes;
	short bits_per_px;
	int compression;
	int image_size;
	int h_res;
	int v_res;
	int num_colors;
	int important_colors;
}DIBHeader;

// Pixel struct def - 24 bytes
typedef struct Pixel {
	unsigned char blue;
	unsigned char green;
	unsigned char red;
}Pixel;

/*
 * check BMP format identifier 
 */
int bmp_check(BMPHeader *bmp) {
	if ((*bmp).format_id[0] == 'B' && (*bmp).format_id[1] == 'M') {
		return 0;
	}
	else {
		return -1;
	}
}

/*
 * check DIB byte size
 */
int dib_check(DIBHeader *dib) {
	if (DIB_SIZE == (*dib).header_size) {
		return 0;
	}
	else {
		return -1;
	}
}

/*
 * invert px colors
 */
void invert(Pixel *px) {
	// bitwise NOT each pixel color
	(*px).blue = ~((*px).blue);
	(*px).green = ~((*px).green);
	(*px).red = ~((*px).red);
}

/*
 * convert to grayscale by doing some weird math
 */
void grayscale(Pixel *px) {
	// normalize - dividing by a double literal should force to float I think
	float blue = (float)((*px).blue)/255.0;
	float green = (float)((*px).green)/255.0;
	float red = (float)((*px).red)/255.0;

	// calculate y value
	float y = ((0.2126 * red) + (0.7152 * green) + (0.0722 * blue));

	// do mystery transform
	if (y <= 0.0031308) {
		y = (float)(12.92 * y);
	}
	else {
		y = (float)(1.055 * pow((double)y, (double)(1.0/2.4)) - 0.055);
	}

	// set pixel equal to y value
	(*px).blue = (char)(y * 255.0);
	(*px).green = (char)(y * 255.0);
	(*px).red = (char)(y * 255.0);
}

int main(int argc, char *argv[]) 
{
	// check expected argument num
	if (argc < 3) {
		printf("Error: missing expected arguments to bmp_edit\n");
		return -1;
	}

	// store command line args
	char transform[50];
	char filename[50];
	strcpy(transform, argv[1]);
	strcpy(filename, argv[2]);

	// check valid transform
	//printf("the transform: %s\n", transform);
	//printf("compare inv: %d\n", strcmp(transform, "-invert"));
	//printf("compare gray: %d\n", strcmp(transform, "-grayscale"));
	if (strcmp(transform, "-invert") && strcmp(transform, "-grayscale")) { //only execute if return !=0 which means error
		printf("Error: invalid transform operation\n");
		return -1;
	}

	// create file pointer
	FILE *f;
	f = fopen(filename, "rb+");
	
	// check BMP header
	BMPHeader bmp;
	fread(&bmp, sizeof(BMPHeader), 1, f);
	
	if (bmp_check(&bmp)) { //only execute if return !=0 which means error
		printf("Error: file type is not supported\n");
		return -1;
	}
	
	// check DIB header
	DIBHeader dib;
	fread(&dib, sizeof(DIBHeader), 1, f);
	
	if (dib_check(&dib)) {
		printf("Error: file type is not supported\n");
		return -1;
	}
	
	// calculate padding value
	int pad = ((PX_SIZE * dib.px_width) % 4);
	if (pad > 0) {
		pad = 4 - pad;
	}

	// iterate through pixel array and process according to transform
	for (int i = 0; i < dib.px_height; i++) {
		for (int j = 0; j < dib.px_width; j++) {

			// read pixel
			Pixel px;
			fread(&px, sizeof(px), 1, f);

			// perform transform
			if (!strcmp(transform, "-invert")) {
				invert(&px);
			}
			else if (!strcmp(transform, "-grayscale")) {
				grayscale(&px);
			}

			// move pointer back to start of current pixel
			fseek(f, -(sizeof(px)), SEEK_CUR);

			// write new pixel to file
			fwrite(&px, sizeof(px), 1, f);
		}
		// at end of row, skip padding if necessary
		fseek(f, pad, SEEK_CUR);
	}
	return 0;
}
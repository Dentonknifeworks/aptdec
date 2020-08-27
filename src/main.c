/* 
 *  This file is part of Aptdec.
 *  Copyright (c) 2004-2009 Thierry Leconte (F4DWV), Xerbo (xerbo@protonmail.com) 2019-2020
 *
 *  Aptdec is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <libgen.h>
#include <math.h>
#include <sndfile.h>
#include <errno.h>
#include <time.h>

#include "common.h"
#include "offsets.h"

// DSP
extern int init_dsp(double F);
extern int getpixelrow(float *pixelv, int nrow, int *zenith, int reset);

// I/O
extern int readRawImage(char *filename, float **prow, int *nrow);
extern int ImageOut(options_t *opts, image_t *img, int offset, int width, char *desc, char chid, char *palette);
extern int initWriter(options_t *opts, image_t *img, int width, int height, char *desc, char *chid);
extern void pushRow(float *row, int width);
extern void closeWriter();

// Image functions
extern int calibrate(float **prow, int nrow, int offset, int width);
extern void histogramEqualise(float **prow, int nrow, int offset, int width);
extern void linearEnhance(float **prow, int nrow, int offset, int width);
extern void temperature(options_t *opts, image_t *img, int offset, int width);
extern void denoise(float **prow, int nrow, int offset, int width);
extern void distrib(options_t *opts, image_t *img, char chid);
extern void flipImage(image_t *img, int width, int offset);
extern void cropNoise(image_t *img);

// Palettes
extern char GviPalette[256*3];
extern char TempPalette[256*3];

// Audio file
static SNDFILE *audioFile;
// Number of channels in audio file
int channels = 1;

// Function declarations
static int initsnd(char *filename);
int getsample(float *sample, int nb);
static int processAudio(char *filename, options_t *opts);
static void usage(void);

int main(int argc, char **argv) {
	fprintf(stderr, VERSION"\n");

	// Check if there are actually any input files
	if(argc == optind || argc == 1){
		fprintf(stderr, "No input files provided.\n");
		usage();
	}

	options_t opts = { "r", "", 19, "", ".", 0, "", "", 1.0, 0 };

	// Parse arguments
	int opt;
	while ((opt = getopt(argc, argv, "o:m:d:i:s:e:p:g:k:r")) != EOF) {
		switch (opt) {
			case 'd':
				opts.path = optarg;
				break;
			case 'm':
				opts.map = optarg;
				break;
			case 'i':
				opts.type = optarg;
				break;
			case 's':
				opts.satnum = atoi(optarg);
				if(opts.satnum < 15 || opts.satnum > 19){
					fprintf(stderr, "Invalid satellite number, it must be the range 15-19\n");
					exit(EPERM);
				}
				break;
			case 'e':
				opts.effects = optarg;
				break;
			case 'r':
				opts.realtime = 1;
				break;
			case 'o':
				opts.filename = optarg;
				break;
			case 'p':
				opts.palette = optarg;
				break;
			case 'g':
				opts.gamma = atof(optarg);
				break;
			case 'k':
				opts.mapOffset = atoi(optarg);
				break;
			default:
				usage();
		}
	}

	// Process the files
	for (; optind < argc; optind++) {
		processAudio(argv[optind], &opts);
	}

	exit(0);
}

static int processAudio(char *filename, options_t *opts){
	// Image info struct
	image_t img;

	// Mapping between wedge value and channel ID
	static struct {
		char *id[7];
		char *name[7];
	} ch = {
		{ "?", 		 "1", 	   "2", 			"3A", 			"4", 				"5", 				"3B"		   },
		{ "unknown", "visble", "near-infrared", "mid-infrared", "thermal-infrared", "thermal-infrared", "mid-infrared" }
	};

	// Buffer for image channel
	char desc[60];

	// Parse file path
	char path[256], extension[32];
	strcpy(path, filename);
	strcpy(path, dirname(path));
	sscanf(basename(filename), "%255[^.].%31s", img.name, extension);

	if(opts->realtime){
		// Set output filename to current time when in realtime mode
		time_t t;
		time(&t);
		strncpy(img.name, ctime(&t), 24);

		// Init a row writer
		initWriter(opts, &img, IMG_WIDTH, MAX_HEIGHT, "Unprocessed realtime image", "r");
	}		

	if(strcmp(extension, "png") == 0){
		// Read PNG into image buffer
		printf("Reading %s\n", filename);
		if(readRawImage(filename, img.prow, &img.nrow) == 0){
			exit(EPERM);
		}
	}else{
		// Attempt to open the audio file
		if (initsnd(filename) == 0)
			exit(EPERM);

		// Build image
		// TODO: multithreading, would require some sort of input buffer
		for (img.nrow = 0; img.nrow < MAX_HEIGHT; img.nrow++) {
			// Allocate memory for this row
			img.prow[img.nrow] = (float *) malloc(sizeof(float) * 2150);

			// Write into memory and break the loop when there are no more samples to read
			if (getpixelrow(img.prow[img.nrow], img.nrow, &img.zenith, (img.nrow == 0)) == 0)
				break;

			if(opts->realtime) pushRow(img.prow[img.nrow], IMG_WIDTH);

			fprintf(stderr, "Row: %d\r", img.nrow);
			fflush(stderr);
		}

		// Close stream
		sf_close(audioFile);
	}

	if(opts->realtime) closeWriter();

	printf("Total rows: %d\n", img.nrow);

	// Fallback for detecting the zenith
	// TODO: encode metadata in raw images
	if(opts->map != NULL && opts->map[0] != '\0' && img.zenith == 0){
		fprintf(stderr, "Guessing zenith in image, map will most likely be misaligned.\n");
		img.zenith = img.nrow / 2;
	}

	// Calibrate
	img.chA = calibrate(img.prow, img.nrow, CHA_OFFSET, CH_WIDTH);
	img.chB = calibrate(img.prow, img.nrow, CHB_OFFSET, CH_WIDTH);
	printf("Channel A: %s (%s)\n", ch.id[img.chA], ch.name[img.chA]);
	printf("Channel B: %s (%s)\n", ch.id[img.chB], ch.name[img.chB]);

	// Crop noise from start and end of image
	if(CONTAINS(opts->effects, Crop_Noise)){
		cropNoise(&img);
	}

	// Denoise
	if(CONTAINS(opts->effects, Denoise)){
		denoise(img.prow, img.nrow, CHA_OFFSET, CH_WIDTH);
		denoise(img.prow, img.nrow, CHB_OFFSET, CH_WIDTH);
	}

	// Flip, for northbound passes
	if(CONTAINS(opts->effects, Flip_Image)){
		flipImage(&img, CH_WIDTH, CHA_OFFSET);
		flipImage(&img, CH_WIDTH, CHB_OFFSET);
	}

	// Temperature
	if (CONTAINS(opts->type, Temperature) && img.chB >= 4) {
		// Create another buffer as to not modify the orignal
		image_t tmpimg = img;
		for(int i = 0; i < img.nrow; i++){
			tmpimg.prow[i] = (float *) malloc(sizeof(float) * 2150);
			memcpy(tmpimg.prow[i], img.prow[i], sizeof(float) * 2150);
		}

		// Perform temperature calibration
		temperature(opts, &tmpimg, CHB_OFFSET, CH_WIDTH);
		ImageOut(opts, &tmpimg, CHB_OFFSET, CH_WIDTH, "Temperature", Temperature, (char *)TempPalette);
	}

	// MCIR
	if (CONTAINS(opts->type, MCIR))
		ImageOut(opts, &img, CHA_OFFSET, CH_WIDTH, "MCIR", MCIR, NULL);

	// Linear equalise
	if(CONTAINS(opts->effects, Linear_Equalise)){
		linearEnhance(img.prow, img.nrow, CHA_OFFSET, CH_WIDTH);
		linearEnhance(img.prow, img.nrow, CHB_OFFSET, CH_WIDTH);
	}

	// Histogram equalise
	if(CONTAINS(opts->effects, Histogram_Equalise)){
		histogramEqualise(img.prow, img.nrow, CHA_OFFSET, CH_WIDTH);
		histogramEqualise(img.prow, img.nrow, CHB_OFFSET, CH_WIDTH);
	}

	// Raw image
	if (CONTAINS(opts->type, Raw_Image)) {
		sprintf(desc, "%s (%s) & %s (%s)", ch.id[img.chA], ch.name[img.chA], ch.id[img.chB], ch.name[img.chB]);
		ImageOut(opts, &img, 0, IMG_WIDTH, desc, Raw_Image, NULL);
	}

	// Palette image
	if (CONTAINS(opts->type, Palleted)) {
		img.palette = opts->palette;
		strcpy(desc, "Palette composite");
		ImageOut(opts, &img, CHA_OFFSET, 909, desc, Palleted, NULL);
	}

	// Channel A
	if (CONTAINS(opts->type, Channel_A)) {
		sprintf(desc, "%s (%s)", ch.id[img.chA], ch.name[img.chA]);
		ImageOut(opts, &img, CHA_OFFSET, CH_WIDTH, desc, Channel_A, NULL);
	}

	// Channel B
	if (CONTAINS(opts->type, Channel_B)) {
		sprintf(desc, "%s (%s)", ch.id[img.chB], ch.name[img.chB]);
		ImageOut(opts, &img, CHB_OFFSET, CH_WIDTH, desc, Channel_B, NULL);
	}

	// Value distribution image
	if (CONTAINS(opts->type, Distribution))
		distrib(opts, &img, Distribution);

	return 1;
}

static int initsnd(char *filename) {
	SF_INFO infwav;
	int	res;

	// Open audio file
	infwav.format = 0;
	audioFile = sf_open(filename, SFM_READ, &infwav);
	if (audioFile == NULL) {
		fprintf(stderr, "Could not open %s\n", filename);
		return 0;
	}

	res = init_dsp(infwav.samplerate);
	printf("Input file: %s\n", filename);
	if(res < 0) {
		fprintf(stderr, "Input sample rate too low: %d\n", infwav.samplerate);
		return 0;
	}else if(res > 0) {
		fprintf(stderr, "Input sample rate too high: %d\n", infwav.samplerate);
		return 0;
	}
	printf("Input sample rate: %d\n", infwav.samplerate);

	channels = infwav.channels;

	return 1;
}

// Read samples from the audio file
int getsample(float *sample, int nb) {
	if(channels == 1){
		return sf_read_float(audioFile, sample, nb);
	}else{
		/* Multi channel audio is encoded such as:
		 *  Ch1,Ch2,Ch1,Ch2,Ch1,Ch2
		 */
		float buf[nb * channels]; // Something like BLKIN*2 could also be used
		int samples = sf_read_float(audioFile, buf, nb * channels);
		for(int i = 0; i < nb; i++) sample[i] = buf[i * channels];
		return samples / channels;
	}
}

static void usage(void) {
	fprintf(stderr,
	"Aptdec [options] audio files ...\n"
	"Options:\n"
	" -i [r|a|b|t|m|p] Output image\n"
	"    r: Raw\n"
	"    a: Channel A\n"
	"    b: Channel B\n"
	"    t: Temperature\n"
	"    m: MCIR\n"
	"    p: Paletted image\n"
	" -e [t|h|d|p|f|l] Effects\n"
	"    t: Crop telemetry\n"
	"    h: Histogram equalise\n"
	"    d: Denoise\n"
	"    p: Precipitation\n"
	"    f: Flip image\n"
	"    l: Linear equalise\n"
	"    c: Crop noise\n"
	" -o <path>        Output filename\n"
	" -d <path>        Image destination directory.\n"
	" -s [15-19]       Satellite number\n"
	" -m <path>        Map file\n"
	" -p <path>        Path to palette\n"
	" -r               Realtime decode\n"
	" -g               Gamma adjustment (1.0 = off)\n"
	" -k               Map offset (in px, default: 0)"
	"\nRefer to the README for more infomation\n");

	exit(EINVAL);
}

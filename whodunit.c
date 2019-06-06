/*
	Program requires 2 command line arguments (input and output file names)
	from the user and reformats the input image file to remove the "noise"
	and convert it back to its original version.
*/
#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
/*
	Function expects to receive 2 command line arguments at execution from user (not including compiler name):
		- 1st argument is the name of "corrupted" file
		- 2nd argument is the value which will become the name/location of output file
*/
{
    //Confirm that 3 arguments were passed in (including program name)
    if (argc != 3)
    {
        fprintf(stderr, "Usage: copy infile outfile\n");
        return 1;
    }

    //Open input file
    FILE *inptr = fopen(argv[1], "r");

    //Return error if input file unable to open
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", argv[1]);
        return 2;
    }

    //Open output file
    FILE *outptr = fopen(argv[2], "w");

    //Return error if output file unable to open
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", argv[2]);
        return 3;
    }

    //Read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    //Read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    //Ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    //Write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    //Write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    //Determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    //Iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            //Replace all red pixels with black pixels
            if (triple.rgbtRed == 0xff && triple.rgbtGreen == 0x00 && triple.rgbtBlue == 0x00)
            {
                triple.rgbtRed = 0x00;
            }
            //Replace all white pixels with black pixels
            if (triple.rgbtRed == 0xff && triple.rgbtGreen == 0xff && triple.rgbtBlue == 0xff)
            {
                triple.rgbtRed = 0x00;
                triple.rgbtGreen = 0x00;
                triple.rgbtBlue = 0x00;
            }

            //Write RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        //Skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    //Close infile
    fclose(inptr);

    //Close outfile
    fclose(outptr);

    return 0;
}
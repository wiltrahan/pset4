/**
 * Copies a BMP piece by piece, just because.
 */

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize n infile outfile.\n");
        return 1;
    }

    // remember filenames
    int n = atoi(argv[1]);
    char *infile = argv[2];
    char *outfile = argv[3];

    if(n < 1 || n > 100) {
        fprintf(stderr, "n must be between 1 & 100\n");
        return 1;
    }

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }



    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;

    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    //hang on to orig size to use in loop down below
    int orig_width = bi.biWidth;
    int orig_height = bi.biHeight;


    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    //new header info
    bi.biHeight *= n;
    bi.biWidth *= n;

    int newPadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = ((bi.biWidth * sizeof(RGBTRIPLE)) + newPadding) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);


    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(orig_height); i < biHeight; i++) {

        for(int row = 0; row < n; row++) {

             // iterate over pixels in scanline
            for (int j = 0; j < orig_width; j++) {

                for(int col = 0; col < n; col++) {
                    // temporary storage
                    RGBTRIPLE triple;

                    // read RGB triple from infile
                    //&triple is the pointer to the struct which contains the bytes to be read
                    //inptr is the File to read from
                    fread(&triple, sizeof(RGBTRIPLE), 1, inptr);


                    // write RGB triple to outfile
                    //read FROM &triple struct write TO outptr file
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);

                    //inptr is the file to seek in, sizeof is the number of bytes to move cursor
                    //SEEK_CUR is the current position in file
                    if (col != (n - 1))
                        fseek(inptr, -sizeof(RGBTRIPLE), SEEK_CUR);
                }

            }

            // skip over padding, if any
            fseek(inptr, padding, SEEK_CUR);

            // then add it back (to demonstrate how)
            for (int k = 0; k < newPadding; k++) {
                fputc(0x00, outptr);
            }

            if(row != (n - 1))
                fseek(inptr, (-sizeof(RGBTRIPLE) * orig_width) - padding, SEEK_CUR);
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
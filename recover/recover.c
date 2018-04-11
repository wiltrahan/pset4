#include <stdio.h>
#include <stdlib.h>
#include <cs50.h>

int main(int argc, char *argv[])
{
    // typedef uint8_t  BYTE;

    if(argc != 2)
    {
        fprintf(stderr, "Usage: ./recover image\n");
        return 1;
    }

    char *infile = argv[1];
    FILE *inptr = fopen(infile, "r");

    if(inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    int picsFound = 0;

    unsigned char buffer[512];
    char jpgName[8];
    FILE *img = fopen(jpgName, "w");

    //read 512 bytes into a buffer
    while(fread(buffer, 512, 1, inptr)){

        //is it the start a jpg
        if(buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {

            //if yes...
            //check if already found jpg
            //if yes
            //close previous file, open new one
            if(img != NULL)
            {
                fclose(img);
            }
            //if no
            //then open to write the file name
            sprintf(jpgName, "%03i.jpg", picsFound);
            //increment pics found to update jpg name
            picsFound++;
            img = fopen(jpgName, "w");
            fwrite(buffer, sizeof(char), 512, img);
        }
        else
        {
            fwrite(buffer, sizeof(char), 512, img);
        }

    }
    //close any remaining files
    fclose(img);
    fclose(inptr);
}
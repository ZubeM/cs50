/**
 * Resizes a BMP Image file.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./copy integer infile outfile\n");
        return 1;
    }

    float n = atof(argv[1]);
    char *infile = argv[2];
    char *outfile = argv[3];
    int padding = 0;
    
    if(n < 0.0 || n > 100.0)
    {
        fprintf(stderr, "Please enter a positive integer less than 100.\n");
        return 5;
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
    
    // Make sure infile is a 24-bit uncompressed BMP 4.0 file
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Improper file type.\n");
        return 4;
    }
    
    BITMAPFILEHEADER nbf;
    nbf = bf;
    
    BITMAPINFOHEADER nbi;
    nbi = bi;
    nbi.biWidth *= n;
    nbi.biHeight *= n;
    // determine padding for scanlines. Then add in the new padding.
     padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int newpadding = (4 - (nbi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    nbi.biSizeImage = (nbi.biWidth * sizeof(RGBTRIPLE) + newpadding) + abs(nbi.biHeight);
    nbf.bfSize = nbi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
  
    // write outfile's BITMAPFILEHEADER
    fwrite(&nbf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&nbi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    //allocate memory for storing scanlines
    
    RGBTRIPLE* buffer;
    if( n < 1.0)
         buffer = malloc((nbi.biWidth / n) * sizeof(RGBTRIPLE));
    else
         buffer = malloc(nbi.biWidth*sizeof(RGBTRIPLE));

    

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    { 
        int t = 0;
         // iterate over pixels in scanline
        for(int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;
            
            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // write RGB triple to outfile
            if( n < 1.0)
            {
                int halvenum = bi.biWidth / nbi.biWidth;
                for( int g = 0; g < halvenum - 1; g++)
                {
                    *(buffer + t) = triple;
                    fseek(inptr, sizeof(RGBTRIPLE), SEEK_CUR);
                    t++;
                }
            }
            else
                for(int l = 0; l < n; l++)
                {
                    *(buffer + t) = triple;
                    t++;
                }
             
        }
        
        
        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);
        
        if(n < 1.0)
        {
            fwrite(buffer, sizeof(RGBTRIPLE), nbi.biWidth, outptr);
            for(int k = 0; k < newpadding; k++)
            {
            fputc(0x00, outptr);
            }
        }
        else
            for(int m = 0; m < n; m++)
            {
                fwrite(buffer, sizeof(RGBTRIPLE), nbi.biWidth, outptr);
            
            

                // then add it back (to demonstrate how)
                for (int k = 0; k < newpadding; k++)
                {
                fputc(0x00, outptr);
                }
            }
               
    }
    
    free(buffer);

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}

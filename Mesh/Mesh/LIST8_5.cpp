#include "ip.h"
#include <stdio.h>

#include <stdlib.h>
#include <string.h>

/***************************************************************************
 * Func: mesh_warp                                                         *
 *                                                                         *
 * Desc: performs 2-pass mesh warp using Douglas Smythe's algorithm        *
 *                                                                         *
 * Params: input - input image to warp                                     *
 *         rows - number of rows in input and output image                 *
 *         cols - number of columns in input and output image              *
 *         source_mesh - mesh associated with input image                  *
 *         dest_mesh - mesh associated with desired image                  *
 *         output - pointer to allocated memory for output image           *
 ***************************************************************************/
void mesh_warp(image_ptr input, int rows, int cols,
	       mesh *source_mesh, mesh *dest_mesh, image_ptr output)
    {
    int mesh_width, mesh_height; /* dimensions of input and output meshes */
    int x, y;                    /* destination coordinates */
    int u, v;                    /* source coordiantes */
    int i;                       /* loop variable */
    int line_size;               /* size of largest image dimension */
    float *x1, *y1, *x2, *y2;    /* pointers used to create splines */
    float *xrow, *yrow;          /* pointers to row and column spline values */
    float *xcol, *ycol;          /* temporary pointers to spline rows and cols */
    float *column_ptr;           /* pointer to copy column data to image */
    float *index;                /* independent variables for spline 0,1,2..*/
    float *scale_factors;        /* scaling factors used in resampling */
    float *interpolated;         /* interpolated row or column */
    image_ptr source;            /* temporary image pointer to current source */
    image_ptr dest;              /* temporary image pointer to destination */
    image_ptr intermediate;      /* intermediate image storage */
    mesh *vert_splines;          /* storage for vertical splines */
    mesh *horz_splines;          /* storage for horizontal splines */


    /* allocate memory */
    line_size = MAX(cols, rows);
    index = (float *) malloc(line_size * sizeof(float));
    xrow = (float *) malloc(line_size * sizeof(float));
    yrow = (float *) malloc(line_size * sizeof(float));
    scale_factors  = (float *) malloc((line_size + 1) * sizeof(float));
    interpolated = (float *) malloc((line_size + 1) * sizeof(float));

    /* variable intializations */
    mesh_width = source_mesh->width;
    mesh_height = source_mesh->height;
    for(i=0; i<line_size; i++)
	index[i] = i;

    /* create vertical splines */
    vert_splines = malloc(sizeof(mesh));
    vert_splines->width = mesh_width;
    vert_splines->height = rows;
    vert_splines->x_data = malloc(sizeof(float) * mesh_width * rows);
    vert_splines->y_data = malloc(sizeof(float) * mesh_width * rows);
    if((vert_splines == NULL) || (vert_splines->x_data == NULL) || (vert_splines->y_data == NULL))
	{
	printf("Failed to malloc memory for vert_splines\n");
	exit(1);
	}

    for(u=0; u < mesh_width; u++)
	{
	/* set up pointers for spline function */
	xcol = (float *) source_mesh->x_data + u;
	ycol = (float *) source_mesh->y_data + u;
	column_ptr = (float *) vert_splines->x_data + u;

	/* initialize xrow and yrow for spline function */
	for(v=0; v < mesh_height; v++)
	    {
	    xrow[v] = *xcol;
	    yrow[v] = *ycol;
	    xcol += mesh_width;
	    ycol += mesh_width;
	    }

	/* generate source vertical spline */
	interp(yrow, xrow, index, interpolated, rows);

	/* store resampled row back into vert_slines */
	for(y=0; y < rows; y++)
	    {
	    *column_ptr = interpolated[y];
	    column_ptr += mesh_width;
	    }
	}

    /* create vertical splines */
    for(u=0; u < mesh_width; u++)
	{
	/* set up pointers for spline function */
	xcol = (float *) dest_mesh->x_data + u;
	ycol = (float *) dest_mesh->y_data + u;
	column_ptr = (float *) vert_splines->y_data + u;

	/* initialize xrow and yrow */
	for(v=0; v < mesh_height; v++)
	    {
	    xrow[v] = *xcol;
	    yrow[v] = *ycol;
	    xcol += mesh_width;
	    ycol += mesh_width;
	    }

	/* generate destination vertical spline */
	interp(yrow, xrow, index, interpolated, rows);

	/* store data back into vert_splines */
	for(y=0; y < rows; y++)
	    {
	    *column_ptr = interpolated[y];
	    column_ptr += mesh_width;
	    }
	}

    /* first pass: warp x using vert_splines */
    intermediate = (image_ptr) IP_MALLOC((unsigned long) cols * rows);
    if(intermediate == NULL)
	{
	printf("Unable to allocate memory for intermediate buffer\n");
	exit(1);
	}
    x1  = (float *) vert_splines->x_data;
    x2  = (float *) vert_splines->y_data;
    source = input;
    dest = intermediate;

    for(y=0; y < rows; y++)
	{
	/* fit spline to x-intercepts; resample over all cols */
	interp(x1, x2, index, scale_factors, cols);
	scale_factors[cols]=cols;    /* initialize last element */
	fant_resample(source, cols, 1, scale_factors, dest);

	/* advance all pointers to next row */
	source += cols;
	dest += cols;
	x1  += mesh_width;
	x2  += mesh_width;
	}

    /* deallocate vert_splines */
    free(vert_splines->x_data);
    free(vert_splines->y_data);
    free(vert_splines);

    /* create table of y-intercepts for intermediate mesh's hor splines */

    /* allocate memory for horz_splines */
    horz_splines = malloc(sizeof(mesh));
    horz_splines->width = cols;
    horz_splines->height = mesh_height;
    horz_splines->x_data = (float *) malloc(sizeof(float) * cols * mesh_height);
    horz_splines->y_data = (float *) malloc(sizeof(float) * cols * mesh_height);
    if((horz_splines == NULL) || (horz_splines->x_data == NULL) || (horz_splines->y_data == NULL))
	{
	printf("Failure to malloc memory for horz_splines\n");
	exit(1);
	}

    /* initialize pointers for spline function */
    x1 = (float *) source_mesh->x_data;
    y1 = (float *) source_mesh->y_data;
    y2 = (float *) horz_splines->x_data;

    for(v=0; v < mesh_height; v++)
	{
	/* generate horizontal spline */
	interp(x1, y1, index, y2, cols);

	/* advance pointers to next row */
	x1 += mesh_width;
	y1 += mesh_width;
	y2 += cols;
	}

    /* initialize pointers for spline funtion */
    x1 = (float *) dest_mesh->x_data;
    y1 = (float *) dest_mesh->y_data;
    y2 = (float *) horz_splines->y_data;

    for(v=0; v < mesh_height; v++)
	{
	/* generate horizontal splines */
	interp(x1, y1, index, y2, cols);

	/* advance pointers to next row */
	x1 += mesh_width;
	y1 += mesh_width;
	y2 += cols;
	}

    /* second pass: warp y */

    /* initialize image pointers */
    source = intermediate;
    dest = output;

    for(x=0; x < cols; x++)
	{
	/* initialize pointers for spline function */
	xcol = (float *) horz_splines->x_data + x;
	ycol = (float *) horz_splines->y_data + x;

	/* initialize xrow and yrow */
	for(v=0; v < mesh_height; v++)
	    {
	    xrow[v] = *xcol;
	    yrow[v] = *ycol;
	    xcol += cols;
	    ycol += cols;
	    }

	/* generate scale factors for image resampling */
	interp(xrow, yrow, index, scale_factors, rows);

	scale_factors[rows]=rows; /* initialize last element */

	/* resample source column based on scale_factors */
	fant_resample(source, rows, cols, scale_factors, dest);

	/* advance pointers to next column */
	source++;
	dest++;
	}

    /* deallocate horz_splines */
    free(horz_splines->x_data);
    free(horz_splines->y_data);
    free(horz_splines);

    /* free dynamically allocated memory */
    free(intermediate);
    free(index);
    free(xrow);
    free(yrow);
    free(scale_factors);
    free(interpolated);
    }



	int main()
	{

		return	1;
	}
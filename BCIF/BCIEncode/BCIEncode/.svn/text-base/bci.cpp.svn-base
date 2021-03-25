// bci.cpp
// Copyright (C) 2007, Bootstrap Studios.
//
// For use, please see the accompanying readme.
//
#include "bci.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "../libjpeg/jpeglib.h"
#ifdef __cplusplus
}
#endif // __cplusplus

#include "../zlib/zlib.h"

// zip file loader.
#include "zip.h"

#include <algorithm>
#include <math.h>

#define CACHE_FILE_NAME		"cache.tmp"
#define JPEG_FILE_NAME		"jpg.tmp"

static void GenMipmap( FILE* temp, unsigned int srcWidth, unsigned int srcHeight, unsigned int bpp );
static bool WriteZippedTGAChunk( zip_t* archive, FILE* temp, unsigned int srcWidth, unsigned int dstWidth, unsigned int dstHeight,
								 unsigned int bpp, const char* name, bool normalmap );
static bool WriteZippedJPGChunk( zip_t* archive, FILE* temp, unsigned int srcWidth, unsigned int dstWidth, unsigned int dstHeight,
								 unsigned int bpp, const char* name, unsigned int quality );
static void CalcCoverageWeights( float* weights, double startX, double dx, unsigned int x, unsigned int count );
static int CompareFileNames( const void *lhs, const void *rhs );

//--------------------------------------------------------------------
bool InitPNGStream( PNG_src_stream_t* stream, FILE* fp )
{
	// initialize the png read struct.
	stream->png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );
	if ( stream->png_ptr == NULL )
		return false;

	// initialize the png info struct.
	stream->png_info_ptr = png_create_info_struct( stream->png_ptr );
	if ( stream->png_info_ptr == NULL )
	{
		png_destroy_read_struct( &stream->png_ptr, png_infopp_NULL, png_infopp_NULL );
		return false;
	}

	// initialize the png jmpbuf config.
	if ( setjmp( png_jmpbuf( stream->png_ptr ) ) )
	{
		png_destroy_read_struct( &stream->png_ptr, &stream->png_info_ptr, png_infopp_NULL );
		return false;
	}

	// setup the png i/o method to use c streams.
	png_init_io( stream->png_ptr, fp );

	// now we need to setup the streaming of PNG data.  We can't
	// count on the entire file fitting in memory.
	png_read_info( stream->png_ptr, stream->png_info_ptr );
	png_get_IHDR( stream->png_ptr, stream->png_info_ptr, &stream->width, &stream->height,
		&stream->bit_depth, &stream->color_type, &stream->interlace_type, int_p_NULL, int_p_NULL );

	// apply color transforms.
	if ( stream->color_type == PNG_COLOR_TYPE_PALETTE )
		png_set_palette_to_rgb( stream->png_ptr );

	// Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel.
	if ( stream->color_type == PNG_COLOR_TYPE_GRAY && stream->bit_depth < 8 )
		png_set_gray_1_2_4_to_8( stream->png_ptr );

	// Expand paletted or RGB images with transparency to full alpha channels
    // so the data will be available as RGBA quartets.
	if ( png_get_valid( stream->png_ptr, stream->png_info_ptr, PNG_INFO_tRNS ) )
		png_set_tRNS_to_alpha( stream->png_ptr );

	// swizzle RGB pixels to BGR (or RGBA to BGRA).
	if ( stream->color_type & PNG_COLOR_MASK_COLOR )
		png_set_bgr( stream->png_ptr );

	// set the number of remaining passes.
	stream->number_passes = png_set_interlace_handling( stream->png_ptr );

	// return true to indicate success.
	return true;
}

//--------------------------------------------------------------------
bool WriteBCIImage( FILE* fp, PNG_src_stream_t* source, unsigned int quality, bool normalmap )
{
	zip_t zip;
	Zip_Create( &zip );

	// open our temp file.
	FILE* temp = 0;
	tmpfile_s( &temp );
	if ( !temp )
	{
		printf( "Error: unable to open temp file!\n" );
		return false;
	}

	// round the image size up to the nearest multiple of 128.
	unsigned int dstWidth = source->width;
	unsigned int dstHeight = source->height;

	// temporary buffer for each line in the PNG image.
	unsigned int bytesPerPel = std::max( source->bit_depth, 8 ) / 8;
	bytesPerPel *= ( ( source->color_type & PNG_COLOR_MASK_COLOR ) != 0 ) ? 3 : 1;
	bytesPerPel += ( ( source->color_type & PNG_COLOR_MASK_ALPHA ) != 0 ) ? 1 : 0;

	unsigned int lineSize = dstWidth * bytesPerPel;
	unsigned char* buffer = new unsigned char[ lineSize ];
//	memset( buffer, 0, totalSize );

	// de-interlace the image into a temporary file.  Note that if we
	// save as a targa, then we flip the image along Y.
	bool saveAsTGA = ( quality == BCI_QUALITY_LOSSLESS || bytesPerPel == 4 );
	for ( int pass = 0; pass < source->number_passes; ++pass )
	{
		// allocate enough memory for one line.
		for ( unsigned int y = 0; y < source->height; ++y )
		{
			// read in the current row, deinterlace it, then write it back out.
			unsigned int addr = ( saveAsTGA ? ( source->height - y - 1 ) : y ) * lineSize;

			// if we're on the second pass, read in the values from the first pass.
			if ( pass > 0 )
			{
				fseek( temp, addr, SEEK_SET );
				fread( buffer, lineSize, 1, temp );
			}

			// read the current row.
			png_read_rows( source->png_ptr, &buffer, png_bytepp_NULL, 1 );

			// write out the current row to the end of the image.  Note that
			// we're flipping the image so that the origin is in the lower left.
			fseek( temp, addr, SEEK_SET );
			fwrite( buffer, lineSize, 1, temp );
		}

		// flush after each line.
		fflush( temp );

		// reset back to the beginning of the temp file.  Is this necessary?
		rewind( temp );
	}

	// save mipmaps.
	char namebuf[ 256 ];
	unsigned int mipLevel = 0;
	while ( dstWidth > 0 || dstHeight > 0 )
	{
		// make sure the image is in the valid range.
		dstWidth = std::max( dstWidth, 1U );
		dstHeight = std::max( dstHeight, 1U );
		lineSize = dstWidth * bytesPerPel;

		// write out chunks.
		unsigned int chunksY = ( dstHeight + ( BCI_CHUNK_SIZE - 1 ) ) / BCI_CHUNK_SIZE;
		unsigned int chunksX = ( dstWidth + ( BCI_CHUNK_SIZE - 1 ) ) / BCI_CHUNK_SIZE;
		for ( unsigned int y = 0; y < chunksY; ++y )
		{
			for ( unsigned int x = 0; x < chunksX; ++x )
			{
				unsigned int tileWidth = std::min( BCI_CHUNK_SIZE, dstWidth );
				unsigned int tileHeight = std::min( BCI_CHUNK_SIZE, dstHeight );

				// seek to the right location in the file.
				unsigned int offset = BCI_CHUNK_SIZE * ( y * lineSize + x * bytesPerPel );

				// if we're doing lossless quality, write out 64x64 targas.
				if ( saveAsTGA )
				{
					fseek( temp, offset, SEEK_SET );
					sprintf_s( namebuf, 256, "mip%02d/chunk_%03d_%03d.tga", mipLevel, x, y );
					WriteZippedTGAChunk( &zip, temp, dstWidth, tileWidth, tileHeight, bytesPerPel, namebuf, normalmap );
				}
				else
				{
					fseek( temp, offset, SEEK_SET );
					sprintf_s( namebuf, 256, "mip%02d/chunk_%03d_%03d.jpg", mipLevel, x, y );
					WriteZippedJPGChunk( &zip, temp, dstWidth, tileWidth, tileHeight, bytesPerPel, namebuf, quality );
				}
			}
		}

		// rewind to the beginning of the file data and generate the
		// next mipmap.
		rewind( temp );
		GenMipmap( temp, dstWidth, dstHeight, bytesPerPel );
		rewind( temp );

		// adjust the width and height and increment the mip level
		// count.
		dstWidth >>= 1;
		dstHeight >>= 1;
		++mipLevel;
	}

	// free our buffer and close the temp file.
	delete[] buffer;
	fclose( temp );

	// clear the cache file.
	fopen_s( &temp, CACHE_FILE_NAME, "wb" );
	fclose( temp );

	// save our zip file and free it's resources.
	Zip_Save( fp, &zip );
	Zip_Free( &zip );

	fflush( fp );
	rewind( fp );

	{
		zip_t zip2;
		Zip_Create( &zip2 );
		Zip_Load( &zip2, fp );

		void* dst = 0;
		zip_file_header_t *file = Zip_FindFile( &zip2, "mip00/chunk_001_003.tga" );
		if ( file )
		{
			size_t size = Zip_ReadFile( &dst, file );
			if ( size > 0 )
			{
				FILE* fpTest = 0;
				fopen_s( &fpTest, "data/chunk_001_003.tga", "wb" );
				fwrite( dst, size, 1, fpTest );
				fclose( fpTest );
				free( dst );
			}
		}
		Zip_Free( &zip2 );
	}

	// return true to indicate success.
	return true;
}

//--------------------------------------------------------------------
void RepackBCIImage( FILE* fp )
{
	// load the zip file and simply reorder things in the best way possible.
	zip_t zip;
	Zip_Load( &zip, fp );

	// get the number of files and allocate space for their names.
	size_t fileCount = Zip_CalcFileCount( &zip );
	char **fileNames = (char **)malloc( fileCount * sizeof(char *) );

	// get the file names from the zip file.
	Zip_GetFileNames( fileNames, &zip );

	// order the file names.
	qsort( fileNames, fileCount, 4, CompareFileNames );
	Zip_Rebuild( &zip, fileNames, fileCount );

	// save and free the zip file.
	Zip_Save( fp, &zip );
	Zip_Free( &zip );
}

//--------------------------------------------------------------------
void GenMipmap( FILE* temp, unsigned int srcWidth, unsigned int srcHeight, unsigned int bpp )
{
	// calculate the destination image width.
	unsigned int dstWidth = std::max( srcWidth >> 1, 1U );
	unsigned int dstHeight = std::max( srcHeight >> 1, 1U );
	unsigned int srcStride = srcWidth * bpp;

	// get image deltas.  Note that this should probably use some
	// clever fixed point stuff.  Especially in the case where the
	// source image is in the ballpark of 256K x 256K.
	double dx = double( srcWidth ) / double( dstWidth );
	double dy = double( srcHeight ) / double( dstHeight );
	double srcX = 0.0;
	double srcY = 0.0;

	unsigned int pixelCountX = unsigned int( ceil( dx ) );
	unsigned int pixelCountY = unsigned int( ceil( dy ) );
	float weightsX[ 3 ];
	float weightsY[ 3 ];

	// allocate memory for the current mip row.
	unsigned char* rowCache = new unsigned char[ 3 * srcStride ];

	// along y...
	unsigned int srcOffset = 0;
	unsigned int srcAdvance = pixelCountY * srcStride;
	unsigned int dstOffset = 0;
	unsigned int dstAdvance = dstWidth * bpp;
	for ( unsigned int y = 0; y < dstHeight; ++y, srcY += dy, srcOffset += srcAdvance, dstOffset += dstAdvance )
	{
		// read in the source data for the current row.
		fseek( temp, srcOffset, SEEK_SET );
		fread( rowCache, pixelCountY * srcStride, 1, temp );

		// gather coverage weights along Y.
		CalcCoverageWeights( weightsY, srcY, dy, y, pixelCountY );

		// along x...
		unsigned char* dst = rowCache;
		for ( unsigned int x = 0; x < dstWidth; ++x, srcX += dx )
		{
			// gather coverage weights along X.
			CalcCoverageWeights( weightsX, srcX, dx, x, pixelCountX );

			// now do the weighted resample.
			unsigned int srcXOffset = ( unsigned int )srcX;
			unsigned char temp[4] = { 0, 0, 0, 0 };
			for ( unsigned int cy = 0; cy < pixelCountY; ++cy )
			{
				// get the current source row.
				unsigned char* src = rowCache + cy * srcStride + srcXOffset * bpp;
				for ( unsigned int cx = 0; cx < pixelCountX; ++cx )
				{
					for ( unsigned int c = 0; c < bpp; ++c, ++src )
					{
						// calculate the current weight.
						float curWeight = weightsY[ cy ] * weightsX[ cx ];
						temp[ c ] += ( unsigned char )( curWeight * float( *src ) );
					}
				}
			}

			// write the color to the array.
			for ( unsigned int c = 0; c < bpp; ++c, ++dst )
				*dst = temp[ c ];
		}

		// flush the data for the current row.
		fseek( temp, dstOffset, SEEK_SET );
		fwrite( rowCache, dstAdvance, 1, temp );
		fflush( temp );

		// reset the source X.
		srcX = 0;
	}
}

//--------------------------------------------------------------------
void CalcCoverageWeights( float* weights, double startX, double dx, unsigned int x, unsigned int count )
{
	float sum = 0.0f;
	double endX = startX + dx;
	for ( unsigned int i = 0; i < count; ++i, ++x )
	{
		float weight = 1.0f;

		// note that this could be sped up quite a bit I would think.
		if ( x < startX )
			weight = 1.0f - float( startX - double( x ) );
		else if ( ( x + 1 ) > endX )
			weight = float( endX - x );

		// store the output weight.
		weights[ i ] = weight;
		sum += weight;
	}

	// normalize weights.
	sum = 1.0f / sum;
	for ( unsigned int i = 0; i < count; ++i )
		weights[ i ] *= sum;

//	else
//		weight = 1.0f;

//	return weight;
}

//--------------------------------------------------------------------
bool WriteZippedTGAChunk( zip_t* zip, FILE* temp, unsigned int srcWidth, unsigned int dstWidth, unsigned int dstHeight,
						  unsigned int bpp, const char* name, bool normalmap )
{
#pragma pack ( push )
#pragma pack ( 1 )
	struct STGAHeaderInfo
	{
		unsigned char	idlen;    // length of optional identification sequence
		unsigned char	cmtype;   // indicates whether a palette is present
		unsigned char	imtype;   // image data type ( e.g., uncompressed RGB )
		unsigned short	cmorg;    // first palette index, if present
		unsigned short	cmcnt;    // number of palette entries, if present
		unsigned char	cmsize;   // number of bits per palette entry
		unsigned short	imxorg;   // horiz pixel coordinate of lower left of image
		unsigned short	imyorg;   // vert pixel coordinate of lower left of image
		unsigned short	imwidth;  // image width in pixels
		unsigned short	imheight; // image height in pixels
		unsigned char	imdepth;  // image color depth ( bits per pixel )
		unsigned char	imdesc;   // image attribute flags
	};
#pragma pack ( pop )

	// read in a line for the chunk.
	unsigned int srcStride = srcWidth * bpp;
	unsigned int dstStride = dstWidth * bpp;
	unsigned int imageSize = dstHeight * dstStride + sizeof( STGAHeaderInfo );
	unsigned char* buf = new unsigned char[ imageSize ];

	// write tga header data.
	STGAHeaderInfo* header = ( STGAHeaderInfo* )buf;
	header->idlen = 0;
	header->cmtype = 0;
	header->imtype = 8 | ( ( bpp > 1 ) ? 2 : 3 );	// greyscale?
	header->cmorg = 0;
	header->cmcnt = 0;
	header->cmsize = 0;
	header->imxorg = 0;
	header->imyorg = 0;
	header->imwidth = dstWidth;
	header->imheight = dstHeight;
	header->imdepth = 8 * bpp;
	header->imdesc = 0;

	// advance the output buffer past the header.
	unsigned char* dst = buf;
	dst += sizeof( STGAHeaderInfo );

	// read in each line.
	for ( unsigned int y = 0; y < dstHeight; ++y, dst += dstStride )
	{
		// read the stuff we care about and advance to the next line.
		fread( dst, dstStride, 1, temp );
		fseek( temp, srcStride - dstStride, SEEK_CUR );

		// rle encode the image.
		

		// set red to zero.
		if ( normalmap )
		{
			for ( unsigned int x = 0; x < dstWidth; ++x )
				dst[ x * bpp + 2 ] = 128;
		}
	}

	// write out the image data to our zip archive.
	//archive.AddFile( name, buf, imageSize, Archive::EM_DEFLATE );
	Zip_AddFile( zip, name, buf, imageSize, ZIP_METHOD_DEFLATE );

	// return true to indicate success.
	return true;
}

//--------------------------------------------------------------------
bool WriteZippedJPGChunk( zip_t* zip, FILE* temp, unsigned int srcWidth, unsigned int dstWidth, unsigned int dstHeight,
						  unsigned int bpp, const char* name, unsigned int quality )
{
	// read in a line for the chunk.
	unsigned int srcStride = srcWidth * bpp;
	unsigned int dstStride = dstWidth * bpp;
	unsigned char* buf = new unsigned char[ dstStride ];

	// jpeg library structures.
	jpeg_compress_struct cinfo;
	jpeg_error_mgr jerr;

	// initialize the cinfo struct.
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	// open the jpeg temp file.
	FILE* jpeg;
	fopen_s(&jpeg, JPEG_FILE_NAME, "w+b" );

	// setup defaults for the jpg image and begin compressing.
	cinfo.in_color_space = JCS_RGB;
	jpeg_stdio_dest(&cinfo, jpeg);
	jpeg_set_defaults(&cinfo);

	// begin compressing.
	cinfo.write_Adobe_marker = 0;
	cinfo.write_JFIF_header = 1;
	cinfo.err->trace_level = 0;
	cinfo.image_width = dstWidth;
	cinfo.image_height = dstHeight;
	cinfo.input_components = bpp;
	cinfo.dct_method = JDCT_ISLOW;
	jpeg_set_colorspace( &cinfo, bpp == 1 ? JCS_GRAYSCALE : JCS_RGB );
	cinfo.mem->max_memory_to_use = 1048576;
	cinfo.optimize_coding = TRUE;
	cinfo.smoothing_factor = 0;
	jpeg_set_quality(&cinfo, quality, FALSE);

//	cinfo.dct_method = JDCT_FLOAT;
	jpeg_start_compress(&cinfo, TRUE);

	// compress scanlines.
	while ( cinfo.next_scanline < dstHeight )
	{
		// read the stuff we care about and advance to the next line.
		fread( buf, dstStride, 1, temp );
		fseek( temp, srcStride - dstStride, SEEK_CUR );

		// swap BGR tp RGB?
		if ( bpp == 3 )
		{
			unsigned char* cur = buf;
			for ( unsigned int x = 0; x < dstWidth; ++x, cur += 3 )
				std::swap( cur[ 0 ], cur[ 2 ] );
		}

		// write out the jpeg data.
		jpeg_write_scanlines( &cinfo, &buf, 1 );
	}

	// finish compressing and cleanup data.
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	// get the jpeg's size.
	unsigned long jpegSize = ftell(jpeg);
	unsigned char* jpegMem = new unsigned char[ jpegSize ];

	// read in the saved jpeg data.
	fflush( jpeg );
	rewind( jpeg );
	fread( jpegMem, jpegSize, 1, jpeg );
	fclose( jpeg );

	// write out the image data to our zip archive.
//	archive.AddFile( name, jpegMem, jpegSize, Archive::EM_NONE );
	Zip_AddFile( zip, name, jpegMem, jpegSize, ZIP_METHOD_NONE );

	// cleanup.
	delete[] jpegMem;

	// return true to indicate success.
	return true;
}

//--------------------------------------------------------------------
int CompareFileNames( const void *lhs, const void *rhs )
{
	const char* lhsName = *(const char **)lhs;
	const char* rhsName = *(const char **)rhs;

	// get info about the level.
	int x0 = 0, y0 = 0, mip0 = 0;
	sscanf( lhsName, "mip%02d/chunk_%03d_%03d", &mip0, &x0, &y0 );

	int x1 = 0, y1 = 0, mip1 = 0;
	sscanf( rhsName, "mip%02d/chunk_%03d_%03d", &mip1, &x1, &y1 );

	if ( mip0 == mip1 )
	{
		__int64 key0 = 0;
		for ( unsigned int i = 0; i < 32; ++i )
			key0 |= ( ( x0 & ( 1 << i ) ) << i ) | ( ( y0 & ( 1 << i ) ) << ( i+1 ) );

		__int64 key1 = 0;
		for ( unsigned int i = 0; i < 32; ++i )
			key1 |= ( ( x1 & ( 1 << i ) ) << i ) | ( ( y1 & ( 1 << i ) ) << ( i+1 ) );

		if ( key0 == key1 )
			return 0;
		else if ( key0 < key1 )
			return -1;
		else
			return 1;
	}
	else if ( mip0 < mip1 )
		return -1;

	return 1;
}

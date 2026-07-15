/*! \file
	\author victorien ferry & www.m4nkind.com 
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
*/
#include "ImageJPEG.h"

BASEOBJECT_DECLARE_CLASS("jpg", ImageJPEG, VirtualImage,"Image File JPEG","JPGImage","Use JPEG file format to manage a image.WARNING: some 03 tags is not supported." );

ImageJPEG::ImageJPEG() : VirtualImage()
{ 
	// we give a file extension for edition mode:
	REGISTER_MEMBER_RESOURCE( mSer_JPEGFile,"JPGFile","jpg" );
	REGISTER_MEMBER_FLAG(mSer_Flags,"Flags",0,"GreyToAlpha");
}

extern "C" {
#include <stdio.h>
#include "jpeglib.h"
/*
 * <setjmp.h> is used for the optional error recovery mechanism shown in
 * the second part of the example.*/
//NOTE: setjmp couldn't be used with some GCC devkit such as the SH4 dreamcast compiler.
// all the setjmp thing was replaced by error returning.
//#include <setjmp.h>
};
typedef struct {
  struct jpeg_source_mgr pub;	/* public fields */
	const unsigned char	*m_fileimage;
	unsigned int	m_filesize;
} my_source_mgr;

typedef my_source_mgr * my_src_ptr;

// nasty error management:
struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */
  unsigned int errorcode;	// !=0 means exit code.
 // jmp_buf setjmp_buffer;	/* for return to caller */
};
typedef struct my_error_mgr * my_error_ptr;
METHODDEF(boolean) fill_input_buffer (j_decompress_ptr cinfo);
METHODDEF(void) init_source (j_decompress_ptr cinfo);
METHODDEF(void) skip_input_data (j_decompress_ptr cinfo, long num_bytes);
METHODDEF(void) term_source (j_decompress_ptr cinfo);
METHODDEF(void) my_error_exit (j_common_ptr cinfo);




//! ridiculous C context to make work the nasty setjmp() error method used by libjpg:
extern "C" {
static int CreateInternal_C(const unsigned char *pFileChunk,unsigned int fileSize, unsigned char **_ppFinalBuffer, 
							unsigned int &_outWidth,unsigned int &_outHeight,unsigned int _flags)
{
	unsigned char	*pLiner;  
	unsigned char	*pOutputBuffer = 0L ;
          
	struct jpeg_decompress_struct	cinfo;
	struct my_error_mgr				jerr;
	my_source_mgr					src;
	int initState=0;
	// ----
	src.m_fileimage = pFileChunk;
	src.m_filesize = fileSize;
	/* Initialize the JPEG decompression object with default error handling. */
	cinfo.err = jpeg_std_error(&(jerr.pub));
	jerr.pub.error_exit = my_error_exit;
	jerr.errorcode = 0;
	/* Establish the setjmp return context for my_error_exit to use. */
/*	if (setjmp(jerr.setjmp_buffer)) {
		if(initState==2) jpeg_finish_decompress(&cinfo);
		if(initState<2)jpeg_destroy_decompress(&cinfo);
		return(1);
	}*/
	jpeg_create_decompress(&cinfo);
		initState =1;
	cinfo.src = (jpeg_source_mgr *)(&src);

	src.pub.init_source			= init_source;
	src.pub.fill_input_buffer	= fill_input_buffer;
	src.pub.skip_input_data		= skip_input_data;
	src.pub.resync_to_restart	= jpeg_resync_to_restart; /* use default method */
	src.pub.term_source			= term_source;
	
	src.pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src.pub.next_input_byte = NULL; /* until buffer loaded */

	// - --------------- -
	/* Read file header, set default decompression parameters */
	jpeg_read_header(&cinfo, TRUE);
  if(jerr.errorcode != 0)
  {
		if(initState==2) jpeg_finish_decompress(&cinfo);
		if(initState<2)jpeg_destroy_decompress(&cinfo);
		return(1);
  }

	// force output format:
	cinfo.out_color_space = JCS_RGB;

	/* Step 5: Start decompressor */
	jpeg_start_decompress(&cinfo);
	if(jerr.errorcode != 0)
	{
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		return(1);
	}
	initState =2;
	if(cinfo.output_width == 0 || cinfo.output_height == 0) 
	{
         //goto error2;
	     jpeg_finish_decompress(&cinfo);
	     jpeg_destroy_decompress(&cinfo);
	     return(1);         
    }
	// we give AzurVeda dimension of the image:
	 _outWidth = cinfo.output_width;
	 _outHeight =  cinfo.output_height;

	// alloc bitmap in m_pBuffer, and exit if already constructed:
	//re if( !VirtualImage::CreateInternal()) goto error2;
	*_ppFinalBuffer = new unsigned char[ _outWidth * _outHeight * 4 ];
	 if( !(*_ppFinalBuffer))
     {// goto error2;
    	jpeg_finish_decompress(&cinfo);
    	jpeg_destroy_decompress(&cinfo);
    	return(1);     
     }
	// JSAMPLEs per row in output buffer
	unsigned int row_stride_size = cinfo.output_width * cinfo.output_components;
	// Make a one-row-high sample array that will go away when done with image 
	pOutputBuffer = *_ppFinalBuffer ;


	unsigned char *plinebuffer = new unsigned char [row_stride_size];
	if( !plinebuffer )
    {
        // goto error2;
    	jpeg_finish_decompress(&cinfo);
    	jpeg_destroy_decompress(&cinfo);
    	return(1);
    }
	while (cinfo.output_scanline < cinfo.output_height) 
	{
		jpeg_read_scanlines(&cinfo, (JSAMPARRAY) &plinebuffer, 1);
		if(jerr.errorcode != 0)
		{
			jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			return(1);
		}
		pLiner = plinebuffer;
		if(_flags&1 )
		{
			for(unsigned int ii=0;ii<cinfo.output_width;ii++)
			{
				unsigned int rr,gg,bb; 
				*pOutputBuffer++ = rr= *(pLiner++);
				*pOutputBuffer++ = gg= *(pLiner++);
				*pOutputBuffer++ = bb= *(pLiner++);
				 rr = rr+gg+bb; 
				 rr /=3;
				 if(rr>255) rr=255;
				*pOutputBuffer++ = rr;

					//1020
			}
		} else
		{
			for(unsigned int ii=0;ii<cinfo.output_width;ii++)
			{
				*pOutputBuffer++ = *(pLiner++);
				*pOutputBuffer++ = *(pLiner++);
				*pOutputBuffer++ = *(pLiner++);
				*pOutputBuffer++ = 255;
			}
		}
		//put_scanline_someplace(plinebuffer[0], row_stride);
	}
	delete [] plinebuffer;
ok:
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	return(0); // OK
	/*
error2:       
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	return(1);
	*/
}
}; 
/*!
	\brief	Method that really build the object using the serializable parameters.
			Close() should close everything opened by Create().
			Mix one or more GreyImage into a RGBA bitmap.
		\return false if creation failed.
*/
bool ImageJPEG::CreateInternal(void)
{	
	const unsigned char *pFileChunk;
	unsigned int	fileSize;
	fileSize = mSer_JPEGFile.GetByteLength();
	if(fileSize>1024*1024*2) return false; // too big for a jpeg !!
	mSer_JPEGFile.GetBinaryAtOffset(pFileChunk,fileSize);
	if(!pFileChunk)
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR_CANTOPENFILE();
		return false;
	}

	if(CreateInternal_C(pFileChunk,fileSize,&m_pBuffer,
		m_PixelWidth,m_PixelHeight,mSer_Flags.Get()) == 0) return true;
	else
	{
		BASEOBJECT_CREATEINTERNAL_EXPLICIT_ERROR("Can't decode JPEG");
		return false;
	}
	
	//test:
	/*
	m_pBuffer = new unsigned char[4*4*4];
	m_PixelWidth = 4;
	m_PixelHeight = 4;
	m_pBuffer[0]=255;
	m_pBuffer[3]=255;
	return true;
	*/
}

// note: Close() is defined in VirtualImage.
/*!
	\brief	Return the pixel width of the image
*/
unsigned int	ImageJPEG::GetPixelWidth()
{
	return( m_PixelWidth );
}
/*!
	\brief	Return the pixel height of the image
*/
unsigned int	ImageJPEG::GetPixelHeight()
{
	return( m_PixelHeight );
}

///
METHODDEF(boolean) fill_input_buffer (j_decompress_ptr cinfo)
{	
	my_src_ptr src = (my_src_ptr) cinfo->src;
	if(src->m_filesize>0)
	{
		src->pub.next_input_byte = src->m_fileimage;
		src->pub.bytes_in_buffer = src->m_filesize;
		src->m_fileimage += src->m_filesize ;
		src->m_filesize = 0;
	} else
	{	// end:
		src->pub.next_input_byte = src->m_fileimage;
		src->pub.bytes_in_buffer = 0;
	}
  return TRUE;
}
METHODDEF(void)
init_source (j_decompress_ptr cinfo)
{
 
}
METHODDEF(void)
skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
 // my_src_ptr src = (my_src_ptr) cinfo->src;
	//...
}
METHODDEF(void)
term_source (j_decompress_ptr cinfo)
{
  /* no work necessary here */
}

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
// printf("my_error_exit\n");
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
 myerr->errorcode = cinfo->err->msg_code ;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
 // (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
 //old longjmp(myerr->setjmp_buffer, 1);
}

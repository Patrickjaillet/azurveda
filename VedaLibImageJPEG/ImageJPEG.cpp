// SPDX-License-Identifier: LGPL-2.1-only

#include "ImageJPEG.h"

BASEOBJECT_DECLARE_CLASS("jpg", ImageJPEG, VirtualImage,"Image File JPEG","JPGImage","Use JPEG file format to manage a image.WARNING: some 03 tags is not supported." );

ImageJPEG::ImageJPEG() : VirtualImage()
{

	REGISTER_MEMBER_RESOURCE( mSer_JPEGFile,"JPGFile","jpg" );
	REGISTER_MEMBER_FLAG(mSer_Flags,"Flags",0,"GreyToAlpha");
}

extern "C" {
#include <stdio.h>
#include "jpeglib.h"

};
typedef struct {
  struct jpeg_source_mgr pub;
	const unsigned char	*m_fileimage;
	unsigned int	m_filesize;
} my_source_mgr;

typedef my_source_mgr * my_src_ptr;

struct my_error_mgr {
  struct jpeg_error_mgr pub;
  unsigned int errorcode;

};
typedef struct my_error_mgr * my_error_ptr;
METHODDEF(boolean) fill_input_buffer (j_decompress_ptr cinfo);
METHODDEF(void) init_source (j_decompress_ptr cinfo);
METHODDEF(void) skip_input_data (j_decompress_ptr cinfo, long num_bytes);
METHODDEF(void) term_source (j_decompress_ptr cinfo);
METHODDEF(void) my_error_exit (j_common_ptr cinfo);

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

	src.m_fileimage = pFileChunk;
	src.m_filesize = fileSize;

	cinfo.err = jpeg_std_error(&(jerr.pub));
	jerr.pub.error_exit = my_error_exit;
	jerr.errorcode = 0;

	jpeg_create_decompress(&cinfo);
		initState =1;
	cinfo.src = (jpeg_source_mgr *)(&src);

	src.pub.init_source			= init_source;
	src.pub.fill_input_buffer	= fill_input_buffer;
	src.pub.skip_input_data		= skip_input_data;
	src.pub.resync_to_restart	= jpeg_resync_to_restart;
	src.pub.term_source			= term_source;

	src.pub.bytes_in_buffer = 0;
	src.pub.next_input_byte = NULL;

	jpeg_read_header(&cinfo, TRUE);
  if(jerr.errorcode != 0)
  {
		if(initState==2) jpeg_finish_decompress(&cinfo);
		if(initState<2)jpeg_destroy_decompress(&cinfo);
		return(1);
  }

	cinfo.out_color_space = JCS_RGB;

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

	     jpeg_finish_decompress(&cinfo);
	     jpeg_destroy_decompress(&cinfo);
	     return(1);
    }

	 _outWidth = cinfo.output_width;
	 _outHeight =  cinfo.output_height;

	*_ppFinalBuffer = new unsigned char[ _outWidth * _outHeight * 4 ];
	 if( !(*_ppFinalBuffer))
     {
    	jpeg_finish_decompress(&cinfo);
    	jpeg_destroy_decompress(&cinfo);
    	return(1);
     }

	unsigned int row_stride_size = cinfo.output_width * cinfo.output_components;

	pOutputBuffer = *_ppFinalBuffer ;

	unsigned char *plinebuffer = new unsigned char [row_stride_size];
	if( !plinebuffer )
    {

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

	}
	delete [] plinebuffer;
ok:
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	return(0);

}
};

bool ImageJPEG::CreateInternal(void)
{
	const unsigned char *pFileChunk;
	unsigned int	fileSize;
	fileSize = mSer_JPEGFile.GetByteLength();
	if(fileSize>1024*1024*2) return false;
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

}

unsigned int	ImageJPEG::GetPixelWidth()
{
	return( m_PixelWidth );
}

unsigned int	ImageJPEG::GetPixelHeight()
{
	return( m_PixelHeight );
}

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
	{
		src->pub.next_input_byte = src->m_fileimage;
		src->pub.bytes_in_buffer = 0;
	}
  return TRUE;
}
METHODDEF(void)
init_source (j_decompress_ptr )
{

}
METHODDEF(void)
skip_input_data (j_decompress_ptr , long )
{

}
METHODDEF(void)
term_source (j_decompress_ptr )
{

}

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{

  my_error_ptr myerr = (my_error_ptr) cinfo->err;
 myerr->errorcode = cinfo->err->msg_code ;

}

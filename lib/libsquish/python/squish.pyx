"""
Squish Python bindings
======================

Squish is a c++ library for compressing and uncompressing image using S3TC
algorithm.

Check website at http://code.google.com/p/libsquish/

"""

cdef extern from "stdlib.h":
    ctypedef unsigned long size_t
    void *malloc(size_t size)
    void free(void *ptr)

cdef extern from "Python.h":
    object PyString_FromStringAndSize(char *s, Py_ssize_t len)

cdef extern from "squish.h" namespace "squish":
    void CompressImage(unsigned char* rgba, int width, int height,
            void* blocks, int flags, float *metric = 0)
    void DecompressImage(unsigned char *rgba, int width, int height,
            void *blocks, int flags)
    int GetStorageRequirements(int width, int height, int flags)

__version__ = '1.0'

#: Use DXT1 compression.
DXT1 = ( 1 << 0 ) 

#: Use DXT3 compression.
DXT3 = ( 1 << 1 ) 

#: Use DXT5 compression.
DXT5 = ( 1 << 2 ) 

#: Use a very slow but very high quality color compressor.
COLOR_ITERATIVE_CLUSTER_FIT = ( 1 << 8 )	

#: Use a slow but high quality color compressor (the default).
COLOR_CLUSTER_FIT = ( 1 << 3 )	

#: Use a fast but low quality color compressor.
COLOR_RANGE_FIT	= ( 1 << 4 )

#: Weight the color by alpha during cluster fit (disabled by default).
WEIGHT_COLOR_BY_ALPHA = ( 1 << 7 )


class SquishException(Exception):
    """Used when squish fail in one function"""
    pass


def compressImage(bytes rgba, int width, int height, int flags):
    """
    Compresses an image in memory.

    :param rgba:	The pixels of the source.
    :param width:	The width of the source image.
    :param height:	The height of the source image.
    :param flags:	Compression flags.

    :return: A string containing the compressed image
	
	The source pixels should be presented as a contiguous array of width*height
    rgba values, with each component as 1 byte each. In memory this should be::
	
		( r1, g1, b1, a1, .... , rn, gn, bn, an ) for n = width*height
		
	The flags parameter should specify either DXT1, DXT3 or DXT5 compression, 
	however, DXT1 will be used by default if none is specified. When using DXT1 
	compression, 8 bytes of storage are required for each compressed DXT block. 
	DXT3 and DXT5 compression require 16 bytes of storage per block.
	
	The flags parameter can also specify a preferred color compressor to use 
	when fitting the RGB components of the data. Possible color compressors 
	are: COLOR_CLUSTER_FIT (the default), COLOR_RANGE_FIT (very fast, low 
	quality) or COLOR_ITERATIVE_CLUSTER_FIT (slowest, best quality).
		
	When using COLOR_CLUSTER_FIT or COLOR_ITERATIVE_CLUSTER_FIT, an additional 
	flag can be specified to weight the importance of each pixel by its alpha 
	value. For images that are rendered using alpha blending, this can 
	significantly increase the perceived quality.
	
	The metric parameter can be used to weight the relative importance of each
	color channel, or pass NULL to use the default uniform weight of 
	( 1.0f, 1.0f, 1.0f ). This replaces the previous flag-based control that 
	allowed either uniform or "perceptual" weights with the fixed values
	( 0.2126f, 0.7152f, 0.0722f ). If non-NULL, the metric should point to a 
	contiguous array of 3 floats.
    """

    cdef int datasize
    cdef void *data
    cdef unsigned char* buf_rgba
    cdef object result = None

    buf_rgba = <unsigned char*><char*>rgba
    if buf_rgba == NULL:
        raise SquishException("Invalid rgba parameter")
    if width <= 0 or height <= 0:
        raise SquishException("Invalid width/height value")

    datasize = GetStorageRequirements(width, height, flags)
    if datasize == 0:
        raise SquishException("Unable to calculate size for storage")
    data = malloc(datasize)
    if data == NULL:
        raise SquishException("Unable to allocate storage")

    CompressImage(buf_rgba, width, height, data, flags)

    try:
        result = PyString_FromStringAndSize(<char *>data, datasize)
    finally:
        free(data)

    return result


def decompressImage(bytes blocks, int width, int height, int flags):
    """
    Decompresses an image in memory.

    :param blocks:	The compressed DXT blocks.
    :param width:	The width of the source image.
    :param height:	The height of the source image.
    :param flags:	Compression flags.

    :return: A string containing the decompressed image
	
	The decompressed pixels will be written as a contiguous array of width*height
	16 rgba values, with each component as 1 byte each. In memory this is:
	
		( r1, g1, b1, a1, .... , rn, gn, bn, an ) for n = width*height
		
	The flags parameter should specify either DXT1, DXT3 or DXT5 compression, 
	however, DXT1 will be used by default if none is specified. All other flags 
	are ignored.
    """

    cdef int datasize = width * height * 4
    cdef void *data
    cdef unsigned char *buf_blocks
    cdef object result = None

    buf_blocks = <unsigned char *>blocks
    if buf_blocks == NULL:
        raise SquishException("Invalid blocks parameter")
    if width <= 0 or height <= 0:
        raise SquishException("Invalid width/height value")

    data = malloc(datasize)
    if data == NULL:
        raise SquishException("Unable to allocate storage")

    DecompressImage(<unsigned char *>data, width, height, buf_blocks, flags)

    try:
        result = PyString_FromStringAndSize(<char *>data, datasize)
    finally:
        free(data)

    return result

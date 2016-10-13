#include <stdlib.h>
#include <memory.h>

#ifndef _MEMBUFFER_H
#define _MEMBUFFER_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
static unsigned char a2e[256] = {
        0,  1,  2,  3, 55, 45, 46, 47, 22,  5, 37, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 60, 61, 50, 38, 24, 25, 63, 39, 28, 29, 30, 31,
        64, 79,127,123, 91,108, 80,125, 77, 93, 92, 78,107, 96, 75, 97,
        240,241,242,243,244,245,246,247,248,249,122, 94, 76,126,110,111,
        124,193,194,195,196,197,198,199,200,201,209,210,211,212,213,214,
        215,216,217,226,227,228,229,230,231,232,233, 74,224, 90, 95,109,
        121,129,130,131,132,133,134,135,136,137,145,146,147,148,149,150,
        151,152,153,162,163,164,165,166,167,168,169,192,106,208,161,  7,
        32, 33, 34, 35, 36, 21,  6, 23, 40, 41, 42, 43, 44,  9, 10, 27,
        48, 49, 26, 51, 52, 53, 54,  8, 56, 57, 58, 59,  4, 20, 62,225,
        65, 66, 67, 68, 69, 70, 71, 72, 73, 81, 82, 83, 84, 85, 86, 87,
        88, 89, 98, 99,100,101,102,103,104,105,112,113,114,115,116,117,
        118,119,120,128,138,139,140,141,142,143,144,154,155,156,157,158,
        159,160,170,171,172,173,174,175,176,177,178,179,180,181,182,183,
        184,185,186,187,188,189,190,191,202,203,204,205,206,207,218,219,
        220,221,222,223,234,235,236,237,238,239,250,251,252,253,254,255
};

static unsigned char e2a[256] = {
        0,  1,  2,  3,156,  9,134,127,151,141,142, 11, 12, 13, 14, 15,
        16, 17, 18, 19,157,133,  8,135, 24, 25,146,143, 28, 29, 30, 31,
        128,129,130,131,132, 10, 23, 27,136,137,138,139,140,  5,  6,  7,
        144,145, 22,147,148,149,150,  4,152,153,154,155, 20, 21,158, 26,
        32,160,161,162,163,164,165,166,167,168, 91, 46, 60, 40, 43, 33,
        38,169,170,171,172,173,174,175,176,177, 93, 36, 42, 41, 59, 94,
        45, 47,178,179,180,181,182,183,184,185,124, 44, 37, 95, 62, 63,
        186,187,188,189,190,191,192,193,194, 96, 58, 35, 64, 39, 61, 34,
        195, 97, 98, 99,100,101,102,103,104,105,196,197,198,199,200,201,
        202,106,107,108,109,110,111,112,113,114,203,204,205,206,207,208,
        209,126,115,116,117,118,119,120,121,122,210,211,212,213,214,215,
        216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,
        123, 65, 66, 67, 68, 69, 70, 71, 72, 73,232,233,234,235,236,237,
        125, 74, 75, 76, 77, 78, 79, 80, 81, 82,238,239,240,241,242,243,
        92,159, 83, 84, 85, 86, 87, 88, 89, 90,244,245,246,247,248,249,
        48, 49, 50, 51, 52, 53, 54, 55, 56, 57,250,251,252,253,254,255
};
*/

#define LINE_CHUNK 1024



class mem_buffer
{
        char            *m_buffer;
        unsigned long    m_size;
        unsigned long    m_chunk;
        unsigned long    m_real;

public:

        // Constructor
        mem_buffer( unsigned long iChunk = 4096, unsigned long iSize = 1024 )
        {
            m_buffer = NULL;
            m_chunk = iChunk;
            m_size = 0;
            m_real = iSize;
            resize( iSize );               
        }
        
        // Destructor
        ~mem_buffer() { this->free(); }
                
        // operator char* type get buffer
        // allows an membuffer instance
        // to be taken as a char* parameter
        operator char*() { return m_buffer; }
        
        // char 
        char& operator[]( long Offset ) { return m_buffer[Offset]; }
        
        // get buffer 
        char* buffer() { return m_buffer; }

        
        
        char* sz() 
        {
            char* retPointer = m_buffer;
            
            if( m_real == m_size )
                retPointer = grow(m_real + 1);
                
            if( retPointer != NULL )
                m_buffer[m_size] == '\0';
            
            return m_buffer;
        } 
        
        // Return buffer size
        unsigned long size() const { return m_size; }
        
        // Return buffer size
        unsigned long real_size() const { return m_real; }
        
        // Expand size
        char* expand( unsigned long iSize )
        {
			//char    *tmpHolder = m_buffer;
            char    *retPointer = resize( m_size + iSize );
			
			if( retPointer != NULL )
			{
				m_size += iSize;
				return retPointer;
			}
			
			return NULL;
        }
        
        char* grow(unsigned long iSize)
        {
                char    *tmpHolder = NULL;
                long    growth = m_chunk*((iSize/m_chunk)+1);
                char    *retPointer = NULL;
                
                if( iSize == 0 )
				{
                        free();
						return NULL;
				}
                else if( m_real != growth || m_buffer == NULL )
                        tmpHolder = (char*)realloc(m_buffer, growth );
                else
                    tmpHolder = m_buffer;
                
				
                if( tmpHolder )
                {
                        m_buffer = tmpHolder;
                        retPointer = &(m_buffer[m_size]);
                        m_real = m_chunk*((iSize/m_chunk)+1);
                }

                return retPointer;
        }
        
        // Reallocation routine
        char* resize( unsigned long iSize )
        {
                char    *tmpHolder = NULL;
                char    *retPointer = NULL;
                
                tmpHolder = grow(iSize);
                
                if( tmpHolder != NULL )
                {
                    //m_size = iSize;
                    retPointer = tmpHolder;
                }

                return retPointer;
        }
        
        void zero()
        {
                memset( m_buffer, 0, m_size );
        }
        
        void reset()
        {
            m_size = 0;
        }
        
        // Deallocation routine
        void free()
        {
                if( m_buffer != NULL )
                {
                        ::free( m_buffer );
                        m_size = 0;
                        m_buffer = NULL;
                }
        }
        
        int binary_hexdump( int cols )
        {
            char* buf = NULL;
            char* outbuf = NULL;
                 int i=0, j=0, bufread=0, count=0;

            //buf = (unsigned char*)malloc( cols+1 );
            outbuf = (char*)malloc( 13+(3*cols)+3+cols+1 );

            for( ; count < m_size; count += cols )
            {
                int c = ( m_size - count );
                bufread = (c > cols) ? cols : c ;

                buf = &(m_buffer[count]);

                sprintf( outbuf, "0x%08x : ", (unsigned int)(&(buf[count])) );

                for( i=0; i < bufread; i++ )
                {
                    int val = 0;
                    val += (unsigned char)buf[i];
                    sprintf( &outbuf[13+i*3], "%02x ", val );
                }

                for( j=0; j < bufread; j++ )
                {
                    if( buf[j] >= 32 && buf[j] <= 126 )
                       outbuf[13+i*3+j] = buf[j];
                    else
                       outbuf[13+i*3+j] = '.';
                }

                outbuf[13+i*3+j] = '\0';

                printf( "%s  %d\n", outbuf, count );

            }

            ::free((void*)outbuf);

            return 0;
        }
};

#endif

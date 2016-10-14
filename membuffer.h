#include <stdlib.h>
#include <memory.h>

#ifndef _MEMBUFFER_H
#define _MEMBUFFER_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

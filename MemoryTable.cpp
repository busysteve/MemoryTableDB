/* 
 * File:   MemoryTable.cpp
 * Author: smathews
 * 
 * Created on October 5, 2016
 */

#include "MemoryTable.h"

MemoryTable::MemoryTable( const char* layout, int chunk_size ) {
    m_counter = 0;
	
    // "fname char(20) pk, lname char(30) pk, age int"
	char* field_tmp = new char[strlen(layout)+1];
	char* savp = NULL;
	char* savp2 = NULL;

	const char* layout_dlim = ",";
	const char* detail_dlim = " ()";

	int typ = 0;			
	int len = 0;
	int off = sizeof(Record);
	std::vector<std::string>  details;

	strcpy( field_tmp, layout );
	
	char* str_field_def = strtok_r( field_tmp, layout_dlim, &savp );
	while( str_field_def != NULL )
	{
		log( "%s\n", str_field_def );

		// pull layout field type details
		char* details_tmp = new char[strlen(str_field_def)+1];
		strcpy( details_tmp, str_field_def );
		char* str_details_def = strtok_r( details_tmp, detail_dlim, &savp2 );
		while( str_details_def != NULL )
		{

			details.push_back( str_details_def );
			log("\t%s\n", str_details_def );
			
			str_details_def = strtok_r( NULL, detail_dlim, &savp2 );
		}

		if( details.size() >= 2 )
		{
			m_fields_by_name[details[0]] = m_fields_by_name.size();
			m_field_names.push_back( details[0] );

			if( 	details[1] == "short" )
			{
				typ = Tshort;
				len = 4;
			}
			else if( 	details[1] == "int" )
			{
				typ = Tint;
				len = 4;
			}
			else if( 	details[1] == "long" )
			{
				typ = Tlong;
				len = 8;
			}
			else if( 	details[1] == "char" 
				|| details[1] == "varchar" 
				|| details[1] == "binary" )
			{
				if( details[1] == "char" && details.size() == 2 )
				{
					typ = Tchar;
					len = 1;
				}
				else if( details.size() > 2 )
				{
					typ = Tarray;
					len = atoi( details[2].c_str() );

					if( details[1] != "binary" && len > 0 )
					{
						len++; // room for NULL terminated strings
					}
				}
			}

			m_field_types[m_field_types.size()] = field_type( typ, len, off );
			off += len;
		}

		details.clear();

		delete [] details_tmp;

		str_field_def = strtok_r( NULL, layout_dlim, &savp );
	}

	m_row_size = off;

	delete [] field_tmp;	

    	m_table_info.m_field_names	= &m_field_names;
    	m_table_info.m_field_types	= &m_field_types;
    	m_table_info.m_fields_by_name	= &m_fields_by_name;
        m_table_info.m_row_size 	= &m_row_size;
}


MemoryTable::~MemoryTable() {
    
}


int MemoryTable::fieldPosByName( const char* name )
{

	fields_by_name::iterator it = m_fields_by_name.find( name );

	if( it != m_fields_by_name.end() )
	{
            return it->second;
	}

	return -1;
}


bool MemoryTable::insert( int count, ... ) 
{
    log("MemoryTable::insert() : ");
	va_list args;


	Record *rec = NULL;

	int size = sizeof(Record)+m_row_size;

    m_fullrec_size = size;
        
	rec = (Record*)m_mem.expand( size );

	memset( rec, 0, size );  // ???

	rec->setRecordInfo( &m_table_info, &(((char*)rec)[sizeof(Record)]) );

	va_start(args, count);

	for( int i=0; i < count; i++ )
	{
		void* pointer = NULL;
		field_type info = m_field_types[i];

		switch( info.type )
		{
			case Tchar: 	// char8
			case Tshort: 	// short16
			case Tint: 	// int32
			{			
			int val = va_arg(args, int );
			memcpy( &((rec->pointer)[info.offset]), &val, info.length ); 
                        log( " col=%d(%d) ", i, val );
			} 
			break;
			case Tlong: 	// long64
			{			
			long val = va_arg(args, long );
			memcpy( &((rec->pointer)[info.offset]), &val, info.length ); 
                        log( " col=%d(%l) ", i, val );
			} 
			break;
			case Tarray: 	// char array space padded
			case Tsz: 	// varchar
			{			
			char* pointer = va_arg(args, char* );
			strncpy( &((rec->pointer)[info.offset]), pointer, info.length );
                        log( " col=%d(%s) ", i, pointer );                        
			} 
			break;
			case Tbin: 	// binary
			{			
			char* pointer = va_arg(args, char* );
			memcpy( &((rec->pointer)[info.offset]), pointer, info.length );
                        log( " col=%d(%s) ", i, pointer );                        
			} 
			break;
		}
	}

	va_end(args);
        
        log(" : end\n");

	if( rec != NULL )
	{
		m_counter++;

		return true;
	}    



	return false;
}

MemoryTable::Record* MemoryTable::findRecord( int num, ... ) 
{
	return NULL;
}


void MemoryTable::clear()
{
    m_counter = 0;
    m_mem.free();
}


int MemoryTable::compare( const void *pointer1, const void *pointer2 )
{
    
    MemoryTable::Record *p1 = (MemoryTable::Record *)pointer1, 
                        *p2 = (MemoryTable::Record *)pointer2;
    
    printf( "MemoryTable::compare() : p1=0x%x : p2=0x%x\n", p1, p2 );
    
    return p1->compare( p2 );
}

void MemoryTable::sort()
{
    m_mem.binary_hexdump( 16 );
    
    printf("\n");
    printf("sort()\n");
    qsort( (void*)m_mem.buffer() , (long)m_counter, m_fullrec_size, compare );
}

long MemoryTable::count()
{
    return m_counter;
}

int MemoryTable::Record::compare( Record* comp )
{
    printf("MemoryTable::Record::compare() : this 0x%x : comp 0x%x \n",
	this, comp );
    
    int cols = (*(ptd->m_field_names)).size();
    
    for( int i=0; i < cols; i++ )
    {
        field_type info = (*(ptd->m_field_types))[i];

        switch( info.type )
        {
            case Tchar: 	// char8
            case Tshort: 	// short16
            case Tint:          // int32
            {			
                int v1 = *(int*)&(this)[info.offset]; 
                int v2 = *(int*)&(comp)[info.offset];
                
                if( v1 < v2 ) return -1;
                if( v1 > v2 ) return 1;
            } 
            break;
            case Tlong: 	// long64
            {			
                long v1 = *(long*)&(this)[info.offset]; 
                long v2 = *(long*)&(comp)[info.offset];
                
                if( v1 < v2 ) return -1;
                if( v1 > v2 ) return 1;
            } 
            break;
            case Tarray: 	// char array space padded
            case Tsz:           // varchar
            {			
                char* v1 = (char*)(&(this)[info.offset]);
                char* v2 = (char*)(&(comp)[info.offset]);
                
                int ret = strcmp( v1, v2 );
                
                if( ret != 0 ) return ret;
                //, pointer, info.length );
            } 
            break;
            case Tbin:          // binary
            {			
                char* v1 = (char*)(&(this)[info.offset]);
                char* v2 = (char*)(&(comp)[info.offset]);
                
                int ret = memcmp( v1, v2, info.length );
                
                if( ret != 0 ) return ret;
            } 
            break;
        }
    }
    
    return 0;
}

MemoryTable::Record* MemoryTable::search( int count, ... )
{
	const char* field_name = NULL;
	
    va_list args;
    
    MemoryTable::Record  rec( &m_table_info, m_row_size );
    
    va_start(args, count);

    for( int i=0; i < count; i++ )
    {
        log(" i = %d\n", i );
        if( (i & 1) != 1 )
        {
            field_name = va_arg(args, char* );
			log("field_name - %s \n", field_name );
        }
        else
        {
            //void* pointer = NULL;
            
            int pos = m_fields_by_name[field_name];
            
            field_type info = m_field_types[pos];
			
            switch( info.type )
            {
                case Tchar: 	// char8
                case Tshort: 	// short16
                case Tint:          // int32
                {			
                    int val = va_arg(args, int );
                    memcpy( &(rec.pointer)[info.offset], &val, info.length ); 
					log("\n field_name=%s | pos=%d | val=%d\n", field_name, pos, val );
                } 
                break;
                case Tlong: 	// long64
                {			
                    long val = va_arg(args, long );
                    memcpy( &(rec.pointer)[info.offset], &val, info.length );
					log("field_name=%s | pos=%d | val=%l\n", field_name, pos, val );
                } 
                break;
                case Tarray: 	// char array space padded
                case Tsz:           // varchar
                {			
                    char* pointer = va_arg(args, char* );
                    strcpy( &(rec.pointer)[info.offset], pointer, info.length );
					log("field_name=%s | pos=%d | pointer=%s | len=%d\n", field_name, pos, pointer, info.length );
                } 
				break;
                case Tbin:          // binary
                {			
                    char* pointer = va_arg(args, char* );
                    memcpy( &(rec.pointer)[info.offset], pointer, info.length );
					log("field_name=%s | pos=%d | pointer=%s | len=%d\n", field_name, pos, pointer, info.length );
                } 
                break;
            }
        }
    }

    va_end(args);
 
    void* pRec = ::bsearch ( (void*)&rec, 
			(const void*)m_mem.buffer(), (size_t)m_counter, 
			(size_t)m_fullrec_size, MemoryTable::compare );
    
	/*
    MemoryTable::Record* item = 
		new MemoryTable::Record
			( m_field_names, m_field_types, m_fields_by_name, pRec );
	*/

    return (MemoryTable::Record*) pRec;

}

int MemoryTable::Record::int_field( int pos )
{
	int *val;

	field( pos, (void**)&val );

	return (*val);

}

/*
const char* MemoryTable::Record::sz_field( int pos )
{
	return (const char*)convert( pos, field( pos ), Tsz );
}
*/

int MemoryTable::Record::field( int pos, void** pData )
{
	int ret = 0;

	if( pos < (*(ptd->m_field_types)).size() ) // TODO: don't use size
	{
		field_type& posType = (*(ptd->m_field_types))[ pos ];

		int typ = posType.type;
		int len = posType.length;
		int off = posType.offset;

		*pData = (void*)(&((char*)pointer)[off]);

		return len;
	}

	return ret;
}

void* MemoryTable::Record::convert( 	int pos, void* val, 
					MemoryTable::Record::Types toType, 
					void** pDest, int maxLen )
{
	int fromType   = (*ptd->m_field_types)[ pos ].type;
	int fromLength = (*ptd->m_field_types)[ pos ].length;

	if( fromType == toType )
	{
		void* pVal;
		field( pos, &pVal );

		if( pDest != NULL )
			*pDest = pVal;

		return pVal;
	}

	switch( fromType )
	{
		case Tchar: 	// char8
		case Tshort: 	// short16
		case Tint: 	// int32
		case Tlong: 	// long64
			{
				if( toType == Tsz || toType == Tarray )
				{
					char* pmem = NULL;
					int len = 0;

					if( pDest != NULL )
					{
						pmem = (char*)(*pDest);
						len = maxLen;
					}
					else
					{
						len = 32;
						pmem = new char[len];
					}

					snprintf( pmem, len, 
						//(const char*)((fromType == Tlong) ? "%l" : "%d"), 
						"%d", 
						*(int*)val );

					if( toType == Tarray ) // Add space padding if so
					{
						int rlen = strlen(pmem);
						memset( pmem+rlen, ' ', (len-rlen) );
						pmem[len-1] = '\0';
					}
					return pmem;
				}
			}
		break;
		case Tarray: // char array, fixed length, space padded, NULL terminted 
			{
				
			}
		break;
		case Tsz: // char array(varchar) , NULL terminted, limited length
			{
				
			}
		break;
	}

	return NULL;
}






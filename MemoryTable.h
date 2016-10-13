/* 
 * File:   MemoryTable.h
 * Author: smathews
 *
 * Created on October 4, 2016
 */

#ifndef MemoryTable_H
#define	MemoryTable_H

#include "membuffer.h"
#include <cmath>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <map>
#include <vector>
#include <string>

#define log     printf
class MemoryTable {

public:
    class Record;
    enum Types { Tchar=1, Tshort, Tint, Tlong, Tarray, Tsz, Tbin };

    struct field_type
    {
		int type;
		int length;
		int offset;
		field_type() :
			type(0), length(0), offset(0)
		{}
		field_type( int typ, int len, int off ) :
			type(typ), length(len), offset(off)
		{}
    };

    typedef std::vector< std::string >      field_names;
    typedef std::map< int, field_type > 	field_types;
    typedef std::map< std::string, int > 	fields_by_name;


    mem_buffer      	m_mem;
    long 		m_counter;
    int 		m_field_count;
    field_names		m_field_names;
    field_types 	m_field_types;
    fields_by_name 	m_fields_by_name;
    int                 m_row_size;
    int                 m_fullrec_size;

    struct table_info
    {
    	field_names 	*m_field_names;
    	field_types 	*m_field_types;
    	fields_by_name 	*m_fields_by_name;
        int             *m_row_size;
    };
	
	table_info m_table_info;


    class Record
    {
	public:
	enum Types { Tchar=1, Tshort, Tint, Tlong, Tarray, Tsz, Tbin };

	private:
	Record();
	void * convert( int pos, void* val, Types toType, void** pDest, int maxLen );

	public:
	Record( table_info* p, char* point ) 
		:
		ptd(p),
		pointer(point)
		{}	

	Record( table_info* p, int row_size ) 
		:
		ptd(p)
		{
                    pointer = new char[row_size];
                }	

        ~Record()
        {
            if( pointer != NULL )
                delete [] pointer;
        }
        
	void setRecordInfo( table_info *p, char* point )
	{
		log( "table_info *p = 0x%x", p );
		ptd = p;
		pointer = point;
	}

	table_info *ptd;
	char* pointer;

	const char* 	sz_field( int pos );
	int         	int_field( int pos );
	long        	long_field( int pos );
	char        	char_field( int pos );
	char*       	binary_field( int pos );
	int       	field( int pos, void** pData );

	int         	compare( Record* comp );

	bool setField( const char* field, void* value, int len );		

        
    };

    friend class Record;


public:


    MemoryTable( const char* layout, int chunk_size = 4096 );
    virtual ~MemoryTable();
    int fieldPosByName( const char* );

    bool insert( int num, ... );
    Record*  findRecord( int num, ... );
    Record*  newRecord( );
    bool insertRecord( Record* prec );

    void clear();
    static int compare( const void *pointer1, const void *pointer2 );
    void sort();
    long count();
    MemoryTable::Record* search( int count, ... );
    
private:

};

#endif	/* MemoryTable_H */



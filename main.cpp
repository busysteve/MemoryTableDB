
#include "MemoryTable.h"

int main( int argc, const char** argv )
{

	MemoryTable people( "fname char(20) pk, lname char(30) pk, age int" );

	people.insert( 3, "steve", "mathews", 45 );
	people.insert( 3, "fred", "sanford", 75 );
	people.insert( 3, "fred", "flintstone", 10075 );
	people.insert( 3, "tom", "jones", 65 );
	people.insert( 3, "jerry", "hanks", 25 );
	people.insert( 3, "steve", "perry", 55 );

	people.sort();

	int age_pos = people.fieldPosByName( "age" );
	
	if( age_pos < 0 )
	{
		printf("fieldPosByName failed\n");
		exit(1);
	}
	
	MemoryTable::Record *rec = people.search( 4, "fname", "steve", "lname", "mathews" );
	
	if( rec == NULL )
	{
		printf("record retrival failed\n");
		exit(1);
	}

	int age = rec->int_field( age_pos );
	
	printf( "age = %d\n", age );
	

	

}











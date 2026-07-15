
// this example shows how to use Base types to load, save or clone base types.
// your make project for your machione should be in your machine directory.
// Make sure your project use the editable mode version of the libs,
// because we use Set() accessors and file outputs.
// Note this example will write a file on the disk! 
#include "PackULong.h"
#include "PackLong.h"
#include "PackString.h"
#include "PackFloat.h"
#include "PackStruct.h"
#include <stdio.h>
#include <time.h>
int main()
{
    // we got a simple unsigned long:
    PackULong   myUnsignedLongWriten,myUnsignedLongRead;

    myUnsignedLongWriten.Set(1337);
    myUnsignedLongWriten.WriteFile("my1337File");
    
    myUnsignedLongRead.ReadFile("my1337File");
    unsigned int value = myUnsignedLongRead.Get();
    // here it is !!!
    printf("%d !!!\n",value);
    // note MakeCloneOf() does the same, without saving a file:
    myUnsignedLongRead.MakeCloneOf(myUnsignedLongWriten);
    // the same goes for all types ! PackString manages strings:
    PackString  str1,str2;
    str1.Set("My sentence is long.");
    str1.AddString("... and It continues on and on !\n");
    str2.MakeCloneOf(str1);
    printf( str2.Get() );
	/*
		The following example Uses the automatic Serialization
		of the AzurVeda base types In order to create and manage
		a struct, edit it a bit, then save and load it, using the
		PackStruct class, which is a base type done for that !
		(note: this example still doesn't use the context features,
		and only uses the base types.) 
	*/
    // we just have a local test class.
    class myStruct : public PackStruct
    { public:
        // the constructor got to register the members to know what to serialize,
        // the same way as with the more complex BaseObject.
        myStruct() : PackStruct()
        {
            // second parameter 0 means no GUI name. Third is always default value:
            REGISTER_MEMBER_PACKSTRING(m_Name,0);
            REGISTER_MEMBER_PACKLONG(m_LongTestMember,0,0);
            REGISTER_MEMBER_PACKFLOAT(m_FloatTestMember,0,0.0f);
        };
        // declare the members of the class:
        PackString  m_Name;
        PackLong    m_LongTestMember;
        PackFloat   m_FloatTestMember;
    };
    myStruct    object1,object2;
    object1.m_Name.Set("Henry");
    object1.m_LongTestMember.Set(4);
    object1.m_FloatTestMember.Set(3.14f);

    object1.WriteFile("serializedstruct");
    object2.ReadFile("serializedstruct");
    printf("object2 values: name:%s long:%d float:%f\n",
        object2.m_Name.Get(),object2.m_LongTestMember.Get(),object2.m_FloatTestMember.Get());
	// just wait 6 seconds to be sure the output is visible: 
	clock_t start = clock();
	while(  (clock()-start)< 6*CLOCKS_PER_SEC ) {};

	return 0;
}

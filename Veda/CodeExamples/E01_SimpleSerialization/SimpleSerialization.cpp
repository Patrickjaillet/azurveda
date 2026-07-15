
#include "PackULong.h"
#include "PackLong.h"
#include "PackString.h"
#include "PackFloat.h"
#include "PackStruct.h"
#include <stdio.h>
#include <time.h>
int main()
{

    PackULong   myUnsignedLongWriten,myUnsignedLongRead;

    myUnsignedLongWriten.Set(1337);
    myUnsignedLongWriten.WriteFile("my1337File");

    myUnsignedLongRead.ReadFile("my1337File");
    unsigned int value = myUnsignedLongRead.Get();

    printf("%d !!!\n",value);

    myUnsignedLongRead.MakeCloneOf(myUnsignedLongWriten);

    PackString  str1,str2;
    str1.Set("My sentence is long.");
    str1.AddString("... and It continues on and on !\n");
    str2.MakeCloneOf(str1);
    printf( str2.Get() );

    class myStruct : public PackStruct
    { public:

        myStruct() : PackStruct()
        {

            REGISTER_MEMBER_PACKSTRING(m_Name,0);
            REGISTER_MEMBER_PACKLONG(m_LongTestMember,0,0);
            REGISTER_MEMBER_PACKFLOAT(m_FloatTestMember,0,0.0f);
        };

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

	clock_t start = clock();
	while(  (clock()-start)< 6*CLOCKS_PER_SEC ) {};

	return 0;
}

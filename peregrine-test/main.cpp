#include "pereginetest.h"

int main()
{
    PeregrineTest test1;
    QTest::qExec(&test1);

    return 0;
}

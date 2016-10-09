// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#include "pereginetest.h"

int main()
{
    PeregrineTest test1;
    QTest::qExec(&test1);

    return 0;
}

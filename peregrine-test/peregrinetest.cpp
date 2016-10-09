// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#include "pereginetest.h"
#include "..\peregrine-desktop\suggestionalgorithm.h"
#include "..\thirdparty\exprtk\exprtk.hpp"
#include <string>

using namespace std;

PeregrineTest::PeregrineTest()
{
}

void PeregrineTest::suggestionKeywordMatchTest()
{
    auto ret = SuggestionAlgorithm::matchKeyword({"foo"}, "FooBar");
    QVERIFY(ret.first == true);

    ret = SuggestionAlgorithm::matchKeyword({ "goo" }, "FooBar");
    QVERIFY(ret.first == false);

    ret = SuggestionAlgorithm::matchKeyword({ "bar" }, "FooBar");
    QVERIFY(ret.first == true);

    ret = SuggestionAlgorithm::matchKeyword({ "foo", "v" }, "FooBar");
    QVERIFY(ret.first == false);

    ret = SuggestionAlgorithm::matchKeyword({ "foo", "bal" }, "FooBar");
    QVERIFY(ret.first == false);

    ret = SuggestionAlgorithm::matchKeyword({ }, "FooBar");
    QVERIFY(ret.first == false);
}

void PeregrineTest::expressionTest()
{
    exprtk::symbol_table<double> st;
    string v = "foo";
    st.add_stringvar("input_text", v, true);
    exprtk::expression<double> expr;
    expr.register_symbol_table(st);
    exprtk::parser<double> p;
    p.compile("input_text == ''", expr);
    double res = expr.value();
    QVERIFY(res == 0);
    p.compile("input_text == 'foo'", expr);
    res = expr.value();
    QVERIFY(res == 1);
}

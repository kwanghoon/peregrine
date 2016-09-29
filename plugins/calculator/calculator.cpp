#include "calculator.h"
#include <exprtk/exprtk.hpp>
#include <QDebug>
#include <qmath.h>
#include <string>

const struct PG_FUNC_TABLE* g_funcTable;

int InitializePlugin(const struct PG_FUNC_TABLE* funcTable, const PG_PLUGIN_CONFIGS* pluginConfigs)
{
    g_funcTable = funcTable;
    return 0;
}

double evaluate(const char* exprStr)
{
    exprtk::symbol_table<double> st;
    {
        st.add_constants();
    }
    exprtk::expression<double> expr;
    expr.register_symbol_table(st);
    expr.register_symbol_table(st);
    exprtk::parser<double> p;
    p.compile(exprStr, expr);
    double result = expr.value();
    return result;
}

int RunAction(const char* actionId, const char* data)
{
    std::string stringifiedResult;
    {
        double result = evaluate(data);
        if (isnan(result))
        {
            stringifiedResult = "NaN";
        }
        else if (isinf(result))
        {
            stringifiedResult = "Inf";
        }
        else
        {
            stringifiedResult = std::to_string(result);
            if (stringifiedResult.find('.') != std::string::npos)
            {
                while (stringifiedResult.back() == '0')
                {
                    stringifiedResult.pop_back();
                }
                if (stringifiedResult.back() == '.')
                {
                    stringifiedResult.pop_back();
                }
            }
        }
    }
    // #TODO: handle buffer overrun
    char json[256];
    sprintf(json, "{\"expression\": \"%s\", \"result\": \"%s\"}", 
        data, stringifiedResult.c_str());
    g_funcTable->fpInvokeQmlFunc("addCalcResult", json);
    return PG_BEHAVIOR_ON_RETURN::PG_REMAIN;
}

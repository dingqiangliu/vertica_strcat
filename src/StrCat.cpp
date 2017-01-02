/* Copyright (c) 2005 - 2011 Vertica, an HP company -*- C++ -*- */
/*
 * Description: User Defined Transform Function: for each partition, output a
 * comma-separated list in a string
 *
 * Create Date: Dec 15, 2011
 */
#include "Vertica.h"
#include <sstream>

using namespace Vertica;
using namespace std;

#define DEFAULT_MAXSIZE 64000
#define DEFAULT_separator ", "

/*
 * Takes in a sequence of string values and produces a single output tuple with
 * a comma separated list of values.  If the output string would overflow the
 * maximum line length, stop appending values and include a ", ..."
 */

class StrCat : public TransformFunction
{
    virtual void processPartition(ServerInterface &srvInterface,
                                  PartitionReader &input_reader,
                                  PartitionWriter &output_writer)
    {
        if (input_reader.getNumCols() < 1)
            vt_report_error(0, "Function need 1 argument at least, but %zu provided", input_reader.getNumCols());

        ParamReader paramReader = srvInterface.getParamReader();
        vint maxsize = DEFAULT_MAXSIZE;
        if (paramReader.containsParameter("maxsize"))
            maxsize = paramReader.getIntRef("maxsize");
        std::string separator = DEFAULT_separator;
        if (paramReader.containsParameter("separator")){
            separator = paramReader.getStringRef("separator").str();
        }

        int columncount = input_reader.getNumCols();
        ostringstream oss[columncount];
        bool exceeded[columncount];
        bool first = true;
        do {
            for(int i=0; i<columncount; i++) {
                const VString& elem = input_reader.getStringRef(i);
                if ( !exceeded[i] ) {
                    std::string s;
                    if (! elem.isNull()) {
                        s = elem.str();
                    }
                    else{
                        s.empty();
                    }

                    size_t curpos = oss[i].tellp();
                    curpos += s.length();

                    if (!first) curpos += separator.length();
                    if (!first) oss[i] << separator;
                    if (curpos > (size_t)maxsize) {
                        exceeded[i] = true;
                        oss[i] << "...";
                    }
                    else {
                        oss[i] << s;
                    }
                }
            }
            
            first = false;
        } while (input_reader.next());

        for(int i=0; i<columncount; i++) {
            VString &summary = output_writer.getStringRef(i);
            summary.copy(oss[i].str().c_str());
        }

        output_writer.next();
    }
};

class StrCatFactory : public TransformFunctionFactory
{
    virtual void getPrototype(ServerInterface &srvInterface, ColumnTypes &argTypes, ColumnTypes &returnType)
    {
        // get parameters
        ParamReader paramReader = srvInterface.getParamReader();

        argTypes.addAny();
        
        // Note: need not add any type to returnType. empty returnType means any columns and types!
    }

    virtual void getReturnType(ServerInterface &srvInterface,
                               const SizedColumnTypes &input_types,
                               SizedColumnTypes &output_types)
    {
        if (input_types.getColumnCount() < 1)
            vt_report_error(0, "Function need 1 argument at least, but %zu provided", input_types.getColumnCount());

        // get parameters
        ParamReader paramReader = srvInterface.getParamReader();
        vint maxsize = DEFAULT_MAXSIZE;
        if (paramReader.containsParameter("maxsize"))
            maxsize = paramReader.getIntRef("maxsize");
        std::string separator = DEFAULT_separator;
        if (paramReader.containsParameter("separator")){
            separator = paramReader.getStringRef("separator").str();
        }

        // output can be wide.  Include extra space for a last ", ..."
        vint resultsize = maxsize + separator.length() + 3;
        char resultColumnName[30];
        int columncount = input_types.getColumnCount();
        for(int i=0; i<columncount; i++) {
            sprintf(resultColumnName, "result%d", i);
            output_types.addVarchar(resultsize, resultColumnName);
        }
    }

    // Defines the parameters for this UDSF. Works similarly to defining
    // arguments and return types.
    virtual void getParameterType(ServerInterface &srvInterface,
                                  SizedColumnTypes &parameterTypes) {
        //parameter: maximum output size, default value is 64000.
        parameterTypes.addInt("maxsize");
        //parameter: separator string for concatenating, default value is ', '.
        parameterTypes.addVarchar(200, "separator");
    }


    virtual TransformFunction *createTransformFunction(ServerInterface &srvInterface)
    { return vt_createFuncObj(srvInterface.allocator, StrCat); }

};

RegisterFactory(StrCatFactory);

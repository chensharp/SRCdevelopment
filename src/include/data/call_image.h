
#if ! defined CPPC_DATA_CALL_IMAGE_H
#define CPPC_DATA_CALL_IMAGE_H

#include <data/context.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace cppc {
  namespace data {

    class CallImage {

    public:
      CallImage( const string &, unsigned int );
      CallImage( const CallImage & );
      ~CallImage();

      bool operator==( const CallImage & );

      inline string getFunctionName() { return functionName; }
      inline void setFunctionName( const string & s ) { functionName = s; }
      inline unsigned int getCalledFromLine() { return calledFromLine; }
      inline void setCalledFromLine( unsigned int l ) { calledFromLine = l; }

      void addParameter( Register * );
      Register * getParameter( const string & );
      void removeParameter( const string & );
      inline vector<Register *> * getParameters() { return parameters; }

      inline BlockMap * getBlocks() { return blocks; }
      void setBlocks( BlockMap * blocks );

      void splitMemory();
      void update( CallImage *, MemoryBlock::MemoryType );

    private:
      CallImage & operator=( const CallImage & );

      string functionName;
      unsigned int calledFromLine;
      vector<Register *> * parameters;
      BlockMap * blocks;
      bool ownsBlockMemory;
    };

  }
}

#endif

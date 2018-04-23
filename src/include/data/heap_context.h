
#if ! defined CPPC_DATA_HEAPCONTEXT_H
#define CPPC_DATA_HEAPCONTEXT_H

#include <data/context.h>

#include <string>

using std::string;

namespace cppc {
  namespace data {

    class HeapContext : public Context {

    public:
      HeapContext( Context *, string &, unsigned int );
      virtual ~HeapContext();

      virtual bool operator==( const Context & );

      virtual inline RegisterVector * getRegisters() { return registers; }
      virtual void addRegister( Register * );
      virtual bool removeRegister( const string & );
      virtual Register * getRegister( const string & );
      void setRegisters( RegisterVector * );
      virtual inline void clearRegisters() {
        this->setRegisters( new RegisterVector() );
      }

      virtual inline string getFunctionName() { return functionName; }
      virtual inline unsigned int getCalledFromLine() const { return calledFromLine; }

      virtual inline Context * clone() { return new HeapContext( *this ); }

      virtual bool isEmpty();

      virtual inline bool getCheckpointHere() { return checkpointHere; }
      virtual inline void setCheckpointHere( bool b ) { checkpointHere = b; }

      virtual Context * addSubcontextTree( Context *, BlockMap &, BlockMap &, MemoryBlock::MemoryType );
      virtual void update( Context *, BlockMap &, BlockMap &, MemoryBlock::MemoryType );

    protected:
      HeapContext( const HeapContext & );

    private:
      virtual HeapContext & operator=( const HeapContext & );

      string functionName;//函数名称
      unsigned int calledFromLine;
      RegisterVector * registers;//维护一组register
      bool checkpointHere;//
    };

  }
}

#endif

#if ! defined CPPC_DATA_REGISTER_H
#define CPPC_DATA_REGISTER_H

#include <data/cppc_types.h>
#include <data/memory_block.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace cppc {
  namespace data {

    class Register {

    public:
      Register( void *, void *, DataType::DataTypeIdentifier, string );
      Register( const Register & );
      Register & operator=( const Register & );
      ~Register();


      inline void * initAddress() { return( _initDir ); }
      inline void setInitAddress( void * initAddress ) { _initDir = initAddress; }
      inline void * endAddress() { return( _endDir ); }
      inline void setEndAddress( void * endDir ) { _endDir = endDir; }
      inline DataType::DataTypeIdentifier type() { return( _type ); }
      inline string name() { return( _name ); }
      inline void setName( string s ) { _name = s; }
      inline void setCode( MemoryBlock::BlockCode code ) { _blockCode = code; }
      inline MemoryBlock::BlockCode getCode() { return _blockCode; }

      bool operator==(const Register & );

    private:
      Register();

      void * _initDir;//首地址
      void * _endDir;//尾地址
      DataType::DataTypeIdentifier _type;//类型
      string _name;//变量名
      MemoryBlock::BlockCode _blockCode;//内存块号码
    };

    typedef vector<Register *> RegisterVector;//数组
  }
}

#endif

#if ! defined CPPC_DATA_FILE_H
#define CPPC_DATA_FILE_H

#include <cppc.h>
#include <data/cppc_basic.h>

#include <map>
#include <string>

using cppc::data::BasicLongType;
using cppc::data::BasicULongType;
using cppc::data::LongType;
using cppc::data::ULongType;

using std::map;
using std::string;

namespace cppc {
  namespace data {

    class CppcFile {

      public:

        typedef BasicLongType FileOffset;
        typedef LongType FileOffsetType;
        typedef BasicUCharacterType FileCode;
        typedef UCharacterType FileCodeType;
        typedef CPPC_DescriptorType DescriptorType;

        CppcFile( FileCode, void *, DescriptorType, FileOffset, string );
        CppcFile( const CppcFile & );
        ~CppcFile();

        inline FileCode get_file_code() { return code; }
        inline void * get_file_descriptor() { return descriptor; }
        inline DescriptorType get_descriptor_type() { return dtype; }
        inline FileOffset get_file_offset() { return offset; }
        inline string get_file_path() { return path; }
        inline void set_file_offset( FileOffset e_offset ) {
          offset = e_offset;
        }
        inline void set_file_descriptor( void * d ) { descriptor = d; }
        inline void set_descriptor_type( DescriptorType t ) { dtype = t; }
        inline void set_file_path( string e_path ) { path = e_path; }

      private:
        //拷贝复制函数
		CppcFile & operator=( const CppcFile & );
        FileCode code;//文件码
        void * descriptor;//文件指针？
        DescriptorType dtype;//
        FileOffset offset;
        string path;
    };

	//文件code
    typedef map<CppcFile::FileCode, CppcFile *> FileMap;
  }
}
#endif

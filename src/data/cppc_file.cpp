
#include <data/cppc_file.h>
#include <util/filemanager/file_system_manager.h>

using cppc::util::filesystem::FileSystemManager;

namespace cppc {
  namespace data {

    CppcFile::CppcFile( FileCode e_code, void * e_desc,
      DescriptorType e_type, FileOffset e_offset, string e_path )
      : code( e_code ), descriptor( e_desc ), dtype( e_type ),
        offset( e_offset ), path( e_path ) {}

    CppcFile::CppcFile( const CppcFile & file )
      : code( file.code ), descriptor( file.descriptor ), dtype( file.dtype ),
        offset( file.offset ), path( file.path ) {}

    CppcFile & CppcFile::operator=( const CppcFile & rhs ) {

      if( this == &rhs ) {
        return *this;
      }

      code = rhs.code;
      descriptor = rhs.descriptor;
      dtype = rhs.dtype;
      offset = rhs.offset;
      path = rhs.path;

      return *this;
    }

    CppcFile::~CppcFile() {}

  }
}

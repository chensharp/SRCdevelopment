#include <checkpointer/checkpointer.h>
#include <controller/controller.h>
#include <controller/actions/controller_helper.h>
#include <controller/actions/register_action.h>
#include <data/data_type.h>
#include <data/data_type_factory.h>

#include <cassert>
#include <cstring>

using cppc::data::DataType;
using cppc::data::DataTypeFactory;

using cppc::data::Register;
using cppc::data::RegisterVector;

namespace cppc {
  namespace controller {
    namespace actions {

	//传入首地址，变量名，数据类型，长度，内存类型，控制状态
      RegisterAction::RegisterAction( void * data, int size,
        DataType::DataTypeIdentifier type, string name, bool allocMem,
        Controller::ControllerState & state ) : memDir( data ),
        elements( size ), datatype( type ), registerName( name ),
        allocatedMemory( allocMem ), controllerState( state ) {}

      RegisterAction::RegisterAction( const RegisterAction & sa )
        : memDir( sa.memDir ), elements( sa.elements ), datatype( sa.datatype ),
        registerName( sa.registerName ), allocatedMemory( sa.allocatedMemory ),
        controllerState( sa.controllerState ) {}

      RegisterAction & RegisterAction::operator=( const RegisterAction & rhs ) {

        if( this == &rhs ) {
          return *this;
        }

        memDir = rhs.memDir;
        elements = rhs.elements;
        datatype = rhs.datatype;
        registerName = rhs.registerName;
        allocatedMemory = rhs.allocatedMemory;
        controllerState = rhs.controllerState;

        return *this;
      }

      RegisterAction::~RegisterAction() {}

      void * RegisterAction::execute() {

	  //计算空间长度
        unsigned int registerSize = DataTypeFactory::instance().getDataTypeSize( datatype ) * elements;
        unsigned char * memContents;

        if( ControllerHelper::getRestartParameter() ) {
          memContents = static_cast<unsigned char *>( controllerState.checkpointer->partialRestart( registerName, registerSize ) );
          if( memContents != 0 ) {
            if( allocatedMemory ) {
              std::memcpy( memDir, memContents, registerSize );
              delete [] memContents;
            } else {
              memDir = memContents;
            }

            addRegister();
          }
        } else {
          addRegister();
        }

        return memDir;
      }

      Register * RegisterAction::addRegister() {
        // 内存地址不可为空
        assert( memDir != 0 );
		//计算空间长度
        DataType::DataTypeSize typeExtent = DataTypeFactory::instance().getDataTypeSize( datatype );
		//计算内存尾地址 = 首地址 + 数目 X 单位长度
        unsigned long memEnd = reinterpret_cast<unsigned long>( memDir ) + typeExtent * elements;
		//声明一个register
        Register * r = new Register( memDir, reinterpret_cast<void*>(memEnd), datatype, registerName );
        //添加到内存，context的数组里
		controllerState.checkpointSkeleton->getContext()->addRegister( r );
		
        // Build associated memblock in skeleton checkpoint
		//为其分配内存块， 
        ControllerHelper::addMemblocksForRegister( r, controllerState.checkpointSkeleton->getMemBlocks() );
        
		//返回register，
		return r;
      }

    }
  }
}

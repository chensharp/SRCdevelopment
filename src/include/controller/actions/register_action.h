
#if ! defined CPPC_CONTROLLER_ACTIONS_REGISTER_ACTION_H
#define CPPC_CONTROLLER_ACTIONS_REGISTER_ACTION_H

#include <controller/controller.h>

using cppc::controller::Controller;

namespace cppc {
  namespace controller {
    namespace actions {

      class RegisterAction {

      public:
        RegisterAction( void *, int, DataType::DataTypeIdentifier, string, bool,
          Controller::ControllerState & );
        ~RegisterAction();

        void * execute();

      private:
        RegisterAction( const RegisterAction & );
        RegisterAction & operator=( const RegisterAction & );
        Register * addRegister();//添加注册变量

        void * memDir;//内存地址
        int elements;//长度
        DataType::DataTypeIdentifier datatype;//数据类型
        string registerName;//注册名字
        bool allocatedMemory;//是否分配内存
		
        Controller::ControllerState & controllerState;
      };

    }
  }
}

#endif

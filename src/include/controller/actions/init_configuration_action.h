#if ! defined CPPC_CONTROLLER_ACTIONS_INIT_CONFIGURATION_ACTION_H
#define CPPC_CONTROLLER_ACTIONS_INIT_CONFIGURATION_ACTION_H

#include <controller/controller.h>

#include <string>

using cppc::controller::Controller;

using std::string;

namespace cppc {
  namespace controller {
    namespace actions {

      class InitConfigurationAction {

      public:
        InitConfigurationAction( int *, char ***,
          Controller::ControllerState& );
        ~InitConfigurationAction();

        int execute();

      private:
        InitConfigurationAction( const InitConfigurationAction & );
        InitConfigurationAction & operator=( const InitConfigurationAction & );

        void initConfigurationManager( int *, char *** );
        void deleteCppcParameter( int *, char ***, int );

        int * argsno;
        char *** args;
        Controller::ControllerState & controllerState;

        const static string CPPC_ARGS_PREFIX;
      };

    }
  }
}

#endif

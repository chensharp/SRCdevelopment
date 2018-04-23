#include <controller/actions/controller_helper.h>
#include <controller/actions/init_configuration_action.h>
//patch 2
#include <data/cppc_basic.h>
#include <data/data_type_factory.h>

#include <data/heap_context.h>
#include <util/configuration/configuration_manager.h>

// patch 1
#include <util/xml/plain_parser.h>
#include <util/xml/xml_parser_factory.h>

//patch 3
#include <writer/io_hdf5_writer.h>
#include <writer/writer_factory.h>

#include <cassert>

using cppc::data::DataTypeFactory;
using cppc::data::HeapContext;
using cppc::util::configuration::ConfigurationManager;
using cppc::util::configuration::ParameterType;
using cppc::util::configuration::ParameterVector;

//patch 1
using cppc::util::xml::PlainParser;
using cppc::util::xml::XmlParserFactory;
//patch 3
using cppc::writer::IOHdf5Writer;
using cppc::writer::WriterFactory;


namespace cppc {
  namespace controller {
    namespace actions {

      const string InitConfigurationAction::CPPC_ARGS_PREFIX( "-CPPC," );

      InitConfigurationAction::InitConfigurationAction( int * e_argsno,
        char *** e_args, Controller::ControllerState & c_state )
        : argsno( e_argsno ), args( e_args ), controllerState( c_state ) {}

      InitConfigurationAction::InitConfigurationAction(
        const InitConfigurationAction & ia ) : argsno( ia.argsno ),
        args( ia.args ), controllerState( ia.controllerState ) {}

      InitConfigurationAction & InitConfigurationAction::operator=(
        const InitConfigurationAction & rhs ) {

        if( this == &rhs ) {
          return *this;
        }

        argsno = rhs.argsno;
        args = rhs.args;
        controllerState = rhs.controllerState;

        return *this;
      }

      InitConfigurationAction::~InitConfigurationAction() {}

//patch 1
      XmlParser * createPlainParser(){
	return new PlainParser();
      }
// patch 2
      template <class T> DataType * createDataType() {
        return new T;
      }

//patch 3
      Writer * createWriter() {
          return new IOHdf5Writer();
      }

      int InitConfigurationAction::execute() {

        assert( !controllerState.initConfig );
	//register plain parser (ad-hoc fix) patch 1
	XmlParserFactory::instance().registerParser( PlainParser::staticParserType(),createPlainParser );


        // Register data types (ad-hoc fix) patch 2
        DataTypeFactory::instance().registerDataType( cppc::data::CharacterType::typeIdentifier, cppc::data::CharacterType::staticSize(), createDataType<cppc::data::CharacterType> );
        DataTypeFactory::instance().registerDataType( cppc::data::DoubleType::typeIdentifier, cppc::data::DoubleType::staticSize(), createDataType<cppc::data::DoubleType> );
        DataTypeFactory::instance().registerDataType( cppc::data::FloatType::typeIdentifier, cppc::data::FloatType::staticSize(), createDataType<cppc::data::FloatType> );
        DataTypeFactory::instance().registerDataType( cppc::data::IntegerType::typeIdentifier, cppc::data::IntegerType::staticSize(), createDataType<cppc::data::IntegerType> );
        DataTypeFactory::instance().registerDataType( cppc::data::LongType::typeIdentifier, cppc::data::LongType::staticSize(), createDataType<cppc::data::LongType> );
        DataTypeFactory::instance().registerDataType( cppc::data::ShortType::typeIdentifier, cppc::data::ShortType::staticSize(), createDataType<cppc::data::ShortType> );
        DataTypeFactory::instance().registerDataType( cppc::data::UCharacterType::typeIdentifier, cppc::data::UCharacterType::staticSize(), createDataType<cppc::data::UCharacterType> );
        DataTypeFactory::instance().registerDataType( cppc::data::UIntegerType::typeIdentifier, cppc::data::UIntegerType::staticSize(), createDataType<cppc::data::UIntegerType> );
        DataTypeFactory::instance().registerDataType( cppc::data::ULongType::typeIdentifier, cppc::data::ULongType::staticSize(), createDataType<cppc::data::ULongType> );
        DataTypeFactory::instance().registerDataType( cppc::data::UShortType::typeIdentifier, cppc::data::UShortType::staticSize(), createDataType<cppc::data::UShortType> );


        // Register writer (ad-hoc fix) patch 3
        WriterFactory::instance().registerWriter( IOHdf5Writer::staticWriterType().getStaticValue(), createWriter );

        // Initialization of the ConfigurationManager
        initConfigurationManager( argsno, args );

        // Register the application id
        controllerState.appName =
          ControllerHelper::getApplicationNameParameter();

        // Create the checkpoint skeleton
        string main_name( "main" );
        Context * root = new HeapContext( 0, main_name, 0 );
        controllerState.checkpointSkeleton = new Checkpoint( ControllerHelper::getWriterTypeParameter(), 0, root );

        // Assertion of the Controller::running variable, we are almost ready to go!
        controllerState.initConfig = true;

        return 0;
      }

      void InitConfigurationAction::initConfigurationManager( int * argsno,
        char *** args ) {

        ParameterVector configuration;

        for( int i = 1; i < (*argsno); i++ ) {

          string parameter = (*args)[ i ];
          if( parameter.find( CPPC_ARGS_PREFIX ) == 0 ) {

            parameter.erase( 0, CPPC_ARGS_PREFIX.length() );

            while( parameter.length() != 0 ) {
              ParameterType newParameter;
              string element = parameter.substr( 0, parameter.find( "," ) );
              newParameter.key = element.substr( 0, element.find( "=" ) );
              element.erase( 0, element.find( "=" ) + 1 );
              newParameter.value = element;

              configuration.push_back( newParameter );

              int cut_pos = parameter.find( "," );
              if( cut_pos != -1 ) {
                parameter.erase( 0, cut_pos + 1 );
              } else {
                parameter.erase( 0, cut_pos );
              }

            }

            deleteCppcParameter( argsno, args, i );
          }
        }

        ConfigurationManager::instance().init( configuration );
      }

      void InitConfigurationAction::deleteCppcParameter( int * argsno,
        char *** args, int position ) {

        for( int i = position; i < (*argsno)-1; i++ ) {
          (*args)[i] = (*args)[i+1];
        }
        (*argsno)--;
      }

    }
  }
}

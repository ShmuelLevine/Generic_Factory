/**
*      @file   Generic_Factory.h
*      @brief  Generic Class for creating Self-Registering Factory classes
*
*              This is a generic class for creating self-registering factory classes.  Factory classes
*              allow for the instantiation of objects of a type which is known only at run-time and not
*              at compile time.  By using a Factory class, it is possible to select the specific concrete
*              class which is required, based on a key of any type, and call a static function to
*              instantiate a new object for use with dynamic polymorphism.
*
*              This generic class allows the user to specify the details for the factory, allowing this
*              one factory definition to be used for an infinite number of class hierarchies without any
*              template specializations.  Furthermore, it is not restricted to calling default
*              constructors; rather, the signature of the constructor is specified as part of the
*              template arguments for the factory.
*
*              The design intent is for models to be self-registering.  More on this in the appropriate files.
*
*
*    @author     Shmuel Levine, <Shmuel@thelevines.ca>
*    @version 0.1
*    @date    2014.06.24
*
*   @internal
*     Created  $DATE$
*    Revision  $Id$
*    Compiler  gcc/g++
*     Company  $COMPANY$
*   Copyright  $COPYRIGHT$
* @endinternal
*
* =============================================================================
*/


#ifndef FX_GENERIC_FACTORY_H
#define FX_GENERIC_FACTORY_H


#include <functional>
#include <memory>
#include <map>

    //namespace fx {
    //    namespace core { 
    //        class IFinancial_Model; 
    //        class Data_Context;
    //    }
    ////    namespace inputs{ class IModel_Input; }
    //}


namespace fx { namespace core {
    

        template <class T>
        struct Shared_Pointer{
            using Type = std::shared_ptr<T>;
        };
        template <class T>
        struct Unique_Pointer{
            using Type = std::unique_ptr<T>;
        };
        template <class T>
        struct Raw_Pointer{
            using Type = T*;
        };
        
        
        template <class AbstractType, template <typename> class Pointer_Type<AbstractType>, class...ConstructorArgs>
//        template <class AbstractType, class...ConstructorArgs>
    class Generic_Factory{

        public:
            static typename Pointer_Type<AbstractType>::Type Construct(std::string key, ConstructorArgs... arguments){
                auto it = Get_Registry()->find(key);
                if (it == Get_Registry()->cend())
                    return nullptr;

                auto constructor = it->second;
                return constructor(std::forward<ConstructorArgs>(arguments)...);
            }

            using Constructor_t = std::function<typename Pointer_Type<AbstractType>::Type(ConstructorArgs...)>;
            using Registry_t = std::map< std::string, Constructor_t>;
            
            Generic_Factory(Generic_Factory const&) = delete;
            Generic_Factory& operator=(Generic_Factory const&) = delete;

        protected:
            Generic_Factory(){}
            static Registry_t* Get_Registry();
        
        private:
            static Registry_t* _registry_;
  
        };

        template <class ConcreteType, class AbstractType, template <typename> class Pointer_Type<AbstractType>, class...ConstructorArgs>
        struct Factory_Registrar : private fx::core::Generic_Factory<AbstractType, Pointer_Type, ConstructorArgs...>{
            using Factory = fx::core::Generic_Factory<AbstractType, Pointer_Type, ConstructorArgs...>;
            using Constructor_t = typename Factory::Constructor_t;

    public:
            Factory_Registrar(std::string const& designator, Constructor_t  object_constructor){
//                fx::core::Generic_Factory<AbstractType, ConstructorArgs...>::Registry_t* registry = Generic_Factory<AbstractType, ConstructorArgs...>::Get_Registry();
                auto registry = Factory::Get_Registry();
                if (registry->find(designator) == registry->cend())
                    registry->insert(std::make_pair(designator, object_constructor));
            }
            unsigned int NO_OP(){ return 0; }
        };

        template <class Return_t, template <typename> class Pointer_Type, class...Args>
        typename Generic_Factory<Return_t, Pointer_Type, Args...>::Registry_t* Generic_Factory<Return_t, Pointer_Type, Args...>::Get_Registry(){
        if (_registry_ == nullptr)
            _registry_ = new Generic_Factory<Return_t, Pointer_Type, Args...>::Registry_t();
        return _registry_;
    }

        template <class Return_t, template <typename> class Pointer_Type, class...Args>
        typename Generic_Factory<Return_t, Pointer_Type, Args...>::Registry_t* Generic_Factory<Return_t, Pointer_Type, Args...>::_registry_ = nullptr;
}}

#endif

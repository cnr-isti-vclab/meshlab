#ifndef NAME_ACCESSFUNCTION_BINDS_H
#define NAME_ACCESSFUNCTION_BINDS_H

#include <typeinfo>
#include <string>
#include <map>
#include <assert.h>

namespace nafb{
	/*
	  This classes gives a simple way to store a table of associations betweeen sdt::string
	  and member access function of  a user defined type;

	  Example:

		// this is the table of bounds between a name and a member function
		vcg::NameAccessFunctionBounds bounds;

		// Associate the name "Normal3f" to the access member N() of class MyVertex
		vcg::AddNameAccessFunctionBound<vcg::Point3f,MyVertex	>(std::string("Normal3f"),&MyVertex::N	,bounds);

		//Associate the name "quality" to the access member Q() of class MyVertex
		vcg::AddNameAccessFunctionBound<float,MyVertex	>(std::string("quality"),&MyVertex::Q	,bounds);

		MyVertex v;
		float myvalue;

		//  call the access  member associated with "quality" (i.e. Q() ) and put its value on myvalue (by reference)
		Get(std::string("quality"),&v,&myvalue, bounds);

		//  call the access  member associated with "quality" (i.e. Q() ) and set its value to myvalue
		Set(std::string("quality"),&v,&myvalue, bounds);
		*/


	struct NameAccessFunctionBound_Base{
		virtual  std::string  Name()  {return _name;}
		virtual  std::string TypeID() = 0;
		std::string _name;
		virtual void Get(void * from , void * result ) = 0;        // call using function
		virtual void Set(void * from , void * to ) = 0;        // call using function
	};


	template <class TYPE, class ElemClass>
			struct NameAccessFunctionBound: public NameAccessFunctionBound_Base{
				NameAccessFunctionBound(){}
				NameAccessFunctionBound(std::string   name, TYPE & (ElemClass::*_fpt)()){_name =  name ;}
				std::string TypeID(){TYPE t; return typeid(TYPE).name();}
				bool operator ==(const NameAccessFunctionBound & o ) const {return Name()==o.Name();}

				void Get(void * c,void * v){ *((TYPE*)v) = (TYPE) ((*(ElemClass*)c).*fpt)();}
				void Set(void * c,void * v){ ((*(ElemClass*)c).*fpt)() =  *((TYPE*)v);}

			private:
				TYPE & (ElemClass::*fpt)();   // pointer to member function
	};



	typedef std::map<std::string,NameAccessFunctionBound_Base*> NameAccessFunctionBounds;
	typedef NameAccessFunctionBounds::iterator BoundsIterator;
	typedef std::pair<std::string,NameAccessFunctionBound_Base*> TypeBound;


	template < class TYPE, class  ElemClass>
	void AddNameAccessFunctionBound(std::string  name,TYPE & (ElemClass::*_fpt)(),NameAccessFunctionBounds & binders ){
		assert(!name.empty()); // you cannot bound a type to an empty string
		BoundsIterator bi = binders.find(name);
		if(bi!=binders.end())
			assert(typeid(TYPE).name() == ((*bi).second)->TypeID() ); // the name was previously bound to a dirrefent type
		else{
			NameAccessFunctionBound<TYPE,ElemClass>   * newbound = new NameAccessFunctionBound<TYPE,ElemClass> (name, _fpt);
			binders.insert( TypeBound(name,newbound));
		}
	}

	template <class TypeElem,class TypeValue>
	 void Get(std::string name,TypeElem &v,TypeValue &myvalue, const NameAccessFunctionBounds & bounds){
		BoundsIterator bi = bounds.find(name);
		assert(bi != bounds.end());
		(*bi).second->Get(v,myvalue);
	}
	template <class TypeElem,class TypeValue>
	 void Set(std::string name,TypeElem &v,TypeValue &myvalue, const NameAccessFunctionBounds & bounds){
		BoundsIterator bi = bounds.find(name);
		assert(bi != bounds.end());
		(*bi).second->Set(v,myvalue);
	}

};

#endif // NAME_ACCESS_FUNCTION_BOUNDS



#include <string>
#include <vector>
#include <stdio.h>


template <class TYPE>
        void myfunc(){
    printf("%d",sizeof(TYPE));
}

struct A{

    template <class T>
            void Called(){
             printf("%d",sizeof(T));
    }
};

//
 struct NameTypeBinder_Base{
     virtual  std::string  Name()  = 0;

     virtual unsigned int SizeOfType() = 0;

     virtual char * CallTemplatedFunc(A ) = 0;
 };

 std::vector<NameTypeBinder_Base*> binders;

 template <class TYPE>
         struct NameTypeBind: public NameTypeBinder_Base{
             NameTypeBind(){}
             NameTypeBind(char *  name){
                 _name = std::string(name);
                 _size_of = sizeof(TYPE);

             }
             std::string  Name()   {return _name;}

             unsigned int SizeOfType() {return _size_of;}

             char *  CallTemplatedFunc(A a )
             {
                 a.Called<TYPE>();return 0;
               };

             std::string _name;
             unsigned int _size_of;

 };

 struct NameUnknownTypeBind: public NameTypeBind<char>{

 };

 template <class TYPE>
 void AddNameTypeBind(char *  name){binders.push_back( new NameTypeBind<TYPE>( name));}

 void AddNameUnknownTypeBind(NameUnknownTypeBind tb){
      std::vector<NameTypeBinder_Base*>::iterator i;int yy = 0;
       for(i = binders.begin(); i != binders.end(); ++i,++yy)
                           if(tb.Name()==(*i)->Name()){
               if(tb.SizeOfType()!=(*i)->SizeOfType())
               {

               }else

                                           return;
                           }

         }

 void Call(NameUnknownTypeBind tb, A a){
      std::vector<NameTypeBinder_Base*>::iterator i;int yy = 0;
       for(i = binders.begin(); i != binders.end(); ++i,++yy)
                           if(tb.Name()==(*i)->Name()){
               if(tb.SizeOfType()!=(*i)->SizeOfType())
               {

               }else
                    (*i)->CallTemplatedFunc(a);
                                           return;
                           }

         }

 template <class A>
     std::string NameOf(){
         std::vector<NameTypeBinder_Base*>::iterator i;
         for(i = binders.begin(); i != binders.end(); ++i)
              if(dynamic_cast<NameTypeBind<A>*> (*i)!=NULL) return (*i)->Name();
         return std::string();
 }




int main(int argc, char *argv[])
{
    AddNameTypeBind<float>("ciccio");
    AddNameTypeBind<short>("ciccio1");
    AddNameTypeBind<char>("ciccio2");


    NameUnknownTypeBind tb;
    tb._name = std::string("ciccio2");
    tb._size_of  = 1;


    A a;
    Call(tb,a);

}

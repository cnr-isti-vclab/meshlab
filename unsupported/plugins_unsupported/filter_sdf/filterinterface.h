#ifndef SINGLEMESHFILTERINTERFACE_H
#define SINGLEMESHFILTERINTERFACE_H
#include "common/interfaces.h"

/**
  * @brief The interface for a filter plugin which defines a *single* action
  *
  * This interface provides a wrapper to MeshFilterInterface which simplifies
  * its interface when the plugin needs to define *one single action*.
  *
  * The "myfilter.h" header file of your plugin should look something like the following:
  * \code
  * #include <filterinterface.h>
  * class s : public SingleMeshFilterInterface{
  *     Q_OBJECT
  *     Q_INTERFACES(MeshFilterInterface)
  *
  * public:
  *     MyFilter();
  *     bool applyFilter(MeshDocument &, RichParameterSet &, vcg::CallBackPos* cb = 0);
  * };
  * \endcode
  *
  * while the implementation file "myfilter.cpp" will be like the following:
  * \code
  * #include "myfilter.h"
  *
  * MyFilter::MyFilter() : SingleMeshFilterInterface( "My Mesh Filter" ){
  *     //... your initialization code here
  * }
  *
  * bool MyFilter::applyFilter(MeshDocument &, RichParameterSet&, vcg::CallBackPos*){
  *     //... your algorithm implementation here
  * }
  *
  * Q_EXPORT_PLUGIN(SdfPlugin)
  * \endcode
  */
class SingleMeshFilterInterface : public QObject, public MeshFilterInterface{
  
public:
  /**
      * @brief Constructor for a filter plugin of a given name
      *
      * @param name the name of the plugin.
      *
      * The name of the plugin is used in the Meshlab menus as well as in the source
      * as a key to identify the plugin uniquely. Tipically your plugin will have to
      * implement a construction like the following:
      * \code
      * MyPlugin::MyPlugin() : SingleMeshFilterInterface( "My Mesh Filter" ){
      *     // ... whatever else you need to do
      * }
      * \endcode
      */
  SingleMeshFilterInterface(QString actionname){
    typeList << 0; // add a dummy type to the typelist
    actionList << new QAction( actionname, this );
  }
  /**
      * @brief Text displayed in "Help => About Plugins"
      *
      * This is a (optional) brief paragraph that describes the functionalities offered by the filter.
      * Remember that SingleMeshFilterInterface defines a single action, this is the right place to
      * provide a longer documentation of what that action really do.
      *
      * You don't have to implement this method, in that case a simple "No documentation available"
      * will be showed.
      */
  virtual QString filterInfo() const{
    return "No documentation available";
  }
  /**
      * @brief The type of filter being implemented
      * @note optional, filter belongs to MeshFilterInterface::Generic by default
      *
      * This method should return one of the values defined within MeshFilterInterface::FilterClass
      * and is used to understand what type of plugin is being developed. This information will be used
      * by meshlab to determine in which filter sub-menu-entry insert this filter. The default value adds the
      * plugin to the bottom of the list.
      */
  virtual FilterClass getClass(){
    return MeshFilterInterface::Generic;
  }
  
  /**
      * @brief The type of filter being implemented
      * @note optional, the most basic filter has no parameters
      *
      * You should overload this function if your plugin needs parameters. You can build your
      * parameter set according to your mesh document (data dependent parameters). A GUI will be
      * automatically designed according to this parameters.
      */
  virtual void initParameterSet(MeshDocument &, RichParameterSet &){ qDebug() << "HERE2!"; }
  
  /**
      * @brief The implementation of the filter algorithm
      * @note required, you will get a linker error otherwise
      *
      * This method should return one of the values defined within MeshFilterInterface::FilterClass
      * and is used to understand what type of plugin is being developed. This information will be used
      * by meshlab to determine in which filter sub-folder insert this filter.
      */
  virtual bool applyFilter(MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos* cb = 0) = 0;
  
  /**
      * @brief The pre-conditions required by the filter on the input mesh
      * @note optional, filter doesn't require anything by default
      *
      * This method should return a combination of the values defined from MeshModel::MeshElement.
      * You can combine elements using the bit-operator |.
      */
  virtual int getPreConditions() const{
    return MeshModel::MM_NONE;
  }
  
  /**
      * @brief The post-conditions of the input mesh
      * @note optional, filter doesn't add any additional field to the output mesh
      * @todo Revise this documentation, never used postConditions() before
      *
      * This method should return a combination of the values defined from MeshModel::MeshElement.
      * You can combine elements using the bit-operator |.
      */
  virtual int postConditions() const{
    return MeshModel::MM_NONE;
  }
  
//--------------------------------------------------------------------------------------
//            dummy implementation of VIRTUAL METHODS: MeshFilterInterface
//
// here we mostly do the re-direction of calls to the single-action functions defined in
// the public interface of MeshFilterInterface above.
//--------------------------------------------------------------------------------------
private:
  QString filterName(FilterIDType ) const{
    return actionList[0]->text();
  }
  QString filterInfo(FilterIDType ) const{
    return filterInfo();
  }
  FilterClass getClass(QAction *){
    return getClass();
  }
  // NOTE: Paolo informed that this will be killed sooner or later.
  // any behavior defined therein should be moved to getPostConditions()
  int getRequirements(QAction* ){
    return postConditions();
  }
  int getPreConditions(QAction* ) const{
    return getPreConditions();
  }
  int postCondition() const{
    return MeshModel::MM_NONE;
  }
  bool applyFilter(QAction *, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos * cb){
    return applyFilter(md, par, cb);
  }
  virtual void initParameterSet(QAction *, MeshDocument &md, RichParameterSet &par){
    initParameterSet(md,par);
  }
};

#endif // SINGLEMESHFILTERINTERFACE_H

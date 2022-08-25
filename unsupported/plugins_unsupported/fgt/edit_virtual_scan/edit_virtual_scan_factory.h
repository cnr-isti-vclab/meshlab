#ifndef EDIT_VIRTUAL_SCAN_FACTORY_PLUGIN_H
#define EDIT_VIRTUAL_SCAN_FACTORY_PLUGIN_H

#include    <QObject>
#include    <common/interfaces.h>

class EditVirtualScanFactory: public QObject, public MeshEditInterfaceFactory
{

    Q_OBJECT
    Q_INTERFACES(MeshEditInterfaceFactory)

public:

    /*  constructor and destructor */
    EditVirtualScanFactory          ( void );
    virtual ~EditVirtualScanFactory ( void );

    /*  MeshEditInterfaceFactory implementation */
    virtual QList< QAction* >   actions                 ( void )        const;
    virtual MeshEditInterface*  getMeshEditInterface    ( QAction* );
    virtual QString             getEditToolDescription  ( QAction* );

private:

    QList< QAction* >   actionList;
    QAction*            editVirtualScan;

};

#endif

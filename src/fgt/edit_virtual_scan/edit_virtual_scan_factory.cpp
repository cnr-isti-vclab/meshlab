#include    "edit_virtual_scan_factory.h"
#include    "edit_virtual_scan_plugin.h"

EditVirtualScanFactory::EditVirtualScanFactory( void )
{
    editVirtualScan = new QAction( QIcon( ":/images/icon.png" ), "Virtual Scan Sampling", this );

    actionList << editVirtualScan;

    foreach( QAction* editAction, actionList )
    {
        editAction->setCheckable( true );
    }
}

EditVirtualScanFactory::~EditVirtualScanFactory( void )
{
    delete editVirtualScan;
}

QList< QAction* > EditVirtualScanFactory::actions( void ) const
{
    return actionList;
}

MeshEditInterface* EditVirtualScanFactory::getMeshEditInterface( QAction* action )
{
    if( action == editVirtualScan )
    {
        return new EditVirtualScanPlugin();
    }
    else
    {
        assert( 0 );
    }
}

QString EditVirtualScanFactory::getEditToolDescription( QAction* action )
{
    if( action == editVirtualScan )
    {
        return EditVirtualScanPlugin::Info();
    }
    else
    {
        return tr( "Invalid Operation" );
    }
}

Q_EXPORT_PLUGIN(EditVirtualScanFactory)

/*===========================================================================*\
 *                                                                           *
 *                               CoMISo                                      *
 *      Copyright (C) 2008-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.rwth-graphics.de                            *
 *                                                                           *
 *---------------------------------------------------------------------------* 
 *  This file is part of CoMISo.                                             *
 *                                                                           *
 *  CoMISo is free software: you can redistribute it and/or modify           *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  CoMISo is distributed in the hope that it will be useful,                *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with CoMISo.  If not, see <http://www.gnu.org/licenses/>.          *
 *                                                                           *
\*===========================================================================*/ 


//=============================================================================
//
//  CLASS MiSolverDialog
//
//=============================================================================


#ifndef COMISO_MISOLVERDIALOG_HH
#define COMISO_MISOLVERDIALOG_HH


#include <CoMISo/Config/config.hh>

//== BUILD-TIME DEPENDENCIES =================================================================
#if(COMISO_QT4_AVAILABLE)
//============================================================================================

//== INCLUDES =================================================================

#include "ui_QtMISolverDialogBaseUI.hh"

// ACGMake users have to include
// #include "QtMISolverDialogBase.hh"


#include <Solver/MISolver.hh>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO
{

//== CLASS DEFINITION =========================================================



/** \class MISolverDialog MISolverDialog.hh <COMISO/.../MISolverDialog.hh>

    Brief Description.

    A more elaborate description follows.
*/
class MISolverDialog
         : public QDialog, public Ui::QtMISolverDialogBaseUI
{
   Q_OBJECT
public:

   /// Default constructor
   MISolverDialog( MISolver&   _misolver,
                   QWidget*    _parent = 0 ):
         QDialog( _parent ),
         Ui::QtMISolverDialogBaseUI(),
         misolver_( _misolver )
   {
      setupUi( this );
      get_parameters();

      connect( okPB, SIGNAL( clicked() ),     this, SLOT( slotOkButton() ) );
      connect( cancelPB, SIGNAL( clicked() ), this, SLOT( slotCancelButton() ) );
   }

   /// Destructor
   ~MISolverDialog() {}

   void get_parameters();
   void set_parameters();


public slots:
   virtual void slotOkButton();
   virtual void slotCancelButton();

private:

   MISolver& misolver_;
};


//=============================================================================
} // namespace COMISO
//=============================================================================
#endif // COMISO_MISOLVERDIALOG_HH defined
//=============================================================================

#endif

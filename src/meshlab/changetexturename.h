/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#ifndef __VCGLIB_TEXTURE_RENAME
#define __VCGLIB_TEXTURE_RENAME

#include <QDialog>

namespace Ui
{
	class RenameTextureDialog;
}

class ChangeTextureNameDialog : public QDialog
{
	Q_OBJECT
public:
	ChangeTextureNameDialog(QWidget *parent);
	ChangeTextureNameDialog(QWidget *parent,std::string oldtexture);
	~ChangeTextureNameDialog();

	void InitDialog();
	std::string GetTextureName(){return texture;}

private slots:
	void SlotOkButton();
	void SlotCancelButton();
	void SlotSearchTextureName();

private:
	Ui::RenameTextureDialog* ui;
	std::string texture;
};//end class

namespace vcg {
namespace tri {
namespace io {
	
	class TextureRename
	{
	public:	
		inline static std::string GetNewTextureName(std::string oldtexturename)
		{
			ChangeTextureNameDialog dialog(new QWidget(),oldtexturename);
			dialog.exec();
			std::string name = dialog.GetTextureName();
			dialog.close();
			return name;
		}
	};
} // end Namespace tri
} // end Namespace io
} // end Namespace vcg

#endif

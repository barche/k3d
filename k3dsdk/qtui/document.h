#ifndef K3DSDK_QTUI_DOCUMENT_H
#define K3DSDK_QTUI_DOCUMENT_H

// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/qtui/node_wrapper.h>
#include <k3dsdk/qtui/property_wrapper.h>

#include <QObject>

namespace k3d
{

class idocument;

namespace qtui
{

/// Populates a newly-created document to provide users with a good out-of-box experience.
void populate_new_document(idocument& Document);

/// Wrapper class for a document, allowing access from QML
class document_model : public QObject
{
	Q_OBJECT
	Q_PROPERTY(property_wrapper* title MEMBER m_title CONSTANT)
public:
	document_model(idocument& Document);

	Q_INVOKABLE node_wrapper lookup_by_name(const QString& Name);

signals:
	void documentClosing();

private:
	idocument& m_document;

	void on_document_close();

	property_wrapper* m_title;
};

} // namespace qtui

} // namespace k3d

#endif // !K3DSDK_QTUI_DOCUMENT_H


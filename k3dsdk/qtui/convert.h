#ifndef K3DSDK_QTUI_CONVERT_H
#define K3DSDK_QTUI_CONVERT_H

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

#include <k3dsdk/color.h>
#include <k3dsdk/convert.h>
#include <k3dsdk/system.h>
#include <k3dsdk/type_registry.h>
#include <k3dsdk/types.h>

#include <QColor>
#include <QString>
#include <QUrl>
#include <QVariant>

#include <boost/any.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/vector.hpp>

namespace k3d
{

namespace qtui
{

/// Types for the conversion between any and variant. Pairs form a mapping, single entries are the same in Qt and K-3D
typedef boost::mpl::vector<
k3d::bool_t,
k3d::double_t,
k3d::int32_t,
boost::mpl::pair<k3d::string_t, QString>,
boost::mpl::pair<k3d::color, QColor>,
QObject*
> k3d_qt_types;

namespace detail
{

struct any_to_variant
{
	any_to_variant(const boost::any& From, QVariant& Result) : m_from(From), m_result(Result)
	{
	}

	// Called if source and target types are the same
	template<typename T>
	void operator()(T)
	{
		if(m_result.isValid())
			return;

		const T* tried_cast = boost::any_cast<const T>(&m_from);
		if(tried_cast != nullptr)
			m_result = QVariant::fromValue(*tried_cast);
	}

	// Called if source and target types differ
	template<typename A, typename B>
	void operator()(boost::mpl::pair<A,B>)
	{
		if(m_result.isValid())
			return;

		const A* tried_cast = boost::any_cast<const A>(&m_from);
		if(tried_cast != nullptr)
			m_result = QVariant(k3d::convert<B>(*tried_cast));
	}

	const boost::any& m_from;
	QVariant& m_result;
};

struct variant_to_any
{
	variant_to_any(const QVariant& From, boost::any& Result) : m_from(From), m_result(Result)
	{
	}

	// Called if source and target types are the same
	template<typename T>
	void operator()(T)
	{
		if(!m_result.empty())
			return;

		const int type_id = qMetaTypeId<T>();
		if(m_from.userType() == type_id)
		{
			m_result = m_from.value<T>();
		}
	}

	// Called if source and target types differ
	template<typename A, typename B>
	void operator()(boost::mpl::pair<A,B>)
	{
		if(!m_result.empty())
			return;

		const int type_id = qMetaTypeId<B>();
		if(m_from.userType() == type_id)
		{
			m_result = k3d::convert<A>(m_from.value<B>());
		}
	}

	const QVariant& m_from;
	boost::any& m_result;
};

}

}

/// Specialization of k3d::convert that converts k3d::string_t to QString.
template<>
inline QString convert(const string_t& From)
{
	return QString(From.c_str());
}

/// Specialization of k3d::convert that converts k3d::color to QColor.
template<>
inline QColor convert(const color& From)
{
	return QColor::fromRgbF(From.red, From.green, From.blue, 1.0);
}

/// Specialization of k3d::convert that converts QColor to k3d::color.
template<>
inline color convert(const QColor& From)
{
	return color(From.redF(), From.greenF(), From.blueF());
}

/// Specialization of k3d::convert that converts QString to k3d::string_t.
template<>
inline string_t convert(const QString& From)
{
	return From.toUtf8().constData();
}

/// Convert boost::any to QVariant
template<>
inline QVariant convert(const boost::any& From)
{
	QVariant result;
	boost::mpl::for_each<qtui::k3d_qt_types>(qtui::detail::any_to_variant(From, result));
	if(!result.isValid())
		k3d::log() << warning << "Failed to convert boost::any of type " << k3d::demangle(From.type()) << " to a QVariant" << std::endl;

	return result;
}

/// Convert QVariant to boost::any
template<>
inline boost::any convert(const QVariant& From)
{
	boost::any result;
	boost::mpl::for_each<qtui::k3d_qt_types>(qtui::detail::variant_to_any(From, result));
	if(result.empty())
		k3d::log() << warning << "Failed to convert QVariant of type " << From.typeName() << " to a QVariant" << std::endl;

	return result;
}

/// Convert boost::filesystem::path to QUrl
template<>
inline QUrl convert(const boost::filesystem::path& Path)
{
	return QUrl::fromLocalFile(k3d::convert<QString>(Path.generic_string()));
}

/// Convert QUrl to boost::filesystem::path
template<>
inline boost::filesystem::path convert(const QUrl& Path)
{
	return boost::filesystem::path(convert<k3d::string_t>(Path.toLocalFile()));
}

} // namespace k3d

#endif // !K3DSDK_QTUI_CONVERT_H


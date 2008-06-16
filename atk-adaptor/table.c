/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "accessible.h"

static AtkTable *
get_table (DBusMessage * message)
{
  AtkObject *obj = spi_dbus_get_object (dbus_message_get_path (message));
  if (!obj)
    return NULL;
  return ATK_TABLE (obj);
}

static AtkTable *
get_table_from_path (const char *path, void *user_data)
{
  AtkObject *obj = spi_dbus_get_object (path);
  if (!obj || !ATK_IS_TABLE(obj))
    return NULL;
  return ATK_TABLE (obj);
}

static dbus_bool_t
impl_get_nRows (const char *path, DBusMessageIter * iter, void *user_data)
{
  AtkTable *table = get_table_from_path (path, user_data);
  if (!table)
    return FALSE;
  return droute_return_v_int32 (iter, atk_table_get_n_rows (table));
}

static dbus_bool_t
impl_get_nColumns (const char *path, DBusMessageIter * iter, void *user_data)
{
  AtkTable *table = get_table_from_path (path, user_data);
  if (!table)
    return FALSE;
  return droute_return_v_int32 (iter, atk_table_get_n_columns (table));
}

static dbus_bool_t
impl_get_caption (const char *path, DBusMessageIter * iter, void *user_data)
{
  AtkTable *table = get_table_from_path (path, user_data);
  if (!table)
    return FALSE;
  return spi_dbus_return_v_object (iter, atk_table_get_caption (table),
				   FALSE);
}

static dbus_bool_t
impl_get_summary (const char *path, DBusMessageIter * iter, void *user_data)
{
  AtkTable *table = get_table_from_path (path, user_data);
  if (!table)
    return FALSE;
  return spi_dbus_return_v_object (iter, atk_table_get_summary (table),
				   FALSE);
}

static dbus_bool_t
impl_get_nSelectedRows (const char *path, DBusMessageIter * iter,
			void *user_data)
{
  AtkTable *table = get_table_from_path (path, user_data);
  gint *selected_rows = NULL;
  int count;
  if (!table)
    return FALSE;
  count = atk_table_get_selected_rows (table, &selected_rows);
  if (selected_rows)
    g_free (selected_rows);
  return droute_return_v_int32 (iter, count);
}

static dbus_bool_t
impl_get_nSelectedColumns (const char *path, DBusMessageIter * iter,
			   void *user_data)
{
  AtkTable *table = get_table_from_path (path, user_data);
  gint *selected_columns = NULL;
  int count;
  if (!table)
    return FALSE;
  count = atk_table_get_selected_columns (table, &selected_columns);
  if (selected_columns)
    g_free (selected_columns);
  return droute_return_v_int32 (iter, count);
}

static DBusMessage *
impl_getAccessibleAt (DBusConnection * bus, DBusMessage * message,
		      void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t row, column;
  DBusError error;
  AtkObject *obj;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &row, DBUS_TYPE_INT32, &column,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  obj = atk_table_ref_at (table, row, column);
  return spi_dbus_return_object (message, obj, TRUE);
}

static DBusMessage *
impl_getIndexAt (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t row, column;
  dbus_int32_t index;
  DBusError error;
  DBusMessage *reply;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &row, DBUS_TYPE_INT32, &column,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  index = atk_table_get_index_at (table, row, column);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &index,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_getRowAtIndex (DBusConnection * bus, DBusMessage * message,
		    void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t index;
  dbus_int32_t row;
  DBusError error;
  DBusMessage *reply;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &index, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  row = atk_table_get_row_at_index (table, index);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &row,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_getColumnAtIndex (DBusConnection * bus, DBusMessage * message,
		       void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t index;
  dbus_int32_t column;
  DBusError error;
  DBusMessage *reply;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &index, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  column = atk_table_get_column_at_index (table, index);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &column,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_getRowDescription (DBusConnection * bus, DBusMessage * message,
			void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t row;
  const gchar *description;
  DBusError error;
  DBusMessage *reply;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &row, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  description = atk_table_get_row_description (table, row);
  if (!description)
    description = "";
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &description,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_getColumnDescription (DBusConnection * bus, DBusMessage * message,
			   void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t column;
  const char *description;
  DBusError error;
  DBusMessage *reply;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &column, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  description = atk_table_get_column_description (table, column);
  if (!description)
    description = "";
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &description,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_getRowExtentAt (DBusConnection * bus, DBusMessage * message,
		     void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t row, column;
  dbus_int32_t extent;
  DBusError error;
  DBusMessage *reply;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &row, DBUS_TYPE_INT32, &column,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  extent = atk_table_get_row_extent_at (table, row, column);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &extent,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_getColumnExtentAt (DBusConnection * bus, DBusMessage * message,
			void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t row, column;
  dbus_int32_t extent;
  DBusError error;
  DBusMessage *reply;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &row, DBUS_TYPE_INT32, &column,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  extent = atk_table_get_column_extent_at (table, row, column);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &extent,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_getRowHeader (DBusConnection * bus, DBusMessage * message,
		   void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t row;
  DBusError error;
  AtkObject *obj;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &row, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  obj = atk_table_get_row_header (table, row);
  return spi_dbus_return_object (message, obj, FALSE);
}

static DBusMessage *
impl_getColumnHeader (DBusConnection * bus, DBusMessage * message,
		      void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t column;
  DBusError error;
  AtkObject *obj;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &column, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  obj = atk_table_get_column_header (table, column);
  return spi_dbus_return_object (message, obj, FALSE);
}

static DBusMessage *
impl_getSelectedRows (DBusConnection * bus, DBusMessage * message,
		      void *user_data)
{
  AtkTable *table = get_table (message);
  gint *selected_rows = NULL;
  gint count;
  DBusMessage *reply;

  if (!table)
    return spi_dbus_general_error (message);
  count = atk_table_get_selected_rows (table, &selected_rows);
  if (!selected_rows)
    count = 0;
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      /* tbd - figure out if this is safe for a 0-length array */
      dbus_message_append_args (reply, DBUS_TYPE_ARRAY, DBUS_TYPE_INT32,
				&selected_rows, count, DBUS_TYPE_INVALID);
    }
  if (selected_rows)
    g_free (selected_rows);
  return reply;
}

static DBusMessage *
impl_getSelectedColumns (DBusConnection * bus, DBusMessage * message,
			 void *user_data)
{
  AtkTable *table = get_table (message);
  gint *selected_columns = NULL;
  gint count;
  DBusMessage *reply;

  if (!table)
    return spi_dbus_general_error (message);
  count = atk_table_get_selected_columns (table, &selected_columns);
  if (!selected_columns)
    count = 0;
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      /* tbd - figure out if this is safe for a 0-length array */
      dbus_message_append_args (reply, DBUS_TYPE_ARRAY, DBUS_TYPE_INT32,
				&selected_columns, count, DBUS_TYPE_INVALID);
    }
  if (selected_columns)
    g_free (selected_columns);
  return reply;
}

static DBusMessage *
impl_isRowSelected (DBusConnection * bus, DBusMessage * message,
		    void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t row;
  DBusError error;
  DBusMessage *reply;
  dbus_bool_t ret;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &row, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  ret = atk_table_is_row_selected (table, row);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_isColumnSelected (DBusConnection * bus, DBusMessage * message,
		       void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t column;
  DBusError error;
  DBusMessage *reply;
  dbus_bool_t ret;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &column, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  ret = atk_table_is_column_selected (table, column);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_isSelected (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t row, column;
  DBusError error;
  DBusMessage *reply;
  dbus_bool_t ret;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &row, DBUS_TYPE_INT32, &column,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  ret = atk_table_is_selected (table, row, column);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_addRowSelection (DBusConnection * bus, DBusMessage * message,
		      void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t row;
  DBusError error;
  DBusMessage *reply;
  dbus_bool_t ret;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &row, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  ret = atk_table_add_row_selection (table, row);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_addColumnSelection (DBusConnection * bus, DBusMessage * message,
			 void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t column;
  DBusError error;
  DBusMessage *reply;
  dbus_bool_t ret;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &column, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  ret = atk_table_add_column_selection (table, column);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_removeRowSelection (DBusConnection * bus, DBusMessage * message,
			 void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t row;
  DBusError error;
  DBusMessage *reply;
  dbus_bool_t ret;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &row, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  ret = atk_table_remove_row_selection (table, row);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_removeColumnSelection (DBusConnection * bus, DBusMessage * message,
			    void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t column;
  DBusError error;
  DBusMessage *reply;
  dbus_bool_t ret;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &column, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  ret = atk_table_remove_column_selection (table, column);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_getRowColumnExtentsAtIndex (DBusConnection * bus, DBusMessage * message,
				 void *user_data)
{
  AtkTable *table = get_table (message);
  dbus_int32_t index;
  DBusError error;
  dbus_int32_t row, column, row_extents, col_extents;
  dbus_bool_t is_selected;
  dbus_bool_t ret;
  DBusMessage *reply;

  AtkObject *cell;
  AtkRole role;

  if (!table)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &index, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  column = atk_table_get_column_at_index (table, index);
  row = atk_table_get_row_at_index (table, index);
  row_extents = atk_table_get_row_extent_at (table, row, column);
  col_extents = atk_table_get_column_extent_at (table, row, column);
  is_selected = atk_table_is_selected (table, row, column);
  cell = atk_table_ref_at (table, row, column);
  role = atk_object_get_role (cell);
  g_object_unref (cell);
  ret = (role == ATK_ROLE_TABLE_CELL ? TRUE : FALSE);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, row, DBUS_TYPE_INT32,
				column, DBUS_TYPE_INT32, row_extents,
				DBUS_TYPE_INT32, col_extents,
				DBUS_TYPE_BOOLEAN, is_selected,
				DBUS_TYPE_BOOLEAN, &ret, DBUS_TYPE_INVALID);
    }
  return reply;
}

static DRouteMethod methods[] = {
  {impl_getAccessibleAt, "getAccessibleAt"},
  {impl_getIndexAt, "getIndexAt"},
  {impl_getRowAtIndex, "getRowAtIndex"},
  {impl_getColumnAtIndex, "getColumnAtIndex"},
  {impl_getRowDescription, "getRowDescription"},
  {impl_getColumnDescription, "getColumnDescription"},
  {impl_getRowExtentAt, "getRowExtentAt"},
  {impl_getColumnExtentAt, "getColumnExtentAt"},
  {impl_getRowHeader, "getRowHeader"},
  {impl_getColumnHeader, "getColumnHeader"},
  {impl_getSelectedRows, "getSelectedRows"},
  {impl_getSelectedColumns, "getSelectedColumns"},
  {impl_isRowSelected, "isRowSelected"},
  {impl_isColumnSelected, "isColumnSelected"},
  {impl_isSelected, "isSelected"},
  {impl_addRowSelection, "addRowSelection"},
  {impl_addColumnSelection, "addColumnSelection"},
  {impl_removeRowSelection, "removeRowSelection"},
  {impl_removeColumnSelection, "removeColumnSelection"},
  {impl_getRowColumnExtentsAtIndex, "getRowColumnExtentsAtIndex"},
  {NULL, NULL}
};

static DRouteProperty properties[] = {
  {impl_get_nRows, NULL, "nRows"},
  {impl_get_nColumns, NULL, "nColumns"},
  {impl_get_caption, NULL, "caption"},
  {impl_get_summary, NULL, "summary"},
  {impl_get_nSelectedRows, NULL, "nSelectedRows"},
  {impl_get_nSelectedColumns, NULL, "nSelectedColumns"},
  {NULL, NULL, NULL}
};

void
spi_initialize_table (DRouteData * data)
{
  droute_add_interface (data, SPI_DBUS_INTERFACE_TABLE, methods,
			properties,
			(DRouteGetDatumFunction) get_table_from_path, NULL);
};

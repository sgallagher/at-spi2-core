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

#define get_object(message) spi_dbus_get_object(dbus_message_get_path(message))

#define TREE_UPDATE_ACCESSIBLE 0
#define TREE_REMOVE_ACCESSIBLE 1

static dbus_bool_t
append_update (DBusMessageIter * iter_array, AtkObject * obj,
			     dbus_bool_t include_children, DRouteData * data)
{
  DBusMessageIter iter_struct, iter_sub_array;
  char *path = NULL;
  char *path_parent;
  const char *name, *desc;
  dbus_uint16_t updating = TREE_UPDATE_ACCESSIBLE;
  int i;
  dbus_uint32_t role;

  gint childcount;
  GSList *l;

  dbus_message_iter_open_container (iter_array, DBUS_TYPE_STRUCT, NULL,
				    &iter_struct);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT16, &updating);
  path = spi_dbus_get_path (obj);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path);
  path_parent = spi_dbus_get_path (atk_object_get_parent(obj));
  if (!path_parent) path_parent = g_strdup("/");
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path_parent);
  g_free(path_parent);
  dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "o",
				    &iter_sub_array);
  childcount = atk_object_get_n_accessible_children (obj);
  for (i = 0; i < childcount; i++)
    {
      AtkObject *child = atk_object_ref_accessible_child (obj, i);
      char *child_path = spi_dbus_get_path (child);
      if (child_path)
	{
	  dbus_message_iter_append_basic (&iter_sub_array,
					  DBUS_TYPE_OBJECT_PATH, &child_path);
	  g_free (child_path);
	}
      if (child)
	g_object_unref (child);
    }
  if (!dbus_message_iter_close_container (&iter_struct, &iter_sub_array))
    goto oom;
  dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "s",
				    &iter_sub_array);
  for (l = data->interfaces; l; l = g_slist_next (l))
    {
      DRouteInterface *iface_def = (DRouteInterface *) l->data;
      void *datum = NULL;
      if (iface_def->get_datum)
	{
	  datum = (*iface_def->get_datum) (path, data->user_data);
	  if (!datum)
	    continue;
	}
      dbus_message_iter_append_basic (&iter_sub_array, DBUS_TYPE_STRING,
				      &iface_def->name);
      if (iface_def->free_datum)
	(*iface_def->free_datum) (datum);
    }
  if (!dbus_message_iter_close_container (&iter_struct, &iter_sub_array))
    goto oom;
  name = atk_object_get_name (obj);
  if (!name)
    name = "";
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &name);
  role = spi_accessible_role_from_atk_role (atk_object_get_role (obj));
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT32, &role);
  desc = atk_object_get_description (obj);
  if (!desc)
    desc = "";
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &desc);
  if (!dbus_message_iter_close_container (iter_array, &iter_struct))
    goto oom;
  if (!include_children) childcount = 0;
  for (i = 0; i < childcount; i++)
    {
      AtkObject *child = atk_object_ref_accessible_child (obj, i);
      dbus_bool_t result;
      if (!child)
	continue;
      result = append_update (iter_array, child, TRUE, data);
      g_object_unref (child);
      if (!result)
	goto oom;
    }
  g_free (path);
  return TRUE;
oom:
  if (path) g_free(path);
  return FALSE;
}

static dbus_bool_t
append_remove (DBusMessageIter * iter_array, const char *path,
			     DRouteData * data)
{
  DBusMessageIter iter_struct, iter_sub_array;
  char *path_parent;
  const char *name, *desc;
  dbus_uint16_t updating = TREE_REMOVE_ACCESSIBLE;
  dbus_uint32_t role;

  dbus_message_iter_open_container (iter_array, DBUS_TYPE_STRUCT, NULL,
				    &iter_struct);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT16, &updating);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path);
  path_parent = "/";
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path_parent);
  dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "o",
				    &iter_sub_array);
  if (!dbus_message_iter_close_container (&iter_struct, &iter_sub_array))
    goto oom;
  dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "s",
				    &iter_sub_array);
  if (!dbus_message_iter_close_container (&iter_struct, &iter_sub_array))
    goto oom;
  name = "";
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &name);
  role = 0;
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT32, &role);
  desc = "";
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &desc);
  if (!dbus_message_iter_close_container (iter_array, &iter_struct))
    goto oom;
  return TRUE;
oom:
  return FALSE;
}

dbus_bool_t
spi_dbus_append_tree (DBusMessage * message, AtkObject * obj,
		      DRouteData * data)
{
  DBusMessageIter iter, iter_array;
  dbus_bool_t result;

  dbus_message_iter_init_append (message, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "(qooaoassus)",
				    &iter_array);
  result = append_update (&iter_array, obj, TRUE, data);
  if (result)
    result = dbus_message_iter_close_container (&iter, &iter_array);
  return result;
}

static DBusMessage *
impl_getRoot (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkObject *root = atk_get_root();
  char *path;
  DBusMessage *reply;

  if (root) path = spi_dbus_get_path(root);
  if (!root || !path)
    return spi_dbus_general_error (message);
  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_OBJECT_PATH, &path,
			    DBUS_TYPE_INVALID);
  g_free (path);
  return reply;
}

static DBusMessage *
impl_getTree (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  DBusMessage *reply;
  AtkObject *root = atk_get_root();

  if (!root) return spi_dbus_general_error(message);
  reply = dbus_message_new_method_return (message);
  spi_dbus_append_tree (reply, root, (DRouteData *) user_data);
  return reply;
}

static DRouteMethod methods[] = {
  {DROUTE_METHOD, impl_getRoot, "getRoot", "o,root,o" },
  {DROUTE_METHOD, impl_getTree, "getTree", "a(qooaoassus),tree,o", TRUE},
  {0, NULL, NULL, NULL}
};

void
spi_initialize_tree (DRouteData * data)
{
  droute_add_interface (data, "org.freedesktop.atspi.Tree",
			methods, NULL, NULL, NULL);
};

static GHashTable *cache_list;

#define UPDATE_NEW     1
#define UPDATE_REFRESH 2
#define UPDATE_REMOVE 3

static int update_pending = 0;
static gint update_pending_id;

typedef struct
{
  DBusMessageIter iter;
  DRouteData *droute;
} CacheIterData;

static void handle_cache_item(char *path, guint action, CacheIterData *d)
{
  AtkObject *obj;

  switch (action)
  {
  case UPDATE_NEW:
  case UPDATE_REFRESH:
  default:
    obj = spi_dbus_get_object(path);
//printf("update %s\n", path);
    append_update(&d->iter, obj, FALSE, d->droute);
    break;
  case UPDATE_REMOVE:
//printf("remove: %s\n", path);
    append_remove(&d->iter, path, d->droute);
    break;
  }
  g_hash_table_remove(cache_list, path);
  }

gboolean spi_dbus_update_cache(DRouteData *data)
{
  DBusMessage *message;
  DBusMessageIter iter;
  CacheIterData d;

  if (update_pending == 0) return FALSE;
//printf("Sending cache\n");
  message = dbus_message_new_signal("/org/freedesktop/atspi/tree", "org.freedesktop.atspi.Tree", "UpdateTree");
  if (!message) goto done;
  dbus_message_iter_init_append (message, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "(qooaoassus)",
				    &d.iter);
  d.droute = data;
  do
  {
    /* This loop is needed because appending an item may cause new children
     * to be registered and consequently added to the hash, so they, too,
     * will need to be sent with the update */
    update_pending = 0;
    g_hash_table_foreach(cache_list, (GHFunc)handle_cache_item, &d);
  } while (update_pending);
  dbus_message_iter_close_container(&iter, &d.iter);
  dbus_connection_send(data->bus, message, NULL);
done:
  return FALSE;
}

void spi_dbus_notify_change(AtkObject *obj, gboolean new, DRouteData *data)
{
  guint action = (new? UPDATE_NEW: UPDATE_REFRESH);
  char *path = spi_dbus_get_path(obj);

  if (!cache_list)
  {
    cache_list = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    if (!cache_list)
    {
      g_free(path);
      return;
    }
  }
  if (g_hash_table_lookup(cache_list, path))
  {
    g_free(path);
    return;
  }
//printf("change: %s\n", path);
  g_hash_table_insert(cache_list, path, (gpointer)action);
  if (update_pending != 2 && data)
  {
    update_pending_id = g_idle_add((GSourceFunc)spi_dbus_update_cache, data);
    update_pending = 2;
  }
  else if (!update_pending) update_pending = 1;
}

void spi_dbus_notify_remove(AtkObject *obj, DRouteData *data)
{
  guint action = UPDATE_REMOVE;
  guint cur_action;
  gchar *path = spi_dbus_get_path(obj);

//printf("notify remove: %s\n", path);
  if (!cache_list)
  {
    g_free(path);
    return;
  }
  cur_action = (guint)g_hash_table_lookup(cache_list, path);
  if (cur_action == UPDATE_NEW)
  {
    /* No one knew that this object ever existed, so just remove it */
//printf("Removing object from send queue\n");
    g_hash_table_remove(cache_list, path);
    g_free(path);
  }
  else
  {
    g_hash_table_insert(cache_list, path, (gpointer)action);
    if (update_pending != 2 && data)
    {
      update_pending_id = g_idle_add((GSourceFunc)spi_dbus_update_cache, data);
      update_pending = 2;
    }
    else if (!update_pending) update_pending = 1;
  }
}

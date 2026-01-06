/*
 * MIT License
 *
 * Copyright (c) 2017 Serge Zaitsev
 * Copyright (c) 2022 Steffen André Langnes
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef WEBVIEW_PLATFORM_LINUX_GTK_COMPAT_HH
#define WEBVIEW_PLATFORM_LINUX_GTK_COMPAT_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../../../macros.h"

#if defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)

#include <gtk/gtk.h>

#if GTK_MAJOR_VERSION >= 4

#ifdef GDK_WINDOWING_X11
#include <gdk/x11/gdkx.h>
#endif

#elif GTK_MAJOR_VERSION >= 3

#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#endif

#endif

namespace webview {
namespace detail {

/**
 * GTK compatibility helper class.
 */
class gtk_compat {
public:
  static gboolean init_check() {
#if GTK_MAJOR_VERSION >= 4
    return gtk_init_check();
#else
    return gtk_init_check(nullptr, nullptr);
#endif
  }

  static GtkWidget *window_new() {
#if GTK_MAJOR_VERSION >= 4
    return gtk_window_new();
#else
    return gtk_window_new(GTK_WINDOW_TOPLEVEL);
#endif
  }

  static void window_set_child(GtkWindow *window, GtkWidget *widget) {
#if GTK_MAJOR_VERSION >= 4
    gtk_window_set_child(window, widget);
#else
    gtk_container_add(GTK_CONTAINER(window), widget);
#endif
  }

  static void window_remove_child(GtkWindow *window, GtkWidget *widget) {
#if GTK_MAJOR_VERSION >= 4
    if (gtk_window_get_child(window) == widget) {
      gtk_window_set_child(window, nullptr);
    }
#else
    gtk_container_remove(GTK_CONTAINER(window), widget);
#endif
  }

  static void widget_set_visible(GtkWidget *widget, bool visible) {
#if GTK_MAJOR_VERSION >= 4
    gtk_widget_set_visible(widget, visible ? TRUE : FALSE);
#else
    if (visible) {
      gtk_widget_show(widget);
    } else {
      gtk_widget_hide(widget);
    }
#endif
  }

  static void window_set_size(GtkWindow *window, int width, int height) {
    // GTK 4 can set a default window size, but unlike GTK 3 it can't resize
    // the window after it has been set up.
#if GTK_MAJOR_VERSION >= 4
    gtk_window_set_default_size(window, width, height);
#else
    gtk_window_resize(window, width, height);
#endif
  }

  static void window_set_max_size(GtkWindow *window, int width, int height) {
// X11-specific features are available in GTK 3 but not GTK 4
#if GTK_MAJOR_VERSION < 4
    GdkGeometry g{};
    g.max_width = width;
    g.max_height = height;
    GdkWindowHints h = GDK_HINT_MAX_SIZE;
    gtk_window_set_geometry_hints(GTK_WINDOW(window), nullptr, &g, h);
#else
    // Avoid "unused parameter" warnings
    (void)window;
    (void)width;
    (void)height;
#endif
  }

  static void widget_set_opacity(GtkWidget *widget, double opacity) {
    gtk_widget_set_opacity(widget, opacity);
  }

  static void window_set_keep_above(GtkWindow *window, gboolean setting) {
#if GTK_MAJOR_VERSION < 4
    gtk_window_set_keep_above(window, setting);
#else
    // GTK 4 does not have a direct equivalent to gtk_window_set_keep_above.
    // It's considered a window manager hint that is not supported by the GTK 4 API.
    (void)window;
    (void)setting;
#endif
  }

  static void widget_add_css_class(GtkWidget *widget, const char *class_name) {
#if GTK_MAJOR_VERSION >= 4
    gtk_widget_add_css_class(widget, class_name);
#else
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_class(context, class_name);
#endif
  }

  static void widget_remove_css_class(GtkWidget *widget,
                                      const char *class_name) {
#if GTK_MAJOR_VERSION >= 4
    gtk_widget_remove_css_class(widget, class_name);
#else
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_remove_class(context, class_name);
#endif
  }

  static void css_provider_load_from_data(GtkCssProvider *provider,
                                          const char *data, gssize length) {
#if GTK_MAJOR_VERSION >= 4
    gtk_css_provider_load_from_string(provider, data);
    (void)length;
#else
    gtk_css_provider_load_from_data(provider, data, length, nullptr);
#endif
  }

  static void widget_add_style_provider(GtkWidget *widget,
                                        GtkStyleProvider *provider,
                                        guint priority) {
#if GTK_MAJOR_VERSION >= 4
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, provider, priority);
#pragma GCC diagnostic pop
#else
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, provider, priority);
#endif
  }
};

} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_LINUX_GTK_COMPAT_HH

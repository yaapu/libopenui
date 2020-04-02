/*
 * Copyright (C) OpenTX
 *
 * Source:
 *  https://github.com/opentx/libopenui
 *
 * This file is a part of libopenui library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#ifndef _STATIC_H_
#define _STATIC_H_

#include "window.h"
#include "button.h" // TODO just for BUTTON_BACKGROUND

class StaticText: public Window {
  public:
    StaticText(Window * parent, const rect_t & rect, std::string text = "", WindowFlags windowFlags = 0, LcdFlags textFlags = 0) :
      Window(parent, rect, windowFlags, textFlags),
      text(std::move(text))
    {
    }

#if defined(DEBUG_WINDOWS)
    std::string getName() override
    {
      return "StaticText \"" + text + "\"";
    }
#endif

    void paint(BitmapBuffer * dc) override;

    void setText(std::string value)
    {
      text = std::move(value);
      invalidate();
    }

  protected:
    std::string text;
};

class Subtitle: public StaticText {
  public:
    Subtitle(Window * parent, const rect_t & rect, const char * text):
      StaticText(parent, rect, text, 0, FONT(BOLD))
    {
    }

#if defined(DEBUG_WINDOWS)
    std::string getName() override
    {
      return "Subtitle \"" + text + "\"";
    }
#endif
};

class StaticBitmap: public Window {
  public:
    StaticBitmap(Window * parent, const rect_t & rect, bool scale = false):
      Window(parent, rect),
      scale(scale)
    {
    }

    StaticBitmap(Window * parent, const rect_t & rect, const char * filename, bool scale = false):
      Window(parent, rect),
      bitmap(BitmapBuffer::loadBitmap(filename)),
      scale(scale)
    {
    }

    StaticBitmap(Window * parent, const rect_t & rect, const BitmapBuffer * bitmap, bool scale = false):
      Window(parent, rect),
      bitmap(bitmap),
      scale(scale)
    {
    }

    void setBitmap(const char * filename)
    {
      setBitmap(BitmapBuffer::loadBitmap(filename));
    }

    void setBitmap(const BitmapBuffer * newBitmap)
    {
      delete bitmap;
      bitmap = newBitmap;
      invalidate();
    }

#if defined(DEBUG_WINDOWS)
    std::string getName() override
    {
      return "StaticBitmap";
    }
#endif

    void paint(BitmapBuffer * dc) override
    {
      if (bitmap) {
        if (scale)
          dc->drawScaledBitmap(bitmap, 0, 0, width(), height());
        else
          dc->drawBitmap((width() - bitmap->width()) / 2, (height() - bitmap->height()) / 2, bitmap);
      }
    }

  protected:
    const BitmapBuffer * bitmap = nullptr;
    bool scale = false;
};

class DynamicText: public StaticText
{
  public:
    DynamicText(Window * parent, const rect_t & rect, std::function<std::string()> textHandler):
      StaticText(parent, rect),
      textHandler(std::move(textHandler))
    {
    }

    void checkEvents() override
    {
      std::string newText = textHandler();
      if (newText != text) {
        text = newText;
        invalidate();
      }
    }

  protected:
    std::function<std::string()> textHandler;
};

template <class T>
class DynamicNumber: public Window
{
  public:
    DynamicNumber(Window * parent, const rect_t & rect, std::function<T()> numberHandler, LcdFlags textFlags = 0, const char * prefix = nullptr, const char * suffix = nullptr):
      Window(parent, rect, 0, textFlags),
      numberHandler(std::move(numberHandler)),
      prefix(prefix),
      suffix(suffix)
    {
    }

    void paint(BitmapBuffer * dc) override
    {
      dc->drawNumber(0, FIELD_PADDING_TOP, value, textFlags, 0, prefix, suffix);
    }

    void checkEvents() override
    {
      T newValue = numberHandler();
      if (value != newValue) {
        value = newValue;
        invalidate();
      }
    }

  protected:
    T value = 0;
    std::function<T()> numberHandler;
    const char * prefix;
    const char * suffix;
};

#endif

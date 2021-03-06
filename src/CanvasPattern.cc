
//
// Pattern.cc
//
// Copyright (c) 2010 LearnBoost <tj@learnboost.com>
//

#include "Canvas.h"
#include "Image.h"
#include "CanvasPattern.h"

const cairo_user_data_key_t *pattern_repeat_key;

Nan::Persistent<FunctionTemplate> Pattern::constructor;

/*
 * Initialize CanvasPattern.
 */

void
Pattern::Initialize(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target) {
  Nan::HandleScope scope;

  // Constructor
  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(Pattern::New);
  constructor.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("CanvasPattern").ToLocalChecked());

  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("CanvasPattern").ToLocalChecked());

  // Prototype
  Nan::Set(target, Nan::New("CanvasPattern").ToLocalChecked(), ctor->GetFunction());
}

/*
 * Initialize a new CanvasPattern.
 */

NAN_METHOD(Pattern::New) {
  if (!info.IsConstructCall()) {
    return Nan::ThrowTypeError("Class constructors cannot be invoked without 'new'");
  }

  cairo_surface_t *surface;

  Local<Object> obj = info[0]->ToObject();

  // Image
  if (Nan::New(Image::constructor)->HasInstance(obj)) {
    Image *img = Nan::ObjectWrap::Unwrap<Image>(obj);
    if (!img->isComplete()) {
      return Nan::ThrowError("Image given has not completed loading");
    }
    surface = img->surface();

  // Canvas
  } else if (Nan::New(Canvas::constructor)->HasInstance(obj)) {
    Canvas *canvas = Nan::ObjectWrap::Unwrap<Canvas>(obj);
    surface = canvas->surface();
  // Invalid
  } else {
    return Nan::ThrowTypeError("Image or Canvas expected");
  }
  repeat_type_t repeat = REPEAT;
  if (0 == strcmp("no-repeat", *String::Utf8Value(info[1]))) {
    repeat = NO_REPEAT;
  } else if (0 == strcmp("repeat-x", *String::Utf8Value(info[1]))) {
    repeat = REPEAT_X;
  } else if (0 == strcmp("repeat-y", *String::Utf8Value(info[1]))) {
    repeat = REPEAT_Y;
  }
  Pattern *pattern = new Pattern(surface, repeat);
  pattern->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

/*
 * Initialize pattern.
 */

Pattern::Pattern(cairo_surface_t *surface, repeat_type_t repeat) {
  _pattern = cairo_pattern_create_for_surface(surface);
  _repeat = repeat;
  cairo_pattern_set_user_data(_pattern, pattern_repeat_key, &_repeat, NULL);
}

repeat_type_t Pattern::get_repeat_type_for_cairo_pattern(cairo_pattern_t *pattern) {
  void *ud = cairo_pattern_get_user_data(pattern, pattern_repeat_key);
  return *reinterpret_cast<repeat_type_t*>(ud);
}

/*
 * Destroy the pattern.
 */

Pattern::~Pattern() {
  cairo_pattern_destroy(_pattern);
}

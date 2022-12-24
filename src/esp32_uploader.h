#ifndef _ESP32_UPLOADER_H_
#define _ESP32_UPLOADER_H_

#include "Arduino.h"
#include <HTTPClient.h>

struct Response
{
  Response() = default;
  int status_code;
  String text;
};

Response esp32_uploader_test();  // 1 kB simulated file.

Response esp32_uploader(const char *url,
                        followRedirects_t redirects,
                        const char *filename,
                        const char *mimetype,
                        const uint8_t *data, size_t n_bytes,
                        const String *headers, // array of headers of the form "key: value"
                        int num_headers);      // number of headers

#endif // _ESP32_UPLOADER_H_

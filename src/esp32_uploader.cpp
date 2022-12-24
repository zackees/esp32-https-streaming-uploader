#include "esp32_uploader.h"
#include "Wifi.h"
#include <HTTPClient.h>

#ifndef ASSERT
#define ASSERT(X, MSG) assert(X)
#endif

#define CONST_STR_LEN(x) (sizeof(x) - 1)

namespace
{
  // Header data for the HTTP POST request
  const char HEADER1[] = "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"datafile\"; filename=\"";
  const char HEADER2[] = "\"\r\nContent-Type: ";
  const char HEADER3[] = "\r\n\r\n";
  // Footer data for the http POST request
  const char FOOTER[] = "\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--";

  class FileUploadStream : public Stream
  {
  public:
    FileUploadStream() = delete;
    FileUploadStream(const char *filename, const char *mimetype, const uint8_t *data, size_t n_bytes)
        : mBufferIndex(0)
    {
      init(filename, mimetype, data, n_bytes);
    }
    virtual ~FileUploadStream() {}

    void init(const char *filename, const char *mimetype, const uint8_t *data, size_t n_bytes)
    {
      mNumDataBytes = n_bytes;
      mData = data;
      // Reserve enough space for the mHeader, rather than just concatenating
      // and fragmenting memory.
      size_t reserve_size = 0;
      reserve_size += CONST_STR_LEN(HEADER1);
      reserve_size += CONST_STR_LEN(filename);
      reserve_size += CONST_STR_LEN(HEADER2);
      reserve_size += CONST_STR_LEN(mimetype);
      reserve_size += CONST_STR_LEN(HEADER3);
      mHeader.reserve(reserve_size + 1);
      mHeader += HEADER1;
      mHeader += filename;
      mHeader += HEADER2;
      mHeader += mimetype;
      mHeader += HEADER3;
    }

    virtual int available()
    {
      ASSERT(mBufferIndex <= size(), "Index out of bounds");
      return size() - mBufferIndex;
    }
    virtual int read()
    {
      if (mBufferIndex < size())
      {
        return at(mBufferIndex++);
      }
      return -1;
    }
    virtual int peek()
    {
      if (mBufferIndex < size())
      {
        return at(mBufferIndex);
      }
      return -1;
    }
    virtual void flush() {}
    virtual size_t write(uint8_t c)
    {
      ASSERT(false, "Not implemented");
    }
    virtual size_t write(const uint8_t *buffer, size_t size)
    {
      ASSERT(false, "Not implemented");
    }

    // Allows the index to span the header, data, and footer.
    int at(size_t idx) const
    {
      ASSERT(idx < size(), "Index out of bounds");
      if (idx < mHeader.length())
      {
        return mHeader.charAt(idx);
      }
      idx -= mHeader.length();
      if (idx < mNumDataBytes)
      {
        return mData[idx];
      }
      idx -= mNumDataBytes;
      if (idx < CONST_STR_LEN(FOOTER))
      {
        return FOOTER[idx];
      }
      return -1;
    }

    size_t size() const
    {
      return mHeader.length() + mNumDataBytes + CONST_STR_LEN(FOOTER);
    }

    String mHeader;
    // String mBuffer;
    int mBufferIndex = 0;
    const uint8_t *mData = nullptr;
    size_t mNumDataBytes = 0;
  };

  void populate_headers(const String *headers, int num_headers, HTTPClient *http)
  {
    if (!headers || num_headers <= 0)
    {
      return;
    }
    for (int i = 0; i < num_headers; i++)
    {
      // split headers on colon
      const String &header = headers[i];
      int colon = header.indexOf(':');
      if (colon > 0)
      {
        String key = header.substring(0, colon);
        String value = header.substring(colon + 1);
        value.trim();
        http->addHeader(key, value);
      } else {
	Serial.printf("Warning, skipping invalid header %s\n", header.c_str());
      }
    }
  }
}

Response esp32_uploader(const char *url,
                        followRedirects_t redirects,
                        const char *filename,
                        const char *mimetype,
                        const uint8_t *data, size_t n_bytes,
                        const String *headers, // array of headers
                        int num_headers)
{
  Response response;
  HTTPClient http;
  http.setFollowRedirects(redirects);
  http.begin(url);
  http.addHeader("Content-Type", "multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
  http.addHeader("Content-Length", String(n_bytes));
  http.addHeader("Content-Disposition", "attachment; filename=" + String(filename));
  populate_headers(headers, num_headers, &http);
  FileUploadStream file_stream(filename, mimetype, data, n_bytes);
  response.status_code = http.sendRequest("POST", &file_stream, file_stream.size());
  response.text = http.getString();
  http.end();
  return response;
}

Response esp32_uploader_test()
{
  const char *url = "https://example/test_upload";
  String fileName = "testfile2.mp3";
  const int nFileSize = 1024; // 1 kB
  uint8_t *fakeFile = static_cast<uint8_t *>(calloc(sizeof(uint8_t), nFileSize));
  String header = "API-KEY: 1234";
  Response response = esp32_uploader(
	  url, HTTPC_STRICT_FOLLOW_REDIRECTS,
	  fileName.c_str(), "mpeg/audio", fakeFile, nFileSize,
	  &header, 1);
  free(fakeFile);
  return response;
}

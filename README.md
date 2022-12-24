# esp32-https-streaming-uploader

A streaming uploader for ESP32 that supports large files and https.

```C++
Response esp32_uploader_test()
{
  const char *url = "https://example/test_upload";
  String fileName = "testfile.mp3";
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
```

This libary requires the file to be in memory (which is my usage scenario).

The file to be uploaded is streamed in place, with no copies. The test code
in this example will create a 1 kB dummy file. But a test file with 2 MB has
been tested (using psram) and it worked fine.

Modifying this code to use something like an SD card should be straightforward.

Pull requests are welcome.

If you like this code please give this repo a star.

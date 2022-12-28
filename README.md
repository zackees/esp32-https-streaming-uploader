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

Not that the default SSL method uses insecure. You should modify it if you want to
use a CA cert and further lock it down.

## Upload form should use "datafile" as the upload target.

This can be changed easily but I simply haven't done this yet for this library. So keep that in mind
for your solution.

## SD Card usage

Modifying this code to use something like an SD card should be straightforward.

If you like this code please give this repo a star.
Note that the fieldname "datafile" is currently hard wired into this lib. So your
endpoint must specify that upload file as "datafile" or you will get a status code
422.

Pull requests are welcome.

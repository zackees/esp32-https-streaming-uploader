# esp32-https-streaming-uploader

A streaming uploader for ESP32 that supports large files and https.

This libary requires the file to be in memory (which is my usage scenario).

The file to be uploaded is streamed in place, with no copies. The test code
in this example will create a 1 kB dummy file. But a test file with 2 MB has
been tested (using psram) and it worked fine.

Modifying this code to use something like an SD card should be straightforward.

Pull requests are welcome.
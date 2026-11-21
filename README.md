

## Compile

```
g++ -std=c++17 -Iinclude -fsyntax-only logger.cpp sinkFactory.cpp fileSink.cpp timeFormats.cpp utils.cpp
```

## Test

```
g++ -std=c++17 -Iinclude tests\loggerTest.cpp sinkFactory.cpp fileSink.cpp timeFormats.cpp utils.cpp -lz -o loggerTest.exe
.\loggerTest.exe
```

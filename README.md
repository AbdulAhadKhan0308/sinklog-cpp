SinkFactory: it contains pre-written sinks as well a registerSink function for
consumer to add their own sink
TimeFormats: has implementation for a timeformatter 
Logger: contains the logger singelton, enriches message with timestamp and routes to sink to be written. Each sink can map multiple levels, but each level is tied to 1 sink.


## Compile

```
g++ -std=c++17 -Iinclude main.cpp sinkFactory.cpp fileSink.cpp consoleSink.cpp timeFormats.cpp utils.cpp -lz -o main.exe
.\main.exe
```

## Test

```
g++ -std=c++17 -Iinclude tests\loggerTest.cpp sinkFactory.cpp consoleSink.cpp timeFormats.cpp utils.cpp -o loggerTest.exe
.\loggerTest.exe
```

## create library

```
g++ -std=c++17 -Iinclude -c sinkFactory.cpp fileSink.cpp consoleSink.cpp timeFormats.cpp utils.cpp
ar rcs liblogging.a sinkFactory.o fileSink.o consoleSink.o timeFormats.o utils.o
```

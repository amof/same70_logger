# Logger
A simple logging library implemented in C99

![screenshot](https://cloud.githubusercontent.com/assets/3920290/23831970/a2415e96-0723-11e7-9886-f8f5d2de60fe.png)

This library has been reworked to be compatible to microcontrolleur environments.

## Dependencies

Mandatory dependency: None

Optionnal dependencies:
* ASF (add library depending what you want to use to output)
* Serial_MDW (https://github.com/amof/same70_serial_mdw)
* Lwip (https://github.com/amof/same70_lwIP)

These ones can be customized with what you have/want.

## Usage
The library provides 6 function-like macros for logging:

```c
log_trace(const char *fmt, ...);
log_debug(const char *fmt, ...);
log_info(const char *fmt, ...);
log_warn(const char *fmt, ...);
log_error(const char *fmt, ...);
log_fatal(const char *fmt, ...);
```

Each function takes a printf format string followed by additional arguments:

```c
log_trace("Hello %s", "world")
```

Resulting in a line with the given format printed to stderr:

```
20:18:26 TRACE src/main.c:11: Hello world
```

## License
This library is free software; you can redistribute it and/or modify it under
the terms of the MIT license. See [LICENSE](LICENSE) for details.

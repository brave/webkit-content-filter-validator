# webkit-content-filter-validator

Compile JSON rules into a [WebKit content filter store](https://webkitgtk.org/reference/webkit2gtk/2.36.2/WebKitUserContentFilterStore.html) to check for syntax errors.

To use, input a file in Apple's [JSON content blocker format](https://developer.apple.com/documentation/safariservices/creating_a_content_blocker), either as the first argument or through stdin.

`webkit-content-filter-validator` will silently return a zero exit code if the file is well-formatted. Otherwise, it will print WebKit's error to stdout and return a nonzero exit code.

## Dependencies

`webkit-content-filter-validator` depends on [WebKit WPE](https://wpewebkit.org/).

## Building

Run `make webkit-content-filter-validator`.

## Running

Check the usage:

```
webkit-content-filter-validator --help
```

## Developing

Run `make compile_flags.txt` to setup `clangd` IDE integration.

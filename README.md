# stac
------

stac is a simple programming language which can be used to generate static,
but also dynamic content - it reads a file with embedded C code onto input
and writes out compilable C code to output. Finally good alternative to PHP,
node.js and other bloatware!

Slightly inspired by node.c created by Tsoding:
	(https://github.com/tsoding/node.c).

## Usage
--------
Use `stac` script - just give input (.stac) file and you are ready to go!
you can also add optional arguments:
* `[-o OUTPUT]` - give an output file, without that option stac will
  produce content to stdout
* `[-t TEMPLATE]` - template file; usable for building more advanced
  websites
* `[-CC CC]` - compiler to use (by default gcc)
* `[-CFLAGS FLAGS]` - additional flags for the compiler

For example:
```
% ./stac in/index.stac
<prints compiled in/index.stac>
% ./stac -t templates/basic.stac in/index.stac
<prints compiled in/index.stac inside template templates/basic.stac>
```

You can also use a `compile` script to just generate C code - syntax is
just like in `stac` script, but without `[-CFLAGS]` and `[-CC]` options.

## Introduction
---------------
stac code is just data. In that data you can embed C code - you must
surround it by percent signs. Example:
```
<html>
	<body>
		<p>% echo("Welcome from C-mode!"); %</p>
	</body>
</html>
```

You can also embed plain data inside C code - of course, just by
surrounding the data by percent signs - for example write:
```
<html>
	<body>
		% for (int i = 0; i < 32; ++i) {
			% Hello from <b>data-mode</b>! %
		} %
	</body>
</html>
```
and you have loop which will produce data 32 times! Awesome!

Technically, the data-mode chunk is statement, more precisely -
`write()` function, so it will print text to a file.

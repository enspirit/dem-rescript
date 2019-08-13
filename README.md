# Doc-e-mate, your document's best friend

This repository holds the `dem` commandline tool, that allows your to generate
and print documents written in Markdown, styled in CSS, with business data
injected from JSON or YAML files.

```
> Text     + Data  + Style  ->  Beautiful Business Documents
> HTML     + Json  + CSS    ->  Beautiful Business Documents
> Markdown + YAML  + SASS   ->  Beautiful Maintainable Business Document
```

## Examples

To get started, try:

```
dem --help
dem compile --help
```

Use the `compile` sub-command to generate a document from markdown, css and
json sources in specific files:

```
dem compile -t examples/index.html.tpl -s examples/index.css  -T examples/index.md -D examples/index.json
```

Otherwise, skip lengthy parameters by following doc-e-mate conventions: current
folder having `index.md` (markdown text), `index.css` (style) and `index.json`
(data) files, as well as an `index.html.tpl` html final template:

```
dem compile
```

## Hacking

The `dem` commandline is written in BuckleScript, wth great help from the
following libraries:
* mustache.js, see https://mustache.github.io/
* markdown-it, see https://github.com/markdown-it/markdown-it and https://spec.commonmark.org/

### Getting started with the source code

Clone the repository, then run the following command to initialize your dev
environment:

```
npm install
```

On certain systems, It might be necessary to run `npm install bs-platform`
first.

### Building the tool

You can build the program with

```
npm run build
```

### Running tests

You can run the tests using:

```
npm run test
```

### Running `dem` in development mode

In development, instead of using the `dem` commandline itself, simply run the
local version:

```
node src/cli.bs.js
```

Note: `npm run build` automatically prepends a node shebang `#!/usr/bin/env node`
in the generated javascript command-line interface `src/cli.bs.js`. Executing it
with `node src/cli.bs.js` might not work in some cases.

Note: You may also consider linking the binary using `npm link` in order to use
the `dem` command, just as would do a user after installing the package. To undo
this, use `npm unlink`.

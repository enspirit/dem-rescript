# Doc-e-mate, your document's best friend

This repository holds the `dem` commandline tool, that allows you to generate
and print documents written in Markdown, styled in CSS, with business data
injected from JSON or YAML files.

```
> Text     + Data  + Style  ->  Beautiful Business Documents
> HTML     + Json  + CSS    ->  Beautiful Business Documents
> Markdown + YAML  + SASS   ->  Beautiful Maintainable Business Document
```

## Install

Install the `dem` command line using `npm`, locally:

```
npm install @enspirit/dem-bs
```

Or globally using the `-g` flag.

## Examples

To get started, try:

```
dem --help
dem compile --help
dem print --help
```

Use the `compile` sub-command to generate an html document from markdown, css and
json sources in specific files:

```
dem compile --html-template examples/index.html.tpl -s examples/index.css  -t examples/index.md -d examples/index.json
```

Otherwise, skip lengthy parameters by following doc-e-mate conventions: current
folder having `index.md` (markdown text), `index.css` (style) and `index.json`
(data) files, as well as an `index.html.tpl` html final template:

```
dem compile
```

Use the `print` sub-command to generate your document and convert it into a
pdf document.

```
dem print
```

This command uses the WeasyPrint solution (see https://weasyprint.org/), so
make sure that it is correctly installed on your machine (the weasyprint command
must be available in a terminal).

By default, output files are named after the text source file. But you may
explicitly choose the name of the produced (html or pdf depending on the
sub-command) files by using the `--output` option.

```
dem compile -o my_document.html
dem print -o my_document.pdf
```

Also, edit your sources files and display the final result live by using
the `--watch` option.

```
dem compile --watch
dem print --watch
```

Doc-e-mate will watch for modifications of any source file located in the
directory of each specified (or default) source file, as well as their
subdirectories. Now, let's edit your sources in your favorite editor on the left
hand side of your screen, and get a live display of the final pdf document on
the right hand side of your screen.

Eventually, generate multiple data instanciations of the same document by using
the `--publipost` option and using an array of data instead of a single data
object, and instantiate the name of the produced document too with the same
mustache format as in the document.

```
dem print --publipost -d array_of_data_instances.json.yml -o my_report_{{id}}.pdf
```

### Mustache partials

Doc-e-mate supports mustache partials. Please take a look at Mustache partials documentation first: https://mustache.github.io/mustache.5.html.

`TL;DR`: you may include text from some separated markdown file called `white_hat.md` and located using the following syntax in your main text file: `{{> white_hat}}`. In this case, `white_hat.md` is located in the same directory as your main text file.

Also do not hesitate to use several levels of inclusion: `white_hat.md` may refer to another markdown file called `shield_of_light.md` located in a subdirectory `powers` with the following: `{{> powers/shield_of_light}}`. Just make sure to always use a path relative to the main text file.

### Build your data with javascript

Doc-e-mate supports data file written in CommonJS. They are loaded using the
NodeJS `require` directive. The only requirement is to export the data with the
following instruction: `module.exports = data;` at the end of the javascript
data file. You may use an asynchronized script which returns a javascript
promise, just do not forget the `--async` option then.

Check the examples directory of these repository for some javascript data
examples.

## Hacking

The `dem` command line is written in BuckleScript, wth great help from the
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

## Using the docker image

We provide a docker image that RUNs the `dem` command by default with
default index.md, index.css and index.json files.

```
docker run enspirit/dem
```

A typical use case is to transform a markdown text `index.md` to a beautiful
.html file. It's straightforward if you mount a docker volume:

```
docker run -v $PWD/index.md:/dem/index.md enspirit/dem
```

Another use case is to print the text as a .pdf file. A similar approach is
used. For now, since `dem print` requires using the `-o` option, it's slightly
more complicated though:

```
docker run -v $PWD/index.md:/dem/index.md -v $PWD/index.pdf:/dem/index.pdf enspirit/dem print -o index.pdf
```

In the use cases above, we have no data, and a default stylesheet
(https://github.com/xz/new.css) is applied by default, so that the resulting
document is great.

If you actually have all .md, .css, and .json files locally, then the following
scenario is probably closer to what you want:

```
docker run -v $PWD:/dem enspirit/dem
docker run -v $PWD:/dem enspirit/dem print -o index.pdf
```

# doc-e-mate in BuckleScript (for developpers only)
A command-line version of doc-e-mate written in BuckleScript using mustache and markdown-it JS libraries.

Once cloned, run the following command for initializing the dev environment:
`npm install`. It might be necessary to run `npm install bs-platform` first.

Then, you can build the program with `npm run build`.

You can run the tests using `npm run test`.

And run the program with `node src/cli.bs.js`.

Note: the `npm run build` command automatically prepends a node shebang
`#!/usr/bin/env node` in the generated javascript command-line interface
`src/cli.bs.js`. So executing it with `node src/cli.bs.js` might not work in
some cases.

You may link the binary using `npm link` in order to use the `dem` command
instead, just as would do a user after installing the package.

To undo this, just use `npm unlink`.

Examples:
- `dem --help`
- `dem compile --help`
- (from the root directory): `dem compile -t examples/index.html.tpl -s examples/index.css  -T examples/index.md -D examples/index.json`
- (from the `examples` directory):
  - `dem`
  - `dem compile`
  - `dem compile -T index.md`
  - `dem compile -D index.json`

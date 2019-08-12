# doc-e-mate in BuckleScript
A command-line version of doc-e-mate written in BuckleScript using mustache and markdown-it JS libraries.

Once cloned, run the following command for initializing the dev environment:
`npm install`. It might be necessary to run `npm install bs-platform` first.

Then, you can build the program with `npm build`.

And run it with `node src/main.bs.js`.

You may link the binary using `npm link` in order to use the `dem` command
instead, just as would do a user after installing the package.

To undo this, just use `npm unlink`.

Go in the `examples` directory and try it!

type t_compilation_template = string;
type t_style = string;
type t_text = string;
type t_data = Js.Json.t;

// Basic compilation function that applies mustache and then markdownIt to some given text and data (in json
// or yaml format).
let compile_body: (t_text, t_data) => string;

// Full compilation function that compiles some given style and compiled body using some given template.
let compile: (t_compilation_template, t_style, t_text, t_data) => string;

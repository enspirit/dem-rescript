type t_template = string;
type t_style = string;
type t_text = string;
type t_data = Js.Json.t;

// Basic compilation function that applies mustache and then markdownIt to some given text and data (in json
// or yaml format).
// All arguments are option types replaced with application default values when empty.
let compile_body: (option(t_text), option(t_data)) => string;

// Full compilation function that compiles some given style and compiled body using some given template.
// All arguments are option types replaced with application default values when empty.
let compile: (option(t_template), option(t_style), option(t_text), option(t_data)) => string;

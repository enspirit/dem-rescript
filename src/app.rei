type t_template = string;
type t_style = string;
type t_text = string;
type t_data = Js.Json.t;
type t_partials = Js.Dict.t(string);

// Instantiate function: applies mustache to some given text and data.
// All arguments are option types replaced with application default values
// when empty.
let instantiate_body: (option(t_text), option(t_data), option(t_partials)) => string;

// Full compilation function that compiles some given style and compiled body
// using some given template.
// All arguments are option types replaced with application default values
// when empty.
let compile: (option(t_template),
              option(t_style),
              option(t_text),
              option(t_data),
              option(t_partials)) => string;

// Scan the given string for call to mustache partials,
// i.e. for instructions of the form: `{{> [name]}}`,
// and return the list of partial names.
let partials_dependencies: string => list(string);

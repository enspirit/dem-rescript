type result =
  | Ok(Js.Promise.t(unit))
  | Error(string)
;

type filename_or_content('a) =
| Filename(string)
| Content('a)
;

type copts = {
  template_fco: option(filename_or_content(App.t_template)),
  style_fco: option(filename_or_content(App.t_style)),
  text_fco: option(filename_or_content(App.t_text)),
  data_fco: option(filename_or_content(App.t_data)),
  watch_mode: bool,
  base_url_opt: option(string),
  output_filename_opt: option(string),
  publipost: bool,
  async: bool
};

type t_instantiation_src = {
  text_opt: option(string),
  partials: Js.Dict.t(string),
  data_opt: option(Js.Json.t),
  expanded_base_url_opt: option(string),
  expanded_output_filename_opt: option(string)
};

type t_compilation_src = {
  text_opt: option(string),
  partials: Js.Dict.t(string),
  data_opt: option(Js.Json.t),
  template_opt: option(string),
  style_opt: option(string),
  expanded_base_url_opt: option(string),
  expanded_output_filename_opt: option(string)
};

type t_print_src = {
  html: string,
  expanded_base_url_opt: option(string),
  expanded_output_filename_opt: option(string)
};

let copts: (option(filename_or_content(App.t_template)),
            option(filename_or_content(App.t_style)),
            option(filename_or_content(App.t_text)),
            option(filename_or_content(App.t_data)),
            bool,
            option(string),
            option(string),
            bool,
            bool)
           => copts;

let instantiate: copts => result;
let compile: copts => result;
let print: copts => result;

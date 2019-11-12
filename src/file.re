open Sugar;

let robust_read = (what, filename) => {
  Logger.info({j|Reading $what from "$filename".|j});
  try (Some(Node.Fs.readFileAsUtf8Sync(filename))) {
  | Caml_js_exceptions.Error(e) => Logger.error @@ Logger.format_caml_js_exn(e); None
  };
}

let read_text = filename => {
  robust_read("text", filename);
}

let read_json_data = filename => {
  let content = robust_read("data", filename);
  switch (content) {
  | None => None
  | Some(data) => try (Some(Js.Json.parseExn(data))) {
    | e => Logger.error @@ Logger.format_exn(e); None
    };
  };
};

let read_yaml_data = filename => {
  let content = robust_read("data", filename);
  switch (content) {
  | None => None
  | Some(data) => try (Some(data -> Yaml.yamlParse())) {
    | e => Logger.error @@ Logger.format_exn(e); None
    };
  };
};

let read_js_data = filename => {
  try {
    Some(node_require(filename) |> objToJson);
  } {
  | e => Logger.error(Logger.format_exn(e)); None
  }
};

let extension = filename => {
  try {
    let ext_pos = String.rindex(filename, '.') + 1;
    let ext_size = String.length(filename) - ext_pos;
    Some(String.sub(filename, ext_pos, ext_size));
  } {
  | Not_found =>
    Logger.error({j|Extension delimiter not found in $filename|j}); None
  };
}

let read_data = data_filename_opt => {
  switch (data_filename_opt) {
  | None when Node.Fs.existsSync("index.json.yml") => read_yaml_data("index.json.yml");
  | None => read_json_data("index.json");
  | Some(data_filename) =>
    switch (extension(data_filename)) {
    | Some("yml")  => read_yaml_data(data_filename);
    | Some("json") => read_json_data(data_filename);
    | Some("js")   => read_js_data(data_filename);
    | _            =>
      Logger.error({j|Unsupported extension in $data_filename.|j}); None
    };
  };
};

let read_template = filename => {
  robust_read("template", filename);
}

let read_css_style = filename => {
  robust_read("style", filename);
};

let read_sass_style = filename => {
  Logger.info({j|Reading style from "$filename".|j});
  try {
    let css_buffer = NodeSass.renderSync(filename);
    Some(css_buffer |> Node.Buffer.toString)
  } {
  | e => Logger.error @@ Logger.format_exn(e); None
  }
};

let read_style = style_filename_opt => {
  switch (style_filename_opt) {
  | None when Node.Fs.existsSync("index.css.sass") => read_sass_style("index.css.sass");
  | None => read_css_style("index.css");
  | Some(style_filename) =>
    switch (extension(style_filename)) {
    | Some("sass") => read_sass_style(style_filename);
    | Some("css")  => read_css_style(style_filename);
    | _            =>
      Logger.error({j|Unsupported extension in $style_filename.|j}); None
    };
  };
};

let robust_path = (path: string) => {
  switch (Node.Fs.existsSync(path)) {
  | false => None
  | true => Some(path)
  }
}

let robust_dir = (path) => {
  try (Some(Node.Fs.readdirSync(path))) {
  | e => Logger.error @@ Logger.format_exn(e); None
  }
}

/* returns the path of the given file, relative to the given root */
let find = (~root:string=".", filepath: string) => {
  let robust_root = robust_path(root) || ".";
  let pattern = filepath ++ ".md";

  let root_pattern = Node.Path.format({ "dir": robust_root, "base": pattern, "name": "", "ext": "", "root": "" });
  switch (Node.Fs.existsSync(root_pattern)) {
  | true => Some(root_pattern)
  | false => None
  }
}

/* a partial is loaded currently identified
 * by its path relatively to the root path
 * (which is the path of the root text file) */
let rec build_partials = (~root:string=".", ~partials: Js.Dict.t(string)=Js.Dict.empty(), dependencies: list(string)) => {
  let root_dir = Node.Path.dirname(root);
  switch (dependencies) {
  | [] => partials
  | [key, ...rem_keys] when key == root => build_partials(~root, ~partials, rem_keys)
  | [key, ...rem_keys] when !has_key(partials, key) => // not in the dictionary yet
    let content = switch (find(~root=root_dir, key)) {
    | None => Logger.warn("Using empty string for partial '" ++ key ++ "'."); ""
    | Some(path) => switch (read_text(path)) {
      | None => Logger.warn("Using empty string for partial '" ++ key ++ "'."); ""
      | Some(c) => c
      };
    };
    let dependencies = App.partials_dependencies(content);
    Js.Dict.set(partials, key, content);
    build_partials(~root, ~partials, List.rev_append(rem_keys, dependencies));
  | [_, ...rem_keys] => build_partials(~root, ~partials, rem_keys)
  }
}

let robust_write = (what, filename, content) => {
  Logger.info({j|Writing $what in "$filename".|j});
  try (Node.Fs.writeFileAsUtf8Sync(filename, content)) {
  | Caml_js_exceptions.Error(e) => Logger.error @@ Logger.format_caml_js_exn(e); ()
  };
}

let write_html = (~output_filename=?, text_filename, html) => {
  let output_filename = output_filename || (Node.Path.basename_ext(text_filename, "md") ++ "html")
  robust_write("html", output_filename, html);
  output_filename;
};

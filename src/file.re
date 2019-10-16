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
  let content = robust_read("data", filename);
  switch (content) {
  | None => None
  | Some(data) => try (Some(JSInterpreter.eval(data))) {
    | e => Logger.error @@ Logger.format_exn(e); None
    };
  };
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

let read_compilation_template = filename => {
  robust_read("compilation template", filename);
}

let read_compilation_style = filename => {
  robust_read("compilation style", filename);
}

let rec build_partials = (~root:string=".", ~partials: Js.Dict.t(string)=Js.Dict.empty(), dependencies: list(string)) => {
  let root_dir = Node.Path.dirname(root);
  switch (dependencies) {
  | [] => partials
  | [key, ...rem_keys] when key == root => build_partials(~root, ~partials, rem_keys)
  | [key, ...rem_keys] when !has_key(partials, key) => // not in the dictionary yet
    let path = Node.Path.format({ "dir": root_dir, "name": key, "ext": ".md", "base": "", "root": "" });
    let content = switch (read_text(path)) {
    | None => Logger.warn("Using empty string for partial '" ++ key ++ "'."); ""
    | Some(t) => t
    };
    Js.Dict.set(partials, key, content);
    build_partials(~root, ~partials, rem_keys);
  | [_, ...rem_keys] => build_partials(~root, ~partials, rem_keys)
  }
}

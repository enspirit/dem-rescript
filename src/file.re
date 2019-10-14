let robust_read = (what, filename) => {
  // Pervasives.prerr_string({j|Reading $what from "$filename".|j});
  try (Some(Node.Fs.readFileAsUtf8Sync(filename))) {
  | Caml_js_exceptions.Error(e) =>
    // Pervasives.prerr_string({j|$e.|j});
    None
  };
}

let read_text = filename => {
  robust_read("text", filename);
}

let read_json_data = filename => {
  let content = robust_read("data", filename);
  switch (content) {
  | None => None
  | Some(data) => Some(Js.Json.parseExn(data));
  };
};

let read_yaml_data = filename => {
  let content = robust_read("data", filename);
  switch (content) {
  | None => None
  | Some(data) => Some(data -> Yaml.yamlParse());
  };
};

let read_js_data = filename => {
  let content = robust_read("data", filename);
  switch (content) {
  | None => None
  | Some(data) =>
    Some(JSInterpreter.eval(data));
  }
};

let extension = filename => {
  let ext_pos = String.rindex(filename, '.') + 1;
  let ext_size = String.length(filename) - ext_pos;
  String.sub(filename, ext_pos, ext_size);
}

let read_data = data_filename_opt => {
  switch (data_filename_opt) {
  | None when Node.Fs.existsSync("index.json.yml") => read_yaml_data("index.json.yml");
  | None => read_json_data("index.json");
  | Some(data_filename) =>
    switch (extension(data_filename)) {
    | "yml"  => read_yaml_data(data_filename);
    | "json" => read_json_data(data_filename);
    | "js"   => read_js_data(data_filename);
    | _ => None;
    };
  };
};

let read_compilation_template = filename => {
  robust_read("compilation template", filename);
}

let read_compilation_style = filename => {
  robust_read("compilation style", filename);
}

let version = () => {
  print_endline("Doc-e-mate in BuckleScript 0.1.0");
};

// Convert Json to Js Object \!/ without type safety \!/
external jsonToObj : Js.Json.t => Js.t({..}) = "%identity";

let markdownItInstance = MarkdownIt.createMarkdownIt();
let markdownIt = (text) => MarkdownIt.render(markdownItInstance, text);

let mustache = Mustache.render;

let text = Node.Fs.readFileAsUtf8Sync("index.md");

let read_json_data = () => {
  Node.Fs.readFileAsUtf8Sync("index.json")
  |> Js.Json.parseExn
}

let read_yaml_data = () => {
  Node.Fs.readFileAsUtf8Sync("index.json.yml")
  -> Yaml.yamlParse()
}

let read_data = () => {
  if (Node.Fs.existsSync("index.json.yml")) {
    read_yaml_data();
  } else {
    read_json_data();
  }
}

let data = read_data() |> jsonToObj;
let compiled_body = mustache(text, data) |> markdownIt;

let default = () => {
  Js.log(compiled_body);
}

let compile = () => {
  let compilation_template = Node.Fs.readFileAsUtf8Sync("index.html.tpl");
  let compiled_style = Node.Fs.readFileAsUtf8Sync("index.css");

  let res = mustache(compilation_template, {
    "compiled_style": compiled_style,
    "compiled_body": compiled_body
  })
  Js.log(res);
}

let spec = [
  ("-version", Arg.Unit(version), ""),
  ("-compile", Arg.Unit(compile), "")
];

let anonArg = _ => ();

let () = {
  switch (Arg.parse(spec, anonArg, "Usage: ")) {
  | exception (Arg.Bad(err)) =>
    prerr_endline(err);
    exit(1);
  | exception (Arg.Help(msg)) =>
    print_endline(msg);
    exit(0);
  | _ => default()
  };
};

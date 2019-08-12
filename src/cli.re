// This is required because Node.js doesn't follow the POSIX standard for argv.
%raw "process.argv.shift()";

let version = () => {
  print_endline("Doc-e-mate in BuckleScript 0.1.0");
};

/*** File readers *************************************************************/
let read_text_file = (filename) => Node.Fs.readFileAsUtf8Sync(filename);

let read_json_data_file = (filename) => {
  Node.Fs.readFileAsUtf8Sync(filename)
  |> Js.Json.parseExn
}

let read_yaml_data_file = (filename) => {
  Node.Fs.readFileAsUtf8Sync(filename)
  -> Yaml.yamlParse()
}

let read_data_file = (data_filename_opt) => {
  let json = switch(data_filename_opt) {
  | None =>
    if (Node.Fs.existsSync("index.json.yml")) {
      read_yaml_data_file("index.json.yml");
    } else {
      read_json_data_file("index.json");
    }
  | Some(data_filename) =>
    let last_point = Js.String2.lastIndexOf(".", data_filename);
    let file_ext = Js.String.sliceToEnd(last_point, data_filename);
    switch (file_ext) {
    | "yml" => read_yaml_data_file(data_filename)
    | "json" => read_json_data_file(data_filename)
    };
  };
  json;
}

/*** Commands *************************************************************/
let compiled_body = (text_filename, data_filename) => {
  App.compile_body(read_text_file(text_filename), read_data_file(None));
}

type verb =
  | Normal
  | Quiet
  | Verbose;

type copts = {
  debug: bool,
  verb,
  prehook: option(string)
};

let str = Printf.sprintf;

let opt_str = (sv) =>
  fun
  | None => "None"
  | Some(v) => str("Some(%s)", sv(v));

let opt_str_str = opt_str((s) => s);

let verb_str =
  fun
  | Normal => "normal"
  | Quiet => "quiet"
  | Verbose => "verbose";

let pr_copts = (oc, copts) =>
  Printf.fprintf(
    oc,
    "debug = %B\nverbosity = %s\nprehook = %s\n",
    copts.debug,
    verb_str(copts.verb),
    opt_str_str(copts.prehook)
  );

let default_compilation = (copts) => {
  `Ok(Js.log(compiled_body));
}

let compile = (copts, ctf, csf, text_filename, data_filename) => {
  let compilation_template = Node.Fs.readFileAsUtf8Sync(ctf);
  let compiled_style = Node.Fs.readFileAsUtf8Sync(csf);
  let text = read_text_file(text_filename);
  let data = read_data_file(data_filename);

  let res = App.compile(compilation_template, compiled_style, text, data)
  Js.log(res);
}

let help = (_, man_format, cmds, topic) =>
  switch topic {
  | None => `Help((`Pager, None)) /* help about the program. */
  | Some(topic) =>
    let topics = ["topics", "patterns", "environment", ...cmds];
    let (conv, _) = Cmdliner.Arg.enum(List.rev_map((s) => (s, s), topics));
    switch (conv(topic)) {
    | `Error(e) => `Error((false, e))
    | `Ok(t) when t == "topics" =>
      List.iter(print_endline, topics);
      `Ok();
    | `Ok(t) when List.mem(t, cmds) => `Help((man_format, Some(t)))
    | `Ok(_) =>
      let page = ((topic, 7, "", "", ""), [`S(topic), `P("Say something")]);
      `Ok(Cmdliner.Manpage.print(man_format, Format.std_formatter, page));
    };
  };

/* Help sections common to all commands */
let help_secs = [
  `S(Cmdliner.Manpage.s_common_options),
  `P("These options are common to all commands."),
  `S("MORE HELP"),
  `P("Use `$(mname) $(i,COMMAND) --help' for help on a single command."),
  `Noblank,
  `P("Use `$(mname) help patterns' for help on patch matching."),
  `Noblank,
  `P("Use `$(mname) help environment' for help on environment variables."),
  `S(Cmdliner.Manpage.s_bugs),
  `P("Please report bugs at https://enspirit.be")
];

/* Options common to all commands */
let copts = (debug, verb, prehook) => {debug, verb, prehook};

let copts_t = {
  let docs = Cmdliner.Manpage.s_common_options;
  let debug = {
    let doc = "Give only debug output.";
    Cmdliner.Arg.(value & flag & info(["debug"], ~docs, ~doc));
  };
  let verb = {
    let doc = "Suppress informational output.";
    let quiet = (Quiet, Cmdliner.Arg.info(["q", "quiet"], ~docs, ~doc));
    let doc = "Give verbose output.";
    let verbose = (Verbose, Cmdliner.Arg.info(["v", "verbose"], ~docs, ~doc));
    Cmdliner.Arg.(last & vflag_all([Normal], [quiet, verbose]));
  };
  let prehook = {
    let doc = "Specify command to run before this $(mname) command.";
    Cmdliner.Arg.(value & opt(some(string), None) & info(["prehook"], ~docs, ~doc));
  };
  Cmdliner.Term.(const(copts) $ debug $ verb $ prehook);
};

/* Commands */
let compile_cmd = {
  let ctf = {
    let doc = "Filename of the compilation template.";
    let docv = "COMPILATION_TEMPLATE_FILENAME";
    Cmdliner.Arg.(value & opt(string, "index.html.tpl") & info(["ctf", "compilation-template-filename"], ~docv, ~doc));
  };
  let csf = {
    let doc = "Filename of the compilation style.";
    let docv = "COMPILATION_STYLE_FILENAME";
    Cmdliner.Arg.(value & opt(string, "index.css") & info(["csf", "compilation-style-filename"], ~docv, ~doc));
  };
  let text_filename = {
    let doc = "Text filename.";
    let docv = "TEXT_FILENAME";
    Cmdliner.Arg.(value & opt(string, "index.md") & info(["T", "text-filename"], ~docv, ~doc));
  };
  let data_filename = {
    let doc = "Data filename.";
    let docv = "DATA_FILENAME";
    Cmdliner.Arg.(value & opt(some(string), None) & info(["D", "data-filename"], ~docv, ~doc));
  };
  let doc = "compiles files in current directory";
  let exits = Cmdliner.Term.default_exits;
  let man = [
    `S(Cmdliner.Manpage.s_description),
    `P(
      "Compiles files ..."
    ),
    `Blocks(help_secs)
  ];
  (
    Cmdliner.Term.(const(compile) $ copts_t $ ctf $ csf $ text_filename $ data_filename),
    Cmdliner.Term.info("compile", ~doc, ~sdocs=Cmdliner.Manpage.s_common_options, ~exits, ~man)
  );
};

let help_cmd = {
  let topic = {
    let doc = "The topic to get help on. `topics' lists the topics.";
    Cmdliner.Arg.(value & pos(0, some(string), None) & info([], ~docv="TOPIC", ~doc));
  };
  let doc = "display help about doc-e-mate and its commands";
  let man = [
    `S(Cmdliner.Manpage.s_description),
    `P("Prints help about doc-e-mate commands and other subjects..."),
    `Blocks(help_secs)
  ];
  (
    Cmdliner.Term.(ret(const(help) $ copts_t $ Cmdliner.Arg.man_format $ Cmdliner.Term.choice_names $ topic)),
    Cmdliner.Term.info("help", ~doc, ~exits=Cmdliner.Term.default_exits, ~man)
  );
};

let default_cmd = {
  let doc = "dem compiles text and data of current directory.";
  let sdocs = Cmdliner.Manpage.s_common_options;
  let exits = Cmdliner.Term.default_exits;
  let man = [
    `S(Cmdliner.Manpage.s_description),
    `P(
      "Automatically mustache and markdown the text and data \n       files in the current directory."
    ),
    `Blocks(help_secs)
  ];
  (
    Cmdliner.Term.(ret(const(default_compilation) $ copts_t)),
    Cmdliner.Term.info("dem", ~doc, ~sdocs, ~exits, ~man)
  );
};

let cmds = [help_cmd, compile_cmd]; //version_cmd
let () = Cmdliner.Term.(exit @@ eval_choice(default_cmd, cmds));

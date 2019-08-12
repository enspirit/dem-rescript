// This is required because Node.js doesn't follow the POSIX standard for argv.
%raw "process.argv.shift()";

let version = () => {
  print_endline("Doc-e-mate in BuckleScript 0.1.0");
};

// Convert Json to Js Object \!/ without type safety \!/
external jsonToObj : Js.Json.t => Js.t({..}) = "%identity";

let markdownItInstance = MarkdownIt.createMarkdownIt();
let markdownIt = (text) => MarkdownIt.render(markdownItInstance, text);

let mustache = Mustache.render;

/*** File readers *************************************************************/
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

/*** Commands *************************************************************/
let compiled_body = mustache(text, data) |> markdownIt;

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

let compile = (copts) => {
  let compilation_template = Node.Fs.readFileAsUtf8Sync("index.html.tpl");
  let compiled_style = Node.Fs.readFileAsUtf8Sync("index.css");

  let res = mustache(compilation_template, {
    "compiled_style": compiled_style,
    "compiled_body": compiled_body
  })
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
    Cmdliner.Term.(const(compile) $ copts_t),
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

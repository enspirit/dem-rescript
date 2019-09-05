// This is required because Node.js doesn't follow the POSIX standard for argv.
%raw "process.argv.shift()";

let version = () => {
  print_endline("Doc-e-mate in BuckleScript 0.1.0");
};

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

let opt_str = sv =>
  fun
  | None => "None"
  | Some(v) => str("Some(%s)", sv(v));

let opt_str_str = opt_str(s => s);

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

let default_compilation = _ => {
  let text_opt = File.read_text("index.md");
  let data_opt = File.read_data(None);
  let compiled_body = App.compile_body(text_opt, data_opt);
  `Ok(Js.log(compiled_body));
};

let compile = (_, ctf_opt, csf_opt, text_filename_opt, data_filename_opt) => {
  let template_opt = File.read_compilation_template(ctf_opt);
  let style_opt = File.read_compilation_style(csf_opt);
  let text_opt = File.read_text(text_filename_opt);
  let data_opt = File.read_data(data_filename_opt);
  let res = App.compile(template_opt, style_opt, text_opt, data_opt);
  `Ok(Js.log(res));
};

let help = (_, man_format, cmds, topic) =>
  switch (topic) {
  | None => `Help((`Pager, None)) /* help about the program. */
  | Some(topic) =>
    let topics = ["topics", "patterns", "environment", ...cmds];
    let (conv, _) = Cmdliner.Arg.enum(List.rev_map(s => (s, s), topics));
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

/* Commands in cmdliner format */
let compile_cmd = {
  let ctf = {
    let doc = "Filename of the compilation template.";
    let docv = "COMPILATION_TEMPLATE_FILENAME";
    Cmdliner.Arg.(
      value
      & opt(string, "index.html.tpl")
      & info(["t", "compilation-template-filename"], ~docv, ~doc)
    );
  };
  let csf = {
    let doc = "Filename of the compilation style.";
    let docv = "COMPILATION_STYLE_FILENAME";
    Cmdliner.Arg.(
      value
      & opt(string, "index.css")
      & info(["s", "compilation-style-filename"], ~docv, ~doc)
    );
  };
  let text_filename = {
    let doc = "Text filename.";
    let docv = "TEXT_FILENAME";
    Cmdliner.Arg.(
      value
      & opt(string, "index.md")
      & info(["T", "text-filename"], ~docv, ~doc)
    );
  };
  let data_filename = {
    let doc = "Data filename.";
    let docv = "DATA_FILENAME";
    Cmdliner.Arg.(
      value
      & opt(some(string), None)
      & info(["D", "data-filename"], ~docv, ~doc)
    );
  };
  let doc = "compiles files in current directory";
  let sdocs = Cmdliner.Manpage.s_common_options;
  let exits = Cmdliner.Term.default_exits;
  let man = [
    `S(Cmdliner.Manpage.s_description),
    `P("Compiles files ..."),
    `Blocks(help_secs)
  ];
  (
    Cmdliner.Term.(ret(const(compile) $ copts_t $ ctf $ csf $ text_filename $ data_filename)),
    Cmdliner.Term.info("compile", ~doc, ~sdocs, ~exits, ~man)
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
      "Automatically mustache and markdown the text and data \n
       files in the current directory. \n
       Text file: index.md \n
       Data file: index.json.yml if it exists, \n
       otherwise: index.json "
    ),
    `Blocks(help_secs)
  ];
  (
    Cmdliner.Term.(ret(const(default_compilation) $ copts_t)),
    Cmdliner.Term.info("dem", ~doc, ~sdocs, ~exits, ~man)
  );
};

// Available legal commands.
let cmds = [help_cmd, compile_cmd]; //version_cmd

// Execute default command if no argument given to default_cmd
// Otherwise, execute the specified command.
let () = Cmdliner.Term.(exit @@ eval_choice(default_cmd, cmds));

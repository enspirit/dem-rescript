// This is required because Node.js doesn't follow the POSIX standard for argv.
%raw "process.argv.shift()";

let version = "0.5.0";

open Sugar;

let close = () => {
  Logger.save();
};

type copts = {
  ctf: string,
  csf: string,
  text_filename: string,
  data_filename: option(string)
};

/****************************************************************************
 * Functions called by CLI commands and connected to the app API
 */
let default_compilation = _ => {
  try {
    let text_filename = "index.md";
    let text_opt = File.read_text(text_filename);
    let data_opt = File.read_data(None);
    let root_partials_dep = App.partials_dependencies(text_opt || "")
    let partials = File.build_partials(~root=text_filename, root_partials_dep);
    let compiled_body = App.compile_body(text_opt, data_opt, Some(partials));
    close();
    `Ok(Js.log(compiled_body));
  } {
  | e => `Error(false, Logger.format_exn(e));
  }
};

let compile_ = (ctf, csf, text_filename, data_filename_opt) => {
  let template_opt = File.read_compilation_template(ctf);
  let style_opt = File.read_compilation_style(csf);
  let text_opt = File.read_text(text_filename);
  let data_opt = File.read_data(data_filename_opt);
  let root_partials_dep = App.partials_dependencies(text_opt || "");
  let partials = File.build_partials(~root=text_filename, root_partials_dep);
  App.compile(template_opt, style_opt, text_opt, data_opt, Some(partials));
}

let compile = (copts) => {
  try {
    let res = compile_(copts.ctf, copts.csf, copts.text_filename, copts.data_filename);
    close();
    `Ok(Js.log(res));
  } {
  | e => `Error(false, Logger.format_exn(e));
  }
};

let print = (copts, output_filename_opt) => {
  try {
    let html = compile_(copts.ctf, copts.csf, copts.text_filename, copts.data_filename);
    let html_filename = File.write_html(copts.text_filename, html);
    Weasyprint.print(html_filename, output_filename_opt);
    close();
    `Ok(());
  } {
  | e => `Error(false, Logger.format_exn(e));
  }
}

/****************************************************************************
 * Functions called by CLI commands that are implemented on the CLI side
 */
let copts = (ctf, csf, text_filename, data_filename) => {ctf, csf, text_filename, data_filename};

/* Options common to all commands */
let copts_t = {
  let docs = Cmdliner.Manpage.s_common_options;
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
  Cmdliner.Term.(const(copts) $ ctf $ csf $ text_filename $ data_filename);
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
  `P("Please report bugs at https://github.com/enspirit/dem-bs")
];

/****************************************************************************
 * CLI Command functions in cmdliner format
 */
let compile_cmd = {
  let doc = "compiles files in current directory";
  let sdocs = Cmdliner.Manpage.s_common_options;
  let exits = Cmdliner.Term.default_exits;
  let man = [
    `S(Cmdliner.Manpage.s_description),
    `P("Generates and prints documents written in Markdown, styled in CSS, with business data injected from
        JSON or YAML files. By default, compiles text, data, template and style files in the current directory.
        Text file: specified one or index.md if it exists, or empty content otherwise.
        Data file: specified one or index.json.yml if it exists, or index.json if it exists,
                   or empty data otherwise.
        Template file: specified one or index.html.tpl if it exists, or a very basic html structure otherwise.
        Style file: specified one or index.css if it exists, or empty style otherwise."),
    `Blocks(help_secs)
  ];
  (
    Cmdliner.Term.(ret(const(compile) $ copts_t)),
    Cmdliner.Term.info("compile", ~doc, ~sdocs, ~exits, ~man)
  );
};

let print_cmd = {
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
 let output_filename = {
   let doc = "Output filename.";
   let docv = "OUTPUT_FILENAME";
   Cmdliner.Arg.(
     value
     & opt(some(string), None)
     & info(["O", "output-filename"], ~docv, ~doc)
   );
 };
 let doc = "print a pdf document after compiling files in current directory";
 let sdocs = Cmdliner.Manpage.s_common_options;
 let exits = Cmdliner.Term.default_exits;
 let man = [
   `S(Cmdliner.Manpage.s_description),
   `P("Prints a PDF document after compiling a doc-e-mate source written with Markdown, styled in CSS, with
       business data injected from JSON or YAML files. By default, compiles text, data, template and style
       files in the current directory.
       Text file: specified one or index.md if it exists, or empty content otherwise.
       Data file: specified one or index.json.yml if it exists, or index.json if it exists,
                  or empty data otherwise.
       Template file: specified one or index.html.tpl if it exists, or a very basic html structure otherwise.
       Style file: specified one or index.css if it exists, or empty style otherwise."),
   `Blocks(help_secs)
 ];
 (
   Cmdliner.Term.(ret(const(print) $ copts_t $ output_filename)),
   Cmdliner.Term.info("print", ~doc, ~sdocs, ~exits, ~man)
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
  let doc = "Doc-e-mate - your document's best friend.";
  let sdocs = Cmdliner.Manpage.s_common_options;
  let exits = Cmdliner.Term.default_exits;
  let man = [
    `S(Cmdliner.Manpage.s_description),
    `P(
      "Generate and print documents written in Markdown, styled in CSS, with business data
       injected from JSON or YAML files. By default, when no command is specified,
       the compile command is called."
    ),
    `Blocks(help_secs)
  ];
  (
    Cmdliner.Term.(ret(const(compile) $ copts_t)),
    Cmdliner.Term.info("dem", ~version=version, ~doc, ~sdocs, ~exits, ~man)
  );
};

// Available legal commands.
let cmds = [help_cmd, compile_cmd, print_cmd];

// Execute default command if no argument given
// Otherwise, execute the specified command.
let () = Cmdliner.Term.(exit @@ eval_choice(default_cmd, cmds));

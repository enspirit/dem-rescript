// This is required because Node.js doesn't follow the POSIX standard for argv.
%raw "process.argv.shift()";

let version = "0.15.1";

open Sugar;

/* Convert from File system interface (Fsi) result to Cmdliner result type */
let copts_t_of_fsi_result = (f) => (copts) => switch (f(copts)) {
| Fsi.Ok(p, _) => Logger.print_stderr(); `Ok(p |> then_resolve(ignore))
| Fsi.Error(m) => `Error(false, m)
};

let copts_t_of_fsi_print_result = (f) => (copts, pipe) => switch (f(copts, pipe), pipe) {
| (Fsi.Ok(p, _), true) => Logger.print_stderr(); `Ok(p |> then_resolve(a => Js.log(a[0])));
| (Fsi.Ok(p, _), false) => Logger.print_stderr(); `Ok(p |> then_resolve(ignore));
| (Fsi.Error(m), _) => `Error(false, m);
};

let fco_of_option_filename = o => {
  o -> Belt.Option.map(f => Fsi.Filename(f))
};

let copts = (template_filename_opt, style_filename_opt, text_filename_opt, data_filename_opt, watch_mode, base_url_opt, output_filename_opt, publipost, async) => {
  let tpl_fco = template_filename_opt |> fco_of_option_filename;
  let style_fco = style_filename_opt |> fco_of_option_filename;
  let text_fco = text_filename_opt |> fco_of_option_filename;
  let data_fco = data_filename_opt |> fco_of_option_filename;
  Fsi.copts(tpl_fco, style_fco, text_fco, data_fco, watch_mode, base_url_opt, output_filename_opt, publipost, async);
};

/* Options common to all commands */
let copts_t = {
  let _ = Cmdliner.Manpage.s_common_options;
  let template_filename = {
    let docv = "FILE";
    let doc = "Compile document using HTML template specified in $(docv). If $(docv) does not exist, nor its
      default implicit replacement, a very basic HTML structure will be used instead.";
    Cmdliner.Arg.(
      value
      & opt(some(string), Some("index.html.tpl"))
      & info(["h", "html-template"], ~docv, ~doc)
    );
  };
  let style_filename = {
    let docv = "FILE";
    let doc = "Style document as specified in $(docv). If $(docv) does not exist, nor its default implicit
      replacements, an empty style will be used instead.";
    Cmdliner.Arg.(
      value
      & opt(some(string), None)
      & info(["s", "style"], ~docv, ~doc)
    );
  };
  let text_filename = {
    let docv = "FILE";
    let doc = "Get document text from $(docv). If $(docv) does not exist, nor its default implicit
      replacement, an empty content will be used instead.";
    Cmdliner.Arg.(
      value
      & opt(some(string), Some("index.md"))
      & info(["t", "text"], ~docv, ~doc)
    );
  };
  let data_filename = {
    let docv = "FILE";
    let doc = "Get document data from $(docv). If $(docv) does not exist, nor its default implicit
      replacements, an empty data will be used instead.";
    Cmdliner.Arg.(
      value
      & opt(some(string), None)
      & info(["d", "data"], ~docv, ~doc)
    );
  };
  let watch_mode = {
    let doc = "Enable watch mode. Watches for modification of source files - which are files with the following
      extensions: md, json, yml, js, css, tpl - located in directories of argument source files, and their
      subdirectories. When a modification is detected, the dem command is run again.
      If no source file is indicated, and no default file can be found, the watch mode is not enabled at all.";
    Cmdliner.Arg.(
      value
      & flag
      & info(["w", "watch"], ~doc)
    );
  };
  let base_url = {
    let docv = "URL";
    let doc = "Set the base for relative URLs in the text source (and HTML output as well). This is exactly
      the same option as in weasyprint.";
    Cmdliner.Arg.(
      value
      & opt(some(string), None)
      & info(["u", "base-url"], ~docv, ~doc)
    );
  };
  let output_filename = {
    let docv = "FILE";
    let doc = "Save document in $(docv). When not used, name the document after the text source file.";
    Cmdliner.Arg.(
      value
      & opt(~vopt=Some("index.html"), some(string), None)
      & info(["o", "output"], ~docv, ~doc)
    );
  };
  let publipost = {
    let doc = "Enable publiposting. Compile or print multiple instances of the same document using as many
      dataset as necessary. In this case, a javascript data file must be given as parameter which exports an
      array containing the data objects. A document will be generated for each object."
    Cmdliner.Arg.(
      value
      & flag
      & info(["publipost"], ~doc)
    );
  };
  let async = {
    let doc = "Allow asynchronized data source. When enabled, allow using some asynchronized source of data.
      Obviously, this has an effect when using a javascript data file only. Also, do not use this option if
      your javascript data file does not export a promise, doc-e-mate would fail in that case."
    Cmdliner.Arg.(
      value
      & flag
      & info(["async"], ~doc)
    );
  };
  Cmdliner.Term.(const(copts)
  $ template_filename
  $ style_filename
  $ text_filename
  $ data_filename
  $ watch_mode
  $ base_url
  $ output_filename
  $ publipost
  $ async);
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
      `Ok(ignore() |> promise);
    | `Ok(t) when List.mem(t, cmds) => `Help((man_format, Some(t)))
    | `Ok(_) =>
      let page = ((topic, 7, "", "", ""), [`S(topic), `P("Say something")]);
      `Ok(Cmdliner.Manpage.print(man_format, Format.std_formatter, page) |> promise);
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
let instantiate_cmd = {
 let doc = "instantiates markdown files with data in current directory";
 let sdocs = Cmdliner.Manpage.s_common_options;
 let exits = Cmdliner.Term.default_exits;
 let man = [
   `S(Cmdliner.Manpage.s_description),
   `P("Instantiates documents written in Markdown with business data injected from
       JSON or YAML files, i.e. produces a new Markdown file without mustache variables. By default,
       instantiates files in the current directory."),
   `Blocks(help_secs)
 ];
 (
   Cmdliner.Term.(ret(const(Fsi.instantiate |> copts_t_of_fsi_result) $ copts_t)),
   Cmdliner.Term.info("instantiate", ~doc, ~sdocs, ~exits, ~man)
 );
};

let compile_cmd = {
  let doc = "compiles files in current directory";
  let sdocs = Cmdliner.Manpage.s_common_options;
  let exits = Cmdliner.Term.default_exits;
  let man = [
    `S(Cmdliner.Manpage.s_description),
    `P("Compiles documents written in Markdown, styled in CSS, with business data injected from
        JSON or YAML files. By default, compiles text, data, template and style files in the current
        directory."),
    `Blocks(help_secs)
  ];
  (
    Cmdliner.Term.(ret(const(Fsi.compile |> copts_t_of_fsi_result) $ copts_t)),
    Cmdliner.Term.info("compile", ~doc, ~sdocs, ~exits, ~man)
  );
};

let print_cmd = {
  let pipe = {
    let doc = "Prevent from writing result into file. Instead, pipes to standard output when enabled."
    Cmdliner.Arg.(
      value
      & flag
      & info(["pipe"], ~doc)
    );
  };
  let doc = "print a pdf document after compiling files in current directory";
  let sdocs = Cmdliner.Manpage.s_common_options;
  let exits = Cmdliner.Term.default_exits;
  let man = [
   `S(Cmdliner.Manpage.s_description),
   `P("Prints a PDF document after compiling a doc-e-mate source written with Markdown, styled in CSS, with
       business data injected from JSON or YAML files. By default, compiles text, data, template and style
       files in the current directory."),
   `Blocks(help_secs)
  ];
  (
   Cmdliner.Term.(ret(const(Fsi.print |> copts_t_of_fsi_print_result) $ copts_t $ pipe)),
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
    Cmdliner.Term.(ret(const(Fsi.compile |> copts_t_of_fsi_result) $ copts_t)),
    Cmdliner.Term.info("dem", ~version=version, ~doc, ~sdocs, ~exits, ~man)
  );
};

// Available legal commands.
let cmds = [help_cmd, instantiate_cmd, compile_cmd, print_cmd];

// Execute default command if no argument given
// Otherwise, execute the specified command.
let _ = switch (Cmdliner.Term.eval_choice(default_cmd, cmds)) {
| `Help | `Version => ignore() |> promise
| `Error(_) => exit(1) |> promise
| `Ok r => r |> then_resolve(ignore)
};

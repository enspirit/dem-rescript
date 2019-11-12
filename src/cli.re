// This is required because Node.js doesn't follow the POSIX standard for argv.
%raw "process.argv.shift()";

let version = "0.8.0";

open Sugar;

let close = () => {
  Logger.save();
};

// common options type
type copts = {
  template_filename: string,
  style_filename_opt: option(string),
  text_filename: string,
  data_filename_opt: option(string),
  watch_mode: bool,
  output_filename_opt: option(string)
};

type t_src = {
  template_opt: option(string),
  style_opt: option(string),
  text_opt: option(string),
  data_opt: option(Js.Json.t),
  html: string
};

/****************************************************************************
 * Functions called by CLI commands and connected to the app API
 */
let write_or_print_html = (output_filename_opt, text_filename, html) => {
  switch(output_filename_opt) {
  | None => Js.log(html);
  | Some(output_filename) => ignore(File.write_html(~output_filename, text_filename, html));
  }
}

let read_and_compile_all = (copts) => {
  let template_opt = File.read_template(copts.template_filename);
  let style_opt = File.read_style(copts.style_filename_opt);
  let text_opt = File.read_text(copts.text_filename);
  let data_opt = File.read_data(copts.data_filename_opt);
  let root_partials_dep = App.partials_dependencies(text_opt || "");
  let partials = File.build_partials(~root=copts.text_filename, root_partials_dep);
  let html = App.compile(template_opt, style_opt, text_opt, data_opt, Some(partials));
  { template_opt, style_opt, text_opt, data_opt, html };
}

let watch_directory_rec = (directory, callback) => {
  let watcher = Chokidar.watch(directory, ());
  let handler = (path: string) => {
    let ext = File.extension(path);
    switch ext {
    | Some("md") | Some("json") | Some("yml") | Some("js") | Some("css") | Some("sass") | Some("tpl") =>
      callback();
    | _ => ();
    };
  };
  let _ = Chokidar.on(watcher, "change", handler);
};

let directories = (copts) => {
  // there is no element in l that is a leading substring of s1 except s1 itself
  let noDistinctSubstring = (l, s1) => {
    !Belt.List.some(l, s2 => s2 != s1 && Js.String.startsWith(s2, s1))
  };
  [
    Some(copts.template_filename),
    copts.style_filename_opt,
    Some(copts.text_filename),
    copts.data_filename_opt,
    copts.output_filename_opt
  ]
  -> Belt.List.map((f) => {
    switch (f) {
    | Some(f) when Node.Fs.existsSync(f) =>
      let root_dir = Node.Path.dirname(f);
      let absolute = Node.Path.resolve(root_dir, "");
      Some(absolute)
    | _ => None
    }
  })
  -> Belt.List.keepMap(x => x)
  |> List.sort_uniq(Pervasives.compare)
  |> (l => Belt.List.keep(l, noDistinctSubstring(l)))
};

let read_compile_and_print = (copts, print) => {
  let do_it = () => {
    let src = read_and_compile_all(copts);
    print(copts, src);
  };
  do_it();
  if (copts.watch_mode) {
    List.iter((f) => watch_directory_rec(f, do_it), directories(copts));
  }
}

let compile = (copts) => {
  try {
    read_compile_and_print(copts, fun (copts, src) => {
      write_or_print_html(copts.output_filename_opt, copts.text_filename, src.html);
    });
    close();
    `Ok();
  } {
  | e => `Error(false, Logger.format_exn(e));
  }
};

let print = (copts) => {
  try {
    read_compile_and_print(copts, fun (copts, src) => {
      let html_filename = File.write_html(copts.text_filename, src.html);
      Weasyprint.print(html_filename, copts.output_filename_opt);
    });
    close();
    `Ok(());
  } {
  | e => `Error(false, Logger.format_exn(e));
  }
}

/****************************************************************************
 * Functions called by CLI commands that are implemented on the CLI side
 */
let copts = (template_filename, style_filename_opt, text_filename, data_filename_opt, watch_mode, output_filename_opt) => {
  template_filename,
  style_filename_opt,
  text_filename,
  data_filename_opt,
  watch_mode,
  output_filename_opt
};

/* Options common to all commands */
let copts_t = {
  let docs = Cmdliner.Manpage.s_common_options;
  let template_filename = {
    let docv = "FILE";
    let doc = "Compile document using HTML template specified in $(docv). If $(docv) does not exist, nor its
      default implicit replacement, a very basic HTML structure will be used instead.";
    Cmdliner.Arg.(
      value
      & opt(string, "index.html.tpl")
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
      & opt(string, "index.md")
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
  let output_filename = {
    let docv = "FILE";
    let doc = "Save document in $(docv). When not used, name the document after the text source file.";
    Cmdliner.Arg.(
      value
      & opt(~vopt=Some("index.html"), some(string), None)
      & info(["o", "output"], ~docv, ~doc)
    );
  };
  Cmdliner.Term.(const(copts) $ template_filename $ style_filename $ text_filename $ data_filename $ watch_mode $ output_filename);
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
    `P("Compiles documents written in Markdown, styled in CSS, with business data injected from
        JSON or YAML files. By default, compiles text, data, template and style files in the current
        directory."),
    `Blocks(help_secs)
  ];
  (
    Cmdliner.Term.(ret(const(compile) $ copts_t)),
    Cmdliner.Term.info("compile", ~doc, ~sdocs, ~exits, ~man)
  );
};

let print_cmd = {
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
   Cmdliner.Term.(ret(const(print) $ copts_t)),
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
let () = switch (Cmdliner.Term.eval_choice(default_cmd, cmds)) {
| `Error _ => exit(1)
| _ => ()
};

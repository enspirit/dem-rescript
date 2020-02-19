// This is required because Node.js doesn't follow the POSIX standard for argv.
%raw "process.argv.shift()";

let version = "0.13.0";

open Sugar;

// common options type
type copts = {
  template_filename: string,
  style_filename_opt: option(string),
  text_filename: string,
  data_filename_opt: option(string),
  watch_mode: bool,
  base_url_opt: option(string),
  output_filename_opt: option(string),
  publipost: bool,
  async: bool
};

type t_instantiation_src = {
  text_opt: option(string),
  partials: Js.Dict.t(string),
  data_opt: option(Js.Json.t),
  expanded_base_url_opt: option(string),
  expanded_output_filename_opt: option(string)
};

type t_compilation_src = {
  text_opt: option(string),
  partials: Js.Dict.t(string),
  data_opt: option(Js.Json.t),
  template_opt: option(string),
  style_opt: option(string),
  expanded_base_url_opt: option(string),
  expanded_output_filename_opt: option(string)
};

type t_print_src = {
  html: string,
  expanded_base_url_opt: option(string),
  expanded_output_filename_opt: option(string)
};

/****************************************************************************
 * Functions called by CLI commands and connected to the app API
 */
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

let directories = copts => {
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
  -> Belt.List.map(f => {
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

let load_instantiation_sources = (copts, data_opt) => {
  let text_opt = File.read_text(copts.text_filename);
  let root_partials_dep = App.partials_dependencies(text_opt || "");
  let partials = File.build_partials(~root=copts.text_filename, root_partials_dep);
  let expanded_base_url_opt = File.expand(data_opt, copts.base_url_opt);
  let expanded_output_filename_opt = File.expand(data_opt, copts.output_filename_opt);
  { text_opt, partials, data_opt, expanded_base_url_opt, expanded_output_filename_opt }
};

let load_compilation_sources = (copts, data_opt) => {
  let i_src = load_instantiation_sources(copts, data_opt);
  let template_opt = File.read_template(copts.template_filename);
  let style_opt = File.read_style(copts.style_filename_opt);
  {
    text_opt:i_src.text_opt,
    partials:i_src.partials,
    data_opt,
    template_opt,
    style_opt,
    expanded_base_url_opt:i_src.expanded_base_url_opt,
    expanded_output_filename_opt:i_src.expanded_output_filename_opt
  };
};

let execute = (~copts, ~read:((copts, 'a) => 'b), ~transform:((copts, 'b) => 'c), ~print:((copts, 'c) => unit)) => {
  let do_it = () => {
    if (copts.publipost && copts.data_filename_opt != None) {
      File.read_data(~batch=true, ~async=copts.async, copts.data_filename_opt)
      |> then_resolve(data => {
        data
        |> List.map(data => {
          read(copts, Some(data))
          |> transform(copts, _)
          |> print(copts, _);
        })
        |> Belt.List.toArray;
      });
    } else {
      File.read_single_data(copts.data_filename_opt)
      |> then_resolve(data => {
        [|read(copts, data)
        |> transform(copts, _)
        |> print(copts, _)|];
      });
    }
  };

  try {
    let result_promise = do_it() |> then_resolve(_ => {
      if (copts.watch_mode) {
        directories(copts) |> List.iter(d => watch_directory_rec(d, ignore_promise(do_it)));
      };
      Logger.print_stderr();
    });
    `Ok(result_promise);
  } {
  | e => `Error(false, Logger.format_exn(e));
  }
};

let instantiate = (copts) => {
  let read = load_instantiation_sources;
  let transform = (_, src:t_instantiation_src) => {
    let markdown = App.instantiate_body(src.text_opt, src.data_opt, Some(src.partials));
    {
      text_opt:Some(markdown),
      partials:src.partials,
      data_opt:src.data_opt,
      template_opt:None,
      style_opt:None,
      expanded_base_url_opt:src.expanded_base_url_opt,
      expanded_output_filename_opt:src.expanded_output_filename_opt
    };
  };
  let print = (copts, src:t_compilation_src) => {
    let output_filename_opt = src.expanded_output_filename_opt;
    let Some(markdown) = src.text_opt;
    switch(output_filename_opt) {
    | None => Js.log(markdown);
    | Some(_) => ignore(File.write_md(~output_filename_opt, copts.text_filename, markdown));
    };
  };
  execute(~copts, ~read, ~transform, ~print);
};

let compile = (copts) => {
  let read = load_compilation_sources;
  let transform = (_, src:t_compilation_src) => {
    let html = App.compile(src.template_opt, src.style_opt, src.text_opt, src.data_opt, Some(src.partials));
    {
      html,
      expanded_base_url_opt:src.expanded_base_url_opt,
      expanded_output_filename_opt:src.expanded_output_filename_opt
    };
  };
  let print = (copts, src:t_print_src) => {
    let output_filename_opt = src.expanded_output_filename_opt;
    switch(output_filename_opt) {
    | None => Js.log(src.html);
    | Some(_) => ignore(File.write_html(~output_filename_opt, copts.text_filename, src.html));
    }
  };
  execute(~copts, ~read, ~transform, ~print);
};

let print = (copts) => {
  let read = load_compilation_sources;
  let transform = (_, src:t_compilation_src) => {
    let html = App.compile(src.template_opt, src.style_opt, src.text_opt, src.data_opt, Some(src.partials));
    {
      html,
      expanded_base_url_opt:src.expanded_base_url_opt,
      expanded_output_filename_opt:src.expanded_output_filename_opt
    };
  };
  let print = (copts, src:t_print_src) => {
    let html_filename = File.write_html(~output_filename_opt=src.expanded_output_filename_opt, copts.text_filename, src.html);
    Weasyprint.print(html_filename, src.expanded_base_url_opt, src.expanded_output_filename_opt);
  };
  execute(~copts, ~read, ~transform, ~print);
};

/****************************************************************************
 * Functions called by CLI commands that are implemented on the CLI side
 */
let copts = (template_filename, style_filename_opt, text_filename, data_filename_opt, watch_mode, base_url_opt, output_filename_opt, publipost, async) => {
  template_filename,
  style_filename_opt,
  text_filename,
  data_filename_opt,
  watch_mode,
  base_url_opt,
  output_filename_opt,
  publipost,
  async
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
   Cmdliner.Term.(ret(const(instantiate) $ copts_t)),
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
let cmds = [help_cmd, instantiate_cmd, compile_cmd, print_cmd];

// Execute default command if no argument given
// Otherwise, execute the specified command.
let _ = switch (Cmdliner.Term.eval_choice(default_cmd, cmds)) {
| `Help | `Version => ignore() |> promise
| `Error(_) => exit(1) |> promise
| `Ok r => r |> then_resolve(ignore)
};

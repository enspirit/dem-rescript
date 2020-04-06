open Sugar;

type result =
  | Ok(Js.Promise.t(unit))
  | Error(string)
;

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
    Ok(result_promise);
  } {
  | e => Error(Logger.format_exn(e));
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

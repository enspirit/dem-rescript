open Sugar

type result =
  | Ok(Js.Promise.t<array<Node.Buffer.t>>, Buffer.t)
  | Error(string)

type filename_or_content<'a> =
  | Filename(string)
  | Content('a)

let fo_of_fco = fco =>
  switch fco {
  | Some(Filename(f)) => Some(f)
  | _ => None
  }

let read_fco1 = (fco, read) => fco->Belt.Option.flatMap(fc =>
    switch fc {
    | Filename(filename) => read(filename)
    | Content(content) => Some(content)
    }
  )

let read_fco2 = (fco, read) =>
  switch fco {
  | Some(Content(content)) => Some(content)
  | Some(Filename(filename)) => read(Some(filename))
  | None => read(None)
  }

let promise_read_data_array = (fco, read) =>
  switch fco {
  | Some(Content(content)) => promise(Sugar.ensure_array(content))
  | Some(Filename(filename)) => read(Some(filename))
  | None => read(None)
  }

let promise_read_single_data = (fco, read) =>
  switch fco {
  | Some(Content(content)) => promise(Some(content))
  | Some(Filename(filename)) => read(Some(filename))
  | None => read(None)
  }

// common options type
type copts = {
  template_fco: option<filename_or_content<App.t_template>>,
  style_fco: option<filename_or_content<App.t_style>>,
  text_fco: option<filename_or_content<App.t_text>>,
  data_fco: option<filename_or_content<App.t_data>>,
  watch_mode: bool,
  base_url_opt: option<string>,
  output_filename_opt: option<string>,
  publipost: bool,
  async: bool
}

type t_instantiation_src = {
  text_opt: option<string>,
  partials: Js.Dict.t<string>,
  data_opt: option<Js.Json.t>,
  expanded_base_url_opt: option<string>,
  expanded_output_filename_opt: option<string>
}

type t_compilation_src = {
  text_opt: option<string>,
  partials: Js.Dict.t<string>,
  data_opt: option<Js.Json.t>,
  template_opt: option<string>,
  style_opt: option<string>,
  expanded_base_url_opt: option<string>,
  expanded_output_filename_opt: option<string>
}

type t_print_src = {
  html: string,
  expanded_base_url_opt: option<string>,
  expanded_output_filename_opt: option<string>
}

let copts = (
  template_fco,
  style_fco,
  text_fco,
  data_fco,
  watch_mode,
  base_url_opt,
  output_filename_opt,
  publipost,
  async,
) => {
  template_fco: template_fco,
  style_fco: style_fco,
  text_fco: text_fco,
  data_fco: data_fco,
  watch_mode: watch_mode,
  base_url_opt: base_url_opt,
  output_filename_opt: output_filename_opt,
  publipost: publipost,
  async: async
}

let watch_directory_rec = (directory, callback) => {
  let watcher = Chokidar.watch(directory, ())
  let handler = (path: string) => {
    let ext = File.extension(path)
    switch ext {
    | Some("md")
    | Some("json")
    | Some("yml")
    | Some("js")
    | Some("css")
    | Some("sass")
    | Some("tpl") =>
      callback()
    | _ => ()
    }
  }
  let _ = Chokidar.on(watcher, "change", handler)
}

let directories = copts => {
  // there is no element in l that is a leading substring of s1 except s1 itself
  let noDistinctSubstring = (l, s1) =>
    !Belt.List.some(l, s2 => s2 != s1 && Js.String.startsWith(s2, s1))
  list{
    fo_of_fco(copts.template_fco),
    fo_of_fco(copts.style_fco),
    fo_of_fco(copts.text_fco),
    fo_of_fco(copts.data_fco),
    copts.output_filename_opt
  }
  ->Belt.List.map(f =>
    switch f {
    | Some(f) when Node.Fs.existsSync(f) =>
      let root_dir = Node.Path.dirname(f)
      let absolute = Node.Path.resolve(root_dir, "")
      Some(absolute)
    | _ => None
    }
  )
  ->Belt.List.keepMap(x => x)
  |> List.sort_uniq(Pervasives.compare)
  |> (l => Belt.List.keep(l, noDistinctSubstring(l)))
}

let load_instantiation_sources = (copts, data_opt) => {
  let text_opt = copts.text_fco->read_fco1(File.read_text)
  let root_partials_dep = App.partials_dependencies(text_opt -> or_else(""))
  let partials = File.build_partials(~root=?fo_of_fco(copts.text_fco), root_partials_dep)
  let expanded_base_url_opt = File.expand(data_opt, copts.base_url_opt)
  let expanded_output_filename_opt = File.expand(data_opt, copts.output_filename_opt)
  {
    text_opt: text_opt,
    partials: partials,
    data_opt: data_opt,
    expanded_base_url_opt: expanded_base_url_opt,
    expanded_output_filename_opt: expanded_output_filename_opt
  }
}

let load_compilation_sources = (copts, data_opt) => {
  let i_src = load_instantiation_sources(copts, data_opt)
  let template_opt = copts.template_fco->read_fco1(File.read_template)
  let style_opt = copts.style_fco->read_fco2(File.read_style)
  {
    text_opt: i_src.text_opt,
    partials: i_src.partials,
    data_opt: data_opt,
    template_opt: template_opt,
    style_opt: style_opt,
    expanded_base_url_opt: i_src.expanded_base_url_opt,
    expanded_output_filename_opt: i_src.expanded_output_filename_opt
  }
}

let execute = (
  ~copts,
  ~read: (copts, 'a) => 'b,
  ~transform: (copts, 'b) => 'c,
  ~print: (copts, 'c) => Node.Buffer.t
) => {
  let do_it = () =>
    if copts.publipost && copts.data_fco != None {
      copts.data_fco->promise_read_data_array(File.read_data(~batch=true, ~async=copts.async))
        |> then_resolve(data =>
          data
          |> List.map(data => read(copts, Some(data)) |> transform(copts, _) |> print(copts, _))
          |> Belt.List.toArray
        )
    } else {
      copts.data_fco->promise_read_single_data(File.read_single_data)
        |> then_resolve(data => [read(copts, data) |> transform(copts, _) |> print(copts, _)])
    }

  try {
    let result_promise = do_it() |> then_resolve((res): array<Node.Buffer.t> =>
      if copts.watch_mode {
        let no_buff = Node.Buffer.fromString("")
        directories(copts) |> List.iter(d => watch_directory_rec(d, ignore_promise(do_it)))
        [no_buff]
      } else {
        res
      }
    )
    Ok(result_promise, Logger.default_buffer)
  } catch {
  | e => Error(Logger.format_exn(e))
  }
}

let instantiate = copts => {
  let read = load_instantiation_sources
  let transform = (_, src: t_instantiation_src) => {
    let markdown = App.instantiate_body(src.text_opt, src.data_opt, Some(src.partials))
    {
      text_opt: Some(markdown),
      partials: src.partials,
      data_opt: src.data_opt,
      template_opt: None,
      style_opt: None,
      expanded_base_url_opt: src.expanded_base_url_opt,
      expanded_output_filename_opt: src.expanded_output_filename_opt
    }
  }
  let print = (copts, src: t_compilation_src) => {
    let output_filename_opt = src.expanded_output_filename_opt
    let Some(markdown) = src.text_opt
    switch output_filename_opt {
    | None => Js.log(markdown) // TODO: Move in cli
    | Some(_) =>
      ignore(
        File.write_md(~output_filename_opt, ~text_filename=?fo_of_fco(copts.text_fco), markdown)
      )
    }
    Node.Buffer.fromString(markdown)
  }
  execute(~copts, ~read, ~transform, ~print)
}

let compile = copts => {
  let read = load_compilation_sources
  let transform = (_, src: t_compilation_src) => {
    let html = App.compile(
      src.template_opt,
      src.style_opt,
      src.text_opt,
      src.data_opt,
      Some(src.partials)
    )
    {
      html: html,
      expanded_base_url_opt: src.expanded_base_url_opt,
      expanded_output_filename_opt: src.expanded_output_filename_opt
    }
  }
  let print = (copts, src: t_print_src) => {
    let output_filename_opt = src.expanded_output_filename_opt
    switch output_filename_opt {
    | None => Js.log(src.html) // TODO: Move in cli
    | Some(_) =>
      ignore(
        File.write_html(~output_filename_opt, ~text_filename=?fo_of_fco(copts.text_fco), src.html)
      )
    }
    Node.Buffer.fromString(src.html)
  }
  execute(~copts, ~read, ~transform, ~print)
}

let print = (copts, pipe) => {
  let read = load_compilation_sources
  let transform = (_, src: t_compilation_src) => {
    let html = App.compile(
      src.template_opt,
      src.style_opt,
      src.text_opt,
      src.data_opt,
      Some(src.partials)
    )
    {
      html: html,
      expanded_base_url_opt: src.expanded_base_url_opt,
      expanded_output_filename_opt: src.expanded_output_filename_opt
    }
  }
  let print = (copts, src: t_print_src) =>
    switch pipe {
    | false =>
      let html_filename = File.write_html(
        ~output_filename_opt=src.expanded_output_filename_opt,
        ~text_filename=?fo_of_fco(copts.text_fco),
        src.html
      )
      Weasyprint.print(html_filename, src.expanded_base_url_opt, src.expanded_output_filename_opt)
      Node.Buffer.fromString("")
    | true => Weasyprint.pipe(src.html, src.expanded_base_url_opt)
    }
  execute(~copts, ~read, ~transform, ~print)
}

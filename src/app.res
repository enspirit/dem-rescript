open Sugar

let markdownItInstance = MarkdownIt.createMarkdownIt(~html=true, ())
let markdownIt = text => MarkdownIt.render(markdownItInstance, text)

let mustache = (text, data, partials_opt) => {
  let partials = partials_opt
  Mustache.render(text, data, ~partials?, ())
}

type t_template = string
type t_style = string
type t_text = string
type t_data = Js.Json.t
type t_partials = Js.Dict.t<string>

let default_text = ""
let default_data = Js.Json.parseExn("{}")
let default_template = `<html>
  <head>
    <style>
      {{{compiled_style}}}
    </style>
  </head>
  <body>
     {{{compiled_body}}}
  </body>
</html>`
let default_style = ""

let partials_dependencies = text => {
  /* look for `{{> [name]}}` patterns
   * (which is the syntax for a mustache partial) */
  let partial_balises = Js.String.match_(%re("/{{> .*}}/g"), text)
  switch partial_balises {
  | None => list{}
  | Some(a) => List.map(s =>
      switch Js.String.match_(%re("/{{> (.*)}}/"), s) {
      | Some([_, name]) => list{name} // extract the partial name itself
      | _ => list{}
      }
    , Array.to_list(a)) |> List.flatten
  }
}

let with_semantics_tags = text => {
  let split_at_html_title_tag = {
    let res = Array.to_list(Js.String.splitByRe(%re("/(<h[1-9]>)/"), text))
    Belt.List.keepMap(res, x => x)
  }

  let html_title_tag_level = s =>
    switch Js.String.match_(%re("/<h([1-9])>/"), s) {
    | None => 0
    | Some(a) => int_of_string(a[1])
    }

  let tag = (~id=None, level) => {
    let (on, off) = switch id {
    | None => ("</", ">")
    | Some(id) => ("<", " id=\"" ++ (id ++ "\">"))
    }
    switch level {
    | 1 => on ++ ("article" ++ off)
    | level when level > 1 => on ++ ("section" ++ off)
    | _ => ""
    }
  }

  let id = text_pieces => Some(
    switch text_pieces {
    | list{} => ""
    | list{hd, ..._} =>
      let closing_tag_pos = Js.String.search(%re("/<\/h[1-9]>/"), hd)
      Js.String.slice(~from=0, ~to_=closing_tag_pos, hd) |> Formatter.to_html_id
    }
  )

  let rec insert_tags = (text_pieces, levels, s) =>
    switch text_pieces {
    | list{} => s
    | list{piece} when List.length(levels) > 0 =>
      s ++ (piece ++ String.concat("", List.map(tag, levels)))
    | list{hd, ...tl} =>
      switch (html_title_tag_level(hd), levels) {
      | (0, _) => insert_tags(tl, levels, s ++ hd)
      | (i, list{}) => insert_tags(tl, list{i}, s ++ (tag(~id=id(tl), i) ++ hd))
      | (i, list{highest, ..._}) when i > highest =>
        insert_tags(tl, list{i, ...levels}, s ++ (tag(~id=id(tl), i) ++ hd))
      | (_, list{highest, ...rem_levels}) => insert_tags(text_pieces, rem_levels, s ++ tag(highest))
      }
    }

  insert_tags(split_at_html_title_tag, list{}, "")
}

let instantiate_body = (text_opt, json_data_opt, partials_opt) => {
  let text = text_opt -> or_else(default_text)
  let json_data = json_data_opt -> or_else(default_data)
  let js_data = json_data |> jsonToObj
  mustache(text, js_data, partials_opt)
}

let compile_body = (text_opt, data_opt, partials_opt) =>
  instantiate_body(text_opt, data_opt, partials_opt)
  |> markdownIt
  |> with_semantics_tags
  |> Formatter.format

let compile = (template_opt, style_opt, text_opt, data_opt, partials_opt) => {
  let template = template_opt -> or_else(default_template)
  let compiled_style = style_opt -> or_else(default_style)
  let compiled_body = compile_body(text_opt, data_opt, partials_opt)
  mustache(
    template,
    {
      "compiled_style": compiled_style,
      "compiled_body": compiled_body,
    },
    partials_opt
  )
  |> Formatter.format
}

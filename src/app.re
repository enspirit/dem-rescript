// Convert Json to Js Object \!/ without type safety \!/
external jsonToObj: Js.Json.t => Js.t({..}) = "%identity";

open Sugar;

let markdownItInstance = MarkdownIt.createMarkdownIt();
let markdownIt = text => MarkdownIt.render(markdownItInstance, text);

let mustache = Mustache.render;

type t_template = string;
type t_style = string;
type t_text = string;
type t_data = Js.Json.t;

let default_text = "";
let default_data = Js.Json.parseExn("{}");
let default_template = {|<html>
  <head>
    <style>
      {{{compiled_style}}}
    </style>
  </head>
  <body>
     {{{compiled_body}}}
  </body>
</html>|};
let default_style = "";

let with_semantics_tags = text => {
  let split_at_html_title_tag = {
    let res = Array.to_list(Js.String.splitByRe([%re "/(<h[1-9]>)/"], text));
    Belt.List.keepMap(res, fun (x) => x);
  }

  let html_title_tag_level = s => {
    switch (Js.String.match([%re "/<h([1-9])>/"], s)) {
    | None => 0
    | Some(a) => int_of_string(a[1])
    };
  };

  let tag = (~id=None, level) => {
    let (on, off) = {
      switch (id) {
      | None => ("</", ">")
      | Some(id) => ("<", " id=\"" ++ id ++ "\">")
      }
    };
    switch (level) {
    | 1 => on ++ "article" ++ off
    | level when level > 1 => on ++ "section" ++ off
    | _ => ""
    };
  };

  let id = (text_pieces) => Some(switch (text_pieces) {
  | [] => ""
  | [hd, ..._] =>
    let closing_tag_pos = Js.String.search([%re "/<\/h[1-9]>/"], hd);
    Js.String.slice(~from=0, ~to_=closing_tag_pos, hd)
    |> Formatter.to_html_id
  });

  let rec insert_tags = (text_pieces, levels, s) => switch (text_pieces) {
  | [] => s
  | [piece] when List.length(levels) > 0 => s ++ piece ++ String.concat("", List.map(tag, levels))
  | [hd, ...tl] => switch (html_title_tag_level(hd), levels) {
    | (0, _) => insert_tags (tl, levels, s ++ hd)
    | (i, []) => insert_tags (tl, [i], s ++ tag(~id=id(tl), i) ++ hd)
    | (i, [highest, ..._]) when i > highest => insert_tags (tl, [i, ...levels], s ++ tag(~id=id(tl), i) ++ hd)
    | (_, [highest, ...rem_levels]) => insert_tags (text_pieces, rem_levels, s ++ tag(highest))
    };
  };

  insert_tags(split_at_html_title_tag, [], "");
}

let compile_body = (text, js_data) => {
  mustache(text, js_data)
  |> markdownIt
  |> with_semantics_tags
  |> Formatter.format
};

let compile_body = (text, json_data) => {
  let text = text || default_text;
  let json_data = json_data || default_data;
  let js_data = json_data |> jsonToObj;
  compile_body(text, js_data);
};

let compile = (compilation_template, compiled_style, text, data) => {
  let compilation_template = compilation_template || default_template;
  let compiled_style = compiled_style || default_style;
  let compiled_body = compile_body(text, data);
  mustache(compilation_template, {
    "compiled_style": compiled_style,
    "compiled_body": compiled_body
  })
  |> Formatter.format;
};

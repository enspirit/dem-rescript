// Convert Json to Js Object \!/ without type safety \!/
external jsonToObj: Js.Json.t => Js.t({..}) = "%identity";

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

let or_else = (opt, default) => {
  switch (opt) {
  | None => default
  | Some(v) => v
  }
}

// syntactic sugar
let (||) = (opt, default) => or_else(opt, default);

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

  let insert_tag = (~close=false, i) => {
    let polarity = close ? "</" : "<";
    switch (i) {
    | 1 => polarity ++ "article>"
    | i when i > 1 => polarity ++ "section>"
    | _ => ""
    };
  };

  let rec insert_tags = (l, levels, s) => switch (l) {
  | [] => s
  | [x] when List.length(levels) > 0 => s ++ x ++ String.concat("", List.map(insert_tag(~close=true), levels))
  | [hd, ...tl] => switch (html_title_tag_level(hd), levels) {
    | (0, _) => insert_tags (tl, levels, s ++ hd)
    | (i, []) => insert_tags (tl, [i], s ++ insert_tag(i) ++ hd)
    | (i, [highest, ..._]) when i > highest => insert_tags (tl, [i, ...levels], s ++ insert_tag(i) ++ hd)
    | (_, [highest, ...rem_levels]) => insert_tags (l, rem_levels, s ++ insert_tag(~close=true, highest))
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

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

let (||) = (opt, default) => or_else(opt, default);

let compile_body = (text, js_data) => mustache(text, js_data) |> markdownIt |> Prettier.format;

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
  |> Prettier.format;
};

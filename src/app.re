// Convert Json to Js Object \!/ without type safety \!/
external jsonToObj: Js.Json.t => Js.t({..}) = "%identity";

let markdownItInstance = MarkdownIt.createMarkdownIt();
let markdownIt = text => MarkdownIt.render(markdownItInstance, text);

let mustache = Mustache.render;

type t_compilation_template = string;
type t_style = string;
type t_text = string;
type t_data = Js.Json.t;

let compile_body = (text, js_data) => mustache(text, js_data) |> markdownIt |> Prettier.format;

let compile_body = (text, json_data) => {
  let js_data = json_data |> jsonToObj;
  compile_body(text, js_data);
};

let compile = (compilation_template, compiled_style, text, data) => {
  let compiled_body = compile_body(text, data);
  mustache(compilation_template, {
    "compiled_style": compiled_style,
    "compiled_body": compiled_body
  })
  |> Prettier.format;
};

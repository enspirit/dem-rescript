external jsonToObj : Js.Json.t => Js.t({..}) = "%identity";

let markdownItInstance = MarkdownIt.createMarkdownIt();
let markdownIt = (text) => MarkdownIt.render(markdownItInstance, text);

let mustache = Mustache.render;

let md_text = Node.Fs.readFileAsUtf8Sync("index.md");
let json_data = Node.Fs.readFileAsUtf8Sync("index.json") -> Js.Json.parseExn;
let js_data = jsonToObj(json_data);

let res = mustache(md_text, js_data) -> markdownIt;

Js.log(res);

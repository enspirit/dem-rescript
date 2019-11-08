// Convert Json <-> Js Object \!/ without type safety \!/
external jsonToObj: Js.Json.t => Js.t({..}) = "%identity";
external objToJson: Js.t({..}) => Js.Json.t = "%identity";

// binding to NodeJS.require
[@bs.val] external require: string => Js.t({..}) = "require";
let node_require = filename => {
  let absolute = Node.Path.resolve(filename, "");
  require(absolute);
};

let or_else = (opt, default) => {
  switch (opt) {
  | None => default
  | Some(v) => v
  }
}
// BEWARE that this shadows the classic || logical or operator
let (||) = (opt, default) => or_else(opt, default);

let has_key = (dict, key) => Js.Dict.get(dict, key) != None;

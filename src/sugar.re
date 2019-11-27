let ensure_array = json => {
  let error = kind => {
    Logger.error({j|$json is $kind, but a json array was expected.|j});
    [];
  }
  switch (Js.Json.classify(json)) {
  | Js.Json.JSONArray(a) => Belt.List.fromArray(a)
  | Js.Json.JSONObject(_) => error("an object")
  |	Js.Json.JSONFalse | Js.Json.JSONTrue => error("a boolean")
  |	Js.Json.JSONNull => error("null")
  |	Js.Json.JSONString(_) => error("a string")
  |	Js.Json.JSONNumber(_) => error("a number")
  };
};

// Convert Json <-> Js Object \!/ without type safety \!/
external jsonToObj: Js.Json.t => Js.t({..}) = "%identity";
external objToJson: Js.t({..}) => Js.Json.t = "%identity";
let objToJsonList = obj => obj |> objToJson |> ensure_array;

// bindings to NodeJS.require
[@bs.val] external require: string => Js.t({..}) = "require";
let node_require = filename => {
  let absolute = Node.Path.resolve(filename, "");
  require(absolute);
};

[@bs.val] external require_promise: string => Js.Promise.t(Js.t({..})) = "require";
let node_require_promise = filename => {
  let absolute = Node.Path.resolve(filename, "");
  require_promise(absolute);
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

let promise = (e) => Js.Promise.make((~resolve, ~reject as _) => resolve(. e));

let then_resolve = (f, p) => p |> Js.Promise.then_(x => Js.Promise.resolve(f(x)));

let ignore_promise = (promise) => () => {
  promise() |> then_resolve(ignore) |> ignore
};

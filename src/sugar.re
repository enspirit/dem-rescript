let or_else = (opt, default) => {
  switch (opt) {
  | None => default
  | Some(v) => v
  }
}
let (||) = (opt, default) => or_else(opt, default);

let has_key = (dict, key) => Js.Dict.get(dict, key) != None;

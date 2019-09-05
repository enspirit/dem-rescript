let trim_lonely_spaces_and_empty_lines = (s) => {
  Js.String.replaceByRe([%re "/^\s*[\\r\\n]/gm"], "", s);
};

let format = (s) => {
  String.trim(s)
  |> trim_lonely_spaces_and_empty_lines
};

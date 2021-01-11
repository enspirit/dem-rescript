let format_js_exn = e => {
  let name = Js.Exn.name(e);
  let location = switch (Js.Exn.fileName(e)) {
  | None => ""
  | Some(f) => " in " ++ f
  };
  let msg = Js.Exn.message(e);
  let stack = Js.Exn.stack(e);
  switch (stack) {
  | None => {j|$name raised$location:\n$msg|j}
  | _ => {j|$stack|j}
  }
};

let format_exn = e => switch (Js.Exn.asJsExn(e)) {
  | None => {j|$e.|j}
  | Some(js_exn) => format_js_exn(js_exn)
};

let format_js_exn = e => {j|$e.|j};

let default_logfile = "dem.log";
let default_buffer = Buffer.create(80); // mutable, check ReasonML Buffer API

let tag = level => switch level {
| -1 => "DEBUG"
| 0  => "INFO"
| 1  => "WARN"
| 2  => "ERROR"
| 3  => "FATAL"
| _  => "UNKNOWN"
};

let log = (~buffer=default_buffer, level, msg) => {
  let tag = tag(level);
  let s = {j|[$tag] $msg\n|j};
  Buffer.add_string(buffer, s);
};

let debug = (~buffer=?, msg) => {
  log(~buffer=?buffer, -1, msg);
};

let info = (~buffer=?, msg) => {
  log(~buffer=?buffer, 0, msg);
};

let warn = (~buffer=?, msg) => {
  log(~buffer=?buffer, 1, msg);
};

let error = (~buffer=?, msg) => {
  log(~buffer=?buffer, 2, msg);
};

let fatal = (~buffer=?, msg) => {
  log(~buffer=?buffer, 3, msg);
};

let save = (~buffer=default_buffer, ~filename=default_logfile, ()) => {
  try (Node.Fs.writeFileAsUtf8Sync(filename, Buffer.contents(buffer))) {
  | Js_exn.Error(e) => Js.Console.error({j|$e.|j});
  };
};

let print_stderr = (~buffer=default_buffer, ()) => {
  try (Js.Console.error(Buffer.contents(buffer))) {
  | Js_exn.Error(e) => Js.Console.error({j|$e.|j});
  };
};

open Sugar;

let base_url_param = (base_url_opt) => switch (base_url_opt) {
| None => ""
| Some(base_url) => {j|-u $base_url|j}
};

let print = (html_filename, base_url_opt, output_filename_opt) => {
  let base_url_param = base_url_param(base_url_opt);
  let output_filename = output_filename_opt || (Node.Path.basename_ext(html_filename, "html") ++ "pdf")
  Logger.info({j|Printing document in "$output_filename" with weasyprint.|j});
  File.ensure_path(output_filename);
  let res = Execa.commandSync({j|weasyprint -e utf-8 $base_url_param -v $html_filename $output_filename|j}, ());
  let exit_code = Execa.StringResult.exitCodeGet(res);
  switch (exit_code) {
  | 0 => ()
  | 127 => Logger.error("weasyprint command not found. Please check your weasyprint installation." ++ Execa.StringResult.stderrGet(res));
  | _ => Logger.fatal("weasyprint returned an unexpected error. Please report to yoann.guyot@enspirit.be" ++ Execa.StringResult.stderrGet(res));
  }
};

let pipe = (html, base_url_opt) => {
  let base_url_param = base_url_param(base_url_opt);
  let options = Execa.options(~shell=false, ~input=html, ~encoding=Js.Nullable.null, ());
  let res = Execa.commandSyncBuffer({j|weasyprint -f pdf -e utf-8 $base_url_param - -|j}, ~options, ());
  let exit_code = Execa.BufferResult.exitCodeGet(res);
  switch (exit_code) {
  | 0 => Execa.BufferResult.stdoutGet(res);
  | 127 => {
    let s = Execa.BufferResult.stderrGet(res);
    Js.Exn.raiseError({j|weasyprint command not found. Please check your weasyprint installation. $s|j});
  }
  | _ => {
    let s = Execa.BufferResult.stderrGet(res);
    Js.Exn.raiseError({j|weasyprint returned an unexpected error. Please report to yoann.guyot@enspirit.be. $s|j});
  }}
};

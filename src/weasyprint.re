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
  let exit_code = Execa.exitCodeGet(res);
  switch (exit_code) {
  | 0 => ()
  | 127 => Logger.error("weasyprint command not found. Please check your weasyprint installation." ++ Execa.stderrGet(res));
  | _ => Logger.fatal("weasyprint returned an unexpected error. Please report to yoann.guyot@enspirit.be" ++ Execa.stderrGet(res));
  }
};

let pipe = (html, base_url_opt) => {
  let base_url_param = base_url_param(base_url_opt);
  let options = Execa.options(~shell=false, ~input=html, ());
  let res = Execa.commandSync({j|weasyprint -f pdf -e utf-8 $base_url_param - -|j}, ~options, ());
  let exit_code = Execa.exitCodeGet(res);
  switch (exit_code) {
  | 0 => Execa.stdoutGet(res);
  | 127 => Js.Exn.raiseError("weasyprint command not found. Please check your weasyprint installation." ++ Execa.stderrGet(res));
  | _ => Js.Exn.raiseError("weasyprint returned an unexpected error. Please report to yoann.guyot@enspirit.be" ++ Execa.stderrGet(res));
  }
};
